#pragma once



#pragma once






void calculate_text_size(const char* msg, float* out_w, float* out_h, float scale, unsigned* out_longest_line, unsigned* out_nlines);

struct gui_draw_context_t {
	idRenderModelGui* m_guimod;
	float m_virtwidth;
	float m_virtheight;

};
struct sh_ui_ele2d_t {
	rb_node m_tree_id_iter;
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
	sh_ui_ele2d_t();
	~sh_ui_ele2d_t();

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







sh_ui_ele2d_t* alloc_e2d(const char* id, float x, float y, float w, float h);

sh_ui_ele2d_t* find_ele_by_id(const char* id);
void snapgui_render(idRenderModelGui* guimod);

static inline sh_ui_ele2d_t* operator "" _ele2d(const char* name) {
	return find_ele_by_id(name);
}