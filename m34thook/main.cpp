#include "mh_defs.hpp"

#include <Windows.h>
#include "game_exe_interface.hpp"
#include "memscan.hpp"
#include "doomoffs.hpp"
#include "meathook.h"
#include "scanner_core.hpp"
#include "fs_hooks.hpp"
#include "mh_config_globals.hpp"
sh_heap_t g_mh_heap = nullptr;
static void* g_mh_heap_base = nullptr;
char* g_mh_module_base = nullptr;
//#define		SUPPORT_NEWER_VERSIONS
MH_NOINLINE
void mh_error_message(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	char errbuf[65536];
	vsprintf_s(errbuf, fmt, ap);

	MessageBoxA(nullptr, errbuf, "M347h00k Error", MB_ICONERROR);

	va_end(ap);
}
CS_COLD_CODE
CS_NOINLINE
CS_NORETURN
void cs::_cs_assert_fail_raise(const cs::cs_assert_fail_descr_t* descr) {

	char errbuff[4096] = { 0 };

	strncpy(errbuff, "Assertion failed in function ", sizeof(errbuff));
	char line_text[32];

	//itoa(descr->line_no, line_text, 10);

	strncat(errbuff, descr->fn_name, sizeof(errbuff));
	strncat(errbuff, line_text, sizeof(errbuff));

	strncat(errbuff, " in file ", sizeof(errbuff));

	strncat(errbuff, descr->_file_name, sizeof(errbuff));


	MessageBoxA(nullptr, errbuff, "M347h00k Assertion Failure", MB_ICONERROR);

	


}
static HMODULE g_real_xinput = nullptr;
snaphak_algo_t g_shalgo{};
__declspec(noinline)
static void init_xinput() {
	char tmpbuf[512];
	memset(tmpbuf, 0, sizeof(tmpbuf));
	ExpandEnvironmentStringsA("%WINDIR%\\System32\\XINPUT1_3.dll", tmpbuf, sizeof(tmpbuf));
	g_real_xinput = LoadLibraryA(tmpbuf);
	if (!g_real_xinput) {
		MessageBoxA(nullptr, "Could not load the original XINPUT library", "Error", 0);
		exit(0);
	}
}

static HMODULE get_real_xinput() {
	if (!g_real_xinput) {

		init_xinput();

	}

	return g_real_xinput;
}

static void* xinputgetstate_ptr = nullptr;
static void* xinputsetstate_ptr = nullptr;
  
extern "C"
__declspec(dllexport)

DWORD XInputGetState(
	DWORD        dwUserIndex,
	void* pState
) {
	if (!xinputgetstate_ptr) {
		xinputgetstate_ptr = GetProcAddress(get_real_xinput(), "XInputGetState");
	}

	return reinterpret_cast<DWORD(*)(DWORD, void*)>(xinputgetstate_ptr)(dwUserIndex, pState);
}

extern "C"
__declspec(dllexport)

DWORD XInputSetState(
	DWORD            dwUserIndex,
	void* pVibration
) {

	if (!xinputsetstate_ptr) {
		xinputsetstate_ptr = GetProcAddress(get_real_xinput(), "XInputSetState");
	}

	return reinterpret_cast<DWORD(*)(DWORD, void*)>(xinputsetstate_ptr)(dwUserIndex, pVibration);
}
static  void* original_gamelib_init = nullptr;
static std::atomic_bool g_did_gamelib_init_already = false;

static int gamelib_init_forwarder(void* x, void* y) {

	if(g_did_gamelib_init_already.exchange(true) == false) {
		descan::run_late_scangroups();

		int result = reinterpret_cast<int (*)(void*, void*)>(original_gamelib_init)(x, y);///doomcall<int>(doomoffs::gamelib_initialize, x, y);

		//descan::run_gamelib_postinit_scangroups();
		meathook_init();

		hook_idfilesystem();
		return result;
	}
	else {
		return 0;
	}

}
static bool g_did_init = false;

static char g_old_getsysteminfo_bytes[32];
static void* g_kernel32_getsysinfo_ptr = nullptr;


static void patch_engine_after_unpack() {

	descan::locate_critical_features();
	
	void* gamelib_init_addr = descan::g_gamelib_initialize_ptr;

	void** ourguy = (void**)(gamelib_init_addr);
	original_gamelib_init = *ourguy;

	*ourguy = (void*)gamelib_init_forwarder;
}

static std::atomic_bool g_did_sysinf_hook_already = false;

static void getsysinf(LPSYSTEM_INFO inf) {

	reinterpret_cast<void (*)(LPSYSTEM_INFO)>(GetProcAddress(GetModuleHandleA("KernelBase.dll"), "GetSystemInfo"))(inf);
	patch_memory(g_kernel32_getsysinfo_ptr, 16, g_old_getsysteminfo_bytes);
	FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
	if(g_did_sysinf_hook_already.exchange(true) == false)
		patch_engine_after_unpack();
}

bool g_mh_hugepage_support = false;

//static HMODULE g_idt7 = nullptr;
BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
) {
	if(DLL_PROCESS_DETACH == fdwReason) {
		undo_all_reach_patches();
	}
	if (g_did_init)
		return TRUE;
	//https://stackoverflow.com/questions/2126657/how-can-i-get-hinstance-from-a-dll
	//"Note As it turns out, HMODULEs and HINSTANCEs are exactly the same thing"
	g_mh_module_base = (char*)hinstDLL;
	sh_algo_init(&g_shalgo);

	mh_global_config_load();
	if (mh_test_devflag(mh_devflags::enable_hugepages_on_start)) {
		g_mh_hugepage_support = sh::vmem::try_enable_hugepages() == 0;
	}

	if (mh_test_devflag(mh_devflags::preallocate_entire_game_heap)) {

		void* ghost_heap_base = sh::vmem::allocate_rw(g_preallocated_ghost_heap_size | (g_mh_hugepage_support ? SNAPHAK_SIZE_FLAG_HUGEPAGES : 0));

		sh_heap_t heap = sh::heap::create_heap_from_mem(ghost_heap_base, g_preallocated_ghost_heap_size, 0);

		void* peb = *mh_lea<void*>(NtCurrentTeb(), 0x60);

		*mh_lea<sh_heap_t>(peb, 0x30) = heap;
	}
	


	g_mh_heap_base = sh::vmem::allocate_rw(MH_HEAP_SIZE | (g_mh_hugepage_support ? SNAPHAK_SIZE_FLAG_HUGEPAGES : 0));

	g_mh_heap = sh::heap::create_heap_from_mem(g_mh_heap_base, MH_HEAP_SIZE, 0);
	g_did_init = true;


	init_reach();
	g_kernel32_getsysinfo_ptr = GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetSystemInfo");
	memcpy(g_old_getsysteminfo_bytes, g_kernel32_getsysinfo_ptr, 16);

	redirect_to_func((void*)getsysinf, (uintptr_t)g_kernel32_getsysinfo_ptr, true);
	FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
	return TRUE;
}
static void* xinputgetcaps_ptr = nullptr;

extern "C"
__declspec(dllexport)
DWORD  XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, void** pCapabilities) {
	if (!xinputgetcaps_ptr)
		xinputgetcaps_ptr = GetProcAddress(get_real_xinput(), "XInputGetCapabilities");

	return reinterpret_cast<DWORD(*)(DWORD dwUserIndex, DWORD dwFlags, void** pCapabilities)>(xinputgetcaps_ptr)(dwUserIndex, dwFlags, pCapabilities);
}