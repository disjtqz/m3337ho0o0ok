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
#include "mhgui.hpp"


static sh_heap_t g_ui_frontend_heap = nullptr;
static void* g_ui_frontend_backing_storage = nullptr;
template<typename T>
using allocator_sh_ui_t = sh::heap::sh_heap_based_allocator_t<T, &g_ui_frontend_heap, 0>;



void init_sh_ingame_ui() {
	g_ui_frontend_backing_storage = sh::vmem::allocate_rw(1024 * 1024);
	g_ui_frontend_heap = sh::heap::create_heap_from_mem(g_ui_frontend_backing_storage, 1024 * 1024, 0);
}


void calculate_text_size(const char* msg, float* out_w, float* out_h, float scale, unsigned* out_longest_line, unsigned* out_nlines) {

	float res = .0;

	unsigned nlines = 0;
	unsigned maxchars = 0;
	unsigned currentlinelength = 0;
	for (unsigned i = 0; ; ++i) {
		if (msg[i] == '\n' || msg[i] == 0) {
			maxchars = maxchars > currentlinelength ? maxchars : currentlinelength;
			currentlinelength = 0;
			nlines++;
			if (!msg[i])
				break;
		}
		else {
			++currentlinelength;
		}
	}

	*out_w = (idRenderModelGui::get_SMALLCHAR_WIDTH() * (float)maxchars) * scale;
	*out_h = nlines * (scale*idRenderModelGui::get_SMALLCHAR_HEIGHT());
	*out_longest_line = maxchars;
	*out_nlines = nlines;

}

void* sh_ui_ele2d_t:: operator new(size_t sz) {
	return sh::heap::alloc_from_heap(g_ui_frontend_heap, sz, 0);
}

void sh_ui_ele2d_t::operator delete(void* vp) {
	sh::heap::free_from_heap(g_ui_frontend_heap, vp, 0);
}
sh_ui_ele2d_t::sh_ui_ele2d_t() {
	rb_init_node(&m_tree_id_iter);
	id = nullptr;
	x = 0;
	m_hidden=false;
	y = 0;
	width = 0;
	height = 0;
	scrollYPos = 0;
	mat_color = 0;
	material = nullptr;
	txt_color = 0;
	txt = nullptr;
	txt_scale = 0;
}

sh_ui_ele2d_t::~sh_ui_ele2d_t() {
	if (txt) {
		sh::heap::free_from_heap(g_ui_frontend_heap, txt, 0);
		txt = nullptr;
	}
}
void sh_ui_ele2d_t::set_text(const char* newtxt) {
	if (txt) {
		sh::heap::free_from_heap(g_ui_frontend_heap, txt, 0);
		txt = nullptr;
		scrollYPos = 0;
	}
	if (newtxt) {
		unsigned len = sh::string::strlength(newtxt);
		char* newtext = (char*)sh::heap::alloc_from_heap(g_ui_frontend_heap, len + 1, 0);

		sh::memops::nt_move_mem_flush(newtext, newtxt, len + 1);


		txt = newtext;
	}
}
void sh_ui_ele2d_t::init_text(const char* message, float scale, idColor color) {
	unsigned len = sh::string::strlength(message);
	char* newtext = (char*)sh::heap::alloc_from_heap(g_ui_frontend_heap, len + 1, 0);

	sh::memops::nt_move_mem_flush(newtext, message, len + 1);
	txt = newtext;

	txt_scale = scale;
	txt_color = color.PackColor();

	sh::memops::sfence_if();


}
void sh_ui_ele2d_t::init_rect(const char* _material, idColor color) {
	material = get_material(_material);
	mat_color = color.PackColor();

}
void sh_ui_ele2d_t::init_common(const char* _id, float _x, float _y, float w, float h) {
	id = _id;
	x = _x;
	y = _y;
	width = w;
	height = h;
}

void sh_ui_ele2d_t::scroll_text_up() {
	if(scrollYPos == 0)
		return;
	--scrollYPos;
}
void sh_ui_ele2d_t::scroll_text_down() {
	++scrollYPos;
}



void sh_ui_ele2d_t::draw(gui_draw_context_t& g) {

	if(m_hidden) {
		return;
	}
	auto mk2d = [](float x, float y) {
		return idVec4{ .x = x, .y = y, .z = .0f, .w = .0f };
	};
	m_current_ctx = &g;
	float rx = calcx();
	float ry = calcy();
	float rw = calcw();
	float rh = calch();


	if (material) {
		idVec4 vrts[4] = { mk2d(rx, ry), mk2d(rw + rx, ry), mk2d(rw + rx, rh + ry), mk2d(rx, rh + ry) };

		for (auto&& v : vrts) {
			v.z = (v.x - x) / rw;
			v.w = (v.y - y) / rh;

		}
		g.m_guimod->set_current_vertexcolor(mat_color);

		g.m_guimod->DrawStretchPic(&vrts[0], &vrts[1], &vrts[2], &vrts[3], material, 1.0);

	}


	if (txt) {
		float fx, fy;
		unsigned longline;
		unsigned maxline;
		calculate_text_size(txt, &fx, &fy, txt_scale, &longline, &maxline);

		if (fx > rw) {
			txt_scale *= rw / fx;
			calculate_text_size(txt, &fx, &fy, txt_scale, &longline, &maxline);
		}

		float just =0;// (width - fx) / 2;

		float currentx = 0;
		float currenty = 0;

		g.m_guimod->set_current_vertexcolor(txt_color);




		float text_increment_y = (txt_scale * idRenderModelGui::get_SMALLCHAR_HEIGHT()) + 1;
		unsigned current_line = 0;
		unsigned i = 0;

		if(scrollYPos > maxline) {
			scrollYPos = maxline;
			return;
		}
		if(scrollYPos) {
			//seek to current line
			for (; txt[i]; ++i) {
				if(txt[i] == '\n') {
					++current_line;
					if(current_line == scrollYPos)
						break;
				}
			}
		}
		for (; txt[i]; ++i) {
			if (txt[i] == '\n') {


				currenty += text_increment_y;
				
				if(currenty + text_increment_y > rh) { //text would be chopped off
					break;
				}

				currentx = just;
			}
			else {
				if (txt[i] != '\t') {
					g.m_guimod->DrawChar(currentx + rx, currenty + ry, txt[i], txt_scale);
				}
				currentx += txt_scale * idRenderModelGui::get_SMALLCHAR_WIDTH();
			}
		}


	}

}
static rb_root g_2d_eles_tree{};

static sh_ui_ele2d_t* try_find_2dele(const char* id, sh::rb::insert_hint_t& hint) {
	return sh::rb::rbnode_find<sh_ui_ele2d_t, cs_offsetof_m(sh_ui_ele2d_t, m_tree_id_iter), const char*>(&g_2d_eles_tree,
		id, [](sh_ui_ele2d_t* l, const char* r) {
			return sh::string::strcmp(l->id, r);
		}, &hint);
}

sh_ui_ele2d_t* alloc_e2d(const char* id, float x, float y, float w, float h) {
	sh::rb::insert_hint_t ihint;
	sh_ui_ele2d_t* result = try_find_2dele(id, ihint);
	if (!result) {
		result = new sh_ui_ele2d_t();//(sh_ui_ele2d_t*)sh::heap::alloc_from_heap(g_ui_frontend_heap, sizeof(sh_ui_ele2d_t),0);
		
		result->id = id;
		ihint.insert(&result->m_tree_id_iter, &g_2d_eles_tree);
	}


	result->init_common(id, x, y, w, h);
	return result;

}

sh_ui_ele2d_t* find_ele_by_id(const char* id) {
	sh::rb::insert_hint_t skiphint;
	return try_find_2dele(id, skiphint);
}

void snapgui_render(idRenderModelGui* guimod) {

	for(auto element : sh::rb::rb_iterate<sh_ui_ele2d_t, cs_offsetof_m(sh_ui_ele2d_t, m_tree_id_iter)>(g_2d_eles_tree)) {

		gui_draw_context_t g{.m_guimod = guimod, .m_virtwidth = guimod->GetVirtualWidth(), .m_virtheight = guimod->GetVirtualHeight()};
		element->draw(g);
	}
}