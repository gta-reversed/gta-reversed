#pragma once
#pragma message("Compiling precompiled header.\n")

#include <Windows.h>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <functional>
#include <iostream>
#include <cassert>
#include <array>
#include <vector>
#include <numeric>
#include <cstring>
#include <tuple>
#include <initializer_list>

#include <ranges>
namespace rng = std::ranges;

#include "Base.h"

#include "HookSystem.h"
#include "reversiblehooks\ReversibleHooks.h"

// DirectX
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "EntryInfoNode.h"
#include "EntryInfoList.h"
#include "KeyGen.h"
#include "Link.h"
#include "LinkList.h"
#include "Matrix.h"
#include "MatrixLink.h"
#include "MatrixLinkList.h"
#include "Pool.h"
#include "PtrList.h"
#include "PtrListDoubleLink.h"
#include "PtrListSingleLink.h"
#include "PtrNode.h"
#include "PtrNodeDoubleLink.h"
#include "PtrNodeSingleLink.h"
#include "QuadTreeNode.h"
#include "Quaternion.h"
#include "Rect.h"
#include "Store.h"
#include "Vector.h"
#include "Vector2D.h"
#include "ListItem_c.h"
#include "List_c.h"
#include "SArray.h"

#include "game_sa\Enums\eCheats.h"
#include "game_sa\Enums\AnimationEnums.h"
#include "game_sa\Enums\eAnimBlendCallbackType.h"
#include "game_sa\Enums\eAudioEvents.h"
#include "game_sa\Enums\eCamMode.h"
#include "game_sa\Enums\eCarMission.h"
#include "game_sa\Enums\eClothesModelPart.h"
#include "game_sa\Enums\eClothesTexturePart.h"
#include "game_sa\Enums\eCrimeType.h"
#include "game_sa\Enums\eDecisionMakerEvents.h"
#include "game_sa\Enums\eEmergencyPedVoices.h"
#include "game_sa\Enums\eEntityStatus.h"
#include "game_sa\Enums\eEntityType.h"
#include "game_sa\Enums\eEventType.h"
#include "game_sa\Enums\eModelID.h"
#include "game_sa\Enums\ePedBones.h"
#include "game_sa\Enums\ePedModel.h"
#include "game_sa\Enums\ePedState.h"
#include "game_sa\Enums\eRadioID.h"
#include "game_sa\Enums\eScriptCommands.h"
#include "game_sa\Enums\eSoundID.h"
#include "game_sa\Enums\eSprintType.h"
#include "game_sa\Enums\eStatModAbilities.h"
#include "game_sa\Enums\eStats.h"
#include "game_sa\Enums\eStatsReactions.h"
#include "game_sa\Enums\eSurfaceType.h"
#include "game_sa\Enums\eTargetDoor.h"
#include "game_sa\Enums\eTaskType.h"
#include "game_sa\Enums\eVehicleClass.h"
#include "game_sa\Enums\eVehicleHandlingFlags.h"
#include "game_sa\Enums\eVehicleHandlingModelFlags.h"
#include "game_sa\Enums\eWeaponFire.h"
#include "game_sa\Enums\eWeaponFlags.h"
#include "game_sa\Enums\eWeaponModel.h"
#include "game_sa\Enums\eWeaponType.h"
#include "game_sa\Enums\eWinchType.h"
#include "game_sa\Enums\eItemDefinitionFlags.h"

#include "game_sa\constants.h"
#include "game_sa\ModelIndices.h"
#include "game_sa\PedGeometryAnalyser.h"
#include "game_sa\Debug.h"
#include "game_sa\MemoryMgr.h"
#include "game_sa\CullZones.h"
#include "game_sa\GridRef.h"
#include "game_sa\VehicleScanner.h"
#include "game_sa\LoadMonitor.h"
#include "game_sa\PedStuckChecker.h"
#include "game_sa\DecisionMakerTypes.h"
#include "game_sa\InformGroupEventQueue.h"
#include "game_sa\InformFriendsEventQueue.h"
#include "game_sa\InterestingEvents.h"
#include "game_sa\Events\GroupEventHandler.h"
#include "game_sa\Events\EventInAir.h"
#include "game_sa\Events\EventAcquaintancePed.h"
#include "game_sa\Events\EventSource.h"
#include "game_sa\Events\EventVehicleDamageCollision.h"
#include "game_sa\Events\EventVehicleCollision.h"
#include "game_sa\Events\EventVehicleThreat.h"
#include "game_sa\Events\EventVehicleToSteal.h"
#include "game_sa\Events\EventVehicleDamageWeapon.h"
#include "game_sa\Events\EventVehicleDied.h"
#include "game_sa\Events\EventVehicleOnFire.h"
#include "game_sa\Events\EventVehicleHitAndRun.h"
#include "game_sa\Events\EventDraggedOutCar.h"
#include "game_sa\Events\EventKnockOffBike.h"
#include "game_sa\Events\EventGotKnockedOverByCar.h"
#include "game_sa\Events\EventAttractor.h"
#include "game_sa\Events\EventGunShot.h"
#include "game_sa\Events\EventGunShotWhizzedBy.h"
#include "game_sa\Events\LeaderEvents.h"
#include "game_sa\Events\GroupEvents.h"
#include "game_sa\Events\MentalStateEvents.h"
#include "game_sa\Events\PotentialWalkIntoEvents.h"
#include "game_sa\Events\EventHitByWaterCannon.h"
#include "game_sa\Events\EventInWater.h"
#include "game_sa\Events\EventDeath.h"
#include "game_sa\Events\EventDeadPed.h"
#include "game_sa\Events\EventVehicleDamage.h"
#include "game_sa\Events\EventPedToFlee.h"
#include "game_sa\Events\EventPedToChase.h"
#include "game_sa\Events\EntityCollisionEvents.h"
#include "game_sa\Events\EventEditableResponse.h"
#include "game_sa\Events\EventSoundQuiet.h"
#include "game_sa\Events\EventHandlerHistory.h"
#include "game_sa\Events\EventGlobalGroup.h"
#include "game_sa\Events\EventGroupEvent.h"
#include "game_sa\Events\EventGroup.h"
#include "game_sa\Events\EventGunAimedAt.h"
#include "game_sa\Events\EventScriptCommand.h"
#include "game_sa\Events\EventDamage.h"
#include "game_sa\Events\EventCreatePartnerTask.h"
#include "game_sa\Events\EventHandler.h"
#include "game_sa\Events\EventScanner.h"
#include "game_sa\Events\Event.h"
#include "game_sa\Crime.h"
#include "game_sa\SurfaceInfo_c.h"
#include "game_sa\SurfaceInfos_c.h"
#include "game_sa\Replay.h"
#include "game_sa\VehicleRecording.h"
#include "game_sa\VehicleAnimGroupData.h"
#include "game_sa\Collision\ColStore.h"
#include "game_sa\Collision\ColAccel.h"
#include "game_sa\PedDamageResponseCalculator.h"
#include "game_sa\PedDamageResponse.h"
#include "game_sa\WaterLevel.h"
#include "game_sa\WaterCreature_c.h"
#include "game_sa\WaterCreatureManager_c.h"
#include "game_sa\CdStreamInfo.h"
#include "game_sa\Plugins\TwoDEffectPlugin\2dEffect.h"
#include "game_sa\3dMarker.h"
#include "game_sa\3dMarkers.h"
#include "game_sa\Accident.h"
#include "game_sa\AccidentManager.h"
#include "game_sa\AttractorScanner.h"
#include "game_sa\AutoPilot.h"
#include "game_sa\Birds.h"
#include "game_sa\BouncingPanel.h"
#include "game_sa\Bridge.h"
#include "game_sa\BrightLights.h"
#include "game_sa\BulletTrace.h"
#include "game_sa\BulletTraces.h"
#include "game_sa\Cam.h"
#include "game_sa\Camera.h"
#include "game_sa\CamPathSplines.h"
#include "game_sa\CarAI.h"
#include "game_sa\CarEnterExit.h"
#include "game_sa\Cheat.h"
#include "game_sa\Clock.h"
#include "game_sa\Clothes.h"
#include "game_sa\ClothesBuilder.h"
#include "game_sa\ControllerConfigManager.h"
#include "game_sa\Coronas.h"
#include "game_sa\Cover.h"
#include "game_sa\CoverPoint.h"
#include "game_sa\Cranes.h"
#include "game_sa\CrimeBeingQd.h"
#include "game_sa\CustomCarEnvMapPipeline.h"
#include "game_sa\CutsceneMgr.h"
#include "game_sa\Darkel.h"
#include "game_sa\Date.h"
#include "game_sa\Decision.h"
#include "game_sa\DecisionMaker.h"
#include "game_sa\DecisionSimple.h"
#include "game_sa\Directory.h"
#include "game_sa\Draw.h"
#include "game_sa\EntityScanner.h"
#include "game_sa\EntryExit.h"
#include "game_sa\Explosion.h"
#include "game_sa\FileLoader.h"
#include "game_sa\FileMgr.h"
#include "game_sa\FileObjectInstance.h"
#include "game_sa\Font.h"
#include "game_sa\Formation.h"
#include "game_sa\Game.h"
#include "game_sa\GangInfo.h"
#include "game_sa\Gangs.h"
#include "game_sa\GangWars.h"
#include "game_sa\Garage.h"
#include "game_sa\Garages.h"
#include "game_sa\General.h"
#include "game_sa\GenericGameStorage.h"
#include "game_sa\cHandlingDataMgr.h"
#include "game_sa\Hud.h"
#include "game_sa\HudColours.h"
#include "game_sa\IniFile.h"
#include "game_sa\IplStore.h"
#include "game_sa\LoadedCarGroup.h"
#include "game_sa\LoadingScreen.h"
#include "game_sa\Localisation.h"
#include "game_sa\MenuManager.h"
#include "game_sa\MenuSystem.h"
#include "game_sa\Messages.h"
#include "game_sa\Mirrors.h"
#include "game_sa\MissionCleanup.h"
#include "game_sa\ModelInfoAccelerator.h"
#include "game_sa\NodeAddress.h"
#include "game_sa\ObjectData.h"
#include "game_sa\common.h"
#include "game_sa\CustomRoadsignMgr.h"
#include "game_sa\CompressedVector.h"
#include "game_sa\CompressedMatrixNotAligned.h"
#include "game_sa\KeyboardState.h"
#include "game_sa\MouseControllerState.h"
#include "game_sa\ControllerState.h"
#include "game_sa\Pad.h"
#include "game_sa\PathFind.h"
#include "game_sa\PedGroup.h"
#include "game_sa\PedGroupIntelligence.h"
#include "game_sa\PedGroupMembership.h"
#include "game_sa\PedGroupPlacer.h"
#include "game_sa\PedGroups.h"
#include "game_sa\PedIK.h"
#include "game_sa\PedIntelligence.h"
#include "game_sa\PedList.h"
#include "game_sa\PedTaskPair.h"
#include "game_sa\PedAttractorManager.h"
#include "game_sa\Pickup.h"
#include "game_sa\Pickups.h"
#include "game_sa\PlayerPedData.h"
#include "game_sa\PlayerInfo.h"
#include "game_sa\PointLights.h"
#include "game_sa\PointList.h"
#include "game_sa\PolyBunch.h"
#include "game_sa\Pools.h"
#include "game_sa\PopCycle.h"
#include "game_sa\Population.h"
#include "game_sa\ProjectileInfo.h"
#include "game_sa\QueuedMode.h"
#include "game_sa\RealTimeShadow.h"
#include "game_sa\RealTimeShadowManager.h"
#include "game_sa\Reference.h"
#include "game_sa\References.h"
#include "game_sa\RegisteredCorona.h"
#include "game_sa\RegisteredMotionBlurStreak.h"
#include "game_sa\Renderer.h"
#include "game_sa\CustomBuildingRenderer.h"
#include "game_sa\RepeatSector.h"
#include "game_sa\Restart.h"
#include "game_sa\RGBA.h"
#include "game_sa\RideAnims.h"
#include "game_sa\RideAnimData.h"
#include "game_sa\RoadBlocks.h"
#include "game_sa\Scene.h"
#include "game_sa\ScriptResourceManager.h"
#include "game_sa\ScriptsForBrains.h"
#include "game_sa\Sector.h"
#include "game_sa\SetPiece.h"
#include "game_sa\SetPieces.h"
#include "game_sa\ShadowCamera.h"
#include "game_sa\Shadows.h"
#include "game_sa\ShinyTexts.h"
#include "game_sa\Shopping.h"
#include "game_sa\ShotInfo.h"
#include "game_sa\SimpleTransform.h"
#include "game_sa\SpecialFX.h"
#include "game_sa\SpecialPlateHandler.h"
#include "game_sa\Sprite.h"
#include "game_sa\Sprite2d.h"
#include "game_sa\Stats.h"
#include "game_sa\StencilShadowObject.h"
#include "game_sa\StencilShadowObjects.h"
#include "game_sa\StencilShadows.h"
#include "game_sa\StoredCollPoly.h"
#include "game_sa\StreamedScripts.h"
#include "game_sa\Streaming.h"
#include "game_sa\StreamingInfo.h"
#include "game_sa\StuckCarCheck.h"
#include "game_sa\Text\Text.h"
#include "game_sa\TheZones.h"
#include "game_sa\TimeCycle.h"
#include "game_sa\Timer.h"
#include "game_sa\TxdStore.h"
#include "game_sa\VisibilityPlugins.h"
#include "game_sa\Wanted.h"
#include "game_sa\Weapon.h"
#include "game_sa\WeaponEffects.h"
#include "game_sa\WeaponInfo.h"
#include "game_sa\Weather.h"
#include "game_sa\World.h"
#include "game_sa\Zone.h"
#include "game_sa\ZoneInfo.h"
#include "game_sa\IplDef.h"
#include "game_sa\RpHAnimBlendInterpFrame.h"
#include "game_sa\RwObjectNameIdAssocation.h"
#include "game_sa\TxdDef.h"
#include "game_sa\GameLogic.h"
#include "game_sa\Maths.h"
#include "game_sa\Remote.h"
#include "game_sa\Animation\AnimAssociationData.h"
#include "game_sa\Animation\AnimBlendFrameData.h"
#include "game_sa\Animation\AnimSequenceFrames.h"
#include "game_sa\Animation\AnimationStyleDescriptor.h"
#include "game_sa\Animation\AnimBlendAssocGroup.h"
#include "game_sa\Animation\AnimBlendAssociation.h"
#include "game_sa\Animation\AnimBlendClumpData.h"
#include "game_sa\Animation\AnimBlendHierarchy.h"
#include "game_sa\Animation\AnimBlendNode.h"
#include "game_sa\Animation\AnimBlendSequence.h"
#include "game_sa\Animation\AnimBlendStaticAssociation.h"
#include "game_sa\Animation\AnimBlock.h"
#include "game_sa\Animation\AnimManager.h"
#include "game_sa\Audio\AudioEngine.h"
#include "game_sa\Audio\AESound.h"
#include "game_sa\Audio\AudioZones.h"
#include "game_sa\Collision\Collision.h"

#include "game_sa\Entity\AnimatedBuilding.h"
#include "game_sa\Entity\Building.h"
#include "game_sa\Entity\Entity.h"
#include "game_sa\Entity\Physical.h"
#include "game_sa\Entity\Placeable.h"
#include "game_sa\Entity\Dummy\Dummy.h"
#include "game_sa\Entity\Dummy\DummyObject.h"
#include "game_sa\Entity\Dummy\DummyPed.h"
#include "game_sa\Entity\Object\CutsceneObject.h"
#include "game_sa\Entity\Object\HandObject.h"
#include "game_sa\Entity\Object\Object.h"
#include "game_sa\Entity\Object\Projectile.h"
#include "game_sa\Entity\Ped\CivilianPed.h"
#include "game_sa\Entity\Ped\CopPed.h"
#include "game_sa\Entity\Ped\EmergencyPed.h"
#include "game_sa\Entity\Ped\Ped.h"
#include "game_sa\Entity\Ped\PlayerSkin.h"
#include "game_sa\Entity\Ped\PlayerPed.h"
#include "game_sa\Entity\Vehicle\Automobile.h"
#include "game_sa\Entity\Vehicle\Bike.h"
#include "game_sa\Entity\Vehicle\Bmx.h"
#include "game_sa\Entity\Vehicle\Boat.h"
#include "game_sa\Entity\Vehicle\Heli.h"
#include "game_sa\Entity\Vehicle\MonsterTruck.h"
#include "game_sa\Entity\Vehicle\Plane.h"
#include "game_sa\Entity\Vehicle\QuadBike.h"
#include "game_sa\Entity\Vehicle\Trailer.h"
#include "game_sa\Entity\Vehicle\Train.h"
#include "game_sa\Entity\Vehicle\Vehicle.h"

#include "game_sa\Audio\config\eAudioBank.h"
#include "game_sa\Audio\config\eAudioSlot.h"
#include "game_sa\Audio\config\eSFX.h"

#include "game_sa\Fx\eFxInfoType.h"
#include "game_sa\Fx\FxManager.h"
#include "game_sa\Fx\Fx.h"

#include "game_sa\Models\AtomicModelInfo.h"
#include "game_sa\Models\LodAtomicModelInfo.h"
#include "game_sa\Models\DamageAtomicModelInfo.h"
#include "game_sa\Models\BaseModelInfo.h"
#include "game_sa\Models\ClumpModelInfo.h"
#include "game_sa\Models\ModelInfo.h"
#include "game_sa\Models\PedModelInfo.h"
#include "game_sa\Models\TimeModelInfo.h"
#include "game_sa\Models\LodTimeModelInfo.h"
#include "game_sa\Models\VehicleModelInfo.h"
#include "game_sa\Models\WeaponModelInfo.h"
#include "game_sa\Plugins\JPegCompressPlugin\JPegCompress.h"
#include "game_sa\Plugins\NodeNamePlugin\NodeName.h"
#include "game_sa\Plugins\PipelinePlugin\PipelinePlugin.h"
#include "game_sa\Plugins\CollisionPlugin\CollisionPlugin.h"

#include "game_sa\RenderWare\D3DIndexDataBuffer.h"
#include "game_sa\RenderWare\D3DResourceSystem.h"
#include "game_sa\RenderWare\D3DTextureBuffer.h"
#include "game_sa\RenderWare\RenderWare.h"
#include "game_sa\RenderWare\rw\rpanisot.h"
#include "game_sa\RenderWare\rw\rpcriter.h"
#include "game_sa\RenderWare\rw\rperror.h"
#include "game_sa\RenderWare\rw\rphanim.h"
#include "game_sa\RenderWare\rw\rpmatfx.h"
#include "game_sa\RenderWare\rw\rpskin.h"
#include "game_sa\RenderWare\rw\rpuvanim.h"
#include "game_sa\RenderWare\rw\rpworld.h"
#include "game_sa\RenderWare\rw\rtanim.h"
#include "game_sa\RenderWare\rw\rtbmp.h"
#include "game_sa\RenderWare\rw\rtdict.h"
#include "game_sa\RenderWare\rw\rtpng.h"
#include "game_sa\RenderWare\rw\rtquat.h"
#include "game_sa\RenderWare\rw\rwcore.h"
#include "game_sa\RenderWare\rw\rwplcore.h"
#include "game_sa\RenderWare\rw\rwtexdict.h"
#include "game_sa\RenderWare\rw\skeleton.h"

#include "game_sa\Scripts\RunningScript.h"
#include "game_sa\Scripts\TheScripts.h"
#include "game_sa\Tasks\Task.h"
#include "game_sa\Tasks\TaskComplex.h"
#include "game_sa\Tasks\TaskManager.h"
#include "game_sa\Tasks\TaskSimple.h"
#include "game_sa\Tasks\TaskTimer.h"
#include "game_sa\Tasks\TaskComplexSequence.h"
#include "game_sa\Tasks\TaskComplexUseSequence.h"
#include "game_sa\Tasks\TaskSequences.h"
#include "game_sa\Tasks\PedScriptedTaskRecord.h"
#include "game_sa\Tasks\ScriptedBrainTaskStore.h"

#ifdef EXTRA_DEBUG_FEATURES
#include "toolsmenu\DebugModules\COcclusionDebugModule.h"
#endif
