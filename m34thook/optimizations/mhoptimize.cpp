#include "xbyak/xbyak.h"
#include "mh_defs.hpp"

#include "game_exe_interface.hpp"
#include "meathook.h"
#include "cmdsystem.hpp"
#include "idtypeinfo.hpp"
#include "scanner_core.hpp"
#include "idLib.hpp"
#include "idStr.hpp"
#include "gameapi.hpp"
#include "memscan.hpp"
#include "snaphakalgo.hpp"


//extern declared in meathook.cpp
void cmd_optimize(idCmdArgs* args) {
	make_ret(descan::g_security_check_cookie);
	make_ret(descan::g_alloca_probe);

	/*
		sqrtss xmm0, xmm0
		ret
	*/
	char sqrtf_override[] = { (char)0xF3, 0x0F, 0x51, (char)0xC0, (char)0xC3 };

	patch_memory_with_undo(descan::g_sqrtf, sizeof(sqrtf_override), sqrtf_override);

	char sqrt_override[] = { (char)0xF2, 0x0F, 0x51, (char)0xC0, (char)0xC3 };

	patch_memory_with_undo(descan::g_sqrt, sizeof(sqrtf_override), sqrt_override);
	if(args->argc > 1) {
		make_ret(descan::g_idlib_vprintf);
	}

#if 1
	redirect_to_func(g_shalgo.m_sroutines.m_strcmp, 0x1403B2A20ULL, true);//test idStr::cmp redirect
	redirect_to_func(g_shalgo.m_sroutines.m_strlen, 0x1403B5460ULL, true);//test strlen redirect
#endif

	//redirect_to_func(g_shalgo.m_sroutines. descan::g__ZN5idStr4IcmpEPKcS1_)
}
