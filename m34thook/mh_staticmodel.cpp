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
#include "memorySystem.hpp"
#include "mh_staticmodel.hpp"

mh_typesizecached_t<yuckystring_m("idStaticModel")> g_idStaticModelSize{};


static mh_new_fieldcached_t<idListVoid, YS("idStaticModel"), YS("surfaces")> g_model_surfs;
/*
struct idStaticModelSurface
{
  idMaterial2 *material;
  idArray<idTriangles*,5> geometry;
  int materialNum;
  char geometryIsReference;
  char field_35[3];
};

*/
static mh_new_fieldcached_t<void*, YS("idStaticModelSurface"), YS("material")> g_model_material;
static mh_new_fieldcached_t<char, YS("idStaticModelSurface"), YS("geometryIsReference")> g_model_geometryIsReference;
static mh_new_fieldcached_t<int, YS("idStaticModelSurface"), YS("materialNum")> g_model_materialNum;

static mh_new_fieldcached_t<void*, YS("idStaticModelSurface"), YS("geometry")> g_model_geometry;


mh_typesizecached_t<yuckystring_m("idStaticModelSurface")> g_idStaticModelSurfaceSize{};

idStaticModelPtr idStaticModelPtr::CreateNew() {

	void* instance = idNew(g_idStaticModelSize());

	call_as<void>(descan::g_idStaticModel_ctor, instance);

	idStaticModelPtr result;
	result.m_ptr = instance;
	return result;
	

}

void idStaticModelPtr::Write(const char* output_path, bool skip_compression) {
	call_as<void>(descan::g_idStaticModel_WriteStaticBModel, m_ptr, output_path, 0, skip_compression);
}


void idStaticModelPtr::Setup_Singlesurface_hack(unsigned num_verts, unsigned num_tris, idDrawVert** out_verts, unsigned short** out_tris) {


	auto old_freetrival = cvar_data(cvr_r_freeTriangleCPUData)->valueInteger;


	cvar_data(cvr_r_freeTriangleCPUData)->valueInteger = 0;

	//try not doing this hack and see if we avoid segfault
#if 0
	call_as<void>(descan::g_idStaticModel_MakeTexturedCube, m_ptr, 10.0f, 10.0f);
#else

	void* tri = idNew(descan::g_idTriangles_sizeof);
	idListVoid* surfptr = g_model_surfs(m_ptr);
	call_as<void>(descan::g_idTriangles_ctor, tri);
	surfptr->list = (unsigned long long*) idNew(g_idStaticModelSurfaceSize() * 32);
	memset(surfptr->list, 0, g_idStaticModelSurfaceSize() * 32);

	surfptr->size = 32;
	surfptr->num = 1;
	void* current_surf = surfptr->list;

	
	*g_model_material(current_surf) = locate_resourcelist_member("idMaterial2", "_default");
	
	*g_model_geometry(current_surf) = tri;
	*g_model_geometryIsReference(current_surf) = 0;
	//idk?
	*g_model_materialNum(current_surf) = 0;
#endif
	cvar_data(cvr_r_freeTriangleCPUData)->valueInteger = old_freetrival;

	idListVoid* surfptr2 = g_model_surfs(m_ptr);


	//classTypeInfo_t* modelsurf_type = idType::FindClassInfo("idStaticModelSurface");


	//get first element of geometry array in first element of our surface array
	void* current_idtriangles = *g_model_geometry(surfptr2->list);


	*out_verts = call_as<idDrawVert*>(descan::g_idTriangles_AllocStaticTriSurfVerts, current_idtriangles, num_verts);
	*out_tris = call_as<unsigned short*>(descan::g_idTriangles_AllocStaticTriSurfIndexes, current_idtriangles, num_tris);
	/*
	.text:0000000140878CC6 83 83 F8 00 00 00 04                    add     dword ptr [rbx+248], 4
.text:0000000140878CCD                         ; 87:     a1->numIndexes += 6;
.text:0000000140878CCD 83 83 F4 00 00 00 06                    add     dword ptr [rbx+244], 6
	*/
	*mh_lea<int>(current_idtriangles, 0xF8) = num_verts;
	*mh_lea<int>(current_idtriangles, 0xF4) = num_tris;
}

void idStaticModelPtr::Finalize_geo() {
	
	call_as<void>(descan::g_idRenderModelStatic_FinishStaticModel, m_ptr, 1u, 1, 1);
	call_as<void>((void*)0x140645400, m_ptr, 0, 0);
}

void idStaticModelPtr::Destroy() {
	//lol we probably leak memory here since we're not actually calling the staticmodel dctor
	call_as<void>(descan::g_idStaticModel_FreeSurfaces, m_ptr);
	idDelete(m_ptr);
}

void idStaticModelPtr::make_standard_tris() {
	idListVoid* surfptr2 = g_model_surfs(m_ptr);
	void* current_idtriangles = *g_model_geometry(surfptr2->list);

	*g_model_geometry(surfptr2->list) = call_as<void*>((void*)0x14087C000, current_idtriangles);
}