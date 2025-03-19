#include "StdInc.h"

#include "Trailer.h"
#include "VehicleRecording.h"

static float TRAILER_TOWED_MINRATIO       = 0.9f; // 0x8D346C
static float RELINK_TRAILER_DIFF_LIMIT_XY = 0.4f; // 0x8D3470
static float RELINK_TRAILER_DIFF_LIMIT_Z  = 1.0f; // 0x8D3474
static float m_fTrailerSuspensionForce    = 1.5f; // 0x8D3464
static float m_fTrailerDampingForce       = 0.1f; // 0x8D3468

void CTrailer::InjectHooks() {
    RH_ScopedVirtualClass(CTrailer, 0x871c28, 71);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6D03A0);

    RH_ScopedInstall(ScanForTowLink, 0x6CF030);

    RH_ScopedVMTInstall(SetupSuspensionLines, 0x6CF1A0, { .reversed = false });
    RH_ScopedVMTInstall(SetTowLink, 0x6CFDF0);
    RH_ScopedVMTInstall(ResetSuspension, 0x6CEE50);
    RH_ScopedVMTInstall(ProcessSuspension, 0x6CF6A0, { .reversed = false });
    RH_ScopedVMTInstall(ProcessEntityCollision, 0x6CFFD0);
    RH_ScopedVMTInstall(ProcessControl, 0x6CED20, { .reversed = false });
    RH_ScopedVMTInstall(ProcessAI, 0x6CF590, { .reversed = false });
    RH_ScopedVMTInstall(PreRender, 0x6CFAC0, { .reversed = false });
    RH_ScopedVMTInstall(GetTowHitchPos, 0x6CEEA0);
    RH_ScopedVMTInstall(GetTowBarPos, 0x6CFD60);
    RH_ScopedVMTInstall(BreakTowLink, 0x6CEFB0);
}

// 0x6D03A0
CTrailer::CTrailer(int32 modelIndex, eVehicleCreatedBy createdBy) : CAutomobile(modelIndex, createdBy, false) {
    m_nVehicleSubType = VEHICLE_TYPE_TRAILER;

    if (m_nModelIndex == MODEL_BAGBOXA || m_nModelIndex == MODEL_BAGBOXB)
        m_fTrailerTowedRatio = -1000.0f;

    CTrailer::SetupSuspensionLines();

    m_nStatus = eEntityStatus::STATUS_ABANDONED;
}

// 0x6CF1A0
void CTrailer::SetupSuspensionLines() {
    plugin::CallMethod<0x6CF1A0, CTrailer*>(this);
}

// 0x6CFDF0
bool CTrailer::SetTowLink(CVehicle* vehicle, bool setMyPosToTowBar) {
    if (!vehicle) {
        return false;
    }

    if (m_nStatus != STATUS_PHYSICS && m_nStatus != STATUS_IS_TOWED && m_nStatus != STATUS_ABANDONED) {
        return false;
    }

    m_nStatus = STATUS_IS_TOWED;

    m_pTowingVehicle = vehicle;
    m_pTowingVehicle->RegisterReference(m_pTowingVehicle);

    vehicle->m_pVehicleBeingTowed = this;
    vehicle->m_pVehicleBeingTowed->RegisterReference(vehicle->m_pVehicleBeingTowed);

    CPhysical::RemoveFromMovingList();
    vehicle->RemoveFromMovingList();

    AddToMovingList();
    vehicle->AddToMovingList();

    vehicle->m_pVehicleBeingTowed->vehicleFlags.bLightsOn = true; // NOTSA | FIX_BUGS

    if (!setMyPosToTowBar) {
        UpdateTractorLink(true, false);
        vehicle->m_vehicleAudio.AddAudioEvent(AE_TRAILER_ATTACH, 0.0f);
        return true;
    }

    if (m_fTrailerTowedRatio > -1000.0f) {
        m_fTrailerTowedRatio = 0.0f;
    }

    SetHeading(GetHeading());

    auto towBarPos = CVector{};
    auto towHitchPos = CVector{};
    if (!GetTowHitchPos(towHitchPos, true, this) || !vehicle->GetTowBarPos(towBarPos, true, this))
        return false;

    towHitchPos -= GetPosition();

    SetPosn(towBarPos - towHitchPos);
    m_vecMoveSpeed = vehicle->m_vecMoveSpeed;
    PlaceOnRoadProperly();

    vehicle->m_vehicleAudio.AddAudioEvent(AE_TRAILER_ATTACH, 0.0f);
    return true;
}

// 0x6CF030
void CTrailer::ScanForTowLink() {
    if (m_pTowingVehicle || CVehicleRecording::IsPlaybackGoingOnForCar(this) || m_nModelIndex == MODEL_FARMTR1)
        return;

    CVector towHitchPos;
    if (!GetTowHitchPos(towHitchPos, false, this))
        return;

    int16 count = 0;
    CEntity* objects[16]{};
    CWorld::FindObjectsInRange(towHitchPos, 10.0f, true, &count, int16(std::size(objects)), objects, false, true, false, false, false);

    CVector towBarPos;
    CVehicle* vehicle = nullptr;
    for (auto& obj : std::span{ objects, (size_t)count }) {
        vehicle = obj->AsVehicle();
        if (vehicle == this) {
            continue;
        }

        if (!vehicle->GetTowBarPos(towBarPos, false, this)) {
            continue;
        }

        const auto dist = DistanceBetweenPoints2D(towBarPos, towHitchPos);
        if (dist < RELINK_TRAILER_DIFF_LIMIT_XY && std::fabs(towHitchPos.z - towBarPos.z) < RELINK_TRAILER_DIFF_LIMIT_Z) {
            SetTowLink(vehicle, false);
            return;
        }
    }
}

// 0x6CEE50
void CTrailer::ResetSuspension() {
    CAutomobile::ResetSuspension();
    rng::fill(m_supportRatios, 1.f);
    if (m_pTowingVehicle && m_fTrailerTowedRatio > -1000.0f)
        m_fTrailerTowedRatio = 0.0f;
    else
        m_fTrailerTowedRatio = 1.0f;
}

// 0x6CF6A0
void CTrailer::ProcessSuspension() {
    plugin::CallMethod<0x6CF6A0, CTrailer*>(this);
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
    const auto didHideTriangles = m_pTowingVehicle == entity || m_pVehicleBeingTowed == entity;
    const auto tNumTri = tcd->m_nNumTriangles, // Saving my sanity here, and unconditionally assigning
               oNumTri = ocd->m_nNumTriangles;
    if (didHideTriangles) {
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
    if (didHideTriangles) {
        tcd->m_nNumTriangles = tNumTri;
        ocd->m_nNumTriangles = oNumTri;
    }

    size_t numProcessedLines{};
    if (tcd->m_nNumLines) {
        // Process the real wheel's susp lines
        for (auto i = 0; i < MAX_CARWHEELS; i++) {
            // 0x6AD0D4
            const auto& cp = suspLineCPs[i];

            const auto touchDist = suspLineTouchDists[i];
            if (touchDist >= 1.f || touchDist >= m_fWheelsSuspensionCompression[i]) {
                continue;
            }

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
            if (touchDist >= 1.f || touchDist >= m_supportRatios[i]) {
                continue;
            }

            numProcessedLines++;
            m_supportRatios[i] = touchDist;
            m_supportCPs[i] = suspLineCPs[suspLineIdx];
        }
    } else {
        tcd->m_nNumLines = NUM_TRAILER_SUSP_LINES;
    }

    if (numColPts > 0 || numProcessedLines > 0) {
        AddCollisionRecord(entity);
        if (!entity->IsBuilding()) {
            entity->AsPhysical()->AddCollisionRecord(this);
        }
        if (numColPts > 0) {
            if (   entity->IsBuilding()
                || (entity->IsObject() && entity->AsPhysical()->physicalFlags.bDisableCollisionForce)
            ) {
                m_bHasHitWall = true;
            }
        }
    }

    return numColPts;
}

// 0x6CED20
void CTrailer::ProcessControl() {
    plugin::CallMethod<0x6CED20, CTrailer*>(this);
}

// 0x6CF590
bool CTrailer::ProcessAI(uint32& extraHandlingFlags) {
    return plugin::CallMethodAndReturn<bool, 0x6CF590, CTrailer*, uint32&>(this, extraHandlingFlags);
}

// 0x6CFAC0
void CTrailer::PreRender() {
    plugin::CallMethod<0x6CFAC0, CTrailer*>(this);
}

// 0x6CEEA0
bool CTrailer::GetTowHitchPos(CVector& outPos, bool bCheckModelInfo, CVehicle* vehicle) {
    auto mi = GetVehicleModelInfo();
    outPos = mi->GetModelDummyPosition(DUMMY_TRAILER_ATTACH);
    if (vehicle && vehicle->m_nModelIndex == MODEL_TOWTRUCK) {
        return false;
    }

    if (outPos.IsZero()) {
        if (bCheckModelInfo) {
            outPos.x = 0.0f;
            outPos.y = mi->GetColModel()->GetBoundingBox().m_vecMax.y + 1.0f;
            outPos.z = 0.5f - m_fFrontHeightAboveRoad;
            outPos = m_matrix->TransformPoint(outPos);
            return true;
        }
        return false;
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
    outPos.y = mi->GetColModel()->GetBoundingBox().m_vecMin.y - -0.05f;
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

    if (m_nStatus != STATUS_IS_TOWED && m_nStatus != STATUS_IS_SIMPLE_TOWED) {
        return false;
    }

    m_nStatus = STATUS_ABANDONED;
    return true;
}
