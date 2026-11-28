#include "StdInc.h"

#include "Garage.h"

void CGarage::InjectHooks() {
    RH_ScopedClass(CGarage);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(BuildRotatedDoorMatrix, 0x4479F0);
    RH_ScopedInstall(TidyUpGarageClose, 0x449D10, { .Reversed = false });
    RH_ScopedInstall(TidyUpGarage, 0x449C50, { .Reversed = false });
    RH_ScopedInstall(StoreAndRemoveCarsForThisHideOut, 0x449900);
    RH_ScopedInstall(EntityHasASphereWayOutsideGarage, 0x449050, { .Reversed = false });
    RH_ScopedInstall(RemoveCarsBlockingDoorNotInside, 0x449690, { .Reversed = false });
    RH_ScopedInstall(IsEntityTouching3D, 0x448EE0, { .Reversed = false });
    RH_ScopedInstall(IsEntityEntirelyOutside, 0x448D30, { .Reversed = false });
    RH_ScopedInstall(IsStaticPlayerCarEntirelyInside, 0x44A830, { .Reversed = false });
    RH_ScopedInstall(IsEntityEntirelyInside3D, 0x448BE0, { .Reversed = false });
    RH_ScopedOverloadedInstall(IsPointInsideGarage, "CVector", 0x448740, bool(CGarage::*)(CVector), { .Reversed = false });
    RH_ScopedInstall(PlayerArrestedOrDied, 0x4486C0, { .Reversed = false });
    RH_ScopedInstall(OpenThisGarage, 0x447D50);
    RH_ScopedInstall(CloseThisGarage, 0x447D70);
    RH_ScopedInstall(InitDoorsAtStart, 0x447600, { .Reversed = false });
    RH_ScopedOverloadedInstall(IsPointInsideGarage, "CVector-float", 0x4487D0, bool(CGarage::*)(CVector, float), { .Reversed = false });
    RH_ScopedInstall(Update, 0x44AA50, { .Reversed = false });
    RH_ScopedInstall(IsGarageEmpty, 0x44A9C0, { .Reversed = false });
}

// 0x4479F0
void CGarage::BuildRotatedDoorMatrix(CEntity* entity, float fDoorPosition) {
    const auto fAngle = fDoorPosition * -HALF_PI;
    const auto fSin = sin(fAngle);
    const auto fCos = cos(fAngle);
    CMatrix& matrix = entity->GetMatrix();

    const auto& vecForward = matrix.GetForward();
    matrix.GetUp() = CVector(-fSin * vecForward.y, fSin * vecForward.x, fCos);
    matrix.GetRight() = CrossProduct(vecForward, matrix.GetUp());
}

// 0x449D10
void CGarage::TidyUpGarageClose() {
    plugin::CallMethod<0x449D10, CGarage*>(this);
}

// 0x449C50
void CGarage::TidyUpGarage() {
    plugin::CallMethod<0x449C50, CGarage*>(this);
}

// 0x449900
void CGarage::StoreAndRemoveCarsForThisHideOut(CStoredCar* storedCars, int32 maxSlot) {
    maxSlot = std::min<int32>(maxSlot, NUM_GARAGE_STORED_CARS);

    for (auto i = 0; i < NUM_GARAGE_STORED_CARS; i++)
        storedCars[i].Clear();

    auto pool = GetVehiclePool();
    auto storedCarIdx{0u};
    for (auto i = pool->GetSize(); i; i--) {
        if (auto vehicle = pool->GetAt(i - 1)) {
            if (IsPointInsideGarage(vehicle->GetPosition()) && vehicle->GetCreatedBy() != MISSION_VEHICLE) {
                if (storedCarIdx < static_cast<uint32>(maxSlot) && !EntityHasASphereWayOutsideGarage(vehicle, 1.0f)) {
                    storedCars[storedCarIdx++].StoreCar(vehicle);
                }

                FindPlayerInfo().CancelPlayerEnteringCars(vehicle);
                CWorld::Remove(vehicle);
                delete vehicle;
            }
        }
    }

    // Clear slots with no vehicles in it
    for (auto i = storedCarIdx; i < NUM_GARAGE_STORED_CARS; i++)
        storedCars[i].Clear();
}

// 0x449050
bool CGarage::EntityHasASphereWayOutsideGarage(CEntity* entity, float fRadius) {
    return plugin::CallMethodAndReturn<bool, 0x449050, CGarage*, CEntity*, float>(this, entity, fRadius);
}

// 0x449690
void CGarage::RemoveCarsBlockingDoorNotInside() {
    plugin::CallMethod<0x449690, CGarage*>(this);
}

// 0x448EE0
bool CGarage::IsEntityTouching3D(CEntity* entity) {
    return plugin::CallMethodAndReturn<bool, 0x448EE0, CGarage*, CEntity*>(this, entity);
}

// 0x448D30
bool CGarage::IsEntityEntirelyOutside(CEntity* entity, float radius) {
    return plugin::CallMethodAndReturn<bool, 0x448D30, CGarage*, CEntity*, float>(this, entity, radius);
}

// 0x44A830
bool CGarage::IsStaticPlayerCarEntirelyInside() {
    return plugin::CallMethodAndReturn<bool, 0x44A830, CGarage*>(this);
}

// 0x448BE0
bool CGarage::IsEntityEntirelyInside3D(CEntity* entity, float radius) {
    return plugin::CallMethodAndReturn<bool, 0x448BE0, CGarage*, CEntity*, float>(this, entity, radius);
}

// 0x448740
bool CGarage::IsPointInsideGarage(CVector point) {
    return plugin::CallMethodAndReturn<bool, 0x448740, CGarage*, CVector>(this, point);
}

// 0x4486C0
eGarageDoorState CGarage::PlayerArrestedOrDied() {
    return plugin::CallMethodAndReturn<eGarageDoorState, 0x4486C0, CGarage*>(this);
}

// 0x447D50
void CGarage::OpenThisGarage() {
  if ( m_nDoorState == GARAGE_DOOR_CLOSED
    || m_nDoorState == GARAGE_DOOR_CLOSING
    || m_nDoorState == GARAGE_DOOR_CLOSED_DROPPED_CAR)
  {
    m_nDoorState = GARAGE_DOOR_OPENING;
  }
}

// 0x447D70
void CGarage::CloseThisGarage() {
    if (m_nDoorState == GARAGE_DOOR_OPEN || m_nDoorState == GARAGE_DOOR_OPENING)
        m_nDoorState = GARAGE_DOOR_CLOSING;
}

// 0x447600
void CGarage::InitDoorsAtStart() {
    plugin::CallMethod<0x447600, CGarage*>(this);
}

// 0x4487D0
bool CGarage::IsPointInsideGarage(CVector point, float radius) {
    return plugin::CallMethodAndReturn<bool, 0x4487D0, CGarage*, CVector, float>(this, point, radius);
}

// 0x44AA50
void CGarage::Update(int32 garageId) {
    plugin::CallMethod<0x44AA50, CGarage*>(this, garageId);
}

bool CGarage::IsHideOut() const {
    switch (m_nType) {
    case eGarageType::SAFEHOUSE_GANTON:
    case eGarageType::SAFEHOUSE_SANTAMARIA:
    case eGarageType::SAGEHOUSE_ROCKSHORE:
    case eGarageType::SAFEHOUSE_FORTCARSON:
    case eGarageType::SAFEHOUSE_VERDANTMEADOWS:
    case eGarageType::SAFEHOUSE_DILLIMORE:
    case eGarageType::SAFEHOUSE_PRICKLEPINE:
    case eGarageType::SAFEHOUSE_WHITEWOOD:
    case eGarageType::SAFEHOUSE_PALOMINOCREEK:
    case eGarageType::SAFEHOUSE_REDSANDSWEST:
    case eGarageType::SAFEHOUSE_ELCORONA:
    case eGarageType::SAFEHOUSE_MULHOLLAND:
    case eGarageType::SAFEHOUSE_CALTONHEIGHTS:
    case eGarageType::SAFEHOUSE_PARADISO:
    case eGarageType::SAFEHOUSE_DOHERTY:
    case eGarageType::SAFEHOUSE_HASHBURY:
    case eGarageType::HANGAR_ABANDONED_AIRPORT:
        return true;
    default:
        return false;
    }
}

// 0x44A9C0
bool CGarage::IsGarageEmpty() {
    return plugin::CallMethodAndReturn<bool, 0x44A9C0, CGarage*>(this);

    CVector cornerA = { m_fLeftCoord, m_fFrontCoord, m_vPosn.z };
    CVector cornerB = { m_fRightCoord, m_fBackCoord, m_fTopZ   };

    int16 outCount[2];
    CEntity* outEntities[16];
    CWorld::FindObjectsIntersectingCube(cornerA, cornerB, outCount, static_cast<int16>(std::size(outEntities)), outEntities, false, true, true, false, false);
    if (outCount[0] <= 0)
        return true;

    int16 entityIndex = 0;

    while (!IsEntityTouching3D(outEntities[entityIndex])) {
        if (++entityIndex >= outCount[0])
            return true;
    }
    return false;
}

/*
void CGarage::CenterCarInGarage(CEntity* entity) {
    auto vehicle = FindPlayerVehicle();
    if (IsAnyOtherCarTouchingGarage(vehicle))
        return;

    auto player = FindPlayerPed();
    if (IsAnyOtherPedTouchingGarage(player))
        return;

    auto pos = entity->GetPosition();

    const auto halfX = (m_fRightCoord + m_fLeftCoord) * 0.5f;
    const auto halfY = (m_fBackCoord + m_fFrontCoord) * 0.5f;
    CVector p1{
        halfX - pos.x,
        halfY - pos.y,
        pos.z - pos.z
    };

    auto dist = p1.Magnitude();
    if (dist >= 0.4f) {
        auto x = halfX - pos.x * 0.4f / dist + pos.x;
        auto y = 0.4f / dist * halfY - pos.y + pos.y;
    } else {
        auto x = halfX;
        auto y = halfY;
    }

    if (!IsEntityEntirelyInside3D(entity, 0.3f))
        entity->SetPosn(entity->GetPosition());
}
*/

// 0x5D3020
void CSaveGarage::CopyGarageIntoSaveGarage(Const CGarage& g) {
    m_nType         = g.m_nType;
    m_nDoorState    = g.m_nDoorState;
    m_nFlags        = g.m_nFlags;
    m_vPosn         = g.m_vPosn;
    m_vDirectionA   = g.m_vDirectionA;
    m_vDirectionB   = g.m_vDirectionB;
    m_fTopZ         = g.m_fTopZ;
    m_fWidth        = g.m_fWidth;
    m_fHeight       = g.m_fHeight;
    m_fLeftCoord    = g.m_fLeftCoord;
    m_fRightCoord   = g.m_fRightCoord;
    m_fFrontCoord   = g.m_fFrontCoord;
    m_fBackCoord    = g.m_fBackCoord;
    m_fDoorPosition = g.m_fDoorPosition;
    m_nTimeToOpen   = g.m_nTimeToOpen;
    m_nOriginalType = g.m_nOriginalType;
    strcpy_s(m_anName, g.m_anName);
}

// 0x5D30C0
void CSaveGarage::CopyGarageOutOfSaveGarage(CGarage& g) const {
    g.m_nType         = m_nType;
    g.m_nDoorState    = m_nDoorState;
    g.m_nFlags        = m_nFlags;
    g.m_vPosn         = m_vPosn;
    g.m_vDirectionA   = m_vDirectionA;
    g.m_vDirectionB   = m_vDirectionB;
    g.m_fTopZ         = m_fTopZ;
    g.m_fWidth        = m_fWidth;
    g.m_fHeight       = m_fHeight;
    g.m_fLeftCoord    = m_fLeftCoord;
    g.m_fRightCoord   = m_fRightCoord;
    g.m_fFrontCoord   = m_fFrontCoord;
    g.m_fBackCoord    = m_fBackCoord;
    g.m_fDoorPosition = m_fDoorPosition;
    g.m_nTimeToOpen   = m_nTimeToOpen;
    g.m_nOriginalType = m_nOriginalType;
    g.m_pTargetCar    = nullptr;
    strcpy_s(g.m_anName, m_anName);
}

void CStoredCar::InjectHooks() {
    RH_ScopedClass(CStoredCar);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(StoreCar, 0x449760);
    RH_ScopedInstall(RestoreCar, 0x447E40);
}

// todo move
// 0x449760
void CStoredCar::StoreCar(CVehicle* vehicle) {
    m_wModelIndex = vehicle->m_nModelIndex;
    m_vPosn       = vehicle->GetPosition();

    m_PackedForward = vehicle->GetForward();

    m_nPrimaryColor   = vehicle->m_nPrimaryColor;
    m_nSecondaryColor  = vehicle->m_nSecondaryColor;
    m_nTertiaryColor   = vehicle->m_nTertiaryColor;
    m_nQuaternaryColor = vehicle->m_nQuaternaryColor;
    m_nRadioStation    = vehicle->m_vehicleAudio.m_AuSettings.RadioStation;
    m_nHandlingFlags   = vehicle->m_nHandlingFlagsIntValue;
    m_anCompsToUse[0]  = vehicle->m_anExtras[0];
    m_anCompsToUse[1]  = vehicle->m_anExtras[1];

    m_nStoredCarFlags = 0;
    bBulletProof      = vehicle->physicalFlags.bBulletProof;
    bFireProof        = vehicle->physicalFlags.bFireProof;
    bExplosionProof   = vehicle->physicalFlags.bExplosionProof;
    bCollisionProof   = vehicle->physicalFlags.bCollisionProof;
    bMeleeProof       = vehicle->physicalFlags.bMeleeProof;
    bUpgradedStereo   = vehicle->vehicleFlags.bUpgradedStereo;
    bHydraulics       = vehicle->handlingFlags.bHydraulicInst;
    bNitro            = vehicle->handlingFlags.bNosInst;

    if (vehicle->IsAutomobile() || vehicle->IsBike()) {
        m_nBombType = vehicle->m_nBombOnBoard;
    }
    rng::copy(vehicle->m_anUpgrades, m_awCarMods);
    m_nPaintJob    = (int8)vehicle->GetRemapIndex();
    m_nNitroBoosts = vehicle->m_nNitroBoosts;
}

// NOTE: Unlike `CCarGenerator::CreateVehicle`, trains (and the fake planes/helis) are created as automobiles
CVehicle* CStoredCar::CreateVehicle(eModelID modelId) {
    switch (CModelInfo::GetModelInfo(modelId)->AsVehicleModelInfoPtr()->m_nVehicleType) {
    case VEHICLE_TYPE_MTRUCK:
        return new CMonsterTruck(modelId, RANDOM_VEHICLE);
    case VEHICLE_TYPE_QUAD:
        return new CQuadBike(modelId, RANDOM_VEHICLE);
    case VEHICLE_TYPE_HELI:
        return new CHeli(modelId, RANDOM_VEHICLE);
    case VEHICLE_TYPE_PLANE:
        return new CPlane(modelId, RANDOM_VEHICLE);
    case VEHICLE_TYPE_BOAT:
        return new CBoat(modelId, RANDOM_VEHICLE);
    case VEHICLE_TYPE_BIKE: {
        auto* const bike = new CBike(modelId, RANDOM_VEHICLE);
        bike->bikeFlags.bOnSideStand = true;
        return bike;
    }
    case VEHICLE_TYPE_BMX: {
        auto* const bmx = new CBmx(modelId, RANDOM_VEHICLE);
        bmx->bikeFlags.bOnSideStand = true;
        return bmx;
    }
    case VEHICLE_TYPE_TRAILER:
        return new CTrailer(modelId, RANDOM_VEHICLE);
    default:
        return new CAutomobile(modelId, RANDOM_VEHICLE, true);
    }
}

// Returns null if the model or any of the upgrades isn't loaded yet (they're requested)
// 0x447E40
CVehicle* CStoredCar::RestoreCar() {
    CStreaming::RequestModel(m_wModelIndex, STREAMING_KEEP_IN_MEMORY);
    for (const auto upgrade : m_awCarMods) {
        if (upgrade != -1) {
            CStreaming::RequestVehicleUpgrade(upgrade, STREAMING_DEFAULT);
        }
    }

    if (!CStreaming::IsModelLoaded(m_wModelIndex)) {
        return nullptr;
    }
    for (const auto upgrade : m_awCarMods) {
        if (upgrade != -1 && !CStreaming::HasVehicleUpgradeLoaded(upgrade)) {
            return nullptr;
        }
    }

    CVehicleModelInfo::ms_compsToUse[0] = m_anCompsToUse[0];
    CVehicleModelInfo::ms_compsToUse[1] = m_anCompsToUse[1];

    auto* const vehicle = CreateVehicle((eModelID)m_wModelIndex);
    vehicle->SetPosn(m_vPosn);

    auto& mat = vehicle->GetMatrix();
    const CVector fwd = m_PackedForward;
    mat.GetForward() = fwd;
    mat.GetRight()   = CVector{ fwd.y, -fwd.x, 0.0f };
    mat.GetUp()      = CVector{ 0.0f, 0.0f, 1.0f };

    vehicle->SetStatus(STATUS_ABANDONED);
    vehicle->m_pDriver = nullptr;
    vehicle->m_vehicleAudio.m_AuSettings.RadioStation = (eRadioID)m_nRadioStation;
    vehicle->m_nHandlingFlagsIntValue                 = (eVehicleHandlingFlags)m_nHandlingFlags;
    vehicle->vehicleFlags.bFreebies             = false;
    vehicle->vehicleFlags.bHasBeenOwnedByPlayer = true;
    vehicle->m_nDoorLock                        = CARLOCK_UNLOCKED;
    if (vehicle->IsAutomobile()) {
        vehicle->m_nBombOnBoard = m_nBombType;
    }

    // NOTE: These are only ever set, never cleared (eg.: The Rhino is bullet and explosion proof by default)
    vehicle->physicalFlags.bBulletProof    |= bBulletProof;
    vehicle->physicalFlags.bFireProof      |= bFireProof;
    vehicle->physicalFlags.bExplosionProof |= bExplosionProof;
    vehicle->physicalFlags.bCollisionProof |= bCollisionProof;
    vehicle->physicalFlags.bMeleeProof     |= bMeleeProof;
    vehicle->handlingFlags.bHydraulicInst  |= bHydraulics;
    vehicle->handlingFlags.bNosInst        |= bNitro;
    if (bUpgradedStereo) {
        vehicle->vehicleFlags.bUpgradedStereo            = true;
        vehicle->m_vehicleAudio.m_AuSettings.BassSetting = eBassSetting::BOOST;
    }

    rng::copy(m_awCarMods, vehicle->m_anUpgrades.begin());
    vehicle->SetupUpgradesAfterLoad();
    vehicle->SetRemap(m_nPaintJob);
    vehicle->vehicleFlags.bEngineOn = false;
    vehicle->m_nNitroBoosts         = m_nNitroBoosts;

    vehicle->m_nPrimaryColor                        = m_nPrimaryColor;
    vehicle->m_nSecondaryColor                      = m_nSecondaryColor;
    vehicle->m_nTertiaryColor                       = m_nTertiaryColor;
    vehicle->vehicleFlags.bDontSetColourWhenRemapping = true;
    vehicle->m_nQuaternaryColor                     = m_nQuaternaryColor;

    return vehicle;
}
