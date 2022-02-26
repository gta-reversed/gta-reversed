#include "StdInc.h"
#include "TaskSimpleIKLookAt.h"

void CTaskSimpleIKLookAt::InjectHooks() {
    RH_ScopedClass(CTaskSimpleIKLookAt);
    RH_ScopedCategoryGlobal(); // TODO: Change this to the appropriate category!

    RH_ScopedInstall(Constructor, 0x633E00);
    RH_ScopedOverloadedInstall(Destructor, "", 0x633EF0, CTaskSimpleIKLookAt*(CTaskSimpleIKLookAt::*)());

    // RH_ScopedInstall(UpdateLookAtInfo, 0x634050);
    // RH_ScopedInstall(GetLookAtEntity, 0x634120);
    // RH_ScopedInstall(GetLookAtOffset, 0x634130);
    // RH_ScopedInstall(Clone_Reversed, 0x633F00);
    // RH_ScopedInstall(GetTaskType_Reversed, 0x633EE0);
    // RH_ScopedInstall(CreateIKChain_Reversed, 0x633FC0);
}

// 0x633E00
CTaskSimpleIKLookAt::CTaskSimpleIKLookAt(const char* name, CEntity* lookAtEntity, int32 time, ePedBones pedBoneID, CVector lookAtOffset, bool useTorso, float speed, uint32 blendTime, uint8 priority) :
    CTaskSimpleIKChain{ name, BONE_HEAD, {0.f, 0.05f, 0.f}, BONE_NORMAL, lookAtEntity, pedBoneID, lookAtOffset, speed, time, blendTime},
    m_nPriority{priority},
    m_bUseTorso{useTorso}
{
}

// 0x633E00
CTaskSimpleIKLookAt* CTaskSimpleIKLookAt::Constructor(char* name, CEntity* lookAtEntity, int32 time, ePedBones pedBoneID, RwV3d lookAtOffset, uint8 useTorso, float fSpeed, int32 blendTime,
                                                      int32 priority) {
    this->CTaskSimpleIKLookAt::CTaskSimpleIKLookAt(name, lookAtEntity, time, pedBoneID, lookAtOffset, useTorso, fSpeed, blendTime, priority);
    return this;
}

// 0x633EF0
CTaskSimpleIKLookAt::~CTaskSimpleIKLookAt() {}

// 0x633EF0
CTaskSimpleIKLookAt* CTaskSimpleIKLookAt::Destructor() {
    this->CTaskSimpleIKLookAt::~CTaskSimpleIKLookAt();
    return this;
}

// 0x634050
void CTaskSimpleIKLookAt::UpdateLookAtInfo(const char* strPurpose, CPed* ped, CEntity* targetPed, int32 time, ePedBones pedBoneID, RwV3d lookAtOffset, bool useTorso, float fSpeed,
                                           int32 blendTime, int32 unused) {
    plugin::CallMethod<0x634050, CTaskSimpleIKLookAt*, const char*, CPed*, CEntity*, int32, ePedBones, RwV3d, bool, float, int32, int32>(this, strPurpose, ped, targetPed, time, pedBoneID,
                                                                                                                                lookAtOffset, useTorso, fSpeed, blendTime, unused);
}

// 0x634120
CEntity* CTaskSimpleIKLookAt::GetLookAtEntity() {
    return plugin::CallMethodAndReturn<CEntity*, 0x634120, CTaskSimpleIKLookAt*>(this);
}

// 0x634130
CVector CTaskSimpleIKLookAt::GetLookAtOffset() { // We return a vector here.. Hopefully this is ABI compatible.
    return plugin::CallMethodAndReturn<CVector, 0x634130, CTaskSimpleIKLookAt*>(this);
}

// 0x633F00
CTaskSimpleIKLookAt* CTaskSimpleIKLookAt::Clone() {
    return plugin::CallMethodAndReturn<CTaskSimpleIKLookAt*, 0x633F00, CTaskSimpleIKLookAt*>(this);
}

// 0x633FC0
bool CTaskSimpleIKLookAt::CreateIKChain(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x633FC0, CTaskSimpleIKLookAt*, CPed*>(this, ped);
}
