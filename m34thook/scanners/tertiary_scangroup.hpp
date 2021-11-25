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

using ideventarg_from_entity_ctor_locator = memscanner_t<
	scanbytes<0x4c, 0x8d, 0x4c, 0x24>, skip<2>, 

	scanbytes< 0x8b>, skip<1>,
	scanbytes<0x4C,0x8D,0x05>,
	late_riprel_to_eventdef_m("volumeMemberRemoved")
>;
//can use to call all ingame events.can get event results too. prettttty handy especially if scriptcmdent isnt present,
//although unlike scriptcmdent process_eventargs can get the result
using ideventreceiver_process_eventargs = memscanner_t<
	scanbytes<0xEB>, skip<1>,
	scanbytes<0x48, 0x8b, 0x16, 0x48, 0x8d, 0xd>,
	riprel32_data_equals< 0x49, 0x6E, 0x74, 0x65, 0x72, 0x6E, 0x61, 0x6C, 0x43, 0x61,
	0x6C, 0x6C, 0x45, 0x76, 0x65, 0x6E, 0x74, 0x28, 0x29, 0x20,
	0x64, 0x6F, 0x65, 0x73, 0x6E, 0x27, 0x74, 0x20, 0x68, 0x61,
	0x6E, 0x64, 0x6C, 0x65, 0x20, 0x65, 0x76, 0x65, 0x6E, 0x74,
	0x20, 0x25, 0x73, 0x2E, 0x00>>;

//v1= 1407D1710
//v666 r1 = 140934520
using parmblock_getvalue_locator = memscanner_t<
	scanbytes< 0x48, 0x83, 0xEC>, skip<1>,
	scanbytes<0x8B, 0x82>,
	match_fieldoffs_m(unsigned, "idDeclRenderParm", "parmType"),
	scanbytes<0x83, 0xc0, 0xf9, 0x83, 0xf8, 0x6, 0x77, 0x1b, 0x44, 0x8b, 0x82, 0x88, 0x0, 0x0, 0x0, 0x48, 0x8d, 0x54, 0x24, 0x20, 0xe8>
>;

namespace scanners_phase3 {
	BSCANENT(locate_vertexcolor_offset_entry, &descan::g_idRenderModelGui_VertexColorOffsPtr, scanbehavior_simple<rendermodelgui_vertexcolor_offset_locator>);
	BSCANENT(ideventarg_from_entity_ctor_enty, &descan::g_eventarg_ctor_identityptr, scanbehavior_locate_csrel_preceding< ideventarg_from_entity_ctor_locator>);

	BSCANENT(locate_processeventargs, &descan::g_eventreceiver_processeventargs, scanbehavior_locate_csrel_preceding< ideventreceiver_process_eventargs>);
	BSCANENT(locate_getparmvalue, &descan::g_idParmBlock_GetParmValue, scanbehavior_locate_csrel_after< parmblock_getvalue_locator>);
	static scangroup_t<locate_vertexcolor_offset_entry, ideventarg_from_entity_ctor_enty, locate_processeventargs, locate_getparmvalue> tertiary_scangroup_pass{};

}
