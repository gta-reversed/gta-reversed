#include "StdInc.h"

#include "TaskComplexPolicePursuit.h"
#include "TaskComplexArrestPed.h"
#include "TaskComplexSeekEntity.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleScratchHead.h"

void CTaskComplexPolicePursuit::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPolicePursuit, 0x8709d4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x68BA70);
    RH_ScopedInstall(Destructor, 0x68D880);

    RH_ScopedInstall(SetWeapon, 0x68BAD0);
    RH_ScopedInstall(ClearPursuit, 0x68BD90);
    RH_ScopedInstall(SetPursuit, 0x68BBD0);
    RH_ScopedInstall(PersistPursuit, 0x68BDC0);
    RH_ScopedInstall(CreateSubTask, 0x68D910);
    RH_ScopedVMTInstall(Clone, 0x68CDD0);
    RH_ScopedVMTInstall(GetTaskType, 0x68BAA0);
    RH_ScopedVMTInstall(MakeAbortable, 0x68BAB0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x68BAC0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6908E0);
    RH_ScopedVMTInstall(ControlSubTask, 0x690920);
}


CTaskComplexPolicePursuit::CTaskComplexPolicePursuit(const CTaskComplexPolicePursuit& o) :
    CTaskComplexPolicePursuit{}
{
}

// 0x68D880
CTaskComplexPolicePursuit::~CTaskComplexPolicePursuit() {
    if (m_Pursuer) {
        ClearPursuit(m_Pursuer);
        if (notsa::IsFixBugs()) {
            CEntity::SafeCleanUpRef(m_Pursuer);
        }
    }
    CEntity::SafeCleanUpRef(m_Persecuted);
}

// 0x68BAD0
// Make sure cop has a weapon on them
void CTaskComplexPolicePursuit::SetWeapon(CPed* ped) { // `ped` is the pursuer
    const auto wantedLevel = FindPlayerWanted()->GetWantedLevel();

    // At level 0 we dont do anything (I don't think this is possible anyways)
    if (wantedLevel == 0) {
        return;
    }

    if (wantedLevel > 1) {
        if (ped->GetActiveWeapon().GetType() != WEAPON_UNARMED) { // Already has a weapon
            return;
        }
    } else /*wantedLevel == 1*/ {
        const auto player = FindPlayerPed();
        if (player->m_standingOnEntity || ped->m_nPedState == PEDSTATE_ARREST_PLAYER) {
            return;
        }
        if (ped->DoWeHaveWeaponAvailable(WEAPON_NIGHTSTICK)) {
            switch (player->GetActiveWeapon().GetWeaponInfo(player).m_nWeaponFire) {
            case WEAPON_FIRE_INSTANT_HIT:
            case WEAPON_FIRE_PROJECTILE:
                break;
            default:
                ped->SetCurrentWeapon(WEAPON_NIGHTSTICK);
                return;
            }
        }
    }

    ped->SetCurrentWeapon(ped->DoWeHaveWeaponAvailable(WEAPON_SHOTGUN) ? WEAPON_SHOTGUN : WEAPON_PISTOL);
}

// 0x68BD90
void CTaskComplexPolicePursuit::ClearPursuit(CCopPed* pursuer) {
    if (FindPlayerPed()) {
        FindPlayerWanted()->RemovePursuitCop(pursuer);
    }
}

// 0x68BBD0
bool CTaskComplexPolicePursuit::SetPursuit(CPed* ped) {
    // Find closest player
    float minDistSq = FLT_MAX;
    CPlayerPed* closestPlayer{};
    for (const auto& v : CWorld::Players) {
        const auto plyr = v.m_pPed;
        if (!plyr) {
            continue;
        }
        const auto distSq = (plyr->GetPosition() - ped->GetPosition()).SquaredMagnitude();
        if (distSq >= minDistSq) {
            continue;
        }
        if (plyr->bInVehicle) {
            if (distSq * plyr->m_pVehicle->GetMoveSpeed().SquaredMagnitude() >= sq(4.f)) { // TODO/BUG: Why `*`?
                continue;
            }
        }
        closestPlayer = plyr;
        minDistSq     = distSq;
    }
    CEntity::ChangeEntityReference(m_Persecuted, closestPlayer);
    return closestPlayer && FindPlayerWanted()->SetPursuitCop(ped->AsCop());
}

// 0x68BDC0
bool CTaskComplexPolicePursuit::PersistPursuit(CCopPed* pursuer) {
    const auto wanted = FindPlayerWanted();

    if (pursuer->m_fHealth <= 0.f) { // 0x68BDD0
        ClearPursuit(pursuer);
    } else if (CCullZones::NoPolice() && !m_bRoadBlockCop) { // 0x68BDF1
        if (pursuer->bHitSomethingLastFrame) { // 0x68BE01
            m_bPlayerInCullZone = m_bRoadBlockCop = true;
            ClearPursuit(pursuer);
        }
    } else if (!CCullZones::NoPolice() && m_bPlayerInCullZone) { // 0x68BE16
        m_bPlayerInCullZone = m_bRoadBlockCop = false;
        ClearPursuit(pursuer);
    } else if (wanted->GetWantedLevel() == 0) { // 0x68BE43
        if (m_bRoadBlockCop && !m_bPlayerInCullZone) {
            m_bPlayerInCullZone = m_bRoadBlockCop = false;
            ClearPursuit(pursuer);
        }
    }

    wanted->RemoveExcessPursuitCops(); // 0x68BE5D
    return wanted->IsInPursuit(pursuer);
}

// 0x68D910
CTask* CTaskComplexPolicePursuit::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_ARREST_PED:
        return new CTaskComplexArrestPed{m_Persecuted};
    case TASK_COMPLEX_SEEK_ENTITY:
        return new CTaskComplexSeekEntity<CEntitySeekPosCalculatorStandard>{
            ped->m_pVehicle,
            50'000,
            1'000,
            ped->m_pVehicle->GetColModel()->GetBoundRadius() + 1.f,
            2.f,
            2.f,
            true,
            true
        };
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{};
    case TASK_SIMPLE_SCRATCH_HEAD:
        return new CTaskSimpleScratchHead{};
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE("Invalid TaskType({})", taskType);
    }
}

// 0x6908E0
CTask* CTaskComplexPolicePursuit::CreateFirstSubTask(CPed* ped) {
    if (SetPursuit(ped->AsCop())) {
        return CreateSubTask(TASK_COMPLEX_ARREST_PED, ped);
    }
    m_bCouldJoinPursuit = false;
    return CreateSubTask(TASK_FINISHED, ped);
}

// 0x690920
CTask* CTaskComplexPolicePursuit::ControlSubTask(CPed* ped) {
    const auto nextSubTaskType = GetNextSubTaskType(ped->AsCop());

    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_ARREST_PED || nextSubTaskType == TASK_COMPLEX_ARREST_PED) { // 0x690A3D
        SetWeapon(ped);
    }

    if (nextSubTaskType == TASK_NONE || !m_pSubTask->MakeAbortable(ped)) { // 0x690A56
        return m_pSubTask;
    }

    if (nextSubTaskType != TASK_COMPLEX_ENTER_CAR_AS_DRIVER) { // 0x690A6C
        return CreateSubTask(nextSubTaskType, ped);
    }

    ped->GetEventGroup().Add(CEventVehicleToSteal{ ped->m_pVehicle });
    return new CTaskSimpleScratchHead{};
}

// 0x690956 (not a function originally)
eTaskType CTaskComplexPolicePursuit::GetNextSubTaskType(CCopPed* pursuer) { // ped is the pursuer
    const auto plyrWanted = FindPlayerWanted();

    if (PersistPursuit(pursuer)) { // 0x690956
        return TASK_NONE;
    }

    if (m_bRoadBlockCop && (!pursuer->bStayInSamePlace || pursuer->GetActiveWeapon().GetType() != WEAPON_PISTOL)) { // 0x690991 (Inverted)
        pursuer->SetCurrentWeapon(WEAPON_PISTOL);
        pursuer->bStayInSamePlace = true;
        return TASK_COMPLEX_ARREST_PED;
    }

    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_ARREST_PED) { // 0x6909BB
        if (pursuer->bInVehicle) {
            return TASK_FINISHED;
        }

        if (!pursuer->m_pVehicle) {
            return TASK_SIMPLE_STAND_STILL;
        }

        if (plyrWanted->GetWantedLevel() == 0) { // 0x6909D7
            return TASK_COMPLEX_ENTER_CAR_AS_DRIVER;
        }

        if ((pursuer->m_pVehicle->GetPosition() - pursuer->GetPosition()).SquaredMagnitude() <= sq(5.f)) {
            return TASK_COMPLEX_SEEK_ENTITY;
        }
    }

    return TASK_NONE;
}
