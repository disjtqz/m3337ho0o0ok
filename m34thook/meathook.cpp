#include "xbyak/xbyak.h"
#include "mh_defs.hpp"

#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "meathook.h"
#include "cmdsystem.hpp"
#include "idtypeinfo.hpp"
#include "eventdef.hpp"
#include "scanner_core.hpp"
#include "idLib.hpp"
#include "idStr.hpp"
#include "clipboard_helpers.hpp"
#include <vector>
#include "errorhandling_hooks.hpp"
#include "gameapi.hpp"
#include "idmath.hpp"
#include "memscan.hpp"
#include "mh_memmanip_cmds.hpp"
#include "snaphakalgo.hpp"
#include <string.h>
#include "mh_guirender.hpp"
#include "mh_editor_mode.hpp"

extern void cmd_optimize(idCmdArgs* args);
void idlib_dump(idCmdArgs* args) {
	idType::do_idlib_dump();
	return;
}

void event_dump(idCmdArgs* args) {
	DumpEventDefs(args->argc < 2 ? false : args->argv[1][0] != '0');
}
void* __fastcall idFileResourceCompressed__GetFile(__int64 a1);

static bool return_1() {

	return 1;
}
extern
void cmd_mh_spawninfo(idCmdArgs* args);
extern void cmd_mh_ang2mat(idCmdArgs* args);

struct idClientGameMsgDoom2016
{
	__int64 servertime;
	unsigned __int16 peerMask;
	char pad10[2];
	int fromPeer;
};

/*
	now handles noclip and notarget
*/
static void toggle_idplayer_boolean(void* player, const char* property_name) {

	auto headattr = idType::FindClassField("idPlayer", property_name);
	if (!headattr) {
		idLib::Printf("Couldn't locate the %s property on idPlayer, tell chrispy.\n", property_name);
		return;
	}


	int field_value = get_classfield_boolean(player, headattr);

	field_value = !field_value;
	if (field_value) {
		idLib::Printf("Turning %s on.\n", property_name);
	}
	else
		idLib::Printf("Disabling %s\n", property_name);
	set_classfield_boolean(player, headattr, field_value);

}


void cmd_noclip(idCmdArgs* args) {
	auto player = get_local_player();

	if (!player) {
		/*
			please dont change this message
		*/
		idLib::Printf("You don't exist ya dumb fuck. Wait until you spawn.\n");
		return;
	}
#if !defined(MH_ETERNAL_V6)
	reinterpret_cast<void (*)(uintptr_t, void*)>(descan::g_noclip_func)(0, player);
#else
	toggle_idplayer_boolean(player, "noClip");
#endif
}


void cmd_notarget(idCmdArgs* args) {
	auto player = get_local_player();
	if (!player) {
		idLib::Printf("No player1 instance available.\n");
		return;
	}

#if !defined(MH_ETERNAL_V6)
	auto headattr = idType::FindClassField("idPlayer", "noTarget");
	if (!headattr) {
		idLib::Printf("Couldn't locate the noTarget property on idPlayer, tell chrispy.\n");
		return;
	}

	int notarget_value = headattr->get(player);

	notarget_value = !notarget_value;
	if (notarget_value) {
		idLib::Printf("Turning notarget on.\n");
	}
	else
		idLib::Printf("Disabling notarget\n");

	headattr->set(player, notarget_value);
#else
	toggle_idplayer_boolean(player, "noTarget");
#endif
}


/*
struct idEntityHierarchy {
	//Offset 8,	 size 48
	//Parent
	idStr parent;
	//Offset 56,	 size 24
	//Children
	idList < idEntityHierarchy::childInfo_t , TAG_IDLIST , false > childrenInfo;
};
struct idMapEntityEditorData {
*/
static void* find_entity_in_mapfile(const char* entname) {

	void* levelmap = get_level_map();

	auto entities_field = idType::FindClassField("idMapFileLocal", "entities");

	char* entities_ptr = ((char*)levelmap) + entities_field->offset;

	auto entities_list_offset = idType::FindClassField(entities_field->type, "list");

	auto entities_num_offset = idType::FindClassField(entities_field->type, "num");


	void** entities_array = *(void***)(entities_ptr + entities_list_offset->offset);

	int numentities = *(int*)(entities_ptr + entities_num_offset->offset);


	auto editordata_field = idType::FindClassField("idMapEntityLocal", "mapEntityEditorData");

	auto original_name_field = idType::FindClassField("idMapEntityLocal", "model");



	for (int i = 0; i < numentities; ++i) {

		void* current_ent = entities_array[i];

		idStr* originalName = mh_lea<idStr>(current_ent, original_name_field->offset);

		if (originalName->data) {
			idLib::Printf(originalName->data);

			if (!strcmp(originalName->data, entname)) {
				return current_ent;
			}
		}
	}

	return nullptr;

}
#include <string>
void cmd_mh_query_type(idCmdArgs* args) {

	if (args->argc < 2)
		return;
	auto t = idType::FindClassInfo(args->argv[1]);
	std::string clipdata{};
	char scratchbuf[2048];

	if (!t) {

		auto e = idType::FindEnumInfo(args->argv[1]);

		if (!e) {
			idLib::Printf("Type %s was not found.\n", args->argv[1]);
			return;
		}

		for (auto eval = e->values; eval && eval->name && eval->name[0]; eval++) {
			sprintf_s(scratchbuf, "\t%s\n", eval->name);

			clipdata += scratchbuf;

			idLib::Printf(scratchbuf);
		}
		idLib::Printf("Dumped type is a Enum\n");
	}
	else {

		for (auto bleh2 = t->variables; bleh2 && bleh2->name; ++bleh2) {

			sprintf_s(scratchbuf, "\t%s%s %s\n", bleh2->type, bleh2->ops, bleh2->name);

			if (bleh2->comment && bleh2->comment[0]) {
				clipdata += "\n\t//";
				clipdata += bleh2->comment;
				clipdata += "\n";
			}
			clipdata += scratchbuf;
			idLib::Printf(scratchbuf);
		}

		idLib::Printf("\nDumped type is a Class\n");
		if (t->superType && t->superType[0]) {
			idLib::Printf("\nFor more fields this class has, use mh_type on its superclass, %s\n", t->superType);
		}
	}
	set_clipboard_data(clipdata.c_str());
}

void cmd_mh_printentitydef(idCmdArgs* args) {
#if 0
	if (args->argc < 2) {
		idLib::Printf("Not enough args.\n");
		return;
	}

#else
	void* ent = nullptr;

	if (args->argc < 2) {
		idLib::Printf("No entity name provided, using player look target.\n");
		ent = get_player_look_target();
	}
	else {
		idLib::Printf(args->argv[1]);

		ent = find_entity(args->argv[1]);
	}
#endif
	if (!ent) {
		idLib::Printf("Couldn't find entity %s.\n", args->argv[1]);
		return;
	}

	auto entdef_field = idType::FindClassField("idEntity", "entityDef");
	if (!entdef_field) {
		idLib::Printf("Couldn't locate entityDef field!! This is bad.\n");
		return;
	}

	void* edef = *reinterpret_cast<void**>(reinterpret_cast<char*>(ent) + entdef_field->offset);

	if (!edef) {
		idLib::Printf("Entity %s's entitydef is null.\n", args->argv[1]);
		return;

	}

	idStr outstr;
	call_as<void>(descan::g_declentitydef_gettextwithinheritance, edef, &outstr, false);

	idLib::Printf("%s\n", outstr.data);
	set_clipboard_data(outstr.data);


}


void cmd_mh_dumpmapfile(idCmdArgs* args) {
	if (args->argc < 2) {
		idLib::Printf("Not enough args.\n");
		return;
	}
	void* levelmap = get_level_map();
	if (!levelmap) {
		idLib::Printf("GetLevelMap returned null.\n");
		return;
	}

	call_as<void>(descan::g_idmapfile_write, levelmap, args->argv[1]);

}

void cmd_mh_testmapentity(idCmdArgs* args) {

	void* ptr = find_entity_in_mapfile(args->argv[1]);

	idLib::Printf("%p\n", ptr);

}


void cmd_mh_forcereload(idCmdArgs* args)
{
	// Force a reload of the file. idGameSystemLocal->persistMapFile
	//memset(((char*)descan::g_idgamesystemlocal + 0x60), 0, sizeof(void*));
	auto PersistMapFileVar = idType::FindClassField("idGameSystemLocal", "persistMapFile");
	memset(((char*)descan::g_idgamesystemlocal + PersistMapFileVar->offset), 0, sizeof(void*));

	// idGameSystemLocal->idGameSystemLocal::mapChangeRequest_t->requested
	//bool* mapChangeRequest_t_requested = ((bool*)((char*)descan::g_idgamesystemlocal + 0xB8));
	auto MapChangeRequest = idType::FindClassField("idGameSystemLocal", "mapChangeRequest");
	auto Requested = idType::FindClassField(MapChangeRequest->type, "requested");
	bool* mapChangeRequest_t_requested = ((bool*)((char*)descan::g_idgamesystemlocal + MapChangeRequest->offset + Requested->offset));
	*mapChangeRequest_t_requested = true;
}



std::vector<std::string> gActiveEncounterNames;
void cmd_active_encounter(idCmdArgs* args)
{
#ifndef MH_ETERNAL_V6
	// Following chain: idGameSystemLocal->idSaveGameSerializer->idSerializeFunctor<idMetrics>->idMetrics->idPlayerMetrics[0]
	long long* idSaveGameSerializer = ((long long*)((char*)descan::g_idgamesystemlocal + 0xB0));
	long long* idSerializerFunctor = (long long*)(*(idSaveGameSerializer));
	long long* idMetrics = (long long*)(*(idSerializerFunctor + 0x59)); // 0x2C8
	long long* idPlayerMetrics = (long long*)(*(idMetrics + 1)); // 0x8
	long long* EncounterArray = (long long*)(*(idPlayerMetrics + 5)); // 0x28
	long long* Encounter = (long long*)(*(EncounterArray));

	long long EncounterListSize = *(Encounter - 1) - (4 * 0x8); // This seems to work but looks a bit fragile.
	long long* EncounterManager = Encounter + 1; // Skip first encounter which always points to an instance of idGameChallenge_CampaignSinglePlayer.
	size_t Index = 1;
	while ((long long)Index < (EncounterListSize / 8)) {
		// Active
		if (EncounterManager == 0) {
			break;
		}

		long long* Encounter = ((long long*)(*EncounterManager));
		if (Encounter == nullptr) {
			break;
		}

		int Active = *((int*)(Encounter + 0xC5)); // 0x628 -- Fragile offset for determining if an encounter is active.
		if (Active != 0) {
			char* Name = (char*)*(Encounter + 0x9); // 0x48 -- Encounter manager pointer to name string.
			if (strcmp("player1", Name) == 0) {
				break;
			}

			char String[MAX_PATH];
			sprintf_s(String, "Active name %s\n", Name);
			OutputDebugStringA(String);
			idLib::Printf(String);

			// Name
			gActiveEncounterNames.push_back(Name);
		}
		EncounterManager++;
		Index += 1;
	}


#else
	for (void* encounter = find_next_entity_with_class("idEncounterManager"); encounter; encounter = find_next_entity_with_class("idEncounterManager", encounter)) {

		if (get_classfield_int(encounter, "idBloatedEntity", "thinkFlags")) {//Active != 0) {
			const char* name = get_entity_name(encounter);


			char String[MAX_PATH];
			sprintf_s(String, "Active name %s\n", name);
			OutputDebugStringA(String);
			idLib::Printf(String);

			// Name
			gActiveEncounterNames.push_back(name);
		}
	}
#endif
}

std::string gCurrentCheckpointName;
void cmd_current_checkpoint(idCmdArgs* args)
{
	// Following chain: idGameSystemLocal->idGameSpawnInfo->checkpointName
	auto MapInstanceVar = idType::FindClassField("idGameSystemLocal", "mapInstance");
	char* idMapInstanceLocal = ((char*)descan::g_idgamesystemlocal + MapInstanceVar->offset);

	char String[MAX_PATH * 10];
	// (char*)((*((longlong*)((char*)descan::g_idgamesystemlocal + 0x50))) + 0x1108)
	/*  AIIIIIEEEE */
	char* Name = (char*)"NULL";
	if (idMapInstanceLocal != nullptr) {
		//auto GameSpawnNameVar = idType::FindClassField(MapInstanceVar->type, "gameSpawnInfo");
		//auto CheckPointNameVar = idType::FindClassField(GameSpawnNameVar->type, "checkpointName");
		//Name = (char*)(*((unsigned long long*)(idMapInstanceLocal + CheckPointNameVar->offset + GameSpawnNameVar->offset))); // checkpoint string. 0x1108 byte

		//this is bad and could break
		//todo: scan to extract this offset
		Name = ((char*)(*((long long*)(idMapInstanceLocal)) + 0x1108));
	}

	if (Name == nullptr || Name[0] == 0) {
		Name = (char*)"<no checkpoint loaded>";
	}

	sprintf_s(String, "Current checkpoint name %s\n", Name);
	OutputDebugStringA(String);
	idLib::Printf(String);
	gCurrentCheckpointName = Name;
}

/*
	can use mapentity/maplocal typeinfo with getlevelmap to facilitate easier editing

*/


void idc_dump(idCmdArgs* args) {
	idType::generate_idc();
}

void test_persistent_text(idCmdArgs* args) {

	add_persistent_text(400, 400, ~0u, 3.0f, args->argv[1]);

}
static mh_fieldcached_t<idVec3> g_field_hphys_bodylinear{};

void test_physics_op(idCmdArgs* args) {
	void* looktarg = get_player_look_target();

	if (!looktarg)
		return;

	void* ephys = get_entity_physics_obj(looktarg);

	if (!ephys) {
		idLib::Printf("Couldnt get phys obj\n");
		return;
	}

	/*if(!is_subclass_of_rttisig(ephys, RTTISIG(".?AVidHavokPhysics_Base@@"))) {
		idLib::Printf("Not using havokphysics\n");
		return;
	}*/
	idVec3* linvel = g_field_hphys_bodylinear(ephys, "idHavokPhysics_Base", "bodyOrigin");

	linvel->z += atof(args->argv[1]);

}

static idVec3 g_last_mh_spawn_pos{};
static  char* g_last_mh_spawn_entity = nullptr;

static FILE* g_spawnfile_recording = nullptr;



MH_NOINLINE
static bool mh_spawn_impl(const char* declname, idVec3* position) {

	void* our_decl = locate_resourcelist_member("idDeclEntityDef", declname);

	if (!our_decl) {
		idLib::Printf("Failed to find entitydef %s\n", declname);
		return false;
	}
	void* resulting_entity = spawn_entity_from_entitydef(our_decl);


	if (!resulting_entity) {
		idLib::Printf("Somehow spawning %s failed, game returned NULL\n", declname);
		return false;

	}
	const char* ename = get_entity_name(resulting_entity);

	if (!ename) {
		idLib::Printf("Entity name was null?!?!?!?\n");
		return false;
	}


	idLib::Printf("Placing entity %s at %f,%f,%f\n", ename, position->x, position->y, position->z);

	set_entity_position(resulting_entity, position);

	if (g_spawnfile_recording != nullptr) {

		fprintf(g_spawnfile_recording, "%s %f %f %f\n", declname, position->x, position->y, position->z);
	}
	return true;
}

static void begin_recording_spawns(idCmdArgs* args) {
	if (args->argc < 2) {
		idLib::Printf("Expected a file name to record spawns to\n");
		return;
	}
	if (g_spawnfile_recording != nullptr) {

		idLib::Printf("Close your current spawnfile first!\n");
		return;
	}

	fopen_s(&g_spawnfile_recording, args->argv[1], "w");

	if (g_spawnfile_recording == nullptr) {
		idLib::Printf("Failed to open spawnfile %s for writing.\n", args->argv[1]);
		return;

	}

}

static void finish_recording_spawns(idCmdArgs* args) {
	if (g_spawnfile_recording) {
		fflush(g_spawnfile_recording);
		fclose(g_spawnfile_recording);
		g_spawnfile_recording = nullptr;

	}

}

static bool execute_spawnlist_line(FILE* f, unsigned lineno) {

	if (feof(f))
		return false;
	//char linebuffer[1024];
	//decided to dynamically allocate linebuffer so some shithead cant make a spawn list that overruns the buffer 
	char* linebuffer = (char*)malloc(1024);

	sh::memops::smol_memzero(linebuffer, 1024);
	fgets(linebuffer, 1024, f);

	unsigned declend = 0;

	for (; linebuffer[declend] != ' '; ++declend);

	unsigned posstart = declend + 1;
	linebuffer[declend] = 0;
	char* endptr = nullptr;
	double x = strtod(&linebuffer[posstart], &endptr);

	while (*endptr == ' ')++endptr;

	double y = strtod(endptr, &endptr);

	while (*endptr == ' ')++endptr;
	double z = strtod(endptr, &endptr);

	idVec3 posresult{ .x = (float)x, .y = (float)y, .z = (float)z };

	bool spawnresult = mh_spawn_impl(linebuffer, &posresult);
	free(linebuffer);

	if (!spawnresult) {
		idLib::Printf("Encountered error executing spawnlist at line %d, not spawning any more\n", lineno);
		return false;
	}
	return true;
}

static void exec_spawnlist(idCmdArgs* args) {
	if (args->argc < 2) {
		idLib::Printf("No spawnlist file provided...\n");
		return;

	}
	FILE* result = nullptr;
	fopen_s(&result, args->argv[1], "r");
	if (!result) {
		idLib::Printf("Failed to load spawnlist from %s\n", args->argv[1]);
		return;
	}

	unsigned lineno = 0;

	while (execute_spawnlist_line(result, ++lineno /*++ instead of ++ so the lines match up in text editors*/)) {


	}

	fclose(result);
}


void mh_spawn_prev(idCmdArgs* args) {
	if (!g_last_mh_spawn_entity) {
		idLib::Printf("Have not spawned any entities yet, spawn an entity before trying prev\n");
		return;
	}


	mh_spawn_impl(g_last_mh_spawn_entity, &g_last_mh_spawn_pos);
}


void mh_spawn(idCmdArgs* args) {

	if (args->argc < 2) {
		idLib::Printf("You need to supply an entitydef to spawn, foole.\n");
		return;
	}
	idVec3 playertrace{};
	//allow user to supply the position
	if (args->argc >= 5) {

		playertrace.x = atof(args->argv[2]);
		playertrace.y = atof(args->argv[3]);
		playertrace.z = atof(args->argv[4]);
	}
	else {
		get_player_trace_pos(&playertrace);
	}
	if (mh_spawn_impl(args->argv[1], &playertrace)) {
		if (g_last_mh_spawn_entity) {
			free(g_last_mh_spawn_entity);
		}
		g_last_mh_spawn_entity = _strdup(args->argv[1]);
		g_last_mh_spawn_pos = playertrace;
	}

}
static void do_nothing() {

}




static void mh_reload_decl(idCmdArgs* args) {

	if (args->argc < 3) {
		idLib::Printf("mh_reload_decl <classname> <decl path>\n");
		return;
	}





	void* memb = locate_resourcelist_member(args->argv[1], args->argv[2]);

	if (!memb) {
		idLib::Printf("Couldnt locate %s of type %s\n", args->argv[2], args->argv[1]);
		return;
	}
	idLib::Printf("calling decl_read_production_file\n");
	if (!reload_decl(memb)) {
		return;
	}

	idLib::Printf("Successfully reloaded %s\n", args->argv[2]);


}
static mh_fieldcached_t<char*> g_field_resourcelist_type{};
static mh_fieldcached_t<char*> g_field_resourcelist_class{};

static mh_fieldcached_t<void*> g_field_resourcelist_next{};

static void mh_list_resource_lists(idCmdArgs* args) {

	void* listofresourcelists = *((void**)descan::g_listOfResourceLists);


	std::string workbuf{};

#define		RESLIST_TABULATION		"\t\t\t\t\t\t\t\t"


	workbuf += "Resource lists\n\n\tCLASSNAME" RESLIST_TABULATION "TYPENAME\n";



	while (listofresourcelists) {
		const char* type = *g_field_resourcelist_type(listofresourcelists, "idResourceList", "typeName");
		const char* classname = *g_field_resourcelist_class(listofresourcelists, "idResourceList", "className");

		workbuf += "\t";
		workbuf += classname;
		workbuf += RESLIST_TABULATION;
		workbuf += type;
		workbuf += "\n";

		listofresourcelists = *g_field_resourcelist_next(listofresourcelists, "idResourceList", "next");
	}

	set_clipboard_data(workbuf.c_str());

	idLib::Printf(workbuf.c_str());
}

static void mh_list_resourcelist_contents(idCmdArgs* args) {

	if (args->argc < 2) {
		idLib::Printf("Expected a resource classname for the first arg\n");
		return;
	}

	void* reslist = resourcelist_for_classname(args->argv[1]);

	if (!reslist) {
		idLib::Printf("Couldn't get resourcelist for %s!\n", args->argv[1]);
		return;
	}

	void* reslistt = idResourceList_to_resourceList_t(reslist);
	if (!reslistt) {
		idLib::Printf("No resourceList_t for idResourceList?? how??\n");
		return;
	}

	unsigned reslen = resourceList_t_get_length(reslistt);

	std::string buildbuf{};


	for (unsigned i = 0; i < reslen; ++i) {

		void* res = resourceList_t_lookup_index(reslistt, i);
		if (!res)
			continue;
		const char* resname = get_resource_name(res);
		if (!resname)
			continue;

		if (args->argc > 2) {
			if (!sh::string::strstr(resname, args->argv[2])) {
				continue;
			}
		}

		buildbuf += resname;
		buildbuf += "\t\t\t";
		buildbuf += *reinterpret_cast<const char**>(reslistt);
		buildbuf += "\n";

	}

	set_clipboard_data(buildbuf.c_str());

	idLib::Printf(buildbuf.c_str());

}
static void* g_original_renderthread_run = nullptr;
static __int64 testdebugtools(void* x) {

	//call_as<void>(descan::g_renderDebugTools, get_rendersystem());
	return call_as<__int64>(g_original_renderthread_run, x);
}
void meathook_final_init() {
	//compute classinfo object super object deltas for quick inheritance traversal
	idType::compute_classinfo_delta2super();
	idType::init_prop_rva_table();

	descan::run_gamelib_postinit_scangroups();
}

static void meathook_cpuinfo(idCmdArgs* args) {
	char cpuinfo_buffer[4096];
	g_shalgo.m_print_cpu_info(&g_shalgo, cpuinfo_buffer, 4096);

	set_clipboard_data(cpuinfo_buffer);
	idLib::Printf(cpuinfo_buffer);
}

static void image_fill(idCmdArgs* args) {
	if (args->argc < 5) {
		idLib::Printf("nope\n");
		return;
	}
	const char* imagename = args->argv[1];
	unsigned width = atoi(args->argv[2]);
	unsigned height = atoi(args->argv[3]);

	unsigned fillvalue = atoi(args->argv[4]);


	char* tmparray = new char[width * height * 4];

	memset(tmparray, fillvalue, width * height * 4);


	upload_2d_imagedata(imagename, tmparray, width, height);

	delete[] tmparray;


}

static void* nothinfunc = (void*)do_nothing;
static void do_cvar_toggle() {

	/*
		replace offset 16 on vftbl of idcmdsystem with a no-op function.
		the function sets a variable on a pointer in tls, and it is called twice to disable first dev commands, then executing bound dev commands
		swapping it out eliminates this issue, also renders g_command_patch_area useless
	*/
	void* cmdsystem = *(void**)descan::g_idcmdsystem;

	void* cmdsystem_vftbl = *(void**)cmdsystem;


	swap_out_ptrs(reinterpret_cast<char*>(cmdsystem_vftbl) + 16, &nothinfunc, 1, false);


}
static void test_cvar_disable(idCmdArgs* args) {
	do_cvar_toggle();

	
}



void arbitrary_exec(idCmdArgs* args) {

	struct idClientGameMsgDoom2016_RemoteConsole
	{
		idClientGameMsgDoom2016 base;
		idStr stringToSerialize;
		int sourcePeer;
		idClientGameMsgDoom2016_RemoteConsole(const char* cmdexec)
		{


			this->base.fromPeer = -1;
			this->base.peerMask = -1;
			this->base.servertime = (__int64)get_class_vtbl(".?AVidClientGameMsg_RemoteConsole@@");

			this->sourcePeer = 0;
			stringToSerialize = cmdexec;

		}
	};

	idLib::Printf("Execute %s on server\n", args->argv[1]);

	idClientGameMsgDoom2016_RemoteConsole remoteconsolecmd{ args->argv[1] };


	void* patchlocs[3] = { (void*)return_1, (void*)return_1, (void*)return_1 };
	//6, 7, 8
	swap_out_ptrs(&get_class_vtbl(".?AVidClientGameMsg_RemoteConsole@@")[6], patchlocs, 3, true);
	call_as<void>(descan::g_handlereliable, *reinterpret_cast<void**>(descan::g_gamelocal), &remoteconsolecmd);

}
/*
	this i think is a fairly accurate recreation of killai's behavior.
	the code for killai is a little more complex than i expected so i decided to ignore the decompilation
	and write a fresh impl instead
*/
static void mh_killai(idCmdArgs* args) {
	void* next_airef = nullptr;


#if 0

	for (void* airef = find_next_entity_with_class("idAI2"); airef != nullptr; airef = next_airef) {

		next_airef = find_next_entity_with_class("idAI2", airef);

		idEventArg unused;
		mh_ScriptCmdEnt("remove", airef);

	}

#else

	void* player1 = get_local_player();

	void* damaged_decl = locate_resourcelist_member("idDeclDamage", "damage/player/fists");
	if (!damaged_decl) {

		idLib::Printf("Couldn't find damage decl for player fists for killai\n");
		return;
	}
	if (!player1)
		return;

	idEventArg entargs[2];

	entargs[0].make_entity(player1);
	entargs[1].make_decl((struct idDecl*)damaged_decl);
	for (void* airef = find_next_entity_with_class("idAI2"); airef != nullptr; airef = next_airef) {

		next_airef = find_next_entity_with_class("idAI2", airef);

		
		mh_ScriptCmdEnt("kill", airef, entargs);

	}

#endif
}


static bool is_bound_to(void* ent, void* owner) {

	return ((void*)ev_getBindMaster(ent).value.er) == owner || ((void*)ev_getBindParent(ent).value.er) == owner;
}

static double get_rand_float(double range) {
	return (((double)rand() / (double)RAND_MAX) * range) - (range / 2.0);
}


#include <immintrin.h>

static void goofy_op(idCmdArgs* args) {
	
	srand((((uintptr_t)args) * (uintptr_t)NtCurrentTeb()) ^ GetTickCount64());



	float distance = atof(args->argv[1]);
	float pushamount = atof(args->argv[2]);
	
	void* player1 = get_local_player();
	idVec3 playerpos;

	idEventArg setlinarg{};
	setlinarg.type = 'v';
	setlinarg.value.v_vec3.Set(pushamount);

	idEventDef* position_getter = ev_getWorldOrigin.Get();



	void* vtmember_identity_callevent = VTBL_MEMBER(idEntity, VTBLOFFS_CALLEVENT)::Get();


	cs_uninit_t<idEventArg> getpos_result;


	call_as<void>(vtmember_identity_callevent,player1, &getpos_result, position_getter, &g_null_eventargs);



	playerpos = getpos_result->value.v_vec3;

	nonplayer_entities_within_distance_iterate(&playerpos, distance, [&pushamount,&setlinarg](void* current) {
		//random float in range -5 to 5

		unsigned randomact = rand();
		idVec3& velfloat = setlinarg.value.v_vec3;
		velfloat.x = get_rand_float(pushamount);
		velfloat.y = get_rand_float(pushamount);
		velfloat.z = get_rand_float(pushamount);

	

		if(randomact & 1)
			ev_setLinearVelocity(current, &setlinarg);

		if (randomact & 6) {

			idVec3 scale = get_object_scale(current);

			scale.x += get_rand_float(pushamount/5.0);
			if (scale.x <= 0)
				scale.x = -scale.x;

			scale.y += get_rand_float(pushamount / 5.0);
			if (scale.y <= 0)
				scale.y = -scale.y;

			scale.z += get_rand_float(pushamount / 5.0);
			if (scale.z <= 0)
				scale.z = -scale.z;

			set_object_scale(current, scale);
		}
		if (randomact & 24) {
			idVec3 color = get_object_color(current);

			float rdiff = get_rand_float(1.0);
			float gdiff = get_rand_float(1.0);
			float bdiff = get_rand_float(1.0);


			color.x += rdiff;
			color.y += gdiff;
			color.z += bdiff;
			

			auto clampreal = [](float v) {
				if (v < 0.0f) {
					return .0f;
				}
				else if (v > 1.0f) {
					return 1.0f;
				}
				return v;
			};

			color.x = clampreal(color.x);
			color.y = clampreal(color.y);
			color.z = clampreal(color.z);
			set_object_color(current, color);
		}
	});
}

static void mh_removeAi(idCmdArgs* args) {
	void* next_airef = nullptr;

	for (void* airef = find_next_entity_with_class("idAI2"); airef != nullptr; airef = next_airef) {

		next_airef = find_next_entity_with_class("idAI2", airef);
		mh_ScriptCmdEnt("remove", airef);

	}
}

static void mh_grab(idCmdArgs* args) {

	get_current_editor()->grab_player_focus();

}

static void mh_kw(idCmdArgs* args) {

	std::string output = idType::keyword_search((const char**)  & args->argv[0], args->argc);
	set_clipboard_data(output.c_str());
	idLib::Printf("%s", output.c_str());
}

static void mh_editor(idCmdArgs* args) {

	get_current_editor()->init_for_session();

}

static void mh_genproptree(idCmdArgs* args) {

	idType::generate_unique_property_key_tree();
}
static void mh_dumppropidxinfo(idCmdArgs* args) {

	idType::dump_prop_rvas();
}

static void mh_ScriptCmdEnt_console(idCmdArgs* args) {

	void* entity = find_entity(args->argv[1]);

	if (!entity) {
		idLib::Printf("No entity %s\n", args->argv[1]);
		return;
	}

	
	idEventDef* evt = idEventDefInterfaceLocal::Singleton()->FindEvent(args->argv[2]);

	if (!evt) {
		
		idLib::Printf("No event %s\n", args->argv[2]);
		return;
	}


	unsigned readvaluepos = 3;

	auto getarg = [&readvaluepos, args]() -> const char* {

		if (readvaluepos < args->argc) {
			return args->argv[readvaluepos++];
		}
		else {
			return "";
		}
	};
	unsigned evargpos = 0;
	idEventArg evargs[8];
	unsigned i = 0;

	auto getevarg = [&evargpos, &evargs]() ->idEventArg& {
		return evargs[evargpos++];
	};
	for (; evt->formatspec[i]; ++i) {

		switch (evt->formatspec[i]) {
		case 'b': 
			getevarg().make_bool(atoi(getarg()) != 0);
			break;
		case 'f':
			getevarg().make_float(atof(getarg()));
			break;
		case 'e':
			getevarg().make_entity(find_entity(getarg()));
			break;
		case 'i': {
			std::string name;
			if (!evt->GetArgTypeName(i, &name)) {
			just_make_it_int:
				getevarg().make_int(atoi(getarg()));
				break;

			}
			else {
				if (name == "int") {

					//getevarg().make_int(atoi(getarg()));
					//break;
					goto just_make_it_int;

				}
				else {
					const char* inval = getarg();


					long long* newv = idType::get_enum_member_value(name.c_str(), inval);

					int value_to_make = newv ? *newv : atoi(inval);

					getevarg().make_int(value_to_make);
					break;
					

					
					
					

				}
			}
			break;
		}
		case 'v': {
			idVec3 v{(float) atof(getarg()), (float)atof(getarg()), (float)atof(getarg()) };

			getevarg().make_vec3(v);
			break;

		}
		case 'd': {

			std::string name;
			if (!evt->GetArgTypeName(i, &name)) {
				//impossible!
			}
			else {
				while (name[name.length() - 1] == '*' || name[name.length() - 1] == '&') {
					name.erase(name.begin() + name.length() - 1);
				}

				void* reslist = resourcelist_for_classname(name.c_str());

				if (!reslist) {
					idLib::Printf("Uh oh! couldnt find resourcelist %s while parsing arg %s!\n", name.c_str(), getarg());

					getevarg().make_decl(nullptr);
					break;
				}
				else {
					const char* current = getarg();

					void* resmember = locate_resourcelist_member_from_idResourceList(reslist,current );
					if (!resmember) {

						idLib::Printf("oh no, we couldnt find %s in resourcelist %s\n", current, name.c_str());
						getevarg().make_decl(nullptr);
						break;
					}
					else {

						getevarg().make_decl((struct idDecl*)resmember);
						break;
					}
				}
			}
			break;
		}
		/*
			this is the way it is in v1's exe
		*/
		case '1':
		case '2':
		case '5':
		case 's': {

			getevarg().make_string(getarg());
			break;
		}

		case 'a': {
			idAngles v{ (float)atof(getarg()),(float)atof(getarg()),(float)atof(getarg()) };

			getevarg().make_angles(&v);
			break;
		}

		

		default: {

			std::string tname;
			evt->GetArgTypeName(i, &tname);

			idLib::Printf("Unsupported arg type %s\n", tname.c_str());
			break;
		}
		}
	}

	mh_ScriptCmdEnt(evt, entity, &evargs[0]);


}
void meathook_init() {
	
	install_gameapi_hooks();

	void** vtbl_render = get_class_vtbl(".?AVidRenderThread@@");
	g_original_renderthread_run = (void*)testdebugtools;


	swap_out_ptrs(&vtbl_render[1], &g_original_renderthread_run, 1, false);





	//redirect_to_func(descan::g_renderDebugTools, (uintptr_t)descan::g_idRender_PrintStats, true);
	redirect_to_func((void*)idFileResourceCompressed__GetFile, (uintptr_t)/* doomsym<void>(doomoffs::idFileResourceCompressed__GetFile)*/ descan::g_idfilecompressed_getfile, true);
	//g_levelReload = redirect_to_func((void*)LevelReload_CaptureParameters, (uintptr_t)/* doomsym<void>(doomoffs::idFileResourceCompressed__GetFile)*/ descan::g_levelreload, true);
	//g_func992170 = redirect_to_func((void*)LevelReload_PreventUninitializedTick, (uintptr_t)/* doomsym<void>(doomoffs::idFileResourceCompressed__GetFile)*/ descan::g_init_func_rva_992170, true);
	install_error_handling_hooks();
	//no longer needed, see below
	//unsigned patchval_enable_commands = 0;
	do_cvar_toggle();

	//patch_memory(descan::g_command_patch_area, 4, (char*)&patchval_enable_commands);

#ifdef MH_DEV_BUILD
	//for array patch for expanding ai
	//please remove for actual release
	DWORD fuckyu;
	VirtualProtect(g_blamdll.image_base, g_blamdll.image_size, PAGE_EXECUTE_READWRITE, &fuckyu);
#endif
	idCmd::register_command("mh_spawninfo", cmd_mh_spawninfo, "Copy your current position and orientation, formatted as spawnPosition and spawnOrientation to the clipboard");

	idCmd::register_command("noclip", cmd_noclip, "Toggle noclip");

	idCmd::register_command("notarget", cmd_notarget, "Toggle notarget");

	idCmd::register_command("mh_printentitydef", cmd_mh_printentitydef, "Print the entitydef of the entity with the provided name to the console");

	idCmd::register_command("mh_dumpmap", cmd_mh_dumpmapfile, "Dump current idMapLocal to a .map file.");
	idCmd::register_command("mh_type", cmd_mh_query_type, "Dump fields for provided class");
	idCmd::register_command("mh_force_reload", cmd_mh_forcereload, "Force reload current level");
	idCmd::register_command("mh_active_encounter", cmd_active_encounter, "Get the list of active encounter managers");
	idCmd::register_command("mh_current_checkpoint", cmd_current_checkpoint, "Get the current checkpoint name");
	idCmd::register_command("mh_optimize", cmd_optimize, "Patches the engine to make stuff run faster. do not use online, might result in slightly different floating point results (probably not though)");
	idCmd::register_command("mh_ang2mat", cmd_mh_ang2mat, "mh_ang2mat pitch yaw roll : converts the pitch, yawand roll values for idAngles to a decl - formatted matrix, copying the result to your clipboard");
	idCmd::register_command("mh_dumpeventdefs", event_dump, "mh_dumpeventdefs <as enum = 0/1>");
	idCmd::register_command("chrispy", mh_spawn, "chrispy <entitydef> <optional xyz position, uses your look direction as default> - spawns an entity at the position");
	idCmd::register_command("rechrispy", mh_spawn_prev, "rechrispy spawns the previous entitydef used with chrispy at the previous position");
	idCmd::register_command("idlib_dump", idlib_dump, "idlib_dump");

	idCmd::register_command("mh_reload_decl", mh_reload_decl, "mh_reload_decl <classname(ex:idDeclWeapon)> <decl path>");
	idCmd::register_command("mh_list_resource_lists", mh_list_resource_lists, "lists all resource lists by classname/typename, copying the result to the clipboard (the clipboard might not be helpful here)");
	idCmd::register_command("mh_list_resources_of_class", mh_list_resourcelist_contents, "<resourcelist classname> lists all resources in a given list, copying result to clipboard");
	idCmd::register_command("idlib_idc", idc_dump, "Generates a .idc file for ida that defines all structs and enums that have typeinfo for this build of eternal");
	idCmd::register_command("mh_cpuinfo", meathook_cpuinfo, "takes no args, dumps info about your cpu for dev purposes");
	idCmd::register_command("image_fill", image_fill, "test");
	idCmd::register_command("test_cvar_disable", test_cvar_disable, "fff");
	idCmd::register_command("mh_spawnfile", exec_spawnlist, "<spawn file path> spawns the entities at the positions from the file");
	idCmd::register_command("mh_start_spawnrec", begin_recording_spawns, "<spawn file path> starts recording all chrispy/rechrispy spawns/spawn positions to a file for later exec by mh_spawnfile");
	idCmd::register_command("mh_end_spawnrec", finish_recording_spawns, "No args, closes current spawnfile");
	idCmd::register_command("mh_killAi", mh_killai, "Kills all living ai");
	idCmd::register_command("mh_removeAi", mh_removeAi, "Removes all living ai");
	idCmd::register_command("mh_grab", mh_grab, "Grab an object");
	idCmd::register_command("mh_pushinradius", goofy_op, "<distance> <force> pushes all within distance by force");
	idCmd::register_command("mh_kw", mh_kw, "Searches all types, enums, typedefs, their comments, field names, typename, template args for the provided keywords");
	idCmd::register_command("mh_editor", mh_editor, "Sets up the editor session");

	idCmd::register_command("mh_genpropset", mh_genproptree, "Regenerated doom_eternal_properties_generated.cpp/hpp for use in mh builds. not for users");
	idCmd::register_command("mh_dumppropidxinfo", mh_dumppropidxinfo, "Debug command for dumping the corresponding addresses/rvas for property indices");

	idCmd::register_command("mh_ScriptCmdEnt", mh_ScriptCmdEnt_console, "ai_ScriptCmdEnt resurrected");
	install_memmanip_cmds();
	//idCmd::register_command("mh_test_persistent_text", test_persistent_text, "Test persistent onscreen text");
	//idCmd::register_command("mh_phys_test", test_physics_op, "test physics ops");
	// Start rpc server.
}