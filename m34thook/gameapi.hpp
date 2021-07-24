#pragma once

#include "idmath.hpp"
void* find_entity(const char* name);
/*
	iterate over all entities with a given classname
	returns null when done
*/
void* find_next_entity_with_class(const char* classname, void* entity_iterator=nullptr);

void* get_level_map();

bool get_classfield_boolean(void* obj, const classVariableInfo_t* varinfo);
bool get_classfield_boolean(void* obj, const char* clazs, const char* field);
void set_classfield_boolean(void* obj, const classVariableInfo_t* varinfo, bool value);

long long get_classfield_int(void* obj, const classVariableInfo_t* varinfo);
long long get_classfield_int(void* obj, const char* clazs, const char* field);

const char* get_entity_name(void* obj);
//get the pointer to the global engine_t instance
/*
	we already have a pointer to a sub field in it (our memorysystem pointer). the full struct on v1 looks like this
		asyncio::idExecutor* asyncIoExecutor
	idBroadcastManager* broadcastManager
	idBuildVersionInfo* buildVersionInfo
	idCheatManager* cheatManager
	idConsole* console
	idConsoleHistory* consoleHistory
	idCloud* cloud
	idDeclManager* declManager
	idDeclTracking* declTracking
	idEngineVersionInfo* engineVersionInfo
	idHttpManager* httpManager
	idImGuiHost* imguiHost
	idJobProcessor* jobProcessor
	idWatchdogTimer* watchdogTimer
	idThreadCallbackManager* mainThreadCallback
	idPlayerProfileInterface* playerProfileManager
	idPsnWebApiManager* psnWebApiManager
	idRemoteMonitor* remoteMonitor
	idResourceCompression* resourceCompression
	idResourceMetaDataObjectFactory* resourceMetaDataObjectFactory
	idResourceStorageInterface* resourceStorageInterface
	idResourceStorageGeneratorInterface* resourceStorageGeneratorInterface
	idResourceStreamFileSpec* resourceStreamFileSpec
	idResourceStreamFileTracker* resourceStreamFileTracker
	idResourceTracking* resourceTracking
	idRSSimpleExplicitBuilderFactory* rSSimpleExplicitBuilderFactory
	idRuntimeConfiguration* runtimeConfiguration
	idSoundSystem* soundSystem
	idStreamDecoder* streamDecoder
	idStreamingInstallManager* streamingInstallManager
	idStreamIo* streamIo
	idSystemInterface* systemInterface
	idTimeManager* timeManager
	idClockManager* clockManager
	idStopWatchManager* stopWatchManager
	idTypeInfoTools* typeInfoTools
	idAnimation* animation
	idBinaryVirtualCache* binaryVirtualCache
	idEditorInterface* editorInterface
	idDeclGlobalFontTable* globalFontTable
	idGlobalTextureManager* globalTextureManager
	idLightmapManager* lightmapManager
	idRenderLayerManager* renderLayerManager
	idRenderSystem* renderSystem
	idResolutionScale* resolutionScale
	idStaticModelManager* staticModelManager
	idWorldGeometryManager* worldGeometryManager
	idCloudAssets* cloudAssets
	libGlobals_t* idlibGlobals
	libInterface_t* libInterface
	idInput* input
	idImageManager* globalImages
	idSnapshotTemplateManager* snapshotTemplateManager
	idSnapshotMetrics* snapshotMetrics
	idEventDefInterface* eventDefInterface
	idTitleStorage* titleStorage
	idForgeCrashReporter* forgeCrashReporter
	idReportData* reportData
	idTIVExport* tivExport
	idDebugHUD* debugHUD
	idCollisionModelManager* collisionModelManager
	idRadPreview* lightmapPreview
	idRecast* autoNav
	idRenderManager* renderManager
	engineTimeInfo_t* engineTimeInfo
	idGeomCacheStreamer* geomCacheStreamer
	idGeometryStreamer* geometryStreamer
	idLightProbeStreamer* lightProbeStreamer
	idGlobalParmStateManager* globalParmStateManager
	idCompileStatusHandler* compileStatusHandler
	Win32Vars_t* win32
	idImageStreamer* imageStreamer
	idStreamerDatabaseIo* streamerDatabaseIo
	idLogicEnvironment* logicEnvironment
	idUsercmdGen* usercmdGen
	idExternalDeclEditor* externalDeclEditor
	idFlightNavCompiler* flightNavCompiler
	idSignInManager* signInManager
	idAchievementSystem* achievementSystem
	idCursor* cursor
	idFirstPartyPlatform* firstparty

	//The following members are just within this struct to transfer the values for the shared globals to the game. Since the ownership of those instances might change, we just implement them here for the time being.
	idMem* memorySystemForTransfer
	idCVarSystem* cvarSystemForTransfer
	idCmdSystem* cmdSystemForTransfer
	idFileSystem* fileSystemForTransfer
	idProfileManager* profileManagerForTransfer

*/
MH_NOINLINE
char* get_engine();

MH_NOINLINE
void* get_cvarsystem();
MH_NOINLINE
void* get_eventdef_interface();
MH_NOINLINE
void* get_rendersystem();

MH_NOINLINE
void* get_cursor();


MH_NOINLINE
void* get_editor_interface();




/*
	get rendermodelgui for idConsole
*/
/*MH_NOINLINE
idRenderModelGui get_console_rendermodel();
*/

MH_NOINLINE
idRenderModelGui* get_cursor_rendermodel();


void add_persistent_text(unsigned x, unsigned y, unsigned RGBA, float scale,  const char* fmt, ...);

void remove_persistent_text(unsigned x, unsigned y);

void* get_player_look_target();


struct idSpawnId {
	int handle;
};
struct idManagedClassPtr;
struct idManagedClass;

struct idManagedClassPtr
{
  idSpawnId spawnId;
  idSpawnId serializedSpawnId;
  idManagedClass *ptr;
  idManagedClassPtr *next;
  idManagedClassPtr *prev;
};



void* get_entity_physics_obj(void* ent);

//wrap all rtti signature references in this just in case they change in the future
#define		RTTISIG(...)			__VA_ARGS__
using rttisig_ref_t = const char*;
//fuck, doesnt work
bool is_subclass_of_rttisig(void* obj, rttisig_ref_t rsig);

void get_player_trace_pos(idVec3* outvec);

void* locate_resourcelist_member(const char* reslist_classname /* example:"idDeclEntityDef" */, const char* member_name);

void* spawn_entity_from_entitydef(void* entdef);

void set_entity_position(void* entity, idVec3* pos);

void** get_class_vtbl(std::string_view clsname);

bool reload_decl(void* decl_ptr );