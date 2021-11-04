#pragma once

#include "idmath.hpp"
void* find_entity(const char* name);
/*
	iterate over all entities with a given classname
	returns null when done
*/
void* find_next_entity_with_class(const char* classname, void* entity_iterator = nullptr);

void* get_level_map();

bool get_classfield_boolean(void* obj, const classVariableInfo_t* varinfo);
bool get_classfield_boolean(void* obj, const char* clazs, const char* field);
void set_classfield_boolean(void* obj, const classVariableInfo_t* varinfo, bool value);

long long get_classfield_int(void* obj, const classVariableInfo_t* varinfo);
long long get_classfield_int(void* obj, const char* clazs, const char* field);

const char* get_entity_name(void* obj);

void* get_material(const char* name);

//warning: returns idResourceList, not resourceList_t!
void* resourcelist_for_classname(const char* clsname);
//warning:returns resourceList_t, not idResourceList
void* resourcelist_for_resource(void* resource);

void* idResourceList_to_resourceList_t(void* resourcelist);

unsigned resourceList_t_get_length(void* reslist);

void* resourceList_t_lookup_index(void* reslist, unsigned idx);
const char* get_resource_name(void* resource);
struct idEventDef;
#if 0
#define		RENDERMODELGUI_VERTEXCOLOR_OFFSET		1200
#else
#define		RENDERMODELGUI_VERTEXCOLOR_OFFSET		(*reinterpret_cast<unsigned*>(descan::g_idRenderModelGui_VertexColorOffsPtr))
#endif
struct idRenderModelGui {
	void DrawFilled(const idColor& color, float x, float y, float w, float h);
	void DrawStretchPic(
		float x,
		float y,
		float z,
		float w,
		float h,
		float s1,
		float t1,
		float s2,
		float t2,
		const void* material);
	void DrawStretchPic(
		const idVec4* topLeft,
		const idVec4* topRight,
		const idVec4* bottomRight,
		const idVec4* bottomLeft,
		const void* material,
		float z);
	//extension

	MH_NOINLINE
		void DrawRectMaterial(float xstart, float ystart, float width, float height, void* material);
	MH_NOINLINE
		void DrawRectMaterial(float xstart, float ystart, float width, float height, const char* material);

	/*
		use non-temporal writes to write out the verts
	*/
	idDrawVert* AllocTris(int numVerts, unsigned short* indexes, int numIndexes, void* material);
	void DrawChar(float x, float y, int character, float scale);
	void DrawString(
		float x,
		float y,
		const char* string,
		const idColor* defaultColor,
		bool forceColor,
		const float scale);
	void SetViewport(int x,
		int y,
		int width,
		int height);
	//added for compat with snaphak version so shingamegui can be ported over
	void set_current_vertexcolor(unsigned vcolor);
	MH_PURE
		static float get_SMALLCHAR_WIDTH();
	MH_PURE
		static float get_SMALLCHAR_HEIGHT();

	float GetVirtualWidth();
	float GetVirtualHeight();
	unsigned GetStringWidth(const char* string, const float scale);

};
struct idDebugHUD;

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
	allocates an entity interface object from the editor interface for this entities type
*/
MH_NOINLINE
void* alloc_entity_interface(void* entity);

void destroy_entity_interface(void* inter);

MH_NOINLINE
void* get_console();

MH_NOINLINE
void* get_globalImages();


/*
	get rendermodelgui for idConsole
*/
/*MH_NOINLINE
idRenderModelGui get_console_rendermodel();
*/

MH_NOINLINE
idRenderModelGui* get_cursor_rendermodel();


void add_persistent_text(unsigned x, unsigned y, unsigned RGBA, float scale, const char* fmt, ...);

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
	idManagedClass* ptr;
	idManagedClassPtr* next;
	idManagedClassPtr* prev;
};



void* get_entity_physics_obj(void* ent);

//wrap all rtti signature references in this just in case they change in the future
#define		RTTISIG(...)			__VA_ARGS__
using rttisig_ref_t = const char*;
//fuck, doesnt work
bool is_subclass_of_rttisig(void* obj, rttisig_ref_t rsig);

void get_player_trace_pos(idVec3* outvec);

void* locate_resourcelist_member(const char* reslist_classname /* example:"idDeclEntityDef" */, const char* member_name, bool end_at_dollar = false);

void* spawn_entity_from_entitydef(void* entdef);

void set_entity_position(void* entity, idVec3* pos);

void** get_class_vtbl(std::string_view clsname);

bool reload_decl(void* decl_ptr);

void install_gameapi_hooks();


void upload_2d_imagedata(const char* imagename, const void* picdata, unsigned width, unsigned height);


union idEventArg_unnamed_type_value
{
	int i;
	float f;
	float v[3];
	float q[4];
	float c[4];
	const char* s;
	const unsigned __int8* x;
	const struct idDecl* d;
	const struct idMD6Anim* anim;
	unsigned int h;
	struct idEventReceiver* er;
	struct idDamageParms* dp;
};

struct __declspec(align(8)) idEventArg
{
	char type;
	char pad[7];
	idEventArg_unnamed_type_value value;

	void __fastcall Copy(

		const idEventArg* other)
	{
		int type; // eax
		const char* s; // rcx
		const char* v6; // rcx

		type = other->type;
		this->type = type;
		switch (type)
		{
		case '1':
		case '2':
		case '4':
		case '5':
		case 'd':
		case 'g':
		case 'i':
		case 'r':
		case 's':
		case 'x':
		case 'y':
			this->value.s = other->value.s;
			break;
		case '3':
			this->value.i = LOWORD(other->value.f);
			break;
		case '8':
			s = this->value.s;
			if (s)
			{
				if (s[8])
					(**(void(__fastcall***)(const char*, __int64))s)(s, 1i64);
			}
			v6 = other->value.s;
			if (v6)
			{
				if (v6[8])//hopefully the number of args here is correct
					v6 = (const char*)(*(__int64(__fastcall**)(const char*))(*(uint64_t*)v6 + 8i64))(v6);
			}
			this->value.s = v6;
			break;
		case 'a':
		case 'v':
			this->value.s = other->value.s;
			this->value.v[2] = other->value.v[2];
			break;
		case 'c':
			this->value = other->value;
			break;
		case 'e':
			this->value.s = (const char*)other->value.i;
			break;
		case 'f':
		case 'm':
		case 'w':
			this->value.i = other->value.i;
			break;
		case 'q':
			this->value = other->value;
			break;
		default:
			return;
		}
	}
	~idEventArg()
	{
		const char* s; // rcx

		if (this->type == 56)
		{
			s = this->value.s;
			if (s)
			{
				if (s[8])
					(**(void(__fastcall***)(const char*, __int64))s)(s, 1i64);
			}
			this->value.s = 0;
			this->type = 0;
		}
		else
		{
			this->value.s = 0;
			this->type = 0;
		}
	}

	idEventArg() : type(0) {

	}

	idEventArg(const idEventArg& a) {
		Copy(&a);
	}
	//call ctor for identity
	void make_entity(void* entity) {
		call_as<void>(descan::g_eventarg_ctor_identityptr, this, entity);

	}

	void make_vec3(idVec3* p) {
		type = 'v';
		value.v[0] = p->x;
		value.v[1] = p->y;
		value.v[2] = p->z;

	}
	void make_angles(idAngles* aa) {



		type = 'a';
		value.v[0] = aa->yaw;
		value.v[1] = aa->pitch;
		value.v[2] = aa->roll;
	}

};
//i expect the reader is more familiar with ai_ScriptCmdEnt than they are with ProcessEventArgs, so this name was chosen
//with the ability to post events like this you have an insane amount of power. i recommend writing some code
//to iterate through all of the eventdefs on the evdef interface and generate boilerplate code that A: at startup grabs all the eventdef pointers
//into global variables and then B: generates boilerplate c++ code with the proper types to call all of the eventdefs
//also using the typeinfo api and declinfo stuff you can reimplement the ai_scriptcmdent command but thats an exercise for the reader
idEventArg mh_ScriptCmdEnt(idEventDef* tdef_name, void* self, idEventArg* args);

idEventArg mh_ScriptCmdEnt(const char* eventdef_name, void* self, idEventArg* args);