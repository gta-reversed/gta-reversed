#include "StdInc.h"

#include "Garage.h"
#include "Garages.h"
#include <extensions/Shapes/AngledRect.hpp>

constexpr auto RESPRAY_COST = 100u;

void CGarage::InjectHooks() {
    RH_ScopedClass(CGarage);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(BuildRotatedDoorMatrix, 0x4479F0);
    RH_ScopedInstall(StoreAndRemoveCarsForThisHideOut, 0x449900);
    RH_ScopedInstall(OpenThisGarage, 0x447D50);
    RH_ScopedInstall(CloseThisGarage, 0x447D70);
    RH_ScopedInstall(TidyUpGarageClose, 0x449D10);
    RH_ScopedInstall(TidyUpGarage, 0x449C50);
    RH_ScopedInstall(EntityHasASphereWayOutsideGarage, 0x449050);
    RH_ScopedInstall(RemoveCarsBlockingDoorNotInside, 0x449690);
    RH_ScopedInstall(IsEntityTouching3D, 0x448EE0);
    RH_ScopedInstall(IsEntityEntirelyOutside, 0x448D30);
    RH_ScopedInstall(IsStaticPlayerCarEntirelyInside, 0x44A830);
    RH_ScopedInstall(IsEntityEntirelyInside3D, 0x448BE0);
    RH_ScopedInstall(PlayerArrestedOrDied, 0x4486C0);
    RH_ScopedInstall(InitDoorsAtStart, 0x447600);
    RH_ScopedOverloadedInstall(IsPointInsideGarage, "NoTolerance", 0x448740, bool(CGarage::*)(CVector));
    RH_ScopedOverloadedInstall(IsPointInsideGarage, "WithTolerance", 0x4487D0, bool(CGarage::*)(CVector, float));
    RH_ScopedInstall(Update, 0x44AA50);
}

// NOTSA [Based on 0x4471E0]
CGarage::CGarage(
    CVector base,
    CVector2D pA,
    CVector2D pB,
    float ceilingZ,
    eGarageType type,
    uint32,
    const char* name,
    uint32 flagsIPL
) :
    m_Base{ base },
    m_DirA{ (pA - base).Normalized(&m_LenA) },
    m_DirB{ (pB - base).Normalized(&m_LenB) },
    m_CeilingZ{ ceilingZ },
    m_Type{ type },
    m_bDoorGoesIn{ flagsIPL & IPL_FLAG_DOOR_GOES_IN },
    m_bCameraFollowsPlayer{ flagsIPL & IPL_FLAG_CAM_FOLLOW_PLAYER },
    m_bDoorOpensUp{ flagsIPL & IPL_FLAG_DOOR_UP_AND_ROTATE }
{
    assert(m_Base.z <= ceilingZ);

    strcpy_s(m_Name, name);

    const auto SetMinMaxAxis = [&](float& min, float& max, size_t axis) {
        std::tie(min, max) = std::minmax({ base[axis], pA[axis], pB[axis], (pA[axis] + pB[axis]) - base[axis] });
    };
    SetMinMaxAxis(m_MinX, m_MaxX, 0);
    SetMinMaxAxis(m_MinY, m_MaxY, 1);
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
    for (auto& veh : GetVehiclePool()->GetAllValid()) {
        if (!veh.IsAutomobile() && !veh.IsBike()) {
            continue;
        }
        if (veh.GetStatus() != STATUS_WRECKED) {
            continue;
        }
        if (!IsEntityTouching3D(&veh)) {
            continue;
        }
        if (!IsClosed()) {
            // TODO/NOTE/BUG:
            // I'm not sure what this check is supposed to do
            // The door is open, so why check if the vehicle is not wholly inside?
            // I think this check is meant to be done in case the door *IS* closed
            // and is meant to check if the vehicle *IS* wholly inside the garage.
            // This is kinda confirmed by looking at `TidyUpGarage` [where the veh. is only removed if inside the garage]
            if (!EntityHasASphereWayOutsideGarage(&veh)) {
                continue;
            }
        }
        CWorld::Remove(&veh);
        delete &veh;
    }
}

// 0x449C50
void CGarage::TidyUpGarage() {
    for (auto& veh : GetVehiclePool()->GetAllValid()) {
        if (!veh.IsAutomobile() && !veh.IsBike()) {
            continue;
        }
        if (veh.GetStatus() != STATUS_WRECKED && veh.GetUp().z >= 0.5f) {
            continue;
        }
        if (!IsPointInsideGarage(veh.GetPosition())) {
            continue;
        }
        CWorld::Remove(&veh);
        delete &veh;
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

// !any_of(outside) = all_of(inside)
// !any_of(inside) = all_of(outside)

bool CGarage::EntityHasASphereTest(CEntity* entity, bool inside, float tolerance) {
    return rng::any_of(
        entity->GetColData()->GetSpheres(),
        [&](CColSphere sp) {
            sp.m_fRadius += tolerance;
    const auto hasSphereInside = IsSphereInsideGarage(TransformObject(sp, entity->GetMatrix()));
    return inside
        ? hasSphereInside
        : !hasSphereInside;
        }
    );
}

// 0x449050
bool CGarage::EntityHasASphereWayOutsideGarage(CEntity* entity, float tolerance) {
    return EntityHasASphereTest(entity, false, tolerance);
}

// NOTSA
bool CGarage::EntityHasSphereInsideGarage(CEntity* entity, float tolerance) {
    return EntityHasASphereTest(entity, true, tolerance);
}

bool CGarage::IsAnyOtherCarTouchingGarage(CVehicle* ignoredVehicle) {
    return plugin::CallMethodAndReturn<bool, 0x449100>(this, ignoredVehicle);
}

void CGarage::ThrowCarsNearDoorOutOfGarage(CVehicle* ignoredVehicle) {
    plugin::CallMethod<0x449220>(this, ignoredVehicle);
}

// 0x449690
void CGarage::RemoveCarsBlockingDoorNotInside() {
    for (auto& veh : GetVehiclePool()->GetAllValid()) {
        if (!IsEntityTouching3D(&veh)) {
            continue;
        }
        if (!IsPointInsideGarage(veh.GetPosition())) {
            continue;
        }
        if (veh.vehicleFlags.bIsLocked) {
            continue;
        }
        if (!veh.CanBeDeleted()) {
            continue;
        }
        CWorld::Remove(&veh);
        delete& veh;
        if (!notsa::IsFixBugs()) { // For some reason original code only deletes one vehicle
            break;
        }
    }
}

// 0x448EE0
bool CGarage::IsEntityTouching3D(CEntity* entity) {
    if (CCollision::TestSphereBox({ entity->GetPosition(), entity->GetColModel()->GetBoundRadius() }, GetAABB())) {
        return true;
    }
    return EntityHasSphereInsideGarage(entity);
}

// 0x448BE0
bool CGarage::IsEntityEntirelyInside3D(CEntity* entity, float tolerance) {
    if (!CCollision::TestSphereBox({ entity->GetPosition(), entity->GetColModel()->GetBoundRadius() }, GetAABB())) {
        return true;
    }
    return !EntityHasSphereInsideGarage(entity);
}

// 0x448D30
bool CGarage::IsEntityEntirelyOutside(CEntity* entity, float tolerance) {
    if (!CCollision::TestSphereBox({ entity->GetPosition(), entity->GetColModel()->GetBoundRadius() }, GetAABB())) {
        return true;
    }
    return !EntityHasSphereInsideGarage(entity);
}

// Address unknown
bool CGarage::IsPlayerOutsideGarage(float tolerance) {
    return IsEntityEntirelyOutside(FindPlayerEntity(), tolerance);
}

// 0x448740 - Original function was __spoils<>, so we have to keep the registers intact!
bool CGarage::IsPointInsideGarage(CVector point) { // TODO: Remove this in favor of the overloaded version
    __asm pushad;
    const auto ret = GetBB().IsPointWithin(point);
    __asm popad;
    return ret;
}

// 0x4487D0 - Original function was __spoils<>, so we have to keep the registers intact!
bool CGarage::IsPointInsideGarage(CVector point, float tolerance) {
    __asm pushad;
    const auto ret = GetBB().IsPointWithin(point, tolerance);
    __asm popad;
    return ret;
}

// 0x44A830
bool CGarage::IsStaticPlayerCarEntirelyInside() {
    const auto plyrVeh = FindPlayerVehicle();
    if (!plyrVeh) {
        return false;
    }
    if (!plyrVeh->IsAutomobile() && !plyrVeh->IsBike()) {
        return false;
    }
    if (FindPlayerPed()->GetTaskManager().Find<TASK_COMPLEX_LEAVE_CAR>()) { // Ped is leaving teh car
        return false;
    }
    if (!GetAABB().IsPointWithin(plyrVeh->GetPosition())) {
        return false;
    }
    if (plyrVeh->GetMoveSpeed().SquaredMagnitude() >= sq(0.01f)) { // The originally compared each component: abs(x) > 0.01, but that's retarded
        return false;
    }
    if (!IsEntityEntirelyInside3D(plyrVeh)) {
        return false;
    }
    return true;
}

// Unknown address
bool CGarage::IsPlayerEntirelyInsideGarage() {
    return IsEntityEntirelyInside3D(FindPlayerEntity());
}

// 0x4486C0
void CGarage::PlayerArrestedOrDied() {
    switch (m_Type) {
    case ONLY_TARGET_VEH:
    case COLLECTORSITEMS:
    case COLLECTSPECIFICCARS:

    case COLLECTCARS_1:
    case COLLECTCARS_2:
    case COLLECTCARS_3:
    case COLLECTCARS_4:

    case UNKN_CLOSESONTOUCH:
    case SIXTY_SECONDS:

    case OPEN_FOR_TARGET_FREEZE_PLAYER:
    case CLOSE_WITH_CAR_DONT_OPEN_AGAIN:

    case STAY_OPEN_WITH_CAR_INSIDE:

    case SCRIPT_ONLY_OPEN:
    case SCRIPT_CONTROLLED:
    case SCRIPT_OPEN_FREEZE_WHEN_CLOSING:

    case SAFEHOUSE_GANTON:
    case SAFEHOUSE_SANTAMARIA:
    case SAGEHOUSE_ROCKSHORE:
    case SAFEHOUSE_FORTCARSON:
    case SAFEHOUSE_VERDANTMEADOWS:
    case SAFEHOUSE_DILLIMORE:
    case SAFEHOUSE_PRICKLEPINE:
    case SAFEHOUSE_WHITEWOOD:
    case SAFEHOUSE_PALOMINOCREEK:
    case SAFEHOUSE_REDSANDSWEST:
    case SAFEHOUSE_ELCORONA:
    case SAFEHOUSE_MULHOLLAND:
    case SAFEHOUSE_CALTONHEIGHTS:
    case SAFEHOUSE_PARADISO:
    case SAFEHOUSE_DOHERTY:
    case SAFEHOUSE_HASHBURY:

    case TUNING_LOCO_LOW_CO:
    case TUNING_WHEEL_ARCH_ANGELS:
    case TUNING_TRANSFENDER:

    case IMPOUND_LS:
    case IMPOUND_SF:
    case IMPOUND_LV:

    case BURGLARY:

    case HANGAR_AT400:
    case HANGAR_ABANDONED_AIRPORT: {
        switch (m_DoorState) {
        case GARAGE_DOOR_OPEN:
        case GARAGE_DOOR_CLOSING:
        case GARAGE_DOOR_OPENING:
            m_DoorState = GARAGE_DOOR_CLOSING;
        }
        break;
    }
    case BOMBSHOP_TIMED:
    case BOMBSHOP_ENGINE:
    case BOMBSHOP_REMOTE:
    case PAYNSPRAY:
    case CRUSHER: {
        switch (m_DoorState) {
        case GARAGE_DOOR_CLOSED:
        case GARAGE_DOOR_CLOSING:
        case GARAGE_DOOR_OPENING:
            m_DoorState = GARAGE_DOOR_OPENING;
        }
        break;
    }
    }
}

// 0x447600
void CGarage::InitDoorsAtStart() {
    m_bRespraysAlwaysFree = false;
    m_bUsedRespray        = false;
    m_bInactive           = false;
    m_bDoorClosed         = true;
    m_TimeToOpen          = 0;
    switch (m_Type) {
    case ONLY_TARGET_VEH:
    case COLLECTORSITEMS:
    case COLLECTSPECIFICCARS:
    case COLLECTCARS_1:
    case COLLECTCARS_2:
    case COLLECTCARS_3:
    case UNKN_CLOSESONTOUCH:
    case SIXTY_SECONDS:
    case OPEN_FOR_TARGET_FREEZE_PLAYER:
    case SCRIPT_ONLY_OPEN:
    case SAFEHOUSE_GANTON:
    case SAFEHOUSE_SANTAMARIA:
    case SAGEHOUSE_ROCKSHORE:
    case SCRIPT_CONTROLLED:
    case STAY_OPEN_WITH_CAR_INSIDE:
    case CLOSE_WITH_CAR_DONT_OPEN_AGAIN:
    case COLLECTCARS_4:
    case SCRIPT_OPEN_FREEZE_WHEN_CLOSING:
    case SAFEHOUSE_FORTCARSON:
    case SAFEHOUSE_VERDANTMEADOWS:
    case SAFEHOUSE_DILLIMORE:
    case SAFEHOUSE_PRICKLEPINE:
    case SAFEHOUSE_WHITEWOOD:
    case SAFEHOUSE_PALOMINOCREEK:
    case SAFEHOUSE_REDSANDSWEST:
    case SAFEHOUSE_ELCORONA:
    case SAFEHOUSE_MULHOLLAND:
    case IMPOUND_LS:
    case IMPOUND_SF:
    case IMPOUND_LV:
    case TUNING_LOCO_LOW_CO:
    case TUNING_WHEEL_ARCH_ANGELS:
    case TUNING_TRANSFENDER:
    case SAFEHOUSE_CALTONHEIGHTS:
    case SAFEHOUSE_PARADISO:
    case SAFEHOUSE_DOHERTY:
    case SAFEHOUSE_HASHBURY:
    case BURGLARY:
    case HANGAR_AT400:
    case HANGAR_ABANDONED_AIRPORT: {
        m_DoorState = GARAGE_DOOR_CLOSED;
        m_DoorOpenness = 0.f;
        break;
    }
    case BOMBSHOP_TIMED:
    case BOMBSHOP_ENGINE:
    case BOMBSHOP_REMOTE:
    case PAYNSPRAY: {
        m_DoorState = GARAGE_DOOR_OPEN;
        m_DoorOpenness = 1.f;
        break;
    }
    default: // Pirulax: TODO/NOTE: Is this even reachable?
        m_DoorState = GARAGE_DOOR_CLOSED;
        break;
    }
}

// 0x447D50
void CGarage::OpenThisGarage() {
  if ( m_DoorState == GARAGE_DOOR_CLOSED
    || m_DoorState == GARAGE_DOOR_CLOSING
    || m_DoorState == GARAGE_DOOR_CLOSED_DROPPED_CAR)
  {
    m_DoorState = GARAGE_DOOR_OPENING;
  }
}

// 0x447D70
void CGarage::CloseThisGarage() {
    if (m_DoorState == GARAGE_DOOR_OPEN || m_DoorState == GARAGE_DOOR_OPENING) {
        m_DoorState = GARAGE_DOOR_CLOSING;
    }
}

// NOTSA [Based on 0x44AA50]
void CGarage::UpdatePlayerCameraStuff() {
    if (m_Type == CRUSHER || m_DoorState == GARAGE_DOOR_CRUSHING) {
        return;
    }
    const auto plyrPed = FindPlayerPed();
    if (!plyrPed || plyrPed->m_bIsInSafePosition) {
        return;
    }
    if (const auto plyrVeh = plyrPed->GetVehicleIfInOne()) {
        if (!CGarage::IsEntityEntirelyInside3D( // 0x44AAFB
            plyrVeh->GetModelID() == eModelID::MODEL_KART
                ? (CEntity*)plyrVeh
                : (CEntity*)plyrPed,
            0.25f
        )) { // 0x44AB1B
            if (!IsEntityEntirelyOutside(plyrVeh)) {
                TheCamera.m_pToGarageWeAreInForHackAvoidFirstPerson = this;
            }
            if (plyrVeh->GetModelID() != MODEL_MRWHOOP) {
                return;
            }
            if (!GetAARect().IsPointInside(plyrVeh->GetPosition2D(), 0.5f)) {
                return;
            }
        }
    } else {
        if (!CGarage::IsEntityEntirelyInside3D(plyrPed, 0.25f)) { // 0x44AAFB [Yes, same as above, but we know the vehicle is null, so that is omitted]
            return;
        }
    }

    CGarages::bCamShouldBeOutside = true;
    TheCamera.m_pToGarageWeAreIn = this;
}

// 0x44AA50
void CGarage::Update(int32 garageId) {
    UpdatePlayerCameraStuff();

    if (m_bInactive) {
        return;
    }

    if (m_bDoorOpensUp) {
        m_bDoorClosed = (m_DoorState != GARAGE_DOOR_OPENING || m_DoorOpenness <= 0.4f) && m_DoorState != GARAGE_DOOR_OPEN;
    }

    const auto  plyrVeh = FindPlayerVehicle();
    const auto  plyrPed = FindPlayerPed();
    const auto& plyrCoors = FindPlayerCoors();

    const auto SetDoorOpeningIfPlayerVehicleClose = [this] { // 0x44B9CC
        const auto plyrVeh = FindPlayerVehicle();
        if (!plyrVeh) {
            return;
        }
        if (CalcDistToGarageRectangleSquared(plyrVeh->GetPosition2D()) <= sq(8.f)) {
            m_DoorState = GARAGE_DOOR_OPENING;
        }
    };

    const auto IsPlayerInRadiusOfGarage = [&, this](float r) {
        return (GetCenter2D() - plyrCoors).SquaredMagnitude() <= sq(r);
    };

    const auto SetIsPlayerInGarage = [this](bool inGaragae) {
        CPad::GetPad()->bPlayerAwaitsInGarage = inGaragae;
        FindPlayerWanted()->m_bPoliceBackOffGarage = inGaragae;
    };

    const auto ProessSlideDoor = [&, this](eGarageDoorState stateOnSlideDoorOpened) {
        if (SlideDoorOpen()) {
            m_DoorState = stateOnSlideDoorOpened;
        }
        if (m_DoorOpenness > 0.5f) {
            SetIsPlayerInGarage(false);
        }
    };

    // Let the hell unfold
    switch (m_Type) {
    case ONLY_TARGET_VEH: {
        switch (m_DoorState) {
        case GARAGE_DOOR_CLOSED: {// 0x44B7FB
            if (m_TargetCar = plyrVeh) {
                SetDoorOpeningIfPlayerVehicleClose();
            }
            return;
        }
        case GARAGE_DOOR_OPEN: { // 0x44B80C
            if (IsPlayerInRadiusOfGarage(30.f)) {
                if (!m_TargetCar || plyrVeh == m_TargetCar) {
                    return;
                }
                if (!IsEntityEntirelyInside3D(m_TargetCar)) {
                    return;
                }
                if (!IsPlayerOutsideGarage(2.f)) {
                    return;
                }
                m_DoorState = GARAGE_DOOR_CLOSING;
                m_bClosingEmpty = false;
                SetIsPlayerInGarage(true);
                return;
            }
            if (CTimer::GetFrameCounter() % 32) {
                return;
            }
            if (m_TargetCar && IsEntityEntirelyInside3D(m_TargetCar)) {
                return;
            }
            m_bClosingEmpty = true;
            m_DoorState     = GARAGE_DOOR_CLOSING;
            return;
        }
        case GARAGE_DOOR_CLOSING: { // 0x44B963
            if (m_TargetCar) {
                ThrowCarsNearDoorOutOfGarage(m_TargetCar);
            }
            if (!SlideDoorClosed()) {
                return;
            }
            if (m_bClosingEmpty) {
                m_DoorState = GARAGE_DOOR_CLOSED;
                return;
            }
            if (m_TargetCar) { // Inverted
                CVehicle::DestroyVehicleAndDriverAndPassengers(m_TargetCar);
                m_DoorState = GARAGE_DOOR_CLOSED_DROPPED_CAR;
                m_TargetCar = nullptr;
            } else {
                m_DoorState = GARAGE_DOOR_CLOSING;
            }
            SetIsPlayerInGarage(false);
            break;
        }
        case GARAGE_DOOR_OPENING: {
            if (SlideDoorOpen()) {
                m_DoorState = GARAGE_DOOR_OPEN;
            }
            return;
        }
        return;
        }
    }
    case BOMBSHOP_TIMED:
    case BOMBSHOP_ENGINE:
    case BOMBSHOP_REMOTE: { // 0x44B419
        switch (m_DoorState) { 
        case GARAGE_DOOR_CLOSED: {
            if (CTimer::GetTimeInMS() <= m_TimeToOpen) { // TODO: Is this correct?
                return;
            }

            // Make sure bomb model is loaded [if needed]
            if (m_Type == BOMBSHOP_REMOTE && !CStreaming::IsModelLoaded(MODEL_BOMB)) {
                CStreaming::RequestModel(MODEL_BOMB, STREAMING_GAME_REQUIRED);
                return;
            }

            // 0x44ABF9
            AudioEngine.ReportFrontendAudioEvent([this] {
                switch (m_Type) {
                case BOMBSHOP_TIMED:  return AE_FRONTEND_CAR_FIT_BOMB_TIMED;
                case BOMBSHOP_ENGINE: return AE_FRONTEND_CAR_FIT_BOMB_BOOBY_TRAPPED;
                case BOMBSHOP_REMOTE: return AE_FRONTEND_CAR_FIT_BOMB_REMOTE_CONTROLLED;
                default:              NOTSA_UNREACHABLE();
                }
            }());

            m_DoorState = GARAGE_DOOR_OPENING;

            // Deduce the price of a bomb [We've checked if the player has enough upon entering, no need to check it here again]
            if (!CGarages::BombsAreFree) {
                VERIFY(FindPlayerInfo().DeductMoney(500)); // Check it again just in case...
            }

            //< 0x44B657 - Install bomb on vehicle if possible
            if (plyrVeh && plyrVeh->CanBomBeInstalled()) { // 0x44B657
                plyrVeh->m_nBombOnBoard = [this] {
                    switch (m_Type) {
                    case BOMBSHOP_TIMED:  return BOB_TIMED;
                    case BOMBSHOP_ENGINE: return BOB_ON_IGNITION;
                    case BOMBSHOP_REMOTE: return BOB_REMOTE;
                    default:              NOTSA_UNREACHABLE();
                    }
                }();
                plyrVeh->m_pWhoInstalledBombOnMe = plyrPed;
                if (m_Type == BOMBSHOP_REMOTE) {
                    CGarages::GivePlayerDetonator();
                }
                CStats::IncrementStat(STAT_KGS_OF_EXPLOSIVES_USED, 10.f);
            }

            //< 0x44B6E8 - Set help message
            if (const auto messageKey = [this] {
                const auto Choose = [](auto a, auto b) -> const char* {
                    switch (CPad::GetPad()->Mode) {
                    case 0:
                    case 1:
                    case 2:
                        return a;
                    case 3:
                        return b;
                    }
                    return nullptr;
                };
                switch (m_Type) {
                case BOMBSHOP_TIMED:  return Choose("GA_6", "GA_6B");
                case BOMBSHOP_ENGINE: return Choose("GA_7", "GA_7B");
                case BOMBSHOP_REMOTE: return "GA_8";
                default:              NOTSA_UNREACHABLE();
                }
            }()) {
                CHud::SetHelpMessage(TheText.Get(messageKey), false, false, true);
            }

            return;
        }
        case GARAGE_DOOR_OPENING: { // 0x44B42F
            if (!IsStaticPlayerCarEntirelyInside()) {
                return;
            }
            if (!plyrVeh || !plyrVeh->CanBomBeInstalled()) {
                return;
            }

            if (plyrVeh->GetBombOnBoard() != BOB_NONE) { // If there's a bomb on board trigger it
                CGarages::TriggerMessage("GA_5", -1, 4000);
                m_DoorState = GARAGE_DOOR_WAITING_PLAYER_TO_EXIT;
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_CAR_ALREADY_RIGGED);
            } else if (CGarages::BombsAreFree || FindPlayerInfo().m_nMoney >= 500) { // Buy bomb if player has enough money
                m_DoorState = GARAGE_DOOR_CLOSING;
                SetIsPlayerInGarage(true);
            } else { // Not enough money
                CGarages::TriggerMessage("GA_4", -1, 4000u);
                m_DoorState = GARAGE_DOOR_WAITING_PLAYER_TO_EXIT;
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_CAR_NO_CASH);
            }
            return;
        }
        case GARAGE_DOOR_CLOSING: { // 0x44B567
            if (plyrVeh) {
                ThrowCarsNearDoorOutOfGarage(plyrVeh);
            }
            if (SlideDoorClosed()) {
                m_DoorState = GARAGE_DOOR_CLOSED;
                m_TimeToOpen = CTimer::GetTimeInMS() + 2000;
            }
            if (m_Type == BOMBSHOP_REMOTE) {
                CStreaming::RequestModel(MODEL_BOMB, STREAMING_GAME_REQUIRED);
            }
            return;
        }
        case GARAGE_DOOR_OPEN: { // 0x44B426
            ProessSlideDoor(GARAGE_DOOR_WAITING_PLAYER_TO_EXIT);
            return;
        }
        }

        break;
    }
    case PAYNSPRAY: { // 0x44AC36
        if (plyrCoors.z >= 950.f) {
            return;
        }
        const auto CallOffChase = [&, this](bool onlyIfCloseEnoughInVehicle) { // 0x44B3C1
            if (onlyIfCloseEnoughInVehicle && (!IsPlayerInRadiusOfGarage(8.f) || !plyrVeh)) {
                return;
            }
            CWorld::CallOffChaseForArea(
                m_MinX - 10.f,
                m_MinY - 10.f,
                m_MaxX + 10.f,
                m_MaxY + 10.f
            );
        };
        switch (m_DoorState) { // 0x44AC49
        case GARAGE_DOOR_CLOSED: {
            if (!CGarages::NoResprays) {
                return;
            }
            if (CTimer::GetTimeInMS() >= m_TimeToOpen) {
                m_DoorState = GARAGE_DOOR_OPENING;

                auto bNeededRespray = false;

                const auto plyrWanted = FindPlayerWanted();
                const auto bPlayerIsWanted = bNeededRespray = plyrWanted->m_nWantedLevel;
                if (bPlayerIsWanted) {
                    plyrWanted->ClearWantedLevelAndGoOnParole();
                    bNeededRespray = true;
                }

                bool bShowChangedColorMessage = false;
                if (plyrVeh && IsVehicleTypeAllowedInside(plyrVeh)) { // 0x44AF40
                    if (plyrVeh->m_fHealth < 970.f) {
                        bNeededRespray = true;
                    }
                    plyrVeh->m_fHealth = std::max(1000.f, plyrVeh->m_fHealth);
                    switch (plyrVeh->m_nVehicleType) {
                    case VEHICLE_TYPE_AUTOMOBILE: plyrVeh->AsAutomobile()->m_fBurnTimer = 0.f; break;
                    case VEHICLE_TYPE_BIKE:       plyrVeh->AsBike()->m_BlowUpTimer = 0; break;
                    }
                    plyrVeh->Fix();
                    plyrVeh->UnFlipIfFlipped();
                    CStats::IncrementStat(STAT_VEHICLE_RESPRAYS);

                    // Set [possibly] new color
                    if (plyrVeh->GetRemapIndex() < 0) { // 0x44B0E4
                        assert(plyrVeh->IsAutomobile()); // Original game did an oopsie here and [potentially] casted a CBike* into CAutomobile*
                        if (!plyrVeh->AsAutomobile()->autoFlags.bShouldNotChangeColour) {
                            plyrVeh->SetRemap(-1);

                            std::array<uint8, 4> newColors;
                            plyrVeh->GetVehicleModelInfo()->ChooseVehicleColour(
                                newColors[0],
                                newColors[1],
                                newColors[2],
                                newColors[3],
                                true
                            );

                            const auto colorsHaveChanged = plyrVeh->SetColors(newColors);
                            bShowChangedColorMessage |= colorsHaveChanged;

                            // Add fx
                            if (colorsHaveChanged) {
                                const auto primColor = RwRGBAReal(CVehicleModelInfo::ms_vehicleColourTable[plyrVeh->m_nPrimaryColor]);
                                FxPrtMult_c fxPrt{
                                    primColor.red, primColor.green, primColor.blue, 0.6f,
                                    0.7f,
                                    1.f,
                                    0.4f
                                };
                                auto& vehPos = plyrVeh->GetPosition();
                                for (auto i = 0; i < 10; i++) { // 0x44B26C
                                    // CVector::Random({ -3.f, -3.f, 0.f }, { 3.f, 3.f, 0.05f })
                                    auto fxVel = CVector{0.f, 0.f, CGeneral::GetRandomNumberInRange(0.f, 0.05f)};
                                    auto fxPos = vehPos + CVector{ CVector2D::Random(-3.f, 3.f) };
                                    g_fx.m_SmokeHuge->AddParticle(
                                        &vehPos,
                                        &fxPos,
                                        0.f,
                                        &fxPrt,
                                        -1.f,
                                        1.2f,
                                        0.6f,
                                        false
                                    );
                                }
                            }
                        }
                    }

                    plyrVeh->vehicleFlags.bDisableParticles = false;
                    plyrVeh->m_fDirtLevel = 0.f;
                } else {
                    bShowChangedColorMessage = bPlayerIsWanted;
                }

                //< 0x44B2DE - Display messages
                if (m_bRespraysAlwaysFree) { // Inverted
                    CGarages::TriggerMessage("GA_22", -1, 4000); // "Your girlfriend has fixed your vehicle."
                } else {
                    if (bShowChangedColorMessage) {
                        if (!bNeededRespray || CGarages::RespraysAreFree) {
                            // "Hope you like the new color" or "Respray is complementary"
                            CGarages::TriggerMessage(CGeneral::RandomChoiceFromList({ "GA_15", "GA_16" }), -1, 4000);
                        }
                    } else {
                        VERIFY(FindPlayerInfo().DeductMoney(RESPRAY_COST));
                        CStats::IncrementStat(STAT_AUTO_REPAIR_AND_PAINTING_BUDGET, (float)RESPRAY_COST);

                        // "New engine and paint job: $100~n~Cops won't recognize you!" or "New engine and paint job: $100"
                        CGarages::TriggerMessage(bPlayerIsWanted ? "GA_2" : "GA_XX", -1, 4000); 
                    }
                }
                m_bUsedRespray = true;
                if (plyrVeh) {
                    plyrVeh->vehicleFlags.bHasBeenResprayed = true;
                }
            }
            CallOffChase(false);
            return;
        }
        case GARAGE_DOOR_OPEN: { // 0x44AC57
            if (CGarages::NoResprays) {
                return;
            }

            const auto ProcessPlayerIsInGarage = [&, this] {
                FindPlayerWanted()->m_bPoliceBackOffGarage = true;
                CGarages::LastGaragePlayerWasIn = garageId;
                CallOffChase(true);
            };

            if (IsStaticPlayerCarEntirelyInside()) {
                const auto WaitForPlayerToExitForReason = [this](const char* reasonTxtKey, eAudioEvents ae) {
                    CGarages::TriggerMessage(reasonTxtKey, -1, 4000);
                    AudioEngine.ReportFrontendAudioEvent(ae);
                    m_DoorState = GARAGE_DOOR_WAITING_PLAYER_TO_EXIT;
                };
                if (CGarages::IsCarSprayable(plyrVeh)) {
                    if (CGarages::RespraysAreFree || FindPlayerInfo().m_nMoney >= RESPRAY_COST) {
                        m_DoorState = GARAGE_DOOR_CLOSING;
                        CPad::GetPad()->bPlayerAwaitsInGarage = true;
                        plyrVeh->m_fDirtLevel = 0.f;
                    } else {
                        WaitForPlayerToExitForReason("GA_3", AE_FRONTEND_CAR_NO_CASH);
                    }
                } else {
                    WaitForPlayerToExitForReason(plyrVeh->IsSubBMX() ? "GA_18" : "GA_1", AE_FRONTEND_CAR_IS_HOT);
                }
                ProcessPlayerIsInGarage();
                return;
            }

            if (!IsPlayerOutsideGarage()) {
                ProcessPlayerIsInGarage();
                return;
            }

            // Player is leaving garage now

            if (garageId == CGarages::LastGaragePlayerWasIn) {
                FindPlayerWanted()->m_bPoliceBackOffGarage = false;
            }

            CallOffChase(true);

            return;
        }
        case GARAGE_DOOR_CLOSING: { // 0x44AE0F
            if (plyrVeh) {
                ThrowCarsNearDoorOutOfGarage(plyrVeh);
            }
            if (SlideDoorClosed()) {
                m_DoorState = GARAGE_DOOR_CLOSED;
                AudioEngine.ReportFrontendAudioEvent(AE_BUY_CAR_RESPRAY);
                m_TimeToOpen = CTimer::GetTimeInMS() + 2000;
                CStats::IncrementStat(STAT_TOTAL_LEGITIMATE_KILLS, CStats::GetStatValue(STAT_KILLS_SINCE_LAST_CHECKPOINT));
                CStats::SetStatValue(STAT_KILLS_SINCE_LAST_CHECKPOINT, 0.f);
            }
            if (plyrVeh) {
                assert(plyrVeh->IsAutomobile());
                plyrVeh->AsAutomobile()->m_fBurnTimer = 0.f;
                plyrVeh->vehicleFlags.bDisableParticles = true;
            }
            CallOffChase(false);
            return;
        }
        case GARAGE_DOOR_OPENING: {// 0x44B79E
            ProessSlideDoor(GARAGE_DOOR_WAITING_PLAYER_TO_EXIT);
            return;
        }
        case GARAGE_DOOR_WAITING_PLAYER_TO_EXIT: {
            if (IsPlayerEntirelyInsideGarage()) {
                m_DoorState = GARAGE_DOOR_OPEN;
            }
            return;
        }
        }
        return;
    }
    case UNKN_CLOSESONTOUCH: { // 0x44BA04
        m_DoorState = [&, this] {
            switch (m_DoorState) {
            case GARAGE_DOOR_OPEN:
                if (IsGarageEmpty()) {
                    return GARAGE_DOOR_CLOSING;
                }
                break;
            case GARAGE_DOOR_OPENING:
                if (SlideDoorOpen()) {
                    return GARAGE_DOOR_OPEN;
                }
                break;
            case GARAGE_DOOR_CLOSING: {
                if (SlideDoorClosed()) {
                    return GARAGE_DOOR_CLOSED;
                }
                if (!IsGarageEmpty()) {
                    return GARAGE_DOOR_OPENING;
                }
                break;
            }
            }
            return m_DoorState; // No change            
        }();
        return;
    }
    case OPEN_FOR_TARGET_FREEZE_PLAYER:
    case CLOSE_WITH_CAR_DONT_OPEN_AGAIN: { // 0x44BA5C
        switch (m_DoorState) {
        case GARAGE_DOOR_CLOSED: {
            if (!plyrVeh || plyrVeh != m_TargetCar) { // NOTE/TODO: Pretty sure they made a mistake and it should've been `plyrVeh != m_TargetCar` [just note that in our case it's inverted, so here it should be `plyrVeh == m_TargetCar`]
                return;
            }
            if (CalcDistToGarageRectangleSquared(plyrVeh->GetPosition2D()) >= sq(17.f)) {
                return;
            }
            m_DoorState = GARAGE_DOOR_OPENING;
            return;
        }
        case GARAGE_DOOR_OPEN: { // 0x44BA89
            if (m_TargetCar && IsPlayerInRadiusOfGarage(30.f)) { // Inverted
                m_bClosingEmpty = true;
            } else {
                if (m_TargetCar != plyrVeh) {
                    return;
                }
                if (!IsStaticPlayerCarEntirelyInside()) {
                    return;
                }
                if (IsAnyCarBlockingDoor()) {
                    return;
                }
                m_bClosingEmpty = false;
                SetIsPlayerInGarage(true);
            }
            m_DoorState = GARAGE_DOOR_CLOSING;
            return;
        }
        case GARAGE_DOOR_CLOSING: { // 0x44BB8D
            if (m_TargetCar) {
                ThrowCarsNearDoorOutOfGarage(m_TargetCar);
            }
            if (!SlideDoorClosed()) {
                return;
            }
            if (m_bClosingEmpty) {
                m_DoorState = GARAGE_DOOR_CLOSED;
            } else {
                if (m_TargetCar) {
                    m_DoorState  = GARAGE_DOOR_CLOSED_DROPPED_CAR;
                    m_TimeToOpen = CTimer::GetTimeInMS() + 2000;
                    m_TargetCar  = nullptr;
                } else {
                    m_DoorState = GARAGE_DOOR_CLOSED;
                }
                SetIsPlayerInGarage(false);
            }
            return;
        }
        case GARAGE_DOOR_OPENING: {
            if (SlideDoorOpen()) {
                m_DoorState = GARAGE_DOOR_OPEN;
            }
            return;
        }
        case GARAGE_DOOR_CLOSED_DROPPED_CAR: {
            if (m_Type == OPEN_FOR_TARGET_FREEZE_PLAYER && CTimer::GetTimeInMS() > m_TimeToOpen) {
                m_DoorState = GARAGE_DOOR_OPENING;
            }
            return;
        }
        }
        return;
    }
    case SCRIPT_ONLY_OPEN: { // 0x44BCA4
        if (m_DoorState == GARAGE_DOOR_OPENING && SlideDoorOpen()) {
            m_DoorState = GARAGE_DOOR_OPEN;
        }
        return;
    }
    case SAFEHOUSE_GANTON:
    case SAFEHOUSE_SANTAMARIA:
    case SAGEHOUSE_ROCKSHORE:
    case SAFEHOUSE_FORTCARSON:
    case SAFEHOUSE_VERDANTMEADOWS:
    case SAFEHOUSE_DILLIMORE:
    case SAFEHOUSE_PRICKLEPINE:
    case SAFEHOUSE_WHITEWOOD:
    case SAFEHOUSE_PALOMINOCREEK:
    case SAFEHOUSE_REDSANDSWEST:
    case SAFEHOUSE_ELCORONA:
    case SAFEHOUSE_MULHOLLAND:
    case SAFEHOUSE_CALTONHEIGHTS:
    case SAFEHOUSE_PARADISO:
    case SAFEHOUSE_DOHERTY:
    case SAFEHOUSE_HASHBURY:
    case HANGAR_AT400:
    case HANGAR_ABANDONED_AIRPORT: { // 0x44BE5D
        const auto maxNumCarsInThiSafeHouse = m_Type == SAFEHOUSE_GANTON
            ? CGarages::MAX_CARS_IN_GANTON_SAFEHOUSE
            : CGarages::MAX_CARS_IN_SAFEHOUSE;
        switch (m_DoorState) {
        case GARAGE_DOOR_CLOSED: {
            if (plyrCoors.z >= 950.f) { // TODO: Magic 950
                return;
            }

            // Check player distance from garage
            const auto plyrDistSqToGrgRect = CalcDistToGarageRectangleSquared(plyrCoors);
            if (plyrDistSqToGrgRect >= sq(3.5f)) {
                if (plyrDistSqToGrgRect >= sq(10.f)) {
                    return;
                }
                if (!plyrVeh || plyrVeh->m_nVehicleSubType == VEHICLE_TYPE_BMX) {
                    return;
                }
            }

            if (   !plyrVeh
                || m_Type == HANGAR_AT400
                || CGarages::CountCarsInHideoutGarage(m_Type) < maxNumCarsInThiSafeHouse
            ) {
                if (m_Type == HANGAR_AT400 || !RestoreCarsForThisHideOut(CGarages::aCarsInSafeHouse[CGarages::FindSafeHouseIndexForGarageType(m_Type)])) {
                    m_DoorState = GARAGE_DOOR_OPENING; // 0x44C1EF
                }
            } else { // Display help message in some specific scnearios
                // Moved these out here, from 0x44C144, as it's pointless to do the stuff below, if in the end this is false
                if (CTimer::GetTimeInMS() - CGarages::LastTimeHelpMessage <= 18'000) {
                    return;
                }
                switch (plyrVeh->GetVehicleAppearance()) {
                case VEHICLE_APPEARANCE_HELI:
                case VEHICLE_APPEARANCE_PLANE:
                    return;
                }
                CObject* doors[2]{};
                FindDoorsWithGarage(doors[0], doors[1]);
                if (rng::none_of(doors, [&](CObject* door) {
                    if (!door) {
                        return false;
                    }
                    if ((door->GetPosition2D() - plyrVeh->GetPosition2D()).SquaredMagnitude() >= sq(5.f)) {
                        return false;
                    }
                    return true;
                })) {
                    return;
                }
                // 0x44C1B4
                CHud::SetHelpMessage(TheText.Get("GA_21"), 0, 0, 1);
                CGarages::LastTimeHelpMessage = CTimer::m_snTimeInMilliseconds;
            }
            return;
        }
        case GARAGE_DOOR_OPEN: { // 0x44BCFC
            const auto distSq = CalcDistToGarageRectangleSquared(plyrCoors);
            if (!IsAnyCarBlockingDoor()) {
                if (   distSq >= sq(25.f)
                    || distSq >= sq(4.f) && (!plyrVeh || plyrVeh->IsSubBMX())
                ) {
                    m_DoorState = GARAGE_DOOR_CLOSING;
                    return;
                }
            }
            if (plyrVeh) {
                if (IsPlayerOutsideGarage() && CountCarsWithCenterPointWithinGarage(plyrVeh) >= maxNumCarsInThiSafeHouse) {
                    m_DoorState = GARAGE_DOOR_CLOSING;
                    return;
                }
            }
            if (distSq >= sq(70.f)) {
                m_DoorState = GARAGE_DOOR_CLOSING;
                RemoveCarsBlockingDoorNotInside();
            }
            return;
        }
        case GARAGE_DOOR_CLOSING: { // 0x44BDF3
            //SlideDoorClosed(); // Um, okay? Maybe it's a typo and should be `SlideDoorClose()`?
            if (IsPlayerOutsideGarage()) { // Inverted
                if (m_DoorOpenness == 0.f) {
                    m_DoorState = GARAGE_DOOR_CLOSED;
                    if (m_Type != HANGAR_AT400) {
                        StoreAndRemoveCarsForThisHideOut(GetStoredCarsInThisSafehouse());
                    }
                }
            } else {
                m_DoorState = GARAGE_DOOR_OPENING;
            }
            return;
        }
        case GARAGE_DOOR_OPENING: { // 0x44BE53
            if (SlideDoorOpen()) {
                m_DoorState = GARAGE_DOOR_OPEN;
            }
            return;
        }
        }
        return;
    }
    case SCRIPT_CONTROLLED: { // 0x44BCAA
        switch (m_DoorState) {
        case GARAGE_DOOR_CLOSING:
            if (SlideDoorClosed()) {
                m_DoorState = GARAGE_DOOR_CLOSED;
            }
            break;
        case GARAGE_DOOR_OPENING:
            if (SlideDoorOpen()) {
                m_DoorState = GARAGE_DOOR_OPEN;
            }
            break;
        }
        return;
    }
    case STAY_OPEN_WITH_CAR_INSIDE: {
        switch (m_DoorState) {
        case GARAGE_DOOR_CLOSED: {
            if (plyrVeh && plyrVeh == m_TargetCar) {
                SetDoorOpeningIfPlayerVehicleClose();
            }
            return;
        }
        case GARAGE_DOOR_OPEN: {
            if (IsPlayerInRadiusOfGarage(30.f)) {
                return;
            }
            if (!m_TargetCar || !IsEntityEntirelyOutside(m_TargetCar)) {
                return;
            }
            m_bClosingEmpty = true;
            m_DoorState = GARAGE_DOOR_CLOSING;
            return;
        }
        case GARAGE_DOOR_CLOSING: {
            if (m_TargetCar) {
                ThrowCarsNearDoorOutOfGarage(m_TargetCar);
            }
            if (SlideDoorClosed()) {
                m_DoorState = GARAGE_DOOR_CLOSED;
            }
            return;
        }
        case GARAGE_DOOR_OPENING: {
            if (SlideDoorOpen()) {
                m_DoorState = GARAGE_DOOR_OPEN;
            }
            return;
        }
        }
        return;
    }
    case SCRIPT_OPEN_FREEZE_WHEN_CLOSING: { // 0x44C2F3
        switch (m_DoorState) {
        case GARAGE_DOOR_OPEN: {
            if (!m_TargetCar || !IsEntityEntirelyInside3D(m_TargetCar)) {
                return;
            }
            if (IsAnyCarBlockingDoor()) {
                return;
            }
            if (!IsPlayerOutsideGarage()) {
                return;
            }
            CPad::GetPad()->bPlayerAwaitsInGarage = true;
            m_bClosingEmpty = false;
            return;
        }
        case GARAGE_DOOR_CLOSING: { // 0x44C303
            if (SlideDoorClosed()) {
                m_DoorState = GARAGE_DOOR_CLOSED;
                CPad::GetPad()->bPlayerAwaitsInGarage = false;
            }
            return;
        }
        case GARAGE_DOOR_OPENING: { 
            if (SlideDoorOpen()) { // 0x44C2FB
                m_DoorState = GARAGE_DOOR_OPEN;
            }
            return;
        }
        }
        return;
    }
    case IMPOUND_LS:
    case IMPOUND_LV:
    case IMPOUND_SF: {
        const auto playerInBBZ = plyrCoors.z <= m_CeilingZ - 2.f && m_Base.z + 2.f >= plyrCoors.z;
        switch (m_DoorState) {
        case GARAGE_DOOR_OPEN: 
        case GARAGE_DOOR_CLOSING: { // 0x44C3B5
            if (!playerInBBZ || m_DoorState == GARAGE_DOOR_CLOSING || CalcDistToGarageRectangleSquared(plyrCoors) >= sq(65.f)) {
                m_DoorState = GARAGE_DOOR_CLOSED;
                StoreAndRemoveCarsForThisHideOut(GetStoredCarsInThisSafehouse(), CGarages::MAX_CARS_IN_IMPOUND);
            }
            return;
        }
        case GARAGE_DOOR_CLOSED: { // 0x44C490
            if (!playerInBBZ) {
                return;
            }
            if (CalcDistToGarageRectangleSquared(plyrCoors) >= sq(36.f)) {
                return;
            }
            const auto cars = GetStoredCarsInThisSafehouse();
            NeatlyLineUpStoredCars(cars);
            if (RestoreCarsForThisImpoundingGarage(cars)) {
                m_DoorState = GARAGE_DOOR_OPEN;
            }
            return;
        }
        }
        return;
    }
    case TUNING_LOCO_LOW_CO:
    case TUNING_WHEEL_ARCH_ANGELS:
    case TUNING_TRANSFENDER: {
        switch (m_DoorState) {
        case GARAGE_DOOR_CLOSED: { // 0x44C6C0
            if (RightModTypeForThisGarage(plyrVeh)) {
                SetDoorOpeningIfPlayerVehicleClose();
            }
            return;
        }
        case GARAGE_DOOR_OPEN: {
            if (IsPlayerInRadiusOfGarage(30.f)) {
                return;
            }
            if (RightModTypeForThisGarage(plyrVeh) && IsEntityTouching3D(plyrVeh)) {
                return;
            }
            if (IsAnyOtherCarTouchingGarage()) {
                return;
            }
            m_bClosingEmpty = true;
            m_DoorState = GARAGE_DOOR_CLOSING;
            return;
        }
        case GARAGE_DOOR_CLOSING: {
            if (plyrVeh) {
                ThrowCarsNearDoorOutOfGarage(plyrVeh);
            }
            if (SlideDoorClosed()) {
                m_DoorState = GARAGE_DOOR_CLOSED;
            }
            return;
        }
        case GARAGE_DOOR_OPENING: {
            if (SlideDoorOpen()) {
                m_DoorState = GARAGE_DOOR_OPEN;
            }
            return;
        }
        }
        return;
    }
    case BURGLARY: { // 0x44C70A
        switch (m_DoorState) { // NOTE/TODO: Where's `case GARAGE_DOOR_CLOSED`?
        case GARAGE_DOOR_OPEN: {
            if (!IsPlayerInRadiusOfGarage(30.f) && !IsAnyOtherCarTouchingGarage()) {
                m_DoorState = GARAGE_DOOR_CLOSING;
            }
            return;
        }
        case GARAGE_DOOR_CLOSING: {
            if (plyrVeh) {
                ThrowCarsNearDoorOutOfGarage(plyrVeh);
            }
            if (SlideDoorClosed()) {
                m_DoorState = GARAGE_DOOR_CLOSED;
            }
            return;
        }
        case GARAGE_DOOR_OPENING: {
            if (SlideDoorOpen()) {
                m_DoorState = GARAGE_DOOR_OPEN;
            }
            return;
        }
        }
        return;
    }
    default: NOTSA_UNREACHABLE();
    }
}

bool CGarage::RightModTypeForThisGarage(CVehicle* vehicle) {
    return plugin::CallMethodAndReturn<bool, 0x447720>(this, vehicle);
}

bool CGarage::IsHideOut() const {
    switch (m_Type) {
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

bool CGarage::IsImpound() const {
    return notsa::contains({ IMPOUND_LS, IMPOUND_LV, IMPOUND_SF }, m_Type);
}

CVector2D CGarage::GetCenterOffset() const {
    return m_DirA * (m_DirA / 2.f)
         + m_DirB * (m_DirB / 2.f);
}

CBoundingBox CGarage::GetAABB() const {
    return {
        CVector{m_MinX, m_MinY, m_Base.z},
        CVector{m_MaxX, m_MaxY, m_CeilingZ}
    };
}

notsa::shapes::AngledRect CGarage::GetBaseAngledRect() const {
    return notsa::shapes::AngledRect{
        m_Base,
        -m_DirA, // AngledRect's AB goes left to right, but ours is vice versa, so flip it
        m_LenA,
        m_DirB,
        m_LenB
    };
}

CRect CGarage::GetAARect() const {
    return {
        m_MinX, m_MinY,
        m_MaxX, m_MaxY,
    };
}

notsa::shapes::AngledBox CGarage::GetBB() const {
    return {
        GetBaseAngledRect(),
        m_Base.z,
        m_CeilingZ
    };
}

bool CGarage::IsVehicleTypeAllowedInside(CVehicle* veh) {
    switch (veh->m_nVehicleType) {
    case VEHICLE_TYPE_BIKE:
    case VEHICLE_TYPE_AUTOMOBILE:
        return true;
    }
    return false;
}

CStoredCar* CGarage::GetStoredCarsInThisSafehouse() {
    return CGarages::GetStoredCarsInSafehouse(CGarages::FindSafeHouseIndexForGarageType(m_Type));
}

// 0x449FF0
void CGarage::FindDoorsWithGarage(CObject*& door1, CObject*& door2) {
    return plugin::CallMethod<0x449FF0>(this, &door1, &door2);
}

bool CGarage::SlideDoorOpen() {
    return plugin::CallMethodAndReturn<bool, 0x44A660>(this);
}

bool CGarage::SlideDoorClosed() {
    return plugin::CallMethodAndReturn<bool, 0x44A750>(this);
}

// 0x44A9C0
bool CGarage::IsGarageEmpty() {
    int16 countEntities;
    std::array<CEntity*, 16> entities;
    CWorld::FindObjectsIntersectingBB(GetAABB(), &countEntities, entities, false, true, true, false, false);
    return rng::all_of(entities | rngv::take(countEntities), [this](auto&& e) {
        return !IsEntityTouching3D(e);
    });
}

// 0x447D80
float CGarage::CalcDistToGarageRectangleSquared(CVector2D pt) {
    return GetAARect().DistSqToPt(pt);
}

// Based on 0x4476D0
void CGarage::ChangeType(eGarageType newType) {
    m_Type = newType;
    switch (newType) {
    case BOMBSHOP_TIMED:
    case BOMBSHOP_ENGINE:
    case BOMBSHOP_REMOTE:
    case PAYNSPRAY: {
        if (IsClosed()) {
            SetOpened();
            m_DoorOpenness = 1.0f;
        }
        break;
    }
    case BURGLARY:
        break;
    default: {
        SetClosed();
        ResetDoorPosition();
        break;
    }
    }
}

// Based on 0x447CD0
void CGarage::Activate() {
    m_bInactive = false;
    if (m_Type == eGarageType::UNKN_CLOSESONTOUCH && IsClosed()) {
        SetOpened();
    }
}

void CGarage::DeActivate() {
    m_bInactive = true;
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

bool CGarage::IsAnyCarBlockingDoor() {
    return plugin::CallMethodAndReturn<bool, 0x4494F0>(this);
}

// 0x4495F0
size_t CGarage::CountCarsWithCenterPointWithinGarage(CVehicle* ignoredVehicle) {
    return plugin::CallMethodAndReturn<bool, 0x4495F0>(this, ignoredVehicle);
}

void CGarage::NeatlyLineUpStoredCars(CStoredCar* cars) {
    plugin::CallMethod<0x448330>(this, cars);
}

// 0x448550
bool CGarage::RestoreCarsForThisHideOut(CStoredCar* cars) {
    return plugin::CallMethodAndReturn<bool, 0x448550>(this, cars);
}

bool CGarage::RestoreCarsForThisImpoundingGarage(CStoredCar* cars) {
    return plugin::CallMethodAndReturn<bool, 0x4485C0>(this, cars);
}

void CGarage::StoreAndRemoveCarsForThisImpoundingGarage(CStoredCar* cars, uint32 numCarsToStore) {
    plugin::CallMethod<0x449A50>(cars, numCarsToStore);
}

// 0x5D3020
void CSaveGarage::CopyGarageIntoSaveGarage(Const CGarage& g) {
    m_nType         = g.m_Type;
    m_nDoorState    = g.m_DoorState;
    m_nFlags        = g.m_nFlags;
    m_vPosn         = g.m_Base;
    m_vDirectionA   = g.m_DirA;
    m_vDirectionB   = g.m_DirB;
    m_fTopZ         = g.m_CeilingZ;
    m_fWidth        = g.m_LenA;
    m_fHeight       = g.m_LenB;
    m_fLeftCoord    = g.m_MinX;
    m_fRightCoord   = g.m_MaxX;
    m_fFrontCoord   = g.m_MinY;
    m_fBackCoord    = g.m_MaxY;
    m_fDoorPosition = g.m_DoorOpenness;
    m_nTimeToOpen   = g.m_TimeToOpen;
    m_nOriginalType = g.m_OriginalType;
    strcpy_s(m_anName, g.m_Name);
}

// 0x5D30C0
void CSaveGarage::CopyGarageOutOfSaveGarage(CGarage& g) const {
    g.m_Type         = m_nType;
    g.m_DoorState    = m_nDoorState;
    g.m_nFlags        = m_nFlags;
    g.m_Base         = m_vPosn;
    g.m_DirA   = m_vDirectionA;
    g.m_DirB   = m_vDirectionB;
    g.m_CeilingZ         = m_fTopZ;
    g.m_LenA        = m_fWidth;
    g.m_LenB       = m_fHeight;
    g.m_MinX    = m_fLeftCoord;
    g.m_MaxX   = m_fRightCoord;
    g.m_MinY   = m_fFrontCoord;
    g.m_MaxY    = m_fBackCoord;
    g.m_DoorOpenness = m_fDoorPosition;
    g.m_TimeToOpen   = m_nTimeToOpen;
    g.m_OriginalType = m_nOriginalType;
    g.m_TargetCar    = nullptr;
    strcpy_s(g.m_Name, m_anName);
}
