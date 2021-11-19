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

struct idClientGameMsgDoom2016
{
	__int64 servertime;
	unsigned __int16 peerMask;
	char pad10[2];
	int fromPeer;
};




void cmd_noclip(idCmdArgs* args) {
	auto player = get_local_player();

	if (!player) {
		/*
			please dont change this message
		*/
		idLib::Printf("You don't exist ya dumb fuck. Wait until you spawn.\n");
		return;
	}

	toggle_idplayer_boolean(player, "noClip");
}


void cmd_notarget(idCmdArgs* args) {
	auto player = get_local_player();
	if (!player) {
		idLib::Printf("No player1 instance available.\n");
		return;
	}
	toggle_idplayer_boolean(player, "noTarget");
}
static bool return_1() {

	return 1;
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


	call_as<void>(vtmember_identity_callevent, player1, &getpos_result, position_getter, &g_null_eventargs);



	playerpos = getpos_result->value.v_vec3;

	nonplayer_entities_within_distance_iterate(&playerpos, distance, [&pushamount, &setlinarg](void* current) {
		//random float in range -5 to 5

		unsigned randomact = rand();
		idVec3& velfloat = setlinarg.value.v_vec3;
		velfloat.x = get_rand_float(pushamount);
		velfloat.y = get_rand_float(pushamount);
		velfloat.z = get_rand_float(pushamount);



		if (randomact & 1)
			ev_setLinearVelocity(current, &setlinarg);

		if (randomact & 6) {

			idVec3 scale = get_object_scale(current);

			scale.x += get_rand_float(pushamount / 5.0);
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
static void ScriptCmd(void* entity, idCmdArgs* args, unsigned startidx) {


	idEventDef* evt = idEventDefInterfaceLocal::Singleton()->FindEvent(args->argv[startidx+1]);

	if (!evt) {

		idLib::Printf("No event %s\n", args->argv[startidx+1]);
		return;
	}


	unsigned readvaluepos = startidx+2;

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
			idVec3 v{ (float)atof(getarg()), (float)atof(getarg()), (float)atof(getarg()) };

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

					void* resmember = locate_resourcelist_member_from_idResourceList(reslist, current);
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

		case '4': {//idMD6Anim, i think we skip?
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

	idEventArg result = mh_ScriptCmdEnt(evt, entity, &evargs[0]);

	std::string returnval;

	switch (result.type) {
	case '1':
	case '2':
	case '5':
	case 's': {
		returnval = result.value.s;
		break;
	}
	case 'b':
	case 'i': {
		//too lazy to do enum name lookups right now
		returnval = std::to_string(result.value.i);
		break;
	}
	case 'f': {
		returnval = std::to_string(result.value.f);
		break;
	}
	case 'e': {
		auto ent = result.value.er;

		if (ent) {
			returnval = get_entity_name(ent);
		}
		else {
			returnval = "NULL";
		}
		break;
	}
	case 'a':
	case 'v': {
		idVec3 vv = result.value.v_vec3;

		returnval = std::to_string(vv.x) + " " + std::to_string(vv.y) + " " + std::to_string(vv.z);
		break;
	}
	default: //retuning void? dont copy result to clipboard
		return;
	}

	set_clipboard_data(returnval.c_str());

}

static void mh_ScriptCmdEnt_console(idCmdArgs* args) {

	void* entity = find_entity(args->argv[1]);

	if (!entity) {
		idLib::Printf("No entity %s\n", args->argv[1]);
		return;
	}
	ScriptCmd(entity, args, 1);
}


static void mh_ScriptCmd(idCmdArgs* args) {

	if (true) {
		idLib::Printf("Sorry, this dont work rn it crashes sometimes because of some shit with the debugentity use scriptcmdent instead.\n");
		return;
	}
	void* entity = get_debug_target();
	if (!entity) {
		idLib::Printf("No debug target selected\n");
		return;
	}
	ScriptCmd(entity, args, 0);

}

/*
	default godmode command sends idClientGameMsg_PlayerCommand_God to the server, allowing easy cheating in mp
	our version just sets the godmode flag on the local player, which still works in sp but will have no effect in mp
	so players will just assume id patched out godmode in mp... which they havent done lol. they did patch out the remoteconsole shit but left godmode in :/

	should probably handle the player index arg for godmode so that doesnt give us away
*/
static void sneakybeaky_godmode(idCmdArgs* args) {
	void* player = get_local_player();
	if (!player)
		return;
	//pass silent = true so we can manually print the original godmode format string
	bool new_value = toggle_idplayer_boolean(player, "godMode", false, false, true);
	//this must match godmodes original format string so nobody suspects it was swapped
	const char* mode = "OFF";
	if (new_value) {
		mode = "ON";
	}
	idLib::Printf("godmode %s for player %d\n", mode, 0);
	//the swap worked, already tested
//	idLib::Printf("new godmode\n");
}
void cheats_postinit() {
	/*
		oh come on man, i made it so clear in the comments that this wasnt something i wanted everyone to know and somehow its the first thing you pick out to tell everyone??
		this kind of stuff is most effective in obscurity. 
	*/
	idCmd::swap_command_impl("God", sneakybeaky_godmode);

}
void install_cheat_cmds() {

	idCmd::register_command("mh_killAi", mh_killai, "Kills all living ai");
	idCmd::register_command("mh_removeAi", mh_removeAi, "Removes all living ai");
	idCmd::register_command("mh_randomact", goofy_op, "<distance> <scalar> uses scalar + random values to randomly change the color, scale, and velocity of all entities within distance from player");
	idCmd::register_command("mh_ScriptCmdEnt", mh_ScriptCmdEnt_console, "<entity> <event> <args> ai_ScriptCmdEnt resurrected, now copies the results of the eventcall to your clipboard for chaining commands together");
	idCmd::register_command("mh_ScriptCmd", mh_ScriptCmd, "<event> <args> ai_ScriptCmd resurrected, now copies the results of the eventcall to your clipboard for chaining commands together");

	idCmd::register_command("noclip", cmd_noclip, "Toggle noclip");

	idCmd::register_command("notarget", cmd_notarget, "Toggle notarget");
}