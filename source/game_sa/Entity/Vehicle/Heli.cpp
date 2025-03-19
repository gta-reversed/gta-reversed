/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"
#include "WindModifiers.h"
#include "Entity.h"

void CHeli::InjectHooks() {
    RH_ScopedVirtualClass(CHeli, 0x871680, 71);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(InitHelis, 0x6C4560);
    RH_ScopedInstall(AddHeliSearchLight, 0x6C45B0);
    RH_ScopedInstall(Pre_SearchLightCone, 0x6C4650);
    RH_ScopedInstall(Post_SearchLightCone, 0x6C46E0);
    RH_ScopedInstall(SwitchPoliceHelis, 0x6C4800);
    RH_ScopedInstall(RenderAllHeliSearchLights, 0x6C7C50);
    RH_ScopedInstall(TestSniperCollision, 0x6C6890);
    RH_ScopedInstall(UpdateHelis, 0x6C79A0);
    RH_ScopedVMTInstall(ProcessFlyingCarStuff, 0x6C4E60, {.reversed = false});
    RH_ScopedVMTInstall(Render, 0x6C4400);
    RH_ScopedVMTInstall(Fix, 0x6C4530);
    RH_ScopedVMTInstall(BurstTyre, 0x6C4330);
    RH_ScopedVMTInstall(SetUpWheelColModel, 0x6C4320);
}

// 0x6C4190
CHeli::CHeli(int32 modelIndex, eVehicleCreatedBy createdBy) : CAutomobile(modelIndex, createdBy, true) {
    m_nVehicleSubType = VEHICLE_TYPE_HELI;

    m_fYawControl                        = 0.0;
    m_fPitchControl                      = 0.0;
    m_fRollControl                       = 0.0;
    m_fThrottleControl                   = 0.0;
    m_fEngineSpeed                       = 0.0;
    m_fMainRotorAngle                    = 0.0;
    m_fRearRotorAngle                    = 0.0;
    m_MinHeightAboveTerrain              = 10.0;
    m_LowestFlightHeight                 = 10.0;
    m_DesiredHeight                      = 10.0;
    m_FlightDirection                    = 0.0;
    m_nHeliFlags.bStopFlyingForAWhile    = 0;
    m_nHeliFlags.bUseSearchLightOnTarget = 0;
    m_nHeliFlags.bWarnTarger             = 0; // fix
    m_LightBrightness                    = 0.0;

    physicalFlags.bDontCollideWithFlyers = true;

    if (modelIndex == MODEL_HUNTER) {
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_OK);
        m_doors[DOOR_LEFT_FRONT].m_fOpenAngle = (3.0f * PI) / 10.0f;
        m_doors[DOOR_LEFT_FRONT].m_fClosedAngle = 0.0f;
        m_doors[DOOR_LEFT_FRONT].m_nAxis = 1;
        m_doors[DOOR_LEFT_FRONT].m_nDirn = 19;
    }

    m_nSwatOnBoard = 4;
    m_SwatRopeActive.fill(0);

    m_LastSearchLightSample = CTimer::GetTimeInMS();

    m_OldSearchLightX.fill(0.0f);
    m_OldSearchLightY.fill(0.0f);

    m_LastTimeSearchLightWasTooFarAwayToShoot = 0;
    m_nNextTalkTimer = CTimer::GetTimeInMS();

    vehicleFlags.bNeverUseSmallerRemovalRange = true; // 0x6C42BD
    m_autoPilot.m_ucHeliTargetDist2 = 10;

    m_GunflashFxPtrs = nullptr;
    m_FiringRateMultiplier = 16;

    m_bStopFlyingForAWhile = 0;
    m_bSearchLightOn = false;
    m_crashAndBurnTurnSpeed = CGeneral::GetRandomNumberInRange(2.f, 8.f);
}

// 0x6C4340
CHeli::~CHeli() {
    if (m_GunflashFxPtrs) {
        for (auto i = 0; i < CVehicle::GetPlaneNumGuns(); i++) {
            if (auto& fx = m_GunflashFxPtrs[i]) {
                fx->Kill();
                g_fxMan.DestroyFxSystem(fx);
            }
        }
        delete[] m_GunflashFxPtrs;
        m_GunflashFxPtrs = nullptr;
    }

    m_vehicleAudio.Terminate();
}

// 0x6C4560
void CHeli::InitHelis() {
    std::ranges::fill(pHelis, nullptr);
    for (auto& light : HeliSearchLights) {
        light.Init();
    }
    NumberOfSearchLights = 0;
    bPoliceHelisAllowed = true;
}

// 0x6C45B0
void CHeli::AddHeliSearchLight(const CVector& origin, const CVector& target, float targetRadius, float power, uint32 coronaIndex, uint8 unknownFlag, uint8 drawShadow) {
    auto& light = HeliSearchLights[NumberOfSearchLights];

    light.m_vecOrigin     = origin;
    light.m_vecTarget     = target;
    light.m_fTargetRadius = targetRadius;
    light.m_fPower        = power;
    light.m_nCoronaIndex  = coronaIndex;
    light.field_24        = unknownFlag;
    light.m_bDrawShadow   = drawShadow;

    NumberOfSearchLights += 1;
}

// 0x6C4640
void CHeli::PreRenderAlways() {
    // NOP
}

// 0x6C4650
void CHeli::Pre_SearchLightCone() {
    ZoneScoped;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,             RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,            RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,        RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,            RWRSTATE(rwSHADEMODEGOURAUD));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION,    RWRSTATE(rwALPHATESTFUNCTIONGREATEREQUAL));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(0));
}

// 0x6C46E0
void CHeli::Post_SearchLightCone() {
    ZoneScoped;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,             RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,            RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATECULLMODE,             RWRSTATE(rwCULLMODECULLBACK));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION,    RWRSTATE(rwALPHATESTFUNCTIONGREATER));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(2u));
}

// 0x6C4750
void CHeli::SpecialHeliPreRender() {
    // NOP
}

// 0x6C4760
CVector CHeli::FindSwatPositionRelativeToHeli(int32 swatNumber) {
    CVector result;
    ((void(__thiscall*)(CHeli*, CVector*, int32))0x6C4760)(this, &result, swatNumber);
    return result;

    switch ( swatNumber ) {
    case 0:
        return { -1.2f, -1.0f, -0.5f };
    case 1:
        return { 1.2f,  -1.0f, -0.5f };
    case 2:
        return { -1.2f, 1.0f,  -0.5f };
    case 3:
        return { 1.2f,  1.0f,  -0.5f };
    default:
        return { 0.0f,  0.0f,  0.0f  };
    }
}

// 0x6C4800
void CHeli::SwitchPoliceHelis(bool enable) {
    bPoliceHelisAllowed = enable;
}

// 0x6C58E0
void CHeli::SearchLightCone(int32 coronaIndex,
                            CVector origin,
                            CVector target,
                            float targetRadius,
                            float power,
                            uint8 unknownFlag,
                            uint8 drawShadow,
                            CVector& useless0,
                            CVector& useless1,
                            CVector& useless2,
                            bool a11,
                            float baseRadius,
                            float a13,
                            float a14,
                            float a15
) {
    ((void(__cdecl*)(int32, CVector, CVector, float, float, uint8, uint8, CVector&, CVector&, CVector&, bool, float, float, float, float))0x6C58E0)(coronaIndex, origin, target, targetRadius, power, unknownFlag, drawShadow, useless0, useless1, useless2, a11, baseRadius, a13, a14, a15);
}

// 0x6C6520
CHeli* CHeli::GenerateHeli(CPed* target, bool newsHeli) {
    return ((CHeli * (__cdecl*)(CPed*, bool))0x6C6520)(target, newsHeli);
}

// 0x6C6890
void CHeli::TestSniperCollision(CVector* origin, CVector* target) {
    CVector point = *target - *origin;

    if (point.z >= point.Magnitude() / 2.0f)
        return;

    for (auto& heli : pHelis) {
        if (!heli || heli->physicalFlags.bBulletProof)
            continue;

        const auto mat = (CMatrix*)heli->m_matrix;
        if (CCollision::DistToLine(*origin, *target, mat->TransformPoint({ -0.43f, 1.49f, 1.5f })) < 0.8f) {
            heli->m_fRotationBalance = (float)(CGeneral::GetRandomNumber() < pow(2, 14) - 1) * 0.1f - 0.05f; // 2^14 - 1 = 16383 [-0.05, 0.05]
            heli->BlowUpCar(FindPlayerPed(), false);
            heli->m_nSwatOnBoard = 0;
        };
    }
}

// 0x6C69C0
bool CHeli::SendDownSwat() {
    return ((bool(__thiscall*)(CHeli*))0x6C69C0)(this);
}

// 0x6C79A0
void CHeli::UpdateHelis() {
    bool hasValidPolmav;
    int currentHeliCount;
    int maxAllowedHelis;
    CVector playerPosition;
    
    // Reset search light count
    CHeli::NumberOfSearchLights = 0;
    
    // Check existing helicopters status
    currentHeliCount = 0;
    hasValidPolmav = false;
    
    // Create a lambda for checking valid helicopters
    auto IsHeliValid = [](CHeli* heli) -> bool {
        return heli != nullptr;
    };
    
    // Check first helicopter
    if (IsHeliValid(CHeli::pHelis[0])) {
        hasValidPolmav = CHeli::pHelis[0]->m_nModelIndex == MODEL_POLMAV && 
                            !CHeli::pHelis[0]->physicalFlags.bRenderScorched && 
                            !CHeli::pHelis[0]->vehicleFlags.bIsDrowning;
        currentHeliCount = 1;
    }
    
    // Check second helicopter
    if (IsHeliValid(CHeli::pHelis[1])) {
        currentHeliCount++;
        if (CHeli::pHelis[1]->m_nModelIndex == MODEL_POLMAV && !CHeli::pHelis[1]->physicalFlags.bRenderScorched) {
            hasValidPolmav = hasValidPolmav || !CHeli::pHelis[1]->vehicleFlags.bIsDrowning;
        }
    }
    
    // Check game conditions for helicopters
    if (IsHeliValid(CHeli::pHelis[0])) {
        hasValidPolmav &= CHeli::pHelis[0]->m_nModelIndex != MODEL_VCNMAV;
    }
    
    if (IsHeliValid(CHeli::pHelis[1])) {
        hasValidPolmav &= CHeli::pHelis[1]->m_nModelIndex != MODEL_VCNMAV;
    }

    // Determine max allowed helicopters based on game conditions
    maxAllowedHelis = (CHeli::bPoliceHelisAllowed
                       && !CCullZones::PlayerNoRain()
                       && !CGame::IsInInterior()
                       && CWeather::OldWeatherType != WEATHER_SANDSTORM_DESERT
                       && CWeather::NewWeatherType != WEATHER_SANDSTORM_DESERT)
        ? FindPlayerWanted()->NumOfHelisRequired()
        : 0;

    // Check if news helicopter is allowed
    const bool useNewsHeliAllowed = hasValidPolmav && CWanted::bUseNewsHeliInAdditionToPolice;
    const int modelIndexOffset = useNewsHeliAllowed ? MODEL_VCNMAV : MODEL_POLMAV;
    
    // Generate new helicopter if needed
    if (CStreaming::ms_aInfoForModel[modelIndexOffset].m_LoadState == 1 && 
        CTimer::GetTimeInMS() > CHeli::TestForNewRandomHelisTimer) {
        
        CHeli::TestForNewRandomHelisTimer = CTimer::GetTimeInMS() + 15000;
        
        if (currentHeliCount < maxAllowedHelis) {
            CPlayerPed* playerPed = FindPlayerPed();
            CHeli* newHeli = CHeli::GenerateHeli(playerPed, useNewsHeliAllowed);
            
            // Register new helicopter - corregido según definición de plantilla
            if (CHeli::pHelis[0]) {
                if (!CHeli::pHelis[1]) {
                    CHeli::pHelis[1] = newHeli;
                    CEntity::RegisterReference(CHeli::pHelis[1]);
                }
            } else {
                CHeli::pHelis[0] = newHeli;
                CEntity::RegisterReference(CHeli::pHelis[0]);
            }
        }
    }
    
    // Process first helicopter
    if (IsHeliValid(CHeli::pHelis[0])) {
        // Check if destroyed
        if (CHeli::pHelis[0]->physicalFlags.bRenderScorched || CHeli::pHelis[0]->vehicleFlags.bIsDrowning) {
            CHeli::pHelis[0]->m_autoPilot.m_nCarMission = MISSION_HELI_FLY_AWAY_FROM_PLAYER;
            CHeli::pHelis[0] = nullptr;
        } 
        else if (CHeli::pHelis[0]->m_autoPilot.m_nCarMission == MISSION_HELI_FLY_AWAY_FROM_PLAYER) {
            // Check distance to player for removal
            playerPosition = FindPlayerCoors();
            CVector camDist = playerPosition - CHeli::pHelis[0]->GetPosition();
            float distanceToPlayer = camDist.Magnitude();

            if (distanceToPlayer > 170.0f) {
                CWorld::Remove(CHeli::pHelis[0]);
                if (CHeli::pHelis[0]) {
                    delete std::exchange(CHeli::pHelis[0], nullptr);
                }
                CHeli::pHelis[0] = nullptr;
            }
        }
    }
    
    // Process second helicopter
    if (IsHeliValid(CHeli::pHelis[1])) {
        // Check if destroyed
        if (CHeli::pHelis[1]->physicalFlags.bRenderScorched || CHeli::pHelis[1]->vehicleFlags.bIsDrowning) {
            CHeli::pHelis[1]->m_autoPilot.m_nCarMission = MISSION_HELI_FLY_AWAY_FROM_PLAYER;
            CHeli::pHelis[1] = nullptr;
        } else if (CHeli::pHelis[1]->m_autoPilot.m_nCarMission == MISSION_HELI_FLY_AWAY_FROM_PLAYER) {
            // Check distance to player for removal
            playerPosition = FindPlayerCoors();
            CVector camDist = playerPosition - CHeli::pHelis[1]->GetPosition();
            float distanceToPlayer = camDist.Magnitude();

            if (distanceToPlayer > 170.0f) {
                CWorld::Remove(CHeli::pHelis[1]);
                if (CHeli::pHelis[1]) {
                    delete std::exchange(CHeli::pHelis[1], nullptr);
                }
                CHeli::pHelis[1] = nullptr;
            }
        }
    }
    
    // Make helicopters fly away if too many
    if (IsHeliValid(CHeli::pHelis[0]) && CHeli::pHelis[0]->m_autoPilot.m_nCarMission != MISSION_HELI_FLY_AWAY_FROM_PLAYER) {
        if (maxAllowedHelis < 1) {
            CHeli::pHelis[0]->m_autoPilot.m_nCarMission = MISSION_HELI_FLY_AWAY_FROM_PLAYER;
            CHeli::pHelis[0]->m_MinHeightAboveTerrain = 100.0f;
            CHeli::pHelis[0]->m_LowestFlightHeight = 100.0f;
        } else {
            maxAllowedHelis--;
        }
    }
    
    // Process second helicopter flying away if too many
    if (IsHeliValid(CHeli::pHelis[1]) && CHeli::pHelis[1]->m_autoPilot.m_nCarMission != MISSION_HELI_FLY_AWAY_FROM_PLAYER && maxAllowedHelis <= 0) {
        CHeli::pHelis[1]->m_autoPilot.m_nCarMission = MISSION_HELI_FLY_AWAY_FROM_PLAYER;
        CHeli::pHelis[1]->m_MinHeightAboveTerrain = 100.0f;
        CHeli::pHelis[1]->m_LowestFlightHeight = 100.0f;
    }
}

// 0x6C7C50
void CHeli::RenderAllHeliSearchLights() {
    ZoneScoped;

    for (auto& light : HeliSearchLights) {
        SearchLightCone(
            light.m_nCoronaIndex,
            light.m_vecOrigin,
            light.m_vecTarget,
            light.m_fTargetRadius,
            light.m_fPower,
            light.field_24,
            light.m_bDrawShadow,
            light.m_vecUseless[0],
            light.m_vecUseless[1],
            light.m_vecUseless[2],
            false,
            0.05f,
            0.0f,
            0.0f,
            1.0f
        );
    }
}

// 0x6C6D30
void CHeli::BlowUpCar(CEntity* damager, bool bHideExplosion) {
    plugin::CallMethod<0x6C6D30, CHeli*, CEntity*, uint8>(this, damager, bHideExplosion);
}

// 0x6C4530
void CHeli::Fix() {
    m_damageManager.ResetDamageStatus();
    SetupDamageAfterLoad();
}

// 0x6C4330
bool CHeli::BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) {
    return false;
}

// 0x6C4320
bool CHeli::SetUpWheelColModel(CColModel* wheelCol) {
    return false;
}

// 0x6C4830
void CHeli::ProcessControlInputs(uint8 playerNum) {
    plugin::CallMethod<0x6C4830, CHeli*, uint8>(this, playerNum);
}

// 0x6C4400
void CHeli::Render() {
    auto* mi = GetVehicleModelInfo();
    m_nTimeTillWeNeedThisCar = CTimer::GetTimeInMS() + 3000;
    mi->SetVehicleColour(m_nPrimaryColor, m_nSecondaryColor, m_nTertiaryColor, m_nQuaternaryColor);

    auto staticRotor = m_aCarNodes[HELI_STATIC_ROTOR];
    RpAtomic* data = nullptr;
    if (staticRotor) {
        RwFrameForAllObjects(staticRotor, GetCurrentAtomicObjectCB, &data);
        if (data)
            CVehicle::SetComponentAtomicAlpha(data, 255);
    }

    auto staticRotor2 = m_aCarNodes[HELI_STATIC_ROTOR2];
    data = nullptr;
    if (staticRotor2) {
        RwFrameForAllObjects(staticRotor2, GetCurrentAtomicObjectCB, &data);
        if (data)
            CVehicle::SetComponentAtomicAlpha(data, 255);
    }

    auto movingRotor = m_aCarNodes[HELI_MOVING_ROTOR];
    data = nullptr;
    if (movingRotor) {
        RwFrameForAllObjects(movingRotor, GetCurrentAtomicObjectCB, &data);
        if (data)
            CVehicle::SetComponentAtomicAlpha(data, 0);
    }

    auto movingRotor2 = m_aCarNodes[HELI_MOVING_ROTOR2];
    data = nullptr;
    if (movingRotor2) {
        RwFrameForAllObjects(movingRotor2, GetCurrentAtomicObjectCB, &data);
        if (data)
            CVehicle::SetComponentAtomicAlpha(data, 0);
    }

    CEntity::Render(); // exactly CEntity
}

// 0x6C4550
void CHeli::SetupDamageAfterLoad() {
    vehicleFlags.bIsDamaged = false;
}

// 0x6C4E60
//Vanilla BUG: There is a small bug related in this case, when it comes to a Seasparrow or a LEVIATHN the blades should not stop when touching the water but there is a small reset.
void CHeli::ProcessFlyingCarStuff() {
    const auto vehicleType = AsAutomobile();

    // Please do not confuse with type of automobile!
    if (!GetStatus() || GetStatus() == STATUS_REMOTE_CONTROLLED || GetStatus() == STATUS_PHYSICS) {
        // Handle wheel angular velocity
        if (vehicleType->m_fHeliRotorSpeed < 0.22f && !physicalFlags.bSubmergedInWater) {
            vehicleType->m_fHeliRotorSpeed += (m_nModelIndex == MODEL_RCGOBLIN || m_nModelIndex == MODEL_RCRAIDER) ? 0.003f : 0.001f;
        }

        // Process flying controls if needed
        if (m_fHeliRotorSpeed > 0.15) {
            if (vehicleFlags.bIsRCVehicle ||
                (m_nWheelsOnGround < 4 && !(IsAmphibiousHeli() && physicalFlags.bTouchingWater) 
                || (m_fThrottleControl > 0.0f) 
                ||std::abs(m_vecMoveSpeed.x) > 0.02f 
                ||std::abs(m_vecMoveSpeed.y) > 0.02f 
                ||std::abs(m_vecMoveSpeed.z) > 0.02f))
            {
                FlyingControl(
                    vehicleFlags.bIsRCVehicle ? FLIGHT_MODEL_RC : FLIGHT_MODEL_HELI,
                    m_fYawControl,
                    m_fPitchControl,
                    m_fRollControl,
                    m_fThrottleControl
                );
            }
        }

        // Process blade collision if needed
        if (vehicleType->m_fHeliRotorSpeed > 0.015f) {
            auto bladeFrame = m_aCarNodes[PLANE_STATIC_PROP];
            if (bladeFrame) {
                auto bladeMatrix = CMatrix();
                bladeMatrix.Attach(RwFrameGetMatrix(bladeFrame), false);

                RpAtomic* atomic = nullptr;
                RwFrameForAllObjects(bladeFrame, GetCurrentAtomicObjectCB, &atomic);

                if (atomic) {
                    if ((atomic->interpolator.flags & rpINTERPOLATORDIRTYSPHERE) != 0) {
                        _rpAtomicResyncInterpolatedSphere(atomic);
                    }

                    float bladeRadius = atomic->boundingSphere.radius;
                    if (bladeRadius > 0.1f) {
                        float collisionFactor = 1.0f;
                        switch (m_nModelIndex) {
                        case MODEL_RCRAIDER:
                        case MODEL_RCGOBLIN:
                            collisionFactor = 0.9f;
                            break;
                        case MODEL_SPARROW:
                        case MODEL_SEASPAR:
                            collisionFactor = 0.8f;
                            break;
                        case MODEL_HUNTER:
                            collisionFactor = 0.5f;
                            break;
                        }

                        if (!GetStatus() || GetStatus() == STATUS_REMOTE_CONTROLLED) {
                            // Probably for optimization reasons only the top rotor was chosen.
                            CVehicle::DoBladeCollision(bladeMatrix.GetPosition(), GetMatrix(), ROTOR_TOP, bladeRadius, collisionFactor);
                        }
                    }
                }

                if (GetStatus() && GetStatus() != STATUS_PHYSICS || vehicleType->m_fHeliRotorSpeed <= 0.0075f) {
                    if (GetStatus() == STATUS_SIMPLE) {
                        CVector windPosition = GetPosition();
                        CWindModifiers::RegisterOne(windPosition, 1, 1.0f);
                    }
                } else {
                    CVector windPosition = GetPosition();
                    CWindModifiers::RegisterOne(windPosition, 1, std::min(vehicleType->m_fHeliRotorSpeed * 6.6666665f, 1.0f));
                }
            }
        }
    } else {
        if (!vehicleType->IsRealHeli()) {
            return;
        }

        vehicleType->vehicleFlags.bEngineOn = false;
        float speedReduction                   = CTimer::ms_fTimeStep * 0.00055f;
        if (speedReduction >= vehicleType->m_fHeliRotorSpeed) {
            vehicleType->m_fHeliRotorSpeed = 0.0f;
        } else {
            m_nFakePhysics = 0;
            vehicleType->m_fHeliRotorSpeed -= speedReduction;
        }
    }

    // Process audio events for specific models
    if (m_nModelIndex != MODEL_RCRAIDER && m_nModelIndex != MODEL_RCGOBLIN && vehicleType->m_fHeliRotorSpeed < 0.154f && vehicleType->m_fHeliRotorSpeed > 0.0044f) {
        RwFrame* bladeFrame = m_aCarNodes[PLANE_STATIC_PROP];
        if (bladeFrame) {
            CVector camDist = TheCamera.GetPosition() - GetPosition();
            float   distSq  = camDist.SquaredMagnitude();

            if (distSq < sq(20.0f) && std::abs(vehicleType->m_fUpDownLightAngle[0] - m_wheelRotation[1]) > DegreesToRadians(30.0f)) { // SQ(20) mts = 400 e. dis.
                CMatrix mat;
                mat.Attach(RwFrameGetMatrix(bladeFrame), false);
                CVector bladeDirection = mat.GetRight();
                m_matrix->InverseTransformVector(bladeDirection);

                float distance = 1.0f / std::max(std::sqrt(distSq), 0.01f);
                if (std::abs(DotProduct(camDist, bladeDirection)) > HELI_ROTOR_DOTPROD_LIMIT) {
                    vehicleType->m_vehicleAudio.AddAudioEvent(AE_HELI_BLADE, 0.0f);
                    vehicleType->m_fUpDownLightAngle[0] = m_wheelRotation[1];
                }
            }
        }
    }
}

// 0x6C5420
void CHeli::PreRender() {
    plugin::CallMethod<0x6C5420, CHeli*>(this);
}

// 0x6C7050
void CHeli::ProcessControl() {
    plugin::CallMethod<0x6C7050, CHeli*>(this);
}
