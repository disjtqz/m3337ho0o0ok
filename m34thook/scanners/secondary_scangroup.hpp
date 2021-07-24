#pragma once

#if !defined(MH_ETERNAL_V6)
//"Couldn't open"
using locate_idmapfilelocal_write_body = memscanner_t<
	scanbytes<0x48, 0x8b, 0xd6, 0x48, 0x8d, 0xd>,
	riprel32_data_equals<  0x43, 0x6F, 0x75, 0x6C, 0x64, 0x6E, 0x27, 0x74, 0x20, 0x6F,
	0x70, 0x65, 0x6E, 0x20, 0x25, 0x73, 0x00>,
	scanbytes<0xE8>,
	match_riprel32_to<&descan::g_idlib_warning>
>;
#else
//v1=140A757A7 
//v6=140BAB957 
using locate_idmapfilelocal_write_body = memscanner_t<
	scanbytes<0x48, 0x8B, 0x75>,
	skip<1>, //skip ptr offset "0x68", might change later, but is same between v1 and v6
	scanbytes<0x48, 0x8D, 0x0D>,
	riprel32_data_equals<  0x77, 0x72, 0x69, 0x74, 0x69, 0x6E, 0x67, 0x20, 0x25, 0x73,
	0x2E, 0x2E, 0x2E, 0x0A, 0x00> //writing %s...\n
>;
#endif


#if 0
using locate_idfilecompressed_getfile = memscanner_t<
	scanbytes<0x48, 0x8b, 0xc8, 0x48, 0x8b, 0xd8, 0x48, 0x8b, 0x10, 0xff, 0x52, 0x68, 0x48, 0x8B, 0xD>,
	match_riprel32_to<&descan::g_doom_memorysystem>,
	scanbytes<0x45, 0x33, 0xc9,
	0xc7, 0x44, 0x24, 0x28, 0x2, 0x0, 0x0, 0x0, 0x48, 0x8b, 0xf0, 0xc7, 0x44, 0x24, 0x20, 0x10, 0x0, 0x0, 0x0, 0x4c, 0x8d, 0x70, 0x1, 0x4c, 0x8b, 0x11, 0x45>>;
//	0x8d, 0x41, 0xd, 0x49, 0x8b, 0xd6, 0x41, 0xff, 0x52, 0x20, 0x4c, 0x8b, 0xb, 0x4c, 0x8b, 0xc6, 0x48, 0x8b, 0xd0, 0x48, 0x8b, 0xcb, 0x48, 0x8b, 0xf8, 0x41, 0xff, 0x51, 0x30, 0x44, 0x88, 0x2c, 0x37, 0x49, 0x8b, 0xd4>>;
#else
#if !defined(MH_ETERNAL_V6)
using locate_idfilecompressed_getfile = memscanner_t<
	scanbytes<0x48, 0x8B, 0x53, 0x08, 0x48, 0x8D, 0x0D>,
	riprel32_data_equals<  0x69, 0x64, 0x46, 0x69, 0x6C, 0x65, 0x52, 0x65, 0x73, 0x6F,
	0x75, 0x72, 0x63, 0x65, 0x43, 0x6F, 0x6D, 0x70, 0x72, 0x65,
	0x73, 0x73, 0x65, 0x64, 0x3A, 0x3A, 0x47, 0x65, 0x74, 0x46,
	0x69, 0x6C, 0x65, 0x3A, 0x20, 0x43, 0x6F, 0x75, 0x6C, 0x64,
	0x20, 0x6E, 0x6F, 0x74, 0x20, 0x64, 0x65, 0x63, 0x6F, 0x6D,
	0x70, 0x72, 0x65, 0x73, 0x73, 0x20, 0x74, 0x65, 0x78, 0x74,
	0x20, 0x66, 0x6F, 0x72, 0x20, 0x25, 0x73, 0x00>>;
#else
using locate_idfilecompressed_getfile = memscanner_t<
	scanbytes<0x48, 0x8D, 0x0D>,
	riprel32_data_equals<  0x69, 0x64, 0x46, 0x69, 0x6C, 0x65, 0x52, 0x65, 0x73, 0x6F,
	0x75, 0x72, 0x63, 0x65, 0x43, 0x6F, 0x6D, 0x70, 0x72, 0x65,
	0x73, 0x73, 0x65, 0x64, 0x3A, 0x3A, 0x47, 0x65, 0x74, 0x46,
	0x69, 0x6C, 0x65, 0x3A, 0x20, 0x43, 0x6F, 0x75, 0x6C, 0x64,
	0x20, 0x6E, 0x6F, 0x74, 0x20, 0x64, 0x65, 0x63, 0x6F, 0x6D,
	0x70, 0x72, 0x65, 0x73, 0x73, 0x20, 0x74, 0x65, 0x78, 0x74,
	0x20, 0x66, 0x6F, 0x72, 0x20, 0x25, 0x73, 0x00>>;
#endif
#endif

using locate_idlib_fatalerror_body = memscanner_t<
	scanbytes<0x4c, 0x8b, 0x44, 0x24, 0x30, 0x4c, 0x8d, 0x4c, 0x24, 0x38, 0xba, 0x8, 0x0, 0x0, 0x0, 0x8d, 0x4a, 0xf9, 0xe8>,
	match_riprel32_to<&descan::g_idlib_vprintf>
>;

using locate_idlib_error_body = memscanner_t<
	scanbytes<0x4c, 0x8b, 0x44, 0x24, 0x30, 0x4c, 0x8d, 0x4c, 0x24, 0x38, 0xba, 0x7, 0x0, 0x0, 0x0, 0x8d, 0x4a, 0xfa, 0xe8>,
	match_riprel32_to<&descan::g_idlib_vprintf>
>;
//v1=140B7B9AF
using locate_find_next_ent_with_class = memscanner_t<
	/*scanbytes<0x49, 0x8D, 0x8E >,
	skip<4>,
	scanbytes<0xE8>,
	skip<4>,*/

	scanbytes<0x4C, 0x8D, 0x05>,
	riprel32_data_equals<  0x69, 0x64, 0x49, 0x6E, 0x66, 0x6F, 0x4C, 0x65, 0x76, 0x65,
	0x6C, 0x46, 0x61, 0x64, 0x65, 0x49, 0x6E, 0x00>, //idInfoLevelFadeIn
	scanbytes<0x48, 0x8b, 0xd7, 0x48, 0x8b, 0xce, 0xe8>>;
namespace scanners_phase2 {
#if !defined(MH_ETERNAL_V6)
	BSCANENT(entry_phase2_locate_findclassinfo, &descan::g_idtypeinfo_findclassinfo, scanbehavior_locate_func<scanner_locate_findclassinfo>);
#else
	BSCANENT(entry_phase2_locate_findclassinfo, &descan::g_idtypeinfo_findclassinfo, scanbehavior_locate_csrel_after<scanner_locate_findclassinfo>);
#endif
	BSCANENT(entry_phase2_locate_idfilecompressed_getfile, &descan::g_idfilecompressed_getfile, scanbehavior_locate_func<locate_idfilecompressed_getfile>);

	BSCANENT(entry_phase2_locate_resourcelist_for_classname, &descan::g_resourcelist_for_classname, scanbehavior_locate_csrel_after<scanner_locate_resourcelist_for_classname>);

	//BSCANENT(entry_phase2_locate_cmd_patch_area, &descan::g_command_patch_area, scanbehavior_simple<locate_cmd_patch_area>);

	BSCANENT(entry_phase2_locate_deserialize_from_json, &descan::g_deserialize_type_from_json, scanbehavior_locate_func<scanner_locate_deserialize_typetojson>);
	//BSCANENT(setentitystate_locator, &descan::g_declentitydef_setentitystate, scanbehavior_locate_func<locate_setentitystate>);
	BSCANENT(locate_idmapfilelocal_write_body_entry, &descan::g_idmapfile_write, scanbehavior_locate_func<locate_idmapfilelocal_write_body>);

	BSCANENT(locate_findenuminfo_entry, &descan::g_idtypeinfo_findenuminfo, scanbehavior_locate_csrel_after<scanner_locate_findenuminfo>);
	BSCANENT(locate_idlib_fatalerror_entry, &descan::g_idlib_fatalerror, scanbehavior_locate_func<locate_idlib_fatalerror_body>);


	BSCANENT(locate_idlib_error_entry, &descan::g_idlib_error, scanbehavior_locate_func<locate_idlib_error_body>);
	BSCANENT(find_next_ent_with_class_locator_entry, &descan::g_find_next_entity_with_class, scanbehavior_locate_csrel_after<locate_find_next_ent_with_class>);

	//9 scanners
#define		PAR_SCANGROUP_P2_1 				entry_phase2_locate_findclassinfo, entry_phase2_locate_idfilecompressed_getfile
#define		PAR_SCANGROUP_P2_2				entry_phase2_locate_resourcelist_for_classname
#define		PAR_SCANGROUP_P2_3				locate_idmapfilelocal_write_body_entry,locate_findenuminfo_entry
#define		PAR_SCANGROUP_P2_4				locate_idlib_fatalerror_entry,locate_idlib_error_entry,find_next_ent_with_class_locator_entry
#ifdef DISABLE_PARALLEL_SCANGROUPS

	using secondary_scangroup_type = scangroup_t<
		PAR_SCANGROUP_P2_1,
		PAR_SCANGROUP_P2_2,
		PAR_SCANGROUP_P2_3,
		PAR_SCANGROUP_P2_4
	>;

#else
	using secondary_scangroup_type = parallel_scangroup_group_t<scangroup_t<PAR_SCANGROUP_P2_1>, scangroup_t<PAR_SCANGROUP_P2_2>, scangroup_t<PAR_SCANGROUP_P2_3>, scangroup_t<PAR_SCANGROUP_P2_4> >;

#endif
	static secondary_scangroup_type secondary_scangroup_pass{};
}