#include "StdInc.h"

#include "Trailer.h"
#include "VehicleRecording.h"

constexpr float BAGGAGE_TRAILER_TOWED_RATIO = -1000.0f;

// wtf
float flt_C1CB28 = *(float*)0xC1CB28; // 0.004f ?

float TRAILER_TOWED_MINRATIO = 0.9f; // 0x8D346C
const float TRAILER_SUPPORT_RETRACTION_RATE = 0.002f; // 0x871C14
const float TRAILER_SUPPORT_EXTENSION_RATE = 0.002f; // 0x871C18
const float TRAILER_SUPPORT_WAIT_EXTENSION_RATE = 0.0005f; // 0x871C1C

float TRAILER_TOWBAR_OFFSET_Y = -0.05f; // 0x871C20

float RELINK_TRAILER_DIFF_LIMIT_XY = 0.4f; // 0x8D3470
float RELINK_TRAILER_DIFF_LIMIT_Z = 1.0f; // 0x8D3474

void CTrailer::InjectHooks() {
    RH_ScopedVirtualClass(CTrailer, 0x871c28, 71);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6D03A0);
    RH_ScopedInstall(ScanForTowLink, 0x6CF030);

    RH_ScopedVMTInstall(SetupSuspensionLines, 0x6CF1A0);
    RH_ScopedVMTInstall(SetTowLink, 0x6CFDF0);
    RH_ScopedVMTInstall(ResetSuspension, 0x6CEE50);
    RH_ScopedVMTInstall(ProcessSuspension, 0x6CF6A0);
    RH_ScopedVMTInstall(ProcessEntityCollision, 0x6CFFD0);
    RH_ScopedVMTInstall(ProcessControl, 0x6CED20);
    RH_ScopedVMTInstall(ProcessAI, 0x6CF590);
    RH_ScopedVMTInstall(PreRender, 0x6CFAC0);
    RH_ScopedVMTInstall(GetTowHitchPos, 0x6CEEA0);
    RH_ScopedVMTInstall(GetTowBarPos, 0x6CFD60);
    RH_ScopedVMTInstall(BreakTowLink, 0x6CEFB0);
}

// 0x6D03A0
CTrailer::CTrailer(int32 modelIndex, eVehicleCreatedBy createdBy) : CAutomobile(modelIndex, createdBy, false) {
    m_nVehicleSubType = VEHICLE_TYPE_TRAILER;

    if (m_nModelIndex == MODEL_BAGBOXA || m_nModelIndex == MODEL_BAGBOXB) {
        m_fTrailerTowedRatio = BAGGAGE_TRAILER_TOWED_RATIO;
    }

    SetupSuspensionLines();
    SetStatus(eEntityStatus::STATUS_ABANDONED);
}

// 0x6CF1A0
void CTrailer::SetupSuspensionLines() {
    if (m_fTrailerTowedRatio == BAGGAGE_TRAILER_TOWED_RATIO) {
        CAutomobile::SetupSuspensionLines();
        return;
    }

    auto* mi = CModelInfo::ms_modelInfoPtrs[m_nModelIndex]->AsVehicleModelInfoPtr();
    CColModel* cl = GetColModel();

    CCollisionData* colData = cl->m_pColData;

    if (!colData->m_pLines) {
        colData->m_nNumLines = NUM_TRAILER_SUSP_LINES;
        colData->m_pLines = (CColLine*)CMemoryMgr::Malloc(sizeof(CColLine) * NUM_TRAILER_SUSP_LINES);
    }

    for (int32 i = 0; i < NUM_TRAILER_WHEELS; ++i) {
        CVector wheelPos;
        mi->GetWheelPosn(i, wheelPos, false);
        m_wheelPosition[i] = wheelPos.z;

        CVector lineStart = wheelPos;
        lineStart.z += m_pHandlingData->m_fSuspensionUpperLimit;
        colData->m_pLines[i].m_vecStart = lineStart;

        CVector lineEnd = lineStart;
        lineEnd.z += m_pHandlingData->m_fSuspensionLowerLimit - m_pHandlingData->m_fSuspensionUpperLimit - mi->m_fWheelSizeFront / 2;
        colData->m_pLines[i].m_vecEnd = lineEnd;

        m_aSuspensionSpringLength[i] = m_pHandlingData->m_fSuspensionUpperLimit - m_pHandlingData->m_fSuspensionLowerLimit;
        m_aSuspensionLineLength[i]   = colData->m_pLines[i].m_vecStart.z - colData->m_pLines[i].m_vecEnd.z;
    }

    cl->m_boundBox.m_vecMin.z = std::min(cl->m_boundBox.m_vecMin.z, colData->m_pLines[0].m_vecEnd.z);

    // 0x6CF313
    const float maxDistSq = std::max(cl->m_boundBox.m_vecMin.Magnitude(), cl->m_boundBox.m_vecMax.Magnitude());
    cl->m_boundSphere.m_fRadius = std::max(cl->m_boundSphere.m_fRadius, maxDistSq);

    if (m_aCarNodes[TRAILER_MISC_A]) {
        RwMatrix matrix;
        RwFrame* wheelFront = m_aCarNodes[TRAILER_MISC_A];
        matrix = *RwFrameGetMatrix(wheelFront);

        auto parent = RwFrameGetParent(wheelFront);
        if (parent) {
            do {
                RwMatrixTransform(&matrix, RwFrameGetMatrix(parent), rwCOMBINEPOSTCONCAT);
                parent = RwFrameGetParent(parent);
            } while (parent != wheelFront && parent);
        }
        m_fHeight = matrix.pos.z;
    }

    CColLine* supportLines = &colData->m_pLines[NUM_TRAILER_WHEELS];
    for (int32 i = 0; i < NUM_TRAILER_SUPPORTS; ++i) {
        CVector startPos;
        startPos.x = (i == 0) ? cl->m_boundBox.m_vecMin.x : -cl->m_boundBox.m_vecMin.x;
        startPos.y = cl->m_boundBox.m_vecMax.y;
        startPos.z = cl->m_boundBox.m_vecMax.z;
        supportLines[i].m_vecStart = startPos;

        CVector endPos = startPos;
        endPos.z = cl->m_boundBox.m_vecMin.z;
        supportLines[i].m_vecEnd = endPos;
    }

    const float supportLegY = fabs(supportLines[0].m_vecStart.y);
    const float avgWheelY = (fabs(colData->m_pLines[0].m_vecStart.y) + fabs(colData->m_pLines[1].m_vecStart.y)) * 0.5f;
    const float weightRatio = supportLegY / (avgWheelY + supportLegY);

    const float suspensionForceFactor = m_pHandlingData->m_fSuspensionForceLevel * 4.0f;
    const float heightAboveRoad = m_aSuspensionSpringLength[0] * (1.0f - weightRatio / suspensionForceFactor) 
                                + mi->m_fWheelSizeFront / 2 - colData->m_pLines[0].m_vecStart.z;

    m_fFrontHeightAboveRoad = heightAboveRoad;
    m_fRearHeightAboveRoad  = heightAboveRoad;

    const float supportForce = CTrailer::m_fTrailerSuspensionForce * 2.0f;
    const float newEndZ = supportLines[0].m_vecStart.z - (supportLines[0].m_vecStart.z + heightAboveRoad) 
                        / (1.0f - (1.0f - weightRatio) / supportForce);

    for (int i = 0; i < NUM_TRAILER_SUPPORTS; ++i) {
        supportLines[i].m_vecEnd.z = newEndZ;
        m_wheelPosition[i] = mi->m_fWheelSizeFront / 2 - m_fFrontHeightAboveRoad;
    }
}

// 0x6CFDF0
bool CTrailer::SetTowLink(CVehicle* vehicle, bool setMyPosToTowBar) {
    if (!vehicle) {
        return false;
    }

    const auto status = GetStatus();
    if (status != STATUS_PHYSICS && status != STATUS_IS_TOWED && status != STATUS_ABANDONED) {
        return false;
    }

    SetStatus(STATUS_IS_TOWED);

    m_pTowingVehicle = vehicle;
    m_pTowingVehicle->RegisterReference(m_pTowingVehicle);

    vehicle->m_pVehicleBeingTowed = this;
    vehicle->m_pVehicleBeingTowed->RegisterReference(vehicle->m_pVehicleBeingTowed);

    CPhysical::RemoveFromMovingList();
    vehicle->RemoveFromMovingList();

    AddToMovingList();
    vehicle->AddToMovingList();

    if (!setMyPosToTowBar) {
        UpdateTractorLink(true, false);
        vehicle->m_vehicleAudio.AddAudioEvent(AE_TRAILER_ATTACH, 0.0f);
        return true;
    }

    if (m_fTrailerTowedRatio > BAGGAGE_TRAILER_TOWED_RATIO) {
        m_fTrailerTowedRatio = 0.0f;
    }

    SetHeading(GetHeading());

    CVector towBarPos{}, towHitchPos{};
    if (!GetTowHitchPos(towHitchPos, true, this) || !vehicle->GetTowBarPos(towBarPos, true, this)) {
        return false;
    }

    towHitchPos -= GetPosition();

    SetPosn(towBarPos - towHitchPos);
    m_vecMoveSpeed = vehicle->m_vecMoveSpeed;
    PlaceOnRoadProperly();

    vehicle->m_vehicleAudio.AddAudioEvent(AE_TRAILER_ATTACH, 0.0f);
    return true;
}

// 0x6CF030
void CTrailer::ScanForTowLink() {
    if (m_pTowingVehicle || CVehicleRecording::IsPlaybackGoingOnForCar(this) || m_nModelIndex == MODEL_FARMTR1) {
        return;
    }

    CVector towHitchPos;
    if (!GetTowHitchPos(towHitchPos, false, this)) {
        return;
    }

    int16 count = 0;
    std::array<CEntity*, 16> objects{};
    CWorld::FindObjectsInRange(towHitchPos, 10.0f, true, &count, int16(objects.size()), objects.data(), false, true, false, false, false);

    for (const auto& entity : std::span{ objects.data(), (size_t)count }) {
        if (auto* vehicle = entity->AsVehicle()) {
            if (vehicle == this) {
                continue;
            }

            CVector towBarPos;
            if (!vehicle->GetTowBarPos(towBarPos, false, this)) {
                continue;
            }

            if (DistanceBetweenPoints2D(towBarPos, towHitchPos) < RELINK_TRAILER_DIFF_LIMIT_XY && std::fabs(towHitchPos.z - towBarPos.z) < RELINK_TRAILER_DIFF_LIMIT_Z) {
                SetTowLink(vehicle, false);
                return;
            }
        }
    }
}

// 0x6CEE50
void CTrailer::ResetSuspension() {
    CAutomobile::ResetSuspension();
    rng::fill(m_supportRatios, 1.f);
    if (m_pTowingVehicle && m_fTrailerTowedRatio > BAGGAGE_TRAILER_TOWED_RATIO) {
        m_fTrailerTowedRatio = 0.0f;
    } else {
        m_fTrailerTowedRatio = 1.0f;
    }
}

// 0x6CF6A0
void CTrailer::ProcessSuspension() {
    CAutomobile::ProcessSuspension();
    if (m_fTrailerTowedRatio == BAGGAGE_TRAILER_TOWED_RATIO) {
        return;
    }

    CVector contactPoints[NUM_TRAILER_SUPPORTS]{};
    CVector contactSpeeds[NUM_TRAILER_SUPPORTS]{};
    float impulseMagnitudes[NUM_TRAILER_SUPPORTS]{};

    for (int32 i = 0; i < NUM_TRAILER_SUPPORTS; ++i) {
        if (m_supportRatios[i] < m_fTrailerTowedRatio2 && m_fTrailerTowedRatio2 > 0.1f) {
            CVector forceDir = m_matrix->GetDown();

            contactPoints[i] = m_supportCPs[i].m_vecPoint - GetPosition();

            const float compression = m_supportRatios[i] / m_fTrailerTowedRatio2;

            CPhysical::ApplySpringCollisionAlt(m_fTrailerSuspensionForce, forceDir, contactPoints[i], compression, 1.0f, m_supportCPs[i].m_vecNormal, impulseMagnitudes[i]);

            contactSpeeds[i] = GetSpeed(contactPoints[i]);

            CVector dampeningDir = m_supportCPs[i].m_vecNormal.z > 0.35f 
                ? -m_supportCPs[i].m_vecNormal 
                : m_matrix->GetDown();

            CPhysical::ApplySpringDampening(m_fTrailerDampingForce, impulseMagnitudes[i], dampeningDir, contactPoints[i], contactSpeeds[i]);
        }
    }

    const auto status = GetStatus();
    if (status != STATUS_IS_TOWED && status != STATUS_IS_SIMPLE_TOWED) {
        eCarWheelStatus originalWheelStatus[NUM_TRAILER_SUPPORTS];
        for (int32 i = 0; i < NUM_TRAILER_SUPPORTS; ++i) {
            originalWheelStatus[i] = m_damageManager.GetWheelStatus((eCarWheel)i);

            if (m_supportRatios[i] >= m_fTrailerTowedRatio2 || m_fTrailerTowedRatio2 <= 0.1f) {
                m_damageManager.SetWheelStatus((eCarWheel)i, eCarWheelStatus::WHEEL_STATUS_BURST);
            } else {
                contactSpeeds[i] = GetSpeed(contactPoints[i]);
                m_damageManager.SetWheelStatus((eCarWheel)i, eCarWheelStatus::WHEEL_STATUS_OK);
            }
        }

        const float traction = flt_C1CB28 * m_pHandlingData->m_fTractionMultiplier * 0.25f;
        CAutomobile::ProcessCarWheelPair(CAR_WHEEL_FRONT_LEFT, CAR_WHEEL_REAR_LEFT, 0.0f, contactSpeeds, contactPoints, traction, 0.0f, 1000.0f, true);

        m_damageManager.SetWheelStatus(CAR_WHEEL_FRONT_LEFT, originalWheelStatus[0]);
        m_damageManager.SetWheelStatus(CAR_WHEEL_REAR_LEFT, originalWheelStatus[1]);
    }
}

// 0x6CFFD0
int32 CTrailer::ProcessEntityCollision(CEntity* entity, CColPoint* outColPoints) {
    if (m_fTrailerTowedRatio == -1000.f) {
        return CAutomobile::ProcessEntityCollision(entity, outColPoints);
    }

    if (m_nStatus != STATUS_SIMPLE) {
        vehicleFlags.bVehicleColProcessed = true;
    }

    const auto tcd = GetColData(),
               ocd = entity->GetColData();

#ifdef FIX_BUGS // Text search for `FIX_BUGS@CAutomobile::ProcessEntityCollision:1`
    if (!tcd || !ocd) {
        return 0;
    }
#endif

    if (physicalFlags.bSkipLineCol || physicalFlags.bProcessingShift || entity->IsPed() || entity->IsVehicle()) {
        tcd->m_nNumLines = 0; // Later reset back to original value
    }

    // Hide triangles in some cases
    const auto shouldHideTriangles = m_pTowingVehicle == entity || m_pVehicleBeingTowed == entity;
    const auto originalTNumTri = tcd->m_nNumTriangles, // Saving my sanity here, and unconditionally assigning
               originalONumTri = ocd->m_nNumTriangles;
    if (shouldHideTriangles) {
        tcd->m_nNumTriangles = ocd->m_nNumTriangles = 0;
    }

    std::array<CColPoint, NUM_TRAILER_SUSP_LINES> suspLineCPs{};
    std::array<float, NUM_TRAILER_SUSP_LINES> suspLineTouchDists{};

    rng::copy(m_fWheelsSuspensionCompression, suspLineTouchDists.begin());
    rng::copy(m_supportRatios, suspLineTouchDists.begin() + m_fWheelsSuspensionCompression.size());

    const auto numColPts = CCollision::ProcessColModels(
        GetMatrix(), *GetColModel(),
        entity->GetMatrix(), *entity->GetColModel(),
        *(std::array<CColPoint, 32>*)(outColPoints),
        suspLineCPs.data(),
        suspLineTouchDists.data(),
        false
    );

    // Restore hidden triangles
    if (shouldHideTriangles) {
        tcd->m_nNumTriangles = originalTNumTri;
        ocd->m_nNumTriangles = originalONumTri;
    }

    size_t numProcessedLines{};
    if (tcd->m_nNumLines) {
        // Process the real wheel's susp lines
        for (auto i = 0; i < NUM_TRAILER_WHEELS; i++) {
            // 0x6AD0D4
            const auto& cp = suspLineCPs[i];
            const auto touchDist = suspLineTouchDists[i];
            if (touchDist < BILLS_EXTENSION_LIMIT && touchDist < m_fWheelsSuspensionCompression[i]) {
                numProcessedLines++;
                m_fWheelsSuspensionCompression[i] = touchDist;
                m_wheelColPoint[i] = cp;
                m_anCollisionLighting[i] = cp.m_nLightingB;
                m_nContactSurface = cp.m_nSurfaceTypeB;

                switch (entity->GetType()) {
                case ENTITY_TYPE_VEHICLE:
                case ENTITY_TYPE_OBJECT: {
                    CEntity::ChangeEntityReference(m_apWheelCollisionEntity[i], entity->AsPhysical());

                    m_vWheelCollisionPos[i] = cp.m_vecPoint - entity->GetPosition();
                    if (entity->IsVehicle()) {
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

            // Process trailer support lines
            for (auto i = 0; i < NUM_TRAILER_SUPPORTS; i++) {
                const auto suspLineIdx = NUM_TRAILER_WHEELS + i;

                const auto touchDist = suspLineTouchDists[suspLineIdx];
                if (touchDist >= BILLS_EXTENSION_LIMIT || touchDist >= m_supportRatios[i]) {
                    continue;
                }

                numProcessedLines++;
                m_supportRatios[i] = touchDist;
                m_supportCPs[i] = suspLineCPs[suspLineIdx];
            }
        }
    } else {
        tcd->m_nNumLines = NUM_TRAILER_SUSP_LINES;
    }

    if (numColPts > 0 || numProcessedLines > 0) {
        AddCollisionRecord(entity);
        if (!entity->IsBuilding()) {
            entity->AsPhysical()->AddCollisionRecord(this);
        }
        if (numColPts > 0 && entity->IsBuilding()
            || (entity->IsObject() && entity->AsPhysical()->physicalFlags.bDisableCollisionForce)) {
            m_bHasHitWall = true;
        }
    }

    return numColPts;
}

// 0x6CED20
void CTrailer::ProcessControl() {
    CAutomobile::ProcessControl();
    if ((m_nFlags & 0x40) == 0 && m_fTrailerTowedRatio != BAGGAGE_TRAILER_TOWED_RATIO) {
        const float timeStep = CTimer::GetTimeStep();
        float& currentExtension = m_fTrailerTowedRatio;

        if (m_pTowingVehicle && currentExtension > 0.0f) {
            // up
            currentExtension = std::max(currentExtension - timeStep * TRAILER_SUPPORT_RETRACTION_RATE, 0.0f);
        } else if (!m_pTowingVehicle && currentExtension < 1.0f) {
            // down
            const float growthRate = (currentExtension <= 0.1f)
                ? timeStep * TRAILER_SUPPORT_WAIT_EXTENSION_RATE
                : timeStep * TRAILER_SUPPORT_EXTENSION_RATE;
            currentExtension = std::min(currentExtension + growthRate, 1.0f);
            m_nFakePhysics = 0;
        }

        const auto status = GetStatus();
        if ((status == STATUS_IS_TOWED || status == STATUS_IS_SIMPLE_TOWED) && TRAILER_TOWED_MINRATIO > currentExtension) {
            m_fTrailerTowedRatio2 = TRAILER_TOWED_MINRATIO;
        } else {
            m_fTrailerTowedRatio2 = currentExtension;
        }
    
        rng::fill(m_supportRatios, m_fTrailerTowedRatio2);
    }
}

// 0x6CF590
bool CTrailer::ProcessAI(uint32& extraHandlingFlags) {
    const auto returnValue = CAutomobile::ProcessAI(extraHandlingFlags);

    if (GetStatus() == STATUS_ABANDONED && (m_fTrailerTowedRatio > 0.5f || m_fTrailerTowedRatio == BAGGAGE_TRAILER_TOWED_RATIO)) {
        ScanForTowLink();
        return returnValue;
    }

    if (GetStatus() != STATUS_REMOTE_CONTROLLED || !m_pTowingVehicle) {
        return returnValue;
    }

    if (m_fTrailerTowedRatio != BAGGAGE_TRAILER_TOWED_RATIO) {
        m_BrakePedal = m_pTowingVehicle->m_BrakePedal;
        return returnValue;
    }

    // Wheel turning for luggage trailers
    float headingTow = m_pTowingVehicle->GetHeading();

    float headingSelf = GetHeading();

    // Calculation of the shortest angle difference
    float angleDiff = headingTow - headingSelf;
    if (angleDiff > PI) {
        angleDiff -= TWO_PI;
    } else if (angleDiff < -PI) {
        angleDiff += TWO_PI;
    }
    m_fSteerAngle = angleDiff;

    return returnValue;
}

// 0x6CFAC0
void CTrailer::PreRender() {
    CAutomobile::PreRender();

    // Support animation
    if (m_aCarNodes[TRAILER_MISC_A]) {
        auto* colData = CModelInfo::GetModelInfo(m_nModelIndex)->m_pColModel->m_pColData;
        if (colData->m_nNumLines > NUM_TRAILER_WHEELS && (!m_vecMoveSpeed.IsZero() || !m_vecTurnSpeed.IsZero() || !m_nFakePhysics)) {
            CMatrix mat;
            mat.Attach(&m_aCarNodes[TRAILER_MISC_A]->modelling, false);

            const float avgSupportRatio       = (m_supportRatios[0] + m_supportRatios[1]) * 0.5f;
            const float supportExtensionRatio = std::min(avgSupportRatio, m_fTrailerTowedRatio);

            const auto& supportLegLine = colData->m_pLines[NUM_TRAILER_WHEELS];

            const float supportTravel = supportExtensionRatio * (supportLegLine.m_vecEnd.z - supportLegLine.m_vecStart.z);
            const float verticalPosRatio = mat.GetPosition().y / supportLegLine.m_vecStart.y;

            CVector newPos = mat.GetPosition();
            const float targetZ = (supportTravel + supportLegLine.m_vecStart.z) * verticalPosRatio
                - (1.0f - verticalPosRatio) * m_fFrontHeightAboveRoad;
            newPos.z = std::min(targetZ, m_fHeight);

            mat.SetTranslate(newPos);
            mat.UpdateRW();
        }
    }

    // Animation for FARMTR1
    if (m_nModelIndex == MODEL_FARMTR1) {
        m_fPropRotate = fmod(m_fPropRotate, TWO_PI);
        const float forwardSpeed = m_vecMoveSpeed.Dot(m_matrix->GetForward());
        const float rotationSpeedFactor = std::clamp(forwardSpeed, 0.0f, 0.1f);
        m_fPropRotate -= rotationSpeedFactor * CTimer::GetTimeStep();
        CVehicle::SetComponentRotation(m_aCarNodes[TRAILER_MISC_B], AXIS_X, m_fPropRotate, true);
    }
}

// 0x6CEEA0
bool CTrailer::GetTowHitchPos(CVector& outPos, bool bCheckModelInfo, CVehicle* vehicle) {
    auto mi = GetVehicleModelInfo();
    outPos = mi->GetModelDummyPosition(DUMMY_TRAILER_ATTACH);

    if (vehicle && vehicle->m_nModelIndex == MODEL_TOWTRUCK) {
        return false;
    }

    if (outPos.IsZero()) {
        if (!bCheckModelInfo) {
            return false;
        }
        outPos.x = 0.0f;
        outPos.y = mi->GetColModel()->GetBoundingBox().m_vecMax.y + 1.0f;
        outPos.z = 0.5f - m_fFrontHeightAboveRoad;
        outPos = m_matrix->TransformPoint(outPos);
        return true;
    }
    outPos = m_matrix->TransformPoint(outPos);
    return true;
}

// 0x6CFD60
bool CTrailer::GetTowBarPos(CVector& outPos, bool bCheckModelInfo, CVehicle* vehicle) {
    if (!bCheckModelInfo) {
        if (m_nModelIndex != MODEL_BAGBOXA && m_nModelIndex != MODEL_BAGBOXB) {
            return false;
        }
    }

    auto mi = GetVehicleModelInfo();
    outPos.x = 0.0f;
    outPos.y = mi->GetColModel()->GetBoundingBox().m_vecMin.y - TRAILER_TOWBAR_OFFSET_Y;
    outPos.z = 0.5f - m_fFrontHeightAboveRoad;
    outPos = m_matrix->TransformPoint(outPos);
    return true;
}

// 0x6CEFB0
bool CTrailer::BreakTowLink() {
    if (m_pTowingVehicle) {
        m_pTowingVehicle->m_vehicleAudio.AddAudioEvent(AE_TRAILER_DETACH, 0.0f);
    }

    if (m_pTowingVehicle) {
        CEntity::ClearReference(m_pTowingVehicle->m_pVehicleBeingTowed);
        CEntity::ClearReference(m_pTowingVehicle);
    }

    const auto status = GetStatus();
    if (status != STATUS_IS_TOWED && status != STATUS_IS_SIMPLE_TOWED) {
        return false;
    }

    SetStatus(STATUS_ABANDONED);
    return true;
}
