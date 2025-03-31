#include "StdInc.h"

#include "cHandlingDataMgr.h"

char (&VehicleNames)[210][14] = *(char(*)[210][14])0x8D3978;

//! 0xC2B9B4 - Used for acceleration conversion from file to game units
constexpr auto ACCEL_CONST = 1.f / sq(50.f); // This number 50 seems to be coming up a lot...;

// 0xC2B9BC - Used for velocity conversion from file to game units
constexpr auto VELOCITY_CONST = 0.277778f / 50.f;

void cHandlingDataMgr::InjectHooks() {
    RH_ScopedClass(cHandlingDataMgr);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(LoadHandlingData, 0x5BD830);
    RH_ScopedInstall(GetFlyingPointer, 0x6F52D0);
    RH_ScopedInstall(GetBoatPointer, 0x6F5300);
    RH_ScopedInstall(HasFrontWheelDrive, 0x6A0480);
    RH_ScopedInstall(HasRearWheelDrive, 0x6A04B0);
    RH_ScopedInstall(GetHandlingId, 0x6F4FD0);
    RH_ScopedInstall(ConvertDataToWorldUnits, 0x6F5010);
    RH_ScopedInstall(ConvertDataToGameUnits, 0x6F5080);
    RH_ScopedInstall(ConvertBikeDataToWorldUnits, 0x6F5240);
    RH_ScopedInstall(ConvertBikeDataToGameUnits, 0x6F5290);
    RH_ScopedInstall(FindExactWord, 0x006F4F30);
};

// NOTSA: Helper
int32 tHandlingData::InitFromData(int32 id, const char* line) {
    m_nVehicleId = id;

    const auto n = sscanf_s(
        line,
        "%*s\t%f\t%f\t%f\t%f\t%f\t%f\t%hhu\t%f\t%f\t%f\t%hhu\t%f\t%f\t%f\t%c\t%c\t%f\t%f\t%hhu\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\t%x\t%x\t%hhu\t%hhu\t%hhu",
        &m_fMass, // 1
        &m_fTurnMass,
        &m_fDragMult,

        &m_vecCentreOfMass.x, // 4
        &m_vecCentreOfMass.y,
        &m_vecCentreOfMass.z,

        &m_nPercentSubmerged, // 7

        &m_fTractionMultiplier, // 8
        &m_fTractionLoss,
        &m_fTractionBias,

        &m_transmissionData.m_nNumberOfGears, // 11
        &m_transmissionData.m_MaxVelocity,
        &m_transmissionData.m_EngineAcceleration,
        &m_transmissionData.m_EngineInertia,
        &m_transmissionData.m_nDriveType, 1u,
        &m_transmissionData.m_nEngineType, 1u,

        &m_fBrakeDeceleration, // 18
        &m_fBrakeBias,
        &m_bABS,
        &m_fSteeringLock,
        &m_fSuspensionForceLevel,

        &m_fSuspensionDampingLevel, // 22
        &m_fSuspensionHighSpdComDamp,
        &m_fSuspensionUpperLimit,
        &m_fSuspensionLowerLimit,
        &m_fSuspensionBiasBetweenFrontAndRear,
        &m_fSuspensionAntiDiveMultiplier,

        &m_fSeatOffsetDistance,
        &m_fCollisionDamageMultiplier,

        &m_nMonetaryValue, // 31
        &m_nModelFlags,
        &m_nHandlingFlags,
        &m_nFrontLights,
        &m_nRearLights,
        &m_nAnimGroup
    );
    m_transmissionData.m_handlingFlags = m_nHandlingFlags;
    m_transmissionData.m_EngineAcceleration *= 0.4f;
    gHandlingDataMgr.ConvertDataToGameUnits(this);
    return n == 35 ? -1 : n;
}

// NOTSA: Helper
int32 tBoatHandlingData::InitFromData(int32 id, const char* line) {
    m_nVehicleId = id;

    const auto n = sscanf_s(
        line,
        "%*s\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f",
        &m_fThrustY,
        &m_fThrustZ,
        &m_fThrustAppZ,
        &m_fAqPlaneForce,
        &m_fAqPlaneLimit,
        &m_fAqPlaneOffset,
        &m_fWaveAudioMult,

        &m_vecMoveRes.x,
        &m_vecMoveRes.y,
        &m_vecMoveRes.z,

        &m_vecTurnRes.x,
        &m_vecTurnRes.y,
        &m_vecTurnRes.z,

        &m_fLookLRBehindCamHeight
    );
    return n == 14 ? -1 : n;
}

// NOTSA: Helper
int32 tFlyingHandlingData::InitFromData(int32 id, const char* line) {
    m_nVehicleId = id;

    const auto n = sscanf_s(
        line,
        "%*s\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f%*c\t%f\t%f\t%f\t%f\t%f\t%f\t%f",
        &m_fThrust, // 1                                            ^^^ there's an extra `s` for the `RCRAIDER` that we have to ignore
        &m_fThrustFallOff,
        &m_fYaw,
        &m_fYawStab,
        &m_fSideSlip, // 5
        &m_fRoll,
        &m_fRollStab,
        &m_fPitch,
        &m_fPitchStab,
        &m_fFormLift, // 10
        &m_fAttackLift,
        &m_fGearUpR,
        &m_fGearDownL,
        &m_fWindMult,
        &m_fMoveRes, // 15

        &m_vecTurnRes.x, // 16
        &m_vecTurnRes.y,
        &m_vecTurnRes.z,

        &m_vecSpeedRes.x, // 19
        &m_vecSpeedRes.y,
        &m_vecSpeedRes.z
    );
    return n == 21 ? -1 : n;
}

// NOTSA: Helper
int32 tBikeHandlingData::InitFromData(int32 id, const char* line) {
    m_nVehicleId = id;

    const auto n = sscanf_s(
        line,
        "%*s\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f",
        &m_fLeanFwdCOM,
        &m_fLeanFwdForce,
        &m_fLeanBakCOM,
        &m_fLeanBakForce,
        &m_fMaxLean,
        &m_fFullAnimLean,
        &m_fDesLean,
        &m_fSpeedSteer,
        &m_fSlipSteer,
        &m_fNoPlayerCOMz,
        &m_fWheelieAng,
        &m_fStoppieAng,
        &m_fWheelieSteer,
        &m_fWheelieStabMult,
        &m_fStoppieStabMult
    );
    gHandlingDataMgr.ConvertBikeDataToGameUnits(this);
    return n == 15 ? -1 : n;
}

// 0x5BD830
void cHandlingDataMgr::LoadHandlingData() {
    CFileMgr::SetDir("DATA");
    const auto file = CFileMgr::OpenFile("HANDLING.CFG", "rb");
    CFileMgr::SetDir("");

    // Automatically closes file on function return
    const notsa::ScopeGuard closeFile{ [&] { CFileMgr::CloseFile(file); } };

    auto nLoadedHandlings{ 0u };

    // Get handling by scanning name from `line` using `scanNameFormat`, then calls `GetHandlingFn` to get the handling ptr
    const auto DoLoadHandling = [&, this](const char* line, auto GetHandlingFn) -> int32 {
        char name[32]{};
        if (sscanf_s(line, "%31s", name, std::size(name)) != 1) { // FIX_BUGS: Sized string read
            return 0;
        }
        const auto id = FindExactWord(name, &VehicleNames[0][0], std::size(VehicleNames[0]), std::size(VehicleNames));
        if (id == -1) {
            return -1; // Issue logged by `FindExactWord`, so no need to care about it here
        }
        const auto ret = std::invoke(GetHandlingFn, this, id)->InitFromData(id, line);
        if (ret == -1) {
            nLoadedHandlings++;
        }
        return ret;
    };

    auto nline{1u}, nLoadedVehHandlings{0u};
    for (auto l = CFileLoader::LoadLine(file); l; l = CFileLoader::LoadLine(file), nline++) {
        if (!strcmp(l, ";the end")) { // End of data
            return;
        }

        // Each function returns the number of tokens read on error otherwise -1
        const auto ret = [&] {
            switch (l[0]) {
            case ';': return -1; // Comment
            case '!': return DoLoadHandling(l + 1, &cHandlingDataMgr::GetBikeHandlingPointer);
            case '$': return DoLoadHandling(l + 1, &cHandlingDataMgr::GetFlyingPointer);
            case '%': return DoLoadHandling(l + 1, &cHandlingDataMgr::GetBoatPointer);
            case '^': return CVehicleAnimGroupData::LoadAGroupFromData(l);
            default:  break;
            }
            nLoadedVehHandlings++;
            return DoLoadHandling(l, &cHandlingDataMgr::GetVehiclePointer);
        }();
        if (ret != -1) {
            NOTSA_UNREACHABLE("Failed to process handling. [Line: {}; #Tokens Read: {}]", nline, ret);
        }
    }
    DEV_LOG("Successfully loaded {}x handlings for {}x vehicles!", nLoadedHandlings, nLoadedVehHandlings);
}

// 0x005BF3D0
void cHandlingDataMgr::Initialise() {
    LoadHandlingData();

    field_0 = 0.1f;
    fWheelFriction = 0.9f;
    field_8 = 1.0f;
    field_C = 0.8f;
    field_10 = 0.98f;
}

// 0x6F52D0
tFlyingHandlingData* cHandlingDataMgr::GetFlyingPointer(uint8 handlingId) {
    if (handlingId >= 186 && handlingId <= 209) {
        return &m_aFlyingHandling[handlingId - 186];
    }
    return &m_aFlyingHandling[0];
}

// 0x6F5300
tBoatHandlingData* cHandlingDataMgr::GetBoatPointer(uint8 handlingId) {
    if (handlingId >= 175u && handlingId <= 186u) {
        return &m_aBoatHandling[handlingId - 175u];
    }
    return &m_aBoatHandling[0];
}

tHandlingData* cHandlingDataMgr::GetVehiclePointer(uint32 handlingId) {
    return &m_aVehicleHandling[handlingId];
}

tBikeHandlingData* cHandlingDataMgr::GetBikeHandlingPointer(uint32 handlingId) {
    return &m_aBikeHandling[handlingId >= 162 ? handlingId - 162 : handlingId]; // NOTE: 162 = VT_BIKE
}

// 0x6A0480
bool cHandlingDataMgr::HasFrontWheelDrive(uint8 handlingId) {
    return GetVehiclePointer(handlingId)->GetTransmission().m_nDriveType != 'R';
};

// 0x6A04B0
bool cHandlingDataMgr::HasRearWheelDrive(uint8 handlingId) {
    return GetVehiclePointer(handlingId)->GetTransmission().m_nDriveType != 'F';
}

// get handling id by name
// 0x6F4FD0
int32 cHandlingDataMgr::GetHandlingId(const char* nameToFind) {
    for (auto [id, name] : rngv::enumerate(VehicleNames)) {
        if (!strcmp(name, nameToFind)) {
            return id;
        }
    }
    NOTSA_UNREACHABLE("Can't find {}", nameToFind);
}

// 0x6F5010
// update some handling variables with some world-specific multipliers
void cHandlingDataMgr::ConvertDataToWorldUnits(tHandlingData* h) {
    const auto t = &h->GetTransmission();

    t->m_MaxVelocity /= VELOCITY_CONST;
    h->m_fBrakeDeceleration /= ACCEL_CONST;
    t->m_EngineAcceleration *= (t->m_nDriveType == '4' ? 4.f : 2.f) / ACCEL_CONST;
    h->m_fCollisionDamageMultiplier *= h->m_fMass / 2000.f;
}

// update some handling variables with some game-specific multipliers
// 0x6F5080
void cHandlingDataMgr::ConvertDataToGameUnits(tHandlingData* h) {
    const auto t = &h->GetTransmission();

    t->m_EngineAcceleration *= ACCEL_CONST;
    t->m_MaxVelocity *= VELOCITY_CONST;
    h->m_fBrakeDeceleration *= ACCEL_CONST;
    h->m_fMassRecpr = 1.f / h->m_fMass;
    h->m_fBuoyancyConstant = h->m_fMass * 0.8f / (float)h->m_nPercentSubmerged;
    h->m_fCollisionDamageMultiplier *= h->m_fMassRecpr * 2000.f;

    auto engineAccelLimit = t->m_EngineAcceleration / 6.f;
    auto maxVelocity      = t->m_MaxVelocity;
    while (maxVelocity > 0.f) {
        maxVelocity -= 0.01f;
        if (h->m_fDragMult >= 0.01f) {
            if ((sq(maxVelocity) * (h->m_fDragMult / 2.f) / 1000.f) <= engineAccelLimit) {
                break;
            }
            continue;
        }

        auto fRecip = 1.f / (sq(maxVelocity) * h->m_fDragMult + 1.f);
        if ((fRecip - 1.f) * -maxVelocity <= engineAccelLimit) {
            break;
        }
    }

    if (h->m_nVehicleId == VT_RCBANDIT) {
        t->m_MaxFlatVelocity    = maxVelocity;
        t->m_MaxReverseVelocity = -maxVelocity;
    } else if (h->m_bUseMaxspLimit) {
        t->m_MaxFlatVelocity    = maxVelocity / 1.2f;
        t->m_MaxReverseVelocity = std::min(-t->m_MaxFlatVelocity / 4.f, -0.2f);
    } else {
        t->m_MaxVelocity     = maxVelocity * 1.2f;
        t->m_MaxFlatVelocity = maxVelocity;
        if (h->m_nVehicleId >= VT_BIKE && h->m_nVehicleId <= VT_FREEWAY) { 
            // 2 wheelers
            t->m_MaxReverseVelocity = -0.05f;
        } else {
            t->m_MaxReverseVelocity = std::min(-maxVelocity * 0.3f, -0.2f);
        }
    }

    t->m_EngineAcceleration /= (t->m_nDriveType == '4') ? 4.f : 2.f;
    t->InitGearRatios();
}

// 0x6F5240
void cHandlingDataMgr::ConvertBikeDataToWorldUnits(tBikeHandlingData* bikeHandling) {
    bikeHandling->m_fMaxLean = RadiansToDegrees(asin(bikeHandling->m_fMaxLean));
    bikeHandling->m_fFullAnimLean = RadiansToDegrees(bikeHandling->m_fFullAnimLean);
    bikeHandling->m_fWheelieAng = RadiansToDegrees(asin(bikeHandling->m_fWheelieAng));
    bikeHandling->m_fStoppieAng = RadiansToDegrees(asin(bikeHandling->m_fStoppieAng));
}

// 0x6F5290
void cHandlingDataMgr::ConvertBikeDataToGameUnits(tBikeHandlingData* bikeHandling) {
    bikeHandling->m_fMaxLean = sin(DegreesToRadians(bikeHandling->m_fMaxLean));
    bikeHandling->m_fFullAnimLean = DegreesToRadians(bikeHandling->m_fFullAnimLean);
    bikeHandling->m_fWheelieAng = sin(DegreesToRadians(bikeHandling->m_fWheelieAng));
    bikeHandling->m_fStoppieAng = sin(DegreesToRadians(bikeHandling->m_fStoppieAng));
}

// 0x006F4F30
int32 cHandlingDataMgr::FindExactWord(const char* name, const char* nameTable, uint32 entrySize, uint32 entryCount) {
    for (auto i = 0u; i < entryCount; i++) {
        const auto entry = &nameTable[entrySize * i];
        if (!strncmp(name, entry, strlen(entry))) {
            return i;
        }
    }
    DEV_LOG("Vehicle name not found in table: {}", name);
    return -1;
}
