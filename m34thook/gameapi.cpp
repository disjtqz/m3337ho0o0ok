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
#include "idmath.hpp"
#include <cassert>
#include <stdlib.h>
#include <stdio.h>
#include "gameapi.hpp"
void* find_entity(const char* name) {
	void* gamloc = *reinterpret_cast<void**>(descan::g_gamelocal);

	void* vftbl = reinterpret_cast<void**>(gamloc)[0];

	void* findentity_func = (((char*)vftbl) + 0x78);
	findentity_func = *(void**)findentity_func;

	return reinterpret_cast<void* (*)(void*, const char*)>(findentity_func)(gamloc, name);
}
void* find_next_entity_with_class(const char* classname, void* entity_iterator) {
	void* gamloc = *reinterpret_cast<void**>(descan::g_gamelocal);

	return call_as<void*>(descan::g_find_next_entity_with_class, gamloc, entity_iterator, classname);
}

void* get_level_map() {
#if !defined(MH_ETERNAL_V6)
	return call_as<void*>(descan::g_maplocal_getlevelmap, *reinterpret_cast<void**>(descan::g_gamelocal));
#else
	void* gamelocal = *reinterpret_cast<void**>(descan::g_gamelocal);

	char* gamelocal_vftbl = *reinterpret_cast<char**>(gamelocal);

	void* getlevelmap_func = *reinterpret_cast<void**>(gamelocal_vftbl + descan::g_vftbl_offset_getlevelmap);

	return call_as<void*>(getlevelmap_func, gamelocal);
#endif
}
bool get_classfield_boolean(void* obj, const classVariableInfo_t* varinfo) {
	if (varinfo->get) {
		return varinfo->get(obj);
	}
	else {
		return *reinterpret_cast<bool*>(reinterpret_cast<char*>(obj) + varinfo->offset);
	}
}

void set_classfield_boolean(void* obj, const classVariableInfo_t* varinfo, bool value) {
	if (varinfo->set) {
		varinfo->set(obj, value);
	}
	else {
		*reinterpret_cast<bool*>(reinterpret_cast<char*>(obj) + varinfo->offset) = value;
	}
}
long long get_classfield_int(void* obj, const classVariableInfo_t* varinfo) {
	if (varinfo->get) {
		return varinfo->get(obj);
	}
	else {
		char* objptr =reinterpret_cast<char*>(obj) + varinfo->offset;


		switch(varinfo->size) {
		case 1:
			return *objptr;
		case 2:
			return *reinterpret_cast<short*>(objptr);

		case 4:
			return *reinterpret_cast<int*>(objptr);
		case 8:
			return *reinterpret_cast<long long*>(objptr);
		}
		assert(false);
		return 0;
	}
}
long long get_classfield_int(void* obj, const char* clazs, const char* field) {
	auto inf = idType::FindClassField(clazs, field);
	return get_classfield_int(obj, inf);
}
bool get_classfield_boolean(void* obj, const char* clazs, const char* field) {
	auto inf = idType::FindClassField(clazs, field);
	return get_classfield_boolean(obj, inf);
}
const char* get_entity_name(void* obj) {
	return reinterpret_cast<idStr*>(reinterpret_cast<char*>(obj) + idType::FindClassField("idEntity", "name")->offset)->data;
}
static char* g_engine_t = nullptr;
char* get_engine() {

	if(g_engine_t == nullptr) {
		g_engine_t = (reinterpret_cast<char*>(descan::g_global_typeinfo_tools) - idType::FindClassField("engine_t", "typeInfoTools")->offset);
	}
	return g_engine_t;
}
struct idRenderModelGui;

struct idDebugHUD {
	struct idDebugHUD_vtbl* vftbl;

};
struct idDebugHUD_vtbl
{
  void (__fastcall *dctor_idDebugHUD)(idDebugHUD *thiz);
  void (__fastcall *Render)(idDebugHUD *thiz, idRenderModelGui *);
  void (__fastcall *Frame)(idDebugHUD *thiz);
  void (__fastcall *Clear)(idDebugHUD *thiz, bool);
  void (*Printf3)(idDebugHUD *thiz, const char *, ...);
  void (__fastcall *Printf1)(idDebugHUD *thiz, int, int, const char *, char *);
  void (*Printf4)(idDebugHUD *thiz, int, int, const char *, ...);
  void (__fastcall *Printf2)(idDebugHUD *thiz, int, int, const idColor *, const idColor *, const char *, char *);
  void (*Printf5)(idDebugHUD *thiz, int, int, const idColor *, const idColor *, const char *, ...);
  void (__fastcall *Printf6)(idDebugHUD *thiz, int, int, const idColor *, const char *, char *);
  void (*Printf7)(idDebugHUD *thiz, int, int, const idColor *, const char *, ...);
  void (*Printf8)(idDebugHUD *thiz, int, int, const idColor *, const float, const char *, ...);
  void (__fastcall *Printf9)(idDebugHUD *thiz, int, int, const idColor *, const float, const int, const char *, char *);
  void (*Printf10)(idDebugHUD *thiz, int, int, const idColor *, const idColor *, const float, const char *, ...);
  void (__fastcall *Printf11)(idDebugHUD *thiz, int, int, const idColor *, const idColor *, const float, const int, const char *, char *);
  void (__fastcall *Printf)(idDebugHUD *thiz, int, int, const idColor *, const idColor *, const float, const int, const char *, char *);
  void *unk2;
  void (*AddPersistentText)(idDebugHUD *thiz, int, int, const idColor *, const float, const char *, ...);
  void (__fastcall *RemovePersistentText)(idDebugHUD *thiz, int, int);
  void (__fastcall *SetLeftAlign)(idDebugHUD *thiz);
  void (__fastcall *SetRightAlign)(idDebugHUD *thiz);
  void (__fastcall *SetTextPosition)(idDebugHUD *thiz, const int, const int);
  void (__fastcall *SetTextScale)(idDebugHUD *thiz, const float);
  void (__fastcall *SetTextColor)(idDebugHUD *thiz, const idColor *);
};


static idDebugHUD* g_debughud = nullptr;

static idDebugHUD* get_debug_hud() {
	if(!g_debughud) {
		g_debughud = *reinterpret_cast<idDebugHUD**>(get_engine() + idType::FindClassField("engine_t", "debugHUD")->offset);
	}
	return g_debughud;
}

void add_persistent_text(unsigned x, unsigned y, unsigned RGBA, float scale,  const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	char vbuf[4096];
	vsprintf_s(vbuf, fmt, ap);

	idColor unpacked_color{};
	unpacked_color.a = (RGBA>>24) / 255.0F;
	unpacked_color.r = static_cast<unsigned char>(RGBA>>16) / 255.0F;
	unpacked_color.g = static_cast<unsigned char>(RGBA>>8) / 255.0F;
	unpacked_color.b = static_cast<unsigned char>(RGBA) / 255.0F;

	idDebugHUD* dhud = get_debug_hud();
	dhud->vftbl->AddPersistentText(dhud, x, y, &unpacked_color, scale, vbuf);


	va_end(ap);
}

void remove_persistent_text(unsigned x, unsigned y) {

	idDebugHUD* dhud = get_debug_hud();
	dhud->vftbl->RemovePersistentText(dhud, x, y);
}

static mh_fieldcached_t<idManagedClassPtr> g_field_player_focus{};



void* get_player_look_target() {
	void* player1=find_entity("player1");
	MH_UNLIKELY_IF(!player1)
		return nullptr;
	idManagedClassPtr* looktarg = g_field_player_focus(player1, "idPlayer", "focusTracker", "focusEntity");


	return (void*)looktarg->ptr;
}
static mh_fieldcached_t<void*> g_entity_physics{};
void* get_entity_physics_obj(void* ent) {
	MH_UNLIKELY_IF(!ent)
		return nullptr;

	return *g_entity_physics(ent, "idEntity", "physics");
}
#if 0
struct _TypeDescriptor {
	void* type_info_vftbl;
	void* whocares;
	char name[];
};
struct __RTTIClassHierarchyDescriptor {
	size_t unused[2];
	size_t numbases;
	//first pointer gets you to the array, second to the object that points to the typedesc
	_TypeDescriptor*** bases;
};
typedef const struct _s__RTTICompleteObjectLocator {
  unsigned long signature;
  unsigned long offset;
  unsigned long cdOffset;
  _TypeDescriptor *pTypeDescriptor;
  __RTTIClassHierarchyDescriptor *pClassDescriptor;
} __RTTICompleteObjectLocator;

bool is_subclass_of_rttisig(void* obj, rttisig_ref_t rsig) {
	MH_UNLIKELY_IF(!obj)
		return false;

	__RTTICompleteObjectLocator* locator = reinterpret_cast<__RTTICompleteObjectLocator**>(obj)[-1];
	MH_UNLIKELY_IF(!locator) {
		return false;
	}
	__RTTIClassHierarchyDescriptor *descr = locator->pClassDescriptor;
	MH_UNLIKELY_IF(!descr)
		return false;

	unsigned numbase = (unsigned)descr->numbases;

	for(unsigned i = 0; i < numbase; ++i) {
		if(!strcmp((*(descr->bases[i]))->name, rsig)) {
			return true;
		}
	}
	return false;
}
#else

#endif
bool is_subclass_of_rttisig(void* obj, rttisig_ref_t rsig) {

	return false;
}

static mh_fieldcached_t<void*> g_eventdef_offset{};
void* get_eventdef_interface() {
	return *g_eventdef_offset(get_engine(), "engine_t", "eventDefInterface");
}
static mh_fieldcached_t<idVec3> g_player_focus_trace{};
void get_player_trace_pos(idVec3* outvec) {
	void* player = find_entity("player1");
	if(!player)
		return;
	*outvec = *g_player_focus_trace(player, "idPlayer", "focusTracker", "focusTrace", "close");
}

static mh_fieldcached_t<void*> g_resourcelist_t_from_resourcelist{};
static mh_fieldcached_t<const char*> g_idresource_get_name{};
void* locate_resourcelist_member(const char* reslist_classname /* example:"idDeclEntityDef" */, const char* member_name) {
	void** entitydef_reslist = call_as<void**>(descan::g_resourcelist_for_classname, reslist_classname);
	if(!entitydef_reslist) {
		idLib::Printf("Failed to locate resourcelist %s\n", reslist_classname);
		return nullptr;

	}
	void* resourcelist = *g_resourcelist_t_from_resourcelist(entitydef_reslist, "idResourceList", "resourceList");

	if(!resourcelist) {
		idLib::Printf("resourcelist %s doesnt have an underlying resourceList_t???\n", reslist_classname);
		return nullptr;
	}
	int reslist_length = *reinterpret_cast<int*>(reinterpret_cast<char*>(resourcelist) + descan::g_offset_resourcelist_length);

	void* our_resource = nullptr;
	for(int i = 0; i < reslist_length; ++i) {
		void* current_resource = call_as<void*>(descan::g_resourcelist_index, resourcelist, i);
		
		MH_UNLIKELY_IF(!current_resource) {
			//should never happen
			continue;
		}
		if(!strcmp(*g_idresource_get_name(current_resource, "idResource", "name", "str"), member_name)) {
			our_resource = current_resource;
			break;
		}

	}
	return our_resource;
}

void* spawn_entity_from_entitydef(void* entdef) {
	/*
		pray this vftbl displ never changes
	*/
	unsigned VFTBLIDX_GAMELOCAL_SPAWN = 0x40 / 8;
	//ewwww

	void* gamelocal = *(void**)descan::g_gamelocal;

	void* gamelocal_vftbl = *(void**)gamelocal;

	void* spawn_func = reinterpret_cast<void**>(gamelocal_vftbl)[VFTBLIDX_GAMELOCAL_SPAWN];

	void* resulting_entity = call_as<void*>(spawn_func, gamelocal, entdef, 0xFFFFFFFF, 0x1FFFFFE, -1);

	return resulting_entity;

}


void set_entity_position(void* entity, idVec3* pos) {
	const char* ename = get_entity_name(entity);

	if(!ename) {
		idLib::Printf("Entity name was null?!?!?!?\n");
		return;
	}

	char outbuff[1024];
	sprintf_s(outbuff, "ai_ScriptCmdEnt %s teleport %f %f %f .0 .0 .0\n", ename, pos->x, pos->y, pos->z);

	idCmd::execute_command_text(outbuff);
		//the below does work, but is not portable
	//2208
	//entity vftbl function at offset 3624 just returns physicsObj pointer on idEntity
	//offset 64 on physics might be set position
	//yup looks right to me
	//void* ent_phys = get_entity_physics_obj(resulting_entity);

	//call_as<void>(reinterpret_cast<void***>(ent_phys)[0][64/8], ent_phys, &playertrace);
}


void** get_class_vtbl(std::string_view clsname) {
	
	auto iter = g_str_to_rrti_type_descr.find(clsname);

	if(iter == g_str_to_rrti_type_descr.end())
		return nullptr;


	return reinterpret_cast<void**>(iter->second);
}