SCANNED_PTR_FEATURE(g_alloca_probe)
SCANNED_PTR_FEATURE(g_security_cookie)
SCANNED_PTR_FEATURE(g_security_check_cookie)
SCANNED_PTR_FEATURE(g_msvc_typeinfo_vtbl)
SCANNED_PTR_FEATURE(g_doom_memorysystem)
SCANNED_PTR_FEATURE(g_sqrtf)
SCANNED_PTR_FEATURE(g_sqrt)
SCANNED_PTR_FEATURE(g_idfilecompressed_getfile)
SCANNED_PTR_FEATURE(g_doom_operator_new)
SCANNED_PTR_FEATURE(g_idfilememory_ctor)
SCANNED_PTR_FEATURE(g_idoodle_decompress)
SCANNED_PTR_FEATURE(g_idcmdsystem)
SCANNED_PTR_FEATURE(g_idcvarsystem)
SCANNED_PTR_FEATURE(g_idlib_vprintf)
SCANNED_PTR_FEATURE(g_idstr_hash)

SCANNED_PTR_FEATURE(g_idtypeinfo_findclassinfo)
SCANNED_PTR_FEATURE(g_idtypeinfo_findenuminfo)

SCANNED_PTR_FEATURE(g_gamelib_initialize_ptr)

SCANNED_PTR_FEATURE(g_idstr__idstr)
SCANNED_PTR_FEATURE(g_idTypeInfoFile_readstr)
SCANNED_PTR_FEATURE(g_resourcelist_for_classname)
SCANNED_PTR_FEATURE(g__ZN5idStr4IcmpEPKcS1_)
SCANNED_PTR_FEATURE(g_listOfResourceLists)
SCANNED_PTR_FEATURE(g_gamelocal)
SCANNED_PTR_FEATURE(g_idgamesystemlocal)

SCANNED_PTR_FEATURE(g_serialize_type_to_json)
SCANNED_PTR_FEATURE(g_deserialize_type_from_json)
SCANNED_PTR_FEATURE(g_atomic_string_set)

SCANNED_PTR_FEATURE(g_declentitydef_setentitystate)
SCANNED_PTR_FEATURE(g_declentitydef_gettextwithinheritance)
SCANNED_PTR_FEATURE(g_idstr_dctor)
SCANNED_PTR_FEATURE(g_idstr_assign_charptr)
SCANNED_PTR_FEATURE(g_noclip_func)
SCANNED_PTR_FEATURE(g_global_typeinfo_tools)
SCANNED_PTR_FEATURE(g_maplocal_getlevelmap)
SCANNED_PTR_FEATURE(g_idmapfile_write)

SCANNED_PTR_FEATURE(g_idlib_fatalerror)
SCANNED_PTR_FEATURE(g_idlib_error)

SCANNED_PTR_FEATURE(g_idfilesystemlocal)
SCANNED_PTR_FEATURE(g_levelreload)
SCANNED_PTR_FEATURE(g_init_func_rva_992170)
SCANNED_PTR_FEATURE(g_find_next_entity_with_class)
	//actually expects resourceList_t*, not idResourceList
SCANNED_PTR_FEATURE(g_resourcelist_index)
SCANNED_PTR_FEATURE(g_rtti_typeinfo_string)

//pointer to resourceManager
SCANNED_PTR_FEATURE(g_resourceManager2)

//for resourceList_t, not idResourceList. use idResourceList typeinfo to get to resourceList_t
SCANNED_UINT_FEATURE(g_offset_resourcelist_length)

SCANNED_UINT_FEATURE(g_vftbl_offset_getlevelmap)

SCANNED_PTR_FEATURE(g_resourceStorageDiskStreamer_GetFile)

SCANNED_PTR_FEATURE(g_resourceStorageInterface2)

SCANNED_PTR_FEATURE(g_idImage_SubImageUpload)

SCANNED_PTR_FEATURE(g_idRenderModelGui__DrawString)
//?DrawStretchPic@idRenderModelGui@@QEAAXMMMMMMMMMPEBVidMaterial@@@Z
SCANNED_PTR_FEATURE(g_idRenderModelGui__DrawStretchPic)

//_ZN16idRenderModelGui14DrawStretchPicERK6idVec4S2_S2_S2_PK10idMaterialf
SCANNED_PTR_FEATURE(g_idRenderModelGui__DrawStretchPicVec4Version)
SCANNED_PTR_FEATURE(g_idRenderModelGui__DrawChar)

SCANNED_PTR_FEATURE(g_idRenderModelGui_AllocTris)

SCANNED_PTR_FEATURE(g_idCommonLocal_Frame)
SCANNED_PTR_FEATURE(g_idCommonLocal_Frame_CallbackPtr)
SCANNED_PTR_FEATURE(g_idResourceList_Add)
SCANNED_PTR_FEATURE(g_idImageManager_ScratchImage)
SCANNED_PTR_FEATURE(g_idcolor_packcolor)
SCANNED_PTR_FEATURE(g_idRenderModelGui_SetViewPort)
//pointer to an int, which is the offset to vertexcolor on idRenderModelGui
SCANNED_PTR_FEATURE(g_idRenderModelGui_VertexColorOffsPtr)

SCANNED_PTR_FEATURE(g_idRenderModelStatic_FinishStaticModel)


SCANNED_PTR_FEATURE(g_idStaticModel_ctor)
SCANNED_PTR_FEATURE(g_idStaticModel_MakeTexturedCube)

SCANNED_PTR_FEATURE(g_SMALLCHAR_HEIGHT)
SCANNED_PTR_FEATURE(g_SMALLCHAR_WIDTH)
SCANNED_PTR_FEATURE(g_idRenderModelGui_GetVirtualWidth)
SCANNED_PTR_FEATURE(g_idRenderModelGui_GetVirtualHeight)
SCANNED_PTR_FEATURE(g_renderSystem2)

SCANNED_PTR_FEATURE(g_idStaticModel_FreeSurfaces)
SCANNED_UINT_FEATURE(g_idTriangles_sizeof)
SCANNED_PTR_FEATURE(g_idTriangles_ctor)
SCANNED_PTR_FEATURE(g_idTriangles_AllocStaticTriSurfVerts)
SCANNED_PTR_FEATURE(g_idTriangles_AllocStaticTriSurfIndexes)

SCANNED_PTR_FEATURE(g_idTriangles_AddCubeFace)

SCANNED_PTR_FEATURE(g_idStaticModel_UpdateBuffers)
SCANNED_PTR_FEATURE(g_idStaticModel_FreeCPUData)