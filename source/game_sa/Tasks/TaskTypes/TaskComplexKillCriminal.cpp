#include "StdInc.h"
#include "TaskComplexKillCriminal.h"

void CTaskComplexKillCriminal::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillCriminal, 0x870a00, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x68BE70);
    RH_ScopedInstall(Destructor, 0x68BF30);

    RH_ScopedInstall(CreateSubTask, 0x68C050, {.reversed = false});
    RH_ScopedInstall(FindNextCriminalToKill, 0x68C3C0, {.reversed = false});
    RH_ScopedInstall(ChangeTarget, 0x68C6E0, {.reversed = false});

    RH_ScopedVMTInstall(Clone, 0x68CE50);
    RH_ScopedVMTInstall(GetTaskType, 0x68BF20);
    RH_ScopedVMTInstall(MakeAbortable, 0x68DAD0, {.reversed = false});
    RH_ScopedVMTInstall(CreateNextSubTask, 0x68E4F0, {.reversed = false});
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x68DC60, {.reversed = false});
    RH_ScopedVMTInstall(ControlSubTask, 0x68E950, {.reversed = false});
}

// 0x68BE70
CTaskComplexKillCriminal::CTaskComplexKillCriminal(CPed* criminal, bool randomize) :
    m_Criminal{criminal},
    m_Randomize{randomize}
{
    assert(m_Criminal);

    if (m_Criminal) {
        if (m_Criminal->IsPlayer() || notsa::contains({ PED_TYPE_COP, PED_TYPE_MEDIC, PED_TYPE_FIREMAN, PED_TYPE_MISSION1 }, m_Criminal->m_nPedType) || m_Criminal->IsCreatedByMission()) {
            m_Criminal = nullptr; // Interesting solution, but okay
            DEV_LOG("Ped can't be a criminal, aborting");
        } else {
            CEntity::SafeRegisterRef(m_Criminal);
        }
    }
}

// 0x68CE50
CTaskComplexKillCriminal::CTaskComplexKillCriminal(const CTaskComplexKillCriminal& o) :
    CTaskComplexKillCriminal{
        o.m_Criminal,
        notsa::IsFixBugs() ? o.m_Randomize : false
    }
{
}

// 0x68BF30
CTaskComplexKillCriminal::~CTaskComplexKillCriminal() {
    CEntity::SafeCleanUpRef(m_Criminal);

    if (m_Cop) {
        CEntity::SafeCleanUpRef(m_Cop);

        m_Cop->m_nTimeTillWeNeedThisPed = CTimer::GetTimeInMS();
        m_Cop->bCullExtraFarAway = false;
        m_Cop->m_fRemovalDistMultiplier = 1.f;
        if (const auto v = m_Cop->m_pVehicle) {
            v->m_nExtendedRemovalRange = false;
            v->vehicleFlags.bNeverUseSmallerRemovalRange = false;
            if (v->IsDriver(m_Cop)) {
                const auto ap = &v->m_autoPilot;
                ap->SetCarMission(MISSION_CRUISE);
                ap->SetDrivingStyle(DRIVING_STYLE_AVOID_CARS);
                ap->SetCruisingSpeed(10);
                if (v->GetStatus() != STATUS_SIMPLE) {
                    CCarCtrl::JoinCarWithRoadSystem(v);
                }
                v->vehicleFlags.bSirenOrAlarm = false;
            }
        }
    }
}

// 0x68C050
CTask* CTaskComplexKillCriminal::CreateSubTask(eTaskType tt, CPed* ped, bool force) {
    return plugin::CallMethodAndReturn<CTask*, 0x68C050, CTaskComplexKillCriminal*, eTaskType, CPed*, bool>(this, tt, ped, force);
}

// 0x68C3C0
CPed* CTaskComplexKillCriminal::FindNextCriminalToKill(CPed* ped, bool any) {
    return plugin::CallMethodAndReturn<CPed*, 0x68C3C0, CTaskComplexKillCriminal*, CPed*, bool>(this, ped, any);
}

// 0x68C6E0
int8 CTaskComplexKillCriminal::ChangeTarget(CPed* newTarget) {
    return plugin::CallMethodAndReturn<int8, 0x68C6E0, CTaskComplexKillCriminal*, CPed*>(this, newTarget);
}

// 0x68DAD0
bool CTaskComplexKillCriminal::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return plugin::CallMethodAndReturn<bool, 0x68DAD0, CTaskComplexKillCriminal*, CPed*, eAbortPriority, CEvent const*>(this, ped, priority, event);
}

// 0x68E4F0
CTask* CTaskComplexKillCriminal::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x68E4F0, CTaskComplexKillCriminal*, CPed*>(this, ped);
}

// 0x68DC60
CTask* CTaskComplexKillCriminal::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x68DC60, CTaskComplexKillCriminal*, CPed*>(this, ped);
}

// 0x68E950
CTask* CTaskComplexKillCriminal::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x68E950, CTaskComplexKillCriminal*, CPed*>(this, ped);
}
