#pragma once
#include <Windows.h>
#include <cstddef>
#include "extern.h"

struct blamdll_t {
	char* text_base;
	char* rdata_base;
	char* data_base;
	unsigned text_size;
	unsigned rdata_size;
	unsigned data_size;
	unsigned idata_size;
	unsigned arch_size;
	unsigned reloc_size;
	void* tls_section;
	char* image_base;
	char* idata_base;
	char* arch_base;
	char* reloc_base;
	unsigned image_size;
	IMAGE_NT_HEADERS* image_headers;
	template<typename T>
	bool is_in_text(T v) {
		return (char*)v >= text_base && (char*)v < (text_base + text_size);
	}

	template<typename T>
	bool is_in_rdata(T v) {
		return (char*)v >= rdata_base && (char*)v < (rdata_base + rdata_size);
	}
	template<typename T>
	bool is_in_data(T v) {
		return (char*)v >= data_base && (char*)v < (data_base + data_size);
	}

	template<typename T>
	bool is_in_image(T v) {
		return (char*)v >= image_base && (char*)v < (image_base + image_size);
	}
};


struct patched_memory_t {
	char* m_original_data;
	char* m_new_data;
	struct patched_memory_t* m_prev_patch;
	size_t m_nbytes;
	void* m_patch_location;
	MH_NOINLINE
		patched_memory_t(void* location, unsigned size, void* newdata) {
		m_original_data = new char[size];
		m_new_data = new char[size];
		memcpy(m_original_data, location, size);
		memcpy(m_new_data, newdata, size);
		m_prev_patch = nullptr;
		m_nbytes = size;
		m_patch_location = location;
	}
	MH_NOINLINE
		~patched_memory_t() {
		delete[] m_original_data;
		// delete m_prev_patch;
	}

	void undo();
	void redo();
};
MH_NOINLINE
void init_reach();

uintptr_t get_reach_base();
MH_NOINLINE
patched_memory_t* patch_memory_with_undo(void* location, unsigned size, char* patched_data);
void patch_memory(void* location, unsigned size, char* patched_data);
MH_NOINLINE
patched_memory_t* redirect_to_func(void* hookfn, uintptr_t reachaddr, bool direct = false);

static void make_ret(void* loc) {
	char val = 0xC3;
	patch_memory(loc, 1, &val);
}


void get_blamdll_info(blamdll_t* out);

MH_NOINLINE
void redirect_to_func_save_rax(void* hookfn, uintptr_t reachaddr);

MH_NOINLINE
void swap_out_ptrs(void* start_addr, void** repls, unsigned n, bool dont_want_replaced = false);


void undo_all_reach_patches();

/*
	finds the address in the import section of a pointer to wantfunc
	or returns null
*/
void** locate_func_in_imports(void* wantfunc);

extern blamdll_t g_blamdll;
void** locate_import_ptr(const char* impname);

template<typename T>
static inline T* doomsym(uintptr_t addr) {
	return reinterpret_cast<T*>(g_blamdll.image_base + addr);
}

template<typename TRet, typename... Ts>
static inline TRet doomcall(uintptr_t addr, Ts... args) {
	return doomsym<TRet(Ts...)>(addr)(args...);
}

template<typename TRet, typename... Ts>
static inline TRet call_as(void* addr, Ts... args) {
	return reinterpret_cast<TRet(*)(Ts...)>(addr)(args...);
}

struct rtti_obj_locator_t {
	int unused[3];
	int typedescr_rva;
	int hierarchy_descr_rva;
	int objbase_rva;
};

struct mh_disassembler_t {
	ud m_ctx;
	void setup_for_addr(void* addr, size_t maxsize = ~0u) {
		ud_init(&m_ctx);
		ud_set_pc(&m_ctx, (uint64_t)addr);
		ud_set_input_buffer(&m_ctx, (uint8_t*)addr, maxsize);
	}

	void decode_enough_to_make_space(void* addr, size_t required_bytes, void** continuation_out, size_t* savesize) {
		
		size_t ngot = 0;

		setup_for_addr(addr, required_bytes*2);

		while(ngot < required_bytes) {
			ngot += ud_decode(&m_ctx);
		}

		*savesize = ngot;
		*continuation_out = mh_lea<char>(addr, ngot);
	}

	void* current_pc() {
		return (void*)(m_ctx.pc);
	}
	template<ud_mnemonic_code_t mnem>
	bool find_next_of_mnem() {

		while(true) {
			int disres=ud_disassemble(&m_ctx);

			if(disres<=0)
				return false;
			auto mnme = ud_insn_mnemonic(&m_ctx);
			if(mnme == mnem) {
				return true;
			}
		}
	}

	bool find_next_call(size_t limit) {
		size_t nb=0;
		while(nb < limit) {
			nb+=ud_disassemble(&m_ctx);
			auto mnme = ud_insn_mnemonic(&m_ctx);
			if(mnme == UD_Icall) {
				return true;
			}
		}
		return false;
	}

	void* extract_pcrel_value(unsigned which_operand) {
		return reinterpret_cast<void*>(((int64_t)m_ctx.operand[which_operand].lval.sdword) + m_ctx.pc);
	}

	void* get_call_target() {
		return reinterpret_cast<void*>(((int64_t)m_ctx.operand[0].lval.sdword) + m_ctx.pc);
	}

	/*
		intended for non-short jmps
	*/
	static void* first_jump_target(void* funcaddr) {
		mh_disassembler_t dis{};
		dis.setup_for_addr(funcaddr);
		/*
			todo: technically, we dont need to disassemble, we can just decode
			and check the first byte to see if its 0xE9
		*/
		if(!dis.find_next_of_mnem<UD_Ijmp>())
			return nullptr;

		return dis.extract_pcrel_value(0);

	}

	static void* first_call_target(void* funcaddr) {
		mh_disassembler_t dis{};
		dis.setup_for_addr(funcaddr);
		/*
			todo: technically, we dont need to disassemble, we can just decode
			and check the first byte to see if its 0xE8
		*/
		if(!dis.find_next_of_mnem<UD_Icall>())
			return nullptr;

		return dis.extract_pcrel_value(0);

	}

	static void* after_first_return(void* funcaddr) {
		mh_disassembler_t dis{};
		dis.setup_for_addr(funcaddr);
		
		/*
			todo: technically, we dont need to disassemble, we can just decode
			and check the first byte
		*/
		if(!dis.find_next_of_mnem<UD_Iret>()) {
			return nullptr;
		}

		return dis.current_pc();
	}
};



void* alloc_execmem(size_t size);

/*
	late binding implementation 

	the resolver is called once, the value is patched into the call site so that future calls just do mov rax, resolved_value
	using regfree_call here tells the compiler that the call will only spoil rax
*/

template<void* (*resolver)()>
struct feature_binder_ptr_t {

	MH_NOINLINE
	MH_REGFREE_CALL
	MH_CODE_SEG(".latebind")
	static void* execute_resolver() {
		char** retaddr = (char**)_AddressOfReturnAddress();

		char writebuf[] = {0x48,(char)0xb8,0,0,0,0,0,0,0,0,0,0};

		void* result = resolver();
		*reinterpret_cast<void**>(&writebuf[2]) = result;

		patch_memory((*retaddr) - 5, 10, writebuf);

		//advance past the nops we emitted for the resolver
		*retaddr += 5;
		return result;
	}

	MH_FORCEINLINE
	MH_PURE
	static void* get() {
		//near call = 0xE8 + (4bytes rva)
		//mov full 64 bit immediate = 10 bytes
		void* result = execute_resolver();
		//the nops also help prevent tail calls from being generated here
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		return result;
	}

	
};