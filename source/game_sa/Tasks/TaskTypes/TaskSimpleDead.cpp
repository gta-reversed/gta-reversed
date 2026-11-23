#include "StdInc.h"

#include "TaskSimpleDead.h"
#include "CarEnterExit.h"
#include "AccidentManager.h"
#include "Shadows.h"
#include "WaterLevel.h"
#include "Localisation.h"
#include "WeaponInfo.h"
#include "Events/EventDeadPed.h"
#include "Events/EventGlobalGroup.h"

// NOTSA: *deathTime* originally int32
// 0x630590
CTaskSimpleDead::CTaskSimpleDead(uint32 deathTime, bool hasDrowned) :
    m_TimeOfDeathMs{ deathTime },
    m_bHasDrowned{ hasDrowned } {
}

// 0x636100
CTaskSimpleDead::CTaskSimpleDead(const CTaskSimpleDead& o) :
    CTaskSimpleDead{ o.m_TimeOfDeathMs, o.m_bHasDrowned } {
}

// 0x630600
bool CTaskSimpleDead::ProcessPed(CPed* ped) {
    const auto isTargetedByPlayer = FindPlayerPed(0)->m_pTargetedObject == ped
        || (FindPlayerPed(1) && FindPlayerPed(1)->m_pTargetedObject == ped);

    const auto wasFirstTime = m_bFirstTime;

    if (m_bFirstTime) {
        m_bFirstTime = false;

        if (ped->bInVehicle) { // Slump in the seat
            const auto door = CCarEnterExit::ComputeTargetDoorToExit(ped->m_pVehicle, ped);
            CAnimManager::BlendAnimation(
                ped->GetRpClump(),
                ANIM_GROUP_DEFAULT,
                door == TARGET_DOOR_DRIVER || door == TARGET_DOOR_REAR_LEFT
                    ? ANIM_ID_CAR_DEAD_LHS
                    : ANIM_ID_CAR_DEAD_RHS,
                4.f
            );
        }

        ped->SetPedState(PEDSTATE_DEAD);

        if (!m_bHasDrowned && !ped->m_standingOnEntity && !isTargetedByPlayer) {
            ped->m_bUsesCollision = false;
        }

        ped->m_fHealth = 0.f;
        ped->RemoveWeaponModel(ped->GetActiveWeapon().GetWeaponInfo().m_nModelId1);
        ped->m_nActiveWeaponSlot = 0;

        if (!ped->IsPlayer()) {
            ped->RemoveWeaponAnims(0, -1000.f);
            ped->CreateDeadPedWeaponPickups();
            ped->CreateDeadPedMoney();
        }

        GetEventGlobalGroup()->Add(CEventDeadPed{ ped, m_bHasDrowned, m_TimeOfDeathMs });
        CAccidentManager::GetInstance()->ReportAccident(ped);
    }

    const auto SetPedIKSlope = [&] { // Align the body to the ground
        const auto& mat            = ped->GetMatrix();
        const auto  pitch          = ped->field_578.Dot(mat.GetForward());
        ped->m_pedIK.m_fSlopeRoll  = std::asin(std::clamp(ped->field_578.Dot(mat.GetRight()), -1.f, 1.f));
        ped->m_pedIK.m_fSlopePitch = std::asin(std::clamp(pitch, -1.f, 1.f));
    };

    if (!m_bFirstTime && m_bHasDrowned && ped->bIsStanding) { // Drowned body reached the ground - NOTE: `m_bFirstTime` is always false here
        ped->m_bUsesCollision = false;
        m_bHasDrowned         = false;
        auto* const anim      = CAnimManager::BlendAnimation(
            ped->GetRpClump(),
            ANIM_GROUP_DEFAULT,
            ANIM_ID_FLOOR_HIT_F,
            8.f
        );
        anim->SetFlag(ANIMATION_IS_FINISH_AUTO_REMOVE, false);
        SetPedIKSlope();
    } else if (!ped->m_bUsesCollision || m_bHasDrowned || !ped->bIsStanding || ped->m_standingOnEntity || isTargetedByPlayer) { // NOTE: The original re-checks the players' targets every frame, not just on the first one
        if (wasFirstTime && (ped->bIsStanding || ped->bWasStanding)) {
            SetPedIKSlope();
        }
    } else {
        ped->m_bUsesCollision = false;
        SetPedIKSlope();
    }

    ped->DeadPedMakesTyresBloody();

    if (CLocalisation::Blood() && !m_bHasDrowned) {
        constexpr auto BLOOD_AFTER_MS      = 2'000;
        constexpr auto BLOOD_RADIUS_PER_MS = 0.00015f;
        constexpr auto BLOOD_RADIUS_MAX    = 0.75f;

        const auto deadForMs               = CTimer::GetTimeInMS() - m_TimeOfDeathMs;

        // Peds walking through the blood get bloody feet
        const auto bloodRadius =
            deadForMs < BLOOD_AFTER_MS
            ? 0.f
            : deadForMs <= 7'000
            ? (float)(deadForMs - BLOOD_AFTER_MS) * BLOOD_RADIUS_PER_MS
            : BLOOD_RADIUS_MAX;
        for (auto& nearby : ped->GetIntelligence()->GetPedScanner().GetEntities<CPed>()) {
            if ((nearby.GetPosition() - ped->GetPosition()).SquaredMagnitude() < sq(bloodRadius)) {
                nearby.m_nDeathTimeMS      = 200; // NOTE: Used as the bloody footprint counter
                nearby.bDoBloodyFootprints = true;
            }
        }

        // Blood pool
        if (deadForMs > BLOOD_AFTER_MS && !m_bBloodPuddleCreated) {
            auto       pos      = ped->GetPosition();
            const auto poolTime = deadForMs - BLOOD_AFTER_MS;
            if (poolTime <= CTimer::GetTimeInMS() - CTimer::GetPreviousTimeInMS()) { // First frame of it
                float waterLevel;
                if (CWaterLevel::GetWaterLevelNoWaves(pos, &waterLevel, nullptr, nullptr) && pos.z <= waterLevel) {
                    m_bBloodPuddleCreated = true; // No blood pool in water
                }
            }
            if (!m_bBloodPuddleCreated && CLocalisation::Blood()) {
                if (poolTime < 5'000) { // Growing
                    CShadows::StoreStaticShadow(
                        reinterpret_cast<uint32>(this) + 0x11,
                        SHADOW_DEFAULT,
                        gpBloodPoolTex,
                        pos,
                        (float)(poolTime * BLOOD_RADIUS_PER_MS),
                        0.f,
                        0.f,
                        -(float)(poolTime * BLOOD_RADIUS_PER_MS),
                        255,
                        200,
                        0,
                        0,
                        4.f,
                        1.f,
                        40.f,
                        false,
                        0.f
                    );
                } else {
                    CShadows::AddPermanentShadow(
                        SHADOW_DEFAULT,
                        gpBloodPoolTex,
                        &pos,
                        BLOOD_RADIUS_MAX,
                        0.f,
                        0.f,
                        -BLOOD_RADIUS_MAX,
                        255,
                        200,
                        0,
                        0,
                        4.f,
                        40'000,
                        1.f
                    );
                    m_bBloodPuddleCreated = true;
                }
            }
        }
    }

    if (!m_bHasDrowned) {
        ped->m_pedIK.bSlopePitch = true;
        ped->m_vecMoveSpeed      = CVector{};
    } else {
        ped->bIsStanding  = false;
        ped->bWasStanding = false;
    }

    return false;
}

void CTaskSimpleDead::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDead, 0x86dea4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x630590, "1", {}, uint32, bool);
    RH_ScopedDestructorInstall(0x6305F0);

    RH_ScopedVMTInstall(Clone, 0x636100);
    RH_ScopedVMTInstall(GetTaskType, 0x6305D0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6305E0);
    RH_ScopedVMTInstall(ProcessPed, 0x630600);
}
