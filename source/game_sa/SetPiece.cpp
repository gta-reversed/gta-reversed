#include "StdInc.h"
#include "Automobile.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskComplexBeCop.h"

void CSetPiece::InjectHooks() {
    RH_ScopedClass(CSetPiece);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(TryToGenerateCopPed, 0x499690);
    RH_ScopedInstall(TryToGenerateCopCar, 0x4998A0);
    RH_ScopedInstall(Update, 0x499A80);
}

// 0x499690
CCopPed* CSetPiece::TryToGenerateCopPed(CVector2D posn) {
    CVector   point{ posn, 1000.0f };
    CColPoint cp{};
    CEntity*  entity{};
    if (CWorld::ProcessVerticalLine(
        point,
        -1000.0f,
        cp,
        entity,
        true,
        false,
        false,
        false,
        true
    )) {
        point.z = cp.m_vecPoint.z + 0.9f;
    }

    int16 numCollidingObjects{};
    CWorld::FindObjectsKindaColliding(
        point,
        0.6f,
        false,
        &numCollidingObjects,
        16,
        nullptr,
        false,
        true,
        true,
        false,
        false
    );

    if (!numCollidingObjects) {
        return nullptr;
    }

    auto* cop = new CCopPed(0);
    cop->SetPosn(point);

    NOTSA_LOG_DEBUG("generated cop at ({}, {}, {}) dist to player {:.2f}", cop->GetPosition().x, cop->GetPosition().y, cop->GetPosition().z, DistanceBetweenPoints(FindPlayerCoors(), cop->GetPosition()));
    CWorld::Add(cop);
    return cop;
}

// 0x4998A0
CVehicle* CSetPiece::TryToGenerateCopCar(CVector2D posn, CVector2D target) {
    const auto carModel = CStreaming::GetDefaultCopCarModel();
    if (carModel < 0) {
        return nullptr;
    }

    auto* car = new CAutomobile(carModel, RANDOM_VEHICLE, true);

    CVector point{ posn, 1000.0f };
    CColPoint cp{};
    CEntity*  entity{};
    if (CWorld::ProcessVerticalLine(
        point,
        -1000.0f,
        cp,
        entity,
        true,
        false,
        false,
        false,
        true
    )) {
        point.z = car->GetHeightAboveRoad() + cp.m_vecPoint.z;
    }

    const CVector dir{ (target - posn).Normalized() };
    // TODO: matrix assign shit
    car->GetForward()  = dir;
    car->GetRight()    = CVector{ dir.y, -dir.x, 0.0f };
    car->GetUp()       = CVector::ZAxisVector();
    car->GetPosition() = point;

    int16 numCollidingObjects{};
    CWorld::FindObjectsKindaColliding(
        point,
        car->GetColModel()->GetBoundRadius(),
        false,
        &numCollidingObjects,
        16,
        nullptr,
        false,
        true,
        true,
        false,
        false
    );

    if (!numCollidingObjects) {
        car->ChangeLawEnforcerState(true);
        NOTSA_LOG_DEBUG("generated cop car at ({}, {}, {}) dist to player {:.2f}", car->GetPosition().x, car->GetPosition().y, car->GetPosition().z, DistanceBetweenPoints(FindPlayerCoors(), car->GetPosition()));
        CWorld::Add(car);
    } else {
        delete std::exchange(car, nullptr);
    }

    return car;
}

// 0x499A80
void CSetPiece::Update() {
    if (m_nLastGenerationTime != 0 && CTimer::GetTimeInMS() <= m_nLastGenerationTime + 4'000u) {
        return;
    }

    const auto playerPos = FindPlayerCoors();
    if (!CRect{ m_AreaRect }.IsPointInside(playerPos)) {
        return;
    }

    const auto SetupCop = [this](const CVector2D& spawn, const CVector2D& target) -> CCopPed* {
        if (auto* cop = TryToGenerateCopPed(spawn)) {
            cop->bCullExtraFarAway = true;
            cop->m_nTimeTillWeNeedThisPed = CTimer::GetTimeInMS() + 10'000;

            cop->GetTaskManager().SetTask(new CTaskComplexBeCop{
                PEDMOVE_WALK,
                false,
                new CTaskComplexGoToPointAndStandStill{
                    PEDMOVE_RUN,
                    CWorld::AddGroundZToCoord(target),
                    0.5f
                }
            }, TASK_PRIMARY_PRIMARY);
            m_nLastGenerationTime = CTimer::GetTimeInMS();

            return cop;
        }
        return nullptr;
    };

    const auto SetupCar = [this](const CVector2D& spawn, const CVector2D& target, eCarDrivingStyle style, eCarMission mission, uint8 speed, uint32 timeNeeded, bool tempAction = false, bool addSpeed = false) -> CVehicle* {
        if (auto* car = TryToGenerateCopCar(spawn, target)) {
            car->m_nStatus                         = STATUS_SIMPLE;
            car->m_autoPilot.m_nCruiseSpeed        = speed;
            car->m_autoPilot.m_nCarDrivingStyle    = style;
            car->m_autoPilot.m_nCarMission         = mission;
            car->m_autoPilot.m_speed               = car->m_autoPilot.m_nCruiseSpeed;
            car->m_autoPilot.m_vecDestinationCoors = CVector{ target };
            car->m_nTimeTillWeNeedThisCar          = CTimer::GetTimeInMS() + timeNeeded;
            if (tempAction) {
                car->m_autoPilot.m_nTempAction = 9;
                car->m_autoPilot.m_nTempActionTime = CTimer::GetTimeInMS() + 100;
            }
            if (addSpeed) {
                car->GetMoveSpeed() = car->GetForward() * 2.0f / 3.0f;
            }
            CCarAI::AddPoliceCarOccupants(car, false);

            return car;
        }
        return nullptr;
    };

    const auto SetupQuickCar = [&](const CVector2D& spawn, const CVector2D& target, eCarDrivingStyle style, eCarMission mission) {
        if ((FindPlayerCoors() - spawn).Dot(FindPlayerSpeed()) >= 0.0f) {
            return;
        }

        if (SetupCar(
            spawn,
            target,
            style,
            mission,
            16,
            10'000,
            true,
            true
        )) {
            m_nLastGenerationTime = CTimer::GetTimeInMS();
        }
    };

    switch (m_nType) {
    case SETPIECE_2CARS_SLOW_SPEED:
    case SETPIECE_2CARS_MEDIUM_SPEED: {
        if (FindPlayerPed()->GetWantedLevel() < 1 || FindPlayerVehicle()) {
            break;
        }

        auto* car1 = SetupCar(
            GetSpawnCoord1(),
            GetTargetCoord1(),
            DRIVING_STYLE_SLOW_DOWN_FOR_CARS,
            MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_1,
            (m_nType == SETPIECE_2CARS_SLOW_SPEED) ? 4 : 24,
            25'000
        );
        if (!car1) {
            break;
        }

        if (!SetupCar(
            GetSpawnCoord2(),
            GetTargetCoord2(),
            DRIVING_STYLE_SLOW_DOWN_FOR_CARS,
            MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_1,
            (m_nType == SETPIECE_2CARS_SLOW_SPEED) ? 4 : 24,
            25'000
        )) {
            CWorld::Remove(car1);
            delete car1;
            break;
        }

        m_nLastGenerationTime = CTimer::GetTimeInMS();
        break;
    }
    case SETPIECE_1CAR_QUICK_SPEED: {
        if (FindPlayerPed()->GetWantedLevel() < 2 || !FindPlayerVehicle()) {
            break;
        }

        SetupQuickCar(GetSpawnCoord1(), GetTargetCoord1(), DRIVING_STYLE_PLOUGH_THROUGH, MISSION_BLOCKPLAYER_FORWARDANDBACK);
        break;
    }
    case SETPIECE_1CAR_MEDIUM_SPEED: {
        if (const auto* p = FindPlayerPed(); p->GetWantedLevel() < 2 || !FindPlayerVehicle()) {
            break;
        }

        SetupQuickCar(GetSpawnCoord1(), GetTargetCoord1(), DRIVING_STYLE_AVOID_CARS, MISSION_RAMPLAYER_CLOSE);
        break;
    }
    case SETPIECE_1PED:
        if (FindPlayerPed()->GetWantedLevel() < 1 && FindPlayerVehicle()) {
            break;
        }

        if (!SetupCop(GetSpawnCoord1(), GetTargetCoord1())) {
            break;
        }
        m_nLastGenerationTime = CTimer::GetTimeInMS();
        break;
    case SETPIECE_2PEDS: {
        if (FindPlayerPed()->GetWantedLevel() < 1 && FindPlayerVehicle()) {
            break;
        }

        auto* cop1 = SetupCop(GetSpawnCoord1(), GetTargetCoord1());
        if (!cop1) {
            break;
        }

        if (!SetupCop(GetSpawnCoord2(), GetTargetCoord2())) {
            CWorld::Remove(cop1);
            delete cop1;
            break;
        }
        m_nLastGenerationTime = CTimer::GetTimeInMS();
        break;
    }
    case SETPIECE_2CARS_QUICK_SPEED_BEFORE_HIT: {
        if (FindPlayerPed()->GetWantedLevel() < 2 && FindPlayerVehicle()) {
            break;
        }

        // NOTE: No check of the first car
        SetupQuickCar(GetSpawnCoord1(), GetTargetCoord1(), DRIVING_STYLE_PLOUGH_THROUGH, MISSION_BLOCKPLAYER_FORWARDANDBACK);
        SetupQuickCar(GetSpawnCoord2(), GetTargetCoord2(), DRIVING_STYLE_PLOUGH_THROUGH, MISSION_BLOCKPLAYER_FORWARDANDBACK);
        break;
    }
    case SETPIECE_2CARS_QUICK_SPEED: {
        if (FindPlayerPed()->GetWantedLevel() < 2 && FindPlayerVehicle()) {
            break;
        }

        // NOTE: No check of the first car
        SetupQuickCar(GetSpawnCoord1(), GetTargetCoord1(), DRIVING_STYLE_AVOID_CARS, MISSION_RAMPLAYER_CLOSE);
        SetupQuickCar(GetSpawnCoord2(), GetTargetCoord2(), DRIVING_STYLE_AVOID_CARS, MISSION_RAMPLAYER_CLOSE);
        break;
    }
    default:
        break;
    }
}
