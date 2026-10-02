#include "StdInc.h"
#include "MonsterTruck.h"

constexpr bool  bClampSuspensionCompression = true;
constexpr float fWheelCrushForceMult        = -0.05f;
constexpr float fWheelCrushDamageMult       = 0.05f;

void CMonsterTruck::InjectHooks() {
    RH_ScopedVirtualClass(CMonsterTruck, 0x8717d8, 71);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6C8D60);
    RH_ScopedInstall(ExtendSuspension, 0x6C7D80);

    RH_ScopedVMTInstall(ProcessEntityCollision, 0x6C8AE0);
    RH_ScopedVMTInstall(ProcessSuspension, 0x6C83A0);
    RH_ScopedVMTInstall(ProcessControlCollisionCheck, 0x6C8330);
    RH_ScopedVMTInstall(ProcessControl, 0x6C8250);
    RH_ScopedVMTInstall(SetupSuspensionLines, 0x6C7FB0);
    RH_ScopedVMTInstall(PreRender, 0x6C7DE0);
    RH_ScopedVMTInstall(ResetSuspension, 0x6C7D40);
    RH_ScopedVMTInstall(BurstTyre, 0x6C7D30);
    RH_ScopedVMTInstall(SetUpWheelColModel, 0x6C7D20);
}

// 0x6C8D60
CMonsterTruck::CMonsterTruck(int32 modelIndex, eVehicleCreatedBy createdBy)
    : CAutomobile(modelIndex, createdBy, false)
{
    std::ranges::fill(m_aBigTyreCompression, 1.0f);
    CMonsterTruck::SetupSuspensionLines();
    autoFlags.bIsMonsterTruck = true;
    m_nVehicleSubType = VEHICLE_TYPE_MTRUCK;
}

// 0x6C8AE0
int32 CMonsterTruck::ProcessEntityCollision(CEntity* entity, CColPoint* colPoint) {
    if (GetStatus() != STATUS_SIMPLE) {
        vehicleFlags.bVehicleColProcessed = true;
    }

    const auto tcm = GetColModel();

    if (physicalFlags.bSkipLineCol || physicalFlags.bProcessingShift || entity->GetIsTypePed()) {
        tcm->GetData()->m_nNumLines = 0;
    }

    auto wheelColPtsTouchDists{ m_wheelPosition };
    const auto numColPts = CCollision::ProcessColModels(
        GetMatrix(), *tcm,
        entity->GetMatrix(), *entity->GetColModel(),
        *reinterpret_cast<std::array<CColPoint, 32>*>(colPoint),
        m_wheelColPoint.data(),
        wheelColPtsTouchDists.data(),
        false
    );

    size_t numProcessedLines = 0;
    if (tcm->GetData()->m_nNumLines) {
        for (int i = 0; i < MAX_CARWHEELS; ++i) {
            const auto  thisWheelTouchDistNow = wheelColPtsTouchDists[i];
            const auto& thisWheelColPtNow     = m_wheelColPoint[i];

            if (thisWheelTouchDistNow <= m_wheelPosition[i])
                continue;

            if (!GetUsesCollision() && numColPts)
                continue;

            ++numProcessedLines;

            m_fWheelsSuspensionCompression[i] = 0.0f;
            m_wheelPosition[i]                = thisWheelTouchDistNow;

            m_anCollisionLighting[i] = thisWheelColPtNow.m_nLightingB;
            m_nContactSurface        = thisWheelColPtNow.m_nSurfaceTypeB;

            switch (entity->GetType()) {
            case ENTITY_TYPE_VEHICLE:
            case ENTITY_TYPE_OBJECT: {
                CEntity::ChangeEntityReference(m_apWheelCollisionEntity[i], entity->AsPhysical());
                m_vWheelCollisionPos[i] = thisWheelColPtNow.m_vecPoint - entity->GetPosition();
                if (entity->GetIsTypeVehicle()) {
                    m_anCollisionLighting[i] = entity->AsVehicle()->m_anCollisionLighting[i];
                }
                break;
            }
            case ENTITY_TYPE_BUILDING: {
                m_pEntityWeAreOn    = entity;
                m_bTunnel           = entity->m_bTunnel;
                m_bTunnelTransition = entity->m_bTunnelTransition;
                break;
            }
            }
        }
    } else {
        tcm->GetData()->m_nNumLines = MAX_CARWHEELS;
    }

    if (numColPts > 0 || numProcessedLines > 0) {
        AddCollisionRecord(entity);
        if (!entity->GetIsTypeBuilding()) {
            entity->AsPhysical()->AddCollisionRecord(this);
        }
        if (numColPts > 0) {
            if (entity->GetIsTypeBuilding() ||
                (entity->GetIsTypeObject() && entity->AsPhysical()->physicalFlags.bDisableCollisionForce)) {
                SetHasHitWall(true);
            }
        }
    }

    return numColPts;
}

// 0x6C83A0
void CMonsterTruck::ProcessSuspension() {
    std::array<CVector, MAX_CARWHEELS> contactPoints{};
    std::array<CVector, MAX_CARWHEELS> directions{};
    std::array<float,   MAX_CARWHEELS> wheelSpringForceDampingLimits{};

    for (int i = 0; i < MAX_CARWHEELS; ++i) {
        directions[i] = GetUp() * -1.0f;
        if (m_fWheelsSuspensionCompression[i] < 1.0f) {
            contactPoints[i] = m_wheelColPoint[i].m_vecPoint - GetPosition();
        } else {
            contactPoints[i] = CVector{};
        }
    }

    // Applying spring force to each wheel
    for (int i = 0; i < MAX_CARWHEELS; ++i) {
        if (m_fWheelsSuspensionCompression[i] >= 1.0f)
            continue;

        float fSuspensionBias = m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear;
        if (i == CAR_WHEEL_REAR_LEFT || i == CAR_WHEEL_REAR_RIGHT) {
            fSuspensionBias = 1.0f - fSuspensionBias;
        }

        ApplySpringCollisionAlt(
            m_pHandlingData->m_fSuspensionForceLevel,
            directions[i],
            contactPoints[i],
            m_fWheelsSuspensionCompression[i],
            fSuspensionBias,
            m_wheelColPoint[i].m_vecNormal,
            wheelSpringForceDampingLimits[i]
        );
    }

    std::array<CVector, MAX_CARWHEELS> contactSpeeds{};
    for (int i = 0; i < MAX_CARWHEELS; ++i) {
        contactSpeeds[i] = GetSpeed(contactPoints[i]);
        if (m_apWheelCollisionEntity[i]) {
            contactSpeeds[i] -= m_apWheelCollisionEntity[i]->GetSpeed(m_vWheelCollisionPos[i]);
        }

        if (m_fWheelsSuspensionCompression[i] < 1.0f && m_wheelColPoint[i].m_vecNormal.z > 0.35f) {
            directions[i] = -m_wheelColPoint[i].m_vecNormal;
        }
    }

    for (int i = 0; i < MAX_CARWHEELS; ++i) {
        if (m_fWheelsSuspensionCompression[i] < 1.0f) {
            ApplySpringDampening(
                m_pHandlingData->m_fSuspensionDampingLevel,
                wheelSpringForceDampingLimits[i],
                directions[i],
                contactPoints[i],
                contactSpeeds[i]
            );
        }
    }

    // We're crushing vehicles with wheels on them
    for (int i = 0; i < MAX_CARWHEELS; ++i) {
        auto& colPoint = m_wheelColPoint[i];
        if (m_fWheelsSuspensionCompression[i] < 1.0f &&
            m_apWheelCollisionEntity[i] &&
            m_apWheelCollisionEntity[i]->GetIsTypeVehicle()) {

            const auto veh = m_apWheelCollisionEntity[i]->AsVehicle();

            if (m_fWheelsSuspensionCompression[i] < 0.5f) {
                veh->VehicleDamage(
                    (1.0f - m_fWheelsSuspensionCompression[i]) * m_fMass * fWheelCrushDamageMult,
                    static_cast<eVehicleCollisionComponent>(colPoint.m_nPieceTypeB),
                    this,
                    &colPoint.m_vecPoint,
                    &colPoint.m_vecNormal,
                    WEAPON_RAMMEDBYCAR
                );
            }

            if (colPoint.m_vecNormal.z > 0.5f) {
                veh->ApplyForce(
                    CVector{
                        colPoint.m_vecNormal.x * 0.25f,
                        colPoint.m_vecNormal.y * 0.25f,
                        colPoint.m_vecNormal.z
                    } * ((1.0f - m_fWheelsSuspensionCompression[i]) * fWheelCrushForceMult * veh->m_fMass),
                    colPoint.m_vecPoint - veh->GetPosition(),
                    true
                );
            }
        }
        m_apWheelCollisionEntity[i] = nullptr;
    }
}

// 0x6C8330
void CMonsterTruck::ProcessControlCollisionCheck(bool applySpeed) {
    ExtendSuspension();
    CAutomobile::ProcessControlCollisionCheck(applySpeed);

    for (size_t i = 0; i < m_fWheelsSuspensionCompression.size(); ++i) {
        if (m_fWheelsSuspensionCompression[i] >= 1.0f) {
            m_fWheelsSuspensionCompression[i] = 1.0f;
        } else {
            m_fWheelsSuspensionCompression[i] =
                (m_aSuspensionSpringLength[i] - m_wheelPosition[i]) /
                (m_aSuspensionSpringLength[i] - m_aSuspensionLineLength[i]);
        }
    }
}

// 0x6C8250
void CMonsterTruck::ProcessControl() {
    for (size_t i = 0; i < m_fWheelsSuspensionCompression.size(); ++i) {
        if (m_fWheelsSuspensionCompression[i] >= 1.0f) {
            m_fWheelsSuspensionCompression[i] = 1.0f;
        } else {
            m_fWheelsSuspensionCompression[i] =
                (m_aSuspensionSpringLength[i] - m_wheelPosition[i]) /
                (m_aSuspensionSpringLength[i] - m_aSuspensionLineLength[i]);
            if (m_fWheelsSuspensionCompression[i] < 0.0f && bClampSuspensionCompression) {
                m_fWheelsSuspensionCompression[i] = 0.0f;
            }
        }
    }

    CAutomobile::ProcessControl();

    if (!m_bWasPostponed && (m_vecMoveSpeed != CVector{} || m_vecTurnSpeed != CVector{})) {
        ExtendSuspension();
    }
}

// 0x6C7FB0
void CMonsterTruck::SetupSuspensionLines() {
    const auto mi = GetVehicleModelInfo();
    auto& cm = *mi->GetColModel();
    auto& cd = *cm.m_pColData;

    m_fSuspensionRadius = mi->m_fWheelSizeFront * 0.5f;

    // We create discs (instead of the usual suspension lines) — one per wheel
    if (!cd.m_pLines) {
        cd.bUsesDisks  = true;
        cd.m_nNumLines = MAX_CARWHEELS;
        cd.m_pDisks    = static_cast<CColDisk*>(CMemoryMgr::Malloc(sizeof(CColDisk) * MAX_CARWHEELS, 0));
    } else if (!cd.bUsesDisks) {
        CMemoryMgr::Free(cd.m_pLines);
    }

    for (int i = 0; i < MAX_CARWHEELS; ++i) {
        CVector wheelPos;
        mi->GetWheelPosn(i, wheelPos, false);

        const CVector dir{ i < 2 ? -1.0f : 1.0f, 0.0f, 0.0f };
        cd.m_pDisks[i].Set(
            m_fSuspensionRadius,
            wheelPos,
            dir,
            m_fSuspensionRadius * 0.6f,
            SURFACE_WHEELBASE,
            CAR_PIECE_WHEEL_LF,
            tColLighting(0xFF)
        );

        switch (i) {
        case CAR_WHEEL_REAR_LEFT:   cd.m_pDisks[i].m_Surface.m_nPiece = CAR_PIECE_WHEEL_RL; break;
        case CAR_WHEEL_FRONT_RIGHT: cd.m_pDisks[i].m_Surface.m_nPiece = CAR_PIECE_WHEEL_RF; break;
        case CAR_WHEEL_REAR_RIGHT:  cd.m_pDisks[i].m_Surface.m_nPiece = CAR_PIECE_WHEEL_RR; break;
        }

        m_aSuspensionSpringLength[i] = wheelPos.z + m_pHandlingData->m_fSuspensionUpperLimit;
        m_aSuspensionLineLength[i]   = wheelPos.z + m_pHandlingData->m_fSuspensionLowerLimit;
    }

    const auto heightAboveRoad = (m_fSuspensionRadius - m_aSuspensionSpringLength[0])
        + (m_aSuspensionSpringLength[0] - m_aSuspensionLineLength[0]) *
          (1.0f - 1.0f / (m_pHandlingData->m_fSuspensionForceLevel * 4.0f));
    m_fFrontHeightAboveRoad = m_fRearHeightAboveRoad = heightAboveRoad;

    for (size_t i = 0; i < m_wheelPosition.size(); ++i) {
        m_fWheelsSuspensionCompression[i] = 1.0f;
        m_wheelPosition[i] = mi->m_fWheelSizeFront * 0.5f - m_fFrontHeightAboveRoad;
    }

    const auto wheelBottomZ = m_fFrontHeightAboveRoad - m_fSuspensionRadius;
    if (wheelBottomZ < cm.m_boundBox.m_vecMin.z) {
        cm.m_boundBox.m_vecMin.z = wheelBottomZ;
    }
    cm.m_boundSphere.m_fRadius = std::max({
        cm.m_boundSphere.m_fRadius,
        cm.m_boundBox.m_vecMin.Magnitude(),
        cm.m_boundBox.m_vecMax.Magnitude()
    });
}

// 0x6C7DE0
void CMonsterTruck::PreRender() {
    for (int i = 0; i < 4; ++i) {
        m_wheelPosition[i] = std::min(m_wheelPosition[i], m_aSuspensionSpringLength[i]);
    }

    CAutomobile::PreRender();

    const auto mi = GetVehicleModelInfo();
    CMatrix mat;
    CVector pos;

    mi->GetWheelPosn(CAR_WHEEL_FRONT_LEFT, pos, false);
    SetTransmissionRotation(m_aCarNodes[MONSTER_TRANSMISSION_F], m_wheelPosition[CAR_WHEEL_FRONT_LEFT], m_wheelPosition[CAR_WHEEL_FRONT_RIGHT], pos, true);

    mi->GetWheelPosn(CAR_WHEEL_REAR_LEFT, pos, false);
    SetTransmissionRotation(m_aCarNodes[MONSTER_TRANSMISSION_R], m_wheelPosition[CAR_WHEEL_REAR_LEFT], m_wheelPosition[CAR_WHEEL_REAR_RIGHT], pos, false);

    if (m_nModelIndex == MODEL_DUMPER && m_aCarNodes[MONSTER_MISC_A]) {
        SetComponentRotation(m_aCarNodes[MONSTER_MISC_A], AXIS_X, (float)m_wMiscComponentAngle * DUMPER_COL_ANGLEMULT, true);
    }
}

// 0x6C7D80
void CMonsterTruck::ExtendSuspension() {
    for (size_t i = 0; i < m_wheelPosition.size(); ++i) {
        m_wheelPosition[i] -= CTimer::GetTimeStep() * m_fSuspensionRadius * fWheelExtensionRate;
        m_wheelPosition[i] = std::clamp(m_wheelPosition[i], m_aSuspensionLineLength[i], m_aSuspensionSpringLength[i]);
        m_fWheelsSuspensionCompression[i] = 1.0f;
    }
}

// 0x6C7D40
void CMonsterTruck::ResetSuspension() {
    CAutomobile::ResetSuspension();
    std::ranges::copy(m_aSuspensionLineLength, m_wheelPosition.begin());
    std::ranges::fill(m_aBigTyreCompression, 1.0f);
}

// 0x6C7D30
bool CMonsterTruck::BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) {
    return false;
}

// 0x6C7D20
bool CMonsterTruck::SetUpWheelColModel(CColModel* colModel) {
    return false;
}
