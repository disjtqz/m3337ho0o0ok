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


static idVec3 g_last_mh_spawn_pos{};
static  char* g_last_mh_spawn_entity = nullptr;

static FILE* g_spawnfile_recording = nullptr;



MH_NOINLINE
static bool mh_spawn_impl(const char* declname, idVec3* position) {

	cs_assert(declname != nullptr);
	cs_assert(position != nullptr);
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

void install_spawning_commands() {
	idCmd::register_command("chrispy", mh_spawn, "chrispy <entitydef> <optional xyz position, uses your look direction as default> - spawns an entity at the position");
	idCmd::register_command("rechrispy", mh_spawn_prev, "rechrispy spawns the previous entitydef used with chrispy at the previous position");
	idCmd::register_command("mh_spawnfile", exec_spawnlist, "<spawn file path> spawns the entities at the positions from the file");
	idCmd::register_command("mh_start_spawnrec", begin_recording_spawns, "<spawn file path> starts recording all chrispy/rechrispy spawns/spawn positions to a file for later exec by mh_spawnfile");
	idCmd::register_command("mh_end_spawnrec", finish_recording_spawns, "No args, closes current spawnfile");
}