#include "StdInc.h"

#include "MonsterTruck.h"

float& CMonsterTruck::DUMPER_COL_ANGLEMULT = *(float*)0x8D33A8;
float& fWheelExtensionRate = *(float*)0x8D33AC;

void CMonsterTruck::InjectHooks() {
    RH_ScopedClass(CMonsterTruck);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6C8D60);
    // RH_ScopedInstall(ProcessEntityCollision, 0x6C8AE0);
    // RH_ScopedInstall(ProcessSuspension, 0x6C83A0);
    // RH_ScopedInstall(ProcessControlCollisionCheck, 0x6C8330);
    // RH_ScopedInstall(ProcessControl, 0x6C8250);
    // RH_ScopedInstall(SetupSuspensionLines, 0x6C7FB0);
    RH_ScopedInstall(PreRender, 0x6C7DE0);
    // RH_ScopedInstall(ExtendSuspension, 0x6C7D80);
    // RH_ScopedInstall(ResetSuspension, 0x6C7D40);
    RH_ScopedInstall(BurstTyre_Reversed, 0x6C7D30);
    RH_ScopedInstall(SetUpWheelColModel_Reversed, 0x6C7D20);
}

// 0x6C8D60
CMonsterTruck::CMonsterTruck(int32 modelIndex, eVehicleCreatedBy createdBy) : CAutomobile(modelIndex, createdBy, false) {
    std::ranges::fill(field_988, 1.0f);
    SetupSuspensionLines();
    npcFlags.bSoftSuspension = true;
    m_nVehicleSubType = VEHICLE_TYPE_MTRUCK;
}

// 0x6C8AE0
int32 CMonsterTruck::ProcessEntityCollision(CEntity* entity, CColPoint* colPoint) {
    return plugin::CallMethodAndReturn<int32, 0x6C8AE0, CMonsterTruck*, CEntity*, CColPoint*>(this, entity, colPoint);
}

// 0x6C83A0
void CMonsterTruck::ProcessSuspension() {
    plugin::CallMethod<0x6C83A0, CMonsterTruck*>(this);
}

// 0x6C8330
void CMonsterTruck::ProcessControlCollisionCheck(bool applySpeed) {
    plugin::CallMethod<0x6C8330, CMonsterTruck*, bool>(this, applySpeed);
}

// 0x6C8250
void CMonsterTruck::ProcessControl() {
    plugin::CallMethod<0x6C8250, CMonsterTruck*>(this);
}

// 0x6C7FB0
void CMonsterTruck::SetupSuspensionLines() {
    plugin::CallMethod<0x6C7FB0, CMonsterTruck*>(this);
}

// 0x6C7DE0
void CMonsterTruck::PreRender() {
    for (auto i = 0; i < 4; i++) {
        m_wheelPosition[i] = std::min(m_wheelPosition[i], m_aSuspensionSpringLength[i]);
    }

    CAutomobile::PreRender();

    const auto mi = GetVehicleModelInfo();
    CMatrix mat;
    CVector pos;

    mi->GetWheelPosn(CARWHEEL_FRONT_LEFT, pos, false);
    SetTransmissionRotation(m_aCarNodes[MONSTER_TRANSMISSION_F], m_wheelPosition[CARWHEEL_FRONT_LEFT], m_wheelPosition[CARWHEEL_FRONT_RIGHT], pos, true);

    mi->GetWheelPosn(CARWHEEL_REAR_LEFT, pos, false);
    SetTransmissionRotation(m_aCarNodes[MONSTER_TRANSMISSION_R], m_wheelPosition[CARWHEEL_REAR_LEFT], m_wheelPosition[CARWHEEL_REAR_RIGHT], pos, false);

    if (m_nModelIndex == MODEL_DUMPER) {
        if (auto node = m_aCarNodes[MONSTER_MISC_A]) {
            SetComponentRotation(node, AXIS_X, (float)m_wMiscComponentAngle * DUMPER_COL_ANGLEMULT, true);
        }
    }
}

// 0x6C7D80
void CMonsterTruck::ExtendSuspension() {
    plugin::CallMethod<0x6C7D80, CMonsterTruck*>(this);
}

// 0x6C7D40
void CMonsterTruck::ResetSuspension() {
    plugin::CallMethod<0x6C7D40, CMonsterTruck*>(this);
    return;

    CAutomobile::ResetSuspension();
    for (auto& suspension : m_wheelPosition) {
        suspension = 1.0f;
    }
}

// 0x6C7D30
bool CMonsterTruck::BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) {
    return false;
}

// 0x6C7D20
bool CMonsterTruck::SetUpWheelColModel(CColModel* colModel) {
    return false;
}
