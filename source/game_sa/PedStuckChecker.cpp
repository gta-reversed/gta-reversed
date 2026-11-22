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
// NOTE: `m_radius` is really the number of frames the ped has been checked for (in the air) since `m_lastNonStuckPoint` was stored.
bool CPedStuckChecker::TestPedStuck(CPed* ped, CEventGroup* eventGroup) {
    const auto Reset = [this] {
        m_radius = 0;
        m_state  = PED_STUCK_STATE_NONE;
    };
    const auto TimeStep = [] {
        return std::max(CTimer::GetTimeStep(), 0.01f);
    };
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
    if (m_radius != 0 && m_radius <= 10'000) {
        moved = ped->GetPosition() - m_lastNonStuckPoint;
        m_radius++;
    } else {
        m_radius            = 1;
        m_state             = PED_STUCK_STATE_NONE;
        m_lastNonStuckPoint = ped->GetPosition();
    }
    const auto frames = (float)m_radius;

    // Has been in the air for long, but hardly moved => stuck on something
    const auto IsStuckOnTheSpot = [&] {
        return frames > 50.f / (TimeStep() * 4.f) && moved.Magnitude() < frames * 0.01f;
    };
    // Has been in the air for long, but hardly moved vertically
    const auto HasntFallen = [&] {
        return frames > 50.f / (TimeStep() + TimeStep()) && std::abs(moved.z) < frames * 0.004f;
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
        CVector dir{ -moved.y, moved.x, 1.f };
        dir.Normalise();

        const auto GroundZ = [&](const CVector& at, CColPoint& cp, float noGroundZ) {
            if (CWorld::ProcessVerticalLine(at, -20.f, cp, hitEntity, true)
                && CWorld::GetIsLineOfSightClear(ped->GetPosition(), at, true, true, false, true)) {
                return cp.m_vecPoint.z;
            }
            return noGroundZ;
        };
        CColPoint  cpA{}, cpB{};
        const auto zA = GroundZ(ped->GetPosition() + dir, cpA, 5001.f);
        const auto zB = GroundZ(ped->GetPosition() - dir, cpB, 5002.f);

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

        if (zA < zB) {
            dir = -dir;
        }
        dir.z = 1.f;
        ped->ApplyMoveForce(CVector{ dir.x * 4.f, dir.y * 4.f, 4.f });
        if (side != -1) {
            ped->SetPosn(ped->GetPosition() + dir * 0.25f);
            const auto heading      = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.f, 0.f));
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
        CEventStuckInAir e{ ped };
        eventGroup->Add(&e);
        return true;
    }
    ped->bIsStanding = false;
    return false;
}
