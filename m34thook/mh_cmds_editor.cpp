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
#include <string>
extern
void cmd_mh_spawninfo(idCmdArgs* args);
extern void cmd_mh_ang2mat(idCmdArgs* args);
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

#if defined(MH_ENABLE_SAFE_DECL_RELOAD)

sh::coros::coro_t* g_decl_reload_context = nullptr;
sh::coros::coro_t* g_decl_reload_main_thread_context = nullptr;

void* g_teb_for_decl_reload = nullptr;

char* g_decl_reload_stack_base = nullptr;

const char* g_seh_trigger_msg = nullptr;

static sh::coros::coro_t g_decl_reload_context_storage{};
static sh::coros::coro_t g_decl_reload_main_thread_context_storage{};


static uintptr_t decl_reload_coro_proc(uintptr_t memb) {
	bool got_execption = false;

	__try {
		uintptr_t result = reload_decl((void*)memb);
		g_decl_reload_context->yield_to(g_decl_reload_main_thread_context, result);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		g_decl_reload_context->yield_to(g_decl_reload_main_thread_context, "A SEH exception was raised trying to reload the resource, program may now be in an invalid state. You may have just reloaded at a bad moment and may try again.\n");
	
	}


		

	
	return 0;
}


static bool call_decl_reload(void* resourcelist_member) {
	bool success = false;
	g_decl_reload_context = &g_decl_reload_context_storage;
	g_decl_reload_main_thread_context = &g_decl_reload_main_thread_context_storage;
	g_teb_for_decl_reload = NtCurrentTeb();
	if (!g_decl_reload_stack_base) {
		g_decl_reload_stack_base = (char*) sh::vmem::allocate_rw(DECL_RELOAD_STACK_SIZE);

	}

	

	g_decl_reload_context->coro_set_up_calling_context(&mh_align_down(mh_lea<char>(g_decl_reload_stack_base, (DECL_RELOAD_STACK_SIZE - 8)), 16)[-8], decl_reload_coro_proc, resourcelist_member);

	uintptr_t result = g_decl_reload_main_thread_context->yield_to(g_decl_reload_context);


	if (result < 2ULL) { //is boolean ret
		success = result == 1;
	}
	else { //is definitely error
		idLib::Printf("Failed to reload decl due to error, program may be in a bad state now. error was %s\n", result);
		success = false;
	}
	sh::vmem::discard_contents(g_decl_reload_stack_base, DECL_RELOAD_STACK_SIZE);
	g_decl_reload_context = nullptr;
	g_decl_reload_main_thread_context = nullptr;
	g_teb_for_decl_reload = nullptr;
	return success;
}

#else
static bool call_decl_reload(void* resourcelist_member) {
	return reload_decl(resourcelist_member);
}

#endif



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
	if (!call_decl_reload(memb)) {
		return;
	}

	idLib::Printf("Successfully reloaded %s\n", args->argv[2]);


}
MH_NOINLINE
static mh_editor_interface_t* get_editor_ensure_init() {
	if (!get_current_editor())
		return nullptr;
	if (!get_current_editor()->is_initialized_for_sess()) {
		get_current_editor()->init_for_session();

	}
	return get_current_editor();
}


//ai_suicideCheck_disable 1
static void mh_grab(idCmdArgs* args) {

	auto ed = get_editor_ensure_init();
	if (!ed)
		return;
	get_current_editor()->grab_player_focus();

}


/*
	noclip
	g_stopTime 1
	hands_show 0
	g_showHud 0
	g_doom5Melee_enable 0 
*/
//ai_suicideCheck_disable 1 to prevent ai from dying in out of bounds situations
static void mh_editor(idCmdArgs* args) {



	set_cvar_integer("hands_show", 0);
	set_cvar_integer("g_showHud", 0);
	toggle_idplayer_boolean(get_local_player(), "noClip", true, true);
	get_current_editor()->init_for_session();

}

static void mh_editor_spawn(idCmdArgs* args) {

	if (args->argc < 2)
		return;

	void* edef = locate_resourcelist_member("idDeclEntityDef", args->argv[1]);
	if (!edef) {
		idLib::Printf("No entitydef %s\n", args->argv[1]);
		return;
	}

	auto ed = get_editor_ensure_init();
	if (!ed)
		return;
	ed->editor_spawn_entitydef(edef);
}
static mh_new_fieldcached_t<void,
	yuckystring_m("idGameSystemLocal"),
	yuckystring_m("mapChangeRequest"),
	yuckystring_m("gameSpawnInfo")> g_get_gamespawninfo{};


static void* get_gamespawninfo() {
	return  g_get_gamespawninfo(descan::g_idgamesystemlocal);

}
static mh_new_fieldcached_t<idListVoid,
	yuckystring_m("idGameSystemLocal"),
	yuckystring_m("mapChangeRequest"),
	yuckystring_m("gameSaveFiles")> g_gamesaves{};


static void set_gamespawninfo_bool(const char* spawninfo_var, bool value) {

	auto spawninf = get_gamespawninfo();


	set_classfield_boolean(spawninf, idType::FindClassField("idGameSpawnInfo", spawninfo_var), value);

}
using gspawn_t = yuckystring_m("idGameSpawnInfo");

static mh_new_fieldcached_t< idStrStatic<1024>, gspawn_t, yuckystring_m("checkpointName")> g_chkpt_name{};
static void force_reload_or_reset(bool reset) {
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

	if (reset) {
		//set_gamespawninfo_bool("isCheckpointReload", false);
		set_gamespawninfo_bool("initialMapLoad", true);
		auto chkpt_name = g_chkpt_name(get_gamespawninfo());
		chkpt_name->base.base.len = 0;
		memset(chkpt_name->base.buffer, 0, 1024);
		//*g_set_initialmapload(descan::g_idgamesystemlocal) = true;
		//nope, this just cleared the inventory and made it fists only
		
		/*auto gamesaves = g_gamesaves(descan::g_idgamesystemlocal);
		gamesaves->num = 0;
		*/

	}
}

void cmd_mh_forcereload(idCmdArgs* args)
{
	force_reload_or_reset(false);
}
void cmd_mh_force_reset(idCmdArgs* args)
{
	force_reload_or_reset(true);
}

void cmd_mh_spmap(idCmdArgs* args) {

	if (args->argc < 2) {
		return;
	}

	char sprintbuff[1024];
	sprintf(sprintbuff, "map maps/game/sp/%s/%s", args->argv[1], args->argv[1]);

	idCmd::execute_command_text(sprintbuff);


}

void cmd_set_angle_incr(idCmdArgs* args) {

	if (args->argc < 2)
		return;

	auto ed = get_editor_ensure_init();
	if (!ed)
		return;
	ed->set_angle_increment(atof(args->argv[1]));
}

void cmd_editor_keys(idCmdArgs* args) {

	idLib::Printf("Editor keys - Uparrow = increment grab dist, Downarrow = decrement grab dist, PGUP - increase ent size, PGDN - decrease ent size,\n"
		"INS - duplicate grabbed entity, KP1 / KP3 - manipulate entity roll, KP4 / KP6 - manipulate entity pitch, KP7 / KP8 - manipulate entity yaw,\n"
		"Z = lock Z axis of current selection. KP8/KP2 = cycle entity rendermodel\n"
		"F3 - Save all changes back to the map in overrides."
	);
}
void install_editor_cmds() {

	idCmd::register_command("mh_spawninfo", cmd_mh_spawninfo, "Copy your current position and orientation, formatted as spawnPosition and spawnOrientation to the clipboard");
	idCmd::register_command("mh_dumpmap", cmd_mh_dumpmapfile, "Dump current idMapLocal to a .map file.");
	idCmd::register_command("mh_ang2mat", cmd_mh_ang2mat, "mh_ang2mat pitch yaw roll : converts the pitch, yawand roll values for idAngles to a decl - formatted matrix, copying the result to your clipboard");
	idCmd::register_command("mh_reload_decl", mh_reload_decl, "mh_reload_decl <classname(ex:idDeclWeapon)> <decl path>");

	idCmd::register_command("mh_force_reload", cmd_mh_forcereload, "Force reload current level");
	idCmd::register_command("mh_force_reset", cmd_mh_force_reset, "Force reset current level");
	idCmd::register_command("mh_grab", mh_grab, "Grab an object");
	idCmd::register_command("mh_editor", mh_editor, "Sets up the editor session");
	idCmd::register_command("mh_editor_spawn", mh_editor_spawn, "Spawns an entity, saving it to the map and grabbing it for manipulation");
	idCmd::register_command("mh_spmap", cmd_mh_spmap, "<map name> shortcut for map maps/game/sp/<map name>/<map name>");
	idCmd::register_command("mh_angleincr", cmd_set_angle_incr, "<double> amount to inc/dec by with angle editing");
	idCmd::register_command("mh_editor_keys", cmd_editor_keys, "tells you editor stuff");
}