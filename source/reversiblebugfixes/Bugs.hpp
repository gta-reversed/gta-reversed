#pragma once

#include "ReversibleBugFix.hpp"

/*!
* This is where the bugs are defined.
* The usual naming convention is `ClassName_Function_BugName` (`BugName` is something unique you come up with).
* Unless the same bug is spanning across multiple functions.
* 
* Fixes that prevent OOB should use `GenericOOB`.
* Fixes that prevent crashes (Like additional null ptr checks) can use `GenericCrash`.
* Feel free to add a new generic bug type if there's nothing that fits your needs.
*/
namespace notsa::bugfixes {
//
// Generic
//
inline const ReversibleBugFix GenericCrashing{
    .Name        = "Generic crashes (null ptr access, etc)",
    .Description = "Fixes bugs that cause null ptr access, and similar anomalies crashing the game",
    .Credit      = "Contributors"
};
inline const ReversibleBugFix GenericUB{
    .Name        = "Generic undefined behaviour (Use-after-free, etc)",
    .Description = "Fixes game crashes/bugs",
    .Credit      = "Contributors"
};
inline const ReversibleBugFix GenericOOB{
    .Name        = "Generic out-of-bounds bugs",
    .Description = "Fixes generic out-of-bounds bugs across the codebase",
    .Credit      = "Contributors"
};
inline const ReversibleBugFix GenericFrameRate{
    .Name        = "Generic framerate related bugs",
    .Description = "Fixes parts of code to not be framerate dependent",
    .Credit      = "Contributors"
};

//
// Other bugs
//
inline const ReversibleBugFix PS2CoronaRendering{
    .Name        = "PS2 Corona Rendering",
    .Description = "Fix corona rendering, so they're like on PS2",
    .Credit      = "SilentPatch Contributors"
};
inline const ReversibleBugFix AnimBlendSequence_SetName_SetBoneTagFlag{
    .Name        = "BoneTag Name Flag",
    .Description = "Correctly set BoneTag flag in `CAnimBlendSequence::SetName`",
    .Credit      = "Pirulax"
};
inline const ReversibleBugFix AESound_UpdatePlayTime_DivisionByZero{
    .Name        = "UpdatePlayTime Division-By-Zero",
    .Description = "Avoid Division-by-zero in CAESound::UpdatePlayTime",
    .Credit      = "Pirulax"
};
inline const ReversibleBugFix CCarCtrl_RemoveDistantCars_UseAfterFree{
    .Name        = "CCarCtrl::RemoveDistantCars Use-After-Free",
    .Description = "Fix user-after-free of vehicles (possibly) deleted by PossiblyRemoveVehicle",
    .Credit      = "Pirulax"
};
inline const ReversibleBugFix CAEVehicleAudioEntity_PlayBicycleSound_VolumeFix{
    .Name        = "CAEVehicleAudioEntity::PlayBicycleSound Volume fix",
    .Description = "Original code didn't account for event base volume",
    .Credit      = "Pirulax"
};
inline const ReversibleBugFix CAEVehicleAudioEntity_PlayAircraftSound_VolumeFix{
    .Name        = "CAEVehicleAudioEntity::PlayAircraftSound Volume fix",
    .Description = "Original code didn't account for event base volume",
    .Credit      = "Pirulax"
};
inline const ReversibleBugFix CPathFind_SwitchRoadsOffInArea_StrayAreas{
    .Name        = "CPathFind::SwitchRoadsOffInArea Stray-Areas",
    .Description = "Fix multiple issues related to saving unused path areas after missions",
    .Credit      = "Contributors"
};
inline const ReversibleBugFix CCustomCarPlateMgr_GeneratePlateText_MissingLettersAndDigits{
    .Name        = "CCustomCarPlateMgr::GeneratePlateText - Expand plate character range",
    .Description = "The original game generates plate letters in the range A-W (23 chars) and digits in 0-8 (9 chars). "
    "This fix expands those ranges to A-Z (26 chars) and 0-9 (10 chars) for more variety.",
    .Credit      = "j0y"
};
inline const ReversibleBugFix CTaskComplexLeaveCarAndFlee_MissingNullCheckForVehicleOnFlee{
    .Name        = "CTaskComplexLeaveCarAndFlee - Missing null check for vehicle on flee",
    .Description = "Fix missing null check for vehicle in CTaskComplexLeaveCarAndFlee",
    .Credit      = "Pirulax"
};
inline const ReversibleBugFix CAECollisionAudioEntity_PlayLoopingCollisionSound_InvalidSurfaceType{
    .Name        = "CAECollisionAudioEntity::PlayLoopingCollisionSound - Invalid surface type causing OOB",
    .Description = "The surface type passed in could've been an `AE_SURFACE_TYPE_*` which when passed to `g_surfaceInfos` causes an OOB. "
                   "The fix ensures the surface type is valid before accessing `g_surfaceInfos`.",
    .Credit      = "Pirulax"
};
inline const ReversibleBugFix CPool_DestructOnClear{
    .Name        = "Destroy objects before marking their memory as free",
    .Description = "Destruct all objects in the pool before deallocating their memory (So that pool objects can clean up after themselves)",
    .Credit      = "Pirulax"
};
inline const ReversibleBugFix CTaskSimpleClimb_ProcessPed_SettleSpeedFrameRate{
    .Name        = "Climb settle speed at high frame rates",
    .Description = "Bound the climb's settle step the way the coarse step above it is bounded, so a few centimetres of offset do not become a killing fall speed above 60 FPS",
    .Credit      = "mrxenginner"
};
inline const ReversibleBugFix CDamageManager_GetLightStatus_IncorrectStatusCheckForLightRR{
    .Name        = "CDamageManager::GetLightStatus - Incorrect status check for LIGHT_REAR_RIGHT",
    .Description = "Fixes incorrect use of `LIGHT_REAR_LEFT` instead of `LIGHT_REAR_RIGHT` for checking light status",
    .Credit      = "aeaeo"
};
inline const ReversibleBugFix CTaskSimpleSlideToCoord_Constructor_UninitializedTimer{
    .Name        = "CTaskSimpleSlideToCoord - Uninitialized timer",
    .Description = "The constructor without an anim doesn't initialize the timer, so `ProcessPed` never starts the 2 second timeout, "
                   "and a ped that can't reach the target position or heading slides forever",
    .Credit      = "W4liss0n"
};
inline const ReversibleBugFix CPedGeometryAnalyser_ComputeRouteRoundSphere_IncorrectDetourPosition{
    .Name        = "CPedGeometryAnalyser::ComputeRouteRoundSphere - Incorrect detour position",
    .Description = "Fix incorrect calculation of detour position in `CPedGeometryAnalyser::ComputeRouteRoundSphere` due to the direction being calculated in 3D instead of 2D (Sometimes giving points that would lead the ped to go into the sphere, instead of around it)",
    .Credit      = "Pirulax"
};
inline const ReversibleBugFix CPedGeometryAnalyser_IsEntityBlockingTarget_IncorrectRadiusCheck{
    .Name        = "CPedGeometryAnalyser::IsEntityBlockingTarget - Incorrect radius check",
    .Description = "Fix incorrect radius check in `CPedGeometryAnalyser::IsEntityBlockingTarget`",
    .Credit      = "Pirulax"
};
inline const ReversibleBugFix CPedGeometryAnalyser_ComputeRouteRoundSphere_AmbigousDetourTarget{
    .Name        = "CPedGeometryAnalyser::ComputeRouteRoundSphere - Fix ambiguous detour target",
    .Description = "Fix missing setting of detour target when the ped is on the edge of the sphere in `CPedGeometryAnalyser::ComputeRouteRoundSphere`",
    .Credit      = "Pirulax"
};
inline const ReversibleBugFix CObjectData_SetObjectData_UninitializedPhysics{
    .Name        = "Physics of objects without object.dat data",
    .Description = "Give an object with no object.dat entry the default elasticity and buoyancy - neither SetObjectData nor CPhysical's constructor sets them, so they held the pool slot's leftovers and could launch a vehicle that touched the object to infinity",
    .Credit      = "mrxenginner"
};
inline const ReversibleBugFix CObject_Render_DetachedPartRendering{
    .Name        = "CObject::Render - Detached car part rendering",
    .Description = "Detached car parts share their materials with the vehicle model, but nothing applies the vehicle's colours and paintjob to them, "
                   "so they were rendered with the placeholder colours. Apply them before rendering the part, and render both of its sides",
    .Credit      = "SilentPatch Contributors"
};
inline const ReversibleBugFix CVehicleModelInfo_SetClump_DirtMaterialsNeverFound{
    .Name            = "Vehicle dirt levels",
    .Description     = "The game never fills `CVehicleModelInfo::m_apDirtMaterials`, so every vehicle is rendered with the maximum dirt "
                       "regardless of its dirt level. The fix fills it with the materials that use the `vehiclegrunge256` texture (like SilentPatch does)",
    .Credit          = "SilentPatch Contributors, W4liss0n",
    .RequiresRestart = true
};
inline const ReversibleBugFix CVehicle_ProcessWheel_and_ProcessBikeWheel_AlreadySkiddingNotReset{
    .Name        = "CVehicle::ProcessWheel/ProcessBikeWheel - alreadySkidding reset",
    .Description = "There are 3 and 4 statics, respectively, describing wheel state which are reset every frame EXCEPT for alreadySkidding. The fix makes alreadySkidding resetable, it has an impact on physics model since traction loss is now calculated as originally intended",
    .Credit      = "re3/VC Contributors"
};
inline const ReversibleBugFix CVehicle_ScanAndMarkTargetForHeatSeekingMissile_NotProperlyStashedLOSTimestamps{
    .Name        = "CVehicle::CVehicle_ScanAndMarkTargetForHeatSeekingMissile - LOS timestamps",
    .Description = "Idea is to stash LOS target visibility var to not call CWorld::GetIsLineOfSightClear every frame, for some reason they use == 0 and > 1 ops respectively for last time LOS check and then flag set, so check performed every frame and not each ~1 sec",
    .Credit      = "aeaeo"
};
};
