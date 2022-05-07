#include "mh_defs.hpp"

#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "meathook.h"
#include "cmdsystem.hpp"
#include "idtypeinfo.hpp"
#include "eventdef.hpp"
#include "scanner_core.hpp"
#include "errorhandling_hooks.hpp"
#include <cstdio>

//trying to help out guy on discord
#define		LOG_ERROR_BEFORE_REBOOT

#define		QUICKLOG_PATH		"quicklog.txt"
template<bool Fatal>
__declspec(noreturn)
static void idfatalerror_override(const char* msg, ...) {
	va_list ap;
	va_start(ap, msg);
	char tmpbuf[16384]{ 0 };
	vsprintf_s(tmpbuf, msg, ap);
#ifdef LOG_ERROR_BEFORE_REBOOT
	FILE* quicklog_file=nullptr;
	fopen_s(&quicklog_file, QUICKLOG_PATH, "w");
	fputs(tmpbuf, quicklog_file);
	fclose(quicklog_file);

#endif

	auto show_the_messagebox = [tmpbuf]() {
		MessageBoxA(nullptr, tmpbuf, Fatal ? "Engine Fatal Error" : "Engine Error", MB_ICONERROR);
	};

#if defined(MH_ENABLE_SAFE_DECL_RELOAD)

	//yield the formatted string on this stack, caller should print it before resetting our stack
	
	if (g_decl_reload_context && NtCurrentTeb() == g_teb_for_decl_reload) {

		//we will not be returned to after this. 
		g_decl_reload_context->yield_to(g_decl_reload_main_thread_context, &tmpbuf[0]);
	}
	else {
		show_the_messagebox();
	}

#else
	//too lazy to check ret
	//
	show_the_messagebox();
#endif
	TerminateProcess(GetCurrentProcess(), 1);

}

void install_error_handling_hooks() {
	redirect_to_func((void*)idfatalerror_override<true>, (uintptr_t)descan::g_idlib_fatalerror);
	redirect_to_func((void*)idfatalerror_override<false>, (uintptr_t)descan::g_idlib_error);
}