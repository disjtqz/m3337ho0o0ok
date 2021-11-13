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
#include "idmath.hpp"
#include <cassert>
#include <stdlib.h>
#include <stdio.h>
#include "gameapi.hpp"
#include "snaphakalgo.hpp"
#include "mh_inputsys.hpp"
#include "mh_guirender.hpp"
#include "mh_mainloop.hpp"



void* find_entity(const char* name) {
	void* gamloc = get_gamelocal();

	void* vftbl = reinterpret_cast<void**>(gamloc)[0];

	void* findentity_func = (((char*)vftbl) + 0x78);
	findentity_func = *(void**)findentity_func;

	return reinterpret_cast<void* (*)(void*, const char*)>(findentity_func)(gamloc, name);
}

void* get_local_player() {
	return find_entity("player1"); //alternatively, lookup entity index 0
}

void* get_world() {
	void** etable = get_entity_table();
	MH_UNLIKELY_IF(!etable)
		return nullptr;
	return etable[WORLD_ENTITY_IDX];
}
void* find_next_entity_with_class(const char* classname, void* entity_iterator) {
	void* gamloc = get_gamelocal();

	return call_as<void*>(descan::g_find_next_entity_with_class, gamloc, entity_iterator, classname);
}

void* get_level_map() {
#if !defined(MH_ETERNAL_V6)
	return call_as<void*>(descan::g_maplocal_getlevelmap, *reinterpret_cast<void**>(descan::g_gamelocal));
#else
	void* gamelocal = get_gamelocal();

	char* gamelocal_vftbl = *reinterpret_cast<char**>(gamelocal);

	void* getlevelmap_func = *reinterpret_cast<void**>(gamelocal_vftbl + descan::g_vftbl_offset_getlevelmap);

	return call_as<void*>(getlevelmap_func, gamelocal);
#endif
}
bool get_classfield_boolean(void* obj, const classVariableInfo_t* varinfo) {
	/*if (varinfo->get) {
		return varinfo->get(obj);
	}
	else {*/
		return get_classfield_int(obj, varinfo) != 0;
		//return *reinterpret_cast<bool*>(reinterpret_cast<char*>(obj) + varinfo->offset);
	//}
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
		char* objptr = reinterpret_cast<char*>(obj) + varinfo->offset;

		long long result;


		switch (varinfo->size) {
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
MH_NOINLINE
char* get_engine() {

	if (g_engine_t == nullptr) {
		g_engine_t = (reinterpret_cast<char*>(descan::g_global_typeinfo_tools) - idType::FindClassField("engine_t", "typeInfoTools")->offset);
	}

	return g_engine_t;
}

struct idDebugHUD {
	struct idDebugHUD_vtbl* vftbl;

};
struct idDebugHUD_vtbl
{
	void(__fastcall* dctor_idDebugHUD)(idDebugHUD* thiz);
	void(__fastcall* Render)(idDebugHUD* thiz, idRenderModelGui*);
	void(__fastcall* Frame)(idDebugHUD* thiz);
	void(__fastcall* Clear)(idDebugHUD* thiz, bool);
	void (*Printf3)(idDebugHUD* thiz, const char*, ...);
	void(__fastcall* Printf1)(idDebugHUD* thiz, int, int, const char*, char*);
	void (*Printf4)(idDebugHUD* thiz, int, int, const char*, ...);
	void(__fastcall* Printf2)(idDebugHUD* thiz, int, int, const idColor*, const idColor*, const char*, char*);
	void (*Printf5)(idDebugHUD* thiz, int, int, const idColor*, const idColor*, const char*, ...);
	void(__fastcall* Printf6)(idDebugHUD* thiz, int, int, const idColor*, const char*, char*);
	void (*Printf7)(idDebugHUD* thiz, int, int, const idColor*, const char*, ...);
	void (*Printf8)(idDebugHUD* thiz, int, int, const idColor*, const float, const char*, ...);
	void(__fastcall* Printf9)(idDebugHUD* thiz, int, int, const idColor*, const float, const int, const char*, char*);
	void (*Printf10)(idDebugHUD* thiz, int, int, const idColor*, const idColor*, const float, const char*, ...);
	void(__fastcall* Printf11)(idDebugHUD* thiz, int, int, const idColor*, const idColor*, const float, const int, const char*, char*);
	void(__fastcall* Printf)(idDebugHUD* thiz, int, int, const idColor*, const idColor*, const float, const int, const char*, char*);
	void* unk2;
	void (*AddPersistentText)(idDebugHUD* thiz, int, int, const idColor*, const float, const char*, ...);
	void(__fastcall* RemovePersistentText)(idDebugHUD* thiz, int, int);
	void(__fastcall* SetLeftAlign)(idDebugHUD* thiz);
	void(__fastcall* SetRightAlign)(idDebugHUD* thiz);
	void(__fastcall* SetTextPosition)(idDebugHUD* thiz, const int, const int);
	void(__fastcall* SetTextScale)(idDebugHUD* thiz, const float);
	void(__fastcall* SetTextColor)(idDebugHUD* thiz, const idColor*);
};


static idDebugHUD* g_debughud = nullptr;

static idDebugHUD* get_debug_hud() {
	if (!g_debughud) {
		g_debughud = *reinterpret_cast<idDebugHUD**>(get_engine() + idType::FindClassField("engine_t", "debugHUD")->offset);
	}
	return g_debughud;
}

void add_persistent_text(unsigned x, unsigned y, unsigned RGBA, float scale, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	char vbuf[4096];
	vsprintf_s(vbuf, fmt, ap);

	idColor unpacked_color{};
	unpacked_color.a = (RGBA >> 24) / 255.0F;
	unpacked_color.r = static_cast<unsigned char>(RGBA >> 16) / 255.0F;
	unpacked_color.g = static_cast<unsigned char>(RGBA >> 8) / 255.0F;
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
	void* player1 = get_local_player();
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
	_TypeDescriptor* pTypeDescriptor;
	__RTTIClassHierarchyDescriptor* pClassDescriptor;
} __RTTICompleteObjectLocator;

bool is_subclass_of_rttisig(void* obj, rttisig_ref_t rsig) {
	MH_UNLIKELY_IF(!obj)
		return false;

	__RTTICompleteObjectLocator* locator = reinterpret_cast<__RTTICompleteObjectLocator**>(obj)[-1];
	MH_UNLIKELY_IF(!locator) {
		return false;
	}
	__RTTIClassHierarchyDescriptor* descr = locator->pClassDescriptor;
	MH_UNLIKELY_IF(!descr)
		return false;

	unsigned numbase = (unsigned)descr->numbases;

	for (unsigned i = 0; i < numbase; ++i) {
		if (!strcmp((*(descr->bases[i]))->name, rsig)) {
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
MH_NOINLINE
void* get_eventdef_interface() {
	return *g_eventdef_offset(get_engine(), "engine_t", "eventDefInterface");
}
static mh_fieldcached_t<void*> g_rendersys_offset{};
MH_NOINLINE
void* get_rendersystem() {
	return *g_rendersys_offset(get_engine(), "engine_t", "renderSystem");
}


static mh_fieldcached_t<void*> g_cursor_offset{};

MH_NOINLINE
void* get_cursor() {
	return *g_cursor_offset(get_engine(), "engine_t", "cursor");
}

static mh_fieldcached_t<void*> g_editor_iface_offset{};
MH_NOINLINE
void* get_editor_interface() {
	return *g_editor_iface_offset(get_engine(), "engine_t", "editorInterface");
}

MH_NOINLINE
void* alloc_entity_interface(void* entity) {
	void* iface = get_editor_interface();
	return call_virtual<void*>(iface, 13, entity);

}
void destroy_entity_interface(void* inter) {
	//vector destructor
	call_virtual<void>(inter, 0);
}
static mh_fieldcached_t<idRenderModelGui*> g_cursor_model_offset{};
MH_NOINLINE
idRenderModelGui* get_cursor_rendermodel() {
	return *g_cursor_model_offset(get_cursor(), "idCursor", "gui");
}


static mh_fieldcached_t<void*> g_console_offset{};
MH_NOINLINE
void* get_console() {
	return *g_console_offset(get_engine(), "engine_t", "console");
}
static mh_fieldcached_t<void*> g_globalimages_offset{};

MH_NOINLINE
void* get_globalImages() {
	return *g_console_offset(get_engine(), "engine_t", "globalImages");

}

static mh_fieldcached_t<idVec3> g_player_focus_trace{};
void get_player_trace_pos(idVec3* outvec) {
	void* player = get_local_player();
	if (!player)
		return;
	*outvec = *g_player_focus_trace(player, "idPlayer", "focusTracker", "focusTrace", "close");
}

static mh_fieldcached_t<void*> g_resourcelist_t_from_resourcelist{};
static mh_fieldcached_t<const char*> g_idresource_get_name{};

void* resourcelist_for_classname(const char* clsname) {
	return call_as<void*>(descan::g_resourcelist_for_classname, clsname);
}

static mh_fieldcached_t<void*> g_resourcelist_for_idResource{};

void* resourcelist_for_resource(void* resource) {
	return *g_resourcelist_for_idResource(resource, "idResource", "resourceListPtr");
}


void* idResourceList_to_resourceList_t(void* resourcelist) {
	return *g_resourcelist_t_from_resourcelist(resourcelist, "idResourceList", "resourceList");
}

unsigned resourceList_t_get_length(void* reslist){
	MH_UNLIKELY_IF(!reslist) {
		sh::fatal("Null reslist passed to resourceList_t_get_length!");
	}
	return *mh_lea<unsigned>(reslist, descan::g_offset_resourcelist_length);
}

void* resourceList_t_lookup_index(void* reslist, unsigned idx) {
	MH_UNLIKELY_IF(!reslist) {
		sh::fatal("Null reslist in lookup_index!");
	}
	return call_as<void*>(descan::g_resourcelist_index, reslist, idx);
}

const char* get_resource_name(void* resource) {
	return *g_idresource_get_name(resource, "idResource", "name", "str");
}
void* locate_resourcelist_member_from_resourceList_t(void* resourcelist, const char* member_name, bool end_at_dollar ) {

	int reslist_length = resourceList_t_get_length(resourcelist);

	void* our_resource = nullptr;
	for (int i = 0; i < reslist_length; ++i) {
		void* current_resource = resourceList_t_lookup_index(resourcelist, i);

		MH_UNLIKELY_IF(!current_resource) {
			//should never happen
			continue;
		}
		const char* currname = get_resource_name(current_resource);

		if (end_at_dollar) {
			unsigned i2 = 0;
			while (true) {
				unsigned passedin = member_name[i2];
				unsigned fromres = currname[i2];
				++i2;
				if (fromres == '$')
					fromres = 0;

				if (fromres != passedin) {
					break;
				}

				if (fromres == 0) {
					our_resource = current_resource;

					break;
				}

			}
		}
		else {
			if (sh::string::streq(currname, member_name)) {
				our_resource = current_resource;
				break;
			}
		}

	}
	return our_resource;

}
void* locate_resourcelist_member_from_idResourceList(void* reslist /* example:"idDeclEntityDef" */, const char* member_name, bool end_at_dollar) {

	void* resourcelist = idResourceList_to_resourceList_t(reslist);

	if (!resourcelist) {
		idLib::Printf("resourcelist doesnt have an underlying resourceList_t???\n");
		return nullptr;
	}
	return locate_resourcelist_member_from_resourceList_t(resourcelist, member_name, end_at_dollar);
}
void* locate_resourcelist_member(const char* reslist_classname /* example:"idDeclEntityDef" */, const char* member_name, bool end_at_dollar) {
	void* reslist = resourcelist_for_classname(reslist_classname);
	if (!reslist) {
		idLib::Printf("Failed to locate resourcelist %s\n", reslist_classname);
		return nullptr;

	}
	return locate_resourcelist_member_from_idResourceList(reslist, member_name, end_at_dollar);
}

static mh_new_fieldcached_t<void*, YS("idResource"), YS("resourceListPtr")> g_resource_resourceListPtr;

void* get_resourceList_t_containing_resource(void* resource) {
	if (!resource)
		return nullptr;

	return *g_resource_resourceListPtr(resource);
}
void* spawn_entity_from_entitydef(void* entdef) {
	/*
		pray this vftbl displ never changes
	*/
	unsigned VFTBLIDX_GAMELOCAL_SPAWN = 0x40 / 8;
	//ewwww

	void* gamelocal = get_gamelocal();

	void* gamelocal_vftbl = *(void**)gamelocal;

	void* spawn_func = reinterpret_cast<void**>(gamelocal_vftbl)[VFTBLIDX_GAMELOCAL_SPAWN];

	void* resulting_entity = call_as<void*>(spawn_func, gamelocal, entdef, 0xFFFFFFFF, 0x1FFFFFE, -1);

	return resulting_entity;

}
//for 0 arg events
idEventArg g_null_eventargs;
MH_LEAF
idEventArg mh_ScriptCmdEnt(idEventDef* MH_NOESCAPE tdef_name, void* MH_NOESCAPE self, idEventArg* MH_NOESCAPE args) {

	if (args == nullptr) {
		args = &g_null_eventargs;
	}
	idEventArg result;
	call_as<void>(descan::g_eventreceiver_processeventargs, self, &result, tdef_name, args);
	return result;

}
//directly calls vftbl offset +0x30 on entity, which seems to be CallEvent on all versions
//this does not call the notice event though for the event passed in, unlike processeventargs, so only use
//for ones with no notice event
MH_LEAF
idEventArg mh_ScriptCmdEntFast(idEventDef* MH_NOESCAPE tdef_name, void* MH_NOESCAPE self, idEventArg* MH_NOESCAPE args) {
	if (args == nullptr) {
		args = &g_null_eventargs;
	}
	idEventArg result;
	call_virtual<void>(self, VTBLOFFS_CALLEVENT, &result, tdef_name, args);
	return result;
}
MH_LEAF
idEventArg mh_ScriptCmdEnt(const char* MH_NOESCAPE eventdef_name, void* MH_NOESCAPE self, idEventArg* MH_NOESCAPE args) {

	return mh_ScriptCmdEnt(idEventDefInterfaceLocal::Singleton()->FindEvent(eventdef_name), self, args);
}
MH_LEAF
idEventArg mh_ScriptCmdEnt_idEntity(idEventDef* MH_NOESCAPE tdef_name, void* MH_NOESCAPE self, idEventArg* MH_NOESCAPE args ) {
	void* callev = VTBL_MEMBER(idEntity, VTBLOFFS_CALLEVENT)::Get();
	idEventArg result;
	call_as<void>(callev,self, &result, tdef_name, args);
	return result;
}
MH_LEAF
void mh_ScriptCmdEnt_idEntity(idEventDef* MH_NOESCAPE tdef_name, void* MH_NOESCAPE self, idEventArg* MH_NOESCAPE args, idEventArg* MH_NOESCAPE out_arg) {
	void* callev = VTBL_MEMBER(idEntity, VTBLOFFS_CALLEVENT)::Get();

	call_as<void>(callev, self, out_arg, tdef_name, args);
}
/*
	now handles noclip and notarget
*/
void toggle_idplayer_boolean(void* player, const char* property_name, bool use_explicit_value, bool explicit_value) {

	auto headattr = idType::FindClassField("idPlayer", property_name);
	if (!headattr) {
		idLib::Printf("Couldn't locate the %s property on idPlayer, tell chrispy.\n", property_name);
		return;
	}
	int field_value;

	if (!use_explicit_value) {
		field_value = get_classfield_boolean(player, headattr);

		field_value = !field_value;
	}
	else {
		field_value = explicit_value;
	}
	if (field_value) {
		idLib::Printf("Turning %s on.\n", property_name);
	}
	else
		idLib::Printf("Disabling %s\n", property_name);
	set_classfield_boolean(player, headattr, field_value);

}
static cs_uninit_t<idEventArg> g_sink_eventarg_ret{};

MH_LEAF
void mh_ScriptCmdEnt_idEntity_void(idEventDef* MH_NOESCAPE tdef_name, void* MH_NOESCAPE self, idEventArg* MH_NOESCAPE args) {

	void* callev = VTBL_MEMBER(idEntity, VTBLOFFS_CALLEVENT)::Get();
	call_as<void>(callev, self, &g_sink_eventarg_ret, tdef_name, args);
}


void set_entity_position(void* entity, idVec3* pos) {
#if 0
	idEventArg teleargs[2];
	teleargs[0].make_vec3(pos);
	idAngles nothin{ .0f, .0f, .0f };
	teleargs[1].make_angles(&nothin);
	ev_teleport(entity, teleargs);
#else
	idEventArg posa;
	posa.make_vec3(pos);
	ev_setWorldOrigin(entity, &posa);

#endif
}

void get_entity_position(void* entity, idVec3* pos) {
	*pos = ev_getWorldOrigin(entity).value.v_vec3;
}
void** get_class_vtbl(std::string_view clsname) {

	auto iter = g_str_to_rrti_type_descr.find(clsname);

	if (iter == g_str_to_rrti_type_descr.end())
		return nullptr;


	return reinterpret_cast<void**>(iter->second);
}

static int located_decl_read_prod_file = -1;

static unsigned g_gameLocal_entitytbl_offset = 0;

MH_NOINLINE 
MH_REGFREE_CALL
CS_COLD_CODE
static void init_entity_table_offset() {

	void** tablechecker = (void**)get_gamelocal();

	if (!tablechecker)
		return;
	//dont use get_local_player here, it might depend on the entity table offset
	void* player1 = find_entity("player1");
	void* world = find_entity("world");
	if (!player1|| !world)
		return;


	//16382 = world
	//0 = player1

	//search within bounds of gamelocal in old v1 idb.
	//hopefully the size doesnt grow hugely
	for (unsigned i = 0; i < 0x1FF9C0 / 8; ++i) {

		if (tablechecker[i] == player1) {

			/*for (unsigned j = 16380; j < 25; ++j) {
				if (tablechecker[i + j] == world) {
					idLib::Printf("world is located at %u\n", j);
				}
			}*/

			if (tablechecker[i + 16382] == world) {
				g_gameLocal_entitytbl_offset = (i * 8);
				break;
			}
		}
	}

}

void** get_entity_table() {
	MH_UNLIKELY_IF(!g_gameLocal_entitytbl_offset) {
		init_entity_table_offset();
		MH_UNLIKELY_IF(!g_gameLocal_entitytbl_offset)//if cant get the offset yet, return nul and let caller handle 
			return nullptr;
	}
	return mh_lea<void*>(get_gamelocal(), g_gameLocal_entitytbl_offset);

}

int* get_entity_spawnid_table() {

	//spawnid table is always after entity table, has been since at least wolf
	return reinterpret_cast<int*>(&get_entity_table()[16383]);
}

void* lookup_entity_index(unsigned idx) {
	return get_entity_table()[idx];
}

void remove_entity(void* entity) {
	ev_remove(entity);
}

//clang unrolls the loop lol
#pragma clang optimize off
static int init_decl_read_prod_file() {

	//the first mismatch on the vftbl is the decl reload function
	//smort, amirite?
	void** vftblphys = get_class_vtbl(".?AVidDeclPhysics@@");
	void** decltypeinfo = get_class_vtbl(".?AVidDeclTypeInfo@@");

	for (unsigned i = 9; i < 20; ++i) {
		if (vftblphys[i] != decltypeinfo[i]) {
			return i;
		}
	}
	return -1;
}
#pragma clang optimize on
static void* g_idmaterial2_idResourceList = nullptr;
static void* g_idmaterial2_resourceList_t = nullptr;

CS_NOINLINE
CS_COLD_CODE
MH_REGFREE_CALL
static void init_idmaterial2_resourcelist_info() {
	g_idmaterial2_idResourceList = resourcelist_for_classname("idMaterial2");
	g_idmaterial2_resourceList_t = idResourceList_to_resourceList_t(g_idmaterial2_idResourceList);

}
static void* g_idimage_idResourceList = nullptr;
static void* g_idimage_resourceList_t = nullptr;
CS_NOINLINE
CS_COLD_CODE
MH_REGFREE_CALL
static void init_idimage_resourcelist_info() {
	g_idimage_idResourceList = resourcelist_for_classname("idImage");
	g_idimage_resourceList_t = idResourceList_to_resourceList_t(g_idimage_idResourceList);

}

static mh_fieldcached_t<unsigned> g_idMaterial2_CompositeMaterialStreamingPacket{};
static mh_fieldcached_t<unsigned> g_idMaterial2_ImageStreamingPacket{};
static mh_fieldcached_t<unsigned> g_idMaterial2_SingleMaterialStreamingPacket{};


bool reload_decl(void* decl_ptr) {

	if (located_decl_read_prod_file < 0) {
		located_decl_read_prod_file = init_decl_read_prod_file();
		if (located_decl_read_prod_file < 0) {
			idLib::Printf("could not find required function for decl reloading!\n");
			return false;
		}
	}
	
	MH_UNLIKELY_IF(!g_idmaterial2_idResourceList) {
		init_idmaterial2_resourcelist_info();
		init_idimage_resourcelist_info();
	}

	void* resourcelist_for = resourcelist_for_resource(decl_ptr);

	if(resourcelist_for == g_idmaterial2_resourceList_t) {
		*g_idMaterial2_CompositeMaterialStreamingPacket(decl_ptr,"idMaterial2", "CompositeMaterialStreamingPacket") = 0;
		*g_idMaterial2_ImageStreamingPacket(decl_ptr,"idMaterial2", "ImageStreamingPacket") = 0;
		*g_idMaterial2_SingleMaterialStreamingPacket(decl_ptr,"idMaterial2", "SingleMaterialStreamingPacket") = 0;
	}
	else if(resourcelist_for == g_idimage_resourceList_t) {
		*mh_lea<unsigned>(decl_ptr, 0xA0) = 0;
	}
	
	idStr tmpstr;

	auto callfn = reinterpret_cast<bool (*)(void*, idStr*)>(reinterpret_cast<void***>(decl_ptr)[0][located_decl_read_prod_file]);
	if (callfn(decl_ptr, &tmpstr)) {
		idLib::Printf("Failed to reload decl : error was %s\n", tmpstr.data);
		return false;
	}
	return true;
}
static mh_fieldcached_t<void*> g_cvarsystem_field{};
MH_NOINLINE
void* get_cvarsystem() {

	void* engine = get_engine();


	return *g_cvarsystem_field(engine, "engine_t", "cvarSystemForTransfer");

}



static bool g_gameapihooks_installed = false;

void install_gameapi_hooks() {
	if (g_gameapihooks_installed)
		return;

	g_gameapihooks_installed = true;

	//mh_gui::install_gui_hooks();
	mh_mainloop::install_mainloop_hooks();
	mh_input::install_input_hooks();
}

void* get_material(const char* name) {
	return locate_resourcelist_member("idMaterial2", name);
}
struct __declspec(align(8)) idImageOpts
{
	int textureType;
	int format;
	int dword8;
	int width;
	int height;
	int dword14;
	int dword18;
	int numLevels;
	int dword20;
	int dword24;
	int dword28;
	short word2C;
	char byte2E;
	char byte2F;
	int dword30;
	int dword34;
	int somevkflags;
	const char* imagename_i_think;
};

void upload_2d_imagedata(const char* imagename, const void* picdata, unsigned width, unsigned height) {
	
	void* image = locate_resourcelist_member("idImage", imagename, true);
	if(!image)
		return;


	call_as<void>(descan::g_idImage_SubImageUpload, image, 0, 0, 0, 0, 0, width, height, 1, picdata, 0);

}

void* get_entity_typeinfo_object(void* ent) {
	//GetType technically takes the this pointer as an arg, but all of them just lea rax and return anyway without referencing rcx
	return call_virtual<void*>(ent, 0x16);
}

static mh_new_fieldcached_t<idVec3, YS("idBloatedEntity"), YS("clipModelInfo"), YS("size")> g_clipmodel_size{};
static mh_new_fieldcached_t<idVec3, YS("idBloatedEntity"), YS("renderModelInfo"), YS("scale")> g_new_field_renderscale{};

void set_object_scale(void* ent, idVec3 newscale) {
	cs_uninit_array_t<idEventArg, 6> args{};
	args[0].make_float(newscale.x);
	args[1].make_float(newscale.y);
	args[2].make_float(newscale.z);
	args[3].make_int(0);
	args[4].make_int(0);
	args[5].make_int(0);
	ev_lerpRenderScale(ent, &args[0]);

	//*g_clipmodel_size(ent) = newscale;

}

idVec3 get_object_scale(void* ent) {

	return *g_new_field_renderscale(ent);
}

idVec3 get_object_color(void* ent) {
	

	cs_uninit_t<idEventArg> res;

	mh_ScriptCmdEnt_idEntity(ev_getColor.Get(), ent, nullptr, &res);


	return res->value.v_vec3;

}


void set_object_color(void* ent, idVec3 newcolor) {

	cs_uninit_array_t<idEventArg, 3> args;
	args[0].make_float(newcolor.x);
	args[1].make_float(newcolor.y);
	args[2].make_float(newcolor.z);
	mh_ScriptCmdEnt_idEntity_void(ev_setColor.Get(), ent, &args[0]);
}
#if 0
void __fastcall ScriptCmd(__int64 rcx0, idEntity* ent, const idCmdArgs* scriptargs)
{
	bool v3; // cc
	const char* eventname; // r12
	const char* v6; // rdx
	idEventDef* Event; // rax
	idEventDef* v8; // rdi
	char* formatspec; // rbx
	unsigned int v10; // er15
	int v11; // er13
	int v12; // edx
	int v13; // er14
	char* v14; // rax
	__int64 v15; // r9
	int v16; // ebx
	__int64 v17; // rdi
	enumTypeInfo_t* EnumInfo; // rax
	char* v19; // r8
	const enumValueInfo_t* EnumValueInfo; // rax
	char* v21; // rcx
	const char* v22; // rcx
	char* v23; // rcx
	float v24; // xmm1_4
	char** v25; // r14
	__int64 j; // r15
	char* v27; // rcx
	float v28; // xmm1_4
	const char** v29; // r14
	__int64 i; // r15
	const char* v31; // rcx
	float v32; // xmm1_4
	const char* v33; // rax
	char* v34; // rdx
	void* EntityByName; // rax
	unsigned int as_entity_index; // eax
	__int64 v37; // rbx
	void* DeclType; // rax
	char* data; // rdx
	const idDecl* v40; // rax
	int v41; // [rsp+20h] [rbp-E0h]
	float v42[3]; // [rsp+28h] [rbp-D8h] BYREF
	int v43; // [rsp+38h] [rbp-C8h]
	unsigned int v44; // [rsp+3Ch] [rbp-C4h]
	idEventArg* v45; // [rsp+40h] [rbp-C0h]
	char* v46; // [rsp+48h] [rbp-B8h]
	idEventDef* v48; // [rsp+58h] [rbp-A8h]
	char* v49; // [rsp+60h] [rbp-A0h]
	idEventArg a1; // [rsp+70h] [rbp-90h] BYREF
	std::string v52; // [rsp+A0h] [rbp-60h] BYREF
	std::string typname; // [rsp+D0h] [rbp-30h] BYREF
	std::string v54; // [rsp+100h] [rbp+0h] BYREF
	idEventArg args[8]; // [rsp+130h] [rbp+30h] BYREF

	v3 = scriptargs->argc <= 0;
	eventname = blankstr;
	if (v3)
		v6 = blankstr;
	else
		v6 = scriptargs->argv[0];
	
	Event = idEventDefInterfaceLocal::Singleton()->FindEvent( v6);
	v48 = Event;
	v8 = Event;
	if (!Event)
	{
		if (scriptargs->argc > 1)
			eventname = scriptargs->argv[1];
		idLib::Warning("ai_ScriptCmd unknown event '%s'", eventname);
		return;
	}
	formatspec = Event->formatspec;
	v49 = formatspec;
	v10 = 0;
	v11 = 1;
	v44 = 0;
	if (!*formatspec)
		goto process_eventargs_and_return;
	v12 = 4;
	v46 = formatspec;
	v41 = 4;
	v45 = args;
	v13 = 2;
	v14 = formatspec;
	while (2)
	{
		v15 = (unsigned int)*v14;
		*(uint64_t*)&v16 = v11;
		v17 = v11;
		switch (*v14)
		{
		case '1':
		case '2':
		case '5':
		case 's':
			if (v13 > scriptargs->argc)
			{
				idLib::Warning("ai_ScriptCmd parm %d expects %d values for %c", v10, 1i64, v15);
				v12 = v41;
			}
			++v11;
			++v13;
			v41 = v12 + 1;
			if (v16 < 0 || v16 >= scriptargs->argc)
				v33 = blankstr;
			else
				v33 = scriptargs->argv[v17];
			a1.value.i_64 = (uint64_t)v33;
			a1.type = 's';
			args[v10].Copy(&a1);
			v45->type = *v46;
			goto LABEL_79;
		case 'a':
			if (v12 > scriptargs->argc)
			{
				idLib::Warning("ai_ScriptCmd parm %d expects %d values for %c", v10, 3i64, v15);
				v12 = v41;
			}
			memset(v42, 0, sizeof(v42));
			v43 = v13 + 3;
			v41 = v12 + 3;
			v11 += 3;
			v29 = (const char**) & scriptargs->argv[v17];
			for (i = 0; i < 3; ++i)
			{
				if (v17 < 0 || v16 >= scriptargs->argc)
					v31 = blankstr;
				else
					v31 = *v29;
				++v16;
				v32 = atof(v31);
				++v17;
				++v29;
				v42[i] = v32;
			}
			a1.type = 97;
			goto LABEL_50;
		case 'b':
			if (v13 > scriptargs->argc)
			{
				idLib::Warning("ai_ScriptCmd parm %d expects %d values for %c", v10, 1i64, v15);
				v12 = v41;
			}
			++v11;
			++v13;
			v41 = v12 + 1;
			if (v16 < 0 || v16 >= scriptargs->argc)
				v22 = blankstr;
			else
				v22 = scriptargs->argv[v17];
			a1.type = 'i';
			*(uint64_t*)&a1.value.i = atoi(v22) != 0;
			goto LABEL_78;
		case 'd':
			if (v11 + 1 > scriptargs->argc)
				idLib::Warning("ai_ScriptCmd parm %d expects %d values for %c", v10, 1i64, (unsigned int)v49[v10]);

			if (v11 >= 0 && v11 < scriptargs->argc)
				eventname = scriptargs->argv[v11];
			std::string a12;
			a12 = eventname;
			v37 = (int)idStr::Find(a12.data(), 58, 0, -1);



			idStr::Append(&v54, (char*)a1.value.s, v37);
			idStr::Append(&v52, (char*)(v37 + a1.value.i_64 + 1), LODWORD(a1.value.v[2]) - (int)v37);
			if (!v52.len)
			{
				idLib::Warning("ai_ScriptCmd decl parm expects declType:declName");
				return;
			}
			DeclType = idDeclManagerLocal::GetDeclType(engine.declManager, v54.data);
			if (DeclType)
			{
				data = v52.data;
				if (v52.data)
				{
					v40 = idDeclInfo::FindWithInheritance(DeclType, v52.data, 1);
					if (v40)
					{
						a1.value.d = v40;
						a1.type = 'd';
						idEventArg::idEventArg(&args[v10], &a1);
						return;
					}
					data = v52.data;
				}
				idLib::Warning("ai_ScriptCmd decl '%s' not found", data);
			}
			else
			{
				idLib::Warning("ai_ScriptCmd decl type '%s' not found", v54.data);
			}
			return;
		case 'e':
			if (v13 > scriptargs->argc)
			{
				idLib::Warning("ai_ScriptCmd parm %d expects %d values for %c", v10, 1i64, v15);
				v12 = v41;
			}
			++v11;
			++v13;
			v41 = v12 + 1;
			if (v16 < 0 || v16 >= scriptargs->argc)
				v34 = blankstr;
			else
				v34 = scriptargs->argv[v17];
			EntityByName = idGameLocal::FindEntityByName(gameLocal, v34);
			if (!EntityByName)
			{                               // failed to get entity by name, try parsing the arg as an entity index instead
				as_entity_index = atoi(scriptargs->argv[v17]);
				if (as_entity_index > 0x3FFF)
					EntityByName = 0i64;
				else
					EntityByName = idEntity::CastTo(gameLocal->entities_list[as_entity_index]);
			}
			idEventArg::idEventArg(&a1, EntityByName);
			goto LABEL_78;
		case 'f':
			if (v13 > scriptargs->argc)
			{
				idLib::Warning("ai_ScriptCmd parm %d expects %d values for %c", v10, 1i64, v15);
				v12 = v41;
			}
			++v11;
			++v13;
			v41 = v12 + 1;
			if (v16 < 0 || v16 >= scriptargs->argc)
				v23 = blankstr;
			else
				v23 = scriptargs->argv[v17];
			a1.type = 'f';
			v24 = atof(v23);
			a1.value.f = v24;
			goto LABEL_78;
		case 'i':
			if (v13 > scriptargs->argc)
			{
				idLib::Warning("ai_ScriptCmd parm %d expects %d values for %c", v10, 1i64, v15);
				v12 = v41;
			}
			++v11;
			v41 = v12 + 1;
			++v13;
			if (v48->GetArgTypeName( v10, &typname) && operator!=(&typname, "int"))
			{
				EnumInfo = idTypeInfoTools::FindEnumInfo(rcx0, typname.data);
				if (!EnumInfo)
				{
					idLib::Warning("no enum %s", typname.data);
					return;
				}
				if (v16 < 0 || v16 >= scriptargs->argc)
					v19 = blankstr;
				else
					v19 = scriptargs->argv[v17];
				EnumValueInfo = idTypeInfoTools::FindEnumValueInfo((idTypeInfoTools*)rcx0, EnumInfo, v19, 0);
				if (!EnumValueInfo)
				{
					if (v16 >= 0 && v16 < scriptargs->argc)
						eventname = scriptargs->argv[*(_QWORD*)&v16];
					idLib::Warning("no enum or value for %s::%s", typname.data, eventname);
					return;
				}
				a1.value.i_64 = EnumValueInfo->value;
				a1.type = 'i';
			}
			else
			{
				if (v16 < 0 || v16 >= scriptargs->argc)
					v21 = blankstr;
				else
					v21 = scriptargs->argv[v17];
				a1.value.i_64 = atoi(v21);
				a1.type = 'i';
			}
		LABEL_78:
			idEventArg::idEventArg(&args[v10], &a1);
		LABEL_79:
			v12 = v41;
		LABEL_80:
			++v10;
			++v45;
			v14 = v46 + 1;
			v44 = v10;
			v46 = v14;
			if (*v14)
				continue;
			v8 = v48;
		process_eventargs_and_return:
			idEventReceiver::ProcessEventArgPtr(ent, &a1, v8, args);
			return;
		case 'v':
			if (v12 > scriptargs->argc)
			{
				idLib::Warning("ai_ScriptCmd parm %d expects %d values for %c", v10, 3i64, v15);
				v12 = v41;
			}
			memset(v42, 0, sizeof(v42));
			v43 = v13 + 3;
			v41 = v12 + 3;
			v11 += 3;
			v25 = &scriptargs->argv[v17];
			for (j = 0i64; j < 3; ++j)
			{
				if (v17 < 0 || v16 >= scriptargs->argc)
					v27 = blankstr;
				else
					v27 = *v25;
				++v16;
				v28 = atof(v27);
				++v17;
				++v25;
				v42[j] = v28;
			}
			a1.type = 'v';
		LABEL_50:
			v10 = v44;
			*(_QWORD*)a1.value.v = *(_QWORD*)v42;
			a1.value.v[2] = v42[2];
			idEventArg::idEventArg(&args[v44], &a1);
			v13 = v43;
			goto LABEL_79;
		case 'x':
			idLib::Warning("ai_ScriptCmd var args not supported from console right now");
			return;
		default:
			goto LABEL_80;
		}
	}
}
#endif


bool is_entity_of_class(void* ent, const char* cname) {
	cs_uninit_t<idEventArg> result;

	cs_uninit_t<idEventArg> namearg;
	namearg->make_string(cname);
	mh_ScriptCmdEnt_idEntity(ev_isClass.Get(), ent, &namearg, &result);
	return (result->value.i & 0xff) != 0;
}
CACHED_EVENTDEF(getDebugTarget);

CACHED_EVENTDEF(setDebugTarget);

void* get_debug_target() {

	return ev_getDebugTarget(get_world()).value.er;
}
void set_debug_target(void* newtarget) {

	idEventArg rg;
	rg.make_entity(newtarget);
	ev_setDebugTarget(get_world(), &rg);
}