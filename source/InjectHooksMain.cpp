#include "StdInc.h"

// Audio
// -- General
#include "AEAudioEnvironment.h"
#include "AEAudioUtility.h"
#include "AESmoothFadeThread.h"
#include "AESound.h"
#include "AEStreamThread.h"
#include "AudioEngine.h"
#include "AudioZones.h"
// -- Entities
#include "AEAudioEntity.h"
#include "AECollisionAudioEntity.h"
#include "AEDoorAudioEntity.h"
#include "AEExplosionAudioEntity.h"
#include "AEFireAudioEntity.h"
#include "AEFrontendAudioEntity.h"
#include "AEGlobalWeaponAudioEntity.h"
#include "AEPedAudioEntity.h"
#include "AEPedlessSpeechAudioEntity.h"
#include "AEPedSpeechAudioEntity.h"
#include "AEPedWeaponAudioEntity.h"
#include "AEPoliceScannerAudioEntity.h"
#include "AEScriptAudioEntity.h"
#include "AETwinLoopSoundEntity.h"
#include "AEVehicleAudioEntity.h"
#include "AEWaterCannonAudioEntity.h"
#include "AEWeaponAudioEntity.h"
#include "AEWeatherAudioEntity.h"
// -- Hardware
#include "AEAudioChannel.h"
#include "AEAudioHardware.h"
#include "AEStaticChannel.h"
#include "AEStreamingChannel.h"
// -- Loaders
#include "AEBankLoader.h"
#include "AEDataStream.h"
#include "AEMFDecoder.h"
#include "AEMP3BankLoader.h"
#include "AEMP3TrackLoader.h"
#include "AEStreamingDecoder.h"
#include "AEStreamTransformer.h"
#include "AETrackLoader.h"
#include "AEVorbisDecoder.h"
#include "AEWaveDecoder.h"
#include "AEWMADecoder.h"
// -- Managers
#include "AEAmbienceTrackManager.h"
#include "AECutsceneTrackManager.h"
#include "AERadioTrackManager.h"
#include "AESoundManager.h"
#include "AEUserRadioTrackManager.h"


#include "Garage.h"
#include "Garages.h"

// FX
#include "FxSystemBP.h"
#include "FxSystem.h"
#include "FxSphere.h"
#include "FxPrimBP.h"
#include "FxPrim.h"
#include "FxMemoryPool.h"
#include "FxManager.h"
#include "FxFrustumInfo.h"
#include "FxEmitterPrt.h"
#include "FxEmitterBP.h"
#include "FxEmitter.h"
#include "Fx.h"

#include "UIRenderer.h"

#include "CarGenerator.h"
#include "TheCarGenerators.h"
#include "Radar.h"
#include "WaterCannons.h"
#include "Lines.h"
#include "Escalators.h"
#include "MovingThings.h"
#include "PlaneTrail.h"
#include "PlaneTrails.h"
#include "Gamma.h"
#include "CustomBuildingPipeline.h"
#include "CustomBuildingDNPipeline.h"
#include "CustomCarEnvMapPipeline.h"
#include "CustomBuildingRenderer.h"
#include "PedType.h"
#include "Occlusion.h"
#include "Occluder.h"
#include "ActiveOccluder.h"
#include "MotionBlurStreaks.h"
#include "GroupEventHandler.h"
#include "SimpleVariablesSaveStructure.h"
#include "BreakManager_c.h"
#include "Buoyancy.h"
#include "CreepingFire.h"
#include "Restart.h"
#include "SetPiece.h"
#include "SetPieces.h"
#include "BulletInfo.h"
#include "Explosion.h"
#include "C_PcSave.h"
#include "FireManager.h"
#include "Skidmarks.h"
#include "CarCtrl.h"
#include "TagManager.h"
#include "Clouds.h"
#include "StuntJumpManager.h"
#include "CustomCarPlateMgr.h"
#include "WindModifiers.h"
#include "PedPlacement.h"
#include "UpsideDownCarCheck.h"
#include "PedClothesDesc.h"
#include "WaterCannon.h"
#include "EntryExitManager.h"
#include "ObjectSaveStructure.h"
#include "PedStats.h"
#include "TrafficLights.h"
#include "CurrentVehicle.h"
#include "PlaceName.h"
#include "UserDisplay.h"
#include "OnscreenTimer.h"
#include "OnscreenCounterEntry.h"
#include "PostEffects.h"
#include "Rope.h"
#include "Ropes.h"
#include "FallingGlassPane.h"
#include "Glass.h"
#include "FurnitureGroup_c.h"
#include "FurnitureManager_c.h"
#include "FurnitureSubGroup_c.h"
#include "Interior_c.h"
#include "InteriorGroup_c.h"
#include "InteriorManager_c.h"
#include "Checkpoint.h"
#include "Checkpoints.h"
#include "IKChain_c.h"
#include "IKChainManager_c.h"
#include "BoneNode_c.h"
#include "BoneNodeManager_c.h"
#include "TheScripts.h"
#include "RunningScript.h"
#include "Scripted2dEffects.h"
#include "ScriptResourceManager.h"
#include "PedAttractorManager.h"
#include "Attractors/PedAttractorPedPlacer.h"
#include "LoadingScreen.h"
#include "Securom.h"
#include "GridRef.h"
#include "MenuSystem.h"
#include "InterestingEvents.h"
#include "Shadows.h"
#include "ShadowCamera.h"
#include "VehicleRecording.h"
#include "Birds.h"
#include "Hud.h"
#include "CarFXRenderer.h"
#include "Curves.h"
#include "ProcObjectMan.h"
#include "ProcSurfaceInfo.h"
#include "Pickup.h"
#include "Pickups.h"
#include "PedIK.h"
#include "HandShaker.h"
#include "TempColModels.h"
#include "ControllerConfigManager.h"

// Plant
#include "PlantMgr.h"
#include "PlantColEntEntry.h"
#include "PlantLocTri.h"
#include "GrassRenderer.h"
#include "PPTriPlantBuffer.h"
#include "PlantSurfPropMgr.h"

// Tasks
#include "TaskSimpleKillPedWithCar.h"
#include "TaskComplexSitDownThenIdleThenStandUp.h"
#include "TaskComplexLeaveCarAsPassengerWait.h"
#include "TaskComplexInvestigateDeadPed.h"
#include "TaskComplexSignalAtPed.h"
#include "TaskSimpleCarCloseDoorFromOutside.h"
#include "TaskSimpleCarFallOut.h"
#include "TaskSimpleCarGetIn.h"
#include "TaskSimpleBeHit.h"
#include "TaskSimpleHitByGunFromFront.h"
#include "TaskSimpleHitByGunFromLeft.h"
#include "TaskSimpleHitByGunFromRear.h"
#include "TaskSimpleHitByGunFromRight.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculator.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.h"
#include "TaskComplexSeekEntityAnyMeansXYOffset.h"
#include "TaskComplexFleeEntity.h"
#include "TaskComplexSmartFleeEntity.h"
#include "TaskSimpleArrestPed.h"
#include "TaskComplexDragPedFromCar.h"
#include "TaskComplexDestroyCar.h"
#include "TaskComplexDestroyCarMelee.h"
#include "TaskComplexDestroyCarArmed.h"
#include "TaskSimpleAbseil.h"
#include "TaskComplexWanderCop.h"
#include "TaskComplexEnterCarAsPassengerTimed.h"
#include "TaskSimpleWaitUntilPedIsInCar.h"
#include "TaskComplexUseMobilePhone.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleCarDrive.h"
#include "TaskSimpleScratchHead.h"
#include "TaskSimpleChoking.h"
#include "TaskComplexPartnerChat.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleGangDriveBy.h"
#include "TaskComplexObserveTrafficLightsAndAchieveHeading.h"
#include "Interior/TaskInteriorGoToInfo.h"
#include "TaskSimpleInAir.h"
#include "TaskComplexHitPedWithCar.h"
#include "TaskSimplePlayerOnFoot.h"
#include "TaskComplexStealCar.h"
#include "TaskComplexFollowPedFootsteps.h"
#include "Interior/TaskInteriorSitAtDesk.h"
#include "TaskComplexFollowLeaderAnyMeans.h"
#include "TaskSimpleFightingControl.h"
#include "TaskComplexGetUpAndStandStill.h"
#include "TaskComplexGoPickUpEntity.h"
#include "TaskSimpleDie.h"
#include "TaskSimpleDuck.h"
#include "TaskComplexPolicePursuit.h"
#include "TaskSimpleFacial.h"
#include "TaskComplexCopInCar.h"
#include "TaskComplexFacial.h"
#include "TaskComplexPlayHandSignalAnim.h"
#include "TaskComplexInWater.h"
#include "TaskComplexInAirAndLand.h"
#include "TaskSimpleGetUp.h"
#include "TaskSimpleUninterruptable.h"
#include "TaskComplexFallAndGetUp.h"
#include "TaskComplexFollowNodeRoute.h"
#include "TaskComplexFollowLeaderInFormation.h"
#include "TaskSimpleTriggerLookAt.h"
#include "TaskSimpleHitHead.h"
#include "TaskUtilityLineUpPedWithCar.h"
#include "Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.h"
#include "TaskSimpleLand.h"
#include "TaskSimpleJetPack.h"
#include "TaskSimpleSitIdle.h"
#include "Interior/TaskInteriorUseInfo.h"
#include "TaskSimpleSetStayInSamePlace.h"
#include "TaskSimpleJump.h"
#include "TaskSimpleFall.h"
#include "TaskSimpleClimb.h"
#include "TaskComplexPlayHandSignalAnim.h"
#include "TaskComplexWaitForPed.h"
#include "TaskComplexJump.h"
#include "TaskComplexWanderStandard.h"
#include "TaskComplexSunbathe.h"
#include "TaskComplexEnterCar.h"
#include "Interior/TaskInteriorBeInShop.h"
#include "TaskSimpleTogglePedThreatScanner.h"
#include "TaskComplexUseGoggles.h"
#include "TaskComplexUseEffectRunning.h"
#include "TaskComplexUseEffectSprinting.h"
#include "TaskComplexCrossRoadLookAndAchieveHeading.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskComplexWalkAlongsidePed.h"
#include "TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h"
#include "Interior/TaskInteriorBeInOffice.h"
#include "TaskSimpleGiveCPR.h"
#include "TaskSimpleCarSetPedInAsPassenger.h"
#include "TaskComplexDriveFireTruck.h"
#include "TaskSimpleSwim.h"
#include "TaskComplexWalkRoundObject.h"
#include "TaskSimplePause.h"
#include "TaskComplexShuffleSeats.h"
#include "TaskSimpleCarShuffle.h"
#include "TaskSimpleHurtPedWithCar.h"
#include "TaskComplexPassObject.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleCarShuffle.h"
#include "TaskSimpleCarSlowBeDraggedOut.h"
#include "TaskComplexReactToGunAimedAt.h"
#include "TaskSimpleNone.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskSimpleThrowProjectile.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskComplexWanderMedic.h"
#include "TaskComplexEvasiveDiveAndGetUp.h"
#include "TaskComplexMoveBackAndJump.h"
#include "TaskComplexSeekEntityAiming.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskComplexKillPedOnFootStealth.h"
#include "TaskComplexDriveWander.h"
#include "TaskSimpleStealthKill.h"
#include "TaskSimpleCarSetPedOut.h"
#include "TaskSimpleAnim.h"
#include "TaskSimpleRunAnim.h"
#include "Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h"
#include "TaskComplexWanderCriminal.h"
#include "TaskComplexWanderProstitute.h"
#include "TaskComplexWalkRoundBuildingAttempt.h"
#include "TaskSimpleLookAbout.h"
#include "TaskComplexEvasiveDiveAndGetUp.h"
#include "TaskComplexPartnerGreet.h"
#include "Interior/TaskInteriorShopKeeper.h"
#include "TaskComplexWaitForDryWeather.h"
#include "TaskSimpleShakeFist.h"
#include "TaskComplexFleeShooting.h"
#include "TaskComplexWalkRoundCar.h"
#include "TaskSimpleDuck.h"
#include "TaskComplexLeaveCarAndFlee.h"
#include "TaskComplexLeaveCarAndWander.h"
#include "TaskComplexMedicTreatInjuredPed.h"
#include "TaskComplexDiveFromAttachedEntityAndGetUp.h"
#include "SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.h"
#include "SeekEntity/TaskComplexSeekEntityXYOffset.h"
#include "SeekEntity/TaskComplexSeekEntityStandard.h"
#include "TaskComplexUseScriptedBrain.h"
#include "TaskSimplePlayHandSignalAnim.h"
#include "TaskComplexCarDrive.h"
#include "TaskComplexKillPedFromBoat.h"
#include "TaskSimpleHitWall.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarSetTempAction.h"
#include "TaskComplexWanderFlee.h"
#include "TaskComplexWaitAtAttractor.h"
#include "TaskComplexTreatAccident.h"
#include "TaskComplexGoToPointAndStandStillTimed.h"
#include "TaskComplexPartnerShove.h"
#include "TaskComplexFleeAnyMeans.h"
#include "TaskSimpleRunNamedAnim.h"
#include "TaskComplexProstituteSolicit.h"
#include "TaskComplexStuckInAir.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleCower.h"
#include "TaskSimpleHitFromBack.h"
#include "TaskSimpleHitFromBehind.h"
#include "TaskSimpleHitFromFront.h"
#include "TaskSimpleHitFromLeft.h"
#include "TaskSimpleHitFromRight.h"
#include "TaskSimpleGoTo.h"
#include "TaskSimpleUseAtm.h"
#include "TaskSimpleCarOpenLockedDoorFromOutside.h"
#include "TaskSimpleCarSetPedSlowDraggedOut.h"
#include "TaskComplexPartner.h"
#include "TaskComplexFollowPointRoute.h"
#include "TaskComplexDrivePointRoute.h"
#include "TaskComplexStareAtPed.h"
#include "TaskSimpleCarSetPedInAsDriver.h"
#include "TaskComplexWaitForBus.h"
#include "TaskSimpleWaitForBus.h"
#include "TaskSimpleStandUp.h"
#include "TaskComplexWander.h"
#include "TaskSimplePutDownEntity.h"
#include "TaskComplexUseSwatRope.h"
#include "TaskComplexRoadRage.h"
#include "TaskSimpleGoToPointFine.h"
#include "TaskComplexExtinguishFires.h"
#include "TaskComplexDie.h"
#include "TaskComplexEnterBoatAsDriver.h"
#include "TaskComplexUseAttractor.h"
#include "TaskSimpleFight.h"
#include "TaskComplexUseWaterCannon.h"
#include "TaskComplexDriveToPoint.h"
#include "TaskSimpleSetPedAsAutoDriver.h"
#include "TaskComplexWalkRoundFire.h"
#include "TaskSimpleSlideToCoord.h"
#include "TaskComplexPartnerDeal.h"
#include "TaskSimplePickUpEntity.h"
#include "TaskComplexEnterCarAsDriverTimed.h"
#include "TaskComplexBeInGroup.h"
#include "TaskComplexBeCop.h"
#include "TaskComplexAvoidOtherPedWhileWandering.h"
#include "TaskComplexArrestPed.h"
#include "TaskComplexEnterAnyCarAsDriver.h"
#include "TaskSimpleSetKindaStayInSamePlace.h"
#include "TaskSimpleSetCharIgnoreWeaponRangeFlag.h"
#include "TaskComplexCarDriveMission.h"
#include "TaskComplexLeaveAnyCar.h"
#include "TaskSimpleLeaveGroup.h"
#include "TaskSimpleCreateCarAndGetIn.h"
#include "TaskComplexWaitForBackup.h"
#include "TaskSimpleIKChain.h"
#include "TaskSimpleIKLookAt.h"
#include "TaskSimplePickUpBike.h"
#include "TaskSimpleIKManager.h"
#include "TaskSimpleGunControl.h"
#include "TaskSimpleIKPointArm.h"
#include "TaskSimpleCarSlowDragPedOut.h"
#include "TaskSimpleWaitUntilPedIsOutCar.h"
#include "TaskComplexAvoidEntity.h"
#include "TaskGangHasslePed.h"
#include "TaskGangHassleVehicle.h"
#include "TaskGoToVehicleAndLean.h"
#include "TaskSimpleSetCharDecisionMaker.h"
#include "TaskLeanOnVehicle.h"
#include "TaskComplexSeekCoverUntilTargetDead.h"
#include "TaskSimpleTurn180.h"
#include "TaskComplexCarSlowBeDraggedOut.h"
#include "TaskComplexCarSlowBeDraggedOutAndStandUp.h"
#include "TaskComplexGangFollower.h"
#include "TaskComplexGangJoinRespond.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexGetOnBoatSeat.h"
#include "TaskComplexGoToAttractor.h"
#include "TaskComplexGoToBoatSteeringWheel.h"
#include "TaskComplexUseScriptedAttractor.h"
#include "TaskSimpleSitDown.h"
#include "TaskComplexGoToCarDoorAndStandStill.h"
#include "TaskComplexGoToPointAiming.h"
#include "TaskComplexGoToPointAndStandStillAndAchieveHeading.h"
#include "TaskComplexGoToPointAnyMeans.h"
#include "TaskComplexGoToPointShooting.h"
#include "TaskComplexGotoDoorAndOpen.h"
#include "TaskComplexHitByGunResponse.h"
#include "TaskComplexHitResponse.h"
#include "TaskSimpleDrown.h"
#include "TaskComplexDieInCar.h"
#include "TaskComplexFallToDeath.h"
#include "TaskSimpleDrownInCar.h"
#include "TaskSimpleThrowControl.h"
#include "TaskSimpleDieInCar.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskComplexTrackEntity.h"
#include "TaskSimpleTired.h"
#include "TaskSimpleAffectSecondaryBehaviour.h"
#include "TaskSimpleCarWaitForDoorNotToBeInUse.h"
#include "TaskSimpleCarWaitToSlowDown.h"
#include "TaskSimpleCarAlign.h"
#include "TaskComplexLeaveBoat.h"
#include "TaskSimpleCarGetOut.h"
#include "TaskSimpleTired.h"
#include "TaskSimpleCarOpenDoorFromOutside.h"
#include "TaskSimpleCarJumpOut.h"
#include "TaskSimpleCarCloseDoorFromInside.h"
#include "TaskSimpleClearLookAt.h"
#include "TaskSimpleCarWaitToSlowDown.h"    
#include "TaskComplexFleePoint.h"
#include "TaskSimpleWaitUntilAreaCodesMatch.h"
#include "TaskSimplePlayerOnFire.h"
#include "TaskComplexOnFire.h"
#include "TaskSimpleBikeJacked.h"
#include "TaskSimpleRunTimedAnim.h"
#include "TaskSimpleSay.h"
#include "TaskComplexEnterCar.h"
#include "TaskComplexLeaveCarAndDie.h"
#include "TaskSimpleHandsUp.h"
#include "TaskComplexEnterCar.h"
#include "TaskComplexSmartFleePoint.h"
#include "Interior/TaskInteriorBeInHouse.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h"
#include "Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h"
#include "Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.h"
#include "Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.h"
#include "Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.h"

// Interior Tasks
#include "Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h"
#include "TaskComplexSmartFleePoint.h"

#include "EventSeenPanickedPed.h"
#include "EventCarUpsideDown.h"
#include "EventPassObject.h"
#include "EventLeanOnVehicle.h"
#include "EventOnFire.h"
#include "EventEscalator.h"
#include "EventCopCarBeingStolen.h"
#include "EventDanger.h"
#include "EventSeenCop.h"
#include "EventScriptedAttractor.h"
#include "EventStuckInAir.h"
#include "EventDontJoinPlayerGroup.h"
#include "EventNewGangMember.h"
#include "EventAcquaintancePedHate.h"
#include "EventAcquaintancePedHateBadlyLit.h"
#include "EventPedCollisionWithPed.h"
#include "EventPedCollisionWithPlayer.h"
#include "EventPlayerCollisionWithPed.h"
#include "EventObjectCollision.h"
#include "EventBuildingCollision.h"
#include "EventLeaderEnteredCarAsDriver.h"
#include "EventLeaderExitedCarAsDriver.h"
#include "EventLeaderQuitEnteringCarAsDriver.h"
#include "EventAreaCodes.h"
#include "EventLeaderEntryExit.h"
#include "Formation.h"

#include "Plugins/BreakablePlugin/BreakablePlugin.h"

#include "platform/win/VideoPlayer/VideoPlayer.h"
#include "platform/win/VideoMode.h"
#include "platform/win/WinPlatform.h"

#include "app/app.h"
#include <RealTimeShadowManager.h>

#include "extensions/utility.hpp"
#include "extensions/CommandLine.h"
#include <RenderBuffer.hpp>

#include "ReversibleHooks/RootHookCategory.h"

void InjectHooksMain() {
    HookInstall(0x53E230, &Render2dStuff);   // [ImGui] This one shouldn't be reversible, it contains imgui debug menu logic, and makes game unplayable without
    HookInstall(0x541DD0, CPad::UpdatePads); // [ImGui] Changes logic of the function and shouldn't be toggled on/off
    HookInstall(0x459F70, CVehicleRecording::Render); // [ImGui] Debug stuff rendering

    CFormation::InjectHooks();
    CHandShaker::InjectHooks();
    CCutsceneMgr::InjectHooks();
    CFileMgr::InjectHooks();
    CPedGroupPlacer::InjectHooks();
    CLoadedCarGroup::InjectHooks();
    RenderBuffer::InjectHooks();
    CStaticShadow::InjectHooks();
    CPedGroup::InjectHooks();
    CPedGroupMembership::InjectHooks();
    CRealTimeShadowManager::InjectHooks();
    CRealTimeShadow::InjectHooks();
    CPopCycle::InjectHooks();
    ProcObjectMan_c::InjectHooks();
    ProcSurfaceInfo_c::InjectHooks();
    RwHelperInjectHooks();
    RwCoreInjectHooks();
    CPad::InjectHooks();
    InjectCommonHooks();
    CEscalator::InjectHooks();
    CShadowCamera::InjectHooks();
    CInterestingEvents::InjectHooks();
    CShadows::InjectHooks();
    CCam::InjectHooks();
    CCarAI::InjectHooks();
    CMenuSystem::InjectHooks();
    CCarFXRenderer::InjectHooks();
    CPedAttractorManager::InjectHooks();
    CPedAttractorPedPlacer::InjectHooks();
    BoneNode_c::InjectHooks();
    BoneNodeManager_c::InjectHooks();
    IKChainManager_c::InjectHooks();
    IKChain_c::InjectHooks();
    CCheckpoint::InjectHooks();
    CCheckpoints::InjectHooks();
    CWeaponEffects::InjectHooks();
    CPedList::InjectHooks();
    CBouncingPanel::InjectHooks();
    CRope::InjectHooks();
    CRopes::InjectHooks();
    CWeaponInfo::InjectHooks();
    CCurrentVehicle::InjectHooks();
    CCurves::InjectHooks();
    CPlaceName::InjectHooks();
    CUserDisplay::InjectHooks();
    COnscreenTimer::InjectHooks();
    COnscreenCounterEntry::InjectHooks();
    CPedPlacement::InjectHooks();
    CWindModifiers::InjectHooks();
    CGenericGameStorage::InjectHooks();
    C_PcSave::InjectHooks();
    CFileLoader::InjectHooks();
    CWorld::InjectHooks();
    CStreamingInfo::InjectHooks();
    CStreaming::InjectHooks();
    InjectCdStreamHooks();
    CMirrors::InjectHooks();
    CFire::InjectHooks();
    CExplosion::InjectHooks();
    CClothesBuilder::InjectHooks();
    CClothes::InjectHooks();
    CBulletInfo::InjectHooks();
    CRestart::InjectHooks();
    CSetPiece::InjectHooks();
    CSetPieces::InjectHooks();
    CCopPed::InjectHooks();
    CCivilianPed::InjectHooks();
    CDamageManager::InjectHooks();
    CCreepingFire::InjectHooks();
    BreakManager_c::InjectHooks();
    BreakObject_c::InjectHooks();
    CFireManager::InjectHooks();
    CGroupEventHandler::InjectHooks();
    CVehicleRecording::InjectHooks();
    CBrightLights::InjectHooks();
    CShinyTexts::InjectHooks();
    CPedTaskPair::InjectHooks();
    CDirectory::InjectHooks();
    CBulletTraces::InjectHooks();
    CDebug::InjectHooks();
    CGame::InjectHooks();
    CControllerState::InjectHooks();
    CKeyboardState::InjectHooks();
    CMouseControllerState::InjectHooks();
    CRect::InjectHooks();
    CVector2D::InjectHooks();
    CQuaternion::InjectHooks();
    CMatrix::InjectHooks();
    CMatrixLink::InjectHooks();
    CMatrixLinkList::InjectHooks();
    CEntryInfoNode::InjectHooks();
    CEntryInfoList::InjectHooks();

    //CPtrList::InjectHooks();

    //CPtrNodeDoubleLink::InjectHooks();
    //CPtrListDoubleLink::InjectHooks();
    //
    //CPtrNodeSingleLink::InjectHooks();
    //CPtrListSingleLink::InjectHooks();

    CReferences::InjectHooks();
    CPopulation::InjectHooks();
    CModelInfo::InjectHooks();
    CBaseModelInfo::InjectHooks();
    CAtomicModelInfo::InjectHooks();
    CLodAtomicModelInfo::InjectHooks();
    CVehicleModelInfo::InjectHooks();
    CClumpModelInfo::InjectHooks();
    CTimeModelInfo::InjectHooks();
    CDamageAtomicModelInfo::InjectHooks();
    CWeaponModelInfo::InjectHooks();
    CShotInfo::InjectHooks();
    CPedModelInfo::InjectHooks();
    CTimeInfo::InjectHooks();
    SurfaceInfos_c::InjectHooks();
    CPlaceable::InjectHooks();
    CEntity::InjectHooks();
    CPhysical::InjectHooks();
    CRenderer::InjectHooks();
    CDummy::InjectHooks();
    CDummyObject::InjectHooks();
    CObject::InjectHooks();
    CObjectData::InjectHooks();
    CObjectSaveStructure::InjectHooks();
    CCutsceneObject::InjectHooks();
    CHandObject::InjectHooks();
    CBuilding::InjectHooks();
    CAnimatedBuilding::InjectHooks();
    CCompressedMatrixNotAligned::InjectHooks();
    CVisibilityPlugins::InjectHooks();
    CPed::InjectHooks();
    CPedIntelligence::InjectHooks();
    CPedGroupIntelligence::InjectHooks();
    CCollision::InjectHooks();
    CTempColModels::InjectHooks();
    CColSphere::InjectHooks();
    CColLine::InjectHooks();
    CColTrianglePlane::InjectHooks();
    CBox::InjectHooks();
    CColModel::InjectHooks();
    CCollisionData::InjectHooks();
    CQuadTreeNode::InjectHooks();
    CColStore::InjectHooks();
    CColAccel::InjectHooks();
    CTrafficLights::InjectHooks();
    CMotionBlurStreaks::InjectHooks();
    CRegisteredMotionBlurStreak::InjectHooks();
    CCover::InjectHooks();
    CVehicleAnimGroup::InjectHooks();
    CVehicleAnimGroupData::InjectHooks();
    CPedDamageResponseCalculator::InjectHooks();
    CPedScriptedTaskRecord::InjectHooks();
    CPedScriptedTaskRecordData::InjectHooks();
    CPedClothesDesc::InjectHooks();
    CScriptedBrainTaskStore::InjectHooks();
    CTimer::InjectHooks();
    CTagManager::InjectHooks();
    CWaterLevel::InjectHooks();
    cBuoyancy::InjectHooks();
    CWanted::InjectHooks();
    CCrime::InjectHooks();
    CEscalators::InjectHooks();
    CWeapon::InjectHooks();
    cTransmission::InjectHooks();
    CPlayerPed::InjectHooks();
    CStats::InjectHooks();
    CCarCtrl::InjectHooks();
    CTheZones::InjectHooks();
    CMenuManager::InjectHooks();
    CSprite2d::InjectHooks();
    CCheat::InjectHooks();
    CBirds::InjectHooks();
    WaterCreature_c::InjectHooks();
    WaterCreatureManager_c::InjectHooks();
    C2dEffect::InjectHooks();
    CCustomRoadsignMgr::InjectHooks();
    COcclusion::InjectHooks();
    COccluder::InjectHooks();
    CActiveOccluder::InjectHooks();
    CGarage::InjectHooks();
    CGarages::InjectHooks();
    CPostEffects::InjectHooks();
    CIniFile::InjectHooks();
    CStencilShadowObject::InjectHooks();
    CStencilShadows::InjectHooks();
    CFont::InjectHooks();
    CGameLogic::InjectHooks();
    CStuckCarCheck::InjectHooks();
    CUpsideDownCarCheck::InjectHooks();
    CCamera::InjectHooks();
    CMessages::InjectHooks();
    CGangWars::InjectHooks();
    CPlayerPedData::InjectHooks();
    CTimeCycle::InjectHooks();
    CSkidmarks::InjectHooks();
    CMovingThings::InjectHooks();
    CRoadBlocks::InjectHooks();
    CBridge::InjectHooks();
    CCranes::InjectHooks();
    CGridRef::InjectHooks();
    CShopping::InjectHooks();
    CInformFriendsEventQueue::InjectHooks();
    C3dMarkers::InjectHooks();
    C3dMarker::InjectHooks();
    CSpecialFX::InjectHooks();
    CFallingGlassPane::InjectHooks();
    CGlass::InjectHooks();
    CPedGroups::InjectHooks();
    CClock::InjectHooks();
    CClouds::InjectHooks();
    CCoronas::InjectHooks();
    CCustomCarPlateMgr::InjectHooks();
    CDraw::InjectHooks();
    CEntryExitManager::InjectHooks();
    CEntryExit::InjectHooks();
    CInformGroupEvent::InjectHooks();
    CInformGroupEventQueue::InjectHooks();
    CGangs::InjectHooks();
    CPlayerInfo::InjectHooks();
    CReplay::InjectHooks();
    CDarkel::InjectHooks();
    CGeneral::InjectHooks();
    CGamma::InjectHooks();
    CHudColours::InjectHooks();
    CTxdStore::InjectHooks();
    CCarEnterExit::InjectHooks();
    CEntityScanner::InjectHooks();
    CPedType::InjectHooks();
    CAcquaintance::InjectHooks();
    CWeather::InjectHooks();
    CPathFind::InjectHooks();
    CPathNode::InjectHooks();
    CNodeRoute::InjectHooks();
    CLoadMonitor::InjectHooks();
    CDecisionMakerTypes::InjectHooks();
    CDecisionMakerTypesFileLoader::InjectHooks();
    CPedStats::InjectHooks();
    CCarGenerator::InjectHooks();
    CTheCarGenerators::InjectHooks();
    CCullZones::InjectHooks();
    CSpecialPlateHandler::InjectHooks();
    CKeyGen::InjectHooks();
    CAccidentManager::InjectHooks();
    CStuntJumpManager::InjectHooks();
    CLocalisation::InjectHooks();
    CSimpleVariablesSaveStructure::InjectHooks();
    CPedGeometryAnalyser::InjectHooks();
    CControllerConfigManager::InjectHooks();

    NodeNamePlugin::InjectHooks();
    JPegPlugin::InjectHooks();
    PipelinePlugin::InjectHooks();
    RpAnimBlendPlugin::InjectHooks();
    CCollisionPlugin::InjectHooks();
    BreakablePlugin::InjectHooks();
    RtAnim::InjectHooks();

    CIplStore::InjectHooks();
    cHandlingDataMgr::InjectHooks();
    CLoadingScreen::InjectHooks();
    CRadar::InjectHooks();
    CText::InjectHooks();
    ModelIndices::InjectHooks();
    CWaterCannons::InjectHooks();
    CWaterCannon::InjectHooks();
    CSprite::InjectHooks();
    CPlaneTrail::InjectHooks();
    CPlaneTrails::InjectHooks();
    CPickup::InjectHooks();
    CPickups::InjectHooks();
    CPedIK::InjectHooks();

    CCustomBuildingPipeline::InjectHooks();
    CCustomBuildingRenderer::InjectHooks();
    CCustomBuildingDNPipeline::InjectHooks();
    CCustomCarEnvMapPipeline::InjectHooks();

    const auto Pools = [] {
        CPools::InjectHooks();
        CIplDefPool::InjectHooks();
        CPedPool::InjectHooks();
        CVehiclePool::InjectHooks();
        CBuildingPool::InjectHooks();
        CObjectPool::InjectHooks();
        CDummyPool::InjectHooks();
        CColModelPool::InjectHooks();
        CTaskPool::InjectHooks();
        CPedIntelligencePool::InjectHooks();
        CPtrNodeSingleLinkPool::InjectHooks();
        CPtrNodeDoubleLinkPool::InjectHooks();
        CEntryInfoNodePool::InjectHooks();
        CPointRoutePool::InjectHooks();
        //CPatrolRoutePool::InjectHooks();
        CEventPool::InjectHooks();
        CNodeRoutePool::InjectHooks();
        CTaskAllocatorPool::InjectHooks();
        CPedAttractorPool::InjectHooks();
    };
    Pools();

    const auto Audio = []() {
        // General
        CAEAudioEnvironment::InjectHooks();
        CAEAudioUtility::InjectHooks();
        CAESmoothFadeThread::InjectHooks();
        CAESound::InjectHooks();
        CAEStreamThread::InjectHooks();
        CAudioEngine::InjectHooks();
        CAudioZones::InjectHooks();

        // Entities
        // CAEAudioEntity::InjectHooks(); -- inlined
        CAECollisionAudioEntity::InjectHooks();
        CAEDoorAudioEntity::InjectHooks();
        CAEExplosionAudioEntity::InjectHooks();
        CAEFireAudioEntity::InjectHooks();
        CAEFrontendAudioEntity::InjectHooks();
        CAEGlobalWeaponAudioEntity::InjectHooks();
        CAEPedAudioEntity::InjectHooks();
        CAEPedlessSpeechAudioEntity::InjectHooks();
        CAEPedSpeechAudioEntity::InjectHooks();
        CAEPedWeaponAudioEntity::InjectHooks();
        CAEPoliceScannerAudioEntity::InjectHooks();
        CAEScriptAudioEntity::InjectHooks();
        CAETwinLoopSoundEntity::InjectHooks();
        CAEVehicleAudioEntity::InjectHooks();
        CAEWaterCannonAudioEntity::InjectHooks();
        // CAEWeaponAudioEntity::InjectHooks();
        CAEWeatherAudioEntity::InjectHooks();

        // Hardware
        CAEAudioChannel::InjectHooks();
        CAEAudioHardware::InjectHooks();
        CAEStaticChannel::InjectHooks();
        CAEStreamingChannel::InjectHooks();

        // Loaders
        CAEBankLoader::InjectHooks();
        CAEDataStream::InjectHooks();
        CAEMFDecoder::InjectHooks();
        CAEMP3BankLoader::InjectHooks();
        CAEMP3TrackLoader::InjectHooks();
        CAEStreamingDecoder::InjectHooks();
        // CAEStreamTransformer::InjectHooks(); -- injected by AEDataStream::InjectHooks.
        CAETrackLoader::InjectHooks();
        CAEVorbisDecoder::InjectHooks();
        CAEWaveDecoder::InjectHooks();
        CAEWMADecoder::InjectHooks();

        // Managers
        CAEAmbienceTrackManager::InjectHooks();
        CAECutsceneTrackManager::InjectHooks();
        CAERadioTrackManager::InjectHooks();
        CAESoundManager::InjectHooks();
        CAEUserRadioTrackManager::InjectHooks();
    };

    const auto Plant = []() {
        CPlantColEntEntry::InjectHooks();
        CPlantMgr::InjectHooks();
        CPlantLocTri::InjectHooks();
        CGrassRenderer::InjectHooks();
        CPPTriPlantBuffer::InjectHooks();
        CPlantSurfPropMgr::InjectHooks();
    };
    Plant();

    const auto Tasks = []() {
        const auto Interior = [] {
            CTaskInteriorBeInHouse::InjectHooks();
            CTaskInteriorBeInOffice::InjectHooks();
            CTaskInteriorBeInShop::InjectHooks();
            CTaskInteriorGoToInfo::InjectHooks();
        // CTaskInteriorLieInBed::InjectHooks();
            CTaskInteriorShopKeeper::InjectHooks();
            CTaskInteriorSitAtDesk::InjectHooks();
        // CTaskInteriorSitInChair::InjectHooks();
            CTaskInteriorUseInfo::InjectHooks();
        };

        CTaskComplexUseClosestFreeScriptedAttractorRun::InjectHooks();
        CTaskComplexUseClosestFreeScriptedAttractorSprint::InjectHooks();
        CTaskComplexUseClosestFreeScriptedAttractor::InjectHooks();

        CTaskSimpleChoking::InjectHooks();
        CTaskComplexEvasiveDiveAndGetUp::InjectHooks();
        CTaskSimpleLeaveGroup::InjectHooks();
        CTaskComplexDiveFromAttachedEntityAndGetUp::InjectHooks();
        CTaskGangHasslePed::InjectHooks();
        CTaskGangHassleVehicle::InjectHooks();
        CTaskGoToVehicleAndLean::InjectHooks();
        CTaskLeanOnVehicle::InjectHooks();
        CTaskComplexCarSlowBeDraggedOut::InjectHooks();
        CTaskComplexCarSlowBeDraggedOutAndStandUp::InjectHooks();
        CTaskComplexDestroyCar::InjectHooks();
        CTaskComplexDestroyCarArmed::InjectHooks();
        CTaskComplexDestroyCarMelee::InjectHooks();
        CTaskComplexDieInCar::InjectHooks();
        // CTaskComplexDiveFromAttachedEntityAndGetUp::InjectHooks();
        CTaskComplexDragPedFromCar__InjectHooks();
        CTaskComplexDrivePointRoute::InjectHooks();
        CTaskComplexEnterCarAsDriverTimed::InjectHooks();
        CTaskComplexEnterCarAsPassengerTimed::InjectHooks();
        // CTaskComplexEnterCarAsPassengerWait::InjectHooks();
        // CTaskComplexEvasiveCower::InjectHooks();
        // CTaskComplexEvasiveDiveAndGetUp::InjectHooks();
        // CTaskComplexEvasiveStep::InjectHooks();
        // CTaskComplexExtinguishFireOnFoot::InjectHooks();
        CTaskComplexExtinguishFires::InjectHooks();
        // CTaskComplexFallAndStayDown::InjectHooks();
        CTaskComplexFallToDeath::InjectHooks();
        CTaskComplexFleeAnyMeans::InjectHooks();
        CTaskComplexFleeEntity::InjectHooks();
        CTaskComplexFleePoint::InjectHooks();
        CTaskComplexFleeShooting::InjectHooks();
        CTaskComplexFollowLeaderAnyMeans::InjectHooks();
        // CTaskComplexFollowNodeRouteShooting::InjectHooks();
        // CTaskComplexFollowPatrolRoute::InjectHooks();
        CTaskComplexFollowPointRoute::InjectHooks();
        CTaskComplexGangFollower::InjectHooks();
        CTaskComplexGangJoinRespond::InjectHooks();
        CTaskComplexGangLeader::InjectHooks();
        CTaskComplexGetOnBoatSeat::InjectHooks();
        CTaskComplexGoToAttractor::InjectHooks();
        CTaskComplexGoToBoatSteeringWheel::InjectHooks();
        CTaskComplexGoToCarDoorAndStandStill::InjectHooks();
        CTaskComplexGoToPointAiming::InjectHooks();
        CTaskComplexGoToPointAndStandStillAndAchieveHeading::InjectHooks();
        CTaskComplexGoToPointAnyMeans::InjectHooks();
        CTaskComplexGoToPointShooting::InjectHooks();
        // CTaskComplexGoToPointUntilCollisionAndStandStill::InjectHooks();
        CTaskComplexGotoDoorAndOpen::InjectHooks();
        CTaskComplexHitByGunResponse::InjectHooks();
        CTaskComplexHitResponse::InjectHooks();
        CTaskComplexInWater::InjectHooks();
        CTaskComplexInvestigateDeadPed::InjectHooks();
        CTaskComplexInvestigateDisturbance::InjectHooks();
        // CTaskComplexKillAllThreats::InjectHooks();
        // CTaskComplexKillCriminal::InjectHooks();

        CTaskComplexKillPedOnFootArmed::InjectHooks();
        CTaskComplexKillPedGroupOnFoot::InjectHooks();
        CTaskComplexKillPedOnFootMelee::InjectHooks();
        CTaskComplexKillPedOnFootStealth::InjectHooks();

        CTaskSimpleWaitUntilLeaderAreaCodesMatch::InjectHooks();
        CTaskComplexLeaveCarAndDie::InjectHooks();
        CTaskComplexLeaveBoat::InjectHooks();
        CTaskComplexLeaveCarAndFlee::InjectHooks();
        CTaskComplexLeaveCarAsPassengerWait::InjectHooks();
        CTaskComplexLeaveCarAndWander::InjectHooks();
        CTaskComplexMoveBackAndJump::InjectHooks();
        CTaskComplexOnFire::InjectHooks();
        // CTaskComplexOpenDriverDoor::InjectHooks();
        // CTaskComplexOpenPassengerDoor::InjectHooks();
        CTaskComplexPassObject::InjectHooks();
        // CTaskComplexPresentIDToCop::InjectHooks();
        // CTaskComplexReactToAttack::InjectHooks();
        CTaskComplexReactToGunAimedAt::InjectHooks();
        CTaskComplexRoadRage::InjectHooks();
        // CTaskComplexScreamInCarThenLeave::InjectHooks();
        CTaskComplexSeekCoverUntilTargetDead::InjectHooks();

        // CTaskComplexSeekEntity<CEntitySeekPosCalculator>::InjectHooks();
        // CTaskComplexSeekEntity<CEntitySeekPosCalculatorDriving>::InjectHooks();
        // CTaskComplexSeekEntity<CEntitySeekPosCalculatorEntitySurface>::InjectHooks();
        // CTaskComplexSeekEntity<CEntitySeekPosCalculatorStandard>::InjectHooks();
        // CTaskComplexSeekEntity<CEntitySeekPosCalculatorVehicleId>::InjectHooks();

        CTaskComplexSeekEntityRadiusAngleOffset::InjectHooks();
        CTaskComplexSeekEntityStandard::InjectHooks();
        CTaskComplexSeekEntityXYOffset::InjectHooks();
        CTaskComplexSeekEntityAiming::InjectHooks();
        //CTaskComplexSeekEntityXYOffset::InjectHooks();
        CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>::InjectHooks();

        // CTaskComplexSeekEntityAiming::InjectHooks();
        CTaskComplexSeekEntityAnyMeansXYOffset::InjectHooks();
        CTaskComplexShuffleSeats::InjectHooks();
        CTaskComplexSignalAtPed::InjectHooks();
        CTaskComplexSitDownThenIdleThenStandUp::InjectHooks();
        CTaskComplexSmartFleeEntity::InjectHooks();
        CTaskComplexSmartFleePoint::InjectHooks();
        CTaskComplexStareAtPed::InjectHooks();
        CTaskComplexStealCar::InjectHooks();
        CTaskComplexTrackEntity::InjectHooks();
        CTaskComplexTurnToFaceEntityOrCoord::InjectHooks();
        CTaskComplexUseAttractor::InjectHooks();
        // CTaskComplexUseAttractorPartner::InjectHooks();
        // CTaskComplexUseClosestFreeScriptedAttractor::InjectHooks();
        // CTaskComplexUseClosestFreeScriptedAttractorRun::InjectHooks();
        // CTaskComplexUseClosestFreeScriptedAttractorSprint::InjectHooks();
        CTaskComplexUseEffect::InjectHooks();
        CTaskComplexUseEffectRunning::InjectHooks();
        CTaskComplexUseEffectSprinting::InjectHooks();
        // CTaskComplexUseEntryExit::InjectHooks();
        // CTaskComplexUsePairedAttractor::InjectHooks();
        CTaskComplexUseScriptedBrain::InjectHooks();
        CTaskComplexUseScriptedAttractor::InjectHooks();
        CTaskComplexWaitAtAttractor::InjectHooks();
        CTaskComplexWaitForBus::InjectHooks();
        CTaskComplexWaitForBackup::InjectHooks();
        CTaskComplexWaitForDryWeather::InjectHooks();
        CTaskComplexWalkAlongsidePed::InjectHooks();
        CTaskComplexWalkRoundBuildingAttempt::InjectHooks();
        CTaskComplexWalkRoundFire::InjectHooks();
        CTaskComplexWanderFlee::InjectHooks();
        CTaskSimpleAffectSecondaryBehaviour::InjectHooks();
        CTaskSimpleArrestPed__InjectHooks();
        CTaskSimpleBeHit::InjectHooks();
        // CTaskSimpleBeHitWhileMoving::InjectHooks();
        // CTaskSimpleBeKickedOnGround::InjectHooks();
        CTaskSimpleBikeJacked::InjectHooks();
        CTaskSimpleCarAlign::InjectHooks();
        CTaskSimpleCarCloseDoorFromInside::InjectHooks();
        CTaskSimpleCarCloseDoorFromOutside::InjectHooks();
        CTaskSimpleCarFallOut::InjectHooks();
        // CTaskSimpleCarForcePedOut::InjectHooks();
        CTaskSimpleCarGetOut::InjectHooks();
        CTaskSimpleCarGetIn::InjectHooks();
        CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::InjectHooks();
        CTaskSimpleCarOpenDoorFromOutside::InjectHooks();
        CTaskSimpleCarJumpOut::InjectHooks();
        CTaskSimpleCarOpenLockedDoorFromOutside::InjectHooks();
        CTaskSimpleCarSetPedSlowDraggedOut::InjectHooks();
        CTaskSimpleCarSetTempAction::InjectHooks();
        CTaskSimpleCarShuffle::InjectHooks();
        CTaskSimpleCarSlowBeDraggedOut::InjectHooks();
        CTaskSimpleCarWaitToSlowDown::InjectHooks();
        CTaskSimpleCarWaitForDoorNotToBeInUse::InjectHooks();
        // CTaskSimpleChat::InjectHooks();
        CTaskSimpleCower::InjectHooks();
        CTaskSimpleClearLookAt::InjectHooks();
        // CTaskSimpleDead::InjectHooks();
        // CTaskSimpleDetonate::InjectHooks();
        // + CTaskSimpleDieInCar
        // CTaskSimpleDoHandSignal::InjectHooks();
        CTaskSimpleDrown::InjectHooks();
        CTaskSimpleDrownInCar::InjectHooks();
        // CTaskSimpleDuckToggle::InjectHooks();
        // CTaskSimpleDuckWhileShotsWhizzing::InjectHooks();
        // CTaskSimpleEvasiveDive::InjectHooks();
        // CTaskSimpleEvasiveStep::InjectHooks();
        CTaskSimpleFightingControl::InjectHooks();
        // CTaskSimpleFinishBrain::InjectHooks();
        CTaskSimpleGunControl::InjectHooks();
        // CTaskSimpleHailTaxi::InjectHooks();
        // CTaskSimpleHailTaxiAndPause::InjectHooks();
        CTaskSimpleHandsUp::InjectHooks();
        CTaskSimpleHitByGunFromFront::InjectHooks();
        CTaskSimpleHitByGunFromLeft::InjectHooks();
        CTaskSimpleHitByGunFromRear::InjectHooks();
        CTaskSimpleHitByGunFromRight::InjectHooks();
        CTaskSimpleHitFromBack::InjectHooks();
        CTaskSimpleHitFromBehind::InjectHooks();
        CTaskSimpleHitFromFront::InjectHooks();
        CTaskSimpleHitFromLeft::InjectHooks();
        CTaskSimpleHitFromRight::InjectHooks();
        CTaskSimpleHitWall::InjectHooks();
        CTaskSimpleKillPedWithCar::InjectHooks();
        CTaskSimpleHurtPedWithCar::InjectHooks();
        CTaskSimpleLookAbout::InjectHooks();
        // CTaskSimpleOnEscalator::InjectHooks();
        CTaskSimplePickUpBike::InjectHooks();
        CTaskSimplePlayerOnFire::InjectHooks();
        // CTaskSimpleRunAnimLoopedMiddle::InjectHooks();
        CTaskSimpleRunTimedAnim::InjectHooks();
        CTaskSimpleSay::InjectHooks();
        CTaskSimpleSetCharDecisionMaker::InjectHooks();
        CTaskSimpleSetCharIgnoreWeaponRangeFlag::InjectHooks();
        CTaskSimpleSetKindaStayInSamePlace::InjectHooks();
        CTaskSimpleSetPedAsAutoDriver::InjectHooks();
        CTaskSimpleShakeFist::InjectHooks();
        CTaskSimpleSitDown::InjectHooks();
        CTaskSimpleSitIdle::InjectHooks();
        CTaskSimpleStandUp::InjectHooks();
        CTaskSimpleThrowControl::InjectHooks();
        // CTaskSimpleTired::InjectHooks();
        // CTaskSimpleThrowControl::InjectHooks();
        CTaskSimpleTired::InjectHooks();
        // CTaskSimpleTriggerEvent::InjectHooks();
        CTaskSimpleUseAtm::InjectHooks();
        CTaskSimpleTurn180::InjectHooks();
        CTaskSimpleWaitForBus::InjectHooks();
        // CTaskSimpleWaitForPizza::InjectHooks();
        CTaskSimpleWaitUntilAreaCodesMatch::InjectHooks();
        // CTaskSimpleWaitUntilLeaderAreaCodesMatch::InjectHooks();
        CTaskSimpleWaitUntilPedIsInCar::InjectHooks();
        CTaskComplexAvoidEntity::InjectHooks();
        CTaskSimpleWaitUntilPedIsOutCar::InjectHooks();
        CTaskComplexSequence::InjectHooks();
        CTaskSimpleCarSlowDragPedOut::InjectHooks();
        CTaskManager::InjectHooks();
        CTaskSimpleCreateCarAndGetIn::InjectHooks();
        CTaskComplexEnterAnyCarAsDriver::InjectHooks();
        CTaskComplexLeaveAnyCar::InjectHooks();
        // CTaskSimpleAbseil::InjectHooks();
        CTaskComplexWanderCop::InjectHooks();
        // CTaskComplexUseMobilePhone::InjectHooks();
        CTaskSimpleStandStill::InjectHooks();
        CTaskSimpleCarDrive::InjectHooks();
        // CTaskSimpleScratchHead::InjectHooks();
        // CTaskSimpleChoking::InjectHooks();
        CTaskComplexPartnerChat::InjectHooks();
        CTaskSimpleUseGun::InjectHooks();
        // CTaskSimpleGangDriveBy::InjectHooks();
        CTaskComplexObserveTrafficLightsAndAchieveHeading::InjectHooks();
        CTaskSimpleInAir::InjectHooks();
        // CTaskComplexHitPedWithCar::InjectHooks();
        CTaskSimplePlayerOnFoot::InjectHooks();
        CTaskComplexFollowPedFootsteps::InjectHooks();
        CTaskComplexGetUpAndStandStill::InjectHooks();
        // CTaskComplexGoPickUpEntity::InjectHooks();
        CTaskSimpleDie__InjectHooks();
        CTaskSimpleDuck::InjectHooks();
        CTaskComplexPolicePursuit::InjectHooks();
        CTaskSimpleFacial::InjectHooks();
        CTaskComplexCopInCar::InjectHooks();
        CTaskComplexFacial::InjectHooks();
        CTaskComplexInAirAndLand::InjectHooks();
        CTaskSimpleGetUp::InjectHooks();
        CTaskSimpleUninterruptable::InjectHooks();
        CTaskComplexFallAndGetUp::InjectHooks();
        CTaskComplexFollowNodeRoute::InjectHooks();
        CTaskComplexFollowLeaderInFormation::InjectHooks();
        // CTaskSimpleTriggerLookAt::InjectHooks();
        CTaskSimpleHitHead::InjectHooks();
        CTaskUtilityLineUpPedWithCar::InjectHooks();
        CTaskSimpleLand::InjectHooks();
        CTaskSimpleJetPack::InjectHooks();
        // + CTaskSimpleSetStayInSamePlace::InjectHooks();
        CTaskSimpleJump::InjectHooks();
        CTaskSimpleFall::InjectHooks();
        CTaskSimpleClimb::InjectHooks();
        CTaskComplexPlayHandSignalAnim::InjectHooks();
        // CTaskComplexWaitForPed::InjectHooks();
        CTaskComplexJump::InjectHooks();
        // CTaskComplexWanderStandard::InjectHooks();
        CTaskComplexSunbathe::InjectHooks();
        CTaskComplexEnterCar::InjectHooks();
        // CTaskSimpleTogglePedThreatScanner::InjectHooks();
        CTaskComplexUseGoggles::InjectHooks();
        // CTaskComplexCrossRoadLookAndAchieveHeading::InjectHooks();
        CTaskComplexGoToPointAndStandStill::InjectHooks();
        CTaskSimpleAchieveHeading::InjectHooks();
        CTaskSimpleGiveCPR::InjectHooks();
        // CTaskSimpleCarSetPedInAsPassenger::InjectHooks();
        CTaskComplexDriveFireTruck::InjectHooks();
        CTaskSimpleSwim::InjectHooks();
        CTaskComplexWalkRoundObject::InjectHooks();
        CTaskSimplePause::InjectHooks();
        // CTaskComplexEnterCarAsPassenger::InjectHooks();
        // + CTaskComplexEnterCarAsDriver::InjectHooks();
        // CTaskSimpleNone::InjectHooks();
        CTaskComplexKillPedOnFoot::InjectHooks();
        CTaskSimpleThrowProjectile::InjectHooks();
        CTaskSimpleGoToPoint::InjectHooks();
        // CTaskComplexWanderMedic::InjectHooks();
        // CTaskSimpleCarDriveTimed::InjectHooks();
        // CTaskComplexDriveWander::InjectHooks();
        CTaskSimpleStealthKill::InjectHooks();
        // CTaskSimpleCarSetPedOut::InjectHooks();
        CTaskSimpleAnim::InjectHooks();
        CTaskSimpleRunAnim::InjectHooks();
        // CTaskComplexWanderCriminal::InjectHooks();
        // CTaskComplexWanderProstitute::InjectHooks();
        CTaskComplexPartnerGreet::InjectHooks();
        CTaskComplexWalkRoundCar::InjectHooks();
        CTaskComplexMedicTreatInjuredPed::InjectHooks();
        CTaskSimplePlayHandSignalAnim::InjectHooks();
        CTaskComplexCarDrive::InjectHooks();
        CTaskComplexCarDriveMission::InjectHooks();
        CTaskComplexKillPedFromBoat::InjectHooks();
        // CTaskComplexLeaveCar::InjectHooks();
        CTaskComplexTreatAccident::InjectHooks();
        CTaskComplexGoToPointAndStandStillTimed::InjectHooks();
        CTaskComplexPartnerShove::InjectHooks();
        CTaskSimpleRunNamedAnim::InjectHooks();
        // CTaskComplexProstituteSolicit::InjectHooks();
        CTaskComplexStuckInAir::InjectHooks();
        CTaskSimpleHoldEntity::InjectHooks();
        CTaskSimpleGoTo::InjectHooks();
        CTaskComplexPartner::InjectHooks();
        // CTaskSimpleCarSetPedInAsDriver::InjectHooks();
        CTaskComplexWander::InjectHooks();
        CTaskSimplePutDownEntity::InjectHooks();
        CTaskComplexUseSwatRope::InjectHooks();
        CTaskSimpleGoToPointFine::InjectHooks();
        CTaskSimpleIKChain::InjectHooks();
        CTaskSimpleIKManager::InjectHooks();
        CTaskSimpleIKPointArm::InjectHooks();
        CTaskSimpleIKLookAt::InjectHooks();
        CTaskComplexDie::InjectHooks();
        CTaskComplexEnterBoatAsDriver::InjectHooks();
        CTaskSimpleFight::InjectHooks();
        CTaskComplexUseWaterCannon::InjectHooks();
        // CTaskComplexDriveToPoint::InjectHooks();
        CTaskSimpleSlideToCoord::InjectHooks();
        // CTaskComplexPartnerDeal::InjectHooks();
        CTaskSimplePickUpEntity::InjectHooks();
        CTaskComplexBeInGroup::InjectHooks();
        // CTaskComplexBeCop::InjectHooks();
        CTaskComplexAvoidOtherPedWhileWandering::InjectHooks();
        CTaskComplexArrestPed::InjectHooks();

        const auto EntitySeekPosCalculators = [] {
            CEntitySeekPosCalculator::InjectHooks();
            CEntitySeekPosCalculatorStandard::InjectHooks();
            CEntitySeekPosCalculatorRadiusAngleOffset::InjectHooks();
            CEntitySeekPosCalculatorXYOffset::InjectHooks();
            CEntitySeekPosCalculatorFixedPos::InjectHooks();
        };
        EntitySeekPosCalculators();
    };

    const auto Events = []() {
        CEvent::InjectHooks();
        CEventSeenPanickedPed::InjectHooks();
        CEventCarUpsideDown::InjectHooks();
        CEventPassObject::InjectHooks();
        CEventLeanOnVehicle::InjectHooks();
        CEventSeenCop::InjectHooks();
        CEventOnFire::InjectHooks();
        // + CEventRevived::InjectHooks();
        CEventHandlerHistory::InjectHooks();
        CEventEditableResponse::InjectHooks();
        CEventDamage::InjectHooks();
        CEventAttractor::InjectHooks();
        CEventScriptedAttractor::InjectHooks();
        CEventGunShot::InjectHooks();
        CEventGunShotWhizzedBy::InjectHooks();
        // + CEventGunAimedAt::InjectHooks();
        // + CEventDeath::InjectHooks();
        // + CEventDeadPed::InjectHooks();
        // + CEventDraggedOutCar::InjectHooks();
        // + CEventGotKnockedOverByCar::InjectHooks();
        CEventKnockOffBike::InjectHooks();
        CEventScriptCommand::InjectHooks();
        CEventSoundQuiet::InjectHooks();
        // + CEventPedToChase::InjectHooks();
        // + CEventPedToFlee::InjectHooks();
        CEventPotentialWalkIntoVehicle::InjectHooks();
        CEventPotentialWalkIntoObject::InjectHooks();
        CEventPotentialWalkIntoFire::InjectHooks();
        CEventPotentialWalkIntoPed::InjectHooks();
        CEventPedCollisionWithPed::InjectHooks();
        CEventPedCollisionWithPlayer::InjectHooks();
        CEventPlayerCollisionWithPed::InjectHooks();
        CEventObjectCollision::InjectHooks();
        CEventBuildingCollision::InjectHooks();
        CEventVehicleToSteal::InjectHooks();
        CEventVehicleDamage::InjectHooks();
        CEventVehicleDamageWeapon::InjectHooks();
        CEventVehicleDied::InjectHooks();
        CEventVehicleOnFire::InjectHooks();
        CEventVehicleHitAndRun::InjectHooks();
        CEventVehicleCollision::InjectHooks();
        CEventVehicleDamageCollision::InjectHooks();
        CEventHitByWaterCannon::InjectHooks();
        // + CEventInWater::InjectHooks();
        // + CEventCreatePartnerTask::InjectHooks();
        CEventInAir::InjectHooks();
        CEventStuckInAir::InjectHooks();
        CEventAcquaintancePed::InjectHooks();
        CEventLeaderEnteredCarAsDriver::InjectHooks();
        CEventLeaderExitedCarAsDriver::InjectHooks();
        CEventLeaderQuitEnteringCarAsDriver::InjectHooks();
        CEventAreaCodes::InjectHooks();
        CEventLeaderEntryExit::InjectHooks();
        // + CEventSpecial::InjectHooks();
        // + CEventFireNearby::InjectHooks();
        // + CEventGroupEvent::InjectHooks();
        CEventGroup::InjectHooks();
        CEventGlobalGroup::InjectHooks();
        CEventPlayerCommandToGroup::InjectHooks();
        CEventPlayerCommandToGroupAttack::InjectHooks();
        CEventPlayerCommandToGroupGather::InjectHooks();
        CEventDontJoinPlayerGroup::InjectHooks();
        CEventNewGangMember::InjectHooks();
        CEventEscalator::InjectHooks();
        CEventDanger::InjectHooks();
        // + CEventSexyVehicle::InjectHooks();
        // + CEventChatPartner::InjectHooks();
        CEventCopCarBeingStolen::InjectHooks();
        CEventHandler::InjectHooks();
        CEventAcquaintancePedHate::InjectHooks();
        CEventAcquaintancePedHateBadlyLit::InjectHooks();
        // + CEventInteriorUseInfo
        // + CEventLowAngerAtPlayer
        // + CEventPedEnteredMyVehicle
        // + CEventPotentialGetRunOver
        // + CEventPotentialWalkIntoBuilding::InjectHooks();
        // + CEventSexyPed
        // + CEventSignalAtPed
        // + CEventSoundLoud
        // - CEventSource::InjectHooks();
        // - CEventScanner::InjectHooks();
    };

    const auto Fx = []() {
        FxSystemBP_c::InjectHooks();
        FxSystem_c::InjectHooks();
        FxSphere_c::InjectHooks();
        FxPrimBP_c::InjectHooks();
        FxMemoryPool_c::InjectHooks();
        FxInfoManager_c::InjectHooks();
        FxManager_c::InjectHooks();
        // ReversibleHooks::Install("FxFrustumInfo_c", "IsCollision", 0x4AA030, &FxFrustumInfo_c::IsCollision);
        FxEmitterPrt_c::InjectHooks();
        FxEmitterBP_c::InjectHooks();
        FxEmitter_c::InjectHooks();
        Fx_c::InjectHooks();
    };

    const auto Vehicle = []() {
        CAutomobile::InjectHooks();
        CBike::InjectHooks();
        CBmx::InjectHooks();
        CBoat::InjectHooks();
        CHeli::InjectHooks();
        CMonsterTruck::InjectHooks();
        CPlane::InjectHooks();
        CQuadBike::InjectHooks();
        CTrailer::InjectHooks();
        CTrain::InjectHooks();
        CVehicle::InjectHooks();
    };

    const auto Interior = []() {
        FurnitureGroup_c::InjectHooks();
        FurnitureManager_c::InjectHooks();
        FurnitureSubGroup_c::InjectHooks();
        Interior_c::InjectHooks();
        InteriorGroup_c::InjectHooks();
        InteriorManager_c::InjectHooks();
    };

    const auto Scripts = []() {
        CScriptsForBrains::InjectHooks();
        CRunningScript::InjectHooks();
        CTheScripts::InjectHooks();
        CStreamedScripts::InjectHooks();
        CScripted2dEffects::InjectHooks();
        CScriptResourceManager::InjectHooks();
    };

    const auto App = []() {
        VideoPlayer::InjectHooks();
        Securom::InjectHooks();
        AppInjectHooks();
        Win32InjectHooks();
        RsInjectHooks();
        VideoModeInjectHooks();
    };

    const auto Animation = []() {
        CAnimBlendAssocGroup::InjectHooks();
        CAnimBlendAssociation::InjectHooks();
        CAnimBlendClumpData::InjectHooks();
        CAnimBlendHierarchy::InjectHooks();
        CAnimBlendNode::InjectHooks();
        CAnimBlendSequence::InjectHooks();
        CAnimBlendStaticAssociation::InjectHooks();
        CAnimManager::InjectHooks();
    };

    Animation();
    App();
    Audio();
    Tasks();
    Events();
    Fx();
    Vehicle();
    Interior();
    Scripts();
}

void InjectHooksMain(HMODULE hThisDLL) {
    ReversibleHooks::OnInjectionBegin(hThisDLL);
    InjectHooksMain();
    ReversibleHooks::OnInjectionEnd();
}
