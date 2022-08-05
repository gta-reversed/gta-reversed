#include "StdInc.h"

#include "Trailer.h"
#include "VehicleRecording.h"

static float TRAILER_TOWED_MINRATIO       = 0.9f; // 0x8D346C
static float RELINK_TRAILER_DIFF_LIMIT_XY = 0.4f; // 0x8D3470
static float RELINK_TRAILER_DIFF_LIMIT_Z  = 1.0f; // 0x8D3474
static float m_fTrailerSuspensionForce    = 1.5f; // 0x8D3464
static float m_fTrailerDampingForce       = 0.1f; // 0x8D3468

void CTrailer::InjectHooks() {
    RH_ScopedClass(CTrailer);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6D03A0);
    // RH_ScopedInstall(SetupSuspensionLines, 0x6CF1A0);
    RH_ScopedInstall(SetTowLink, 0x6CFDF0);
    RH_ScopedInstall(ScanForTowLink, 0x6CF030);
    RH_ScopedInstall(ResetSuspension, 0x6CEE50);
    // RH_ScopedInstall(ProcessSuspension, 0x6CF6A0);
    // RH_ScopedInstall(ProcessEntityCollision, 0x6CFFD0);
    // RH_ScopedInstall(ProcessControl, 0x6CED20);
    // RH_ScopedInstall(ProcessAI, 0x6CF590);
    // RH_ScopedInstall(PreRender, 0x6CFAC0);
    RH_ScopedInstall(GetTowHitchPos, 0x6CEEA0);
    RH_ScopedInstall(GetTowBarPos, 0x6CFD60);
    RH_ScopedInstall(BreakTowLink, 0x6CEFB0);
}

// 0x6D03A0
CTrailer::CTrailer(int32 modelIndex, eVehicleCreatedBy createdBy) : CAutomobile(modelIndex, createdBy, false) {
    m_fTrailerColPointValue1 = 1.0f;
    m_fTrailerColPointValue2 = 1.0f;
    m_fTrailerTowedRatio     = 1.0f;
    m_fTrailerTowedRatio2    = 1.0f;

    m_nVehicleSubType = VEHICLE_TYPE_TRAILER;

    if (m_nModelIndex == MODEL_BAGBOXA || m_nModelIndex == MODEL_BAGBOXB)
        m_fTrailerTowedRatio = -1000.0f;

    SetupSuspensionLines(); // V1053 Calling the 'SetupSuspensionLines' virtual function in the constructor may lead to unexpected result at runtime.

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

    if (m_nStatus != STATUS_PHYSICS && m_nStatus != STATUS_REMOTE_CONTROLLED && m_nStatus != STATUS_ABANDONED) {
        return false;
    }

    m_nStatus = STATUS_REMOTE_CONTROLLED;

    m_pTractor = vehicle;
    m_pTractor->RegisterReference(m_pTractor);

    vehicle->m_pTrailer = this;
    vehicle->m_pTrailer->RegisterReference(vehicle->m_pTrailer);

    CPhysical::RemoveFromMovingList();
    vehicle->RemoveFromMovingList();

    AddToMovingList();
    vehicle->AddToMovingList();

    vehicle->m_pTrailer->vehicleFlags.bLightsOn = true; // NOTSA | FIX_BUGS

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
    if (m_pTractor || CVehicleRecording::IsPlaybackGoingOnForCar(this) || m_nModelIndex == MODEL_FARMTR1)
        return;

    CVector towHitchPos;
    if (!GetTowHitchPos(towHitchPos, false, this))
        return;

    int16 count = 0;
    CEntity* objects[16]{};
    CWorld::FindObjectsInRange(&towHitchPos, 10.0f, true, &count, int16(std::size(objects)), objects, false, true, false, false, false);

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
    m_fTrailerColPointValue1 = 1.0f;
    m_fTrailerColPointValue2 = 1.0f;
    if (m_pTractor && m_fTrailerTowedRatio > -1000.0f)
        m_fTrailerTowedRatio = 0.0f;
    else
        m_fTrailerTowedRatio = 1.0f;
}

// 0x6CF6A0
void CTrailer::ProcessSuspension() {
    plugin::CallMethod<0x6CF6A0, CTrailer*>(this);
}

// 0x6CFFD0
int32 CTrailer::ProcessEntityCollision(CEntity* entity, CColPoint* colPoint) {
    return plugin::CallMethodAndReturn<int32, 0x6CFFD0, CTrailer*, CEntity*, CColPoint*>(this, entity, colPoint);
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
            outPos = MultiplyMatrixWithVector(*m_matrix, outPos);
            return true;
        }
        return false;
    }
    outPos = MultiplyMatrixWithVector(*m_matrix, outPos);
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
    outPos = MultiplyMatrixWithVector(*m_matrix, outPos);
    return true;
}

// 0x6CEFB0
bool CTrailer::BreakTowLink() {
    if (m_pTractor) {
        m_pTractor->m_vehicleAudio.AddAudioEvent(AE_TRAILER_DETACH, 0.0f);
    }

    if (m_pTractor) {
        CEntity::ClearReference(m_pTractor->m_pTrailer);
        CEntity::ClearReference(m_pTractor);
    }

    if (m_nStatus != STATUS_REMOTE_CONTROLLED && m_nStatus != STATUS_PLAYER_DISABLED) {
        return false;
    }

    m_nStatus = STATUS_ABANDONED;
    return true;
}
