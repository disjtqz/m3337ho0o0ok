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
#include "GLState.hpp"
#include <mutex>
#include "clipboard_helpers.hpp"
#include "mh_gui_rasterizer.hpp"
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
	/*if (!com_debugHUD) {
		com_debugHUD = // idCVar::Find("com_debugHUD");
	}*/

	auto cvd = idCVar::Find("com_debugHUD");

	if (!cvd || !cvd->data) {
		return; //we're on v1, and running before the rvas have been populated

	}
	set_cvar_integer("com_debugHUD", 1);

	ensure_text_rasterization_initialized();
	//com_debugHUD->data->valueInteger = 1;

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
		//this makes testing a lot of different mtrs much easier
		const char* txt = get_clipboard_data();
		
		if (!txt || txt[0] == 0) {
			g_testmaterial.clear();
		}
		else {
			g_testmaterial = txt;
		}
	}
	else {
		g_testmaterial = args->argv[1];
	}
}

static void mh_testgui(idCmdArgs* args) {
	
	mh_gui::show_test_gui();
}

static void mh_set_cursor_glstate(idCmdArgs* args) {
	auto crsg = get_cursor_rendermodel();
	crsg->SetGLState(crsg->GetGLState() | atoi(args->argv[1]));

}

#define idDebugHUDLocal_Render_VtblIdx	1
void mh_gui::install_gui_hooks() {
	

	void** debughudvtbl = get_class_vtbl(".?AVidDebugHUDLocal@@");


	g_original_rendergui = (void*)mh_rendergui_callback;
	swap_out_ptrs(&debughudvtbl[idDebugHUDLocal_Render_VtblIdx], &g_original_rendergui, 1, false);

	init_sh_ingame_ui();
	idCmd::register_command("mh_testgui", mh_testgui, "test");

	idCmd::register_command("mh_testmaterial", cmd_set_testmaterial, "Takes one arg, a material name. Renders the material to the test window. Passing no args clears the test window");
	idCmd::register_command("mh_set_cursor_glstate", mh_set_cursor_glstate, "<statebits> test func");
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





static void postrender_dlgbox(void* ud, mh_dom_t* dom, idRenderModelGui* rmg) {

	//mh_ui_vector_t<idDrawVert> verts;
	//mh_ui_vector_t<unsigned short> indices;
	float textend;
	{
		mh_uigeo_builder_t gb;




		gb.set_material(g_dbgfont_mtr);
		//bb.a = 0.5;
		//make_circle(&gb.verts, &gb.indices, 400, 400, 200, 128, bb);
		//draw_char(&gb, 'H', 400, 400, mh_colorRed);
		textend = draw_string(&gb, 400, 400, "hello world custom text", mh_colorRed);
		//gb.set_material("_white");
		//submit_rect(&gb, 800, 800, 200, 200);
	//	submit_line(&gb, 100, 100, 900, 900, colorWhite);
		gb.stream2gui(rmg);

	}
	{

		mh_uigeo_builder_t gb2;

		submit_hollow_rect(&gb2, 399, 399 + (get_smallchar_height() / 2.0f), textend - 399, get_smallchar_height(), 1.0f, mh_colorBlue);

		gb2.stream2gui(rmg);
	}
	{
		mh_uigeo_builder_t gbc;
		make_circle(&gbc, 800, 800, 200, 128, mh_colorRed);


		mh_color_t othercircle{ 0, 0, 255, 64 };
		make_circle(&gbc, 800, 800, 100, 64, othercircle);

		gbc.stream2gui(rmg);
	}

	{

		mh_uigeo_builder_t gbsc;

		make_circle_quarter(&gbsc, 1000, 1000, 50, 64, 3, mh_colorGreen);
		gbsc.stream2gui(rmg);
	}
}

static mh_dom_t* g_dlgbox_dom = nullptr;

MH_NOINLINE
MH_REGFREE_CALL
static void init_dlgbox_dom() {
	mh_dom_t* newdm = mh_gui::new_named_dom("global_lineedit_dlgbox");

	auto basebox_ele = newdm->alloc_e2d("basebox", 0.40, 0.40, 0.10, 0.10);

	basebox_ele->init_rect("_white", colorBrown);

	//create thin black bars around rect
	auto box_outline_top = newdm->alloc_e2d("basebox_top_outline", 0.40, 0.40, 0.10, 0.01);
	box_outline_top->init_rect("_white", colorBlack);

	auto box_outline_bottom = newdm->alloc_e2d("basebox_bottom_outline", 0.40, 0.49, 0.10, 0.01);

	box_outline_bottom->init_rect("_white", colorBlack);


	auto box_outline_left = newdm->alloc_e2d("basebox_left_outline", 0.40, 0.40, 0.01, 0.10);

	box_outline_left->init_rect("_white", colorBlack);


	auto box_outline_right = newdm->alloc_e2d("basebox_right_outline", 0.49, 0.40, 0.01, 0.10);

	box_outline_right->init_rect("_white", colorBlack);

	auto textedit_contents = newdm->alloc_e2d("textedit_contents", 0.42, 0.42, 0.08, 0.08);

	textedit_contents->init_text("Textedit contents.", 1, colorGreen);
	textedit_contents->set_depth(1.0f);

	newdm->add_postrender_cb(postrender_dlgbox, nullptr);
}

mh_dom_t* mh_gui::get_dialogbox_dom() {
	if (!g_dlgbox_dom) {


	}

}
void mh_gui::show_test_gui() {

	/*mh_dom_t* mydom = new_named_dom("testgui1");

	auto testele = mydom->alloc_e2d("testele", 0.25, 0.25, 0.5, 0.5);
	testele->m_text_positioning_style = _text_style_centered;
	testele->init_rect("swf/hud/menus/re_spec_station_textures/swf_images/common/frame/frame_backplate_", colorWhite);
	testele->init_text("Heres some text for our testgui", 1.0, colorCyan);*/

	init_dlgbox_dom();
}