#include "StdInc.h"

#include "TaskSimpleFightingControl.h"
#include "PedStats.h"
#include "TaskSimpleFight.h"

#include "EventObjectCollision.h"
#include "EventBuildingCollision.h"

constexpr auto FIGHT_CTRL_MAX_ATTACK_ANGLE_RAD = DegreesToRadians(15);
constexpr auto FIGHT_CTRL_FIGHT_IDLE_TIME      = 60000.f; // 0x8D2E50
constexpr auto FIGHT_CTRL_ATTACK_DELAY_MS      = 2'000u;  // 0x8D2E4C

void CTaskSimpleFightingControl::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleFightingControl, 0x86d700, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x61DC10);
    RH_ScopedInstall(Destructor, 0x61DCA0);

    RH_ScopedInstall(CalcMoveCommand, 0x624B50);

    RH_ScopedVMTInstall(Clone, 0x622EB0);
    RH_ScopedVMTInstall(GetTaskType, 0x61DC90);
    RH_ScopedVMTInstall(MakeAbortable, 0x61DD00);
    RH_ScopedVMTInstall(ProcessPed, 0x62A0A0);
}

// 0x61DC10
CTaskSimpleFightingControl::CTaskSimpleFightingControl(CEntity* entityToFight, float angleRad, float attackRange) :
    m_target{entityToFight},
    m_angleRad{angleRad},
    m_maxAttackRange{ attackRange }
{
    CEntity::SafeRegisterRef(m_target);
}

// NOTSA
CTaskSimpleFightingControl::CTaskSimpleFightingControl(const CTaskSimpleFightingControl& o) :
    CTaskSimpleFightingControl{o.m_target, o.m_angleRad, o.m_maxAttackRange }
{
}

// 0x61DCA0
CTaskSimpleFightingControl::~CTaskSimpleFightingControl() {
    CEntity::SafeCleanUpRef(m_target);
}

// 0x624B50
int16 CTaskSimpleFightingControl::CalcMoveCommand(CPed* ped) {
    if (!m_target) {
        return -1;
    }

    const auto& targetPos = m_target->GetPosition();
    const auto& pedPos    = ped->GetPosition();

    const auto segmentPedToTarget = targetPos - pedPos;

    // Check if target is outside of the attacking angle
    if (std::abs(CGeneral::LimitRadianAngle(segmentPedToTarget.Heading()) - ped->m_fCurrentRotation) >= FIGHT_CTRL_MAX_ATTACK_ANGLE_RAD) {
        m_nextAttackTime += (uint32)CTimer::GetTimeStepInMS();
        return 0;
    }

    switch (m_target->GetType()) {
    case ENTITY_TYPE_PED: {
        const auto range = segmentPedToTarget.Magnitude() - m_maxAttackRange;
        if (range > 0.1f) {
            return 3;
        }

        const auto rnd = rand();

        if (range > -0.1f) { // Inverted
            return (rnd % 16 == 0) ? 7 : -1;
        }

        if (range >= 0.8f) {
            if (rand() % 64 == 0) {
                return 8;
            }
            if (rand() % 64 == 0) {
                return 10;
            }
        } else if (rnd % 16 == 0) {
            return 9;
        }
     
        break;
    }
    case ENTITY_TYPE_VEHICLE: {
        std::array<CVector, 4> planeNormals{};
        std::array<float, 4>   planeDs{};
        CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(targetPos.z, *m_target, planeNormals, planeDs);

        const auto hitSide = CPedGeometryAnalyser::ComputeEntityHitSide(pedPos, *m_target);
        if ((DotProduct(planeNormals[+hitSide], pedPos) + planeDs[+hitSide]) >= 0.7f) {
            return 3;
        }

        break;
    }
    default: {
        if (segmentPedToTarget.Magnitude() - m_maxAttackRange > 0.3f) {
            return 3;
        }
        break;
    }
    }

    return -1;
}

// 0x61DD00
bool CTaskSimpleFightingControl::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_LEISURE:
        return false;
    case ABORT_PRIORITY_IMMEDIATE:
        return true;
    case ABORT_PRIORITY_URGENT: {
        if (event) {
            if (event->GetEventPriority() < 22) {
                return false;
            }

            switch (event->GetEventType()) {
            case EVENT_PED_COLLISION_WITH_PED:
            case EVENT_PED_COLLISION_WITH_PLAYER:
            case EVENT_PLAYER_COLLISION_WITH_PED: {
                return false;
            }
            case EVENT_VEHICLE_COLLISION:
            case EVENT_OBJECT_COLLISION:
            case EVENT_BUILDING_COLLISION: {
                if (!m_target) {
                    return true;
                }

                // This is not how they did it, but I'd like to avoid using blackmagic.
                // Once we reverse all stuff related to these events we should make a common base class
                // like `CEventEntityCollision` or something similar.
                const auto impactNormal = [event] {
                    switch (event->GetEventType()) {
                    case EVENT_VEHICLE_COLLISION:
                        return static_cast<const CEventVehicleCollision*>(event)->m_impactNormal;
                    case EVENT_OBJECT_COLLISION:
                        return static_cast<const CEventObjectCollision*>(event)->m_impactNormal;
                    case EVENT_BUILDING_COLLISION:
                        return static_cast<const CEventBuildingCollision*>(event)->m_impactNormal;
                    default:
                        NOTSA_UNREACHABLE();
                    }
                }();

                // Check if vector are opposing each other with 45deg tolerance
                if (-DotProduct((m_target->GetPosition() - ped->GetPosition()).Normalized(), impactNormal) < SQRT_2 / 2) {
                    return false; // They aren't
                }
            }
            }
        }

        if (ped->GetIntelligence()->GetTaskFighting()) {
            return ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, priority, event);
        }

        return true;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x62A0A0
bool CTaskSimpleFightingControl::ProcessPed(CPed* ped) {
    if (!m_target || m_bool) {
        return true;
    }

    // NOTE: The original reads the stat's `m_flags` (+0x30) here
    const auto shootingRate = !ped->IsCreatedBy(PED_MISSION) && !ped->IsCop() && ped->m_nWeaponShootingRate == 40
        ? (float)ped->m_pStats->m_flags
        : (float)ped->m_nWeaponShootingRate;

    ped->GiveWeaponAtStartOfFight();

    auto& tm      = ped->GetTaskManager();
    int32 command = 0;
    if (!tm.GetTaskSecondary(TASK_SECONDARY_ATTACK)) { // 0x62A146
        tm.SetTaskSecondary(new CTaskSimpleFight{ m_target, false, (uint32)FIGHT_CTRL_FIGHT_IDLE_TIME }, TASK_SECONDARY_ATTACK);
        m_nextAttackTime = 0;
    } else if (!notsa::isa<CTaskSimpleFight>(tm.GetTaskSecondary(TASK_SECONDARY_ATTACK))) { // 0x62A1B7
        tm.GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
        return false;
    } else if (m_nextAttackTime > CTimer::GetTimeInMS()) { // 0x62A21D
        if (m_target->GetIsTypePed() && !m_someTime && CGeneral::GetRandomNumberInRange(0.f, 100.f) < 2.f * shootingRate * 0.025f) {
            m_someTime = CGeneral::GetRandomNumberInRange(500, 2'000, true);
            command    = 2;
        } else if (m_someTime) { // 0x62A270
            const auto stepMs = (uint32)CTimer::GetTimeStepInMS();
            m_someTime        = m_someTime > stepMs ? m_someTime - stepMs : 0;
            command           = 2;
        }
    } else { // 0x62A1E1
        m_nextAttackTime = 0;
        command          = ped->m_nFightingStyle != STYLE_STANDARD && ped->GetActiveWeapon().GetType() == WEAPON_UNARMED ? 12 : 11;
    }

    auto* const tFight = notsa::cast<CTaskSimpleFight>(tm.GetTaskSecondary(TASK_SECONDARY_ATTACK));
    m_maxAttackRange   = tFight->GetComboData().m_fRanges;
    if (!m_nextAttackTime) { // 0x62A2DC
        const auto delay = CGeneral::GetRandomNumberInRange(0.25f, 1.25f) / (shootingRate * m_unk3 * 0.025f * 0.7f + 0.3f) * (float)FIGHT_CTRL_ATTACK_DELAY_MS;
        m_nextAttackTime = CTimer::GetTimeInMS() + (uint32)delay;
    }
    if (tFight->m_nComboSet <= MELEE_COMBO_MOVE) { // 0x62A33D
        if (const auto moveCommand = CalcMoveCommand(ped); moveCommand > -1) {
            command = moveCommand;
        }
    }
    tFight->ControlFight(m_target, (uint8)command);
    return false;
}
