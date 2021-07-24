#include "mh_defs.hpp"

#include "game_exe_interface.hpp"
#include "memscan.hpp"
#include "scanner_core.hpp"
#include <map>
#include <string_view>
#include "snaphakalgo.hpp"
#include <cstdio>
#include "idtypeinfo.hpp"
#include "gameapi.hpp"
#define		DISABLE_PARALLEL_SCANGROUPS

#define		PRINT_SCAN_TIME_TAKEN
//only 8k in my db but lets overestimate to be safe
#define		ASSUMED_MAX_VTBLS		16000

#define		SCANNED_PTR_FEATURE(name, ...)		located_feature_t descan:: name = 0;
#define		SCANNED_UINT_FEATURE(name,...)		 unsigned descan::  name = 0;
#include "xmacro_scanned_features.hpp"
#undef SCANNED_PTR_FEATURE
#undef SCANNED_UINT_FEATURE




#include "scanners/scanners_typeinfo.hpp"
#define		BSCANENT(name, ...)\
	const char* ___name_##name = #name;\
	using  name = blockscan_entry_definition_t<__VA_ARGS__, &___name_##name>
#include "scanners/initial_scangroup.hpp"
#include "scanners/secondary_scangroup.hpp"

#include "scanners/tertiary_scangroup.hpp"
std::map<std::string_view, void*> g_str_to_rrti_type_descr{};

/*
	todo: this does THREE PASSES over the image. it could probably benefit from a bit of vectorization

*/
MH_NOINLINE
static void scan_for_vftbls() {
#ifdef PRINT_SCAN_TIME_TAKEN
	uint64_t tickcount = GetTickCount64();
#endif
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

	find_by_rtti_node_t* tmp_rva_nodes = (find_by_rtti_node_t* )malloc(sizeof(find_by_rtti_node_t) * ASSUMED_MAX_VTBLS);//new find_by_rtti_node_t[200000];
	unsigned current_tmpnode = 0;

	rb_root root_find_by_rtti{};
	

	for (size_t i = 0; i < (g_blamdll.image_size / 8); ++i) {
		if (base[i] == typeinfo_vtable) {

			std::string_view name = reinterpret_cast<char*>(&base[i]) + 16;

			g_str_to_rrti_type_descr[name] = &base[i];

			uint64_t boundaddr = (uint64_t)&base[i];

			smallest_seen_rtti_base_descr_addr = min(smallest_seen_rtti_base_descr_addr, boundaddr);

			largest_seen_rtti_base_descr_addr = max(largest_seen_rtti_base_descr_addr, boundaddr);

			unsigned newrva=boundaddr - (uint64_t)g_blamdll.image_base;

			find_by_rtti_node_t* newnode = &tmp_rva_nodes[current_tmpnode++];
			rb_init_node(&newnode->m_node);
			sh::rb::insert_hint_t hint;
			find_by_rtti_node_t* willbnull = sh::rb::rbnode_find<find_by_rtti_node_t, 0>(&root_find_by_rtti, newrva, [](find_by_rtti_node_t* l, unsigned r) {
					return static_cast<ptrdiff_t>(static_cast<int>(l->m_rvakey - r));
				},&hint
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
				},&unused_hint
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
#ifdef PRINT_SCAN_TIME_TAKEN

	unsigned tickstaken = (unsigned)(GetTickCount64() - tickcount);

	char buffer[128];
	sprintf_s(buffer, "Scanning for vtbls took %d ms", tickstaken);

	MessageBoxA(nullptr, buffer, "Scan time", MB_OK);

#endif

}



volatile bool wait_for_debugger = true;
MH_NOINLINE
void descan::locate_critical_features() {

#ifdef PRINT_SCAN_TIME_TAKEN
	uint64_t tickcount = GetTickCount64();
#endif
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
	if (!g_doom_memorysystem)
		g_doom_memorysystem = locate_csrelative_address_after<locate_memorysystem>();

	g_idcvarsystem = ((void**)g_doom_memorysystem) + 1;
	g_idcmdsystem = ((void**)g_doom_memorysystem) + 2;
	descan::g_idfilesystemlocal = ((void**)g_doom_memorysystem) + 3;


#ifdef PRINT_SCAN_TIME_TAKEN

	unsigned tickstaken = (unsigned)(GetTickCount64() - tickcount);

	char buffer[128];
	sprintf_s(buffer, "Scanning binary took %d ms", tickstaken);

	MessageBoxA(nullptr, buffer, "Scan time", MB_OK);

#endif

	//g_idfilecompressed_getfile = locate_csrelative_address_preceding<locate_idfilecompressed_getfile>();

	//__debugbreak();
}
#define		DRAWSTRING_VFTBL_INDEX 0x11
#define		DRAWSTRETCHPIC_VFTBL_INDEX 0xC
#define		DRAWCHAR_VFTBL_INDEX	0x10
MH_NOINLINE
void obtain_rendermodelgui_stuff() {
	void** dbgmodel = get_class_vtbl(".?AVidDebugModelGui@@");

	descan::g_idRenderModelGui__DrawString = mh_disassembler_t::first_jump_target(dbgmodel[DRAWSTRING_VFTBL_INDEX]);
	descan::g_idRenderModelGui__DrawStretchPic = mh_disassembler_t::first_call_target(dbgmodel[DRAWSTRETCHPIC_VFTBL_INDEX]);
	descan::g_idRenderModelGui__DrawChar = mh_disassembler_t::first_jump_target(dbgmodel[DRAWCHAR_VFTBL_INDEX]);
}

MH_NOINLINE
void descan::run_late_scangroups() {

	std::thread vtbl_scan_thread{scan_for_vftbls};

	//scan_for_vftbls();
	
	scanners_phase2::secondary_scangroup_pass.execute_on_image();
	descan::g_idtypeinfo_findclassinfo = hunt_assumed_func_start_back(descan::g_idtypeinfo_findclassinfo);
	descan::g_idfilecompressed_getfile = hunt_assumed_func_start_back(descan::g_idfilecompressed_getfile);
	g__ZN5idStr4IcmpEPKcS1_ =
		scan_guessed_function_boundaries<scanbehavior_locate_csrel_after<scanner_locate_listofResourceLists_and_idstricmp>>(descan::g_resourcelist_for_classname);

	descan::g_idlib_fatalerror = hunt_assumed_func_start_back(descan::g_idlib_fatalerror);
	descan::g_idlib_error = hunt_assumed_func_start_back(descan::g_idlib_error);
	g_idmapfile_write = hunt_assumed_func_start_back(descan::g_idmapfile_write);
	vtbl_scan_thread.join();

	obtain_rendermodelgui_stuff();
	scanners_phase3::tertiary_scangroup_pass.execute_on_image();
}
	