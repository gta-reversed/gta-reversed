#include "StdInc.h"

#include "Garage.h"

void CGarage::InjectHooks() {
    RH_ScopedClass(CGarage);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(BuildRotatedDoorMatrix, 0x4479F0);
    RH_ScopedInstall(TidyUpGarageClose, 0x449D10);
    RH_ScopedInstall(TidyUpGarage, 0x449C50);
    RH_ScopedInstall(StoreAndRemoveCarsForThisHideOut, 0x449900);
    RH_ScopedInstall(EntityHasASphereWayOutsideGarage, 0x449050);
    RH_ScopedInstall(RemoveCarsBlockingDoorNotInside, 0x449690);
    RH_ScopedInstall(IsEntityTouching3D, 0x448EE0);
    RH_ScopedInstall(IsEntityEntirelyOutside, 0x448D30);
    RH_ScopedInstall(IsStaticPlayerCarEntirelyInside, 0x44A830);
    RH_ScopedInstall(IsEntityEntirelyInside3D, 0x448BE0);
    RH_ScopedOverloadedInstall(IsPointInsideGarage, "CVector", 0x448740, bool(CGarage::*)(CVector));
    RH_ScopedInstall(PlayerArrestedOrDied, 0x4486C0);
    RH_ScopedInstall(OpenThisGarage, 0x447D50);
    RH_ScopedInstall(CloseThisGarage, 0x447D70);
    RH_ScopedInstall(InitDoorsAtStart, 0x447600);
    RH_ScopedOverloadedInstall(IsPointInsideGarage, "CVector-float", 0x4487D0, bool(CGarage::*)(CVector, float));
    RH_ScopedInstall(Update, 0x44AA50, { .Reversed = false });
    RH_ScopedInstall(IsGarageEmpty, 0x44A9C0);
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

// Removes wrecked cars that are touching the garage, but aren't entirely inside it
// 0x449D10
void CGarage::TidyUpGarageClose() {
    auto pool = GetVehiclePool();
    for (auto i = pool->GetSize() - 1; i; i--) { // NOTE: Slot 0 is skipped in the original too
        auto vehicle = pool->GetAt(i);
        if (!vehicle || !vehicle->IsAutomobile() && !vehicle->IsBike()) {
            continue;
        }
        if (vehicle->GetStatus() != STATUS_WRECKED || !IsEntityTouching3D(vehicle)) {
            continue;
        }

        if (!IsClosed()) {
            const auto& spheres = vehicle->GetColData()->GetSpheres();
            if (spheres.empty()) {
                continue;
            }

            const auto& mat = vehicle->GetMatrix();
            const auto IsSphereInside = [&](const CColSphere& sphere) {
                return IsPointInsideGarage(mat.TransformPoint(sphere.m_vecCenter), sphere.m_fRadius);
            };
            if (rng::all_of(spheres, IsSphereInside)) {
                continue;
            }
        }

        CWorld::Remove(vehicle);
        delete vehicle;
    }
}

// Removes wrecked or upside down cars that are inside the garage
// 0x449C50
void CGarage::TidyUpGarage() {
    auto pool = GetVehiclePool();
    for (auto i = pool->GetSize() - 1; i; i--) { // NOTE: Slot 0 is skipped in the original too
        auto vehicle = pool->GetAt(i);
        if (!vehicle || !vehicle->IsAutomobile() && !vehicle->IsBike()) {
            continue;
        }
        if (!IsPointInsideGarage(vehicle->GetPosition())) {
            continue;
        }
        if (vehicle->GetStatus() == STATUS_WRECKED || vehicle->GetUp().z < 0.5f) {
            CWorld::Remove(vehicle);
            delete vehicle;
        }
    }
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
    const auto& mat = entity->GetMatrix();
    return rng::any_of(entity->GetColData()->GetSpheres(), [&](const CColSphere& sphere) {
        return !IsPointInsideGarage(mat.TransformPoint(sphere.m_vecCenter), fRadius + sphere.m_fRadius);
    });
}

// Removes the first car that is touching the garage, but isn't inside it (and can be deleted)
// 0x449690
void CGarage::RemoveCarsBlockingDoorNotInside() {
    auto pool = GetVehiclePool();
    for (auto i = pool->GetSize(); i; i--) {
        auto vehicle = pool->GetAt(i - 1);
        if (!vehicle || !IsEntityTouching3D(vehicle)) {
            continue;
        }
        if (IsPointInsideGarage(vehicle->GetPosition())) {
            continue;
        }
        if (vehicle->vehicleFlags.bIsLocked || !vehicle->CanBeDeleted()) {
            continue;
        }
        CWorld::Remove(vehicle);
        delete vehicle;
        return;
    }
}

// 0x448EE0
bool CGarage::IsEntityTouching3D(CEntity* entity) {
    const auto radius = entity->GetModelInfo()->GetColModel()->GetBoundRadius();
    const auto& pos   = entity->GetPosition();
    if (m_fLeftCoord - radius > pos.x || radius + m_fRightCoord < pos.x) {
        return false;
    }
    if (m_fFrontCoord - radius > pos.y || radius + m_fBackCoord < pos.y) {
        return false;
    }
    if (m_vPosn.z - radius > pos.z || radius + m_fTopZ < pos.z) {
        return false;
    }

    const auto& mat = entity->GetMatrix();
    return rng::any_of(entity->GetColData()->GetSpheres(), [&](const CColSphere& sphere) {
        return IsPointInsideGarage(mat.TransformPoint(sphere.m_vecCenter), sphere.m_fRadius);
    });
}

// 0x448D30
bool CGarage::IsEntityEntirelyOutside(CEntity* entity, float radius) {
    const auto& pos = entity->GetPosition();
    if (   m_fLeftCoord - radius < pos.x && radius + m_fRightCoord > pos.x
        && m_fFrontCoord - radius < pos.y && radius + m_fBackCoord > pos.y
    ) {
        return false;
    }

    const auto colData = entity->GetColData();
    if (!colData) {
        return true;
    }

    const auto& mat = entity->GetMatrix();
    return rng::none_of(colData->GetSpheres(), [&](const CColSphere& sphere) {
        return IsPointInsideGarage(mat.TransformPoint(sphere.m_vecCenter), radius + sphere.m_fRadius);
    });
}

// 0x44A830
bool CGarage::IsStaticPlayerCarEntirelyInside() {
    const auto vehicle = FindPlayerVehicle();
    if (!vehicle || !vehicle->IsAutomobile() && !vehicle->IsBike()) {
        return false;
    }
    if (FindPlayerPed()->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_LEAVE_CAR)) {
        return false;
    }

    const auto& pos = vehicle->GetPosition();
    if (pos.x < m_fLeftCoord || pos.x > m_fRightCoord || pos.y < m_fFrontCoord || pos.y > m_fBackCoord) {
        return false;
    }

    const auto& speed = vehicle->GetMoveSpeed();
    if (std::abs(speed.x) > 0.01f || std::abs(speed.y) > 0.01f || std::abs(speed.z) > 0.01f) {
        return false;
    }
    if (speed.SquaredMagnitude() > 0.0001f) {
        return false;
    }

    return IsEntityEntirelyInside3D(vehicle, 0.0f);
}

// 0x448BE0
bool CGarage::IsEntityEntirelyInside3D(CEntity* entity, float radius) {
    const auto& pos = entity->GetPosition();
    if (m_fLeftCoord - radius > pos.x || radius + m_fRightCoord < pos.x) {
        return false;
    }
    if (m_fFrontCoord - radius > pos.y || radius + m_fBackCoord < pos.y) {
        return false;
    }
    if (m_vPosn.z - radius > pos.z || radius + m_fTopZ < pos.z) {
        return false;
    }

    const auto colData = entity->GetColData();
    if (!colData) {
        return true;
    }

    const auto& mat = entity->GetMatrix();
    return rng::all_of(colData->GetSpheres(), [&](const CColSphere& sphere) {
        return IsPointInsideGarage(mat.TransformPoint(sphere.m_vecCenter), radius - sphere.m_fRadius);
    });
}

// 0x448740
bool CGarage::IsPointInsideGarage(CVector point) {
    if (point.z < m_vPosn.z || point.z > m_fTopZ) {
        return false;
    }

    const auto dx = point.x - m_vPosn.x;
    const auto dy = point.y - m_vPosn.y;

    const auto a = dx * m_vDirectionA.x + dy * m_vDirectionA.y;
    if (a < 0.0f || a > m_fWidth) {
        return false;
    }

    const auto b = dx * m_vDirectionB.x + dy * m_vDirectionB.y;
    if (b < 0.0f || b > m_fHeight) {
        return false;
    }

    return true;
}

// 0x4486C0
void CGarage::PlayerArrestedOrDied() {
    switch (m_nType) {
    case INVALID:
        break;
    case BOMBSHOP_TIMED:
    case BOMBSHOP_ENGINE:
    case BOMBSHOP_REMOTE:
    case PAYNSPRAY:
    case CRUSHER: {
        switch (m_nDoorState) {
        case GARAGE_DOOR_CLOSED:
        case GARAGE_DOOR_CLOSING:
        case GARAGE_DOOR_OPENING:
            m_nDoorState = GARAGE_DOOR_OPENING;
            break;
        }
        break;
    }
    default: {
        if (m_nType > HANGAR_ABANDONED_AIRPORT) {
            break;
        }
        switch (m_nDoorState) {
        case GARAGE_DOOR_OPEN:
        case GARAGE_DOOR_CLOSING:
        case GARAGE_DOOR_OPENING:
            m_nDoorState = GARAGE_DOOR_CLOSING;
            break;
        }
        break;
    }
    }
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
    m_bInactive            = false;
    m_bUsedRespray         = false;
    m_bDoorClosed          = true;
    m_bRespraysAlwaysFree  = false;
    m_nDoorState           = GARAGE_DOOR_CLOSED;
    m_nTimeToOpen          = 0;

    switch (m_nType) {
    case INVALID:
    case CRUSHER:
        break;
    case BOMBSHOP_TIMED:
    case BOMBSHOP_ENGINE:
    case BOMBSHOP_REMOTE:
    case PAYNSPRAY:
        m_nDoorState    = GARAGE_DOOR_OPEN;
        m_fDoorPosition = 1.0f;
        break;
    default:
        if (m_nType > HANGAR_ABANDONED_AIRPORT) {
            break;
        }
        m_nDoorState    = GARAGE_DOOR_CLOSED;
        m_fDoorPosition = 0.0f;
        break;
    }
}

// 0x4487D0
bool CGarage::IsPointInsideGarage(CVector point, float radius) {
    if (m_vPosn.z - radius > point.z || radius + m_fTopZ < point.z) {
        return false;
    }

    const auto dx = point.x - m_vPosn.x;
    const auto dy = point.y - m_vPosn.y;

    const auto a = dx * m_vDirectionA.x + dy * m_vDirectionA.y;
    if (a < -radius || a > radius + m_fWidth) {
        return false;
    }

    const auto b = dx * m_vDirectionB.x + dy * m_vDirectionB.y;
    if (b < -radius || b > radius + m_fHeight) {
        return false;
    }

    return true;
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
    const CVector cornerA = { m_fLeftCoord, m_fFrontCoord, m_vPosn.z };
    const CVector cornerB = { m_fRightCoord, m_fBackCoord, m_fTopZ };

    int16 numEntities{};
    std::array<CEntity*, 16> entities{};
    CWorld::FindObjectsIntersectingCube(cornerA, cornerB, &numEntities, (int16)entities.size(), entities.data(), false, true, true, false, false);

    return std::none_of(entities.begin(), entities.begin() + numEntities, [this](CEntity* entity) {
        return IsEntityTouching3D(entity);
    });
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

// todo move
// 0x449760
void CStoredCar::StoreCar(CVehicle* vehicle) {
    plugin::CallMethod<0x449760, CStoredCar*, CVehicle*>(this, vehicle);
}

// 0x447E40
CVehicle* CStoredCar::RestoreCar() {
    return plugin::CallMethodAndReturn<CVehicle*, 0x447E40, CStoredCar*>(this);
}
