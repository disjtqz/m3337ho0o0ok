#include "xbyak/xbyak.h"

#include "mh_defs.hpp"

#include <Windows.h>
#include <intrin.h>
#include "game_exe_interface.hpp"
#include "snaphakalgo.hpp"
#include <mutex>
#include "scanner_core.hpp"
static HMODULE g_reach_module = nullptr;

blamdll_t g_blamdll{};

struct patched_memory_t;

static void patch_memory_impl(void* where, char* patch_bytes, unsigned length) {
	DWORD old_prot;

	VirtualProtect(where, length, PAGE_EXECUTE_READWRITE, &old_prot);

	memcpy(where, patch_bytes, length);
	VirtualProtect(where, length, old_prot, &old_prot);
}

void patched_memory_t::undo() {
	patch_memory_impl(m_patch_location, m_original_data, m_nbytes);
}

void patched_memory_t::redo() {
	patch_memory_impl(m_patch_location, m_new_data, m_nbytes);
}

static patched_memory_t* m_patch_undo_stack_head = nullptr;


bool undo_last_patch() {
	if (m_patch_undo_stack_head == nullptr)
		return false;

	patched_memory_t* patch = m_patch_undo_stack_head;
	m_patch_undo_stack_head = patch->m_prev_patch;

	patch->undo();
	delete patch;
	return true;
}
void patch_memory(void* location, unsigned size, char* patched_data) {
	patch_memory_impl(location, patched_data, size);
}
MH_NOINLINE
patched_memory_t* patch_memory_with_undo(void* location, unsigned size, char* patched_data) {
	patched_memory_t* patch = new patched_memory_t(location, size, patched_data);
	patch_memory_impl(location, patched_data, size);
	patch->m_prev_patch = m_patch_undo_stack_head;
	m_patch_undo_stack_head = patch;

	return patch;
}
uintptr_t get_reach_base() {

	return reinterpret_cast<uintptr_t>(g_reach_module);
}

MH_NOINLINE
void init_reach() {
	if (g_reach_module == nullptr) {
		g_reach_module = GetModuleHandleA("DOOMEternalx64vk.exe");
		get_blamdll_info(&g_blamdll);

	}

}

MH_NOINLINE
patched_memory_t* redirect_to_func(void* hookfn, uintptr_t reachaddr, bool direct) {

	auto oldfunc = reachaddr;

	static char dabytes[] = {
	   0x48, (char)0xb8, 0,0,0,0, 0,0,0,0,0x50, (char)0xc3
	};

	*reinterpret_cast<void**>(&dabytes[2]) = hookfn;
	return patch_memory_with_undo((void*)oldfunc, sizeof(dabytes), dabytes);

}

MH_NOINLINE
void redirect_to_func_save_rax(void* hookfn, uintptr_t reachaddr) {
	static char dabytes[] = { 0x50, 0x48, (char)0xB8, 0x32, (char)0x89, 0x04, 0x39, (char)0xFF, (char)0x7F, 0x00, 0x00, (char)0xFF, (char)0xE0 };
	*reinterpret_cast<void**>(&dabytes[3]) = hookfn;

	patch_memory_with_undo((void*)reachaddr, sizeof(dabytes), dabytes);


}



MH_NOINLINE
void swap_out_ptrs(void* start_addr, void** repls, unsigned n, bool dont_want_replaced) {

	void** temp = new void* [n];

	memcpy(temp, start_addr, n * 8);

	for (unsigned i = 0; i < n; ++i) {
		void** p = temp;
		p += i;
		void* o = *p;
		*p = repls[i];
		if (!dont_want_replaced)
			repls[i] = o;
	}
	patch_memory_with_undo(start_addr, n * 8, (char*)temp);
	delete[] temp;

}


void get_blamdll_info(blamdll_t* out) {

	memset(out, 0, sizeof(blamdll_t));
	char* ntdllc = reinterpret_cast<char*>(get_reach_base());
	auto base = reinterpret_cast<IMAGE_DOS_HEADER*>(get_reach_base());
	auto winheader = reinterpret_cast<IMAGE_NT_HEADERS*>(ntdllc + base->e_lfanew);
#if 0
	out->image_headers = winheader;
	out->text_base = ntdllc +
		winheader->OptionalHeader.BaseOfCode;

	out->text_size = winheader->OptionalHeader.SizeOfCode;

	auto sechead = (_IMAGE_SECTION_HEADER*)(((char*)&winheader->OptionalHeader) + sizeof(winheader->OptionalHeader));

	for (unsigned i = 0; i < winheader->FileHeader.NumberOfSections; ++i) {
		if (!strcmp((const char*)(sechead[i].Name), ".rdata")) {

			out->rdata_base = sechead[i].VirtualAddress + ntdllc;
			out->rdata_size = sechead[i].SizeOfRawData;
		}
		if (!strcmp((const char*)(sechead[i].Name), ".data")) {

			out->data_base = sechead[i].VirtualAddress + ntdllc;
			out->data_size = sechead[i].SizeOfRawData;
		}

		if(!strcmp((const char*)(sechead[i].Name), ".idata")) {
			out->idata_base = sechead[i].VirtualAddress + ntdllc;
			out->idata_size = sechead[i].SizeOfRawData;
		}

		if(!strcmp((const char*)(sechead[i].Name), ".arch")) {
			out->arch_base = sechead[i].VirtualAddress + ntdllc;
			out->arch_size = sechead[i].SizeOfRawData;			
		}
		if(!strcmp((const char*)(sechead[i].Name), ".reloc")) {
			out->reloc_base = sechead[i].VirtualAddress + ntdllc;
			out->reloc_size = sechead[i].SizeOfRawData;			
		}
	}

	if(!out->arch_base) {
		out->arch_base = out->data_base;
		out->arch_size = out->data_size;
	}
#endif
	/*
	out->rdata_base = ntdllc + winheader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress +  winheader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;

	out->rdata_size = winheader->OptionalHeader.SizeOfInitializedData;

	out->data_base = out->rdata_base + out->rdata_size;
	out->data_size = winheader->OptionalHeader.SizeOfUninitializedData;
	*/
	out->image_base = (char*)get_reach_base();
	out->image_size = winheader->OptionalHeader.SizeOfImage;

}



void undo_all_reach_patches() {
	while (undo_last_patch())
		;
}


static std::mutex g_execmem_lock{};
static HANDLE g_execmem_heap=nullptr;

void* alloc_execmem(size_t size) {
	#if 1
	g_execmem_lock.lock();

	if(!g_execmem_heap) {
		g_execmem_heap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 0, 0);

	}

	void* result = HeapAlloc(g_execmem_heap, 0, size + 32);



	g_execmem_lock.unlock();
	return result;

	
#else
	return VirtualAlloc(nullptr, size, MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#endif
}


void* detour_with_thunk_for_original(void* detour_from, void* detour_to, bool use_r9_instead) {
	Xbyak::CodeGenerator redirector{};
	Xbyak::CodeGenerator reporiginal_thunk{};
	redirector.mov(redirector.rax, (uintptr_t)detour_to);
	redirector.jmp(redirector.rax);


	auto redirector_reg = use_r9_instead ? redirector.r9 : redirector.rax;


	mh_disassembler_t disas{};
	void* original_getfile_continuation = nullptr;
	size_t savesize = 0;
	disas.decode_enough_to_make_space(detour_from, redirector.getSize(), &original_getfile_continuation, &savesize);


	Xbyak::CodeGenerator original_thunk{};

	original_thunk.mov(redirector_reg, (uintptr_t)original_getfile_continuation);
	original_thunk.jmp(redirector_reg);



	void* g_original_ds_getfile = alloc_execmem(original_thunk.getSize() + savesize + 32);
	size_t setupsize = savesize;

	memcpy(g_original_ds_getfile, detour_from, setupsize);

	disas.setup_for_addr(g_original_ds_getfile, 128);

	Xbyak::CodeGenerator* to_use = &original_thunk;

	if (disas.find_next_call(setupsize)) {
		if(disas.get_call_target() == descan::g_alloca_probe) {
		memset((void*)(disas.m_ctx.pc - 5), 0x90, 5);



		reporiginal_thunk.mov(redirector_reg, (uintptr_t)descan::g_alloca_probe);
		reporiginal_thunk.call(redirector_reg);

		reporiginal_thunk.mov(redirector_reg, (uintptr_t)original_getfile_continuation);
		reporiginal_thunk.jmp(redirector_reg);

		to_use = &reporiginal_thunk;
		}
		else {
			mh_error_message("Unimplemented call redirect handler!");
			return nullptr;
		}



	}

	memcpy(mh_lea<char>(g_original_ds_getfile, savesize), to_use->getCode(), to_use->getSize());


	//g_original_ds_getfile now has the bytes we will clobber on the original + the instructions to jump back to the function, past the part we overwrote

	patch_memory(detour_from, redirector.getSize(), (char*)redirector.getCode());
	return g_original_ds_getfile;
}