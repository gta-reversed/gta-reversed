#include <StdInc.h>

#include "Commands.hpp"
#include <CommandParser/Parser.hpp>

using namespace notsa::script;
/*!
* Various camera commands
*/

namespace {
bool IsPointOnScreen(CVector pos, float radius) {
    if (pos.z <= MAP_Z_LOW_LIMIT) {
        pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
    }
    return TheCamera.IsSphereVisible(pos, radius);
}

void ShakeCam(float strength) {
    CamShakeNoPos(&TheCamera, strength / 1000.0f);
}

void AttachCameraToVehicleLookAtVehicle(CVehicle& attachTo, CVector offset, CVehicle& lookAt, float tilt, eSwitchType switchType) {
    CVector zero{};
    TheCamera.TakeControlAttachToEntity(
        &lookAt,
        &attachTo,
        &offset,
        &zero,
        tilt,
        switchType,
        1
    );
}

void DoCameraBump(float horizontal, float vertical) {
    CCamera::GetActiveCamera().DoCamBump(horizontal, vertical);
}

// DO_FADE - 0x47C7C7
void DoCamFade(CRunningScript* S, uint32 time, eFadeFlag direction) {
    TheCamera.Fade((float)time / 1000.f, direction);
    if (S->m_UsesMissionCleanup) {
        CTheScripts::bScriptHasFadedOut = direction == eFadeFlag::FADE_IN;
    }
}


// POINT_CAMERA_AT_CAR - 0x47C3FD
void PointCameraAtCar(CVehicle* car, eCamMode mode, eSwitchType switchType) {
    if (!car) {
        return;
    }
    TheCamera.TakeControl(car, mode, switchType, 1);
}

// POINT_CAMERA_AT_CHAR - 0x47C44A
void PointCameraAtChar(CPed* ped, eCamMode mode, eSwitchType switchType) {
    if (!ped) {
        return;
    }
    TheCamera.TakeControl(ped->IsPlayer() ? ped : nullptr, mode, switchType, 1);
}

// RESTORE_CAMERA (346) - 0x47C4CC
void RestoreCamera() {
    TheCamera.Restore();
}

// SET_FIXED_CAMERA_POSITION (351) - 0x47C53B
void SetFixedCameraPosition(CVector pos, CVector offset) {
    TheCamera.SetCamPositionForFixedMode(pos, offset);
}

// POINT_CAMERA_AT_POINT (352) - 0x47C59C
void PointCameraAtPoint(CVector pos, eSwitchType switchType) {
    if (pos.z <= -100.f) {
        pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
    }
    TheCamera.TakeControlNoEntity(pos, switchType, 1);
}

// RESTORE_CAMERA_JUMPCUT (747) - 0x4807A3
void RestoreCameraJumpcut() {
    TheCamera.RestoreWithJumpCut();
}

// SET_CAMERA_ZOOM (810) - 0x48161D
void SetCameraZoom(int16 zoomMode) { 
    switch (TheCamera.GetActiveCam().m_nMode) {
    case MODE_FOLLOWPED:       TheCamera.SetZoomValueFollowPedScript(zoomMode); break;
    case MODE_CAM_ON_A_STRING: TheCamera.SetZoomValueCamStringScript(zoomMode); break;
    }
}

// SET_CAMERA_BEHIND_PLAYER (883) - 0x483320
void SetCameraBehindPlayer() {
    TheCamera.SetCameraDirectlyBehindForFollowPed_CamOnAString();
}

// SET_CAMERA_IN_FRONT_OF_PLAYER (968) - 0x48502B
void SetCameraInFrontOfPlayer() {
    TheCamera.SetCameraDirectlyInFrontForFollowPed_CamOnAString();
}

// GET_DEBUG_CAMERA_COORDINATES (1108) - 0x48A3E3
CVector GetDebugCameraCoordinates() {
    return TheCamera.m_aCams[2].m_vecSource;
}

// GET_DEBUG_CAMERA_POINT_AT (1123) - 0x48A776
CVector GetDebugCameraPointAt() {
    return TheCamera.m_aCams[2].m_vecSource + TheCamera.m_aCams[2].m_vecFront;
}

// IS_DEBUG_CAMERA_ON (1440) - 0x48F071
bool IsDebugCameraOn() {
    return TheCamera.m_bWorldViewerBeingUsed;
}

// ATTACH_CAMERA_TO_VEHICLE (1657) - 0x49506A
void AttachCameraToVehicle(CVehicle& attachTo, CVector offset, CVector rot, float tilt, eSwitchType switchType) {
    TheCamera.TakeControlAttachToEntity(
        nullptr,
        &attachTo,
        &offset,
        &rot,
        tilt,
        switchType,
        1
    );
}

// ATTACH_CAMERA_TO_VEHICLE_LOOK_AT_CHAR (1659) - 0x49518B
void AttachCameraToVehicleLookAtChar(CVehicle& attachTo, CVector offset, CPed& lookAt, float tilt, eSwitchType switchType) {
    TheCamera.TakeControlAttachToEntity(
        &lookAt,
        &attachTo,
        &offset,
        nullptr,
        tilt,
        switchType,
        1
    );
}

// ATTACH_CAMERA_TO_CHAR (1660) - 0x495210
void AttachCameraToChar(CPed& attachTo, CVector offset, CVector rot, float tilt, eSwitchType switchType) {
    TheCamera.TakeControlAttachToEntity(
        nullptr,
        &attachTo,
        &offset,
        &rot,
        tilt,
        switchType,
        1
    );
}

// ATTACH_CAMERA_TO_CHAR_LOOK_AT_CHAR (1662) - 0x49531F
void AttachCameraToCharLookAtChar(CPed& attachTo, CVector offset, CPed& lookAt, float tilt, eSwitchType switchType) {
    TheCamera.TakeControlAttachToEntity(
        &lookAt,
        &attachTo,
        &offset,
        nullptr,
        tilt,
        switchType,
        1
    );
}

// GET_ACTIVE_CAMERA_COORDINATES (1677) - 0x495A84
CVector GetActiveCameraCoordinates() {
    return TheCamera.GetActiveCam().m_vecSource;
}

// GET_ACTIVE_CAMERA_POINT_AT (1678) - 0x495AA8
CVector GetActiveCameraPointAt() {
    return TheCamera.GetActiveCam().m_vecFront + TheCamera.GetActiveCam().m_vecSource;
}

// GET_CAMERA_FOV (2049)
float GetCameraFov() {
    return TheCamera.GetActiveCam().m_fFOV;
}

// SET_FIRST_PERSON_IN_CAR_CAMERA_MODE (2082)
void SetFirstPersonInCarCameraMode(bool enabled) {
    TheCamera.m_bDisableFirstPersonInCar = !enabled;
    if (!enabled) {
        TheCamera.m_nCarZoom = 2;
    }
}

// CAMERA_SET_VECTOR_TRACK (2336)
void CameraSetVectorTrack(CVector from, CVector to, int32 duration, bool ease) {
    TheCamera.VectorTrackLinear(&to, &from, (float)(duration), ease);
}

// CAMERA_SET_LERP_FOV (2338)
void CameraSetLerpFov(float from, float to, int32 duration, bool ease) {
    TheCamera.LerpFOV(from, to, (float)(duration), ease);
}

// CAMERA_RESET_NEW_SCRIPTABLES (2341)
void CameraResetNewScriptables() {
    TheCamera.InitialiseScriptableComponents();
}

// CAMERA_PERSIST_TRACK (2351)
void CameraPersistTrack(bool enabled) {
    TheCamera.m_bCameraPersistTrack = enabled;
}

// CAMERA_PERSIST_POS (2352)
void CameraPersistPos(bool enabled) {
    TheCamera.m_bCameraPersistPosition = enabled;
}

// CAMERA_PERSIST_FOV (2353)
void CameraPersistFov(bool enabled) {
    TheCamera.m_bBlockZoom = enabled;
}

// CAMERA_IS_VECTOR_MOVE_RUNNING (2355)
bool CameraIsVectorMoveRunning() {
    return TheCamera.VectorMoveRunning();
}

// CAMERA_IS_VECTOR_TRACK_RUNNING (2356)
bool CameraIsVectorTrackRunning() {
    return TheCamera.VectorTrackRunning();
}

// CAMERA_SET_VECTOR_MOVE (2358)
void CameraSetVectorMove(CVector from, CVector to, float time, bool ease) {
    TheCamera.VectorMoveLinear(&to, &from, time, ease);
}

// SET_CINEMA_CAMERA (2365)
void SetCinemaCamera(bool enabled) {
    TheCamera.m_bCinemaCamera = enabled;
}

// SET_CAMERA_IN_FRONT_OF_CHAR (2372)
void SetCameraInFrontOfChar(CPed& target) {
    TheCamera.SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString(&target);
}

// CAMERA_SET_SHAKE_SIMULATION_SIMPLE (2460)
void CameraSetShakeSimulationSimple(int32 type, float ms, float intensity) {
    TheCamera.AddShakeSimple(ms, type, intensity);
}

// SET_PLAYER_IN_CAR_CAMERA_MODE (2477)
void SetPlayerInCarCameraMode(int32 zoom) {
    if (zoom != 4) {
        TheCamera.m_nCarZoom = zoom;
    }
}

// ALLOW_FIXED_CAMERA_COLLISION (2540)
void AllowFixedCameraCollision(bool enabled) {
    gAllowScriptedFixedCameraCollision = enabled;
}

// SET_VEHICLE_CAMERA_TWEAK (2543)
void SetVehicleCameraTweak(eModelID model, float dist, float alt, float angle) {
    TheCamera.m_aCamTweak[4] = {
        .ModelID = model,
        .Dist    = dist,
        .Alt     = alt,
        .Angle   = angle
    };
}

// RESET_VEHICLE_CAMERA_TWEAK (2544)
void ResetVehicleCameraTweak() {
    TheCamera.m_aCamTweak[4] = {
        .ModelID = MODEL_INVALID,
        .Dist    = 1.f,
        .Alt     = 1.f,
        .Angle   = 0.f
    };
}

// SET_CAMERA_POSITION_UNFIXED (2597)
void SetCameraPositionUnfixed(CVector2D offset) {
    auto* const cam = &TheCamera.GetActiveCam();
    cam->m_fVerticalAngle = offset.x;
    cam->m_fHorizontalAngle = offset.y;
    if (cam->m_nMode == MODE_FOLLOWPED) {
        cam->Process_FollowPed_SA(FindPlayerPed()->GetPosition(), 0.f, 0.f, 0.f, true);
    } else if (auto* const veh = FindPlayerVehicle()) {
        if (notsa::contains({ MODE_CAM_ON_A_STRING, MODE_BEHINDBOAT, MODE_BEHINDCAR }, cam->m_nMode)) {
            cam->Process_FollowCar_SA(veh->GetPosition(), 0.f, 0.f, 0.f, true);
        }
    }
}

// GET_PLAYER_IN_CAR_CAMERA_MODE (2617)
auto GetPlayerInCarCameraMode() {
    return TheCamera.m_nCarZoom;
}
};

void notsa::script::commands::camera::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Camera");

    REGISTER_COMMAND_HANDLER(COMMAND_IS_POINT_ON_SCREEN, IsPointOnScreen);
    REGISTER_COMMAND_HANDLER(COMMAND_SHAKE_CAM, ShakeCam);
    REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CAMERA_TO_VEHICLE_LOOK_AT_VEHICLE, AttachCameraToVehicleLookAtVehicle);
    REGISTER_COMMAND_HANDLER(COMMAND_DO_CAMERA_BUMP, DoCameraBump);
    REGISTER_COMMAND_HANDLER(COMMAND_DO_FADE, DoCamFade);

    REGISTER_COMMAND_HANDLER(COMMAND_POINT_CAMERA_AT_CAR, PointCameraAtCar);
    REGISTER_COMMAND_HANDLER(COMMAND_POINT_CAMERA_AT_CHAR, PointCameraAtChar);
    REGISTER_COMMAND_HANDLER(COMMAND_RESTORE_CAMERA, RestoreCamera);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_FIXED_CAMERA_POSITION, SetFixedCameraPosition);
    REGISTER_COMMAND_HANDLER(COMMAND_POINT_CAMERA_AT_POINT, PointCameraAtPoint);
    REGISTER_COMMAND_HANDLER(COMMAND_RESTORE_CAMERA_JUMPCUT, RestoreCameraJumpcut);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAMERA_ZOOM, SetCameraZoom);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAMERA_BEHIND_PLAYER, SetCameraBehindPlayer);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAMERA_IN_FRONT_OF_PLAYER, SetCameraInFrontOfPlayer);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_GENERATE_CARS_AROUND_CAMERA);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_DEBUG_CAMERA_COORDINATES, GetDebugCameraCoordinates);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_DEBUG_CAMERA_FRONT_VECTOR);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_DEBUG_CAMERA_POINT_AT, GetDebugCameraPointAt);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SWITCH_SECURITY_CAMERA);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_FIRST_PERSON_CONTROL_CAMERA);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SWITCH_LIFT_CAMERA);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_FIRST_PERSON_WEAPON_CAMERA);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_DEBUG_CAMERA_ON, IsDebugCameraOn);
    REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CAMERA_TO_VEHICLE, AttachCameraToVehicle);
    REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CAMERA_TO_VEHICLE_LOOK_AT_CHAR, AttachCameraToVehicleLookAtChar);
    REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CAMERA_TO_CHAR, AttachCameraToChar);
    REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CAMERA_TO_CHAR_LOOK_AT_CHAR, AttachCameraToCharLookAtChar);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_ACTIVE_CAMERA_COORDINATES, GetActiveCameraCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_ACTIVE_CAMERA_POINT_AT, GetActiveCameraPointAt);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ATTACH_CAMERA_TO_OBJECT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ATTACH_CAMERA_TO_OBJECT_LOOK_AT_VEHICLE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ATTACH_CAMERA_TO_OBJECT_LOOK_AT_CHAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ATTACH_CAMERA_TO_OBJECT_LOOK_AT_OBJECT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ATTACH_CAMERA_TO_VEHICLE_LOOK_AT_OBJECT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_DRAW_CROSS_IN_FRONT_OF_DEBUG_CAMERA);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAMERA_FOV, GetCameraFov);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_FIRST_PERSON_IN_CAR_CAMERA_MODE, SetFirstPersonInCarCameraMode);
    REGISTER_COMMAND_HANDLER(COMMAND_CAMERA_SET_VECTOR_TRACK, CameraSetVectorTrack);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CAMERA_SET_SHAKE_SIMULATION);
    REGISTER_COMMAND_HANDLER(COMMAND_CAMERA_SET_LERP_FOV, CameraSetLerpFov);
    REGISTER_COMMAND_HANDLER(COMMAND_CAMERA_RESET_NEW_SCRIPTABLES, CameraResetNewScriptables);
    REGISTER_COMMAND_HANDLER(COMMAND_CAMERA_PERSIST_TRACK, CameraPersistTrack);
    REGISTER_COMMAND_HANDLER(COMMAND_CAMERA_PERSIST_POS, CameraPersistPos);
    REGISTER_COMMAND_HANDLER(COMMAND_CAMERA_PERSIST_FOV, CameraPersistFov);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CAMERA_IS_FOV_RUNNING);
    REGISTER_COMMAND_HANDLER(COMMAND_CAMERA_IS_VECTOR_MOVE_RUNNING, CameraIsVectorMoveRunning);
    REGISTER_COMMAND_HANDLER(COMMAND_CAMERA_IS_VECTOR_TRACK_RUNNING, CameraIsVectorTrackRunning);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CAMERA_IS_SHAKE_RUNNING);
    REGISTER_COMMAND_HANDLER(COMMAND_CAMERA_SET_VECTOR_MOVE, CameraSetVectorMove);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CINEMA_CAMERA, SetCinemaCamera);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CAMERA_BEHIND_CHAR);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAMERA_IN_FRONT_OF_CHAR, SetCameraInFrontOfChar);
    REGISTER_COMMAND_HANDLER(COMMAND_CAMERA_SET_SHAKE_SIMULATION_SIMPLE, CameraSetShakeSimulationSimple);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_IN_CAR_CAMERA_MODE, SetPlayerInCarCameraMode);
    REGISTER_COMMAND_HANDLER(COMMAND_ALLOW_FIXED_CAMERA_COLLISION, AllowFixedCameraCollision);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_VEHICLE_CAMERA_TWEAK, SetVehicleCameraTweak);
    REGISTER_COMMAND_HANDLER(COMMAND_RESET_VEHICLE_CAMERA_TWEAK, ResetVehicleCameraTweak);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAMERA_POSITION_UNFIXED, SetCameraPositionUnfixed);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_PLAYER_IN_CAR_CAMERA_MODE, GetPlayerInCarCameraMode);
}
