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

class mh_editor_local_t : public mh_editor_interface_t {

	std::string m_prevgrab_entity_name;

	void bind_entity_to_player(void* entity);
	bool is_prev_grabbed_entity_valid();
	bool is_valid_entity_for_editing(void* entity);
	void* get_grabbed_entity();

public:
	virtual void grab(void* entity);
	virtual void grab_player_focus();
	virtual void ungrab();

};
void mh_editor_local_t::bind_entity_to_player(void* entity) {

	idEventArg argy;
	argy.make_entity(get_local_player());

	ev_bind(entity, &argy);
}
bool mh_editor_local_t::is_prev_grabbed_entity_valid() {
	if (m_prevgrab_entity_name.length()) {
		void* ubent = find_entity(m_prevgrab_entity_name.c_str());
		return ubent != nullptr;
	}
	return false;
}

bool mh_editor_local_t::is_valid_entity_for_editing(void* entity) {
	if (!entity)
		return false;

	if (entity == get_world())
		return false;
	return true;

}

void* mh_editor_local_t::get_grabbed_entity() {
	if (!is_prev_grabbed_entity_valid())
		return nullptr;
	else {

		return find_entity(m_prevgrab_entity_name.c_str());
	}

}
void mh_editor_local_t::grab(void* entity) {


	//clear old grab 
	if (is_prev_grabbed_entity_valid()) {
		ungrab();
		return;
	}

	void* targ = entity;
	if (!is_valid_entity_for_editing(targ))
		return;
	bind_entity_to_player(targ);
	m_prevgrab_entity_name = get_entity_name(targ);
}

void mh_editor_local_t::grab_player_focus() {

	void* ltarg = get_player_look_target();
	if (!ltarg)
		return;

}
void mh_editor_local_t::ungrab() {
	void* grb = get_grabbed_entity();

	if (grb) {
		ev_unbind(grb);
	}
	m_prevgrab_entity_name.clear();
	
}

static mh_editor_local_t g_local_editor{};



mh_editor_interface_t* get_current_editor() {
	return &g_local_editor;

}