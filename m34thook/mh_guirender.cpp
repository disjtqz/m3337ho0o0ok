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

#include "mhgui.hpp"
#include <mutex>
static idCVar* com_debugHUD = nullptr;

static void* g_original_rendergui = nullptr;
static std::string g_testmaterial = "";

static std::mutex g_doms_mutex{};

struct domentry_t {
	rb_node m_node;
	const char* m_name;
	mh_dom_t* m_dom;

	domentry_t() {
		rb_init_node(&m_node);
		m_name=nullptr;
		m_dom=nullptr;
	}

};

static rb_root g_doms_search_tree{};


static void mh_render_doms(idRenderModelGui* guimodel) {
	
	g_doms_mutex.lock();

	for(auto&& domnode : sh::rb::rb_iterate<domentry_t, cs_offsetof_m(domentry_t, m_node)>(g_doms_search_tree)) {
		domnode->m_dom->snapgui_render(guimodel);
	}

	g_doms_mutex.unlock();
}


static void mh_rendergui_callback(idDebugHUD* dbghud, idRenderModelGui* rgui) {
	//always show debughud
	//com_debugHUD->data->valueInteger=1;
	if (!com_debugHUD) {
		com_debugHUD = idCVar::Find("com_debugHUD");
	}
	com_debugHUD->data->valueInteger = 1;

	call_as<void>(g_original_rendergui, dbghud, rgui);

	mh_render_doms(rgui);
	//also has memory corruption :(
	// 
	//rgui->DrawFilled(colorBlue, 100, 100, 200, 200);
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

static void mh_testgui(idCmdArgs* args) {
	
	mh_gui::show_test_gui();
}

#define idDebugHUDLocal_Render_VtblIdx	1
void mh_gui::install_gui_hooks() {
	

	void** debughudvtbl = get_class_vtbl(".?AVidDebugHUDLocal@@");


	g_original_rendergui = (void*)mh_rendergui_callback;
	swap_out_ptrs(&debughudvtbl[idDebugHUDLocal_Render_VtblIdx], &g_original_rendergui, 1, false);

	init_sh_ingame_ui();
	idCmd::register_command("mh_testgui", mh_testgui, "test");

	idCmd::register_command("mh_testmaterial", cmd_set_testmaterial, "Takes one arg, a material name. Renders the material to the test window. Passing no args clears the test window");
}



mh_dom_t* mh_gui::new_named_dom(const char* name) {
	
	g_doms_mutex.lock();
	sh::rb::insert_hint_t hint{};
	domentry_t* domfor = sh::rb::rbnode_find<domentry_t, 0>(&g_doms_search_tree, name, [](domentry_t* entry, const char* name) {
			return sh::string::strcmp(entry->m_name, name);
		}, &hint);

	if(domfor) {
		idLib::Printf("Attempted to create dom named %s, but it already exists! returning existing dom.\n");

	}
	else {
		domfor = new domentry_t();
		domfor->m_name = _strdup(name);
		domfor->m_dom = new_dom();
		hint.insert(&domfor->m_node, &g_doms_search_tree);
	}

	mh_dom_t* result = domfor->m_dom;

	g_doms_mutex.unlock();

	return result;

}

void mh_gui::show_test_gui() {

	mh_dom_t* mydom = new_named_dom("testgui1");

	auto testele = mydom->alloc_e2d("testele", 0.25, 0.25, 0.5, 0.5);
	testele->m_text_positioning_style = _text_style_centered;
	testele->init_rect("swf/hud/menus/re_spec_station_textures/swf_images/common/frame/frame_backplate_", colorWhite);
	testele->init_text("Heres some text for our testgui", 1.0, colorCyan);
}