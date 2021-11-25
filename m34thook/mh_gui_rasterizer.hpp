#pragma once

struct mh_uigeo_builder_t {
	void* m_mtr;
	mh_ui_vector_t<idDrawVert> verts;
	mh_ui_vector_t<unsigned short> indices;
	mh_uigeo_builder_t() : m_mtr(get_material("_white")) {
		verts.reserve(256);
		indices.reserve(256 * 3);
	}


	void stream2gui(idRenderModelGui* rmg) {
		idDrawVert* wrv = rmg->AllocTris(verts.size(), indices.data(), indices.size(), m_mtr);
		sh::memops::nt_move_mem(wrv, verts.data(), sizeof(idDrawVert) * verts.size());
	}
	void set_material(const char* name) {
		m_mtr = get_material(name);
		if (!m_mtr)
			m_mtr = get_material("_white");


	}

	void set_material(void* mtr) {
		if (!mtr)
			mtr = get_material("_white");
		m_mtr = mtr;
	}
	unsigned idxbase() const {
		return verts.size();
	}

	unsigned addvert(idDrawVert v) {
		unsigned res = verts.size();
		verts.push_back(v);
		return res;
	}

	template<typename... Ts>
	void addidx(Ts... vals) {

		unsigned localtris[] = { vals... };
		for (auto&& tr : localtris) {
			indices.push_back(tr);
		}
	}

};

void make_partial_triangle_fan(
	mh_ui_vector_t<idDrawVert>* verts,
	mh_ui_vector_t<unsigned short>* indices,
	float centerx,
	float centery,
	float radius,
	unsigned numsegm,
	mh_color_t color);

void make_circle(
	mh_ui_vector_t<idDrawVert>* verts,
	mh_ui_vector_t<unsigned short>* indices,
	float centerx,
	float centery,
	float radius,
	unsigned numsegm,
	mh_color_t color);

void submit_rect(mh_uigeo_builder_t* ub, float upperx, float uppery, float width, float height, mh_color_t color = mh_colorWhite);

void submit_line(mh_uigeo_builder_t* ub, float x1, float y1, float x2, float y2);

void submit_hollow_rect(mh_uigeo_builder_t* ub, float x, float y, float width, float height, float thiqness, mh_color_t color);

extern void* g_dbgfont_mtr;
void ensure_text_rasterization_initialized();
void draw_char(mh_uigeo_builder_t* ub, unsigned character, float xpos, float ypos, mh_color_t color, float scale=1.0f);
//returns ending xpos
float draw_string(mh_uigeo_builder_t* ub, float xpos, float ypos, const char* string, mh_color_t color, float scale=1.0f);
float get_smallchar_height();
static unsigned num_indices_for_string(unsigned stringlen) {
	return stringlen * 6;
}
static unsigned numverts_for_string(unsigned stringlen) {
	return stringlen * 4;
}