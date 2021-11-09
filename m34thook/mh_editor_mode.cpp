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
#include "rapiddecl/rapidjson.h"
#include "rapiddecl/document.h"
#include "rapiddecl/prettywriter.h"
#include "rapiddecl/stringbuffer.h"
class mh_editor_local_t;

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

//simple double precision vec3. 
struct editor_vec3_t {
	union {
		struct {
			double x, y, z;
		};
		struct {
			__m128d xmmlo;
			__m128d xmmhi;
		};
	};


	editor_vec3_t() : x(.0), y(.0), z(.0) {}


	editor_vec3_t(const idVec3* v) : x(v->x), y(v->y), z(v->z) {}
	editor_vec3_t(const idVec3& v) : editor_vec3_t(&v) {}
	editor_vec3_t(__m128d lowpart, __m128d highpart) : xmmlo(lowpart), xmmhi(highpart) {}


	editor_vec3_t(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
	editor_vec3_t(double dval) {
		xmmlo = _mm_set1_pd(dval);
		xmmhi = xmmlo;
	}

	editor_vec3_t& operator = (const idVec3& other) {

		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	editor_vec3_t& operator =(const idVec3* other) {
		x = other->x;
		y = other->y;
		z = other->z;
		return *this;
	}

	editor_vec3_t operator +(editor_vec3_t other)const {
		return editor_vec3_t(_mm_add_pd(xmmlo, other.xmmlo), _mm_add_pd(xmmhi, other.xmmhi));
	}
	editor_vec3_t operator -(editor_vec3_t other) const {
		return editor_vec3_t(_mm_sub_pd(xmmlo, other.xmmlo), _mm_sub_pd(xmmhi, other.xmmhi));
	}
	editor_vec3_t operator *(editor_vec3_t other) const {
		return editor_vec3_t(_mm_mul_pd(xmmlo, other.xmmlo), _mm_mul_pd(xmmhi, other.xmmhi));
	}
	editor_vec3_t operator /(editor_vec3_t other)const  {
		return editor_vec3_t(_mm_div_pd(xmmlo, other.xmmlo), _mm_div_pd(xmmhi, other.xmmhi));
	}
	//helper func for dp sqrt
	static double do_sqrt(double val) {
		__m128d dv = _mm_set_sd(val);
		return _mm_cvtsd_f64(_mm_sqrt_sd(dv, dv));

	}

	editor_vec3_t squared()const {
		return *this * *this;
	}

	double hsum() const {
		return x + y + z;
	}
	
	double distance3d(editor_vec3_t other) const {

		double sum = (*this - other).squared().hsum();
		return do_sqrt(sum);
	}


	double dot(editor_vec3_t other) const {

		return (*this * other).hsum();

	}
	

	editor_vec3_t normalized() const {


		double sqrlen = dot(*this);

		double sqrtval = do_sqrt(sqrlen);
		return *this / sqrtval;

	}

	operator idVec3() const {
		return idVec3{ (float)x, (float)y, (float)z };
	}


};

static mh_new_fieldcached_t < void*, YS("idEntity"), YS("entityDef")>g_entitydef_identity{};
//static mh_fieldcached_t<void*> g_entitydef_identity;

static mh_new_fieldcached_t<void*, YS("idMapEntityLocal"), YS("entityDef")> g_mapentity_entitydef{};
static mh_new_fieldcached_t<const char*, YS("idDecl"), YS("textSource")> g_iddecl_textsource{};
static mh_new_fieldcached_t<idVec3, YS("idBloatedEntity"), YS("renderModelInfo"), YS("scale")> g_field_renderscale{};
static mh_new_fieldcached_t<idVec3, YS("idBloatedEntity"), YS("clipModelInfo"), YS("size")> g_clipmodel_size{};
static mh_new_fieldcached_t<idVec3, YS("idBloatedEntity"), YS("renderModelInfo"), YS("scale")> g_new_field_renderscale{};


class entity_iface_t {

public:
	virtual editor_vec3_t get_entity_scale() = 0;
	virtual void set_entity_scale(editor_vec3_t newscale) = 0;

	virtual editor_vec3_t get_entity_clipscale() = 0;
	virtual void set_entity_clipscale(editor_vec3_t newscale) = 0;

	virtual std::string get_entitydef_text() = 0;


	virtual editor_vec3_t get_entity_position() = 0;

	virtual void set_entity_position(editor_vec3_t pos) = 0;
};

//entity at runtime
class entity_rt_t : public entity_iface_t {
	void* m_ptr;
public:
	virtual editor_vec3_t get_entity_scale() {
		return *g_new_field_renderscale(m_ptr);
	}
	virtual void set_entity_scale(editor_vec3_t newscale) {

		cs_uninit_array_t<idEventArg, 6> args{};
		//or use setRenderModelScale, but iirc that doesnt work as well as lerp
		args[0].make_float(newscale.x);
		args[1].make_float(newscale.y);
		args[2].make_float(newscale.z);
		args[3].make_int(0);
		args[4].make_int(0);
		args[5].make_int(0);
		ev_lerpRenderScale(m_ptr, &args[0]);
	}

	virtual editor_vec3_t get_entity_clipscale() {
		return *g_clipmodel_size(m_ptr);

	}
	virtual void set_entity_clipscale(editor_vec3_t newscale) {

		*g_clipmodel_size(m_ptr) = static_cast<idVec3>( newscale );
	}

	virtual std::string get_entitydef_text() {
		void* edef_grabbed = *g_entitydef_identity(m_ptr);
		if (!edef_grabbed)
			return "";

		const char* src = *g_iddecl_textsource(edef_grabbed);

		return src;
	}


	virtual editor_vec3_t get_entity_position() {
		return mh_ScriptCmdEnt_idEntity(ev_getWorldOrigin.Get(), m_ptr).value.v_vec3;
	}

	virtual void set_entity_position(editor_vec3_t pos) {

		cs_uninit_t<idEventArg> arg;
		arg->make_vec3(pos);

		mh_ScriptCmdEntFast(ev_setWorldOrigin.Get(), m_ptr, &arg);
	}
	entity_rt_t() : m_ptr(nullptr) {}

	void set_entity(void* en) {
		m_ptr = (en);
	}
	
};
using dmemb_iter_t = rapiddecl::Value::MemberIterator;

class entity_decl_t : public entity_iface_t {
	rapiddecl::Document m_decl;
	rapiddecl::Value* m_edit;//cached edit

public:


	static double get_value_or_z(rapiddecl::Value* v, const char* name) {
		if (!v->HasMember(name))
			return .0;

		return v->FindMember(name)->value.GetDouble();

	}
	editor_vec3_t get_vec3_from_value(rapiddecl::Value* v) {

		return editor_vec3_t{ get_value_or_z(v, "x"), get_value_or_z(v, "y"), get_value_or_z(v, "z") };

	}

	auto& al() {
		return m_decl.GetAllocator();
	}

	void create_value_from_vec3(rapiddecl::Value& r, editor_vec3_t v) {
		r.SetObject();

		r.AddMember("x", v.x, al());
		r.AddMember("y", v.y, al());
		r.AddMember("z", v.z, al());

		
	}


	rapiddecl::Value* edit() {
		return m_edit;

	}
	virtual editor_vec3_t get_entity_scale() {

		auto ed = edit();

		dmemb_iter_t iter = ed->FindMember("renderModelInfo");
		if (iter == ed->MemberEnd())
			return editor_vec3_t{ 1.0 };

		dmemb_iter_t scaleiter = iter->value.FindMember("scale");

		if (scaleiter == iter->value.MemberEnd()) {
			return editor_vec3_t{ 1.0 };
		}

		return get_vec3_from_value(&scaleiter->value);
	}
	virtual void set_entity_scale(editor_vec3_t newscale) {

	}

	virtual editor_vec3_t get_entity_clipscale() {

	}
	virtual void set_entity_clipscale(editor_vec3_t newscale) {

	}

	virtual std::string get_entitydef_text() {

	}


	virtual editor_vec3_t get_entity_position() {

		return get_vec3_from_value(&m_decl.FindMember("edit")->value.FindMember("spawnPosition")->value);
	}

	virtual void set_entity_position(editor_vec3_t pos) {

	}

	

};

class mh_editor_local_t : public mh_editor_interface_t {
	std::string m_prevgrab_entity_name;
	mh_editor_input_handler_t m_input_forwarder;
	mh_dom_t* m_gui;
	mh_ui_ele_t* m_current_entity_name_label;

	mh_ui_ele_t* m_entitydef_source;
	rapiddecl::Document m_current_parsed_entitydef;

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

public:
	virtual void grab(void* entity);
	virtual void grab_player_focus();
	virtual void ungrab();
	event_consumed_e receive_char_event(unsigned gotchar);
	event_consumed_e receive_keydown_event(const char* keyname);
	event_consumed_e receive_keyup_event(const char* keyname);
	void receive_mouse_move_event(unsigned x, unsigned y);

	
	virtual void init_for_session();
};

static mh_new_fieldcached_t<void, YS("idPlayer"), YS("focusTracker"), YS("focusPointTrace")> g_idPlayer_focusTracker_focusTrace;

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
	void* res =call_virtual<void*>(get_gamelocal(), descan::g_vftbl_offset_MapFindEntity_idEntity / 8, get_gamelocal(), get_grabbed_entity());

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
	m_current_entity_name_label = m_gui->alloc_e2d("current_selected_ent_name", 0.80, 0.10, 0.20, 0.10);
	m_current_entity_name_label->init_text("", 1.0, colorCyan);

	m_entitydef_source = m_gui->alloc_e2d("entitydef_source", 0.1, 0.1, 0.8, 0.8);

	m_entitydef_source->init_text("", 0.75, colorGreen);

	mh_mainloop::add_preframe_callback(mh_editor_local_t::prerun_forwarder, (void*)this);

	mh_mainloop::add_postframe_callback(mh_editor_local_t::postrun_forwarder, (void*)this);
	
}

void mh_editor_local_t::tick_prerun() {

}

void mh_editor_local_t::tick_postrun() {


}
void mh_editor_local_t::init_for_session() {

	m_input_forwarder.set_editor(this);

	m_prevgrab_entity_name.clear();
	
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
	if (sh::string::streq(keyname, "MWHEELUP")) {
		void* grabent = get_grabbed_entity();
		if (!grabent)
			return event_consumed_e::UNCONSUMED;


		idVec3 scale = get_entity_scale();

		scale.x += 0.1;
		scale.y += 0.1;
		scale.z += 0.1;

		set_entity_scale(scale);
		set_entity_clipscale(scale);
		return event_consumed_e::CONSUMED;

	}
	else if (sh::string::streq(keyname, "MWHEELDOWN")) {

		void* grabent = get_grabbed_entity();
		if (!grabent)
			return event_consumed_e::UNCONSUMED;


		idVec3 scale = get_entity_scale();

		scale.x -= 0.1;
		scale.y -= 0.1;
		scale.z -= 0.1;


		set_entity_scale(scale);
		set_entity_clipscale(scale);
		return event_consumed_e::CONSUMED;
	}
	return event_consumed_e::UNCONSUMED;

}


event_consumed_e mh_editor_local_t::receive_keyup_event(const char* keyname) {
	
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
void mh_editor_local_t::receive_mouse_move_event(unsigned x, unsigned y) {

}
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
	m_current_entity_name_label->set_text(m_prevgrab_entity_name.c_str());
}

void mh_editor_local_t::grab_player_focus() {

	void* ltarg = get_player_look_target();
	if (!ltarg)
		return;
	grab(ltarg);
}
void mh_editor_local_t::ungrab() {
	void* grb = get_grabbed_entity();

	if (grb) {
		ev_unbind(grb);
	}
	m_prevgrab_entity_name.clear();
	m_current_entity_name_label->set_text("");
	m_entitydef_source->set_text("");
}

static mh_editor_local_t g_local_editor{};



mh_editor_interface_t* get_current_editor() {
	return &g_local_editor;

}