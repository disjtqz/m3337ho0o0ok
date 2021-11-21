#include "xbyak/xbyak.h"
#include "mh_defs.hpp"

#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "meathook.h"
#include "cmdsystem.hpp"
#include "idtypeinfo.hpp"
#include "eventdef.hpp"
#include "scanner_core.hpp"
#include "idLib.hpp"
#include "idStr.hpp"
#include "clipboard_helpers.hpp"
#include <vector>
#include "errorhandling_hooks.hpp"
#include "gameapi.hpp"
#include "idmath.hpp"
#include "memscan.hpp"
#include "mh_memmanip_cmds.hpp"
#include "snaphakalgo.hpp"
#include <string.h>
#include "mh_guirender.hpp"
#include "mh_editor_mode.hpp"
#include <string>
#include <immintrin.h>

extern void install_spawning_commands();
extern void install_cheat_cmds();
extern void install_query_cmds();
extern void install_editor_cmds();
extern void install_dumpngen_cmds();
extern void install_miscndev_cmds();
//executed on first frame
//see mh_cmds_cheats
extern void cheats_postinit();


void* __fastcall idFileResourceCompressed__GetFile(__int64 a1);

static bool return_1() {

	return 1;
}


static void do_nothing() {

}
sh_heap_t g_mh_heap = nullptr;
void* g_mh_heap_base = nullptr;
sh_heap_t get_mh_heap() {
	return g_mh_heap;
}

static void* g_original_renderthread_run = nullptr;
static __int64 testdebugtools(void* x) {

	//call_as<void>(descan::g_renderDebugTools, get_rendersystem());
	return call_as<__int64>(g_original_renderthread_run, x);
}
void meathook_final_init() {
	//compute classinfo object super object deltas for quick inheritance traversal
	idType::init_prop_rva_table();
	idType::compute_classinfo_mh_payloads();
	idCVar::get_cvardata_rvas();
	
	

	descan::run_gamelib_postinit_scangroups();
	cheats_postinit();
}

static void* nothinfunc = (void*)do_nothing;
static void do_cvar_toggle() {

	/*
		replace offset 16 on vftbl of idcmdsystem with a no-op function.
		the function sets a variable on a pointer in tls, and it is called twice to disable first dev commands, then executing bound dev commands
		swapping it out eliminates this issue, also renders g_command_patch_area useless
	*/
	void* cmdsystem = *(void**)descan::g_idcmdsystem;

	void* cmdsystem_vftbl = *(void**)cmdsystem;


	swap_out_ptrs(reinterpret_cast<char*>(cmdsystem_vftbl) + 16, &nothinfunc, 1, false);


}
static void test_cvar_disable(idCmdArgs* args) {
	do_cvar_toggle();

	
}



void meathook_init() {
	g_mh_heap = sh::heap::
	install_gameapi_hooks();

	void** vtbl_render = get_class_vtbl(".?AVidRenderThread@@");
	g_original_renderthread_run = (void*)testdebugtools;


	swap_out_ptrs(&vtbl_render[1], &g_original_renderthread_run, 1, false);





	//redirect_to_func(descan::g_renderDebugTools, (uintptr_t)descan::g_idRender_PrintStats, true);
	redirect_to_func((void*)idFileResourceCompressed__GetFile, (uintptr_t)/* doomsym<void>(doomoffs::idFileResourceCompressed__GetFile)*/ descan::g_idfilecompressed_getfile, true);
	//g_levelReload = redirect_to_func((void*)LevelReload_CaptureParameters, (uintptr_t)/* doomsym<void>(doomoffs::idFileResourceCompressed__GetFile)*/ descan::g_levelreload, true);
	//g_func992170 = redirect_to_func((void*)LevelReload_PreventUninitializedTick, (uintptr_t)/* doomsym<void>(doomoffs::idFileResourceCompressed__GetFile)*/ descan::g_init_func_rva_992170, true);
	install_error_handling_hooks();
	//no longer needed, see below
	//unsigned patchval_enable_commands = 0;
	do_cvar_toggle();
	

	//patch_memory(descan::g_command_patch_area, 4, (char*)&patchval_enable_commands);

#ifdef MH_DEV_BUILD
	//for array patch for expanding ai
	//please remove for actual release
	DWORD fuckyu;
	VirtualProtect(g_blamdll.image_base, g_blamdll.image_size, PAGE_EXECUTE_READWRITE, &fuckyu);
#endif

	





	idCmd::register_command("test_cvar_disable", test_cvar_disable, "fff");

	install_memmanip_cmds();
	install_spawning_commands();
	install_cheat_cmds();
	install_query_cmds();
	install_editor_cmds();
	install_dumpngen_cmds();
	install_miscndev_cmds();
	//idCmd::register_command("mh_test_persistent_text", test_persistent_text, "Test persistent onscreen text");
	//idCmd::register_command("mh_phys_test", test_physics_op, "test physics ops");
	// Start rpc server.
}