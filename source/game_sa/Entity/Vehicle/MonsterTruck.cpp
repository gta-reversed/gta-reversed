#include "StdInc.h"

#include "MonsterTruck.h"
auto& fWheelExtensionRate = StaticRef<float>(0x8D33AC);
// 0x8D33B0 - gates clamping of negative wheel compression ratios to zero in `ProcessControl`
auto& bClampNegativeWheelCompression = StaticRef<bool>(0x8D33B0);

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
CMonsterTruck::CMonsterTruck(int32 modelIndex, eVehicleCreatedBy createdBy) : CAutomobile(modelIndex, createdBy, false) {
    std::ranges::fill(field_988, 1.0f);
    CMonsterTruck::SetupSuspensionLines();
    autoFlags.bIsMonsterTruck = true;
    m_nVehicleSubType = VEHICLE_TYPE_MTRUCK;
}

// 0x6C8AE0
int32 CMonsterTruck::ProcessEntityCollision(CEntity* entity, CColPoint* colPoint) {
    if (GetStatus() != STATUS_SIMPLE) {
        vehicleFlags.bVehicleColProcessed = true; // OK
    }

    const auto tcm = GetColModel();

    if (physicalFlags.bSkipLineCol || physicalFlags.bProcessingShift || entity->GetIsTypePed()) {
        tcm->GetData()->m_nNumLines = 0; // hmm..... (Later reset back to 4)
    }

    auto wheelColPtsTouchDists{ m_wheelPosition };
    const auto numColPts = CCollision::ProcessColModels(
        GetMatrix(), *tcm,
        entity->GetMatrix(), *entity->GetColModel(),
        *(std::array<CColPoint, 32>*)(colPoint), // trust me bro
        m_wheelColPoint.data(),
        wheelColPtsTouchDists.data(),
        false
    );

    size_t numProcessedLines{};
    if (tcm->GetData()->m_nNumLines) {
        for (auto i = 0; i < MAX_CARWHEELS; i++) {
            const auto  thisWheelTouchDistNow = wheelColPtsTouchDists[i];
            const auto& thisWheelColPtNow = m_wheelColPoint[i];

            if (thisWheelTouchDistNow <= m_wheelPosition[i]) {
                continue;
            }

            if (!(GetUsesCollision() || !numColPts)) { // TODO: Why is this in the loop body?
                continue;
            }

            numProcessedLines++;

            m_fWheelsSuspensionCompression[i] = 0.f;
            m_wheelPosition[i] = thisWheelTouchDistNow;

            m_anCollisionLighting[i] = thisWheelColPtNow.m_nLightingB;
            m_nContactSurface = thisWheelColPtNow.m_nSurfaceTypeB;

            // Same as in CAutomobile::ProcessEntityCollision
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
                m_pEntityWeAreOn = entity;
                m_bTunnel = entity->m_bTunnel;
                m_bTunnelTransition = entity->m_bTunnelTransition;
                break;
            }
            }
        }
    } else {
        tcm->GetData()->m_nNumLines = MAX_CARWHEELS; // TODO: Magic (Each wheel has 1 suspension line right now, but hardcoding like this isnt good)
    }

    if (numColPts > 0 || numProcessedLines > 0) {
        AddCollisionRecord(entity);
        if (!entity->GetIsTypeBuilding()) {
            entity->AsPhysical()->AddCollisionRecord(this);
        }
        if (numColPts > 0) {
            if (   entity->GetIsTypeBuilding()
                || (entity->GetIsTypeObject() && entity->AsPhysical()->physicalFlags.bDisableCollisionForce)
            ) {
                SetHasHitWall(true);
            }
        }
    }

    return numColPts;
}

// 0x6C83A0
void CMonsterTruck::ProcessSuspension() {
    CVector directions[4]{};
    CVector contactPoints[4]{};
    for (auto i = 0; i < 4; i++) {
        directions[i] = GetUp() * -1.0f;
        if (m_fWheelsSuspensionCompression[i] < 1.0f) {
            contactPoints[i] = m_wheelColPoint[i].m_vecPoint - GetPosition();
        } else {
            contactPoints[i] = CVector{};
        }
    }

    float impulseMagnitudes[4]{};
    for (auto i = 0; i < 4; i++) {
        if (m_fWheelsSuspensionCompression[i] >= 1.0f) {
            continue;
        }
        float bias = m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear;
        if (i == CAR_WHEEL_REAR_LEFT || i == CAR_WHEEL_REAR_RIGHT) {
            bias = 1.0f - bias;
        }
        ApplySpringCollisionAlt(
            m_pHandlingData->m_fSuspensionForceLevel,
            directions[i],
            contactPoints[i],
            m_fWheelsSuspensionCompression[i],
            bias,
            m_wheelColPoint[i].m_vecNormal,
            impulseMagnitudes[i]
        );
    }

    CVector contactSpeeds[4]{};
    for (auto i = 0; i < 4; i++) {
        contactSpeeds[i] = CPhysical::GetSpeed(contactPoints[i]);
        if (m_apWheelCollisionEntity[i]) {
            contactSpeeds[i] -= m_apWheelCollisionEntity[i]->GetSpeed(m_vWheelCollisionPos[i]);
        }
        if (m_fWheelsSuspensionCompression[i] < 1.0f && m_wheelColPoint[i].m_vecNormal.z > 0.35f) {
            directions[i] = -m_wheelColPoint[i].m_vecNormal;
        }
    }

    for (auto i = 0; i < 4; i++) {
        if (m_fWheelsSuspensionCompression[i] < 1.0f) {
            ApplySpringDampening(
                m_pHandlingData->m_fSuspensionDampingLevel,
                impulseMagnitudes[i],
                directions[i],
                contactPoints[i],
                contactSpeeds[i]
            );
        }
    }

    for (auto i = 0; i < 4; i++) {
        const auto compression = m_fWheelsSuspensionCompression[i];
        const auto wheelEntity = m_apWheelCollisionEntity[i];
        if (compression < 1.0f && wheelEntity && wheelEntity->GetIsTypeVehicle()) {
            auto& colPoint = m_wheelColPoint[i];
            if (compression < 0.5f) {
                wheelEntity->AsVehicle()->VehicleDamage(
                    (1.0f - compression) * m_fMass * 0.05f,
                    (eVehicleCollisionComponent)colPoint.m_nPieceTypeB,
                    this,
                    &colPoint.m_vecPoint,
                    &colPoint.m_vecNormal,
                    WEAPON_RAMMEDBYCAR
                );
            }
            if (colPoint.m_vecNormal.z > 0.5f) {
                const auto pressScale = (1.0f - compression) * wheelEntity->m_fMass * -0.05f;
                wheelEntity->ApplyForce(
                    CVector{
                        colPoint.m_vecNormal.x * 0.25f * pressScale,
                        colPoint.m_vecNormal.y * 0.25f * pressScale,
                        colPoint.m_vecNormal.z * pressScale
                    },
                    colPoint.m_vecPoint - wheelEntity->GetPosition(),
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
    for (auto i = 0; i < 4; i++) {
        if (m_fWheelsSuspensionCompression[i] >= 1.0f) {
            m_fWheelsSuspensionCompression[i] = 1.0f;
        } else {
            m_fWheelsSuspensionCompression[i] = (m_aSuspensionSpringLength[i] - m_wheelPosition[i]) / (m_aSuspensionSpringLength[i] - m_aSuspensionLineLength[i]);
        }
    }
}

// 0x6C8250
void CMonsterTruck::ProcessControl() {
    const bool clampNegatives = bClampNegativeWheelCompression;
    for (auto i = 0; i < 4; i++) {
        if (m_fWheelsSuspensionCompression[i] >= 1.0f) {
            m_fWheelsSuspensionCompression[i] = 1.0f;
        } else {
            const auto ratio = (m_aSuspensionSpringLength[i] - m_wheelPosition[i]) / (m_aSuspensionSpringLength[i] - m_aSuspensionLineLength[i]);
            m_fWheelsSuspensionCompression[i] = ratio;
            if (ratio < 0.0f && clampNegatives) {
                m_fWheelsSuspensionCompression[i] = 0.0f;
            }
        }
    }
    CAutomobile::ProcessControl();
    if (!GetWasPostponed() && (GetMoveSpeed() != 0.0f || m_vecTurnSpeed != 0.0f)) {
        ExtendSuspension();
    }
}

// 0x6C7FB0
void CMonsterTruck::SetupSuspensionLines() {
    const auto mi = CModelInfo::GetModelInfo(m_nModelIndex)->AsVehicleModelInfoPtr();
    auto& cm = *mi->GetColModel();
    auto& cd = *cm.m_pColData;

    m_fSuspensionRadius = mi->m_fWheelSizeFront * 0.5f;
    if (!cd.m_pLines) {
        cd.m_nNumLines = 4;
        cd.m_pDisks = static_cast<CColDisk*>(CMemoryMgr::Malloc(0x90));
    } else if (!cd.bUsesDisks) {
        CMemoryMgr::Free(cd.m_pLines);
    }

    for (auto i = 0; i < 4; i++) {
        CVector wheelPos;
        mi->GetWheelPosn(i, wheelPos, false);
        auto& disk = cd.m_pDisks[i];
        disk.m_vecCenter = wheelPos;
        disk.m_fRadius = m_fSuspensionRadius;
        disk.m_vThickness = CVector{ i >= 2 ? 1.0f : -1.0f, 0.0f, 0.0f };
        disk.m_fThickness = m_fSuspensionRadius * 0.6f;
        disk.m_Surface.m_nMaterial = SURFACE_WHEELBASE;
        disk.m_Surface.m_nPiece = 13;
        disk.m_Surface.m_nLighting = tColLighting{ 0xFF };
        switch (i) {
        case CAR_WHEEL_REAR_LEFT:   disk.m_Surface.m_nPiece = 15; break;
        case CAR_WHEEL_FRONT_RIGHT: disk.m_Surface.m_nPiece = 14; break;
        case CAR_WHEEL_REAR_RIGHT:  disk.m_Surface.m_nPiece = 16; break;
        default: break;
        }
        m_aSuspensionSpringLength[i] = wheelPos.z + m_pHandlingData->m_fSuspensionUpperLimit;
        m_aSuspensionLineLength[i] = wheelPos.z + m_pHandlingData->m_fSuspensionLowerLimit;
    }

    const auto heightAboveRoad = (m_fSuspensionRadius - m_aSuspensionSpringLength[0])
        + (m_aSuspensionSpringLength[0] - m_aSuspensionLineLength[0]) * (1.0f - 1.0f / (m_pHandlingData->m_fSuspensionForceLevel * 4.0f));
    m_fFrontHeightAboveRoad = heightAboveRoad;
    m_fRearHeightAboveRoad = heightAboveRoad;

    for (auto i = 0; i < 4; i++) {
        m_fWheelsSuspensionCompression[i] = 1.0f;
        m_wheelPosition[i] = mi->m_fWheelSizeFront * 0.5f - heightAboveRoad;
    }

    const auto lineBottomZ = heightAboveRoad - m_fSuspensionRadius;
    if (lineBottomZ < cm.m_boundBox.m_vecMin.z) {
        cm.m_boundBox.m_vecMin.z = lineBottomZ;
    }
    cm.m_boundSphere.m_fRadius = std::max({ cm.m_boundSphere.m_fRadius, cm.m_boundBox.m_vecMin.Magnitude(), cm.m_boundBox.m_vecMax.Magnitude() });
    if (cm.m_boundSphere.m_fRadius <= CModelInfo::GetModelInfo(MODEL_DUMPER)->AsVehicleModelInfoPtr()->GetColModel()->m_boundSphere.m_fRadius) {
        return;
    }
    CModelInfo::GetModelInfo(MODEL_DUMPER)->AsVehicleModelInfoPtr()->GetColModel()->m_boundSphere.m_fRadius = cm.m_boundSphere.m_fRadius;
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
    for (auto i = 0; i < 4; i++) {
        const auto newPos = m_wheelPosition[i] - CTimer::GetTimeStep() * m_fSuspensionRadius * fWheelExtensionRate;
        m_wheelPosition[i] = newPos;
        if (m_aSuspensionLineLength[i] <= newPos) {
            if (m_aSuspensionSpringLength[i] < newPos) {
                m_wheelPosition[i] = m_aSuspensionSpringLength[i];
            }
        } else {
            m_wheelPosition[i] = m_aSuspensionLineLength[i];
        }
        m_fWheelsSuspensionCompression[i] = 1.0f;
    }
}

// 0x6C7D40
void CMonsterTruck::ResetSuspension() {
    CAutomobile::ResetSuspension();
    for (auto i = 0; i < 4; i++) {
        m_wheelPosition[i] = m_aSuspensionLineLength[i];
        field_988[i] = 1.0f;
    }
}
bool CMonsterTruck::BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) {
    return false;
}

// 0x6C7D20
bool CMonsterTruck::SetUpWheelColModel(CColModel* colModel) {
    return false;
}
