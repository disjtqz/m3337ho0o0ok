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
	//too lazy to check ret
	//
	MessageBoxA(nullptr, tmpbuf, Fatal ? "Engine Fatal Error" : "Engine Error", MB_ICONERROR);

	TerminateProcess(GetCurrentProcess(), 1);

}

void install_error_handling_hooks() {
	redirect_to_func((void*)idfatalerror_override<true>, (uintptr_t)descan::g_idlib_fatalerror);
	redirect_to_func((void*)idfatalerror_override<false>, (uintptr_t)descan::g_idlib_error);
}