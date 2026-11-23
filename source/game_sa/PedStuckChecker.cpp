#include "StdInc.h"

#include "PedStuckChecker.h"
#include "Events/EventInWater.h"
#include "Events/EventStuckInAir.h"

void CPedStuckChecker::InjectHooks() {
    RH_ScopedClass(CPedStuckChecker);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(TestPedStuck, 0x602C00);
}

// 0x602C00
bool CPedStuckChecker::TestPedStuck(CPed* ped, CEventGroup* eventGroup) {
    const auto Reset = [this] {
        m_FramesInAir = 0;
        m_state       = PED_STUCK_STATE_NONE;
    };
    const auto timeStep = std::max(CTimer::GetTimeStep(), 0.01f);
    const auto WasPushedByAPed = [ped] { // Don't move peds that are being pushed by other peds
        return ped->m_fDamageIntensity > 0.f
            && ped->m_pDamageEntity
            && ped->m_pDamageEntity->GetIsTypePed()
            && ped->m_vecLastCollisionImpactVelocity.z > 0.3f;
    };

    if (!ped->GetUsesCollision() || ped->m_pAttachedTo || ped->m_nPedState == PEDSTATE_DIE || ped->m_nPedState == PEDSTATE_DEAD) {
        Reset();
        return false;
    }

    const auto intel = ped->GetIntelligence();
    if (ped->bIsStanding
        || ped->bWasStanding
        || !((ped->m_pDamageEntity && ped->m_fDamageIntensity > 0.f) || (ped->IsPlayer() && ped->m_bIsStuck))
        || intel->GetTaskSwim()
        || intel->GetTaskJetPack()
        || intel->GetTaskClimb()) {
        Reset();
        return false;
    }

    CVector moved{};
    if (m_FramesInAir != 0 && m_FramesInAir <= 10'000) {
        moved = ped->GetPosition() - m_lastNonStuckPoint;
        m_FramesInAir++;
    } else {
        m_FramesInAir       = 1;
        m_state             = PED_STUCK_STATE_NONE;
        m_lastNonStuckPoint = ped->GetPosition();
    }
    // Has been in the air for long, but hardly moved => stuck on something
    const auto IsStuckOnTheSpot = [&] {
        return (float)m_FramesInAir > 50.f / (timeStep * 4.f) && moved.Magnitude() < (float)m_FramesInAir * 0.01f;
    };
    // Has been in the air for long, but hardly moved vertically
    const auto HasntFallen = [&] {
        return (float)m_FramesInAir > 50.f / (timeStep + timeStep) && std::abs(moved.z) < (float)m_FramesInAir * 0.004f;
    };

    CEntity* hitEntity{};
    if (IsStuckOnTheSpot() || (ped->m_nNumEntitiesCollided > 1 && HasntFallen())) { // 0x602E40
        if (((uint8)ped->m_nRandomSeed + CTimer::GetFrameCounter() + 3) % 8) {      // Try to put it on the ground every 8th frame only
            m_state = PED_STUCK_STATE_STUCK;
        } else {
            CVector origin = ped->GetPosition();
            origin.z += 1.f;
            CColPoint cp{};
            if (CWorld::ProcessVerticalLine(origin, ped->GetPosition().z - 1.f, cp, hitEntity, true, true, false, true)) {
                if ((!ped->bHeadStuckInCollision || cp.m_vecPoint.z + 1.f < ped->GetPosition().z) && !WasPushedByAPed()) {
                    origin.z = cp.m_vecPoint.z + 1.f;
                    ped->SetPosn(origin);
                    ped->bHeadStuckInCollision = false;
                }
                ped->bIsStanding = true;
                Reset();
            }
        }
    } else if (ped->m_nNumEntitiesCollided == 1 && ped->m_apCollidedEntities[0] && ped->m_apCollidedEntities[0]->GetIsTypeBuilding() && HasntFallen()) { // 0x603048
        // Leaning against a building: find ground to the left/right of the direction the ped moved in, push it there
        const auto dir = CVector{ -moved.y, moved.x, 1.f }.Normalized();

        const auto GetGroundZ = [&](const CVector& at, CColPoint& cp, float noGroundZ) {
            if (CWorld::ProcessVerticalLine(at, -20.f, cp, hitEntity, true)
                && CWorld::GetIsLineOfSightClear(ped->GetPosition(), at, true, true, false, true)) {
                return cp.m_vecPoint.z;
            }
            return noGroundZ;
        };
        CColPoint  cpA{}, cpB{};
        const auto zA = GetGroundZ(ped->GetPosition() + dir, cpA, 5001.f);
        const auto zB = GetGroundZ(ped->GetPosition() - dir, cpB, 5002.f);

        int16 side    = 0;
        if (zA > 5000.f && zB > 5000.f) {
            side = -1;
        } else {
            const auto belowFeet = ped->GetPosition().z - 1.f;
            if (zA > belowFeet && zA < 5000.f && (zB < 5001.f || zA < zB)) {
                side = 1;
            } else if (zB > belowFeet && zB < 5001.f) {
                side = 2;
            }
            if (side > 0 && !ped->bHeadStuckInCollision && !WasPushedByAPed()) {
                ped->SetPosn(side == 2 ? cpB.m_vecPoint : cpA.m_vecPoint);
                ped->GetPosition().z += 1.f;
            }
        }

        const auto pushDir = CVector{ CVector2D{ zA < zB ? -dir : dir }, 1.f };
        ped->ApplyMoveForce(CVector{ pushDir.x * 4.f, pushDir.y * 4.f, 4.f });
        if (side != -1) {
            ped->SetPosn(ped->GetPosition() + pushDir * 0.25f);
            const auto heading      = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(pushDir.x, pushDir.y, 0.f, 0.f));
            ped->m_fCurrentRotation = ped->m_fAimingRotation = heading;
            ped->SetOrientation(0.f, 0.f, heading);
        }
        m_state = PED_STUCK_STATE_WAS_STUCK;
    }

    if (m_state == PED_STUCK_STATE_NONE) {
        return false;
    }
    auto* const inWater = static_cast<CEventInWater*>(eventGroup->GetEventOfType(EVENT_IN_WATER));
    if (!inWater || inWater->GetAcceleration() <= 1.f) {
        eventGroup->Add(CEventStuckInAir{ ped });
        return true;
    }
    ped->bIsStanding = false;
    return false;
}
