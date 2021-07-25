#include "mh_defs.hpp"
#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "idtypeinfo.hpp"
#include "scanner_core.hpp"
#include "snaphakalgo.hpp"
#include <string>
#include <set>
#include <vector>
#include <algorithm>
static constexpr const char* g_all_entity_names[] = {
"jakesDevWidget","idWorldspawn","idWorldClipBounds","idWeaponEntity","idWaveShield","idWaterEntity","idVolume_VisualImpairment","idVolume_Trigger","idVolume_TraversalGen","idVolume_ToggleableDamageOverTime","idVolume_TogglePlayerBodyReaction","idVolume_StreamTreeMarkup","idVolume_StealthField","idVolume_StatusEffect","idVolume_Stairs","idVolume_SpaceWarp","idVolume_Siphon","idVolume_SecretHint","idVolume_RuneHint","idVolume_RunCycleHint","idVolume_RevivePlayer","idVolume_PlayerViewEffect","idVolume_PlayerUseProxy","idVolume_PlayerEnvOverride","idVolume_PlayerBodyReaction","idVolume_PlasmaPuddleFX","idVolume_PlasmaPuddle","idVolume_PlacedFlightVolume","idVolume_OliviasGuardFight_DangerZone","idVolume_MatterBallWarp","idVolume_MancubusSteamFX","idVolume_MancubusSteam","idVolume_MancubusFlamethrower","idVolume_LocationCalling","idVolume_LightRigModifier","idVolume_KillDownedInjured","idVolume_InvasionSpawnZone","idVolume_InvasionInhibit","idVolume_Invasion","idVolume_Gravity","idVolume_ForceDormant","idVolume_FlightObstacle","idVolume_Flight","idVolume_EmpField","idVolume_DoomGravity","idVolume_DeployedLaser","idVolume_DemonicPossession","idVolume_DemonicBait","idVolume_DamageOverTime","idVolume_Damage","idVolume_CustomLedgeGrab","idVolume_ControlPointSpawnInfluence","idVolume_Blocking","idVolume","idUtilityCamera","idUmbraVolume","idUmbraVisibilityContributionVolume","idTurret","idTrigger_VoiceCommunication","idTrigger_VisibilityController","idTrigger_TestPlayerState","idTrigger_Teleporter_Fade","idTrigger_Teleporter","idTrigger_TakeDamage","idTrigger_StatDrivenEvent","idTrigger_SonicBoom","idTrigger_Repulsor","idTrigger_RemoveInventoryItems","idTrigger_RemoveEntities","idTrigger_Push","idTrigger_Multiplayer","idTrigger_MovementModifier","idTrigger_ModPlayerVelocity","idTrigger_InvasionAreaExit","idTrigger_InvasionAreaEnter","idTrigger_Hurt","idTrigger_GorillaBar","idTrigger_ForcePlayerBodyReactionOnce","idTrigger_Facing","idTrigger_EnergyField","idTrigger_DynamicHurt","idTrigger_DummyFire","idTrigger_Dormancy","idTrigger_DemonDamageOutModifier","idTrigger_Damage","idTrigger_ChallengeBoundry","idTrigger_BouncePad","idTrigger_AIInteraction","idTriggerToucher","idTrigger","idTrailerCamera","idToucher","idTexlodNodeGenerationArea","idTest_Sector","idTest_MetaData_WebPath","idTest_MetaData_Target","idTest_MetaData_String","idTest_MetaData_SectorYaw","idTest_MetaData_SectorPitch","idTest_MetaData_Plane","idTest_MetaData_MoveableSpheres","idTest_MetaData_Model","idTest_MetaData_Line","idTest_MetaData_FovYaw","idTest_MetaData_FovPitch","idTest_MetaData_Fov","idTest_MetaData_Cylinder2","idTest_MetaData_Cylinder","idTest_MetaData_Circle","idTest_MetaData_Bounds","idTestModel2","idTestEntity","idTeleporterPad","idTeamCapturePoint","idTarget_VisibilityController","idTarget_ViewPos","idTarget_UnlockUltraNightmare","idTarget_TutorialGui","idTarget_Timer","idTarget_Timeline","idTarget_TestPlayerState","idTarget_Teleport","idTarget_SwapNavMesh","idTarget_SwapFaction","idTarget_Subtitle","idTarget_StartSoundShader","idTarget_Spawn_Target","idTarget_Spawn_Parent","idTarget_Spawn","idTarget_SoundDuck","idTarget_Snap_Objective","idTarget_ShowHands","idTarget_ShowGui","idTarget_Show","idTarget_ShakeTrigger","idTarget_SetSpawnSpot","idTarget_SetInEncounterGroup","idTarget_SetGroupCombatStage","idTarget_Secret","idTarget_RemoveSaveGate","idTarget_RemoveItems","idTarget_Remove","idTarget_RandomImpulse","idTarget_Print","idTarget_PodiumSpawn","idTarget_PlayerWhiplash","idTarget_PlayerViewEffect","idTarget_PlayerStatus","idTarget_PlayerStatModifier","idTarget_PlayerModifier","idTarget_PlayerInvulnerability","idTarget_PlayerCheckpoint","idTarget_PlayVoiceOver","idTarget_Ping","idTarget_Perk","idTarget_Path","idTarget_POI","idTarget_Objective_Triggered","idTarget_Objective_Replace","idTarget_Objective_HideEntities","idTarget_Objective_Give","idTarget_Objective_Complete","idTarget_Notification","idTarget_ModifyTraversalClass","idTarget_ModifyGroup","idTarget_Melee","idTarget_MapGroupUnlock","idTarget_MakeActivatable","idTarget_LightController","idTarget_LevelTransition","idTarget_LayerStateChange","idTarget_InventoryCheck","idTarget_Intro","idTarget_InteractionAction","idTarget_HideHands","idTarget_Hide","idTarget_GroupMessage","idTarget_GroupExpression","idTarget_GiveWeaponUpgradePoints","idTarget_GiveItems","idTarget_GeomCacheStreamer","idTarget_GameStateIntSet","idTarget_GameStateIntInc","idTarget_GameChallengeGameOver","idTarget_GUICommand","idTarget_ForceGroupRole","idTarget_ForceDormancy","idTarget_FirstThinkActivate","idTarget_FireWeapon","idTarget_FastTravelUnlock","idTarget_FastTravelInhibit","idTarget_FakeEnemy","idTarget_FadeComplete","idTarget_EquipItem","idTarget_Enemy","idTarget_EndOfCampaign","idTarget_EndInvasion","idTarget_EncounterChallenge","idTarget_EnableTarget","idTarget_EnableGroup","idTarget_EnableAIEvent","idTarget_DynamicChallenge_Start","idTarget_DynamicChallenge_PointTrigger","idTarget_DynamicChallenge_GiveBonus","idTarget_DynamicChallenge_FailChallenge","idTarget_DynamicChallenge_End","idTarget_DummyFire","idTarget_DormancyRadius","idTarget_Disconnect_GoToScreen","idTarget_Disconnect","idTarget_DisableInvasion","idTarget_DisableEscMenu","idTarget_DevLoadoutSwap","idTarget_DemonBountyAiTypes","idTarget_DeactivateStatusEffects","idTarget_Damage","idTarget_Cvar","idTarget_CurrencyCheck","idTarget_Credits","idTarget_Count_Random","idTarget_Count","idTarget_ConditionalAccessor","idTarget_Conditional","idTarget_Command","idTarget_CollisionDamage","idTarget_Codex","idTarget_ClearFakeEnemy","idTarget_ChangeVolume_PlayerEnvOverride","idTarget_ChangeMaterial","idTarget_ChangeColor","idTarget_Break","idTarget_Award_RushAttack","idTarget_Award_Adrenaline","idTarget_ApplyImpulse","idTarget_ApplyExplosionImpulse","idTarget_AnimWebUnpause","idTarget_AnimWebPause","idTarget_AnimWebChangeStateVia","idTarget_AnimWebChangeState","idTarget_AmplitudeTrigger","idTarget_AdaptiveTickToggle","idTarget_ActionScript","idTarget_AIProxy","idTarget_AIGlobalSettings","idTarget_AIEvent","idTargetSpawnGroup","idTarget","idSyncEntity","idSummoningTemplate","idStaticWaterEntity","idStaticVisibilityBlocker","idStaticMultiGuiEntity","idStaticEntity","idSpringCamera","idSplinePath","idSpitfireCannon","idSpectatorCamera","idSpectacleCamera","idSpawnPoint","idSpawnNodeSplines","idSpawnNode","idSpawnArea","idSoundTrigger","idSoundSubtitlesEntity","idSoundSphereEntity","idSoundPrefetchEntity","idSoundEntity","idSoundBoxEntity","idSoundBeamEntity","idSoundAreaEntity","idSlowMotionCamera","idRotatableCamera","idRiftBuildPosition","idRibbon2Emitter","idResurrectionProxy","idRegenArea","idReferenceMap","idPurifyingBeam","idProp_WeaponStatic","idProp_WeaponPickup","idProp_Usable","idProp_Static","idProp_Spawnable","idProp_PlayableDemonCircle","idProp_Pickup","idProp_OnlineCollectible","idProp_Moveable","idProp_HealthPickup","idProp_Explosive","idProp_ElectricArmor","idProp_DemonCircle","idProp_Coop_Billboard","idProp_Coop","idProp_ContinuallyUsed","idProp_BreakableLoot","idProp_Breakable","idProp_ArmorPickup","idProp_AIArmor","idProp2","idProp","idProjectile_SwarmMissile_V2","idProjectile_SwarmMissile","idProjectile_SiphonGrenade","idProjectile_RollingFire","idProjectile_Rocket","idProjectile_MeatHook","idProjectile_Mancubus_Smoke","idProjectile_GrenadeFast","idProjectile_Grenade","idProjectile_EMP","idProjectile_Destroyer","idProjectile_DamageOverTime","idProjectile_CyberdemonSwarmMissile","idProjectile_CacoDemonRocket","idProjectile_BfgBurn","idProjectile_BfgArc","idProjectile_Auger","idProjectile_AIHomingRocket","idProjectileShield","idProjectile","idPoseableEntity","idPortalWorld","idPortalSurface","idPlayerStart","idPlayer","idPieceEmitter","idPhotoModeCamera","idPerceptionVolume","idPathCorner","idParticleEmitter","idOverTheShoulderCamera","idObjective_Relay","idNetworkedParticleEmitter","idNetworkedEntityFx","idNetworkLight","idMusicEntity","idMultiplayerTrigger","idMoverModifier","idMover","idMidnightCutscene","idLowGravityMover","idLogicEntity","idLight","idLensFlare","idJostleSwivel","idJostleSpring","idJostleAnimated","idItemPropSpawner","idInventoryStorage","idInvasionBlocker","idInvasionAreaManager","idInteractionCamera","idInteractable_WorldCache","idInteractable_WeaponModBot","idInteractable_VegaTraining","idInteractable_Tutorial","idInteractable_SonicBoost","idInteractable_SlayerGate_Coop","idInteractable_SlayerGate","idInteractable_Shooter","idInteractable_Rune","idInteractable_Respec","idInteractable_Obstacle","idInteractable_NightmareMarker","idInteractable_Moveable","idInteractable_Minigame","idInteractable_LootDrop","idInteractable_LootCrate","idInteractable_JukeBox","idInteractable_HighlightSelector","idInteractable_GoreNest","idInteractable_GoreBud","idInteractable_GiveItems","idInteractable_EliteGuard_Coop","idInteractable_EliteGuard","idInteractable_Doom","idInteractable_CurrencyExchange","idInteractable_CollectibleViewer","idInteractable_Challenge_Shrine","idInteractable_BatterySocket","idInteractable_Automap","idInteractable","idInfo_UniversalTraversal","idInfo_Trigger_Facing_Target","idInfo_TraversalPoint","idInfo_TraversalChain","idInfo_TraversalBase","idInfo_TeleportDestination","idInfo_SpawnConditionProxy","idInfo_FastTravel","idInfo_DoorTraversalChain","idInfo_BounceDestination","idInfoWaitForButtonAfterLoad","idInfoTraversal_Ultimate","idInfoTraversal_Bot","idInfoTraversalEndPoint","idInfoTraversal","idInfoTexLod","idInfoStandPoint","idInfoSplineChild","idInfoSpawnPoint","idInfoRuler","idInfoRoam","idInfoPlayerHud","idInfoPath","idInfoOrbit","idInfoLookTargetGroup","idInfoLookTarget","idInfoLevelFadeIn","idInfoGoalPosition","idInfoFocus","idInfoFlightVolumeEntrance","idInfoExportHint","idInfoDebugMarker","idInfoCoverExposed","idInfoCover","idInfoCloudShot","idInfoBounds","idInfoAmbient","idInfoAAS","idInfo","idInfluenceSpawnSettings","idIOS_MeteorCrater","idHeightmapVolumeEntity","idGuiEntity_Text","idGuiEntity_Cinematic","idGuiEntity","idGoreEntity","idGladiatorShield","idGeomCacheEntity","idGameChallenge_Shell","idGameChallenge_PVPTutorial","idGameChallenge_PVP","idGameChallenge_CampaignSinglePlayer","idGameChallenge","idFuncSwing","idFuncShadowCaster","idFuncRotate","idFreeDbgCam","idFreeCamera","idFlightVolumeTraversalSpline","idExtraLifeTeleportLocation","idEscapePod","idEnvironmentalDamage_PointManager_Trigger","idEnvironmentalDamage_Point","idEnvironmentalDamage_Hurt_Trigger","idEnvArea","idEntityFxRandom","idEntityFx","idEntityCamera","idEntity","idEngineEntity","idEnergyShell","idEncounterVolume_ValidCover","idEncounterTrigger_RaiseUserFlag","idEncounterTrigger_PreCombat","idEncounterTrigger_OverrideCombatGrouping","idEncounterTrigger_Exit","idEncounterTrigger_Commit","idEncounterTrigger_AmbientAIAudio","idEncounterModifier_SetNextScriptIndex","idEncounterModifier_SetEnabledState","idEncounterModifier_Reset","idEncounterManager","idEncounterGroupMgr","idEncounterAmbientAudio","idElectricBoltEmitter","idEditorModelEntity","idDynamicVisibilityBlocker","idDynamicEntity_Damageable","idDynamicEntity","idDestructibleManager","idDestructible","idDestroyableProp","idDemonPlayer_Summoner","idDemonPlayer_Revenant","idDemonPlayer_Prowler","idDemonPlayer_Pinky","idDemonPlayer_PainElemental","idDemonPlayer_Marauder","idDemonPlayer_Mancubus","idDemonPlayer_Cacodemon","idDemonPlayer_Baron","idDemonPlayer_Archvile","idDemonPlayer_Arachnotron","idDemonPlayerStart","idDemonPlayer","idDecalEntity_Coop","idDecalEntity","idDebrisEntity","idDeathmatchPlayerStart","idDamageableAttachment","idCyberdemonMissileShooter","idCrusher","idCoopStartingInventory","idCombatVolume","idCombatGrouping","idCollisionStreamArea","idCollisionExclusionVolume","idCollectibleEntity","idCinematicCamera","idCaptureFrames","idCampaignInvaderStart","idCameraView","idCamera","idBuildArea","idBreakablePlatform","idBreakable","idBotPathNodeEntity","idBotActionEntity_Usable","idBotActionEntity_Info_RoamGoal","idBotActionEntity_Info_POI","idBotActionEntity_Info","idBotActionEntity","idBossInfo","idBloatedEntity","idBillboard","idBfgBurn","idBasePropSpawner","idBarricade","idBFG_Geyser","idAutomapSectionRevealTrigger","idAutomapMapGroupRevealEntity","idArchvileTemplate","idAnnouncementManager","idAnimated_ThreatSensor","idAnimated_AnimWeb_Samuel","idAnimated_AnimWeb","idAnimatedSimple_Faust","idAnimatedSimple_AnimSys","idAnimatedInteractable","idAnimatedEntity","idAnimatedAttachment_AF","idAnimatedAttachment","idAnimated","idAnimNode","idAnimCamera","idAmbientTriggerModifier_SetAITypes","idAmbientNodeGenerationArea","idAmbientNodeExclusionArea","idAlignedEntity","idActorWorkNode","idActorAmbientFilter","idActor","idActionNode","idAbnormalPlayerMetrics","idAITest_Rotation","idAITest","idAISearchHintGroup","idAISearchHintAuto","idAISearchHintAnimation","idAIMapMarkupEntity","idAIInteraction","idAIHintGroup","idAIHint","idAIDebugCamera","idAIDeathVolume","idAICombatHint_SuppressionFirePoint","idAICombatHint","idAIAASHint","idAI2","idAFEntity_GetUpTest","idAFEntity_Generic","idAFEntity_Dummy","idAFEntity_Corpse","idAASReachability","idAASObstacle"
};

static constexpr const char* g_all_types[] =
#include "alltypes.h"
;
//#include "types_with_vtables.hpp"

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
static void* get_typeinfo_generated(unsigned which = 0) {

	void* typeinfo_tools = *(void**)descan::g_global_typeinfo_tools;

	if (g_offset_typeinfo == -1) {
		/*
			shit, these dont exist in v6
		*/
		int offs_to_generated = idType::FindClassField("idTypeInfoTools", "generatedTypeInfo")->offset;


		int sizeof_gentype = idType::FindClassInfo("idTypeInfoTools::registeredTypeInfo_t")->size;


		char* generated_ptr = (char*)typeinfo_tools;
		generated_ptr += offs_to_generated;

		g_offset_typeinfo = offs_to_generated;
		g_offset_typeinfo2 = offs_to_generated + sizeof_gentype;

	}

	void* typeinfogenerated = *reinterpret_cast<void**>(reinterpret_cast<char*>(typeinfo_tools) + (!which ? g_offset_typeinfo : g_offset_typeinfo2));

	return typeinfogenerated;

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
	if (g_offset_classtypes == -1) {
		g_offset_classtypes = FindClassField("typeInfoGenerated_t", "classes")->offset;
		g_offset_nclasstypes = FindClassField("typeInfoGenerated_t", "numClasses")->offset;
	}
	char* typegen = (char*)get_typeinfo_generated(whichsource);


	classTypeInfo_t* classes = *reinterpret_cast<classTypeInfo_t**>(typegen + g_offset_classtypes);

	out_n = *reinterpret_cast<unsigned*>(typegen + g_offset_nclasstypes);
	return classes;
}
enumTypeInfo_t* idType::EnumTypes(unsigned& out_n, unsigned whichsource) {
	if (g_offset_enums == -1) {
		g_offset_enums = FindClassField("typeInfoGenerated_t", "enums")->offset;
		g_offset_nenums = FindClassField("typeInfoGenerated_t", "numEnums")->offset;
	}
	char* typegen = (char*)get_typeinfo_generated(whichsource);


	enumTypeInfo_t* enums = *reinterpret_cast<enumTypeInfo_t**>(typegen + g_offset_enums);

	out_n = *reinterpret_cast<unsigned*>(typegen + g_offset_nenums);
	return enums;
}
typedefInfo_t* idType::TypedefTypes(unsigned& out_n, unsigned whichsource) {
	if (g_offset_typedefs == -1) {
		g_offset_typedefs = FindClassField("typeInfoGenerated_t", "typedefs")->offset;
		g_offset_ntypedefs = FindClassField("typeInfoGenerated_t", "numTypedefs")->offset;
	}
	char* typegen = (char*)get_typeinfo_generated(whichsource);


	typedefInfo_t* typs = *reinterpret_cast<typedefInfo_t**>(typegen + g_offset_typedefs);

	out_n = *reinterpret_cast<unsigned*>(typegen + g_offset_ntypedefs);
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
	for (classTypeInfo_t* clptr = clstype; clptr && !located_var; clptr = FindClassInfo(clptr->superType)) {
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
		onclass = FindClassInfo(field->type);
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