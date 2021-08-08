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
#include "idmath.hpp"
#include <cassert>
#include <stdlib.h>
#include <stdio.h>
#include "gameapi.hpp"
#include "snaphakalgo.hpp"
#include "memorySystem.hpp"
#include "mh_staticmodel.hpp"

mh_typesizecached_t<yuckystring_m("idStaticModel")> g_idStaticModelSize{};

idStaticModelPtr idStaticModelPtr::CreateNew() {

	void* instance = idNew(g_idStaticModelSize());

	call_as<void>(descan::g_idStaticModel_ctor, instance);

	idStaticModelPtr result;
	result.m_ptr = instance;
	return result;
	

}