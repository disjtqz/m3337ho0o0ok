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
#include "mhgui.hpp"
#include "mh_inputsys.hpp"
#include "mh_mainloop.hpp"
#include "mh_editor_mode.hpp"
#include "rapiddecl.hpp"
#include "fs_hooks.hpp"
#include <filesystem>


#include "mh_editor_math.hpp"

#define		editor_assert_m(...)		cs_assert(__VA_ARGS__)
class mh_editor_local_t;

#define		NEW_EDITOR_GRAB_MODE

/*
	little bit of background: back when things were a little more primitive, i was mulling over how i wanted to do decl parsing/unparsing. my idea was to just acquire references
	to the json parse/unparse functions for id's typeinfo system. ScorpOrXor on the other hand just modified rapidjson's parser to accept decls which i still think is an absolutely brilliant solution,
	and so i ditched the json scanner idea for it.

	i did change the source dir name to rapiddecl and the namespace name in the rapidjson include file to rapiddecl though, just in case i actually need json parsing and need to throw
	rapidjson in here too
*/

namespace decl_parsing {

	/*
		do it this way (the way scorp does in his code)

	*/
	static void parse(rapiddecl::Document& input_doc, const char* input_txt) {

		input_doc.Parse<rapiddecl::kParseCommentsFlag | rapiddecl::kParseTrailingCommasFlag | rapiddecl::kParseNanAndInfFlag | rapiddecl::kIgnoreWhiteSpacing>(input_txt);
	}

	static std::string unparse(rapiddecl::Document& doc) {

		rapiddecl::StringBuffer tmp{};

		rapiddecl::PrettyWriter< rapiddecl::StringBuffer,
			rapiddecl::UTF8<char>,
			rapiddecl::UTF8<char>,
			rapiddecl::CrtAllocator,
			rapiddecl::kWriteValidateEncodingFlag |
			rapiddecl::kWriteNanAndInfFlag |
			rapiddecl::kWriteKeepNumForNullArray> pretty{ tmp };

		doc.Accept(pretty, 0);


		std::string result = tmp.GetString();
		return result;
	}

	template< typename TCurr, typename... Ts>


	MH_SEMIPURE
		static inline
		rapiddecl::Value* nested_get_value1(rapiddecl::Value* MH_NOESCAPE input, TCurr current, Ts... propnames) {
		dmemb_iter_t iter = input->FindMember(current);


		if (iter == input->MemberEnd()) {
			return nullptr;
		}

		if constexpr (sizeof...(Ts) == 0) {
			return &iter->value;
		}
		else {
			return nested_get_value1(&iter->value, propnames...);
		}
	}
	template<typename... Ts>
	MH_FLATTEN
		MH_SEMIPURE
		static inline rapiddecl::Value* nested_get_value(rapiddecl::Value* MH_NOESCAPE input, Ts... propnames) {
		return nested_get_value1(input, current, propnames...);
	}



}

//ahahah this fucking works
//offset to it hasnt changed since v1
//this is incredible! it actually uses the in-memory values for adding the entity
//to the map, so we can manipulate them using events and typeinfo queries
static void update_map_entity(void* entity, bool add) {
	auto gloc = get_gamelocal();
	call_virtual<void>(gloc, 0xf8 / 8, entity, add, nullptr);
}

#define	TRACE_EDITOR(fmt, ...)		idLib::Printf(fmt "\n", __VA_ARGS__)
class mh_editor_input_handler_t :public mh_input_handler_t {
	mh_editor_local_t* m_editor;

public:
	virtual event_consumed_e receive_char_event(unsigned gotchar);
	virtual event_consumed_e receive_keydown_event(const char* keyname);
	virtual event_consumed_e receive_keyup_event(const char* keyname);
	virtual void receive_mouse_move_event(unsigned x, unsigned y);
	mh_editor_input_handler_t() {}

	void set_editor(mh_editor_local_t* eloc) {

		m_editor = eloc;
	}
};





static mh_new_fieldcached_t < void*, YS("idEntity"), YS("entityDef")>g_entitydef_identity{};
//static mh_fieldcached_t<void*> g_entitydef_identity;

static mh_new_fieldcached_t<void*, YS("idMapEntityLocal"), YS("entityDef")> g_mapentity_entitydef{};
static mh_new_fieldcached_t<const char*, YS("idDecl"), YS("textSource")> g_iddecl_textsource{};
static mh_new_fieldcached_t<idVec3, YS("idBloatedEntity"), YS("renderModelInfo"), YS("scale")> g_field_renderscale{};
static mh_new_fieldcached_t<idVec3, YS("idBloatedEntity"), YS("clipModelInfo"), YS("size")> g_clipmodel_size{};
static mh_new_fieldcached_t<idVec3, YS("idBloatedEntity"), YS("renderModelInfo"), YS("scale")> g_new_field_renderscale{};


CACHED_EVENTDEF(getMins);
CACHED_EVENTDEF(getMaxs);

static editor_bounds_t get_entity_bbox(void* ent) {

	idEventArg result;
	mh_ScriptCmdEnt_idEntity(ev_getMins.Get(), ent, nullptr, &result);
	editor_bounds_t bnds;

	bnds.b[0] = result.value.v_vec3;
	mh_ScriptCmdEnt_idEntity(ev_getMaxs.Get(), ent, nullptr, &result);
	bnds.b[1] = result.value.v_vec3;

	return bnds;
}

CACHED_EVENTDEF(setSpawnPosition);
CACHED_EVENTDEF(setOrigin);
CACHED_EVENTDEF(getAngles);
CACHED_EVENTDEF(setAngles);

static editor_angles_t editor_get_angles(void* entity) {
	editor_assert_m(entity != nullptr);

	idEventArg result = ev_getAngles(entity);
	//idEventArg result;
//	mh_ScriptCmdEnt_idEntity(ev_getAngles.Get(), entity, nullptr, &result);

	editor_angles_t res;
	res = result.value.a_angles;

	return res;
}

static mh_new_fieldcached_t<idMat3, YS("idEntity"), YS("spawnOrientation")>  g_entity_spawnorientation;

static void editor_set_angles(void* entity, editor_angles_t ang) {


	editor_mat3_t mat = ang.to_mat3();

	idMat3 idmat = mat.to_id();

	*g_entity_spawnorientation(entity) = idmat;

	idEventArg onearg[2];
	onearg[1].make_angles(ang.to_id());
	idVec3 tmp;
	get_entity_position(entity, &tmp);
	onearg[0].make_vec3(tmp);
	ev_teleport(entity, onearg);



}
static void set_spawn_pos(void* ent, editor_vec3_t pos) {
	cs_uninit_t<idEventArg> thearg;
	thearg->make_vec3(pos.operator idVec3());
	ev_setSpawnPosition(ent, &thearg);
}

static void editor_set_pos(void* ent, editor_vec3_t pos) {
	set_spawn_pos(ent, pos);
	idVec3 tmp = static_cast<idVec3>(pos);
	set_entity_position(ent, &tmp);

	idEventArg fuknut;
	fuknut.make_vec3(pos);
	ev_setOrigin(ent, &fuknut);

}
static editor_vec3_t get_player_feet_position() {

	void* plyer = get_local_player();
	if (!plyer)
		return editor_vec3_t{};

	idVec3 tmppos;
	get_entity_position(plyer, &tmppos);

	idVec3 mins;
	cs_uninit_t<idEventArg> result_mins;
	mh_ScriptCmdEnt_idEntity(ev_getMins.Get(), plyer, nullptr, &result_mins);

	tmppos.z = mins.z;

	return tmppos;
}
CACHED_EVENTDEF(getModel);
CACHED_EVENTDEF(setModel);


static mh_new_fieldcached_t<idAtomicString, YS("idBloatedEntity"), YS("renderModelInfo"), YS("model")> g_bloatedentity_mdl_name{};


static void set_entity_model_name(void* entity, const char* name) {
	idEventArg mdlnamearg;
	mdlnamearg.make_string(name);
	ev_setModel(entity, &mdlnamearg);

	g_bloatedentity_mdl_name(entity)->set(name);
}



/*
	cycle the model used for this entity to the next index in the staticmodel list, or the previous
*/
static void cycle_staticmodel(void* entity, int delta) {

	const char* mdl = ev_getModel(entity).value.s;

	void* rlist = resourcelist_for_classname("idStaticModel");
	void* rlistt = idResourceList_to_resourceList_t(rlist);
	unsigned int numres = resourceList_t_get_length(rlistt);

	unsigned i;

	for (i = 0; i < numres; ++i) {
		void* rsr = resourceList_t_lookup_index(rlistt, i);

		const char* rsptr = get_resource_name(rsr);

		if (sh::string::streq(rsptr, mdl)) {
			break;
		}
	}

	int deltaposition = i + delta;

	if (deltaposition < 0) {
		deltaposition += (int)numres;
	}
	if (deltaposition == numres) {
		deltaposition = 0;

	}

	set_entity_model_name(entity, get_resource_name(resourceList_t_lookup_index(rlistt, deltaposition)));

}

class edited_entity_t {
	
	mh_entityref_t m_ent;

public:
	virtual void* self() {
		return m_ent.get();
	}
	virtual void set_model(const char* name) {
		void* ent = self();
		if(ent)
			set_entity_model_name(ent, name);
	}
	virtual void cycle_rendermodel(int direction) {
		void* ent = self();
		if (ent) {
			cycle_staticmodel(ent, direction);
		}
	}
	virtual void set_position(editor_vec3_t pos) {
		void* ent = self();
		if (ent) {
			editor_set_pos(ent, pos);
		}
	}
	virtual void set_angles(editor_angles_t ang) {
		void* ent = self();
		if (ent) {
			editor_set_angles(ent, ang);
		}
	}
	//updates entity in mapfile
	virtual void update_map() {

		void* ent = self();
		if (ent) {
			update_map_entity(ent, false);
		}
	}
	virtual void add_to_map() {

		void* ent = self();
		if (ent) {
			update_map_entity(ent, true);
		}
	}


};

class mh_editor_local_t : public mh_editor_interface_t {
	//std::string m_prevgrab_entity_name;

	mh_entityref_t m_grabbed_entity;
	double m_grab_distance;
	mh_editor_input_handler_t m_input_forwarder;
	mh_dom_t* m_gui;
	mh_ui_ele_t* m_current_entity_name_label;

	mh_ui_ele_t* m_entitydef_source;
	rapiddecl::Document m_current_parsed_entitydef;
	editor_vec3_t m_grabbed_object_original_pos;
	editor_vec3_t m_grabbed_object_prior_pos;

	char m_temp_pathbuf[OVERRIDE_PATHBUF_SIZE];

	//the value by which entities scale down/up each time the scale keys are used
	double m_current_size_increment;
	//amount each time with uparrow/downarrow we adjust the offset of the grabbed object
	double m_object_distance_increment;
	double m_angle_increment;
	struct {
		unsigned m_initialized : 1;
		unsigned m_locked_z : 1;

	};

	void bind_entity_to_player(void* entity);
	bool is_prev_grabbed_entity_valid();
	bool is_valid_entity_for_editing(void* entity);
	void* get_grabbed_entity();
	void init_dom();

	void tick_prerun();

	void tick_postrun();

	idVec3 get_entity_scale();
	void set_entity_scale(idVec3 newscale);

	idVec3 get_entity_clipscale();
	void set_entity_clipscale(idVec3 newscale);

	void* get_mapentity();
	void* get_entitydef();

	const char* get_entitydef_text();

	void set_entitydef_text(const char* s);


	const char* get_current_map_file_output_path();
	void write_current_map_to_overrides();
	static void prerun_forwarder(void* ud) {
		reinterpret_cast<mh_editor_local_t*>(ud)->tick_prerun();
	}

	static void postrun_forwarder(void* ud) {
		reinterpret_cast<mh_editor_local_t*>(ud)->tick_postrun();
	}

	void* get_local_player_focus_trace();


	editor_vec3_t get_player_look_direction();
	//normal of the surface that the line trace from the player hit
	editor_vec3_t get_player_look_hit_normal();
	//gets a point along the ray the players look direction casts
	editor_vec3_t get_point_in_player_direction(double distance);
public:
	mh_editor_local_t() {
		m_grab_distance = 10.0;
		m_grabbed_entity.clear();
		m_current_size_increment = 0.1;
		m_object_distance_increment = 0.1;
		m_initialized = 0;
		m_locked_z = 0;
		m_angle_increment = 0.5;
	}
	virtual void grab(void* entity);
	virtual void grab_player_focus();
	virtual void ungrab();
	virtual void set_angle_increment(double val) {
		m_angle_increment = val;
	}
	virtual void editor_spawn_entitydef(void* entitydef);
	event_consumed_e receive_char_event(unsigned gotchar);
	event_consumed_e receive_keydown_event(const char* keyname);
	event_consumed_e receive_keyup_event(const char* keyname);
	void receive_mouse_move_event(unsigned x, unsigned y);
	virtual bool is_initialized_for_sess() {

		return m_initialized;
	}

	virtual void init_for_session();
};

static mh_new_fieldcached_t<void, YS("idPlayer"), YS("focusTracker"), YS("focusTrace")> g_idPlayer_focusTracker_focusTrace;

static mh_new_fieldcached_t<idVec3, YS("idFocusTrace"), YS("close")> g_focustrace_close;

static mh_new_fieldcached_t < idVec3, YS("idFocusTrace"), YS("tr"), YS("c"), YS("normal")> g_focustrace_tr_contact_normal{};


void* mh_editor_local_t::get_local_player_focus_trace() {
	return g_idPlayer_focusTracker_focusTrace(get_local_player());
}

editor_vec3_t mh_editor_local_t::get_player_look_direction() {
	editor_vec3_t endpoint = *g_focustrace_close(get_local_player_focus_trace());
	idVec3 tmp_playerpos;
	get_entity_position(get_local_player(), &tmp_playerpos);
	editor_vec3_t playerpos = tmp_playerpos;


	return (endpoint - playerpos).normalized();
}

editor_vec3_t mh_editor_local_t::get_player_look_hit_normal() {
	return *g_focustrace_tr_contact_normal(get_local_player_focus_trace());

}
//returns idMapEntityLocal corresponding to grabbed entity
void* mh_editor_local_t::get_mapentity() {
	void* res = call_virtual<void*>(get_gamelocal(), descan::g_vftbl_offset_MapFindEntity_idEntity / 8, get_gamelocal(), get_grabbed_entity());

	TRACE_EDITOR("get_mapentity returned %p", res);

	return res;

}

void* mh_editor_local_t::get_entitydef() {
	/*void* mapentity = get_mapentity();
	if (!mapentity)
		return nullptr;
		*/
	void* grabbed = get_grabbed_entity();
	if (!grabbed)
		return nullptr;
	void* edef_grabbed = *g_entitydef_identity(grabbed);
	if (!edef_grabbed) {
		return nullptr;
	}

	//const char* txtsource = *g_iddecl_textsource(edef_grabbed, "idDecl", "textSource");
	//void* ed = *g_mapentity_entitydef(mapentity, "idMapEntityLocal", "entityDef");
	void* ed = edef_grabbed;

	TRACE_EDITOR("get_entitydef returned %p", ed);
	return ed;
}

const char* mh_editor_local_t::get_entitydef_text() {
	void* edef = get_entitydef();
	if (!edef)
		return nullptr;

	const char* src = *g_iddecl_textsource(edef);

	TRACE_EDITOR("get_entitydef_text is returning\n%s", src);
	return src;

}

void mh_editor_local_t::set_entitydef_text(const char* s) {

	void* edef = get_entitydef();
	if (!edef)
		return;
	call_as<void>(descan::g_idDecl_SetText, edef, s, 0u);
	call_as<void>(descan::g_idDecl_Reparse, edef, false);//pass false for fromDisk
}

idVec3 mh_editor_local_t::get_entity_scale() {

	return *g_field_renderscale(get_grabbed_entity());


}
void mh_editor_local_t::set_entity_scale(idVec3 newscale) {
	cs_uninit_array_t<idEventArg, 6> args{};
	//or use setRenderModelScale, but iirc that doesnt work as well as lerp
	args[0].make_float(newscale.x);
	args[1].make_float(newscale.y);
	args[2].make_float(newscale.z);
	args[3].make_int(0);
	args[4].make_int(0);
	args[5].make_int(0);
	ev_lerpRenderScale(get_grabbed_entity(), &args[0]);

}


idVec3 mh_editor_local_t::get_entity_clipscale() {
	return *g_clipmodel_size(get_grabbed_entity());
}
void mh_editor_local_t::set_entity_clipscale(idVec3 newscale) {
	*g_clipmodel_size(get_grabbed_entity()) = newscale;
}
void mh_editor_local_t::init_dom() {
	if (m_gui)
		return;

	mh_input::install_input_handler(&m_input_forwarder);
	m_gui = mh_gui::new_named_dom("editor_gui");
	m_current_entity_name_label = m_gui->alloc_e2d("current_selected_ent_name", 0.65, 0.10, 0.20, 0.10);
	m_current_entity_name_label->init_text("", 1.0, colorCyan);

	m_entitydef_source = m_gui->alloc_e2d("entitydef_source", 0.1, 0.1, 0.8, 0.8);

	m_entitydef_source->init_text("", 0.75, colorGreen);

	mh_mainloop::add_preframe_callback(mh_editor_local_t::prerun_forwarder, (void*)this);

	mh_mainloop::add_postframe_callback(mh_editor_local_t::postrun_forwarder, (void*)this);

}
editor_vec3_t mh_editor_local_t::get_point_in_player_direction(double distance) {
	editor_vec3_t transldir = get_player_look_direction();

	transldir = transldir * distance;
	idVec3 ppos;
	get_entity_position(get_local_player(), &ppos);

	transldir = transldir + ppos;
	return transldir;

}
void mh_editor_local_t::tick_prerun() {
#if defined(NEW_EDITOR_GRAB_MODE)

	void* grabbed = get_grabbed_entity();
	if (grabbed) {

		editor_vec3_t transldir = get_point_in_player_direction(m_grab_distance);

		if (m_locked_z) {

			transldir.z = m_grabbed_object_prior_pos.z;

		}
		m_grabbed_object_prior_pos = transldir;


		editor_set_pos(grabbed, transldir);

	}
#endif
}

void mh_editor_local_t::tick_postrun() {


}
void mh_editor_local_t::init_for_session() {
	m_initialized = 1;
	m_input_forwarder.set_editor(this);

	m_grabbed_entity.clear();

	init_dom();


}
event_consumed_e mh_editor_input_handler_t::receive_char_event(unsigned gotchar) {
	return m_editor->receive_char_event(gotchar);
}
event_consumed_e mh_editor_input_handler_t::receive_keydown_event(const char* keyname) {
	return m_editor->receive_keydown_event(keyname);
}
event_consumed_e mh_editor_input_handler_t::receive_keyup_event(const char* keyname) {
	return m_editor->receive_keyup_event(keyname);
}
void mh_editor_input_handler_t::receive_mouse_move_event(unsigned x, unsigned y) {
	m_editor->receive_mouse_move_event(x, y);
}



event_consumed_e mh_editor_local_t::receive_char_event(unsigned gotchar) {
	return event_consumed_e::UNCONSUMED;
}
event_consumed_e mh_editor_local_t::receive_keydown_event(const char* keyname) {
	if (sh::string::streq(keyname, "PGUP")) {
		void* grabent = get_grabbed_entity();
		if (!grabent)
			return event_consumed_e::UNCONSUMED;


		idVec3 scale = get_entity_scale();

		scale.x += m_current_size_increment;
		scale.y += m_current_size_increment;
		scale.z += m_current_size_increment;

		set_entity_scale(scale);
		set_entity_clipscale(scale);

		return event_consumed_e::CONSUMED;

	}
	else if (sh::string::streq(keyname, "PGDN")) {

		void* grabent = get_grabbed_entity();
		if (!grabent)
			return event_consumed_e::UNCONSUMED;


		idVec3 scale = get_entity_scale();

		scale.x -= m_current_size_increment;
		scale.y -= m_current_size_increment;
		scale.z -= m_current_size_increment;


		set_entity_scale(scale);
		set_entity_clipscale(scale);
		return event_consumed_e::CONSUMED;
	}
	else if (sh::string::streq(keyname, "INS")) {

		void* grabee = get_grabbed_entity();
		if (!grabee)
			return event_consumed_e::UNCONSUMED;

		void* edef = get_entitydef();

		if (edef) {
			void* newe = spawn_entity_from_entitydef(edef);

			if (newe) {
				idVec3 newpos;
				get_entity_position(grabee, &newpos);
				editor_set_pos(newe, newpos);
				update_map_entity(newe, true);
				ungrab();

				grab(newe);
				return event_consumed_e::CONSUMED;
			}

		}
	}
	else if (sh::string::streq(keyname, "KP_1")) {
		void* grabee = get_grabbed_entity();
		if (!grabee)
			return event_consumed_e::UNCONSUMED;

		editor_angles_t ang = editor_get_angles(grabee);
		ang.yaw -= m_angle_increment;

		editor_set_angles(grabee, ang);
		return event_consumed_e::CONSUMED;
	}
	else if (sh::string::streq(keyname, "KP_3")) {
		void* grabee = get_grabbed_entity();
		if (!grabee)
			return event_consumed_e::UNCONSUMED;

		editor_angles_t ang = editor_get_angles(grabee);
		ang.yaw += m_angle_increment;

		editor_set_angles(grabee, ang);
		return event_consumed_e::CONSUMED;
	}

	else if (sh::string::streq(keyname, "KP_4")) {
		void* grabee = get_grabbed_entity();
		if (!grabee)
			return event_consumed_e::UNCONSUMED;

		editor_angles_t ang = editor_get_angles(grabee);
		ang.pitch -= m_angle_increment;

		editor_set_angles(grabee, ang);
		return event_consumed_e::CONSUMED;
	}
	else if (sh::string::streq(keyname, "KP_6")) {
		void* grabee = get_grabbed_entity();
		if (!grabee)
			return event_consumed_e::UNCONSUMED;

		editor_angles_t ang = editor_get_angles(grabee);
		ang.pitch += m_angle_increment;

		editor_set_angles(grabee, ang);
		return event_consumed_e::CONSUMED;
	}
	else if (sh::string::streq(keyname, "KP_7")) {
		void* grabee = get_grabbed_entity();
		if (!grabee)
			return event_consumed_e::UNCONSUMED;

		editor_angles_t ang = editor_get_angles(grabee);
		ang.roll -= m_angle_increment;

		editor_set_angles(grabee, ang);
		return event_consumed_e::CONSUMED;
	}
	else if (sh::string::streq(keyname, "KP_9")) {
		void* grabee = get_grabbed_entity();
		if (!grabee)
			return event_consumed_e::UNCONSUMED;

		editor_angles_t ang = editor_get_angles(grabee);
		ang.roll += m_angle_increment;

		editor_set_angles(grabee, ang);
		return event_consumed_e::CONSUMED;
	}
	else if (sh::string::streq(keyname, "KP_2")) {
		void* grabee = get_grabbed_entity();
		if (!grabee)
			return event_consumed_e::UNCONSUMED;
		cycle_staticmodel(grabee, -1);
		return event_consumed_e::CONSUMED;
	}
	else if (sh::string::streq(keyname, "KP_8")) {
		void* grabee = get_grabbed_entity();
		if (!grabee)
			return event_consumed_e::UNCONSUMED;
		cycle_staticmodel(grabee, 1);
		return event_consumed_e::CONSUMED;
	}
	//increase distance to project object outwards
	else if (sh::string::streq(keyname, "UPARROW")) {

		if (!this->is_prev_grabbed_entity_valid())
			return event_consumed_e::UNCONSUMED;

		m_grab_distance += m_object_distance_increment;

		return event_consumed_e::CONSUMED;
	}
	else if (sh::string::streq(keyname, "DOWNARROW")) {
		if (!this->is_prev_grabbed_entity_valid())
			return event_consumed_e::UNCONSUMED;

		m_grab_distance -= m_object_distance_increment;

		if (m_grab_distance < 0)
			m_grab_distance = 0;

		return event_consumed_e::CONSUMED;
	}
	else if (sh::string::streq(keyname, "Z")) {
		if (get_grabbed_entity()) {
			m_locked_z = !m_locked_z;
			return event_consumed_e::CONSUMED;
		}
	}
	return event_consumed_e::UNCONSUMED;

}


static mh_new_fieldcached_t<idStr, YS("idMapFileLocal"), YS("name")> g_mapfile_name;


event_consumed_e mh_editor_local_t::receive_keyup_event(const char* keyname) {
	if (sh::string::streq(keyname, "F3")) {

		write_current_map_to_overrides();
		return event_consumed_e::CONSUMED;
	}
	if (!get_grabbed_entity())
		return event_consumed_e::UNCONSUMED;
	if (sh::string::streq(keyname, "F1")) {

		const char* txt = get_entitydef_text();
		if (txt) {
			idLib::Printf("%s", txt);
			m_entitydef_source->set_text(txt);

			decl_parsing::parse(m_current_parsed_entitydef, txt);

			std::string unparsed = decl_parsing::unparse(m_current_parsed_entitydef);

			idLib::Printf("result json = \n%s\n", unparsed.c_str());
			return event_consumed_e::CONSUMED;
		}
	}

	else if (sh::string::streq(keyname, "F2")) {
		void* edef_grabbed = *g_entitydef_identity(get_grabbed_entity());
		if (!edef_grabbed) {

			idLib::Printf("Was null\n");
			return event_consumed_e::UNCONSUMED;
		}

		const char* txtsource = *g_iddecl_textsource(edef_grabbed);

		if (!txtsource) {

			idLib::Printf("Txtsource was null\n");
		}
		else {

			idLib::Printf("%s\n", txtsource);
		}
		return event_consumed_e::CONSUMED;
	}


	return event_consumed_e::UNCONSUMED;

}
void mh_editor_local_t::write_current_map_to_overrides() {

	const char* pth = get_current_map_file_output_path();
	if (!pth)
		return;
	idLib::Printf("Saving map to %s\n", pth);


	call_as<void>(descan::g_idmapfile_write, get_level_map(), pth);
	namespace fs = std::filesystem;

	fs::path expected = pth;
	expected = expected.replace_extension(".map");

	fs::path dup = expected;

	dup = dup.replace_extension(".entities");

	std::string exps = expected.generic_string();
	std::string dups = dup.generic_string();

	idLib::Printf("Moving %s to %s\n", exps.c_str(), dups.c_str());

	fs::rename(expected, dup);

}
const char* mh_editor_local_t::get_current_map_file_output_path() {
	void* lvlmap = get_level_map();
	if (lvlmap) {

		idStr* mapname = g_mapfile_name(lvlmap);

		if (mapname->data) {
			get_override_path(mapname->data, m_temp_pathbuf);
			return m_temp_pathbuf;
		}
		else
			return nullptr;
	}
	else
		return nullptr;

}
void mh_editor_local_t::receive_mouse_move_event(unsigned x, unsigned y) {

}
void mh_editor_local_t::bind_entity_to_player(void* entity) {
#if !defined(NEW_EDITOR_GRAB_MODE)
	idEventArg argy;
	argy.make_entity(get_local_player());

	ev_bind(entity, &argy);

#else
	ev_unbind(entity);
	editor_vec3_t eent;
	editor_vec3_t eplayer;
	idVec3 entpos;
	idVec3 ppos;

	get_entity_position(entity, &entpos);
	get_entity_position(get_local_player(), &ppos);

	eent = entpos;
	eplayer = ppos;
	m_grabbed_object_original_pos = eent;
	m_grabbed_object_prior_pos = eent;
	m_grab_distance = eent.distance3d(ppos);



#endif
}
bool mh_editor_local_t::is_prev_grabbed_entity_valid() {
	return m_grabbed_entity.get() != nullptr;
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
		return m_grabbed_entity.get();
		//return find_entity(m_prevgrab_entity_name.c_str());
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
	m_grabbed_entity = targ;
	m_current_entity_name_label->set_text(get_entity_name(entity));
}
void mh_editor_local_t::editor_spawn_entitydef(void* entitydef) {

	cs_assert(entitydef != nullptr);
	if (is_prev_grabbed_entity_valid()) {
		ungrab();
	}

	void* newe = spawn_entity_from_entitydef(entitydef);

	if (!newe) {
		TRACE_EDITOR("Failed to spawn entitydef!\n");
		return;
	}

	editor_vec3_t grabdist = this->get_point_in_player_direction(m_grab_distance);

	editor_set_pos(newe, grabdist);
	update_map_entity(newe, true);
	grab(newe);
}
void mh_editor_local_t::grab_player_focus() {

	void* ltarg = get_player_look_target();

	if (!ltarg || ltarg == get_grabbed_entity()) {
		ungrab();
		return;
	}


	grab(ltarg);
}
void mh_editor_local_t::ungrab() {
	void* grb = get_grabbed_entity();

	if (grb) {

#if !defined(NEW_EDITOR_GRAB_MODE)
		ev_unbind(grb);
#else

		//fix up our localoffset from binding
		idEventArg tmpbind = ev_getBindMaster(grb);

		//ev_unbind(grb);
		idVec3 pp = m_grabbed_object_prior_pos;

		editor_set_pos(grb, &pp);
		//set_entity_position(grb, &pp);

		void* oldmaster = tmpbind.value.er;

		if (oldmaster != nullptr && oldmaster != get_world()) {


			//	ev_bind(grb, &tmpbind);
		}

		update_map_entity(grb, false);
#endif
	}
	//m_prevgrab_entity_name = "";
	m_grabbed_entity.clear();
	m_current_entity_name_label->set_text("");
	m_entitydef_source->set_text("");


}

static mh_editor_local_t g_local_editor{};



mh_editor_interface_t* get_current_editor() {
	return &g_local_editor;

}