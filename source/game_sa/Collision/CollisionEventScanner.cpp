#include "StdInc.h"

#include "EventBuildingCollision.h"
#include "CollisionEventScanner.h"
#include "PedClothesDesc.h"

void CCollisionEventScanner::ScanForCollisionEvents(CPed* ped, CEventGroup* eg) {
    if (!ped->m_pDamageEntity || ped->m_fDamageIntensity == 0.f) {
        m_bAlreadyHitByCar = false;
        return;
    }

    const auto pedMoveState = ped->GetIntelligence()->GetMoveStateFromGoToTask();

    switch (ped->m_pDamageEntity->GetType()) {
    case eEntityType::ENTITY_TYPE_BUILDING: { // 0x605127
        const auto o = ped->m_pDamageEntity->AsBuilding();
        eg->Add(CEventBuildingCollision{
            ped->m_nPieceType,
            ped->m_fDamageIntensity,
            o,
            ped->m_vecLastCollisionImpactVelocity,
            ped->m_vecLastCollisionPosn,
            pedMoveState
        });
        break;
    }
    case eEntityType::ENTITY_TYPE_VEHICLE: { // 0x6045A1
        const auto v = ped->m_pDamageEntity->AsVehicle();
        if (m_bAlreadyHitByCar) {
            break;
        }
        const auto vehMoveSpdSq = v->GetMoveSpeed().SquaredMagnitude();
        if (v->IsSubTrain() && v->physicalFlags.bDisableCollisionForce && ped->m_bIsStuck && ped->bIsStanding && !ped->m_standingOnEntity && vehMoveSpdSq >= sq(0.01f)) { // 0x604613
            ped->KillPedWithCar(v, 15.f, false);
        }

        const auto DoEventDamage = [&]{
            // TODO/NOTE: Maybe add to event group? [Though, the function calls below do have a side-effect]
            CEventDamage{
                    v,
                    CTimer::GetTimeInMS(),
                    WEAPON_RAMMEDBYCAR,
                    PED_PIECE_TORSO,
                    (uint8)CPedGeometryAnalyser::ComputePedHitSide(*ped, *v),
                    false,
                    ped->bInVehicle
            }.ComputeDamageResponseIfAffectsPed(
                ped,
                CPedDamageResponseCalculator{
                    v,
                    ped->m_fDamageIntensity,
                    WEAPON_RAMMEDBYCAR,
                    PED_PIECE_TORSO,
                    false
                },
                false
            );
        };

        if (vehMoveSpdSq >= sq(0.05f)) { // 0x604631
            auto dmgIntensity = ped->m_fDamageIntensity;
            if (ped->bIsStanding) { // 0x604650
                const auto shiftDotColDir = CVector2D{ ped->m_vecLastCollisionImpactVelocity }.Dot(ped->m_vecAnimMovingShift);
                if (shiftDotColDir < 0.f) { // Opposite directions
                    dmgIntensity = std::max(0.f, shiftDotColDir * ped->m_fMass + dmgIntensity);
                }
            }

            if (ped->IsPlayer()) { // 0x6046A9
                dmgIntensity = std::min(20.f, dmgIntensity);

                const auto vehBB         = &v->GetColModel()->GetBoundingBox();
                const auto vehBBCenterOS = vehBB->GetCenter();
                const auto colHeading    = v->GetHeading() - (v->GetMatrix() * vehBBCenterOS - ped->GetPosition()).Heading();
                const auto vehBBHeading  = std::atan2(vehBB->m_vecMax.x - vehBB->m_vecMin.x, vehBB->m_vecMax.y - vehBB->m_vecMin.y); // NOTE: Pretty sure they fucked this up, missing `-` before the 1st arg
                const auto angle         = std::abs(colHeading) < vehBBHeading || std::abs(colHeading) > PI - vehBBHeading
                    ? (ped->GetPosition() - v->GetPosition()).Normalized().Dot(v->GetMoveSpeed()) // 0x6048DF
                    : std::copysign(v->GetRight().Dot(v->GetMoveSpeed()), -colHeading);
                if (angle > 0.1f) { // 0x60491B
                    ped->KillPedWithCar(v, dmgIntensity, false);
                }
            } else if (!ped->m_pAttachedTo || ped->m_pAttachedTo->GetType() != ENTITY_TYPE_VEHICLE) {
                ped->KillPedWithCar(v, dmgIntensity, false); // 0x604A0D
            } else {
                DoEventDamage(); // 0x604944
            }
        } else if (ped->m_pAttachedTo && ped->m_pAttachedTo->IsVehicle()) { // 0x604A34
            DoEventDamage();
        } else if (!ped->IsPlayer()) { // 0x604AE1
            eg->Add(CEventVehicleCollision{
                ped->m_nPieceType,
                ped->m_fDamageIntensity,
                v,
                ped->m_vecLastCollisionImpactVelocity,
                ped->m_vecLastCollisionPosn,
                ped->GetIntelligence()->GetMoveStateFromGoToTask(),
                0
            });  
        }
        break;
    }
    case ENTITY_TYPE_PED: { // 0x604B63 

        break;
    }
    }
    // 0x60515F
    if (const auto pd = ped->m_pPlayerData) {
        const auto dmge = ped->m_pDamageEntity;
        if (   notsa::contains({ ENTITY_TYPE_VEHICLE, ENTITY_TYPE_OBJECT }, dmge->GetType())
            && pd->m_pPedClothesDesc->GetIsWearingBalaclava()
        ) {
            const auto DoEventSoundQuiet = [&](float soundLevel, uint32& lastTime, uint32 interval) {
                const auto now = CTimer::GetTimeInMS();
                if (now > lastTime + interval) {
                    GetEventGlobalGroup()->Add(CEventSoundQuiet{ ped, soundLevel, (uint32)-1, {} });
                    lastTime = CTimer::GetTimeInMS();
                }
            };

            auto& nLastBumpSoundEvent = StaticRef<uint32>(0xC0B1B0);
            auto& nLastDoorSoundEvent = StaticRef<uint32>(0xC0B1B4);

            if (dmge->IsPhysical() && !dmge->AsPhysical()->physicalFlags.bDisableMoveForce) {
                if (std::abs(dmge->AsPhysical()->GetTurnSpeed().z) > 0.04f) {
                    DoEventSoundQuiet(40.f, nLastDoorSoundEvent, 2000);
                }
            } else if (ped->m_fDamageIntensity > 1.f) {
                DoEventSoundQuiet(ped->m_fDamageIntensity <= 3.f ? 30.f : 40.f, nLastBumpSoundEvent, 1000);
            }
        }
    }
    m_bAlreadyHitByCar = false;
}

void CCollisionEventScanner::InjectHooks() {
    RH_ScopedClass(CCollisionEventScanner);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(ScanForCollisionEvents, 0x604500, { .reversed = false });
}
