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

#include "mh_guirender.hpp"
static idCVar* com_debugHUD = nullptr;

static void* g_original_rendergui = nullptr;
static std::string g_testmaterial = "";

static void mh_rendergui_callback(idDebugHUD* dbghud, idRenderModelGui* rgui) {
	//always show debughud
	//com_debugHUD->data->valueInteger=1;
	if (!com_debugHUD) {
		com_debugHUD = idCVar::Find("com_debugHUD");
	}
	com_debugHUD->data->valueInteger = 1;

	call_as<void>(g_original_rendergui, dbghud, rgui);

	if(g_testmaterial.length()) {

		void* mtr = get_material(g_testmaterial.c_str());

		if(mtr) {
			rgui->DrawRectMaterial(0, 0, 400, 400, mtr);
		}

		
	}
	/*rgui->DrawFilled(colorBrown, 0, 0, 200, 200);
	rgui->DrawString(500, 500, "yo yo yo, we got some text baybee", &colorCyan, true, 3);*/
}

static void cmd_set_testmaterial(idCmdArgs* args) {
	if(args->argc < 2){
		g_testmaterial.clear();
	}
	else {
		g_testmaterial = args->argv[1];
	}
}


#define idDebugHUDLocal_Render_VtblIdx	1
void mh_gui::install_gui_hooks() {
	

	void** debughudvtbl = get_class_vtbl(".?AVidDebugHUDLocal@@");


	g_original_rendergui = (void*)mh_rendergui_callback;
	swap_out_ptrs(&debughudvtbl[idDebugHUDLocal_Render_VtblIdx], &g_original_rendergui, 1, false);


	idCmd::register_command("mh_testmaterial", cmd_set_testmaterial, "Takes one arg, a material name. Renders the material to the test window. Passing no args clears the test window");
}