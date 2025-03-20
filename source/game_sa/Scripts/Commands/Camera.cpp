#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>

#include "CommandParser/Parser.hpp"
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

// COMMAND_DO_FADE @ 0x47C7C7
void DoCamFade(CRunningScript* S, uint32 time, eFadeFlag direction) {
    TheCamera.Fade((float)time / 1000.f, direction);
    if (S->m_UsesMissionCleanup) {
        CTheScripts::bScriptHasFadedOut = direction == eFadeFlag::FADE_IN;
    }
}
};

void notsa::script::commands::camera::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Camera");

    REGISTER_COMMAND_HANDLER(COMMAND_IS_POINT_ON_SCREEN, IsPointOnScreen);
    REGISTER_COMMAND_HANDLER(COMMAND_SHAKE_CAM, ShakeCam);
    REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CAMERA_TO_VEHICLE_LOOK_AT_VEHICLE, AttachCameraToVehicleLookAtVehicle);
    REGISTER_COMMAND_HANDLER(COMMAND_DO_CAMERA_BUMP, DoCameraBump);
    REGISTER_COMMAND_HANDLER(COMMAND_DO_FADE, DoCamFade);
}
