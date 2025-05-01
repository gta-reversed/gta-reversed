#include "StdInc.h"
#include "TaskSimpleHurtPedWithCar.h"

void CTaskSimpleHurtPedWithCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHurtPedWithCar, 0x86f2e4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x653EA0);
    RH_ScopedInstall(Destructor, 0x653F30);

    RH_ScopedVMTInstall(Clone, 0x655A90);
    RH_ScopedVMTInstall(GetTaskType, 0x653F10);
    RH_ScopedVMTInstall(MakeAbortable, 0x653F20);
    RH_ScopedVMTInstall(ProcessPed, 0x653F90);
}

// 0x653EA0
CTaskSimpleHurtPedWithCar::CTaskSimpleHurtPedWithCar(CVehicle* veh, float impulseMagnitude) :
    m_Car{veh},
    m_ImpulseMag{impulseMagnitude}
{
}

// 0x655A90
CTaskSimpleHurtPedWithCar::CTaskSimpleHurtPedWithCar(const CTaskSimpleHurtPedWithCar& o) :
    CTaskSimpleHurtPedWithCar{o.m_Car, o.m_ImpulseMag}
{
}

// 0x653F90
bool CTaskSimpleHurtPedWithCar::ProcessPed(CPed* ped) {
    CPedDamageResponseCalculator dmgRespCalc{
        m_Car,
        30.f,
        WEAPON_RAMMEDBYCAR,
        PED_PIECE_TORSO,
        false
    };
    CEventDamage dmgEvent{
        m_Car,
        CTimer::GetTimeInMS(),
        WEAPON_RAMMEDBYCAR,
        PED_PIECE_TORSO,
        (uint8)CPedGeometryAnalyser::ComputePedHitSide(*ped, *m_Car),
        false,
        ped->bInVehicle
    };

    dmgEvent.ComputeDamageResponseIfAffectsPed(ped, dmgRespCalc, true);
    m_bWillKillPed = dmgEvent.GetDamageResponse().m_bHealthZero;
    if (m_bWillKillPed) {
        ped->GetEventGroup().Add(&dmgEvent);
    }

    ped->bKnockedUpIntoAir = false;
    ped->bIsStanding       = false;
    ped->m_vecMoveSpeed    = CVector{ CVector2D{ m_Car->GetMoveSpeed() } * 0.75f };

    m_Car->ApplyMoveForce((ped->GetPosition() - m_Car->GetPosition()).Normalized() * (std::min(m_Car->m_fMass / 1400.f, 1.f) * -60.f)); // TODO: 1400.f? I've seen it used a few times...

    return true;
}
