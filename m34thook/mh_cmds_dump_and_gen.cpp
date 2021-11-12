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


void idlib_dump(idCmdArgs* args) {
	idType::do_idlib_dump();
	return;
}

void event_dump(idCmdArgs* args) {
	DumpEventDefs(args->argc < 2 ? false : args->argv[1][0] != '0');
}




static void mh_genproptree(idCmdArgs* args) {

	idType::generate_unique_property_key_tree();
}
static void mh_dumppropidxinfo(idCmdArgs* args) {

	idType::dump_prop_rvas();
}

static void mh_gencvarset(idCmdArgs* args) {

	idCVar::generate_name_table();
}

void idc_dump(idCmdArgs* args) {
	idType::generate_idc();
}

void install_dumpngen_cmds() {


	idCmd::register_command("mh_dumpeventdefs", event_dump, "mh_dumpeventdefs <as enum = 0/1>");

	idCmd::register_command("idlib_dump", idlib_dump, "idlib_dump");
	idCmd::register_command("mh_genpropset", mh_genproptree, "Regenerated doom_eternal_properties_generated.cpp/hpp for use in mh builds. not for users");

	idCmd::register_command("mh_gencvarset", mh_gencvarset, "Regenerate doom_eternal_cvars_generated.cpp/hpp for mh build");
	idCmd::register_command("mh_dumppropidxinfo", mh_dumppropidxinfo, "Debug command for dumping the corresponding addresses/rvas for property indices");
	idCmd::register_command("idlib_idc", idc_dump, "Generates a .idc file for ida that defines all structs and enums that have typeinfo for this build of eternal");

}
