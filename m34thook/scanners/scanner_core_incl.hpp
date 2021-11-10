#pragma once


#define		DISABLE_PARALLEL_SCANGROUPS
//#define		DISABLE_PHASE2_PARALLEL_SCANGROUPS


//#define		PRINT_SCAN_TIME_TAKEN
//only 8k in my db but lets overestimate to be safe
#define		ASSUMED_MAX_VTBLS		16000




#include "scanners/scanners_typeinfo.hpp"
#define		BSCANENT(name, ...)\
	static const char* ___name_##name = #name;\
	using  name = blockscan_entry_definition_t<__VA_ARGS__, &___name_##name>
#include "scanners/initial_scangroup.hpp"
#include "scanners/secondary_scangroup.hpp"

#include "scanners/tertiary_scangroup.hpp"


struct scantimer_t {
	uint64_t start_time;
#ifdef PRINT_SCAN_TIME_TAKEN
	void start() {
		start_time = GetTickCount64();
	}

	void end(const char* name) {

		unsigned tickstaken = (unsigned)(GetTickCount64() - start_time);

		char buffer[128];
		sprintf_s(buffer, "%s took %d ms", name, tickstaken);

		MessageBoxA(nullptr, buffer, "Scan time", MB_OK);

	}
#else
	void start() {
	}
	void end(const char* name) {

	}
#endif
};

/*
	todo: this does THREE PASSES over the image. it could probably benefit from a bit of vectorization

*/
MH_NOINLINE
static void scan_for_vftbls() {

	scantimer_t vtbl_time{};
	vtbl_time.start();
	uint64_t* base = (uint64_t*)g_blamdll.image_base;

	std::uint64_t typeinfo_vtable = *reinterpret_cast<uint64_t*>(((char*)descan::g_rtti_typeinfo_string) - 16);


	uint64_t smallest_seen_rtti_base_descr_addr = (uint64_t)(g_blamdll.image_base + g_blamdll.image_size);

	uint64_t largest_seen_rtti_base_descr_addr = (uint64_t)(g_blamdll.image_base);

	std::map<unsigned, std::string_view> find_by_rtti_locator{};

	struct find_by_rtti_node_t {
		rb_node m_node;
		unsigned m_rvakey;

		std::string_view m_text;


	};

	find_by_rtti_node_t* tmp_rva_nodes = (find_by_rtti_node_t*)malloc(sizeof(find_by_rtti_node_t) * ASSUMED_MAX_VTBLS);//new find_by_rtti_node_t[200000];
	unsigned current_tmpnode = 0;

	rb_root root_find_by_rtti{};


	for (size_t i = 0; i < (g_blamdll.image_size / 8); ++i) {
		if (base[i] == typeinfo_vtable) {

			std::string_view name = reinterpret_cast<char*>(&base[i]) + 16;

			g_str_to_rrti_type_descr[name] = &base[i];

			uint64_t boundaddr = (uint64_t)&base[i];

			smallest_seen_rtti_base_descr_addr = min(smallest_seen_rtti_base_descr_addr, boundaddr);

			largest_seen_rtti_base_descr_addr = max(largest_seen_rtti_base_descr_addr, boundaddr);

			unsigned newrva = boundaddr - (uint64_t)g_blamdll.image_base;

			find_by_rtti_node_t* newnode = &tmp_rva_nodes[current_tmpnode++];
			rb_init_node(&newnode->m_node);
			sh::rb::insert_hint_t hint;
			find_by_rtti_node_t* willbnull = sh::rb::rbnode_find<find_by_rtti_node_t, 0>(&root_find_by_rtti, newrva, [](find_by_rtti_node_t* l, unsigned r) {
				return static_cast<ptrdiff_t>(static_cast<int>(l->m_rvakey - r));
				}, &hint
				);
			newnode->m_rvakey = newrva;
			newnode->m_text = name;


			mh_assume_m(!willbnull);
			hint.insert(&newnode->m_node, &root_find_by_rtti);


			//find_by_rtti_locator[boundaddr - (uint64_t)g_blamdll.image_base] = name;

		}
	}



	unsigned smallest_rva = smallest_seen_rtti_base_descr_addr - (uint64_t)g_blamdll.image_base;
	unsigned largest_rva = largest_seen_rtti_base_descr_addr - (uint64_t)g_blamdll.image_base;

	uint32_t* rvabase = (uint32_t*)g_blamdll.image_base;

	uint64_t smallest_objlocator_addr = (uint64_t)(g_blamdll.image_base + g_blamdll.image_size);

	uint64_t largest_objlocator_addr = (uint64_t)(g_blamdll.image_base);
	std::map<void*, std::string_view> locator_to_name{};

	for (size_t i = 0; i < (g_blamdll.image_size / 4); ++i) {

		uint32_t currva = rvabase[i];

		if (currva <= largest_rva && currva >= smallest_rva) {
			sh::rb::insert_hint_t unused_hint;

			find_by_rtti_node_t* foundnode = sh::rb::rbnode_find<find_by_rtti_node_t, 0>(&root_find_by_rtti, currva, [](find_by_rtti_node_t* l, unsigned r) {
				return static_cast<ptrdiff_t>(static_cast<int>(l->m_rvakey - r));
				}, &unused_hint
				);
			//auto iter = find_by_rtti_locator.find(currva);

			if (!foundnode) {
				continue;
			}


			void* baserva = rvabase[i + 2] + g_blamdll.image_base;


			void* basecheck = &rvabase[i - 3];

			if (baserva != basecheck) {
				continue;

			}

			//we now have the complete object locator
			//g_str_to_rrti_type_descr[iter->second] = baserva;
			locator_to_name[baserva] = foundnode->m_text;
			smallest_objlocator_addr = min((uint64_t)baserva, smallest_objlocator_addr);
			largest_objlocator_addr = max((uint64_t)baserva, largest_objlocator_addr);

		}
	}
	g_str_to_rrti_type_descr.clear();

	//final pass to actually get the vtables

	for (size_t i = 0; i < (g_blamdll.image_size / 8); ++i) {

		uint64_t currva = base[i];

		if (currva <= largest_objlocator_addr && currva >= smallest_objlocator_addr) {


			auto namefor = locator_to_name.find((void*)currva);

			if (namefor == locator_to_name.end())
				continue;

			g_str_to_rrti_type_descr[namefor->second] = &base[i + 1];
		}
	}
	free(tmp_rva_nodes);

	//vtbl_time.end("Scanning for vtbls");


}


static volatile bool wait_for_debugger = true;

static std::atomic_bool g_did_locate_p1 = false;
MH_NOINLINE
static void descan_locate_critical_features() {
	if (g_did_locate_p1.exchange(true) == true)
		return;

	scantimer_t p1_timer{};
	p1_timer.start();

	sh::vmem::prefetch_for_seq_access(g_blamdll.image_base, g_blamdll.image_size);

	FILE* DebugFileStream = nullptr;
	if (fopen_s(&DebugFileStream, "wait_for_debugger.txt", "rb") == 0) {
		fclose(DebugFileStream);
		//i forgot that i had the file created and was getting frustrated, thinking i had broken something.
		//added this so that doesnt happen again
		MessageBoxA(nullptr, "Found wait_for_debugger.txt, looping forever!", "Debugger", 0);

		while (wait_for_debugger);
	}

	initial_scanners::initial_scangroup.execute_on_image();



	//	descan::g_declentitydef_gettextwithinheritance = hunt_assumed_func_start_back(descan::g_declentitydef_gettextwithinheritance);
#if !defined(MH_ETERNAL_V6)
	descan::g_noclip_func = hunt_assumed_func_start_back(descan::g_noclip_func);

	descan::g_maplocal_getlevelmap = hunt_assumed_func_start_back(descan::g_maplocal_getlevelmap);
#endif
	//fallback
	if (!descan::g_doom_memorysystem)
		descan::g_doom_memorysystem = locate_csrelative_address_after<locate_memorysystem>();

	descan::g_idcvarsystem = ((void**)descan::g_doom_memorysystem) + 1;
	descan::g_idcmdsystem = ((void**)descan::g_doom_memorysystem) + 2;
	descan::g_idfilesystemlocal = ((void**)descan::g_doom_memorysystem) + 3;

	p1_timer.end("Scangroup 1");


	//g_idfilecompressed_getfile = locate_csrelative_address_preceding<locate_idfilecompressed_getfile>();

	//__debugbreak();
	}
//v1=0140C6CF58
//allowing constant offsets because they're the same in v1 and v6, and probably wont change
using scanner_extract_getvirtualdims = memscanner_t<
	scanbytes<0x48, 0x8b, 0x4b, 0x40, 0xe8>,
	skip_and_capture_rva<&descan::g_idRenderModelGui_GetVirtualWidth>,
	scanbytes<0xf3, 0xf, 0x2c, 0xc8, 0x8b, 0xc7, 0x39, 0x0d>,
	skip<4>,
	scanbytes<0xf, 0x4c, 0xd>,
	skip<4>,
	scanbytes<0x85, 0xc9, 0xf, 0x4f, 0xc1, 0x89, 0x5>,
	skip<4>,
	scanbytes<0x48, 0x8b, 0x4b, 0x40, 0xe8>
>; //last 4 = rva to getvirtualheight
#define		DRAWSTRING_VFTBL_INDEX 0x11
#define		DRAWSTRETCHPIC_VFTBL_INDEX 0xC
#define		DRAWCHAR_VFTBL_INDEX	0x10
MH_NOINLINE
static void obtain_rendermodelgui_stuff() {
	void** dbgmodel = get_class_vtbl(".?AVidDebugModelGui@@");
#if 0
	descan::g_idRenderModelGui__DrawString = mh_disassembler_t::first_jump_target(dbgmodel[DRAWSTRING_VFTBL_INDEX]);
	descan::g_idRenderModelGui__DrawStretchPic = mh_disassembler_t::first_call_target(dbgmodel[DRAWSTRETCHPIC_VFTBL_INDEX]);
	descan::g_idRenderModelGui__DrawStretchPicVec4Version = mh_disassembler_t::first_call_target(descan::g_idRenderModelGui__DrawStretchPic);
	descan::g_idRenderModelGui__DrawChar = mh_disassembler_t::first_jump_target(dbgmodel[DRAWCHAR_VFTBL_INDEX]);

#else

	descan::g_idRenderModelGui__DrawStretchPic = mh_disassembler_t::nth_call_target(descan::g_idRenderModelGui__DrawFilled, 2);
	descan::g_idRenderModelGui__DrawStretchPicVec4Version = mh_disassembler_t::first_call_target(descan::g_idRenderModelGui__DrawStretchPic);
#endif
	void** aasgui = get_class_vtbl(".?AVidAASGUI@@");
	//renderdebuggui function
	void* start = aasgui[3];

	void* end = mh_disassembler_t::after_first_return(start);


	descan::g_idRenderModelGui_GetVirtualHeight = run_range_scanner<scanbehavior_locate_csrel_after<scanner_extract_getvirtualdims>>(start, end);
	/*
		todo: scan for getvirtualwidth/height
	*/

}


using scanner_extract_staticmodel_ctor = memscanner_t<
	scanbytes<0xb9>,
	match_sizeof_type_m(unsigned, "idStaticModel"),
	scanbytes<0xe8>,
	match_riprel32_to<&descan::g_doom_operator_new>,
	scanbytes<0x48, 0x85, 0xc0, 0x74>,
	skip<1>,
	scanbytes<0x48, 0x8b, 0xc8, 0xe8>>;//last four is riprel to staticmodel

//v1=140646F2F
using scanner_extract_maketexturedcube = memscanner_t<
	scanbytes<0xf3, 0xf, 0x10, 0x15>,

	riprel32_data_equals<  0xBF, 0x0E, 0x1C, 0x3E>,
	scanbytes<0x48, 0x8b, 0xcb, 0xf3, 0xf, 0x10, 0xd>,
	riprel32_data_equals<  0xBF, 0x0E, 0x1C, 0xBE>,

	scanbytes<0xe8>>;

//same in v1 and v6
#define		IDCONSOLELOCAL_DRAW_OFFSET		(0x70 / 8)
//v1 = 014043CCA4
using scanner_get_smallchar_height = memscanner_t<
	scanbytes<0x48, 0x8b, 0xd>,
	skip_and_capture_rva<&descan::g_renderSystem2>,
	scanbytes<0x48, 0x8b, 0x1>,
	scanbytes<0xff, 0x90>,
	skip<2>,
	scanbytes<0x0, 0x0, 0xf3, 0xf, 0x10, 0x5>,
	skip_and_capture_rva<&descan::g_SMALLCHAR_HEIGHT>,
	scanbytes<0xf3, 0xf, 0x59>,
	masked_eq_byte<0xF0, 0xC0>, //c7 (xmm7) in v6, c6 (xmm6) in v1
	scanbytes<0xc6, 0xc7, 0x45>,
	skip<1>,
	scanbytes<0x0, 0x0, 0x20, 0x41>>;

static void run_scanners_over_idconsolelocal_draw() {
	/*
		weird issue with idconsolelocal - theres a duplicate typeinfo object that claims to be it, but in reality
		it points to idprintlistener or something, so instead we use the actual console instance, hence why this needs to run after init
	*/
	void* consolelocal_draw = reinterpret_cast<void***>(get_console())[0][IDCONSOLELOCAL_DRAW_OFFSET];

	//get_class_vtbl(".?AVidConsoleLocal@@")[IDCONSOLELOCAL_DRAW_OFFSET];


//void* end = mh_disassembler_t::after_first_return(consolelocal_draw);

	void* end = hunt_assumed_func_start_forward(reinterpret_cast<unsigned char*>(consolelocal_draw) + 1);

	MH_UNLIKELY_IF(!end) {
		mh_error_message("Failed to find end of idConsoleLocal::Draw!");
		return;
	}

	run_range_scanner<scanbehavior_identity<&descan::g_SMALLCHAR_HEIGHT, scanner_get_smallchar_height>>(consolelocal_draw, end);
	MH_LIKELY_IF(descan::g_SMALLCHAR_HEIGHT) {
		/*
			smallchar_width has been located 4 bytes before smallchar height
			since at least Doom 2016, although in TNO it comes after
		*/
		descan::g_SMALLCHAR_WIDTH = mh_lea<void>(descan::g_SMALLCHAR_HEIGHT, -4LL);
	}
}
//v1=140644932
using scanner_extract_many_from_maketexturedcube = memscanner_t<
	scanbytes<0xf, 0x28, 0xf2, 0xf, 0x28, 0xf9, 0xe8>,
	skip_and_capture_rva<&descan::g_idStaticModel_FreeSurfaces>,
	scanbytes<0xba>,
	skip<1>, //skip memtag arg
	scanbytes<0x0, 0x0, 0x0, 0xb9>,
	skip_and_capture_4byte_value<&descan::g_idTriangles_sizeof>,
	scanbytes<0xe8>,
	match_riprel32_to<&descan::g_doom_operator_new>,
	scanbytes<0x48, 0x85, 0xc0, 0x74>,
	skip<1>,

	scanbytes<0x48, 0x8b, 0xc8, 0xe8>,
	skip_and_capture_rva<&descan::g_idTriangles_ctor>

>;

using scanner_find_allocstatictrisurfverts = memscanner_t<
	scanbytes<0xba, 0x18, 0x0, 0x0, 0x0, 0x48, 0x8b, 0xcb,
	0xe8>, skip_and_capture_rva<&descan::g_idTriangles_AllocStaticTriSurfVerts>,
	scanbytes<
	0xba, 0x24, 0x0, 0x0, 0x0, 0x48, 0x8b, 0xcb,
	0xe8>,
	skip_and_capture_rva<&descan::g_idTriangles_AllocStaticTriSurfIndexes>
>;

using scanner_find_updatebuffers = memscanner_t<
	scanbytes<0xe8>,
	match_riprel32_to<&descan::g_idRenderModelStatic_FinishStaticModel>,
	scanbytes<0x45, 0x33, 0xc0, 0x33, 0xd2, 0x48, 0x8b, 0xcf, 0xe8>,
	skip_and_capture_rva<&descan::g_idStaticModel_UpdateBuffers>
>;
static void run_scanners_over_staticmodelmanager_init() {

	void** vftbl_for = get_class_vtbl(".?AVidStaticModelManagerLocal@@");

	void* initfunc = vftbl_for[0];

	void* endpos = vftbl_for[0x30 / 8]; //in all versions, this vtbl function is placed directly after init, so we can use it to save processing time on finding the func end

	if (endpos < initfunc) {

		endpos = mh_disassembler_t::after_first_return(initfunc);

		if (!endpos) {
			mh_error_message("Failed to run idStaticModelManagerLocal::Init scanners!");
			return;
		}
	}
	descan::g_idStaticModel_ctor = run_range_scanner<scanbehavior_locate_csrel_after<scanner_extract_staticmodel_ctor>>(initfunc, endpos);
	descan::g_idStaticModel_MakeTexturedCube = run_range_scanner<scanbehavior_locate_csrel_after<scanner_extract_maketexturedcube>>(initfunc, endpos);

	void* start = descan::g_idStaticModel_MakeTexturedCube;

	void* end = hunt_assumed_func_start_forward(mh_lea<char>(start, 1));

	void* end_of_first_part = run_range_scanner<scanbehavior_simple<scanner_extract_many_from_maketexturedcube>>(start, end);

	void* cubeface_next = run_range_scanner<scanbehavior_simple<scanner_find_allocstatictrisurfverts>>(end_of_first_part, end);

	descan::g_idTriangles_AddCubeFace = mh_disassembler_t::first_call_target(cubeface_next);

	void* nextjmp_is_freecpu = run_range_scanner<scanbehavior_simple<scanner_find_updatebuffers>>(cubeface_next, end);

	descan::g_idStaticModel_FreeCPUData = mh_disassembler_t::first_jump_target(nextjmp_is_freecpu);

	mh_disassembler_t::extract_call_targets(descan::g_idStaticModel_FreeSurfaces, descan::g_idListVoid_SetSize, descan::g_idTriangles_dctor, descan::g_doom_operator_delete);
}

static void extract_extra_functions_from_mapsavereference() {

	auto gamelocal_vtbl = get_class_vtbl(".?AVidMapInstanceLocal@@");

	void* mapsaveref = gamelocal_vtbl[descan::g_vftbl_offset_MapSaveReference / 8];

	void* expected_end = mh_disassembler_t::after_first_return(mapsaveref);


	using locate_lea_of_world = memscanner_t<scanbytes< 0x48, 0x8D, 0x15>, riprel32_data_equals< 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x00>>;

	void* start_extracting_loc = run_range_scanner<scanbehavior_simple<locate_lea_of_world>>(mapsaveref, expected_end);


	mh_disassembler_t::extract_call_targets(start_extracting_loc, descan::g_idResource_SetName, descan::g_idDecl_SetText, descan::g_idDecl_Reparse, descan::g_idMapEntity_SetEntityDef);
}

MH_NOINLINE
static void descan_run_late_scangroups() {
	scantimer_t late_scangroup_timer{};
	late_scangroup_timer.start();

	std::thread vtbl_scan_thread{ scan_for_vftbls };
	

	//scan_for_vftbls();
	scanners_phase2::secondary_scangroup_pass.execute_on_image();

	unsigned levelmapoffs = load4_no_cache(&descan::g_vftbl_offset_getlevelmap);

	descan::g_idtypeinfo_findclassinfo = hunt_assumed_func_start_back(descan::g_idtypeinfo_findclassinfo);
	descan::g_idfilecompressed_getfile = hunt_assumed_func_start_back(descan::g_idfilecompressed_getfile);
	descan::g__ZN5idStr4IcmpEPKcS1_ =
		scan_guessed_function_boundaries<scanbehavior_locate_csrel_after<scanner_locate_listofResourceLists_and_idstricmp>>(descan::g_resourcelist_for_classname);

	descan::g_idlib_fatalerror = hunt_assumed_func_start_back(descan::g_idlib_fatalerror);
	descan::g_idlib_error = hunt_assumed_func_start_back(descan::g_idlib_error);
	descan::g_idmapfile_write = hunt_assumed_func_start_back(descan::g_idmapfile_write);
	
	//g_idMapFile_WriteInternal

	using locate_writeinternal_call = memscanner_t< scanbytes<0x48, 0x8b, 0xd6, 0x48, 0x8d, 0xd>,
		riprel32_data_equals< 0x43, 0x6F, 0x75, 0x6C, 0x64, 0x6E, 0x27, 0x74, 0x20, 0x6F,
		0x70, 0x65, 0x6E, 0x20, 0x25, 0x73, 0x00>,

		scanbytes<0xe8>, skip_and_capture_rva<&descan::g_idLib_Warning>,
		scanbytes<0xEB>, //0x53 in all but whatever
		skip<1>>;



	void* end_write = mh_disassembler_t::after_first_return(descan::g_idmapfile_write);

	void* nextcall_is_writeinternal = run_range_scanner<scanbehavior_simple<locate_writeinternal_call>>(descan::g_idmapfile_write, end_write);


	descan::g_idMapFile_WriteInternal = mh_disassembler_t::first_call_target(nextcall_is_writeinternal);


	vtbl_scan_thread.join();
	//lol the mapsavereference stuff needs access to the vtables, i had put it before and was confused over why it worked under a debugger but not otherwise
	if (levelmapoffs) {

		/*
		SCANNED_UINT_FEATURE(g_vftbl_offset_MapFindEntity_idEntity)
SCANNED_UINT_FEATURE(g_vftbl_offset_MapFindEntity_ccharptr)
SCANNED_UINT_FEATURE(g_vftbl_offset_MapSaveReference)
SCANNED_UINT_FEATURE(g_vftbl_offset_MapRemoveEntity)
		*/
		descan::g_vftbl_offset_MapFindEntity_idEntity = levelmapoffs - 16;
		descan::g_vftbl_offset_MapFindEntity_ccharptr = levelmapoffs - 24;
		descan::g_vftbl_offset_MapSaveReference = levelmapoffs - 32;
		descan::g_vftbl_offset_MapRemoveEntity = levelmapoffs - 48;
		extract_extra_functions_from_mapsavereference();
	}
	obtain_rendermodelgui_stuff();

	late_scangroup_timer.end("P2 Scanners+Vtbl Threaded");


}

//v1 = 00000001403F3DA7

using locate_writestaticbmodel = memscanner_t<
	scanbytes<0xe8>, skip_and_capture_rva<&descan::g_idStr_AppendPath>,
	scanbytes<0x48, 0x8D, 0x15>,
	riprel32_data_equals< 0x62, 0x6D, 0x6F, 0x64, 0x65, 0x6C, 0x00>, //bmodel
	scanbytes<0x48, 0x8D, 0x4D>,
	skip<1>, //0x70 sp offset in all v
	scanbytes<0xe8>,
	skip_and_capture_rva<&descan::g_idStr_SetFileExtension>,
	scanbytes<0x48, 0x8b, 0x55>,
	skip<1>,
	scanbytes<0x48, 0x8d, 0x8d>,
	skip<2>,
	scanbytes<0x0, 0x0, 0x45, 0x33, 0xc9, 0x45, 0x33, 0xc0, 0xe8>>;


MH_NOINLINE
static void run_scanners_over_ExportDuplicatCollisionGeometryModel() {
		
	void* exportimpl = idCmd::find_command_by_name("exportDuplicateCollisionGeometryModel");
	if (!exportimpl) {
		mh_error_message("Failed to find command exportDuplicateCollisionGeometryModel! id may have removed it!");
		return;

	}
	void* retpos = mh_disassembler_t::after_first_return(exportimpl);

	descan::g_idStaticModel_WriteStaticBModel = run_range_scanner<scanbehavior_locate_csrel_after< locate_writestaticbmodel>>(exportimpl, retpos);

}
/*
	at this point we have access to things like typeinfotools
*/
MH_NOINLINE
static void descan_run_gamelib_postinit_scangroups() {
	scantimer_t postinit_timer{};
	postinit_timer.start();

	run_scanners_over_staticmodelmanager_init();
	run_scanners_over_idconsolelocal_draw();
	//run_scanners_over_ExportDuplicatCollisionGeometryModel();
	scanners_phase3::tertiary_scangroup_pass.execute_on_image();

	postinit_timer.end("Postinit scan");
}