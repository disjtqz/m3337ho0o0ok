#pragma once

//most complicated scanner so far imo
// checked against ms store, v1
//v1= 14046AD57 
//use scanbehavior_simple, we get a pointer to an int
using rendermodelgui_vertexcolor_offset_locator = memscanner_t<
	scanbytes<0xE8>,//might as well grab setviewport while we're here
	skip_and_capture_rva<&descan::g_idRenderModelGui_SetViewPort>,
	scanbytes<0x48,0x8b,0x47>, skip_and_push<1>, scanbytes<0x48,0x8d,0x4c,0x24>,
	skip_and_push<1>,
	scanbytes<0x0F,0x28,0x05>,
	riprel32_data_equals<  0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 
  0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F>,
	scanbytes<0xf,0x11,0x44,0x24>,
	cmp_backref<1, 1>,//0x60
	scanbytes<0x48,0xc7,0x80>,
	skip<2>,
	scanbytes<0x0,0x0,0x2c,0x0,0x0,0x0, 
	0x48,0x8b,0x5f>,
	cmp_backref<2, 1>,//0x30
	scanbytes<0xe8>,
	match_riprel32_to<&descan::g_idcolor_packcolor>,
	scanbytes<0x89,0x83> //last four is the offset to vertexcolor
	//0xeb,0xea,0xea,0xff>
>;

namespace scanners_phase3 {
	BSCANENT(locate_vertexcolor_offset_entry, &descan::g_idRenderModelGui_VertexColorOffsPtr, scanbehavior_simple<rendermodelgui_vertexcolor_offset_locator>);

	static scangroup_t<locate_vertexcolor_offset_entry> tertiary_scangroup_pass{};

}
