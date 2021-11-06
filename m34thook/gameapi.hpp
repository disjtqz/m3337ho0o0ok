#pragma once

#include "idmath.hpp"

#define	WORLD_ENTITY_IDX		16382
#define	LAST_PLAYER_IDX			12

#define	VTBLOFFS_CALLEVENT		0x30
void* find_entity(const char* name);
/*
	iterate over all entities with a given classname
	returns null when done
*/
void* find_next_entity_with_class(const char* classname, void* entity_iterator = nullptr);
void* get_local_player();
void* get_level_map();

void* get_world();
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

int* get_entity_spawnid_table();

void** get_entity_table();
//0-16384
void* lookup_entity_index(unsigned idx);

static void* get_gamelocal() {

	return *reinterpret_cast<void**>(descan::g_gamelocal);
}
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
//returns idtypeinfo for entity
void* get_entity_typeinfo_object(void* ent);

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

void get_entity_position(void* entity, idVec3* pos);


void kill_entity(void* entity);
void remove_entity(void* entity);

void** get_class_vtbl(std::string_view clsname);


template<typename TYuckyStr>
class tmpl_vftbl_reference_t{
	static inline constexpr std::string_view name = yuckystring_str_from_type_m(TYuckyStr);
	static inline void** vtbl = nullptr;

	MH_NOINLINE
	MH_REGFREE_CALL
	CS_COLD_CODE
	static void init_vtbl() {

		vtbl = get_class_vtbl(name);
	}
public:
	MH_PURE
	static inline void** Get() {
		MH_UNLIKELY_IF(!vtbl) {

			init_vtbl();

		}
		return vtbl;
	}
};


template<typename vtbl_ref, unsigned offset>
class tmpl_vtbl_member_t {
	static inline void* member = nullptr;
	MH_NOINLINE
	MH_REGFREE_CALL
	CS_COLD_CODE
	static void init_vtbl_member() {

		member = vtbl_ref::Get()[offset / 8];

	}

public:
	MH_PURE
	static inline void* Get() {
		MH_UNLIKELY_IF(!member) {

			init_vtbl_member();

		}
		return member;
	}
};

#define	CACHED_SIMPLE_VTBL(name)		using vtb_##name = tmpl_vftbl_reference_t<yuckystring_m(".?AV" #name "@@")>

CACHED_SIMPLE_VTBL(idEntity);



#define	VTBL_MEMBER(clsname, memberoffset)		tmpl_vtbl_member_t<vtb_##clsname, memberoffset>




bool reload_decl(void* decl_ptr);

void install_gameapi_hooks();


void upload_2d_imagedata(const char* imagename, const void* picdata, unsigned width, unsigned height);


union idEventArg_unnamed_type_value
{
	int i;
	float f;
	float v[3];
	idVec3 v_vec3;

	float q[4];
	float c[4];
	uint64_t i_64;//doesnt actually exist in game code, is for decompiled stuff
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

	void make_string(const char* s) {
		this->type = 's';
		this->value.s = s;

	}

	void make_bool(bool b) {

		this->type = 'b';
		this->value.i = b ? 1 : 0;
	}
	void make_angles(idAngles* aa) {



		type = 'a';
		value.v[0] = aa->yaw;
		value.v[1] = aa->pitch;
		value.v[2] = aa->roll;
	}

	void make_decl(struct idDecl* d) {
		this->type = 'd';
		this->value.d = d;

	}

};
//i expect the reader is more familiar with ai_ScriptCmdEnt than they are with ProcessEventArgs, so this name was chosen
//with the ability to post events like this you have an insane amount of power. i recommend writing some code
//to iterate through all of the eventdefs on the evdef interface and generate boilerplate code that A: at startup grabs all the eventdef pointers
//into global variables and then B: generates boilerplate c++ code with the proper types to call all of the eventdefs
//also using the typeinfo api and declinfo stuff you can reimplement the ai_scriptcmdent command but thats an exercise for the reader
idEventArg mh_ScriptCmdEnt(idEventDef* tdef_name, void* self, idEventArg* args = nullptr);

idEventArg mh_ScriptCmdEnt(const char* eventdef_name, void* self, idEventArg* args = nullptr);


idEventArg mh_ScriptCmdEntFast(idEventDef* tdef_name, void* self, idEventArg* args = nullptr);
//directly calls idEntity::CallEvent from the idEntity vtbl instead of dispatching to self's method
//can be much faster for classes that have a deep inheritance chain and is safe if the event is only implemented by idEntity
idEventArg mh_ScriptCmdEnt_idEntity(idEventDef* tdef_name, void* self, idEventArg* args = nullptr);

void mh_ScriptCmdEnt_idEntity_void(idEventDef* tdef_name, void* self, idEventArg* args = nullptr);
template<typename TYuckyStr>
struct cached_eventdef_t {
	static inline idEventDef* m_def;

	static inline constexpr const char* name = yuckystring_str_from_type_m(TYuckyStr);
	MH_REGFREE_CALL
		MH_NOINLINE
		CS_COLD_CODE
		static void init_eventdef() {
		m_def = idEventDefInterfaceLocal::Singleton()->FindEvent(name);

	}
	MH_PURE
	static idEventDef* Get() {

		MH_UNLIKELY_IF(!m_def) {
			init_eventdef();
		}
		return m_def;
	}

	idEventArg operator()(void* self, idEventArg* args = nullptr) const {

		return mh_ScriptCmdEnt(Get(), self, args);
	}

};



#define		CACHED_EVENTDEF(name)		static constexpr cached_eventdef_t<yuckystring_m(#name)>  ev_##name {};

/*
enum evnum_t
{
  EVID_remove = 0x0,
  EVID_ae_forceVoiceOver = 0x1,
  EVID_ae_hideRenderModel = 0x2,
  EVID_ae_hideRenderModelWithFXControl = 0x3,
  EVID_ae_showRenderModel = 0x4,
  EVID_ae_remove = 0x5,
  EVID_ae_removeTime = 0x6,
  EVID_ae_activateTarget = 0x7,
  EVID_ae_activateTargetPlayer = 0x8,
  EVID_removeSaveGate = 0x9,
  EVID_poolInterface_PostSpawn = 0xA,
  EVID_postSpawnComponents = 0xB,
  EVID_updateGoreComponent = 0xC,
  EVID_startFX = 0xD,
  EVID_stopFX = 0xE,
  EVID_stopAllFX = 0xF,
  EVID_spawnStatIncrease = 0x10,
  EVID_dropEntity = 0x11,
  EVID_findEntity = 0x12,
  EVID_getName = 0x13,
  EVID_setName = 0x14,
  EVID_activate = 0x15,
  EVID_activatedThrough = 0x16,
  EVID_activateTargets = 0x17,
  EVID_makeActivatable = 0x18,
  EVID_becomeActive = 0x19,
  EVID_becomeInactive = 0x1A,
  EVID_numTargets = 0x1B,
  EVID_getTarget = 0x1C,
  EVID_removeTarget = 0x1D,
  EVID_addTarget = 0x1E,
  EVID_randomTarget = 0x1F,
  EVID_bind = 0x20,
  EVID_bindPosition = 0x21,
  EVID_bindToJoint = 0x22,
  EVID_bindToJointFlags = 0x23,
  EVID_fakeAnimMethod = 0x24,
  EVID_unbind = 0x25,
  EVID_removeBinds = 0x26,
  EVID_setOwner = 0x27,
  EVID_setModel = 0x28,
  EVID_getModel = 0x29,
  EVID_setContents = 0x2A,
  EVID_getContents = 0x2B,
  EVID_addContentsFlag = 0x2C,
  EVID_removeContentsFlag = 0x2D,
  EVID_setClipMask = 0x2E,
  EVID_getClipMask = 0x2F,
  EVID_addClipMaskFlag = 0x30,
  EVID_removeClipMaskFlag = 0x31,
  EVID_setShaderParms = 0x32,
  EVID_setColorAndAlpha = 0x33,
  EVID_setColor = 0x34,
  EVID_getColor = 0x35,
  EVID_isHidden = 0x36,
  EVID_hide = 0x37,
  EVID_show = 0x38,
  EVID_hideRenderModel = 0x39,
  EVID_showRenderModel = 0x3A,
  EVID_startSoundShader = 0x3B,
  EVID_stopSound = 0x3C,
  EVID_fadeSound = 0x3D,
  EVID_fadePitch = 0x3E,
  EVID_playVoiceOver = 0x3F,
  EVID_startSound = 0x40,
  EVID_setHighlight = 0x41,
  EVID_getWorldOrigin = 0x42,
  EVID_setWorldOrigin = 0x43,
  EVID_getOrigin = 0x44,
  EVID_setOrigin = 0x45,
  EVID_localToWorldForOffset = 0x46,
  EVID_getAngles = 0x47,
  EVID_getRenderModelAngles = 0x48,
  EVID_setAngles = 0x49,
  EVID_getModelForward = 0x4A,
  EVID_getSpawnAxisForward = 0x4B,
  EVID_setLinearVelocity = 0x4C,
  EVID_getLinearVelocity = 0x4D,
  EVID_setAngularVelocity = 0x4E,
  EVID_getAngularVelocity = 0x4F,
  EVID_getSize = 0x50,
  EVID_getMins = 0x51,
  EVID_getMaxs = 0x52,
  EVID_touches = 0x53,
  EVID_restorePosition = 0x54,
  EVID_setSpawnPosition = 0x55,
  EVID_distanceTo = 0x56,
  EVID_distanceToPoint = 0x57,
  EVID_angleTo = 0x58,
  EVID_setCanBecomeDormant = 0x59,
  EVID_teleport = 0x5A,
  EVID_clampAngles = 0x5B,
  EVID_testFunctionality = 0x5C,
  EVID_spawnBind = 0x5D,
  EVID_getClass = 0x5E,
  EVID_isClass = 0x5F,
  EVID_isSpecificClass = 0x60,
  EVID_setRenderParm = 0x61,
  EVID_lerpRenderParm = 0x62,
  EVID_lerpRenderParmFrom = 0x63,
  EVID_lerpRenderScale = 0x64,
  EVID_setTakesDamage = 0x65,
  EVID_break = 0x66,
  EVID_giveItem = 0x67,
  EVID_damage = 0x68,
  EVID_setGravity = 0x69,
  EVID_applyImpulse = 0x6A,
  EVID_notifyOfCollision = 0x6B,
  EVID_getClosestPointOnLine = 0x6C,
  EVID_getBindMaster = 0x6D,
  EVID_getBindParent = 0x6E,
  EVID_isProdBuilding = 0x6F,
  EVID_midnightStarted = 0x70,
  EVID_midnightInterrupted = 0x71,
  EVID_midnightFinished = 0x72,
  EVID_midnightSkipped = 0x73,
  EVID_midnightFailed = 0x74,
  EVID_addAIEvent = 0x75,
  EVID_touch = 0x76,
  EVID_untouch = 0x77,
  EVID_numEntitiesInVolume = 0x78,
  EVID_numEntitiesInVolumeByName = 0x79,
  EVID_numAIsInVolume = 0x7A,
  EVID_numPlayersInVolume = 0x7B,
  EVID_isEntityInVolume = 0x7C,
  EVID_volumeMemberAdded = 0x7D,
  EVID_volumeMemberRemoved = 0x7E,
  EVID_bossVoiceOverDamageLinePlayed = 0x7F,
  EVID_notice_Suicide = 0x80,
  EVID_notice_DemonSuicide = 0x81,
  EVID_notice_TeamKill = 0x82,
  EVID_notice_AIKill = 0x83,
  EVID_notice_AIAssist = 0x84,
  EVID_notice_PlayerKill = 0x85,
  EVID_notice_DemonKill = 0x86,
  EVID_notice_GloryKill = 0x87,
  EVID_notice_LeaderKill = 0x88,
  EVID_notice_PlayerDeath = 0x89,
  EVID_notice_DoubleKill = 0x8A,
  EVID_notice_TripleKill = 0x8B,
  EVID_notice_QuadKill = 0x8C,
  EVID_notice_KillingSpree = 0x8D,
  EVID_notice_Rampage = 0x8E,
  EVID_notice_Dominating = 0x8F,
  EVID_notice_XpGained = 0x90,
  EVID_notice_PlayerPointCapture = 0x91,
  EVID_notice_DamageToAIEnemy = 0x92,
  EVID_notice_PlayerDamageTaken = 0x93,
  EVID_notice_DemonPlayerDamageTaken = 0x94,
  EVID_notice_Revive = 0x95,
  EVID_notice_Incapacitated = 0x96,
  EVID_notice_StealthAIKill = 0x97,
  EVID_notice_ComboAIKill = 0x98,
  EVID_notice_DefendAIKill = 0x99,
  EVID_notice_Impressive = 0x9A,
  EVID_notice_Revenge = 0x9B,
  EVID_notice_FirstBlood = 0x9C,
  EVID_notice_Defense = 0x9D,
  EVID_notice_Offense = 0x9E,
  EVID_notice_PlayerPointCaptureOwnership = 0x9F,
  EVID_notice_PlayerPointCaptureExtracted = 0xA0,
  EVID_notice_AirMail = 0xA1,
  EVID_notice_Pull = 0xA2,
  EVID_notice_Afterlife = 0xA3,
  EVID_notice_Denial = 0xA4,
  EVID_notice_CloseCall = 0xA5,
  EVID_notice_PlayerRevived = 0xA6,
  EVID_notice_PlayerIncapacitated = 0xA7,
  EVID_notice_HeadShotKill = 0xA8,
  EVID_notice_ThawedTeammate = 0xA9,
  EVID_notice_PointCapture = 0xAA,
  EVID_getJointHandle = 0xAB,
  EVID_hideShowMesh = 0xAC,
  EVID_addProxyAnimator = 0xAD,
  EVID_animWeb_SetBlendVar = 0xAE,
  EVID_detachAndRemove = 0xAF,
  EVID_removeCopyTarget = 0xB0,
  EVID_ae_setOutlineVisible = 0xB1,
  EVID_ae_default = 0xB2,
  EVID_ae_sound = 0xB3,
  EVID_ae_soundTagTracking = 0xB4,
  EVID_ae_soundNoSkip = 0xB5,
  EVID_ae_stopSound = 0xB6,
  EVID_ae_soundMusic = 0xB7,
  EVID_ae_pitchedSound = 0xB8,
  EVID_ae_soundWeapon = 0xB9,
  EVID_ae_soundVoice = 0xBA,
  EVID_ae_soundVoice2 = 0xBB,
  EVID_ae_soundVoiceHighPrio = 0xBC,
  EVID_ae_soundBody = 0xBD,
  EVID_ae_soundBody2 = 0xBE,
  EVID_ae_soundBody3 = 0xBF,
  EVID_ae_soundItem = 0xC0,
  EVID_ae_playAIVO = 0xC1,
  EVID_ae_playAIVO_CheckContext = 0xC2,
  EVID_ae_aIEvent = 0xC3,
  EVID_ae_detachEntity = 0xC4,
  EVID_ae_detachEntityLater = 0xC5,
  EVID_ae_disableClip = 0xC6,
  EVID_ae_attachEntity = 0xC7,
  EVID_ae_spawnEntityAtTag = 0xC8,
  EVID_ae_setAllowHzChange = 0xC9,
  EVID_ae_declParticle = 0xCA,
  EVID_ae_startFX = 0xCB,
  EVID_ae_startFXCustomization = 0xCC,
  EVID_ae_stopFX = 0xCD,
  EVID_ae_stopFXCustomization = 0xCE,
  EVID_ae_trigger = 0xCF,
  EVID_ae_damage = 0xD0,
  EVID_ae_damageEntity = 0xD1,
  EVID_ae_addBloodSplatter = 0xD2,
  EVID_ae_addBloodPool = 0xD3,
  EVID_ae_damagePlayer = 0xD4,
  EVID_ae_radiusDamage = 0xD5,
  EVID_ae_arcDamage = 0xD6,
  EVID_ae_addPlayerWhiplash = 0xD7,
  EVID_ae_genericApex = 0xD8,
  EVID_ae_pause = 0xD9,
  EVID_ae_aFStart = 0xDA,
  EVID_ae_aFBeginSync = 0xDB,
  EVID_ae_startBlendOutOfSync = 0xDC,
  EVID_ae_setControllerShake = 0xDD,
  EVID_ae_forceChangeState = 0xDE,
  EVID_ae_hideMesh = 0xDF,
  EVID_ae_showMesh = 0xE0,
  EVID_ae_showKit = 0xE1,
  EVID_ae_setMaterial = 0xE2,
  EVID_ae_launchProjectileAtTarget = 0xE3,
  EVID_ae_fireWeaponFromDecl = 0xE4,
  EVID_ae_startDOF = 0xE5,
  EVID_ae_endDOF = 0xE6,
  EVID_ae_clearDOF = 0xE7,
  EVID_ae_startBlur = 0xE8,
  EVID_ae_endBlur = 0xE9,
  EVID_ae_addUICallout = 0xEA,
  EVID_ae_removeUICallout = 0xEB,
  EVID_ae_updateEchoProjectorPosition = 0xEC,
  EVID_ae_setEchoOpacity = 0xED,
  EVID_ae_fadeEchoOpacity = 0xEE,
  EVID_ae_glitchEcho = 0xEF,
  EVID_ae_setAchievement = 0xF0,
  EVID_ae_clearContents = 0xF1,
  EVID_ae_spawnBreakable = 0xF2,
  EVID_ae_lerpToFov = 0xF3,
  EVID_ae_bindToTag = 0xF4,
  EVID_ae_showEntity = 0xF5,
  EVID_ae_showTargetEntity = 0xF6,
  EVID_ae_hideEntity = 0xF7,
  EVID_ae_hudHide = 0xF8,
  EVID_ae_hudShow = 0xF9,
  EVID_ae_reticleHide = 0xFA,
  EVID_ae_reticleShow = 0xFB,
  EVID_ae_hudHideElements = 0xFC,
  EVID_ae_hudPlayBootSequence = 0xFD,
  EVID_ae_breakConstraint = 0xFE,
  EVID_ae_breakPoseConstraint = 0xFF,
  EVID_breakConstraint = 0x100,
  EVID_ae_disableTwoLegIKBlend = 0x101,
  EVID_ae_enableTwoLegIKBlend = 0x102,
  EVID_ae_detonateSelf = 0x103,
  EVID_changeAnimState = 0x104,
  EVID_changeAnimStateVia = 0x105,
  EVID_forceAnimState = 0x106,
  EVID_changeAnimStateVia2 = 0x107,
  EVID_changeAnimState2 = 0x108,
  EVID_animState_IsFinished = 0x109,
  EVID_setBlendScalar = 0x10A,
  EVID_startVoiceOver = 0x10B,
  EVID_ae_voiceOver = 0x10C,
  EVID_ae_holsterItem = 0x10D,
  EVID_ae_equipItem = 0x10E,
  EVID_ae_setStage = 0x10F,
  EVID_ae_itemStartFX = 0x110,
  EVID_ae_itemStopFX = 0x111,
  EVID_ae_goreEnableByName = 0x112,
  EVID_ae_triggerAttachments = 0x113,
  EVID_ae_playerControllerShake = 0x114,
  EVID_postSpawn = 0x115,
  EVID_triggerStuff = 0x116,
  EVID_startTouch = 0x117,
  EVID_facing = 0x118,
  EVID_enable = 0x119,
  EVID_disable = 0x11A,
  EVID_toggleEnableState = 0x11B,
  EVID_getUserData = 0x11C,
  EVID_takeDamageReady = 0x11D,
  EVID_getLengthOnSpline = 0x11E,
  EVID_getParentSpline = 0x11F,
  EVID_fadeOut = 0x120,
  EVID_explode = 0x121,
  EVID_explodeParms = 0x122,
  EVID_attach = 0x123,
  EVID_detach = 0x124,
  EVID_physToRest = 0x125,
  EVID_updatePhysPassThrough = 0x126,
  EVID_removeProjectilePhys = 0x127,
  EVID_launch = 0x128,
  EVID_removeAIEvents = 0x129,
  EVID_clientReceivedExplodeState = 0x12A,
  EVID_stopDashFX = 0x12B,
  EVID_notice_EV_EndGloryKill = 0x12C,
  EVID_notice_EV_PropsDroppedNearPlayer = 0x12D,
  EVID_dashDamageBoost = 0x12E,
  EVID_notice_EV_ExplosionSpawn = 0x12F,
  EVID_removeMeteredPips = 0x130,
  EVID_dropsBlocked = 0x131,
  EVID_notice_StartRound = 0x132,
  EVID_animWeb_ActiveStart = 0x133,
  EVID_animWeb_ReactivateDeactivateEnd = 0x134,
  EVID_animWeb_LaunchStart = 0x135,
  EVID_animWeb_LaunchClamped = 0x136,
  EVID_animWeb_LaunchEnd = 0x137,
  EVID_animWeb_DeactivateEnd = 0x138,
  EVID_useDeferredPosition = 0x139,
  EVID_spawnWeaponPieceEmitter = 0x13A,
  EVID_setUseDeferredPosition = 0x13B,
  EVID_resolveFire = 0x13C,
  EVID_netCheckStopFire = 0x13D,
  EVID_deferredFire = 0x13E,
  EVID_notice_EV_StartGloryKill = 0x13F,
  EVID_notice_PlayerVsPlayerKill = 0x140,
  EVID_notice_DamageFromSummonedDemon = 0x141,
  EVID_notice_ModifierEvent = 0x142,
  EVID_sendReliableStopFXGameMsg = 0x143,
  EVID_leftFoot = 0x144,
  EVID_rightFoot = 0x145,
  EVID_numOfItemTypeInInventory = 0x146,
  EVID_giveInventoryItem = 0x147,
  EVID_removeInventoryItem = 0x148,
  EVID_removeAllInventoryItems = 0x149,
  EVID_getInventorySlotCount = 0x14A,
  EVID_getInventoryDeclByIndex = 0x14B,
  EVID_getInventoryDeclClassName = 0x14C,
  EVID_getInventoryDeclMaxCount = 0x14D,
  EVID_getAmmoDeclNameForWeapon = 0x14E,
  EVID_getEquippedWeaponDecl = 0x14F,
  EVID_hideAttachment = 0x150,
  EVID_showAttachment = 0x151,
  EVID_isDead = 0x152,
  EVID_weaponBurstMode = 0x153,
  EVID_getWeaponReadyState = 0x154,
  EVID_increaseHealth = 0x155,
  EVID_decreaseHealth = 0x156,
  EVID_needsHealth = 0x157,
  EVID_pauseWeapon = 0x158,
  EVID_unpauseWeapon = 0x159,
  EVID_gibbed = 0x15A,
  EVID_unequipAndDeleteDroppedItem = 0x15B,
  EVID_ae_leftFoot = 0x15C,
  EVID_ae_rightFoot = 0x15D,
  EVID_ae_leftRearFoot = 0x15E,
  EVID_ae_rightRearFoot = 0x15F,
  EVID_ae_leftHandImpact = 0x160,
  EVID_ae_rightHandImpact = 0x161,
  EVID_ae_land = 0x162,
  EVID_ae_startRagdoll = 0x163,
  EVID_ae_freeAttachments = 0x164,
  EVID_ae_unbindAttachments = 0x165,
  EVID_ae_unbindLastAttachment = 0x166,
  EVID_ae_attachmentsStartFX = 0x167,
  EVID_ae_attachmentsStopFX = 0x168,
  EVID_ae_hideAttachment = 0x169,
  EVID_ae_showAttachment = 0x16A,
  EVID_ae_spawnAttachment = 0x16B,
  EVID_ae_dropWeapons = 0x16C,
  EVID_ae_giveInventoryItem = 0x16D,
  EVID_ae_removeInventoryItem = 0x16E,
  EVID_saveCorpseToMap = 0x16F,
  EVID_ae_goreEnableFullBodyByName = 0x170,
  EVID_ae_dropToGround = 0x171,
  EVID_breakConstraint_TwoImpulses = 0x172,
  EVID_startRagdoll = 0x173,
  EVID_startDeathSmokeFX = 0x174,
  EVID_aFBeginSync = 0x175,
  EVID_goreEnable = 0x176,
  EVID_goreEnableFullBodyByName = 0x177,
  EVID_setupCorpse = 0x178,
  EVID_beginFadeCorpse = 0x179,
  EVID_beginCorpseRemoval = 0x17A,
  EVID_setupCloneGore = 0x17B,
  EVID_handsHitReactions_ReturnedToDefaultState = 0x17C,
  EVID_equipmentLauncherBurstFire = 0x17D,
  EVID_launchEquipmentLauncher = 0x17E,
  EVID_ae_handsSuppressAllAdditiveAnims = 0x17F,
  EVID_ae_handsStartJointMeleeTrace = 0x180,
  EVID_ae_handsStartMeleeTraceSlash = 0x181,
  EVID_ae_handsStartMeleeTraceTag = 0x182,
  EVID_ae_handsEndMeleeTrace = 0x183,
  EVID_ae_startMeleeLunge = 0x184,
  EVID_ae_stopMeleeLunge = 0x185,
  EVID_ae_testGloryKill = 0x186,
  EVID_ae_startAdvancedScreenShake = 0x187,
  EVID_ae_startScreenShake = 0x188,
  EVID_ae_meleeSound = 0x189,
  EVID_ae_soundSetRTPC = 0x18A,
  EVID_ae_soundSetWeaponRTPC = 0x18B,
  EVID_ae_playWeaponLoadedSound = 0x18C,
  EVID_ae_stopWeaponLoadedSound = 0x18D,
  EVID_ae_fireWeaponRight = 0x18E,
  EVID_ae_dryfireWeaponRight = 0x18F,
  EVID_ae_fireWeaponRightPrimaryOnly = 0x190,
  EVID_ae_fireWeaponRightSecondaryOnly = 0x191,
  EVID_ae_fireWeaponRightForceSecondary = 0x192,
  EVID_ae_releaseWeaponTrigger = 0x193,
  EVID_ae_fireWeaponRightBurstShot = 0x194,
  EVID_ae_reloadWeaponRightSavedAmmo = 0x195,
  EVID_ae_reloadWeaponRight = 0x196,
  EVID_ae_reloadWeaponRightForce = 0x197,
  EVID_ae_reloadWeaponFullAmmoRight = 0x198,
  EVID_ae_handsEquipInventoryItem = 0x199,
  EVID_ae_handsUnequipInventoryItem = 0x19A,
  EVID_ae_handsUnequipInventoryItemInSlot = 0x19B,
  EVID_ae_attachTempInventoryItem = 0x19C,
  EVID_ae_detachTempInventoryItem = 0x19D,
  EVID_ae_startThrowItem = 0x19E,
  EVID_ae_attachThrowItem = 0x19F,
  EVID_ae_throwItem = 0x1A0,
  EVID_ae_throwWeapon = 0x1A1,
  EVID_ae_throwThrowItem = 0x1A2,
  EVID_ae_launchEquipmentLauncher = 0x1A3,
  EVID_ae_launchGroundslam = 0x1A4,
  EVID_ae_endThrowItem = 0x1A5,
  EVID_ae_useQueuedEquipmentItem = 0x1A6,
  EVID_ae_dropItem = 0x1A7,
  EVID_ae_hideHandsInstantly = 0x1A8,
  EVID_ae_hideMeshForSlot = 0x1A9,
  EVID_ae_showMeshForSlot = 0x1AA,
  EVID_ae_hudHide_Hands = 0x1AB,
  EVID_ae_hudShow_Hands = 0x1AC,
  EVID_ae_clearItem = 0x1AD,
  EVID_ae_setUpdateItemPosition = 0x1AE,
  EVID_ae_zoomIn = 0x1AF,
  EVID_ae_zoomOut = 0x1B0,
  EVID_ae_zoomInhibitOn = 0x1B1,
  EVID_ae_zoomInhibitOff = 0x1B2,
  EVID_ae_hideInhibitOn = 0x1B3,
  EVID_ae_hideInhibitOff = 0x1B4,
  EVID_ae_interruptTransitionForMelee = 0x1B5,
  EVID_ae_setInterruptible = 0x1B6,
  EVID_ae_setInterruptibleForWeaponSwitch = 0x1B7,
  EVID_ae_setNewModUsable = 0x1B8,
  EVID_ae_forceReadyToFire = 0x1B9,
  EVID_ae_meleeNextStageWindowOpen = 0x1BA,
  EVID_ae_meleeNextStageWindowClose = 0x1BB,
  EVID_ae_equipWeapon = 0x1BC,
  EVID_ae_unequipWeapon = 0x1BD,
  EVID_ae_weaponGUIShow = 0x1BE,
  EVID_ae_rememberCurrentWeaponRight = 0x1BF,
  EVID_ae_equipRememberedWeaponRight = 0x1C0,
  EVID_ae_equipDefaultMeleeWeapon = 0x1C1,
  EVID_ae_unequipDefaultMeleeWeapon = 0x1C2,
  EVID_ae_equipNextWeaponRight = 0x1C3,
  EVID_ae_equipNextWeaponLeft = 0x1C4,
  EVID_ae_equipDoomBlade = 0x1C5,
  EVID_ae_equipDoom5MeleeFists = 0x1C6,
  EVID_ae_hideHandsItem = 0x1C7,
  EVID_ae_startCameraShake = 0x1C8,
  EVID_ae_attachCamera = 0x1C9,
  EVID_ae_detachCamera = 0x1CA,
  EVID_ae_startSlowMotion = 0x1CB,
  EVID_ae_stopSlowMotion = 0x1CC,
  EVID_ae_forceLandingEffect = 0x1CD,
  EVID_ae_additiveAnimStartBlendOut = 0x1CE,
  EVID_ae_startFX_Hands = 0x1CF,
  EVID_ae_stopFX_Hands = 0x1D0,
  EVID_ae_swapPointingHands = 0x1D1,
  EVID_ae_ejectShell = 0x1D2,
  EVID_ae_ejectShell2 = 0x1D3,
  EVID_ae_ejectShellLeft = 0x1D4,
  EVID_ae_ejectShellRight = 0x1D5,
  EVID_ae_ejectShellPrimaryOnly = 0x1D6,
  EVID_ae_ejectShellLeftPrimaryOnly = 0x1D7,
  EVID_ae_ejectShellRightPrimaryOnly = 0x1D8,
  EVID_ae_ejectShellSecondaryOnly = 0x1D9,
  EVID_ae_ejectShellLeftSecondaryOnly = 0x1DA,
  EVID_ae_ejectShellRightSecondaryOnly = 0x1DB,
  EVID_ae_weaponClearCollapsibleJoints = 0x1DC,
  EVID_ae_weaponAnimUpdateAmmo = 0x1DD,
  EVID_ae_weaponAnimFireAmmo = 0x1DE,
  EVID_ae_weaponAnimReloadAmmo = 0x1DF,
  EVID_ae_weaponAnimResetAmmo = 0x1E0,
  EVID_ae_weaponRequiresReload = 0x1E1,
  EVID_ae_weaponThrowIgnoresIsReloading = 0x1E2,
  EVID_ae_weaponSwitchOnEmpty = 0x1E3,
  EVID_ae_chainsawSetState = 0x1E4,
  EVID_ae_quickChainsawSwipeStarted = 0x1E5,
  EVID_ae_quickChainsawSwipeEnded = 0x1E6,
  EVID_ae_chainsawPullCord = 0x1E7,
  EVID_ae_changeHandsFOVScale = 0x1E8,
  EVID_ae_resetHandsFOVScale = 0x1E9,
  EVID_ae_updateSwimHiddenMeshes = 0x1EA,
  EVID_ae_showAllHandsMeshes = 0x1EB,
  EVID_ae_showHandMeshesForRightWeapon = 0x1EC,
  EVID_ae_chainsawSetRevValue = 0x1ED,
  EVID_ae_setControllerRumble = 0x1EE,
  EVID_ae_clearControllerRumble = 0x1EF,
  EVID_ae_clearAllControllerRumble = 0x1F0,
  EVID_ae_setMovementDuringCustomAnim = 0x1F1,
  EVID_ae_setHandsDepthHack = 0x1F2,
  EVID_ae_resetHandsDepthHack = 0x1F3,
  EVID_ae_handsWeaponFireFinished = 0x1F4,
  EVID_ae_blockFireModeChange = 0x1F5,
  EVID_ae_unblockFireModeChange = 0x1F6,
  EVID_animWeb_ThrowItemEnded = 0x1F7,
  EVID_animWeb_MeleeEnded = 0x1F8,
  EVID_animWeb_CrucibleMeleeRightSwingBegun = 0x1F9,
  EVID_animWeb_CrucibleMeleeLeftSwingBegun = 0x1FA,
  EVID_animWeb_CrucibleMeleeSwingEnded = 0x1FB,
  EVID_animWeb_MeleeToShoot = 0x1FC,
  EVID_animWeb_ShootStarted = 0x1FD,
  EVID_animWeb_DryfireEnded = 0x1FE,
  EVID_animWeb_ShootEnded = 0x1FF,
  EVID_animWeb_ShootEndedReloadStarted = 0x200,
  EVID_animWeb_ShootDelayEnded = 0x201,
  EVID_animWeb_ShootEndedReloadEnded = 0x202,
  EVID_animWeb_ChangeWeaponBringdownEnded = 0x203,
  EVID_animWeb_ChangeWeaponEnded = 0x204,
  EVID_animWeb_GenericHideEnded = 0x205,
  EVID_animWeb_ChangeWeaponModStarted = 0x206,
  EVID_animWeb_ShowHiddenMeshes = 0x207,
  EVID_animWeb_ChangeWeaponModEnded = 0x208,
  EVID_animWeb_ChainsawStopCuttingEnded = 0x209,
  EVID_animWeb_CustomAnimEnded = 0x20A,
  EVID_animWeb_ChainsawQuickAnimStarted = 0x20B,
  EVID_animWeb_ChainsawQuickAnimEnded = 0x20C,
  EVID_animWeb_BlendToChargeStarted = 0x20D,
  EVID_animWeb_BlendToChargeEnded = 0x20E,
  EVID_animWeb_BlendFromChargeEnded = 0x20F,
  EVID_animWeb_ShootEndedBlendToChargeEnded = 0x210,
  EVID_ae_setInvulnerability = 0x211,
  EVID_ae_removeInvulnerability = 0x212,
  EVID_resizePool = 0x213,
  EVID_updateVisuals = 0x214,
  EVID_changeCurrentWaterThresholdState = 0x215,
  EVID_setShader = 0x216,
  EVID_setLightParms = 0x217,
  EVID_setRadiusXYZ = 0x218,
  EVID_setRadius = 0x219,
  EVID_on = 0x21A,
  EVID_off = 0x21B,
  EVID_fadeOutLight = 0x21C,
  EVID_fadeInLight = 0x21D,
  EVID_fadeWithTable = 0x21E,
  EVID_postOff = 0x21F,
  EVID_syncEnd = 0x220,
  EVID_syncStart = 0x221,
  EVID_syncChangeState = 0x222,
  EVID_syncBindParticipant = 0x223,
  EVID_isAnimBlending = 0x224,
  EVID_startSlowMotion = 0x225,
  EVID_abortSync = 0x226,
  EVID_cleanUpAnimatedEnity = 0x227,
  EVID_detachCopyTarget = 0x228,
  EVID_ae_syncScaleAnimRate = 0x229,
  EVID_ae_syncRestAnimRate = 0x22A,
  EVID_ae_syncStartWindow = 0x22B,
  EVID_ae_syncStartInput = 0x22C,
  EVID_ae_syncEndInput = 0x22D,
  EVID_ae_syncRandomInputWindow = 0x22E,
  EVID_ae_syncAfterRandomInputWindow = 0x22F,
  EVID_ae_syncForceStateOnLose = 0x230,
  EVID_ae_syncForceStateOnDamage = 0x231,
  EVID_ae_syncForceStateOnDeath = 0x232,
  EVID_ae_syncWindowHealthSpec = 0x233,
  EVID_ae_syncBindParticipant = 0x234,
  EVID_ae_syncEnd = 0x235,
  EVID_ae_syncActivateTarget = 0x236,
  EVID_ae_syncActivateTargetPlayer = 0x237,
  EVID_ae_syncInterpolateToTarget = 0x238,
  EVID_ae_animSyncDeltaCorrectionEndPos = 0x239,
  EVID_ae_teleportParticipantsToTarget = 0x23A,
  EVID_ae_lerpParticipantRenderScale = 0x23B,
  EVID_ae_animSyncStartDeltaCorrection = 0x23C,
  EVID_ae_animSyncBoundsJointReference = 0x23D,
  EVID_ae_gloryKillDamage = 0x23E,
  EVID_ae_syncChangeState = 0x23F,
  EVID_ae_dropFatLoot = 0x240,
  EVID_ae_finalAttackerPosition = 0x241,
  EVID_animWeb_SyncEnd = 0x242,
  EVID_ae_consumeNonGuiInput = 0x243,
  EVID_ae_hideParticipant = 0x244,
  EVID_ae_hideParticipant_ByIndex = 0x245,
  EVID_checkpointReloaded = 0x246,
  EVID_notice_EntityRemoved = 0x247,
  EVID_interactStateEnter = 0x248,
  EVID_forceIntroBringup = 0x249,
  EVID_forceIntroAccentBringup = 0x24A,
  EVID_notice_PropUsed = 0x24B,
  EVID_notice_PlayerRespawn = 0x24C,
  EVID_notice_PersonalTeleporterUsed = 0x24D,
  EVID_notice_EndRound = 0x24E,
  EVID_notice_DemonRunePostShow = 0x24F,
  EVID_notice_DemonPlayerCreated = 0x250,
  EVID_clearDesiredDossierPage = 0x251,
  EVID_skillUpgradeAwarded = 0x252,
  EVID_skipCinematic = 0x253,
  EVID_showArgentSelection = 0x254,
  EVID_showRuneTrialStart = 0x255,
  EVID_showDeathScreen = 0x256,
  EVID_disableEscMenu = 0x257,
  EVID_requestNightmareMarkers = 0x258,
  EVID_throwWeapon = 0x259,
  EVID_requestSuicide = 0x25A,
  EVID_noPlayerDeath = 0x25B,
  EVID_getPlayerHealth = 0x25C,
  EVID_getPlayerMaxHealth = 0x25D,
  EVID_getPlayerShields = 0x25E,
  EVID_getPlayerMaxShields = 0x25F,
  EVID_addCodexEntry = 0x260,
  EVID_touchedBouncePad = 0x261,
  EVID_touchedTeleporter = 0x262,
  EVID_trackEntity = 0x263,
  EVID_getUserButtons = 0x264,
  EVID_getUserMove = 0x265,
  EVID_getUserAngles = 0x266,
  EVID_getViewAngles = 0x267,
  EVID_setViewAngles = 0x268,
  EVID_getViewPos = 0x269,
  EVID_isPlayerOnGround = 0x26A,
  EVID_updateInventoryItems = 0x26B,
  EVID_doesPlayerHavePerk = 0x26C,
  EVID_isPlayerPerkActive = 0x26D,
  EVID_givePlayerPerk = 0x26E,
  EVID_activatePlayerPerk = 0x26F,
  EVID_giveCodex = 0x270,
  EVID_startSpectatingTeammate = 0x271,
  EVID_enableWeapon = 0x272,
  EVID_disableWeapon = 0x273,
  EVID_meleeAttack = 0x274,
  EVID_equipItem = 0x275,
  EVID_scalePlayerDamage = 0x276,
  EVID_setGameStateInt = 0x277,
  EVID_getGameStateInt = 0x278,
  EVID_setDeathTrigger = 0x279,
  EVID_nextMap = 0x27A,
  EVID_devMap = 0x27B,
  EVID_endRevive = 0x27C,
  EVID_resumeLiving = 0x27D,
  EVID_respawn = 0x27E,
  EVID_teleport_TouchTriggers = 0x27F,
  EVID_postTeleportTC = 0x280,
  EVID_postFastTravel = 0x281,
  EVID_reInitHands = 0x282,
  EVID_endFastTravel = 0x283,
  EVID_fastTravelUnlockControls = 0x284,
  EVID_respawnWithFade = 0x285,
  EVID_fadeView = 0x286,
  EVID_fadeOutView = 0x287,
  EVID_setForceInhibitControl = 0x288,
  EVID_enableFPHands = 0x289,
  EVID_enableFPHandsAndHUD = 0x28A,
  EVID_enableFPHandsWithIntroBringup = 0x28B,
  EVID_enableFPHandsWithIntroAccentBringup = 0x28C,
  EVID_disableFPHandsExtraSlow = 0x28D,
  EVID_forcePraetorHandsAndBody = 0x28E,
  EVID_carry_ShowHands = 0x28F,
  EVID_kill = 0x290,
  EVID_doHudInitialFlash = 0x291,
  EVID_endSpecificTutorial = 0x292,
  EVID_setAntiGravity = 0x293,
  EVID_setVulnerabilityMod = 0x294,
  EVID_getFocusEntity = 0x295,
  EVID_enableBodyReactions = 0x296,
  EVID_enableBodyReaction = 0x297,
  EVID_getBodyReactionsEnabled = 0x298,
  EVID_playBodyReaction = 0x299,
  EVID_playBodyReactionFromType = 0x29A,
  EVID_playerViewEffect = 0x29B,
  EVID_playerWhiplash = 0x29C,
  EVID_inFOV = 0x29D,
  EVID_playerBecameDemon = 0x29E,
  EVID_demonReturnedtoPlayer = 0x29F,
  EVID_ae_bringDownFinished = 0x2A0,
  EVID_ae_wallClimbEffect = 0x2A1,
  EVID_ae_hideWeapon = 0x2A2,
  EVID_ae_showWeapon = 0x2A3,
  EVID_ae_attachReviveItem = 0x2A4,
  EVID_ae_activateReviveItem = 0x2A5,
  EVID_ae_attachEquippedWeapon = 0x2A6,
  EVID_ae_reviveAttachmentStartFX = 0x2A7,
  EVID_ae_reviveSound = 0x2A8,
  EVID_ae_showThrowItem = 0x2A9,
  EVID_ae_hideThrowItem = 0x2AA,
  EVID_ae_startCalculateLedgeGrabHandOffset = 0x2AB,
  EVID_animWeb_LedgeGrabEnded = 0x2AC,
  EVID_animWeb_LedgeGrabStarted = 0x2AD,
  EVID_animWeb_MonkeyBarEnded = 0x2AE,
  EVID_animWeb_MonkeyBarStarted = 0x2AF,
  EVID_animWeb_DashingStarted = 0x2B0,
  EVID_ae_releasePlayerFromSync = 0x2B1,
  EVID_ae_prowlerVictimCameraClear = 0x2B2,
  EVID_ae_syncDealtLethalBlow = 0x2B3,
  EVID_giveItemsFromLoadout = 0x2B4,
  EVID_fireOnslaughtProjectile = 0x2B5,
  EVID_statusEffectAdd = 0x2B6,
  EVID_statusEffectRemove = 0x2B7,
  EVID_statusEffectStart = 0x2B8,
  EVID_statusEffectEnd = 0x2B9,
  EVID_teleporterCooldownStart = 0x2BA,
  EVID_teleporterCooldownFinished = 0x2BB,
  EVID_damagePlayer = 0x2BC,
  EVID_isUpright = 0x2BD,
  EVID_setUsable = 0x2BE,
  EVID_setUsePercent = 0x2BF,
  EVID_getUsePercent = 0x2C0,
  EVID_setRateOfIncrease = 0x2C1,
  EVID_setRateOfDecrease = 0x2C2,
  EVID_isAtRest = 0x2C3,
  EVID_numPickupHands = 0x2C4,
  EVID_fade = 0x2C5,
  EVID_pulse = 0x2C6,
  EVID_breakIt = 0x2C7,
  EVID_notice_BeginGame = 0x2C8,
  EVID_notice_EV_StartTouch = 0x2C9,
  EVID_notice_EV_Touch = 0x2CA,
  EVID_notice_EV_Untouch = 0x2CB,
  EVID_damageBreak = 0x2CC,
  EVID_resetPropPhysics = 0x2CD,
  EVID_applyVelocity = 0x2CE,
  EVID_clearProhibitedPlayerMask = 0x2CF,
  EVID_getNumSplineChildren = 0x2D0,
  EVID_getSplineChild = 0x2D1,
  EVID_getNextSplineChild = 0x2D2,
  EVID_getClosestSplineChild = 0x2D3,
  EVID_getSplineLength = 0x2D4,
  EVID_getDistanceAlongPath = 0x2D5,
  EVID_getPositionAlongPath = 0x2D6,
  EVID_getDirectionAlongPath = 0x2D7,
  EVID_deactivate = 0x2D8,
  EVID_countInGroup = 0x2D9,
  EVID_averageHealthInGroup = 0x2DA,
  EVID_highestHealthInGroup = 0x2DB,
  EVID_lowestHealthInGroup = 0x2DC,
  EVID_setCurrentlyUsable = 0x2DD,
  EVID_setEnabledState = 0x2DE,
  EVID_notice_EV_VolumeMemberAdded = 0x2DF,
  EVID_notice_EV_VolumeMemberRemoved = 0x2E0,
  EVID_openPortal = 0x2E1,
  EVID_closePortal = 0x2E2,
  EVID_postClone = 0x2E3,
  EVID_fadeNoStipple = 0x2E4,
  EVID_crush = 0x2E5,
  EVID_resetTarget = 0x2E6,
  EVID_triggerShow = 0x2E7,
  EVID_getSmokeSystemRate = 0x2E8,
  EVID_setSmokeSystemRate = 0x2E9,
  EVID_setRenderModelScale = 0x2EA,
  EVID_randomPath = 0x2EB,
  EVID_addSpline = 0x2EC,
  EVID_targetAction = 0x2ED,
  EVID_modifyCount = 0x2EE,
  EVID_getCurrentCount = 0x2EF,
  EVID_getLimitCount = 0x2F0,
  EVID_intro_KillAI = 0x2F1,
  EVID_doTransition = 0x2F2,
  EVID_deferredDummyFire = 0x2F3,
  EVID_doFastTravelUnlock = 0x2F4,
  EVID_vegaChallengeCompleted = 0x2F5,
  EVID_notice_DoCheckpoint = 0x2F6,
  EVID_doCheckpoint = 0x2F7,
  EVID_getNumEntityDefs = 0x2F8,
  EVID_getLastSpawned = 0x2F9,
  EVID_isSpawnAvailable = 0x2FA,
  EVID_spawnEntity = 0x2FB,
  EVID_getTimeTillAvailable = 0x2FC,
  EVID_aISpawned = 0x2FD,
  EVID_entitySpawned = 0x2FE,
  EVID_postSpawnTimeline = 0x2FF,
  EVID_broadcastEndInteraction = 0x300,
  EVID_checkActiveFlags = 0x301,
  EVID_handleDormancyChange = 0x302,
  EVID_interactableAction = 0x303,
  EVID_notice_PlayerFullHealth = 0x304,
  EVID_notice_PlayerFiringShot = 0x305,
  EVID_resolveUnTouch = 0x306,
  EVID_ae_interactableAction = 0x307,
  EVID_ae_activateTargetsLastUser = 0x308,
  EVID_ae_hide = 0x309,
  EVID_ae_dropLoot = 0x30A,
  EVID_dropLootEntity = 0x30B,
  EVID_launchModSwapTutorial = 0x30C,
  EVID_reset = 0x30D,
  EVID_startAnim = 0x30E,
  EVID_showExternalEntity = 0x30F,
  EVID_bindActivator = 0x310,
  EVID_releaseActivator = 0x311,
  EVID_doMapTransition = 0x312,
  EVID_vegaTrainingResult = 0x313,
  EVID_vegaTrainingDeactivate = 0x314,
  EVID_ae_showRuneMenu = 0x315,
  EVID_ae_disableCollision = 0x316,
  EVID_ae_enableCollision = 0x317,
  EVID_exitArcade = 0x318,
  EVID_playDoom = 0x319,
  EVID_animWeb_LeftCycleEnded = 0x31A,
  EVID_animWeb_RightCycleEnded = 0x31B,
  EVID_handsHitReactions_ReactionEnded = 0x31C,
  EVID_ae_bringUpComplete = 0x31D,
  EVID_ae_multiplayerStartMeleeTrace = 0x31E,
  EVID_ae_multiplayerEndMeleeTrace = 0x31F,
  EVID_ae_legsCrossing = 0x320,
  EVID_ae_sprintTransitionPlaying = 0x321,
  EVID_ae_showMeshKit = 0x322,
  EVID_ae_hideMeshKit = 0x323,
  EVID_closeoutDeath = 0x324,
  EVID_ae_fKPoseFrame = 0x325,
  EVID_ae_fKPoseFrame_Explicit = 0x326,
  EVID_ae_aIReleaseTrigger = 0x327,
  EVID_ae_aIPullTrigger = 0x328,
  EVID_ae_setMasterWeaponSlot = 0x329,
  EVID_ae_setFireMode = 0x32A,
  EVID_ae_setSuppressAimDownMuzzle = 0x32B,
  EVID_ae_kill = 0x32C,
  EVID_ae_launchItem = 0x32D,
  EVID_ae_launchItemFromLocation = 0x32E,
  EVID_ae_launchExistingItemFromLocation = 0x32F,
  EVID_ae_launchItemFromWeapon = 0x330,
  EVID_ae_launchLoopEnd = 0x331,
  EVID_ae_canDropProjectile = 0x332,
  EVID_ae_dropProjectiles = 0x333,
  EVID_ae_activateLootDropForEnemy = 0x334,
  EVID_ae_equipInventoryItem = 0x335,
  EVID_ae_holsterInventoryItem = 0x336,
  EVID_ae_dropAttachment = 0x337,
  EVID_ae_enableHeadTracking = 0x338,
  EVID_ae_disableHeadTracking = 0x339,
  EVID_ae_enableDeathAnimations = 0x33A,
  EVID_ae_disableDeathAnimations = 0x33B,
  EVID_ae_enableStaggeringPain = 0x33C,
  EVID_ae_disableStaggeringPain = 0x33D,
  EVID_ae_suppressHeadTracking = 0x33E,
  EVID_ae_unsuppressHeadTracking = 0x33F,
  EVID_ae_addFocusTrackingFlags = 0x340,
  EVID_ae_removeFocusTrackingFlags = 0x341,
  EVID_ae_snapHeadTrackingToAnim = 0x342,
  EVID_ae_enableWorldCollision = 0x343,
  EVID_ae_disableWorldCollision = 0x344,
  EVID_ae_enableDamage = 0x345,
  EVID_ae_disableDamage = 0x346,
  EVID_ae_enableMeatHook = 0x347,
  EVID_ae_disableMeatHook = 0x348,
  EVID_ae_enableIK = 0x349,
  EVID_ae_disableIK = 0x34A,
  EVID_ae_startSphereModelTrace = 0x34B,
  EVID_ae_endSphereModelTrace = 0x34C,
  EVID_ae_startSphereModelTraceWithLifetime = 0x34D,
  EVID_ae_startSphereModelTraceWithAutoStop = 0x34E,
  EVID_ae_showShieldAttachment = 0x34F,
  EVID_ae_hideShieldAttachment = 0x350,
  EVID_ae_rechargeShield = 0x351,
  EVID_ae_startJetPackFX = 0x352,
  EVID_ae_stopJetPackFX = 0x353,
  EVID_ae_setLookFocusOffset = 0x354,
  EVID_ae_setAimFocusOffset = 0x355,
  EVID_ae_setLookFocusRotationOffset = 0x356,
  EVID_ae_setAimFocusRotationOffset = 0x357,
  EVID_ae_blendIntoFocusOffsetYaw = 0x358,
  EVID_ae_endFocusOffsetYaw = 0x359,
  EVID_ae_aimSweep_Setup = 0x35A,
  EVID_ae_aimSweep_Go = 0x35B,
  EVID_ae_startWeaponFX = 0x35C,
  EVID_ae_stopWeaponFX = 0x35D,
  EVID_ae_setDefaultMoveMode = 0x35E,
  EVID_ae_setFullDeltaGravityMovementMode = 0x35F,
  EVID_ae_setFullDeltaMovementMode_NoGravity = 0x360,
  EVID_ae_fullBodyGibExplosion = 0x361,
  EVID_ae_explode = 0x362,
  EVID_ae_popOffArmor = 0x363,
  EVID_ae_setArmor = 0x364,
  EVID_ae_setHealth = 0x365,
  EVID_ae_healHealth = 0x366,
  EVID_ae_damageHealth = 0x367,
  EVID_ae_takeDamage = 0x368,
  EVID_ae_playerScoreEvent = 0x369,
  EVID_ae_setAimFocusToEnemy = 0x36A,
  EVID_ae_setDeltaScale = 0x36B,
  EVID_ae_animDeltaCorrection = 0x36C,
  EVID_ae_animDeltaCorrectionEnd = 0x36D,
  EVID_ae_animDeltaCorrectionGoal = 0x36E,
  EVID_ae_animDeltaCorrectionSetFlags = 0x36F,
  EVID_ae_animDeltaCorrectionRotationRate = 0x370,
  EVID_ae_animDeltaCorrectionGoal_new = 0x371,
  EVID_ae_animDeltaCorrectionMarkRefPoint = 0x372,
  EVID_ae_aDC_TranslationMaxScale = 0x373,
  EVID_ae_aDC_TranslationMaxCorrectionPerFrame = 0x374,
  EVID_ae_aDC_TranslationMinMaxCorrectionPerFrame = 0x375,
  EVID_ae_aDC_TranslationMaxTranslationPerFrame = 0x376,
  EVID_ae_jointKeyframe = 0x377,
  EVID_ae_endSplineTargetingSequence = 0x378,
  EVID_ae_animReleaseWaitHandle = 0x379,
  EVID_ae_animComplete = 0x37A,
  EVID_ae_cycle = 0x37B,
  EVID_ae_phaseMarker = 0x37C,
  EVID_ae_phaseMarker_Triped = 0x37D,
  EVID_ae_phaseMarker_Quad = 0x37E,
  EVID_ae_goreSpawnEnt = 0x37F,
  EVID_ae_resurrectProxy = 0x380,
  EVID_ae_resurrectPull = 0x381,
  EVID_ae_resurrectComplete = 0x382,
  EVID_ae_ignitionComplete = 0x383,
  EVID_ae_postIgnite = 0x384,
  EVID_ae_throwProjectile = 0x385,
  EVID_ae_throwProjectileFromTag = 0x386,
  EVID_ae_throwProjectileFromTagToTarget = 0x387,
  EVID_ae_poseFrame = 0x388,
  EVID_ae_fSM_SetParentCanInterrupt = 0x389,
  EVID_ae_fSM_SetInterruptWindow = 0x38A,
  EVID_ae_makeAttackCheck = 0x38B,
  EVID_ae_clearAttackCheck = 0x38C,
  EVID_ae_clearAttackSelection = 0x38D,
  EVID_ae_setAllowCancelAttack = 0x38E,
  EVID_ae_recoverFromStagger = 0x38F,
  EVID_ae_recoverFromStaggerVulnerable = 0x390,
  EVID_ae_startRetaliationChecks = 0x391,
  EVID_ae_scaleWalkRate = 0x392,
  EVID_ae_setTakesDamage = 0x393,
  EVID_ae_aerialUnitStartDropping = 0x394,
  EVID_ae_testFloorHeight = 0x395,
  EVID_ae_testTraceToEnemy = 0x396,
  EVID_ae_testTraceToEnemyWithRadius = 0x397,
  EVID_ae_testBounds = 0x398,
  EVID_ae_testTraceForward = 0x399,
  EVID_ae_encounter_RaiseUserFlag = 0x39A,
  EVID_ae_gL_StartFlailEffect = 0x39B,
  EVID_ae_gL_TossRightFlail = 0x39C,
  EVID_ae_gL_TossLeftFlail = 0x39D,
  EVID_ae_gL_DislodgeRightFlail = 0x39E,
  EVID_ae_gL_DislodgePulseRightFlail = 0x39F,
  EVID_ae_gL_DislodgeLeftFlail = 0x3A0,
  EVID_ae_gL_DislodgePulseLeftFlail = 0x3A1,
  EVID_ae_gL_StartClearoutExtension = 0x3A2,
  EVID_ae_gL_EndClearoutExtension = 0x3A3,
  EVID_ae_gL_SetFlailExtension = 0x3A4,
  EVID_ae_gL_StartOverheadExtension = 0x3A5,
  EVID_ae_gL_EndOverheadExtension = 0x3A6,
  EVID_ae_gL_StartEtherealRush = 0x3A7,
  EVID_ae_gL_StartJacobsLadder = 0x3A8,
  EVID_ae_gL_EndJacobsLadder = 0x3A9,
  EVID_ae_gL_PulseJacobsLadder = 0x3AA,
  EVID_ae_gL_SpawnThresholdMinions = 0x3AB,
  EVID_ae_gL_UpdateMinionSpawners = 0x3AC,
  EVID_ae_gL_LaunchEtherealRush = 0x3AD,
  EVID_ae_gL_PlayShieldAnimation = 0x3AE,
  EVID_ae_gL_LiftWall = 0x3AF,
  EVID_ae_gL_LiftElectricWall = 0x3B0,
  EVID_ae_gL_RepelPlayer = 0x3B1,
  EVID_ae_gL_CalculateHopStrikeGoal = 0x3B2,
  EVID_ae_gL_CalculateBullRushGoal = 0x3B3,
  EVID_ae_gL_SetFlailScalingEnabled = 0x3B4,
  EVID_ae_gL_ChainPopDamage = 0x3B5,
  EVID_ae_gL_FlailHeadDamage = 0x3B6,
  EVID_ae_gL_FlailBashDamage = 0x3B7,
  EVID_ae_gL_Stage2Transition = 0x3B8,
  EVID_ae_gL_ClearStage1Wounds = 0x3B9,
  EVID_ae_gL_ShowSyncShield = 0x3BA,
  EVID_ae_gL_SetDamageEnableState = 0x3BB,
  EVID_ae_gL_StartProjectileShield = 0x3BC,
  EVID_ae_gL_EndProjectileShield = 0x3BD,
  EVID_ae_gL_StartBullrushHeatCone = 0x3BE,
  EVID_ae_gL_EndBullrushHeatCone = 0x3BF,
  EVID_ae_gL_RemoveFlails = 0x3C0,
  EVID_ae_gL_ClearSyncFlag = 0x3C1,
  EVID_ae_iOS_StartMeteorRain = 0x3C2,
  EVID_ae_iOS_StartMeteorDetonation = 0x3C3,
  EVID_ae_boss_IOS_Event = 0x3C4,
  EVID_ae_boss_KhanMakerEvent = 0x3C5,
  EVID_ae_cacodemonBileAttack = 0x3C6,
  EVID_ae_cacodemonRadiusAttack = 0x3C7,
  EVID_ae_cacodemonSetOpenMouthPercent = 0x3C8,
  EVID_ae_cacodemonSetPhaseSide = 0x3C9,
  EVID_ae_cacodemonCommitSnareDestination = 0x3CA,
  EVID_ae_boss_ShowHealthHUD = 0x3CB,
  EVID_ae_boss_ExitGloryKillState = 0x3CC,
  EVID_ae_boss_SpawnCollisionHullForDeath = 0x3CD,
  EVID_ae_boss_MoveSyncInteraction = 0x3CE,
  EVID_ae_boss_DropLoot = 0x3CF,
  EVID_ae_waveBlastTelegraph = 0x3D0,
  EVID_ae_sphereBlastTelegraph = 0x3D1,
  EVID_ae_waveBlast = 0x3D2,
  EVID_ae_setBlackboardIntValue = 0x3D3,
  EVID_ae_setBlackboardStringValue = 0x3D4,
  EVID_ae_removeBlackboardIntValue = 0x3D5,
  EVID_ae_removeBlackboardEntry = 0x3D6,
  EVID_ae_addDamageStateMemoryKey = 0x3D7,
  EVID_ae_removeDamageStateMemoryKey = 0x3D8,
  EVID_ae_setOverrideFocusTagName = 0x3D9,
  EVID_ae_clearOverrideFocusTag = 0x3DA,
  EVID_ae_setOverrideOrientationTagName = 0x3DB,
  EVID_ae_clearOverrideOrientationTag = 0x3DC,
  EVID_ae_setOverrideTrackingParms = 0x3DD,
  EVID_ae_clearOverrideTrackingParms = 0x3DE,
  EVID_ae_startFXWithAutoStop = 0x3DF,
  EVID_ae_startFX_ThisAnimOnly = 0x3E0,
  EVID_ae_startFX_CheckContext = 0x3E1,
  EVID_ae_stopFX_CheckContext = 0x3E2,
  EVID_ae_startFX_CheckPainReaction = 0x3E3,
  EVID_ae_stopFX_CheckPainReaction = 0x3E4,
  EVID_ae_checkRecovery = 0x3E5,
  EVID_ae_setStaggerLength = 0x3E6,
  EVID_ae_setSyncMeleeQueriedGroups = 0x3E7,
  EVID_ae_resetPositionAwareness = 0x3E8,
  EVID_ae_awkwardPose_Enter = 0x3E9,
  EVID_ae_awkwardPose_Leave = 0x3EA,
  EVID_ae_suppressVisualOffset = 0x3EB,
  EVID_ae_forceDeath = 0x3EC,
  EVID_ae_updateTrajectories = 0x3ED,
  EVID_ae_setThreatManagementDeclByName = 0x3EE,
  EVID_ae_setDamageStateByName = 0x3EF,
  EVID_ae_phaseShift = 0x3F0,
  EVID_ae_animatedAttachment_Show = 0x3F1,
  EVID_ae_animatedAttachment_Hide = 0x3F2,
  EVID_ae_animatedAttachment_PlayAnim = 0x3F3,
  EVID_ae_addAIEvent = 0x3F4,
  EVID_ae_continueTraversalChain = 0x3F5,
  EVID_ae_setOverrideTwitchPainType = 0x3F6,
  EVID_ae_setOptionalPainString = 0x3F7,
  EVID_ae_clearOptionalPainString = 0x3F8,
  EVID_ae_spawnProjectileAtLocation = 0x3F9,
  EVID_ae_scaleEntity = 0x3FA,
  EVID_ae_startLaserSweep = 0x3FB,
  EVID_ae_startAttackSweep = 0x3FC,
  EVID_ae_selectAimConfiguration = 0x3FD,
  EVID_ae_selectAimFocusPoint = 0x3FE,
  EVID_ae_startProjectileBoundsTests = 0x3FF,
  EVID_ae_setContextFlags = 0x400,
  EVID_ae_clearContextFlags = 0x401,
  EVID_ae_setMemoryKeyInt = 0x402,
  EVID_ae_beginRandomRagdollSequence = 0x403,
  EVID_ae_setLaserState = 0x404,
  EVID_ae_genericFraction = 0x405,
  EVID_ae_collisionFlags = 0x406,
  EVID_ae_trySyncInteraction = 0x407,
  EVID_ae_blendWindowOverride = 0x408,
  EVID_ae_blendWindowOverride_Dest = 0x409,
  EVID_ae_blendWindowOverride_Flags = 0x40A,
  EVID_ae_blendWindowOverride_StartFrame = 0x40B,
  EVID_ae_resetNextTriggerPullTime = 0x40C,
  EVID_ae_suppressLightFalter = 0x40D,
  EVID_ae_unsuppressLightFalter = 0x40E,
  EVID_ae_setSourcePosePosition = 0x40F,
  EVID_ae_setSpawnPosFromOrigin = 0x410,
  EVID_ae_releaseToken = 0x411,
  EVID_ae_validateAttackTarget = 0x412,
  EVID_ae_reValidateAttack = 0x413,
  EVID_ae_handleFSMTranscode = 0x414,
  EVID_ae_cyberMancubusMortarProjectileStart = 0x415,
  EVID_ae_addClassicDeathSuppressionFlags = 0x416,
  EVID_ae_removeClassicDeathSuppressionFlags = 0x417,
  EVID_ae_enterPainState = 0x418,
  EVID_ae_emptyPainBucket = 0x419,
  EVID_ae_arachnotron_LaunchGrenade = 0x41A,
  EVID_ae_whiplash_WhipJointKeyframe = 0x41B,
  EVID_ae_whiplash_WhipEvent = 0x41C,
  EVID_ae_pushActorsFromLandingZone = 0x41D,
  EVID_ae_spawnLostSoul = 0x41E,
  EVID_ae_lostSoulSpawnFinished = 0x41F,
  EVID_ae_spitLostSoul = 0x420,
  EVID_ae_throwLostSoulSpawn = 0x421,
  EVID_ae_throwLostSoulThrow = 0x422,
  EVID_ae_rushLostSoul = 0x423,
  EVID_ae_rushLostSoulFinished = 0x424,
  EVID_ae_debug = 0x425,
  EVID_ae_test = 0x426,
  EVID_ae_spawnSomething = 0x427,
  EVID_ae_setRateScale = 0x428,
  EVID_ae_setRateScaleWithLerp = 0x429,
  EVID_ae_setWebScalar = 0x42A,
  EVID_ae_disableAFBody = 0x42B,
  EVID_ae_forceStartBurnFade = 0x42C,
  EVID_ae_applyBuffModifiers = 0x42D,
  EVID_ae_sled_Detach = 0x42E,
  EVID_ae_featureCategory = 0x42F,
  EVID_ae_setAllowedToHandleIncomingProjectiles = 0x430,
  EVID_ae_setDisableLockOn = 0x431,
  EVID_ae_playAdditiveAnim = 0x432,
  EVID_ae_stopAdditiveAnim = 0x433,
  EVID_ae_startAdvancedScreenShakeForPlayersInRadius = 0x434,
  EVID_ae_lerpMeleeGlow = 0x435,
  EVID_ae_setHangoutIndex = 0x436,
  EVID_ae_playAIVO_SpawnAnim = 0x437,
  EVID_ae_clearDamageOverTime = 0x438,
  EVID_ae_hideInventory = 0x439,
  EVID_ae_removeLastInflictorProjectile = 0x43A,
  EVID_awarenessGained = 0x43B,
  EVID_setIgnoreEnemies = 0x43C,
  EVID_setIgnorePlayers = 0x43D,
  EVID_path = 0x43E,
  EVID_setAIExternalVelocity = 0x43F,
  EVID_getHealth = 0x440,
  EVID_animWeb_Wait = 0x441,
  EVID_animWeb_StartDeltaCorrection = 0x442,
  EVID_animWeb_TraversalChain = 0x443,
  EVID_animWeb_SetScalar = 0x444,
  EVID_dropWeapons = 0x445,
  EVID_removeFromWorldStateUsersList = 0x446,
  EVID_startDeath_Anim = 0x447,
  EVID_doom5MeleeExecute = 0x448,
  EVID_turnOffLaserSight = 0x449,
  EVID_turnOnLaserSight = 0x44A,
  EVID_removeBody = 0x44B,
  EVID_setFocus = 0x44C,
  EVID_clearLookFocus = 0x44D,
  EVID_clearAimFocus = 0x44E,
  EVID_setRemoveHealthWhenDamaged = 0x44F,
  EVID_setBehaviorDecl = 0x450,
  EVID_setHasteRateScale = 0x451,
  EVID_endSync = 0x452,
  EVID_isEntityVisible = 0x453,
  EVID_enableAIEvent = 0x454,
  EVID_addSoundStimulus = 0x455,
  EVID_modifyTraversalClass = 0x456,
  EVID_setEnemy = 0x457,
  EVID_setGroupRole = 0x458,
  EVID_unlockGroupRole = 0x459,
  EVID_debugPrintTreeWeb = 0x45A,
  EVID_endCurrentScriptAction = 0x45B,
  EVID_setInitialState = 0x45C,
  EVID_pullTrigger = 0x45D,
  EVID_releaseTrigger = 0x45E,
  EVID_pullTriggerRight = 0x45F,
  EVID_releaseTriggerRight = 0x460,
  EVID_pullTriggerLeft = 0x461,
  EVID_releaseTriggerLeft = 0x462,
  EVID_stopVoiceOver = 0x463,
  EVID_setAlertCycle = 0x464,
  EVID_setSubWeb = 0x465,
  EVID_setAimPoint = 0x466,
  EVID_setFireMode = 0x467,
  EVID_finishFire = 0x468,
  EVID_setScriptAbort = 0x469,
  EVID_setPlayerEnemy = 0x46A,
  EVID_enableDamage = 0x46B,
  EVID_enablePain = 0x46C,
  EVID_isPainEnabled = 0x46D,
  EVID_enableBodyRotation = 0x46E,
  EVID_enableHeadTracking = 0x46F,
  EVID_forceAwarenessByDistance = 0x470,
  EVID_forceGlobalAwarenessByEntityType = 0x471,
  EVID_clearWorldState = 0x472,
  EVID_teleportSetup = 0x473,
  EVID_teleportShow = 0x474,
  EVID_dropAttachment = 0x475,
  EVID_radiusDamageDealt = 0x476,
  EVID_damageOverTimeDealt = 0x477,
  EVID_suicide = 0x478,
  EVID_ignite = 0x479,
  EVID_bountyDamageScale = 0x47A,
  EVID_hasAwarenessOfEnemy = 0x47B,
  EVID_enablePerception = 0x47C,
  EVID_enableSpawnSettingsActions = 0x47D,
  EVID_getMonsterType = 0x47E,
  EVID_setGoalPosition = 0x47F,
  EVID_clearGoalPosition = 0x480,
  EVID_setOverridePositioningParmsName = 0x481,
  EVID_clearOverridePositioningParmsName = 0x482,
  EVID_setOverrideResurrectionProxy = 0x483,
  EVID_clearOverrideResurrectionProxy = 0x484,
  EVID_dropProjectiles = 0x485,
  EVID_clearDroppedProjectiles = 0x486,
  EVID_startRagdollSync = 0x487,
  EVID_stopRagdoll = 0x488,
  EVID_applyRagdollImpulse = 0x489,
  EVID_onRagdollStarted = 0x48A,
  EVID_setRagdollGravityFactor = 0x48B,
  EVID_notice_AIResurrectedFromProxy = 0x48C,
  EVID_notice_AISummonInitiated = 0x48D,
  EVID_notice_AISummonAborted = 0x48E,
  EVID_notifyBoss_PlayerDamageTakenInTimeWindow = 0x48F,
  EVID_notifyBoss_PlayerDeath = 0x490,
  EVID_notice_EV_StartGloryDeath = 0x491,
  EVID_equipInventoryItemByIndex = 0x492,
  EVID_holsterInventoryItemByIndex = 0x493,
  EVID_startWaveAttack = 0x494,
  EVID_abortWaveAttacks = 0x495,
  EVID_rift_ClearRift = 0x496,
  EVID_rift_Cleanup = 0x497,
  EVID_player_GiveInventoryItem = 0x498,
  EVID_updateModelTransform = 0x499,
  EVID_updateRenderModelDeferredPositions = 0x49A,
  EVID_setSolid = 0x49B,
  EVID_exitShift = 0x49C,
  EVID_enterShift = 0x49D,
  EVID_enterPainState = 0x49E,
  EVID_comboKill = 0x49F,
  EVID_scoreInterrupt = 0x4A0,
  EVID_meleeInteractionCompletedNotify = 0x4A1,
  EVID_exitSlowMotion = 0x4A2,
  EVID_enableShield = 0x4A3,
  EVID_disableShield = 0x4A4,
  EVID_executeSyncInteractionDecl = 0x4A5,
  EVID_enableFullBodyGoreByName = 0x4A6,
  EVID_detonateEntity = 0x4A7,
  EVID_addUserToAnimNode = 0x4A8,
  EVID_removeUserFromAnimNode = 0x4A9,
  EVID_executeCodeAction = 0x4AA,
  EVID_setupCodeMoveAction = 0x4AB,
  EVID_restartFSM = 0x4AC,
  EVID_tempMemory_Add = 0x4AD,
  EVID_tempMemory_Remove = 0x4AE,
  EVID_tempMemory_Clear = 0x4AF,
  EVID_updateCollisionFromGameFlags = 0x4B0,
  EVID_setWorldCollision = 0x4B1,
  EVID_endSphereModelTrace = 0x4B2,
  EVID_teleport_NoReset = 0x4B3,
  EVID_deactivateAllPhysics = 0x4B4,
  EVID_startMuzzleBoundsQueries = 0x4B5,
  EVID_setLaserState = 0x4B6,
  EVID_handleQueuedReliableMessage = 0x4B7,
  EVID_handleDeferredDamageParms = 0x4B8,
  EVID_clearDamageDeclModList = 0x4B9,
  EVID_freeze = 0x4BA,
  EVID_timeout = 0x4BB,
  EVID_hardTimeout = 0x4BC,
  EVID_releaseStaticLists = 0x4BD,
  EVID_spawnSomething = 0x4BE,
  EVID_applyBuffModifiers = 0x4BF,
  EVID_handleEnterPainState = 0x4C0,
  EVID_handleExitPainState = 0x4C1,
  EVID_addToEMPList = 0x4C2,
  EVID_startAnimDeltaCorrection = 0x4C3,
  EVID_checkForNetworkedPainUpdate = 0x4C4,
  EVID_ae_enableTorsoTracking = 0x4C5,
  EVID_ae_disableTorsoTracking = 0x4C6,
  EVID_ae_ledgeGrabImpact = 0x4C7,
  EVID_ae_setPlayerControlPercent = 0x4C8,
  EVID_ae_setPlayerControlMode = 0x4C9,
  EVID_ae_setAnimMotionPower = 0x4CA,
  EVID_ae_soundNoSkipRemoteOnly = 0x4CB,
  EVID_ae_slideCameraToDistance = 0x4CC,
  EVID_ae_forceCameraPitch = 0x4CD,
  EVID_ae_resetCameraViewLimits = 0x4CE,
  EVID_ae_swapPlayerModels = 0x4CF,
  EVID_ae_exitDemonPlayerState = 0x4D0,
  EVID_animWeb_StopTurning = 0x4D1,
  EVID_hideDemonBody = 0x4D2,
  EVID_showRenderModelAndWeapon = 0x4D3,
  EVID_notice_DamageToEnemy = 0x4D4,
  EVID_notice_PropDamaged = 0x4D5,
  EVID_notice_PropPickUp = 0x4D6,
  EVID_notice_PropPushed = 0x4D7,
  EVID_setAndHidePlayer = 0x4D8,
  EVID_ae_sendGroundPoundSpikes = 0x4D9,
  EVID_ae_setProwlerPerchStance = 0x4DA,
  EVID_ae_playVentFX = 0x4DB,
  EVID_startMancubusSmoke = 0x4DC,
  EVID_notice_ShieldWallCreated = 0x4DD,
  EVID_notice_ShieldWallRemoved = 0x4DE,
  EVID_ae_cacodemonFire = 0x4DF,
  EVID_ae_showLostSoul = 0x4E0,
  EVID_ae_hideLostSoul = 0x4E1,
  EVID_notice_PlayerJoin = 0x4E2,
  EVID_destroyedChanged = 0x4E3,
  EVID_emitSmokeDecl = 0x4E4,
  EVID_teamBlocked = 0x4E5,
  EVID_partBlocked = 0x4E6,
  EVID_moverPulse = 0x4E7,
  EVID_setRotateSpeed = 0x4E8,
  EVID_resumeCrusherMovement = 0x4E9,
  EVID_swfRender = 0x4EA,
  EVID_damageDone = 0x4EB,
  EVID_envHurtPostSpawn = 0x4EC,
  EVID_getDebugTarget = 0x4ED,
  EVID_setDebugTarget = 0x4EE,
  EVID_reallyDeactivate = 0x4EF,
  EVID_interactActivate = 0x4F0,
  EVID_animWeb_Finished = 0x4F1,
  EVID_ae_attachItem = 0x4F2,
  EVID_ae_attachItemAndGive = 0x4F3,
  EVID_ae_attachItemForManualDetach = 0x4F4,
  EVID_ae_attachSpecialItemForManualDetach = 0x4F5,
  EVID_ae_attachFPHandsItemForManualDetach = 0x4F6,
  EVID_ae_attachDefaultMeleeWeapon = 0x4F7,
  EVID_ae_detachItem = 0x4F8,
  EVID_ae_footstepEffect = 0x4F9,
  EVID_ae_soundBodyLoopUntilStopped = 0x4FA,
  EVID_ae_soundBodyLoopUntilAnimEnd = 0x4FB,
  EVID_ae_soundBodyLoopStop = 0x4FC,
  EVID_ae_attachCameraOnce = 0x4FD,
  EVID_ae_attachCameraExt = 0x4FE,
  EVID_ae_attachLimitViewCamera = 0x4FF,
  EVID_ae_setViewAnglesFromCamera = 0x500,
  EVID_ae_setViewAnglesFromCameraExt = 0x501,
  EVID_ae_setViewAnglesToDefault = 0x502,
  EVID_ae_startAdvancedScreenShakeNoOverlap = 0x503,
  EVID_ae_startCameraShakeBerserk = 0x504,
  EVID_ae_fadeView = 0x505,
  EVID_ae_hideHands = 0x506,
  EVID_ae_showHands = 0x507,
  EVID_ae_forceIntroBringupOnce = 0x508,
  EVID_ae_handsGenericHide = 0x509,
  EVID_ae_handsGenericUnhide = 0x50A,
  EVID_ae_incrementWeaponCustomSkin = 0x50B,
  EVID_ae_forcePraetorHandsAndBody = 0x50C,
  EVID_ae_entitySnapToPlayer = 0x50D,
  EVID_ae_playerSnapToEntity = 0x50E,
  EVID_ae_inhibitPlayerMovement = 0x50F,
  EVID_ae_inhibitPlayerView = 0x510,
  EVID_ae_meleeAttack = 0x511,
  EVID_ae_gloryKillShockwave = 0x512,
  EVID_ae_leftFootLadder = 0x513,
  EVID_ae_rightFootLadder = 0x514,
  EVID_ae_animStartBlendOut = 0x515,
  EVID_ae_unbind = 0x516,
  EVID_ae_contextualJumpAllowMovement = 0x517,
  EVID_ae_giveCollectible = 0x518,
  EVID_ae_setInventoryItemCustomSkinIndex = 0x519,
  EVID_ae_givePerk = 0x51A,
  EVID_ae_forceWeaponChange = 0x51B,
  EVID_ae_forceWeaponChangePerSettings = 0x51C,
  EVID_ae_nextLevelTransition = 0x51D,
  EVID_ae_healPlayer = 0x51E,
  EVID_ae_giveCodex = 0x51F,
  EVID_ae_weaponAdvanceCylinder = 0x520,
  EVID_ae_chainsawStall = 0x521,
  EVID_ae_activateRuinDamage = 0x522,
  EVID_ae_highlightRuinArea = 0x523,
  EVID_ae_showArgentSelection = 0x524,
  EVID_ae_animatedAttachment_Attach = 0x525,
  EVID_ae_animatedAttachment_Detach = 0x526,
  EVID_ae_inventoryItemPerformAction = 0x527,
  EVID_ae_setPlayerInvulnerable = 0x528,
  EVID_ae_setPlayerVulnerable = 0x529,
  EVID_ae_swapToDemonPlayerHands = 0x52A,
  EVID_soundEntity_On = 0x52B,
  EVID_soundEntity_Off = 0x52C,
  EVID_soundEntity_StartSound = 0x52D,
  EVID_soundEntity_StopSound = 0x52E,
  EVID_soundEntity_PauseSound = 0x52F,
  EVID_soundEntity_ResumeSound = 0x530,
  EVID_musicEntity_On = 0x531,
  EVID_musicEntity_Off = 0x532,
  EVID_musicEntity_SetState = 0x533,
  EVID_musicEntity_SetSwitch = 0x534,
  EVID_initTestModel = 0x535,
  EVID_ae_cameraFade = 0x536,
  EVID_ae_cameraTimeScale = 0x537,
  EVID_ae_cameraDepthOfField = 0x538,
  EVID_ae_cameraSound = 0x539,
  EVID_ae_cameraRenderParm = 0x53A,
  EVID_ae_cameraTeleportClient = 0x53B,
  EVID_ae_cameraTeleportClientToViewPos = 0x53C,
  EVID_ae_cameraCVarFloat = 0x53D,
  EVID_ae_cameraCVarString = 0x53E,
  EVID_ae_cameraSetJoint = 0x53F,
  EVID_ae_cameraSetFieldOfView = 0x540,
  EVID_ae_cameraLerpToFOV = 0x541,
  EVID_getUser = 0x542,
  EVID_notice_AIGainedAwarenessOfEntity = 0x543,
  EVID_playerFastTraveled = 0x544,
  EVID_spawnSingleAI = 0x545,
  EVID_spawnAI = 0x546,
  EVID_staggeredAISpawn = 0x547,
  EVID_groupBudgetSpawn = 0x548,
  EVID_maintainAICount = 0x549,
  EVID_stopMaintainingAICount = 0x54A,
  EVID_spawnArchvile = 0x54B,
  EVID_changeArchvileTemplate = 0x54C,
  EVID_spawnBuffPod = 0x54D,
  EVID_activateTarget = 0x54E,
  EVID_logicDesignerCustomEvent = 0x54F,
  EVID_setAIMemoryKey = 0x550,
  EVID_removeAIMemoryKey = 0x551,
  EVID_setMusicState = 0x552,
  EVID_setNextScriptIndex = 0x553,
  EVID_setNextScriptIndexRandom = 0x554,
  EVID_proceedToNextScript = 0x555,
  EVID_makeAIAwareOfPlayer = 0x556,
  EVID_restoreDefaultPerception = 0x557,
  EVID_activateCombatGrouping = 0x558,
  EVID_forceChargeOnAllAI = 0x559,
  EVID_clearCombatRoles = 0x55A,
  EVID_setCombatRoles = 0x55B,
  EVID_forceAIToFlee = 0x55C,
  EVID_raiseEventFlagOnExternalScript = 0x55D,
  EVID_migrateAIFromExternalScript = 0x55E,
  EVID_damageAI = 0x55F,
  EVID_removeAI = 0x560,
  EVID_disableScriptManager = 0x561,
  EVID_startLockdown = 0x562,
  EVID_stopLockdown = 0x563,
  EVID_startAmbientSounds = 0x564,
  EVID_stopAmbientSounds = 0x565,
  EVID_clearFactionOverrides = 0x566,
  EVID_setFactionRelation = 0x567,
  EVID_wait = 0x568,
  EVID_waitMulitpleConditions = 0x569,
  EVID_waitKillCount = 0x56A,
  EVID_waitKillCountOrSyncStart = 0x56B,
  EVID_waitRandomKillCount = 0x56C,
  EVID_waitForStatCount = 0x56D,
  EVID_waitAIRemaining = 0x56E,
  EVID_waitAIHealthLevel = 0x56F,
  EVID_waitMaintainComplete = 0x570,
  EVID_waitStaggeredSpawnComplete = 0x571,
  EVID_waitForEventFlag = 0x572,
  EVID_completeCombatScoring = 0x573,
  EVID_markScriptAsPreCommit = 0x574,
  EVID_setNextPreCommitIndex = 0x575,
  EVID_designerComment = 0x576,
  EVID_ae_gL_ShieldPromptComplete = 0x577,
  EVID_forceBreak = 0x578,
  EVID_sendEvent = 0x579,
  EVID_notice_EV_Show = 0x57A,
  EVID_notice_PropAbandoned = 0x57B,
  EVID_notice_PropKilled = 0x57C,
  EVID_notice_StartEndOfRound = 0x57D,
  EVID_notice_StartGameOver = 0x57E,
  EVID_notice_EV_WeaponDropped = 0x57F,
  EVID_notice_EV_StatusRemoved = 0x580,
  EVID_notice_PlayerLeave = 0x581,
  EVID_ae_turretFire = 0x582,
  EVID_notice_Survivor = 0x583,
  EVID_notice_PlayerKillAssist = 0x584,
  EVID_notice_KillingSpree_20 = 0x585,
  EVID_notice_KillingSpree_30 = 0x586,
  EVID_notice_DemonPlayerTimeOut = 0x587,
  EVID_notice_DemonPlayerGhostSpawned = 0x588,
  EVID_notice_CaptureFlag = 0x589,
  EVID_notice_ReturnFlag = 0x58A,
  EVID_notice_StealFlag = 0x58B,
  EVID_notice_MedalScoredGeneric = 0x58C,
  EVID_matchScoreHalfwayDone = 0x58D,
  EVID_matchTimeHalfwayDone = 0x58E,
  EVID_playGlobalCallout = 0x58F,
  EVID_playGameEventCallout = 0x590,
  EVID_playTeamCallout = 0x591,
  EVID_initialSaveGame = 0x592,
  EVID_reloadTest = 0x593,
  EVID_demonRespawnCallouts = 0x594,
  EVID_endPlayingGame = 0x595,
  EVID_debugKillSlayer = 0x596,
  EVID_ae_voiceTrack = 0x597,
  EVID_stopKilling = 0x598,
  EVID_notice_AIFSMTransitionState = 0x599,
  EVID_midnightLogicEvent = 0x59A,
  EVID_notice_InteractSucceeded = 0x59B,
  EVID_notice_EV_Activate = 0x59C,
  EVID_notice_PlayerUseProxyUsed = 0x59D,
  EVID_notice_EV_AISpawned = 0x59E,
  EVID_notice_EV_EntitySpawned = 0x59F,
  EVID_notice_EV_Facing = 0x5A0,
  EVID_notice_StatAggregated = 0x5A1,
  EVID_postAttach = 0x5A2,
  EVID_midnightStartLogic = 0x5A3,
  EVID_midnightBlendOutStarted = 0x5A4,
  EVID_interactStateExit = 0x5A5,
  EVID_notice_EV_StartAnim = 0x5A6,
  EVID_onContactImpulse = 0x5A7,
  EVID_notice_PlayerJumped = 0x5A8,
  EVID_notice_PlayerDoubleJumped = 0x5A9,
  EVID_notice_PlayerDashed = 0x5AA,
  EVID_grenadePostSpawn = 0x5AB,
  EVID_notice_PlayerHealed = 0x5AC,
  EVID_handleUpgrades = 0x5AD,
};

*/
CACHED_EVENTDEF(teleport);
CACHED_EVENTDEF(getWorldOrigin);
CACHED_EVENTDEF(kill);
CACHED_EVENTDEF(remove);
CACHED_EVENTDEF(removeBinds);
CACHED_EVENTDEF(setLinearVelocity);
CACHED_EVENTDEF(bind);
CACHED_EVENTDEF(bindPosition);

CACHED_EVENTDEF(getBindMaster);
CACHED_EVENTDEF(getBindParent);
CACHED_EVENTDEF(unbind);
extern idEventArg g_null_eventargs;


template<typename TCallable>
static void nonplayer_entities_within_distance_iterate(idVec3* base_point, float distance, TCallable&& callback) {
	idEventDef* position_getter = ev_getWorldOrigin.Get();



	void* vtmember_identity_callevent = VTBL_MEMBER(idEntity, VTBLOFFS_CALLEVENT)::Get();

	idVec3 playerpos = *base_point;


	void** entity_tbl = get_entity_table();
	if (!entity_tbl)
		return;

	//get_entity_position(player1, &playerpos);
	for (unsigned i = LAST_PLAYER_IDX; i < WORLD_ENTITY_IDX; ++i) {

		void* current = entity_tbl[i];

		if (!current)
			continue;

		idVec3 posfor;
		//get_entity_position(current, &posfor);

		call_as<void>(vtmember_identity_callevent, current, &getpos_result, position_getter, &g_null_eventargs);

		posfor = getpos_result->value.v_vec3;

		if (playerpos.Distance(posfor) > distance)
			continue;
		callback(current);
		
	}
}