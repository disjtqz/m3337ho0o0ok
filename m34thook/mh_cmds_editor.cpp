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




//ai_suicideCheck_disable 1
static void mh_grab(idCmdArgs* args) {

	get_current_editor()->grab_player_focus();

}


/*
	noclip
	g_stopTime 1
	hands_show 0
	r_skipGuis 1
*/
//ai_suicideCheck_disable 1 to prevent ai from dying in out of bounds situations
static void mh_editor(idCmdArgs* args) {
	cvar_data(cvr_hands_show)->valueInteger = 0;
	cvar_data(cvr_g_showHud)->valueInteger = 0;
	toggle_idplayer_boolean(get_local_player(), "noClip", true, true);
	get_current_editor()->init_for_session();

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

void install_editor_cmds() {

	idCmd::register_command("mh_spawninfo", cmd_mh_spawninfo, "Copy your current position and orientation, formatted as spawnPosition and spawnOrientation to the clipboard");
	idCmd::register_command("mh_dumpmap", cmd_mh_dumpmapfile, "Dump current idMapLocal to a .map file.");
	idCmd::register_command("mh_ang2mat", cmd_mh_ang2mat, "mh_ang2mat pitch yaw roll : converts the pitch, yawand roll values for idAngles to a decl - formatted matrix, copying the result to your clipboard");
	idCmd::register_command("mh_reload_decl", mh_reload_decl, "mh_reload_decl <classname(ex:idDeclWeapon)> <decl path>");

	idCmd::register_command("mh_force_reload", cmd_mh_forcereload, "Force reload current level");

	idCmd::register_command("mh_grab", mh_grab, "Grab an object");
	idCmd::register_command("mh_editor", mh_editor, "Sets up the editor session");

}