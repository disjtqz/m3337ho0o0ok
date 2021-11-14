#pragma once

MH_TRIVIAL_ABI
struct idStaticModelPtr {
	void* m_ptr;

	static idStaticModelPtr CreateNew();


	void Destroy();

	void Write(const char* output_path, bool skip_compression = true);



	void Setup_Singlesurface_hack(unsigned num_verts, unsigned num_indexes, idDrawVert** out_verts, unsigned short** out_indexes);

	void make_standard_tris();
	void Finalize_geo();

};