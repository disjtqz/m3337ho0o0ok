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
#include "gameapi.hpp"



//needed for argument 2 of placementcreateinstance
static unsigned g_sizeof_idbloatedentity = 0;

static void* g_bloatedentity_vtbl = nullptr;
static idTypeInfo* g_idbloatedentity_typeinfo = nullptr;
static idclass_placement_create_func_t g_idbloatedentity_placementcreate = nullptr;


//static idEventArg* NewEntitycallevent(void* ent, idEventArg* retval, idEventDef* event, idEventArg* args);

//static void* g_original_bloatedentity_callevent = (void*)NewEntitycallevent;
#define		MAX_CALLEVENT_REPL_MEM		(64 * 1024)


unsigned g_exemem_pos = 0;

static char* exemem_base = 0;

idEventDef* gwo = nullptr;
idEventDef* swo = nullptr;

unsigned g_evid_xchgpos = 0;
static idEventArg* NewEntitycallevent(void* ev, idEventArg* retval, idEventDef* event, idEventArg* args, void* original_method) {


	if (event->eventnum == g_evid_xchgpos) {
		retval->type = 0;

		idVec3 mypos;
		idVec3 targpos;

		

		void* otherentity = find_entity(args[0].value.s);


		
		get_entity_position(ev, &mypos);
		get_entity_position(otherentity, &targpos);

		set_entity_position(ev, &targpos);
		set_entity_position(otherentity, &mypos);

		return retval;

	}

	return call_as<idEventArg*>(original_method, ev, retval, event, args);
}







static void replace_unused_events() {
	auto evi = idEventDefInterfaceLocal::Singleton();
	idEventDef* old_fadesound = evi->FindEvent("testFunctionality");

	old_fadesound->argNames = "target;";
	old_fadesound->argTypes = "char*;";
	old_fadesound->argDefaultValues = ";";
	old_fadesound->formatspec = "s";
	old_fadesound->numargs = 1;
	old_fadesound->name = "mh_xchgpos";
	old_fadesound->returnType = 0;
	g_evid_xchgpos = old_fadesound->eventnum;
}


/*
		sub     rsp, 56                             ; 00000038H
		lea     rax, OFFSET FLAT:void * NewEntitycallevent3(void *,void *,void *,void *) ; NewEntitycallevent3
		mov     QWORD PTR [rsp+32], rax
		call    void * NewEntitycallevent(void *,void *,void *,void *,void *)   ; NewEntitycallevent
		add     rsp, 56                             ; 00000038H
		ret     0
*/
static void build_new_callevent(void* vtbl) {
	void* orig_method = *mh_lea<void*>(vtbl, VTBLOFFS_CALLEVENT);


	Xbyak::CodeGenerator cgen{};
	cgen.sub(cgen.rsp, 56);
	
	//cgen.push((uintptr_t)orig_method);
	cgen.mov(cgen.rax, (uintptr_t)orig_method);

	cgen.mov(cgen.qword[cgen.rsp + 32], cgen.rax);

	cgen.mov(cgen.rax, (uintptr_t)(void*)NewEntitycallevent);

	cgen.call(cgen.rax);
	cgen.add(cgen.rsp, 56);
	cgen.ret();


	char* newptr = &exemem_base[g_exemem_pos];

	memcpy(newptr, cgen.getCode(), cgen.getSize());

	g_exemem_pos += cgen.getSize();

	swap_out_ptrs(mh_lea<void*>(vtbl, VTBLOFFS_CALLEVENT), (void**) & newptr, 1, true);


}



void init_new_entity_type_stuff() {
#if 0
	exemem_base = (char*)sh::vmem::allocate_rwx(MAX_CALLEVENT_REPL_MEM);

	g_bloatedentity_vtbl = get_class_vtbl(".?AVidBloatedEntity@@");
	void* lol = *mh_lea<void*>(g_bloatedentity_vtbl, VTBLOFFS_IDCLASS_GETTYPE);

	//pass nullptr for this, because GetType never references this
	g_idbloatedentity_typeinfo = call_as<idTypeInfo*>(lol, nullptr);
	g_sizeof_idbloatedentity = g_idbloatedentity_typeinfo->size;
	g_idbloatedentity_placementcreate = g_idbloatedentity_typeinfo->PlacementCreateInstance;



	//swap_out_ptrs(mh_lea<void>(g_bloatedentity_vtbl, VTBLOFFS_CALLEVENT), &g_original_bloatedentity_callevent, 1);
	
	auto inheritors = idType::get_entity_inheritors();

	for (auto&& inher : *inheritors) {
		idTypeInfo* curr = from_de_rva<idTypeInfo>(inher);

		char namebuf[512];
		sprintf_s(namebuf, ".?AV%s@@", curr->classname);

		void* vtbl = get_class_vtbl(namebuf);
		if (!vtbl) {
			continue;
		}
		build_new_callevent(vtbl);
	}

	replace_unused_events();

	gwo = ev_getWorldOrigin.Get();
	swo = ev_setWorldOrigin.Get();
#endif
	//idType::get_entity_inheritors
}