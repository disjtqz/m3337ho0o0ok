#pragma once


/*
	idVec3 pos;
	idVec2 lightmapUV;
	unsigned __int8 normal[4];
	unsigned __int8 tangent[4];
	unsigned __int8 color[4];
	idVec2 materialUV;
	unsigned __int16 materials[2];
	unsigned __int16 unused[2];
*/
/*
	no normals, no lightmap, no tangent, just unsigned short positions,colors and uvs

	Z might not be necessary... really only need it if i want to do culling of surfaces?

	the idea here is that we can have one thread that generates data in our vertex format, which can be kept around for multiple frames (until it gets invalidated by something)
	and then on another thread we handle allocating the gui surfaces and unpacking our vertex format to idDrawVert and streaming it (in the same loop preferably, we can do a couple of shuffles and ors and then stream)
	if we want to do a full fledged editor ingame, we're going to be rendering a lot of gui shit, so this approach might be best. it definitely is a bit more complicated than simple immediate rendering though



	no, this is not a good idea i think. because we mix integer and float types, we are forced to incur a stall when unpacking to stream
*/
struct mh_vertex2d_t {
	unsigned short m_position[3];
	
	unsigned short m_uv[2];
	unsigned int m_color;
};


struct mh_vertex2d_packed_t {
	unsigned short m_position[2];//4 bytes
	float m_uv[2]; //8 bytes = 12
	unsigned int m_color; //4 bytes = 16
};

/*
	this is a more sensible approach that lets us unpack without mixing pipeline operations
*/
struct mh_uisurface_soa_t {
	unsigned short* m_positions_x;
	unsigned short* m_positions_y;
	unsigned short* m_positions_z;
	float* m_uv_s;
	float* m_uv_t;
	unsigned* m_color;



};


struct mh_uigeo_builder_t {
	void* m_mtr;
	mh_ui_vector_t<idDrawVert> verts;
	mh_ui_vector_t<unsigned short> indices;

	//todo: cache white material!!
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
	mh_uigeo_builder_t* ub,
	float centerx,
	float centery,
	float radius,
	unsigned numsegm,
	mh_color_t color);

void make_circle_quarter(mh_uigeo_builder_t* ub,
	float centerx,
	float centery,
	float radius,
	unsigned numsegm,
	unsigned quadrant,
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