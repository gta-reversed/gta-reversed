#include "StdInc.h"

#include "IdleCam.h"

CIdleCam& gIdleCam = *(CIdleCam*)0xB6FDA0;
uint32& gbCineyCamProcessedOnFrame = *(uint32*)0xB6EC40;

void CIdleCam::InjectHooks() {
    RH_ScopedClass(CIdleCam);
    RH_ScopedCategoryGlobal();

    // RH_ScopedInstall(GetLookAtPositionOnTarget, 0x50EAE0);
    // RH_ScopedInstall(Init, 0x50E6D0);
    // RH_ScopedInstall(Reset, 0x50A160);
    // RH_ScopedInstall(ProcessIdleCamTicker, 0x50A200);
    // RH_ScopedInstall(SetTarget, 0x50A280);
    // RH_ScopedInstall(FinaliseIdleCamera, 0x50E760);
    // RH_ScopedInstall(SetTargetPlayer, 0x50EB50);
    // RH_ScopedInstall(IsTargetValid, 0x517770);
    // RH_ScopedInstall(ProcessTargetSelection, 0x517870);
    // RH_ScopedInstall(ProcessSlerp, 0x5179E0);
    // RH_ScopedInstall(ProcessFOVZoom, 0x517BF0);
    // RH_ScopedInstall(Run, 0x51D3E0);
    // RH_ScopedInstall(Process, 0x522C80);
}

// 0x517760
CIdleCam::CIdleCam() {
    Init();
}

// 0x50E6D0
void CIdleCam::Init() {
    plugin::CallMethod<0x50E6D0, CIdleCam*>(this);
}

// 0x50A160
void CIdleCam::Reset(bool a1) {
    plugin::CallMethod<0x50A160, CIdleCam*, bool>(this, a1);
}

// 0x50A200
void CIdleCam::ProcessIdleCamTicker() {
    plugin::CallMethod<0x50A200, CIdleCam*>(this);
}

// inlined
bool CIdleCam::IsItTimeForIdleCam() {
    // return (float)field_94 <= field_28;
    return false; // Android
}

// wrong name?
// 0x50E690
void CIdleCam::IdleCamGeneralProcess() {

}

// 0x50EAE0
void CIdleCam::GetLookAtPositionOnTarget(CEntity* target, CVector* posn) {
    plugin::CallMethod<0x50EAE0, CIdleCam*, CEntity*, CVector*>(this, target, posn);
}

// 0x517BF0
void CIdleCam::ProcessFOVZoom(float a1) {
    plugin::CallMethod<0x517BF0, CIdleCam*, float>(this, a1);
}

// 0x517770
bool CIdleCam::IsTargetValid(CEntity* target) {
    return plugin::CallMethodAndReturn<bool, 0x517770, CIdleCam*, CEntity*>(this, target);
}

// 0x50A280
void CIdleCam::SetTarget(CEntity* target) {
    plugin::CallMethod<0x50A280, CIdleCam*, CEntity*>(this, target);
}

// 0x50EB50
void CIdleCam::SetTargetPlayer() {
    plugin::CallMethod<0x50EB50, CIdleCam*>(this);
}

// 0x517870
void CIdleCam::ProcessTargetSelection() {
    plugin::CallMethod<0x517870, CIdleCam*>(this);
}

// inlined, see ProcessSlerp
void CIdleCam::VectorToAnglesRotXRotZ(CVector* posn, float* outA1, float* outA2) {
    assert(false);
}

// 0x5179E0
void CIdleCam::ProcessSlerp(float* outA1, float* outA2) {
    plugin::CallMethod<0x5179E0, CIdleCam*, float*, float*>(this, outA1, outA2);
}

// 0x50E760
void CIdleCam::FinaliseIdleCamera(float a1, float a2, float a3) {
    plugin::CallMethod<0x50E760, CIdleCam*, float, float, float>(this, a1, a2, a3);
}

// 0x51D3E0
void CIdleCam::Run() {
    plugin::CallMethod<0x51D3E0, CIdleCam*>(this);
}

// 0x522C80
void CIdleCam::Process() {
    plugin::CallMethod<0x522C80, CIdleCam*>(this);
}
