#include "StdInc.h"

#include "MonsterTruck.h"

auto& fWheelExtensionRate = StaticRef<float>(0x8D33AC);

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
    std::ranges::fill(m_aBigTyreCompression, 1.0f);
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
    CVector vecOffset[4]{};
    CVector vecDirWheel[4]{};

    for (int i = 0; i < MAX_CARWHEELS; i++) {
        vecDirWheel[i] = -GetUp();
        vecOffset[i] = CVector(0.0f, 0.0f, 0.0f);

        if (m_fWheelsSuspensionCompression[i] < 1.0f) {
            CVector pos = GetPosition();
            vecOffset[i] = m_wheelColPoint[i].m_vecPoint - pos;
        }
    }

    float aWheelSpringForces[4]{};
    float fBiasFront = m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear;
    float fBiasRear = 1.0f - fBiasFront;

    if (m_fWheelsSuspensionCompression[0] < 1.0f) {
        ApplySpringCollisionAlt(m_pHandlingData->m_fSuspensionForceLevel, vecDirWheel[0], vecOffset[0],
            m_fWheelsSuspensionCompression[0], fBiasFront, m_wheelColPoint[0].m_vecNormal, aWheelSpringForces[0]);
    }

    if (m_fWheelsSuspensionCompression[1] < 1.0f) {
        ApplySpringCollisionAlt(m_pHandlingData->m_fSuspensionForceLevel, vecDirWheel[1], vecOffset[1],
            m_fWheelsSuspensionCompression[1], fBiasRear, m_wheelColPoint[1].m_vecNormal, aWheelSpringForces[1]);
    }

    if (m_fWheelsSuspensionCompression[2] < 1.0f) {
        ApplySpringCollisionAlt(m_pHandlingData->m_fSuspensionForceLevel, vecDirWheel[2], vecOffset[2],
            m_fWheelsSuspensionCompression[2], fBiasFront, m_wheelColPoint[2].m_vecNormal, aWheelSpringForces[2]);
    }

    if (m_fWheelsSuspensionCompression[3] < 1.0f) {
        ApplySpringCollisionAlt(m_pHandlingData->m_fSuspensionForceLevel, vecDirWheel[3], vecOffset[3],
            m_fWheelsSuspensionCompression[3], fBiasRear, m_wheelColPoint[3].m_vecNormal, aWheelSpringForces[3]);
    }

    CVector vecSpeed[4]{};
    for (int i = 0; i < 4; i++) {
        vecSpeed[i] = GetSpeed(vecOffset[i]);

        CPhysical* pHitEntity = m_apWheelCollisionEntity[i];
        if (pHitEntity) {
            CVector entitySpeed = pHitEntity->GetSpeed(vecOffset[i]);
            vecSpeed[i] -= entitySpeed;
        }

        if (m_fWheelsSuspensionCompression[i] < 1.0f && m_wheelColPoint[i].m_vecNormal.z > 0.35f) {
            vecDirWheel[i] = -m_wheelColPoint[i].m_vecNormal;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (m_fWheelsSuspensionCompression[i] < 1.0f) {
            ApplySpringDampening(m_pHandlingData->m_fSuspensionDampingLevel, aWheelSpringForces[i], vecDirWheel[i], vecOffset[i], vecSpeed[i]);
        }
    }

    for (int i = 0; i < MAX_CARWHEELS; i++) {
        float fRatio = m_fWheelsSuspensionCompression[i];
        auto pTargetEntity = (CAutomobile*)m_apWheelCollisionEntity[i];

        if (fRatio < 1.0f && pTargetEntity) {
            if (pTargetEntity->GetType() == 2) {
                if (fRatio < 0.5f) {
                    float fDamageIntensity = (0.05f - (fRatio * 0.05f)) * m_fMass;
                    pTargetEntity->VehicleDamage(fDamageIntensity, (eVehicleCollisionComponent)m_wheelColPoint[i].m_nPieceTypeB,
                        this, &m_wheelColPoint[i].m_vecPoint, &m_wheelColPoint[i].m_vecNormal, WEAPON_RAMMEDBYCAR);

                    if (m_wheelColPoint[i].m_vecNormal.z > 0.5f) {
                        float fForceMult = (1.0f - fRatio) * -0.05f * m_fMass;

                        CVector vecForce(
                            m_wheelColPoint[i].m_vecNormal.x * 0.25f * fForceMult,
                            m_wheelColPoint[i].m_vecNormal.y * 0.25f * fForceMult,
                            m_wheelColPoint[i].m_vecNormal.z * fForceMult
                        );

                        CVector targetPos = pTargetEntity->GetPosition();
                        CVector vecPoint = m_wheelColPoint[i].m_vecPoint - targetPos;

                        pTargetEntity->ApplyForce(vecForce, vecPoint, true);
                    }
                }
            }
        }

        m_apWheelCollisionEntity[i] = nullptr;
    }
}

// 0x6C8330
void CMonsterTruck::ProcessControlCollisionCheck(bool applySpeed) {
    float fExtensionStep = CTimer::ms_fTimeStep * (fWheelExtensionRate * m_fSuspensionRadius);

    for (int i = 0; i < MAX_CARWHEELS; i++) {
        float fNewHeight = m_wheelPosition[i] - fExtensionStep;

        if (fNewHeight < m_aSuspensionLineLength[i]) {
            fNewHeight = m_aSuspensionLineLength[i];
        } else if (fNewHeight > m_aSuspensionSpringLength[i]) {
            fNewHeight = m_aSuspensionSpringLength[i];
        }

        m_fWheelsSuspensionCompression[i] = fNewHeight;
        m_wheelPosition[i] = 1.0f;
    }

    CAutomobile::ProcessControlCollisionCheck(applySpeed);

    for (auto i = 0; i < MAX_CARWHEELS; i++) {
        float fRatio = 1.0f;

        if (m_wheelPosition[i] < 1.0f) {
            float fSuspensionLen = m_aSuspensionSpringLength[i];
            float fCompressedDist = fSuspensionLen - m_wheelPosition[i];
            float fTotalTravel = fSuspensionLen - m_aSuspensionLineLength[i];

            fRatio = fCompressedDist / fTotalTravel;
        }

        m_fWheelsSuspensionCompression[i] = fRatio;
    }
}

// 0x6C8250
void CMonsterTruck::ProcessControl() {
    for (auto i = 0; i < MAX_CARWHEELS; ++i) {
        if (m_fWheelsSuspensionCompression[i] < 1.0f) {
            float suspLen = m_aSuspensionSpringLength[i];
            float diff = suspLen - m_wheelPosition[i];
            float lineDiff = suspLen - m_aSuspensionLineLength[i];

            float ratio = diff / lineDiff;
            m_fWheelsSuspensionCompression[i] = (ratio < 0.0f) ? 0.0f : ratio;
        } else {
            m_fWheelsSuspensionCompression[i] = 1.0f;
        }
    }

    CAutomobile::ProcessControl();

    if (!GetWasPostponed() &&
        (m_vecMoveSpeed.x != 0.0f || m_vecMoveSpeed.y != 0.0f || m_vecMoveSpeed.z != 0.0f ||
         m_vecTurnSpeed.x != 0.0f || m_vecTurnSpeed.y != 0.0f || m_vecTurnSpeed.z != 0.0f)) {
        float stepOffset = CTimer::ms_fTimeStep * (fWheelExtensionRate * m_fSuspensionRadius);

        for (auto i = 0; i < MAX_CARWHEELS; ++i) {
            float targetHeight = m_wheelPosition[i] - stepOffset;

            m_wheelPosition[i] = std::clamp(targetHeight, m_aSuspensionLineLength[i], m_aSuspensionSpringLength[i]);

            m_fWheelsSuspensionCompression[i] = 1.0f;
        }
    }
}

// 0x6C7FB0
void CMonsterTruck::SetupSuspensionLines() {
    auto pModelInfo = (CVehicleModelInfo*)CModelInfo::ms_modelInfoPtrs[m_nModelIndex];
    auto pColModel = pModelInfo->GetColModel();
    auto pColData = pColModel->GetData();

    m_fSuspensionRadius = pModelInfo->m_fWheelSizeFront * 0.5f;

    if (!pColData->m_pDisks || pColData->bUsesDisks == 0) {
        if (pColData->m_pDisks && pColData->bUsesDisks == 0) {
            CMemoryMgr::Free(pColData->m_pDisks);
        }
        pColData->bUsesDisks = 1;
        pColData->m_nNumLines = MAX_CARWHEELS;
        pColData->m_pDisks = (CColDisk*)CMemoryMgr::Malloc(4 * sizeof(CColDisk));
    }

    auto pDisks = pColData->m_pDisks;

    for (auto i = 0; i < MAX_CARWHEELS; i++) {
        CVector vecWheelPos{};
        pModelInfo->GetWheelPosn(i, vecWheelPos, false);

        CVector vecThickness{};
        vecThickness.x = (i >= 2) ? 1.0f : -1.0f;

        pDisks[i].Set(m_fSuspensionRadius, vecWheelPos, vecThickness, m_fSuspensionRadius * 0.6f, SURFACE_WHEELBASE, 13);

        if (i >= 1 && i <= 3) {
            uint8_t pieceTypes[3]  = { 15, 14, 16 };
            pDisks[i].m_Surface.m_nPiece = pieceTypes[i - 1];
        }

        m_aSuspensionSpringLength[i] = vecWheelPos.z + m_pHandlingData->m_fSuspensionUpperLimit;
        m_aSuspensionLineLength[i] = vecWheelPos.z + m_pHandlingData->m_fSuspensionLowerLimit;
    }

    float fTotalTravel = m_aSuspensionSpringLength[0] - m_aSuspensionLineLength[0];
    float fRestForceMult = (1.0f / (m_pHandlingData->m_fSuspensionForceLevel * -4.0f)) + 1.0f;
    float fRestCompression = fTotalTravel * fRestForceMult;

    float fHeightAboveRoad = m_fSuspensionRadius - (m_aSuspensionSpringLength[0] - fRestCompression);
    m_fFrontHeightAboveRoad = fHeightAboveRoad;
    m_fRearHeightAboveRoad = fHeightAboveRoad;

    float fWheelRadius = pModelInfo->m_fWheelSizeFront * 0.5f;
    for (auto i = 0; i < MAX_CARWHEELS; i++) {
        m_fWheelsSuspensionCompression[i] = 1.0f;
        m_wheelPosition[i] = -(fHeightAboveRoad - fWheelRadius);
    }

    float fBottomExtent = fHeightAboveRoad - m_fSuspensionRadius;
    if (fBottomExtent < pColModel->GetBoundingBox().m_vecMin.z) {
        pColModel->GetBoundingBox().m_vecMin.z = fBottomExtent;
    }

    CVector vecBoxExt = (pColModel->GetBoundingBox().m_vecMin.Magnitude() > pColModel->GetBoundingBox().m_vecMax.Magnitude())
        ? pColModel->GetBoundingBox().m_vecMin
        : pColModel->GetBoundingBox().m_vecMax;

    float fNewRadius = vecBoxExt.Magnitude();
    if (pColModel->GetBoundingSphere().m_fRadius < fNewRadius) {
        pColModel->GetBoundingSphere().m_fRadius = fNewRadius;
    }
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
    float fExtensionStep = (fWheelExtensionRate * m_fSuspensionRadius) * CTimer::ms_fTimeStep;

    for (auto i = 0; i < MAX_CARWHEELS; ++i) {
        float fNewHeight = m_wheelPosition[i] - fExtensionStep;
        m_wheelPosition[i] = fNewHeight;

        if (fNewHeight < m_aSuspensionLineLength[i]) {
            m_wheelPosition[i] = m_aSuspensionLineLength[i];
        } else if (fNewHeight > m_aSuspensionSpringLength[i]) {
            m_wheelPosition[i] = m_aSuspensionSpringLength[i];
        }

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
