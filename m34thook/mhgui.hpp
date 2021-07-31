#pragma once

/*
	good gui-looking materials we can use: 
		loading/loading_info_textures/swf_images/loading_screen/loading_tip_box_
		swf/hud/menus/dossier/dossier_arsenal_textures/swf_images/dossier/arsenal/weaponnav_bg_


	really good:
		swf/hud/menus/dossier/dossier_arsenal_textures/swf_images/end_of_level/bg_window_large_
		swf/hud/menus/dossier/dossier_map_textures/swf_images/hud/demonic_corruption/hud_slayer_demoncorruption_container_single_
		swf/main_menu/screens/seasons_textures/swf_images/seasons/profile_nameplate_rect_bg_
		swf/main_menu/screens/seasons_textures/swf_images/seasons/profile_icon_circle_bg_
		swf/hud/menus/re_spec_station_textures/swf_images/common/frame/frame_backplate_
	good icons:
		swf/hud/menus/dossier/dossier_arsenal_textures/swf_images/icons/icon_list_item_notification_

	scrollbar-y:
	swf/hud/menus/dossier/dossier_codex_textures/swf_images/common/scrollbar/hud_scrollbar_track_center_
*/

void calculate_text_size(const char* msg, float* out_w, float* out_h, float scale, unsigned* out_longest_line, unsigned* out_nlines);

struct gui_draw_context_t {
	struct idRenderModelGui* m_guimod;
	float m_virtwidth;
	float m_virtheight;

};

class mh_dom_t;

struct mh_ui_ele_t {
	rb_node m_tree_id_iter;
	mh_dom_t* m_owning_dom;

	const char* id;
	float x, y;
	float width, height;

	struct {
		unsigned m_hidden : 1;
	};
	unsigned scrollYPos;

	unsigned mat_color;
	void* material;
	unsigned txt_color;
	const char* txt;
	float txt_scale;

	gui_draw_context_t* m_current_ctx;
	void draw(gui_draw_context_t& rmg);
	void set_text(const char* newtxt);

	void init_text(const char* message, float scale, idColor color);
	void init_rect(const char* material, idColor color);
	void init_common(const char* id, float _x, float _y, float w, float h);

	static void* operator new(size_t sz);

	static void operator delete(void* vp);
	mh_ui_ele_t();
	~mh_ui_ele_t();

	void scroll_text_up();
	void scroll_text_down();

	float calcw() const {
		return width*m_current_ctx->m_virtwidth;
	}
	float calch() const {
		return height * m_current_ctx->m_virtheight;
	}

	float calcx() const {
		return x* m_current_ctx->m_virtwidth;
	}
	float calcy() const {
		return y * m_current_ctx->m_virtheight;
	}

	void hide() {
		m_hidden = true;
	}

	void unhide() {
		m_hidden =false;
	}
};



class mh_dom_t {
public:
	virtual ~mh_dom_t() {
	}
	virtual mh_ui_ele_t* alloc_e2d(const char* id, float x, float y, float w, float h) = 0;

	virtual mh_ui_ele_t* find_ele_by_id(const char* id) = 0;
	virtual void snapgui_render(struct idRenderModelGui* guimod) = 0;


};
void init_sh_ingame_ui();
mh_dom_t* new_dom();


