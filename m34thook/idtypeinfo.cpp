#include "mh_defs.hpp"
#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "idtypeinfo.hpp"
#include "scanner_core.hpp"
#include "snaphakalgo.hpp"
#include <algorithm>
#include "idLib.hpp"
#include "clipboard_helpers.hpp"
#include "cmdsystem.hpp"

#include "mh_headergen.hpp"
#include "gameapi.hpp"
#include <future>
#if 0
static constexpr const char* g_all_entity_names[] = {
"jakesDevWidget","idWorldspawn","idWorldClipBounds","idWeaponEntity","idWaveShield","idWaterEntity","idVolume_VisualImpairment","idVolume_Trigger","idVolume_TraversalGen","idVolume_ToggleableDamageOverTime","idVolume_TogglePlayerBodyReaction","idVolume_StreamTreeMarkup","idVolume_StealthField","idVolume_StatusEffect","idVolume_Stairs","idVolume_SpaceWarp","idVolume_Siphon","idVolume_SecretHint","idVolume_RuneHint","idVolume_RunCycleHint","idVolume_RevivePlayer","idVolume_PlayerViewEffect","idVolume_PlayerUseProxy","idVolume_PlayerEnvOverride","idVolume_PlayerBodyReaction","idVolume_PlasmaPuddleFX","idVolume_PlasmaPuddle","idVolume_PlacedFlightVolume","idVolume_OliviasGuardFight_DangerZone","idVolume_MatterBallWarp","idVolume_MancubusSteamFX","idVolume_MancubusSteam","idVolume_MancubusFlamethrower","idVolume_LocationCalling","idVolume_LightRigModifier","idVolume_KillDownedInjured","idVolume_InvasionSpawnZone","idVolume_InvasionInhibit","idVolume_Invasion","idVolume_Gravity","idVolume_ForceDormant","idVolume_FlightObstacle","idVolume_Flight","idVolume_EmpField","idVolume_DoomGravity","idVolume_DeployedLaser","idVolume_DemonicPossession","idVolume_DemonicBait","idVolume_DamageOverTime","idVolume_Damage","idVolume_CustomLedgeGrab","idVolume_ControlPointSpawnInfluence","idVolume_Blocking","idVolume","idUtilityCamera","idUmbraVolume","idUmbraVisibilityContributionVolume","idTurret","idTrigger_VoiceCommunication","idTrigger_VisibilityController","idTrigger_TestPlayerState","idTrigger_Teleporter_Fade","idTrigger_Teleporter","idTrigger_TakeDamage","idTrigger_StatDrivenEvent","idTrigger_SonicBoom","idTrigger_Repulsor","idTrigger_RemoveInventoryItems","idTrigger_RemoveEntities","idTrigger_Push","idTrigger_Multiplayer","idTrigger_MovementModifier","idTrigger_ModPlayerVelocity","idTrigger_InvasionAreaExit","idTrigger_InvasionAreaEnter","idTrigger_Hurt","idTrigger_GorillaBar","idTrigger_ForcePlayerBodyReactionOnce","idTrigger_Facing","idTrigger_EnergyField","idTrigger_DynamicHurt","idTrigger_DummyFire","idTrigger_Dormancy","idTrigger_DemonDamageOutModifier","idTrigger_Damage","idTrigger_ChallengeBoundry","idTrigger_BouncePad","idTrigger_AIInteraction","idTriggerToucher","idTrigger","idTrailerCamera","idToucher","idTexlodNodeGenerationArea","idTest_Sector","idTest_MetaData_WebPath","idTest_MetaData_Target","idTest_MetaData_String","idTest_MetaData_SectorYaw","idTest_MetaData_SectorPitch","idTest_MetaData_Plane","idTest_MetaData_MoveableSpheres","idTest_MetaData_Model","idTest_MetaData_Line","idTest_MetaData_FovYaw","idTest_MetaData_FovPitch","idTest_MetaData_Fov","idTest_MetaData_Cylinder2","idTest_MetaData_Cylinder","idTest_MetaData_Circle","idTest_MetaData_Bounds","idTestModel2","idTestEntity","idTeleporterPad","idTeamCapturePoint","idTarget_VisibilityController","idTarget_ViewPos","idTarget_UnlockUltraNightmare","idTarget_TutorialGui","idTarget_Timer","idTarget_Timeline","idTarget_TestPlayerState","idTarget_Teleport","idTarget_SwapNavMesh","idTarget_SwapFaction","idTarget_Subtitle","idTarget_StartSoundShader","idTarget_Spawn_Target","idTarget_Spawn_Parent","idTarget_Spawn","idTarget_SoundDuck","idTarget_Snap_Objective","idTarget_ShowHands","idTarget_ShowGui","idTarget_Show","idTarget_ShakeTrigger","idTarget_SetSpawnSpot","idTarget_SetInEncounterGroup","idTarget_SetGroupCombatStage","idTarget_Secret","idTarget_RemoveSaveGate","idTarget_RemoveItems","idTarget_Remove","idTarget_RandomImpulse","idTarget_Print","idTarget_PodiumSpawn","idTarget_PlayerWhiplash","idTarget_PlayerViewEffect","idTarget_PlayerStatus","idTarget_PlayerStatModifier","idTarget_PlayerModifier","idTarget_PlayerInvulnerability","idTarget_PlayerCheckpoint","idTarget_PlayVoiceOver","idTarget_Ping","idTarget_Perk","idTarget_Path","idTarget_POI","idTarget_Objective_Triggered","idTarget_Objective_Replace","idTarget_Objective_HideEntities","idTarget_Objective_Give","idTarget_Objective_Complete","idTarget_Notification","idTarget_ModifyTraversalClass","idTarget_ModifyGroup","idTarget_Melee","idTarget_MapGroupUnlock","idTarget_MakeActivatable","idTarget_LightController","idTarget_LevelTransition","idTarget_LayerStateChange","idTarget_InventoryCheck","idTarget_Intro","idTarget_InteractionAction","idTarget_HideHands","idTarget_Hide","idTarget_GroupMessage","idTarget_GroupExpression","idTarget_GiveWeaponUpgradePoints","idTarget_GiveItems","idTarget_GeomCacheStreamer","idTarget_GameStateIntSet","idTarget_GameStateIntInc","idTarget_GameChallengeGameOver","idTarget_GUICommand","idTarget_ForceGroupRole","idTarget_ForceDormancy","idTarget_FirstThinkActivate","idTarget_FireWeapon","idTarget_FastTravelUnlock","idTarget_FastTravelInhibit","idTarget_FakeEnemy","idTarget_FadeComplete","idTarget_EquipItem","idTarget_Enemy","idTarget_EndOfCampaign","idTarget_EndInvasion","idTarget_EncounterChallenge","idTarget_EnableTarget","idTarget_EnableGroup","idTarget_EnableAIEvent","idTarget_DynamicChallenge_Start","idTarget_DynamicChallenge_PointTrigger","idTarget_DynamicChallenge_GiveBonus","idTarget_DynamicChallenge_FailChallenge","idTarget_DynamicChallenge_End","idTarget_DummyFire","idTarget_DormancyRadius","idTarget_Disconnect_GoToScreen","idTarget_Disconnect","idTarget_DisableInvasion","idTarget_DisableEscMenu","idTarget_DevLoadoutSwap","idTarget_DemonBountyAiTypes","idTarget_DeactivateStatusEffects","idTarget_Damage","idTarget_Cvar","idTarget_CurrencyCheck","idTarget_Credits","idTarget_Count_Random","idTarget_Count","idTarget_ConditionalAccessor","idTarget_Conditional","idTarget_Command","idTarget_CollisionDamage","idTarget_Codex","idTarget_ClearFakeEnemy","idTarget_ChangeVolume_PlayerEnvOverride","idTarget_ChangeMaterial","idTarget_ChangeColor","idTarget_Break","idTarget_Award_RushAttack","idTarget_Award_Adrenaline","idTarget_ApplyImpulse","idTarget_ApplyExplosionImpulse","idTarget_AnimWebUnpause","idTarget_AnimWebPause","idTarget_AnimWebChangeStateVia","idTarget_AnimWebChangeState","idTarget_AmplitudeTrigger","idTarget_AdaptiveTickToggle","idTarget_ActionScript","idTarget_AIProxy","idTarget_AIGlobalSettings","idTarget_AIEvent","idTargetSpawnGroup","idTarget","idSyncEntity","idSummoningTemplate","idStaticWaterEntity","idStaticVisibilityBlocker","idStaticMultiGuiEntity","idStaticEntity","idSpringCamera","idSplinePath","idSpitfireCannon","idSpectatorCamera","idSpectacleCamera","idSpawnPoint","idSpawnNodeSplines","idSpawnNode","idSpawnArea","idSoundTrigger","idSoundSubtitlesEntity","idSoundSphereEntity","idSoundPrefetchEntity","idSoundEntity","idSoundBoxEntity","idSoundBeamEntity","idSoundAreaEntity","idSlowMotionCamera","idRotatableCamera","idRiftBuildPosition","idRibbon2Emitter","idResurrectionProxy","idRegenArea","idReferenceMap","idPurifyingBeam","idProp_WeaponStatic","idProp_WeaponPickup","idProp_Usable","idProp_Static","idProp_Spawnable","idProp_PlayableDemonCircle","idProp_Pickup","idProp_OnlineCollectible","idProp_Moveable","idProp_HealthPickup","idProp_Explosive","idProp_ElectricArmor","idProp_DemonCircle","idProp_Coop_Billboard","idProp_Coop","idProp_ContinuallyUsed","idProp_BreakableLoot","idProp_Breakable","idProp_ArmorPickup","idProp_AIArmor","idProp2","idProp","idProjectile_SwarmMissile_V2","idProjectile_SwarmMissile","idProjectile_SiphonGrenade","idProjectile_RollingFire","idProjectile_Rocket","idProjectile_MeatHook","idProjectile_Mancubus_Smoke","idProjectile_GrenadeFast","idProjectile_Grenade","idProjectile_EMP","idProjectile_Destroyer","idProjectile_DamageOverTime","idProjectile_CyberdemonSwarmMissile","idProjectile_CacoDemonRocket","idProjectile_BfgBurn","idProjectile_BfgArc","idProjectile_Auger","idProjectile_AIHomingRocket","idProjectileShield","idProjectile","idPoseableEntity","idPortalWorld","idPortalSurface","idPlayerStart","idPlayer","idPieceEmitter","idPhotoModeCamera","idPerceptionVolume","idPathCorner","idParticleEmitter","idOverTheShoulderCamera","idObjective_Relay","idNetworkedParticleEmitter","idNetworkedEntityFx","idNetworkLight","idMusicEntity","idMultiplayerTrigger","idMoverModifier","idMover","idMidnightCutscene","idLowGravityMover","idLogicEntity","idLight","idLensFlare","idJostleSwivel","idJostleSpring","idJostleAnimated","idItemPropSpawner","idInventoryStorage","idInvasionBlocker","idInvasionAreaManager","idInteractionCamera","idInteractable_WorldCache","idInteractable_WeaponModBot","idInteractable_VegaTraining","idInteractable_Tutorial","idInteractable_SonicBoost","idInteractable_SlayerGate_Coop","idInteractable_SlayerGate","idInteractable_Shooter","idInteractable_Rune","idInteractable_Respec","idInteractable_Obstacle","idInteractable_NightmareMarker","idInteractable_Moveable","idInteractable_Minigame","idInteractable_LootDrop","idInteractable_LootCrate","idInteractable_JukeBox","idInteractable_HighlightSelector","idInteractable_GoreNest","idInteractable_GoreBud","idInteractable_GiveItems","idInteractable_EliteGuard_Coop","idInteractable_EliteGuard","idInteractable_Doom","idInteractable_CurrencyExchange","idInteractable_CollectibleViewer","idInteractable_Challenge_Shrine","idInteractable_BatterySocket","idInteractable_Automap","idInteractable","idInfo_UniversalTraversal","idInfo_Trigger_Facing_Target","idInfo_TraversalPoint","idInfo_TraversalChain","idInfo_TraversalBase","idInfo_TeleportDestination","idInfo_SpawnConditionProxy","idInfo_FastTravel","idInfo_DoorTraversalChain","idInfo_BounceDestination","idInfoWaitForButtonAfterLoad","idInfoTraversal_Ultimate","idInfoTraversal_Bot","idInfoTraversalEndPoint","idInfoTraversal","idInfoTexLod","idInfoStandPoint","idInfoSplineChild","idInfoSpawnPoint","idInfoRuler","idInfoRoam","idInfoPlayerHud","idInfoPath","idInfoOrbit","idInfoLookTargetGroup","idInfoLookTarget","idInfoLevelFadeIn","idInfoGoalPosition","idInfoFocus","idInfoFlightVolumeEntrance","idInfoExportHint","idInfoDebugMarker","idInfoCoverExposed","idInfoCover","idInfoCloudShot","idInfoBounds","idInfoAmbient","idInfoAAS","idInfo","idInfluenceSpawnSettings","idIOS_MeteorCrater","idHeightmapVolumeEntity","idGuiEntity_Text","idGuiEntity_Cinematic","idGuiEntity","idGoreEntity","idGladiatorShield","idGeomCacheEntity","idGameChallenge_Shell","idGameChallenge_PVPTutorial","idGameChallenge_PVP","idGameChallenge_CampaignSinglePlayer","idGameChallenge","idFuncSwing","idFuncShadowCaster","idFuncRotate","idFreeDbgCam","idFreeCamera","idFlightVolumeTraversalSpline","idExtraLifeTeleportLocation","idEscapePod","idEnvironmentalDamage_PointManager_Trigger","idEnvironmentalDamage_Point","idEnvironmentalDamage_Hurt_Trigger","idEnvArea","idEntityFxRandom","idEntityFx","idEntityCamera","idEntity","idEngineEntity","idEnergyShell","idEncounterVolume_ValidCover","idEncounterTrigger_RaiseUserFlag","idEncounterTrigger_PreCombat","idEncounterTrigger_OverrideCombatGrouping","idEncounterTrigger_Exit","idEncounterTrigger_Commit","idEncounterTrigger_AmbientAIAudio","idEncounterModifier_SetNextScriptIndex","idEncounterModifier_SetEnabledState","idEncounterModifier_Reset","idEncounterManager","idEncounterGroupMgr","idEncounterAmbientAudio","idElectricBoltEmitter","idEditorModelEntity","idDynamicVisibilityBlocker","idDynamicEntity_Damageable","idDynamicEntity","idDestructibleManager","idDestructible","idDestroyableProp","idDemonPlayer_Summoner","idDemonPlayer_Revenant","idDemonPlayer_Prowler","idDemonPlayer_Pinky","idDemonPlayer_PainElemental","idDemonPlayer_Marauder","idDemonPlayer_Mancubus","idDemonPlayer_Cacodemon","idDemonPlayer_Baron","idDemonPlayer_Archvile","idDemonPlayer_Arachnotron","idDemonPlayerStart","idDemonPlayer","idDecalEntity_Coop","idDecalEntity","idDebrisEntity","idDeathmatchPlayerStart","idDamageableAttachment","idCyberdemonMissileShooter","idCrusher","idCoopStartingInventory","idCombatVolume","idCombatGrouping","idCollisionStreamArea","idCollisionExclusionVolume","idCollectibleEntity","idCinematicCamera","idCaptureFrames","idCampaignInvaderStart","idCameraView","idCamera","idBuildArea","idBreakablePlatform","idBreakable","idBotPathNodeEntity","idBotActionEntity_Usable","idBotActionEntity_Info_RoamGoal","idBotActionEntity_Info_POI","idBotActionEntity_Info","idBotActionEntity","idBossInfo","idBloatedEntity","idBillboard","idBfgBurn","idBasePropSpawner","idBarricade","idBFG_Geyser","idAutomapSectionRevealTrigger","idAutomapMapGroupRevealEntity","idArchvileTemplate","idAnnouncementManager","idAnimated_ThreatSensor","idAnimated_AnimWeb_Samuel","idAnimated_AnimWeb","idAnimatedSimple_Faust","idAnimatedSimple_AnimSys","idAnimatedInteractable","idAnimatedEntity","idAnimatedAttachment_AF","idAnimatedAttachment","idAnimated","idAnimNode","idAnimCamera","idAmbientTriggerModifier_SetAITypes","idAmbientNodeGenerationArea","idAmbientNodeExclusionArea","idAlignedEntity","idActorWorkNode","idActorAmbientFilter","idActor","idActionNode","idAbnormalPlayerMetrics","idAITest_Rotation","idAITest","idAISearchHintGroup","idAISearchHintAuto","idAISearchHintAnimation","idAIMapMarkupEntity","idAIInteraction","idAIHintGroup","idAIHint","idAIDebugCamera","idAIDeathVolume","idAICombatHint_SuppressionFirePoint","idAICombatHint","idAIAASHint","idAI2","idAFEntity_GetUpTest","idAFEntity_Generic","idAFEntity_Dummy","idAFEntity_Corpse","idAASReachability","idAASObstacle"
};

static constexpr const char* g_all_types[] =
#include "alltypes.h"
;
#endif
//#include "types_with_vtables.hpp"
struct typeInfoGenerated_t {
	enumTypeInfo_t* enums;
	int numEnums;
	classTypeInfo_t* classes;
	int numClasses;
	typedefInfo_t* typedefs;
	int numTypedefs;
};


struct enum_or_class_t {
	uintptr_t p;

	void set(classTypeInfo_t* cls) {
		p = ((uintptr_t)cls) | 1;
	}
	void set(enumTypeInfo_t* enm) {
		p = ((uintptr_t)enm);
	}

	bool isClass() const {
		return p & 1;
	}

	bool isEnum() const {
		return !(p & 1);
	}

	enumTypeInfo_t* getEnum() {
		if (!isEnum())return nullptr;
		return (enumTypeInfo_t*)p;
	}

	classTypeInfo_t* getClass() {
		if (!isClass())return nullptr;
		return (classTypeInfo_t*)(p & ~1ULL);
	}

};

static const char* g_idc_skipset[] = {
	"char",
	"unsigned char",
	"short",
	"unsigned short",
	"int",
	"unsigned int",
	"long",
	"unsigned long",
	"long long",
	"unsigned long long",
	"float",
	"double",
	"bool"
};

static bool g_did_init_skipinfo = false;

struct type_node_t {
	enum_or_class_t m_typeinfo;

};
static int g_offset_typeinfo = -1;
static int g_offset_typeinfo2 = -1;

static void* get_typeinfo_tools() {
	return *(void**)descan::g_global_typeinfo_tools;

}

using typeinfo_tools_getter_t = feature_binder_ptr_t<get_typeinfo_tools>;

MH_NOINLINE
classTypeInfo_t* idType::FindClassInfo(const char* cname) {

	void* typeinfo_tools =get_typeinfo_tools();



	return reinterpret_cast<classTypeInfo_t * (*)(void*, const char*)>(descan::g_idtypeinfo_findclassinfo)(typeinfo_tools, cname);

}

static typeInfoGenerated_t* get_typeinfo_generated(unsigned which = 0) {

	void* typeinfo_tools = *(void**)descan::g_global_typeinfo_tools;

	if (g_offset_typeinfo == -1) {
		/*
			shit, these dont exist in v6
		*/
		int offs_to_generated = 0;//idType::FindClassField("idTypeInfoTools", "generatedTypeInfo")->offset;


		int sizeof_gentype = 56;// idType::FindClassInfo("idTypeInfoTools::registeredTypeInfo_t")->size;


		char* generated_ptr = (char*)typeinfo_tools;
		generated_ptr += offs_to_generated;

		g_offset_typeinfo = offs_to_generated;
		g_offset_typeinfo2 = offs_to_generated + sizeof_gentype;

	}

	void* typeinfogenerated = *reinterpret_cast<void**>(reinterpret_cast<char*>(typeinfo_tools) + (!which ? g_offset_typeinfo : g_offset_typeinfo2));

	return (typeInfoGenerated_t*)typeinfogenerated;

}
MH_NOINLINE
enumTypeInfo_t* idType::FindEnumInfo(const char* ename) {


	void* typeinfo_tools = *(void**)descan::g_global_typeinfo_tools;



	return reinterpret_cast<enumTypeInfo_t * (*)(void*, const char*)>(descan::g_idtypeinfo_findenuminfo)(typeinfo_tools, ename);
}

static void dump1(std::string& filetext, const char* entname, std::set<std::string>& seen, std::vector<const char*>& queued) {

	auto ctype = idType::FindClassInfo(entname);

	if (!ctype)
		return;
	filetext += "\nstruct ";
	filetext += entname;
	if (ctype->superType && ctype->superType[0]) {
		filetext += " : public ";
		filetext += ctype->superType;

	}
	filetext += " {\n";

	for (auto bleh2 = ctype->variables; bleh2 && bleh2->name; ++bleh2) {
		//bleh2->
		filetext += "\t//";

		char buff[64];
		sprintf_s(buff, "Offset %d,\t size %d\n", bleh2->offset, bleh2->size);

		filetext += buff;
		if (bleh2->comment && bleh2->comment[0]) {
			filetext += "\t//";
			filetext += bleh2->comment;
			filetext += "\n";
		}
		filetext += "\t";
		filetext += bleh2->type;


		if (bleh2->ops) {
			filetext += bleh2->ops;
		}
		filetext += " ";
		filetext += bleh2->name;
		filetext += ";\n";
	}
	filetext += "};\n";
}

void idType::do_idlib_dump() {
#if 0
	std::set<std::string> seen_names{};

	for (auto&& ename : g_all_entity_names)
		seen_names.insert(ename);

	std::string filetext{};
	std::vector<const char*> queued_types{};
	queued_types.reserve(131072);

	for (auto entname : g_all_types) {


		dump1(filetext, entname, seen_names, queued_types);


	}
#if 0
	while (queued_types.size() != 0) {
		std::vector<const char*> old_queued_types = std::move(queued_types);
		queued_types.reserve(65536);
		for (auto&& nq : old_queued_types) {
			dump1(filetext, nq, seen_names, queued_types);
		}
	}
#endif
	FILE* outtypes = nullptr;
	fopen_s(&outtypes, "eternal_idlib.h", "w");

	fputs(filetext.c_str(), outtypes);
	fclose(outtypes);
#endif
}

static std::string normalize_type_name(std::string_view input) {
	std::string result{};
	result.reserve(input.length());
	for (unsigned i = 0; i < input.length(); ++i) {
		unsigned char c = input[i];
		unsigned cc = c;
		if (cc == ' ') {
			continue;
		}
		result.push_back(cc);
	}
	return result;
}
static std::set<std::string_view> g_fuckset{};

static std::set<std::string_view> g_handled_classes_step1{};


#define		IDC_HUGEBUF_SIZE (1024*1024*256)
static bool should_skip_type(std::string_view typenam) {
	/*return std::binary_search(g_idc_skipset, &g_idc_skipset[sizeof(g_idc_skipset) / sizeof(g_idc_skipset[0])], typenam, [](const char* l, const char* r) {
		return strcmp(l, r);
		});*/
	return g_fuckset.find(typenam) != g_fuckset.end();

}
static char opsbuffer[256];





void idType::generate_idc() {


	if (!g_did_init_skipinfo) {
		/*std::qsort(g_idc_skipset, sizeof(g_idc_skipset) / sizeof(g_idc_skipset)[0], sizeof(g_idc_skipset[0]), [](const void* x, const void* y)->int {
			return strcmp(reinterpret_cast<const char* const*>(x)[0], reinterpret_cast<const char* const*>(y)[0]);
			});
			*/

		for (auto&& skipper : g_idc_skipset) {
			g_fuckset.insert(skipper);

		}
		g_did_init_skipinfo = true;
	}
	std::string result{};
	std::set<std::string> our_defined_types{};


	result += "from idc import*\nfrom idaapi import *\nid = -1\nti=tinfo_t()\ntl=get_idati()\n";//"#define UNLOADED_FILE   1\n#include <idc.idc>\nstatic main(void){auto id = -1;";


	for (unsigned iterwhen = 0; iterwhen < 2; ++iterwhen)
	{

		unsigned nclasses;
		classTypeInfo_t* iterclasses = ClassTypes(nclasses, iterwhen);

		//add the structures
		for (unsigned i = 0; true; ++i) {

			if (!iterclasses[i].name) {
				break;
			}

			std::string_view name_sv = iterclasses[i].name;
			if (should_skip_type(name_sv))
				continue;
			result += "add_struc(-1,\"";
			//UNWIND_CODE",0);
			std::string normalized = normalize_type_name(name_sv);

			result += normalized;
			our_defined_types.insert(std::move(normalized));
			result += "\",0)\n";

		}

	}


	for (unsigned iterwhen = 0; iterwhen < 2; ++iterwhen) {
		unsigned nenums;


		enumTypeInfo_t* enums = EnumTypes(nenums, iterwhen);

		for (unsigned i = 0; i < nenums; ++i) {
			if (!enums[i].name) {
				continue;
			}
			std::string_view name_sv = enums[i].name;

			if (should_skip_type(name_sv))
				continue;


			result += "id = add_enum(0xFFFFFFFFFFFFFFFF,\"";

			std::string normalized = normalize_type_name(name_sv);
			result += normalized;
			our_defined_types.insert(std::move(normalized));
			result += "\",0x1100000)\n";

			enumTypeInfo_t* currenum = &enums[i];

			auto vals = currenum->values;

			while (vals->name && vals->name[0]) {
				result += "add_enum_member(id,\"";
				result += vals->name;
				result += "\",";
				result += std::to_string(vals->value);
				result += ",0XFFFFFFFFFFFFFFFF)\n";
				++vals;

			}
		}
	}
	for (unsigned iterwhen = 0; iterwhen < 2; ++iterwhen) {
		unsigned nclasses;
		classTypeInfo_t* classes = ClassTypes(nclasses, iterwhen);
		for (unsigned i = 0; true; ++i) {

			if (!classes[i].name) {
				break;
			}
			if (should_skip_type(classes[i].name))
				continue;
			result += "id = get_struc_id(\"";
			std::string normalized_name = normalize_type_name(classes[i].name);

			result += normalized_name;

			result += "\")\n";
			result += "sptr=get_struc(get_struc_id(\"";
			result += normalized_name;
			result += "\"))\n";

			auto emit_field = [&result, &our_defined_types](const char* name, size_t offset, size_t size, const char* typ, const char* ops) {
				result += "idc.add_struc_member(id,\"";
				result += name;
				result += "\",	";
				std::string offstr = std::to_string((int)offset);
				result += offstr;

				std::string typ2 = normalize_type_name(typ);




				result += ",0x000400,0XFFFFFFFFFFFFFFFF,";

				result += std::to_string((int)size);

				result += ")\n";

				if (typ && typ[0]) {

					std::string deferred_type_append = "";

					deferred_type_append += "memb=get_member(sptr," + offstr + ")\n";
					deferred_type_append += "ti.get_named_type(tl,\"" + typ2 + "\")\n";


					memset(opsbuffer, 0, sizeof(opsbuffer));

					bool in_array = false;
					unsigned opbuffer_writepos = 0;

					for (unsigned opindex = 0; ops[opindex]; opindex++) {
						unsigned opchar = ops[opindex];

						if (in_array) {
							if (opchar == ']') {
								opsbuffer[opbuffer_writepos] = 0;

								int arrbound = atoi(opsbuffer);

								in_array = false;
								opbuffer_writepos = 0;

								deferred_type_append += "ti.create_array(ti, ";
								deferred_type_append += opsbuffer;
								deferred_type_append += ")\n";

							}
							else {
								if ((opchar >= '0' && opchar <= '9') || opchar == '-') {
									opsbuffer[opbuffer_writepos++] = opchar;
								}
								else {
									//cant parse named constant
									return;
								}
							}
						}
						else {
							if (opchar == '[') {
								in_array = true;
							}
							else {
								if (opchar == '*') {
									deferred_type_append += "ti.create_ptr(ti)\n";
								}
							}
						}
					}

					result += deferred_type_append;
					result += "set_member_tinfo(sptr, memb, 0, ti,SET_MEMTI_MAY_DESTROY|SET_MEMTI_USERTI)\n";
				}



			};


			classTypeInfo_t* currclass = &classes[i];

			if (currclass->superType && currclass->superType[0]) {
				auto found = idType::FindClassInfo(currclass->superType);
				if (found) {
					size_t sz = found->size;
					emit_field("base", 0, sz, currclass->superType, "");
				}
			}

			classVariableInfo_t* vars = currclass->variables;
			if (vars) {
				while (vars->name && vars->name[0]) {
					if (vars->size != -1) {

						const char* typ_ptr = vars->type;




						emit_field(vars->name, vars->offset, vars->size, typ_ptr, vars->ops);
					}
					vars++;

				}
			}

		}
	}
	//add_struc_member(id,"n",	0X8,	0x30000400,	-1,	8);
	//use parse_decls to gen the typedefs
	//parse_decls(inputtype, flags=0)
	//result += "}";

	FILE* resfile = nullptr;

	fopen_s(&resfile, "DoomEternalTypes.py", "wb");

	fwrite(result.c_str(), 1, result.length(), resfile);
	//fputs(result.c_str(), resfile);
	fclose(resfile);


}
MH_NOINLINE
classVariableInfo_t* idType::FindClassField(const char* cname, const char* fieldname) {
	auto playerclass = idType::FindClassInfo(cname);
	if (!playerclass)
		return nullptr;
	auto headattr = playerclass->variables;
	classVariableInfo_t* notarget_field = nullptr;
	while (headattr && headattr->name) {
		if (sh::string::streq(headattr->name, fieldname)) {
			return headattr;
		}
		++headattr;
	}
	return nullptr;
}

static int g_offset_classtypes = -1;
static int g_offset_nclasstypes = -1;
static int g_offset_enums = -1;
static int g_offset_nenums = -1;
static int g_offset_typedefs = -1;
static int g_offset_ntypedefs = -1;
classTypeInfo_t* idType::ClassTypes(unsigned& out_n, unsigned whichsource) {
	/*if (g_offset_classtypes == -1) {
		g_offset_classtypes = FindClassField("typeInfoGenerated_t", "classes")->offset;
		g_offset_nclasstypes = FindClassField("typeInfoGenerated_t", "numClasses")->offset;
	}*/
	typeInfoGenerated_t* typegen = get_typeinfo_generated(whichsource);


	classTypeInfo_t* classes = typegen->classes;

	out_n = typegen->numClasses;
	return classes;
}
unsigned idType::NumClassesTotal() {
	unsigned num1, num2;

	ClassTypes(num1, 0);
	ClassTypes(num2, 1);
	return num1 + num2;
}
enumTypeInfo_t* idType::EnumTypes(unsigned& out_n, unsigned whichsource) {
	/*if (g_offset_enums == -1) {
		g_offset_enums = FindClassField("typeInfoGenerated_t", "enums")->offset;
		g_offset_nenums = FindClassField("typeInfoGenerated_t", "numEnums")->offset;
	}*/
	typeInfoGenerated_t* typegen = get_typeinfo_generated(whichsource);

	out_n = typegen->numEnums;


	//enumTypeInfo_t* enums = *reinterpret_cast<enumTypeInfo_t**>(typegen + g_offset_enums);

	//out_n = *reinterpret_cast<unsigned*>(typegen + g_offset_nenums);
	return typegen->enums;
}
typedefInfo_t* idType::TypedefTypes(unsigned& out_n, unsigned whichsource) {
	/*if (g_offset_typedefs == -1) {
		g_offset_typedefs = FindClassField("typeInfoGenerated_t", "typedefs")->offset;
		g_offset_ntypedefs = FindClassField("typeInfoGenerated_t", "numTypedefs")->offset;
	}*/
	typeInfoGenerated_t* typegen = get_typeinfo_generated(whichsource);


	typedefInfo_t* typs = typegen->typedefs;//*reinterpret_cast<typedefInfo_t**>(typegen + g_offset_typedefs);

	out_n = typegen->numTypedefs;//*reinterpret_cast<unsigned*>(typegen + g_offset_ntypedefs);
	return typs;
}

classVariableInfo_t* idType::try_locate_var_by_name(classVariableInfo_t* from, const char* field) {
	for (classVariableInfo_t* clvar = from; clvar->name && clvar->name[0]; ++clvar) {
		if (sh::string::streq(clvar->name, field)) {
			return clvar;
		}
	}
	return nullptr;
}
MH_NOINLINE
classVariableInfo_t* idType::try_locate_var_by_name_inher(classTypeInfo_t* clstype, const char* field) {
	classVariableInfo_t* located_var = nullptr;
	for (classTypeInfo_t* clptr = clstype; clptr && !located_var; clptr = get_class_superclass(clptr) ) {
		located_var = try_locate_var_by_name(clptr->variables, field);
	}
	return located_var;
}
MH_NOINLINE
int idType::fieldspec_calculate_offset(classTypeInfo_t* onclass, char* _rwbuff) {

	unsigned char* ucbuff, * priorstart;
	bool done_with_fields;
	int current_offset;
	classVariableInfo_t* field;


	field = nullptr;

	current_offset = 0;
	done_with_fields = false;
	ucbuff = (unsigned char*)_rwbuff;

find_field_end:
	priorstart = ucbuff;
	while (true) {
		unsigned currval = *ucbuff;

		if (currval == (unsigned)'.') {
			*ucbuff = 0;
			ucbuff++;
			break;

		}
		else if (currval == 0u) {
			done_with_fields = true;
			break;

		}

		else {
			ucbuff++;
		}
	}

	if (!*priorstart)
		return current_offset;

	field = try_locate_var_by_name_inher(onclass, (const char*)priorstart);
	MH_UNLIKELY_IF(!field)
		return -1;

	current_offset += field->offset;

	if (!done_with_fields) {
		onclass = get_field_class(field);// FindClassInfo(field->type);
		MH_UNLIKELY_IF(!onclass) {
			return -1;
		}
		goto find_field_end;
	}


	return current_offset;

}

static char g_idtype_json_ptrbuf[64];

template<typename T, size_t fieldlength>
static void idtype_json_add_field(std::string& outbuf, const char(&name)[fieldlength], T text) {


	outbuf += "\"";
	outbuf += name;
	outbuf += "\":";

	if constexpr (std::is_integral_v<T>) {
		outbuf += std::to_string(text);
	}
	else if constexpr (std::is_pointer_v<T>) {

		if constexpr (std::is_same_v<char, std::remove_cvref_t<std::remove_pointer_t<T>>>) {
			outbuf += "\"";
			outbuf += text;
			outbuf += "\"";
		}
		else {
			sprintf_s(g_idtype_json_ptrbuf, "%p", text);
			outbuf += "\"";
			outbuf += static_cast<char*>(g_idtype_json_ptrbuf); //force decay to remove arr extent
			outbuf += "\"";


		}
	}


}

template<typename T, size_t fieldlength>
static void idtype_json_add_next_field(std::string& outbuf, const char(&name)[fieldlength], T text) {
	idtype_json_add_field<T, fieldlength>(outbuf, name, text);
	outbuf+=",";
}
static void idtype_append_typedef_json(std::string& outbuf, typedefInfo_t* tdefinfo) {

	outbuf += "{";


	idtype_json_add_next_field(outbuf, "name", tdefinfo->name);
	idtype_json_add_next_field(outbuf, "type", tdefinfo->type);
	idtype_json_add_next_field(outbuf, "ops", tdefinfo->ops);
	idtype_json_add_field(outbuf, "size", tdefinfo->size);

	outbuf += "}";
}

static void idtype_append_enum_value(std::string& outbuf, enumValueInfo_t* value) {
	outbuf += "{";

	idtype_json_add_next_field(outbuf, "name", value->name);
	idtype_json_add_field(outbuf, "value", value->value);

	outbuf += "}";
}


static void idtype_append_classvar_value(std::string& outbuf, classVariableInfo_t* varinfo) {
	outbuf += "{";	
	idtype_json_add_next_field(outbuf, "type",varinfo->type);
	idtype_json_add_next_field(outbuf, "ops", varinfo->ops);
	idtype_json_add_next_field(outbuf, "name", varinfo->name);

	idtype_json_add_next_field(outbuf, "offset",varinfo->offset);
	idtype_json_add_next_field(outbuf, "size", varinfo->size);
	idtype_json_add_next_field(outbuf, "flags", varinfo->flags);
	idtype_json_add_next_field(outbuf, "comment", varinfo->comment);

	idtype_json_add_next_field(outbuf, "get_ea", varinfo->get);
	idtype_json_add_next_field(outbuf, "set_ea", varinfo->set);
	idtype_json_add_field(outbuf, "reallocate_ea", varinfo->reallocate);

	outbuf += "}";
}

static void idtype_append_class(std::string& outbuf, classTypeInfo_t* typeinf) {
	
}
void idType::generate_json() {

}
/*
	todo: figure out how to use valueIndex
*/
MH_NOINLINE
const char* idType::get_enum_member_name_for_value(enumTypeInfo_t* enumtype, long long value) {
	enumValueInfo_t* values = enumtype->values;

	while(values->name && values->name[0]) {
		
		if(values->value == value) {
			return values->name;
		}
		else {
			++values;
		}

	}
	return nullptr;

}

bool idType::enum_member_is(enumTypeInfo_t* enm, long long value, const char* membername) {
	const char* name = get_enum_member_name_for_value(enm, value);
	MH_UNLIKELY_IF(!name)
		return false;
	return sh::string::streq(name, membername);
}


MH_NOINLINE
long long* idType::get_enum_member_value(const char* ename, const char* mname) {

	enumTypeInfo_t* etyp = FindEnumInfo(ename);
	MH_UNLIKELY_IF(!etyp) {
		return nullptr;
	}
	enumValueInfo_t* values = etyp->values;

	while(values->name && values->name[0]) {
		
		if(sh::string::streq(values->name, mname)){
			return &values->value;
		}

		++values;
	}
	return nullptr;
}


//shared between header gen code and startup prop ptr locating code
static strviewset_t get_set_of_prop_names() {

	strviewset_t allprops{};




	for (unsigned which = 0; which < 2; ++which) {
		unsigned nclass = 0;
		classTypeInfo_t* clss = idType::ClassTypes(nclass, which);

		for (unsigned i = 0; i < nclass; ++i) {

			classTypeInfo_t* cltype = &clss[i];

			if (!cltype->name) {
				continue;
			}
			auto vars = cltype->variables;
			if (vars) {
				while (vars->name && vars->name[0]) {

					allprops.insert(vars->name);

					++vars;
				}
			}
		}

	}
	return allprops;

}





MH_NOINLINE
void idType::generate_unique_property_key_tree() {
	strviewset_t allprops = get_set_of_prop_names();


	std::vector<unsigned char> bbuff = pack_strset(allprops);
	unsigned total_required_bytes = bbuff.size();


	std::vector<unsigned char> compout = compress_packet_strset(bbuff);
	const char* propkey_type = "unsigned";

	if (allprops.size() < 65536)
		propkey_type = "unsigned short";




	std::string proptxt_result = "#pragma once\nenum de_prop_e : ";

	proptxt_result += propkey_type;
	proptxt_result += "{";


	for (auto&& prop : allprops) {

		proptxt_result += "prp_";



		proptxt_result += prop;
		proptxt_result += ",";
	}

	proptxt_result += "};static constexpr unsigned DE_NUMPROPS = ";
	proptxt_result += std::to_string(allprops.size());

	proptxt_result += ";static constexpr unsigned DE_PROP_MASK = ";

	unsigned prop_mask = (~0u >> std::countl_zero(allprops.size()));

	proptxt_result += std::to_string(prop_mask);
	proptxt_result += ";";
	proptxt_result += "static constexpr unsigned DE_PROP_BITS = ";

	proptxt_result += std::to_string(std::popcount(prop_mask));
	proptxt_result += ";static constexpr unsigned ALLPROPS_COMPRESSED_SIZE = ";


	proptxt_result += std::to_string(compout.size());

	proptxt_result += ";static constexpr unsigned ALLPROPS_DECOMPRESSED_SIZE = ";
	proptxt_result += std::to_string(total_required_bytes);

		
	proptxt_result += ";\n__declspec(allocate(\"cmptbl\")) extern unsigned char ALLPROPS_COMPRESSED_DATA[ALLPROPS_COMPRESSED_SIZE];";
	write_cfile(std::move(proptxt_result), "doom_eternal_properties_generated.hpp");



	std::string proptxt_header = "#include \"doom_eternal_properties_generated.hpp\"\n";

	proptxt_header += "__declspec(allocate(\"cmptbl\")) unsigned char ALLPROPS_COMPRESSED_DATA[ALLPROPS_COMPRESSED_SIZE] = {";

	proptxt_header += expand_bytes_to_c_bytearray(compout);

	proptxt_header += "};";



	write_cfile(std::move(proptxt_header), "doom_eternal_properties_generated.cpp");



}
unsigned g_propname_rvas[DE_NUMPROPS];

uint64_t g_propname_hashes[DE_NUMPROPS];

mh_classtypeextra_t* g_typeextra;

void idType::compute_classinfo_mh_payloads() {
#if !defined(DISABLE_MH_TYPEINFO_EXTENSIONS)
	void* typeinfo_tools = get_typeinfo_tools();
	unsigned totalclasses = NumClassesTotal();




	std::map<const char*, de_prop_e> tmp_name2prop;

	for (unsigned i = 0; i < DE_NUMPROPS; ++i) {
		const char* nam = from_de_rva<const char>(g_propname_rvas[i]);
		if (!nam)
			continue;
		tmp_name2prop[nam] = (de_prop_e)i;
	}


	unsigned required_uint16s = 0;

	



	for (unsigned which = 0; which < 2; ++which) {
		unsigned nclass = 0;
		classTypeInfo_t* clss = idType::ClassTypes(nclass, which);

		for (unsigned i = 0; i < nclass; ++i) {

			classTypeInfo_t* cltype = &clss[i];
			if (!cltype->name) {
				continue;
			}

			if (!cltype->variableNameHashes) {
				cltype->m_mh_added_delta2super = 0;
			}
			else {
				//save the number of fields in here for the next pass
				cltype->m_mh_added_delta2super = sh::ibulk::find_indexof_u64(cltype->variableNameHashes, 0) ;
			}
			required_uint16s += cltype->m_mh_added_delta2super;

			// cltype->metaData = nullptr;
			

		}

	}

	unsigned u16mem = required_uint16s * sizeof(de_prop_e);


	g_typeextra = (mh_classtypeextra_t*)sh::vmem::allocate_rw((totalclasses * sizeof(mh_classtypeextra_t)) + u16mem);

	de_prop_e* current_propalloc_ptr = (de_prop_e*)(&g_typeextra[totalclasses]);

	unsigned current_class = 0;



	for (unsigned which = 0; which < 2; ++which) {
		unsigned nclass = 0;
		classTypeInfo_t* clss = idType::ClassTypes(nclass, which);

		for (unsigned i = 0; i < nclass; ++i) {

			classTypeInfo_t* cltype = &clss[i];
			if (!cltype->name) {
				continue;
			}


			mh_classtypeextra_t* current_classextra = &g_typeextra[current_class++];

			if(cltype->metaData) {
				current_classextra->m_metadata = *cltype->metaData;
			}
			else {
				current_classextra->m_metadata.metaData = "";

			}
			
			cltype->metaData = &current_classextra->m_metadata;

			current_classextra->m_num_fields = cltype->m_mh_added_delta2super;
			if (current_classextra->m_num_fields == 0) {
				current_classextra->m_offset2fields = ~0u;
			}
			else {
				current_classextra->m_offset2fields = ((uintptr_t)current_propalloc_ptr) - ((uintptr_t)current_classextra);
				
				de_prop_e* myprops = current_propalloc_ptr;
				current_propalloc_ptr += current_classextra->m_num_fields;
				//populate the property array
				for (unsigned j = 0; j < current_classextra->m_num_fields; ++j) {

					
					/*auto uptr = std::lower_bound(&g_propname_rvas[0], &g_propname_rvas[DE_NUMPROPS], cltype->variables[j].name, [](unsigned rva, const char* name) {
						return strcmp(from_de_rva<const char>(rva), name);
						});
						*/

					auto iter = tmp_name2prop.find(cltype->variables[j].name);

					if (iter != tmp_name2prop.end()) {
						myprops[j] = iter->second;
					}
					else {
						myprops[j] = (de_prop_e)~0u;
					}

				}

			}


		}

	}

	for (unsigned which = 0; which < 2; ++which) {
		unsigned nclass = 0;
		classTypeInfo_t* clss = idType::ClassTypes(nclass, which);

		for (unsigned i = 0; i < nclass; ++i) {

			classTypeInfo_t* cltype = &clss[i];
			//cltype->metaData = nullptr;
			if (!cltype->name) {
				continue;
			}
			
			if (cltype->superType && cltype->superType[0]) {

				classTypeInfo_t* deltaptr = call_as<classTypeInfo_t*>(descan::g_idtypeinfo_findclassinfo, typeinfo_tools, cltype->superType);
				cltype->m_mh_added_delta2super = static_cast<int>( ((intptr_t)deltaptr) - ((intptr_t)cltype) );

			}
			else {

				cltype->m_mh_added_delta2super = 0;
			}

			auto vars = cltype->variables;
			if (vars) {
				while (vars->name && vars->name[0]) {

					
					classTypeInfo_t* cltype_for = call_as<classTypeInfo_t*>(descan::g_idtypeinfo_findclassinfo, typeinfo_tools, vars->type);


					if (cltype_for) {

						vars->m_mh_added_delta2type = static_cast<int>(((intptr_t)cltype_for) - ((intptr_t)vars));
					}
					else {
						vars->m_mh_added_delta2type = 0;

					}
					++vars;
				}
			}

		}

	}

#endif
	//return reinterpret_cast<classTypeInfo_t * (*)(void*, const char*)>(descan::g_idtypeinfo_findclassinfo)(typeinfo_tools, cname);

}

//called from meathook_final_init because typeinfogenerated doesnt exist until it
//actually, it probably exists much earlier at some point between the second stage running and us giving exec back to the game, but that would require another hook
void idType::init_prop_rva_table() {
#if !defined(DISABLE_MH_PROP_RVAS)
	

	bvec_t hugebuffer_decompress1 = decompress_strset(ALLPROPS_COMPRESSED_DATA, ALLPROPS_COMPRESSED_SIZE, ALLPROPS_DECOMPRESSED_SIZE);

	//all of our data is already sorted so this is kinda a waste
	//made this use emplace_hint pushing back
	strviewset_t decompressed_set = unpack_strset(hugebuffer_decompress1, DE_NUMPROPS);

	//todo: do decompress/build of set and get_set_of_prop_names in parallel
	strviewset_t unpset = get_set_of_prop_names();

	//put 0 thought into this, but obviously it should be made to take advantage of the fact all the data is sorted


	strviewset_t::iterator current_findpos = decompressed_set.begin();
	for (unsigned i = 0; i < DE_NUMPROPS; ++i) {

		strviewset_t::iterator loc = unpset.find(*current_findpos);

		if (loc == unpset.end()) {
			//not good!!
			//add some output here!
			//probably happens on v1 since these props were generated from 6.66

			g_propname_rvas[i] = 0;
			g_propname_hashes[i] = 0;
		}
		else {
			g_propname_rvas[i] = to_de_rva(loc->data());
			g_propname_hashes[i] = idType::calculate_field_name_hash(loc->data(), loc->length());

		}
		current_findpos++;
	}
#endif
}

void idType::dump_prop_rvas() {
#if !defined(DISABLE_MH_PROP_RVAS)
	std::string result = "Dump of all property indices:\n";

	char sprintfbuf[2048];
	for (unsigned i = 0; i < DE_NUMPROPS; ++i) {

		unsigned currrva = g_propname_rvas[i];

		if (currrva == 0) {
			sprintf(sprintfbuf, "\tprop index %d = NULL\n", i);

		}
		else {
			const char* pos = (g_blamdll.image_base + currrva);

			sprintf(sprintfbuf, "\tprp_%s = (rva 0x%X, ptr %p)\n", pos, currrva, pos);
		}


		result += &sprintfbuf[0];

	}

	idLib::Printf("%s", result.c_str());
	set_clipboard_data(result.c_str());
#endif
}

#define	__ROL8__	std::rotl<unsigned long long>
#define	__ROR8__	std::rotr<unsigned long long>
static 
unsigned __int64   sub_1419985E0(uint64_t* a1, __int64 a2)
{
	char* v3; // rdi
	__int64 v4; // r8
	__int64 v5; // rsi
	unsigned __int64 v6; // rbx
	__int64 v7; // rdx
	unsigned __int64 v8; // r10
	__int64 v9; // rcx
	unsigned __int64 v10; // rdx
	__int64 v11; // rax
	unsigned __int64 v12; // r9
	__int64 v13; // rcx
	unsigned __int64 v14; // r10
	unsigned __int64 v15; // r8
	
	v3 = (char*)a1 + a2;
	v4 = a1[1];
	v5 = 2 * a2 - 0x651E95C4D06FBFB1i64;
	v6 = 0x9AE16A3B2F90404Fui64 * *a1;
	v7 = v5 * *(uint64_t*)((char*)a1 + a2 - 8);
	v8 = 0x9AE16A3B2F90404Fui64 * *((uint64_t*)v3 - 2)
		+ std::rotr<unsigned long long>(v7, 30)
		+ std::rotl(v4 - 0x651E95C4D06FBFB1i64 * *a1, 21);
	v9 = v7 + std::rotr<unsigned long long>(v4 - 0x651E95C4D06FBFB1i64, 18);
	v10 = v5 * (v8 + *((uint64_t*)v3 - 4));
	v11 = a1[3];
	v12 = (v6 + v9) ^ (v5 * (v8 ^ (v6 + v9))) ^ ((v5 * (v8 ^ (v6 + v9))) >> 47);
	v13 = v5 * a1[2];
	v14 = v13 + v10 + std::rotr<unsigned long long>(v11 - 0x651E95C4D06FBFB1i64 * *a1, 18);
	v15 = v5
		* (v14 ^ (v5 * (*((uint64_t*)v3 - 3) + v5 * ((v5 * v12) ^ ((v5 * v12) >> 47)))
			+ std::rotr<unsigned long long>(v10, 30)
			+ std::rotl<unsigned long long>(v11 + v13, 21)));
	return v5 * ((v5 * (v14 ^ v15 ^ (v15 >> 47))) ^ ((v5 * (v14 ^ v15 ^ (v15 >> 47))) >> 47));
}
static
unsigned __int64 HashLen0to16(uint64_t* a1, unsigned __int64 a2)
{
	unsigned __int64 v3; // r9
	__int64 v4; // rdx
	unsigned __int64 v5; // r8
	unsigned __int64 v6; // rdx
	unsigned __int64 v8; // r9
	__int64 v9; // rdx
	unsigned __int64 v10; // rcx
	unsigned __int64 v11; // rdx

	if (a2 < 8)
	{
		if (a2 < 4)
		{
			if (a2)
			{
				v11 = (0xC3A5C85C97CB3127ui64 * ((unsigned int)a2 + 4 * *((unsigned __int8*)a1 + a2 - 1))) ^ (0x9AE16A3B2F90404Fui64 * *(unsigned __int8*)a1 - 0x1E95C4D06FBFB100i64 * *((unsigned __int8*)a1 + (a2 >> 1)));
				return 0x9AE16A3B2F90404Fui64 * (v11 ^ (v11 >> 47));
			}
			else
			{
				return 0x9AE16A3B2F90404Fui64;
			}
		}
		else
		{
			v8 = 2 * a2 - 0x651E95C4D06FBFB1i64;
			v9 = *(unsigned int*)((char*)a1 + a2 - 4);
			v10 = v8 * (v9 ^ (a2 + 8i64 * *(unsigned int*)a1));
			return v8 * ((v8 * (v9 ^ v10 ^ (v10 >> 47))) ^ ((v8 * (v9 ^ v10 ^ (v10 >> 47))) >> 47));
		}
	}
	else
	{
		v3 = 2 * a2 - 0x651E95C4D06FBFB1i64;
		v4 = *(uint64_t*)((char*)a1 + a2 - 8);
		v5 = v3 * (v4 + __ROR8__(*a1 - 0x651E95C4D06FBFB1i64, 25));
		v6 = v3 * (v5 ^ (*a1 - 0x651E95C4D06FBFB1i64 + v3 * __ROL8__(v4, 27)));
		return v3 * ((v3 * (v5 ^ v6 ^ (v6 >> 47))) ^ ((v3 * (v5 ^ v6 ^ (v6 >> 47))) >> 47));
	}
}
#define	_QWORD		long long
static
__int64  cryptstr_thunked(_QWORD* a1, unsigned __int64 a2)
{
	__int64 v5; // rax
	unsigned __int64 v6; // rcx
	unsigned __int64 v7; // r10
	unsigned __int64 v8; // rdx
	unsigned __int64 v9; // r9
	unsigned __int64 v10; // r8
	__int64 v11; // rdx
	__int64 v12; // r9
	__int64 v13; // rax
	__int64 v14; // r10
	__int64 v15; // r12
	__int64 v16; // r15
	char* v17; // r13
	__int64 v18; // r8
	__int64* v19; // r14
	__int64 v20; // r11
	__int64 v21; // rdi
	__int64 v22; // rbx
	__int64 v23; // rsi
	__int64 v24; // r12
	__int64 v25; // r15
	__int64 v26; // r9
	__int64 v27; // rax
	unsigned __int64 v28; // rsi
	__int64 v29; // r8
	__int64 v30; // rdx
	__int64 v31; // r10
	__int64 v32; // rax
	__int64 v33; // rcx
	__int64 v34; // r9
	__int64 v35; // rcx
	__int64 v36; // rdx
	__int64 v37; // r11
	__int64 v38; // r8
	__int64 v39; // r11
	__int64 v40; // rdx
	__int64 v41; // rbp
	__int64 v42; // r9
	__int64 v43; // rcx
	__int64 v44; // r8
	__int64 v45; // rdi
	__int64 v46; // r14
	unsigned __int64 v47; // rsi
	__int64 v48; // rcx
	__int64 v49; // rdx
	__int64 v50; // r9
	__int64 v51; // r10
	__int64 v52; // rbx
	__int64 v53; // r8
	__int64 v54; // r11
	__int64 v55; // rcx
	__int64 v56; // r10
	__int64 v57; // rdx
	unsigned __int64 v58; // r9
	__int64 v59; // r10
	unsigned __int64 v60; // rdx
	unsigned __int64 v61; // r8
	unsigned __int64 v62; // rdx
	char* v63; // [rsp+20h] [rbp-48h]
	__int64 v64; // [rsp+78h] [rbp+10h]
	__int64 v65; // [rsp+80h] [rbp+18h]
	__int64 v66; // [rsp+88h] [rbp+20h]

	if (a2 > 0x20)
	{
		if (a2 > 0x40)
		{
			v11 = 0;
			v12 = 0x134A747F856D0526i64;
			v13 = 0;
			v14 = 0;
			v15 = *a1 + 0x1529CBA0CA458FFi64;
			v64 = 0;
			v16 = 0x226BB95B4E64B6D4i64;
			v17 = (char*)a1 + ((a2 - 1) & 0xFFFFFFFFFFFFFFC0ui64);
			v66 = ((int)a2 - 1) & 0x3F;
			v18 = 0;
			v63 = &v17[v66 - 63];
			v19 = a1 + 1;
			do
			{
				v20 = *v19;
				v21 = v19[5];
				v22 = v19[4];
				v19 += 8;
				v23 = v15 + v16 + v20 + v11;
				v24 = v14 + v12;
				v25 = __ROL8__(v21 + v13 + v16, 22);
				v26 = *(v19 - 6);
				v27 = *(v19 - 9) - 0x4B6D499041670D8Di64 * v13;
				v28 = v18 ^ (0xB492B66FBE98F273ui64 * __ROL8__(v23, 27));
				v16 = v11 + v22 - 0x4B6D499041670D8Di64 * v25;
				v29 = v14 + v27 + v26;
				v30 = v27;
				v31 = *(v19 - 7);
				v15 = 0xB492B66FBE98F273ui64 * __ROL8__(v24, 31);
				v32 = v31 + v20 + v27;
				v33 = v32 + v26;
				v34 = *(v19 - 2);
				v65 = v33;
				v35 = v30 + __ROR8__(v28 + v29, 21);
				v36 = v15 + v64 + *(v19 - 5);
				v13 = v35 + __ROL8__(v32, 20);
				v37 = v22 + v21 + v36;
				v38 = v36 + __ROR8__(v16 + v34 + v31 + v36, 21);
				v11 = v65;
				v14 = v37 + v34;
				v18 = __ROL8__(v37, 20) + v38;
				v12 = v28;
				v64 = v18;
			}             while (v19 - 1 != (__int64*)v17);
			v39 = *((_QWORD*)v63 + 1);
			v40 = v14 + v66 + v65;
			v41 = 2i64 * (unsigned __int8)v28 - 0x4B6D499041670D8Di64;
			v42 = v40 + v14 + v66;
			v43 = 9 * v18;
			v44 = *((_QWORD*)v63 + 3);
			v45 = v41 * __ROL8__(v42 + v28, 31);
			v46 = v43 ^ (v41 * __ROL8__(v16 + v40 + v15 + v39, 27));
			v47 = 9 * v40 + *((_QWORD*)v63 + 5) + v41 * __ROL8__(v16 + v13 + *((_QWORD*)v63 + 6), 22);
			v48 = *(_QWORD*)v63 + v13 * v41;
			v49 = v42 + v48 + v44;
			v50 = *((_QWORD*)v63 + 2);
			v51 = v50 + v39 + v48;
			v52 = v51 + v44;
			v53 = *((_QWORD*)v63 + 7);
			v54 = __ROL8__(v51, 20) + v48 + __ROR8__(v46 + v49, 21);
			v55 = v45 + v64 + *((_QWORD*)v63 + 4);
			v56 = *((_QWORD*)v63 + 5) + *((_QWORD*)v63 + 6) + v55;
			v57 = __ROR8__(v47 + v50 + v55 + v53, 21);
			v58 = v56 + v53;
			v59 = __ROL8__(v56, 20);
			v60 = v41
				* ((v59 + v57 + v55) ^ (v41 * (v54 ^ (v59 + v57 + v55))) ^ ((v41
					* (v54 ^ (unsigned __int64)(v59 + v57 + v55))) >> 47));
			v61 = v45 + v41 * (v60 ^ (v60 >> 47));
			v62 = v41
				* (v61 ^ (v46
					+ v41
					* ((v41 * ((v41 * (v52 ^ v58)) ^ v58 ^ ((v41 * (v52 ^ v58)) >> 47))) ^ ((v41
						* ((v41 * (v52 ^ v58)) ^ v58 ^ ((v41 * (v52 ^ v58)) >> 47))) >> 47))
					- 0x3C5A37A36834CED9i64 * (v47 ^ (v47 >> 47))));
			return v41 * ((v41 * (v61 ^ v62 ^ (v62 >> 47))) ^ ((v41 * (v61 ^ v62 ^ (v62 >> 47))) >> 47));
		}
		else
		{
			return sub_1419985E0((uint64_t*)a1, a2);
		}
	}
	else if (a2 > 0x10)
	{
		v5 = a1[1];
		v6 = 0xB492B66FBE98F273ui64 * *a1;
		v7 = 2 * a2 - 0x651E95C4D06FBFB1i64;
		v8 = v7 * *(_QWORD*)((char*)a1 + a2 - 8);
		v9 = v6 + v8 + __ROR8__(v5 - 0x651E95C4D06FBFB1i64, 18);
		v10 = v7
			* (v9 ^ (0x9AE16A3B2F90404Fui64 * *(_QWORD*)((char*)a1 + a2 - 16)
				+ __ROR8__(v8, 30)
				+ __ROL8__(v5 + v6, 21)));
		return v7 * ((v7 * (v9 ^ v10 ^ (v10 >> 47))) ^ ((v7 * (v9 ^ v10 ^ (v10 >> 47))) >> 47));
	}
	else
	{
		return HashLen0to16((uint64_t*)a1, a2);
	}
}
uint64_t idType::calculate_field_name_hash(const char* name, size_t length) {
	return cryptstr_thunked((long long*)name, length);
}


classVariableInfo_t* mh_fast_field_get(classTypeInfo_t* cls, de_prop_e propert) {
	classTypeInfo_t* currsrch = cls;

	do {
		mh_classtypeextra_t* xtra = (mh_classtypeextra_t*)currsrch->metaData;

		if (!xtra->m_num_fields)
			return nullptr;


		de_prop_e* props = mh_lea<de_prop_e>(xtra, xtra->m_offset2fields);

		unsigned propidx = sh::ibulk::find_first_equal16((unsigned short*)props, xtra->m_num_fields, propert);

		
		if (propidx != ~0u) {

			classVariableInfo_t* var = &currsrch->variables[propidx];

			return var;

		}
		currsrch = idType::get_class_superclass(currsrch);

	} while (currsrch);
	return nullptr;
}

CACHED_SIMPLE_VTBL(idClass);

idTypeInfo* idType::get_typeinfo_root() {
	void* typegetter = vtb_idClass::Get()[VTBLOFFS_IDCLASS_GETTYPE / 8];

	return call_as<idTypeInfo*>(typegetter);

}
static std::vector<unsigned> g_entity_inheritors{};

static void build1(idTypeInfo* rva) {
	g_entity_inheritors.push_back(to_de_rva(rva));

	for (auto child = rva->node.child; child; child = child->sibling) {
		build1(child->owner);
	}
	
}
static int qsort_rva(const void* x, const void* y) {
	return *(const int*)x - *(const int*)y;

}
MH_NOINLINE
MH_REGFREE_CALL
CS_COLD_CODE
static void build_entity_inheritor_set() {

	void* typegetter = vtb_idEntity::Get()[VTBLOFFS_IDCLASS_GETTYPE / 8];

	idTypeInfo* enttype = call_as<idTypeInfo*>(typegetter);

	build1(enttype);
	//ensure there are no duplicate rvas (should not be possible, lets just be sure)
	std::vector<unsigned> localinher = std::move(g_entity_inheritors);
	qsort(&localinher[0], localinher.size(), sizeof(localinher[0]), qsort_rva);
	unsigned i = 0;
	unsigned previous_value = ~0u;

	g_entity_inheritors.reserve(localinher.size());
	for (; i < localinher.size(); ++i) {
		unsigned value = localinher[i];

		if (value == previous_value)
			continue;

		g_entity_inheritors.push_back(value);
		previous_value = value;

	}
	g_entity_inheritors.shrink_to_fit();

}

std::vector<unsigned>* idType::get_entity_inheritors() {
	MH_LIKELY_IF (g_entity_inheritors.size() != 0) {
		return &g_entity_inheritors;
	}
	else {
		build_entity_inheritor_set();
		return &g_entity_inheritors;
	}
}

static bool ops_has_ptr(const char* s) {
	if (!s)
		return false;
	for (unsigned i = 0; s[i]; ++i) {
		if (s[i] == '*')
			return true;
	}
	return false;

}

static bool ops_is_only_ptr(const char* s) {
	return s[0] == '*' && !s[1];
}
//todo:resolve typedefs, handle arrays, handle typedef ops
static std::string stringify_properties(void* obj, classTypeInfo_t* clstype, std::string tabulation) {

	char tmpbuf[128];
	std::string result = "";
	auto vr = clstype->variables;
	if (!vr) {

		return "<NO REFLECTION DATA AVAILABLE>";
	}
	
	while (vr->name && vr->name[0]) {
		using namespace sh::string;

		result += tabulation;

		result += vr->type;
		result += " ";
		result += vr->name;
		result += " = ";
		unsigned voffs = vr->offset;

		classTypeInfo_t* fldclass = nullptr;

		if (ops_has_ptr(vr->ops)) {
			void* currptr = *mh_lea<void*>(obj, voffs);
			if (streq(vr->type, "char") && ops_is_only_ptr(vr->ops)) {

				if (!currptr) {
					result += "NULL";
				}
				else {
					result += "\"";
					result += (const char*)currptr;
					result += "\"";
				}
				
			}
			else {
				sprintf_s(tmpbuf, "%p", currptr);
				result += (const char*)(&tmpbuf[0]);
			}
		}
		else {
			if (streq(vr->type, "float")) {

				result += std::to_string(*mh_lea<float>(obj, voffs));

			}
			else if (streq(vr->type, "double")) {

				result += std::to_string(*mh_lea<double>(obj, voffs));

			}
#define	HANDLE_PRIM_INTEGRAL_TYPEDEF(name, typeld)	\
			else if (streq(vr->type, #name)) {\
			typeld val = 0;\
			if (vr->get) {\
				val = vr->get(obj);\
			}\
			else {\
				val = *mh_lea<typeld>(obj, voffs);\
			}\
			result += std::to_string(val);\
			}
#define	HANDLE_PRIM_INTEGRAL(name)		HANDLE_PRIM_INTEGRAL_TYPEDEF(name, name)
			HANDLE_PRIM_INTEGRAL(int)
			HANDLE_PRIM_INTEGRAL(bool)
			HANDLE_PRIM_INTEGRAL(short)
			HANDLE_PRIM_INTEGRAL(char)
			HANDLE_PRIM_INTEGRAL(long long)
			HANDLE_PRIM_INTEGRAL(unsigned int)
			HANDLE_PRIM_INTEGRAL(unsigned short)
			HANDLE_PRIM_INTEGRAL(unsigned char)
			HANDLE_PRIM_INTEGRAL(unsigned long long)
			HANDLE_PRIM_INTEGRAL_TYPEDEF(byte, unsigned char)
			HANDLE_PRIM_INTEGRAL_TYPEDEF(uint16, unsigned short)
			else {
				fldclass = idType::get_field_class(vr);

				if (fldclass) {
					result += idType::stringify_object(mh_lea<void>(obj, voffs), fldclass, tabulation + "\t");

				}
				else {
					auto fldenum = idType::FindEnumInfo(vr->type);

					if (fldenum) {
						long long val = get_classfield_int(obj, vr);

						if (fldenum->flags & ENUMFLAG_IS_BITFLAGS) {
							if (!val) {
								result += "<No flags set>";

							}
							else {

								unsigned added_one = 0;
								
								while (val) {

									auto valcount = std::countl_zero<unsigned long long>(val);

									unsigned long long highbit = 1ULL << 63;

									auto current_bit = highbit >> valcount;

									val ^= current_bit;

									auto current_bit_name = idType::get_enum_member_name_for_value(fldenum, current_bit);

									if(added_one) {
										result += " | ";
									}
									if (!current_bit_name) {
										result += "COULDNTFINDBIT";

									}
									else
										result += current_bit_name;

									added_one |= 1;

								}
							}
							
						}
						else
							result += idType::get_enum_member_name_for_value(fldenum, val);

					}
					else {
						result += "<UNHANDLED>";
					}
				}
			}

		}
		if (fldclass) {
			result += "\n";
		}
		else
			result += ";\n";
		++vr;


	}
	return result;
}

static std::string stringify_inner(void* obj, classTypeInfo_t* clstype, std::string tabulation ) {
	auto supertype = idType::get_class_superclass(clstype);
	std::string result = "";

	if (supertype) {
		result = stringify_inner(obj, supertype, tabulation);

	}
	return result + stringify_properties(obj, clstype, tabulation);

}

std::string idType::stringify_object(void* obj, classTypeInfo_t* clstype,std::string tabulation ) {
	return  "{\n" + stringify_inner(obj, clstype,tabulation) + tabulation + "}";



}