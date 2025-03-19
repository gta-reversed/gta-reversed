#include "StdInc.h"

#include "CarGenerator.h"

#include "TheCarGenerators.h"
#include "Occlusion.h"
#include "CarCtrl.h"
#include "TheScripts.h"

bool& CCarGenerator::m_bHotdogVendorPositionOffsetInitialized = *reinterpret_cast<bool*>(0xC2B974);
CVector& CCarGenerator::m_HotdogVendorPositionOffset = *reinterpret_cast<CVector*>(0xC2B968);

void CCarGenerator::InjectHooks()
{
    RH_ScopedClass(CCarGenerator);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(CheckForBlockage, 0x6F32E0);
    RH_ScopedInstall(CheckIfWithinRangeOfAnyPlayers, 0x6F2F40);
    RH_ScopedInstall(DoInternalProcessing, 0x6F34D0);
    RH_ScopedInstall(Process, 0x6F3E90);
    RH_ScopedInstall(Setup, 0x6F2E50);
    RH_ScopedInstall(SwitchOff, 0x6F2E30);
    RH_ScopedInstall(SwitchOn, 0x6F32C0);
    RH_ScopedInstall(CalcNextGen, 0x6F2E40);
}

// 0x6F32E0
bool CCarGenerator::CheckForBlockage(int32 modelId) {
    auto colModel = CModelInfo::GetModelInfo(modelId)->GetColModel();
    float radius = colModel ? colModel->GetBoundRadius() : 2.0f;

    int16 entityCount;
    CEntity* objects[8];

    CVector posn = UncompressLargeVector(m_vecPosn);
    CWorld::FindObjectsKindaColliding(posn, radius, true, &entityCount, 8, objects, false, true, true, false, false);

    for (auto& obj : std::span{ objects, (size_t)entityCount }) {
        auto entityColModel = obj->GetModelInfo()->GetColModel();
        if (colModel)
        {
            if (obj->GetPosition().z + entityColModel->GetBoundingBox().m_vecMax.z + 1.0f > posn.z + colModel->GetBoundingBox().m_vecMin.z &&
                obj->GetPosition().z + entityColModel->GetBoundingBox().m_vecMin.z - 1.0f < posn.z + colModel->GetBoundingBox().m_vecMax.z
            ) {
                bWaitUntilFarFromPlayer = true;
                return true;
            }
        }
        else
        {
            if (obj->GetPosition().z + entityColModel->GetBoundingBox().m_vecMax.z + 1.0f > posn.z - 1.0f &&
                obj->GetPosition().z + entityColModel->GetBoundingBox().m_vecMin.z - 1.0f < posn.z + 1.0f
            ) {
                bWaitUntilFarFromPlayer = true;
                return true;
            }
        }
    }

    return false;
}

// 0x6F2F40
bool CCarGenerator::CheckIfWithinRangeOfAnyPlayers()
{
    bool bVisible = false;
    CVector relPosn = FindPlayerCentreOfWorld(CWorld::PlayerInFocus) - UncompressLargeVector(m_vecPosn);
    if (fabs(relPosn.z) > 50.0f)
        return false;

    if (m_nModelId > 0 &&
        CModelInfo::GetModelInfo(m_nModelId)->AsVehicleModelInfoPtr()->IsBoat() &&
        relPosn.Magnitude2D() < TheCamera.m_fGenerationDistMultiplier * 240.0f)
    {
        CVector origin = UncompressLargeVector(m_vecPosn);
        if (TheCamera.IsSphereVisible(origin, 0.0f) && !COcclusion::IsPositionOccluded(origin, 0.0f))
        {
            bVisible = true;
        }
    }

    if (relPosn.Magnitude2D() >= TheCamera.m_fGenerationDistMultiplier * 160.0f && !bVisible)
    {
        bWaitUntilFarFromPlayer = false;
        return false;
    }

    if (CTheCarGenerators::GenerateEvenIfPlayerIsCloseCounter)
        return true;

    if (bWaitUntilFarFromPlayer)
        return false;

    float posnZ = UncompressLargeVector(m_vecPosn).z;
    if ((CGame::CanSeeOutSideFromCurrArea() && posnZ < 950.0f || !CGame::CanSeeOutSideFromCurrArea() && posnZ >= 950.0f)
        && (relPosn.Magnitude2D() >= TheCamera.m_fGenerationDistMultiplier * 160.0f - 20.0f || bHighPriority)
        && DotProduct2D(FindPlayerSpeed(-1), relPosn) <= 0.0f
    ) {
        return true;
    }

    return false;
}

// 0x6F34D0
void CCarGenerator::DoInternalProcessing()
{
    int32 actualModelId;
    CVehicle* vehicle;
    float baseZ;
    CColPoint colPoint{};
    CEntity* entity;
    tCarGenPlateText plate{};
    int32 tractorDriverPedType;

    bool nightTime = CClock::ClockHoursInRange(21, 7);
    if (!bIgnorePopulationLimit
        && (nightTime && CCarCtrl::NumParkedCars >= 10 || !nightTime && CCarCtrl::NumParkedCars >= 5)
    )
        return;

    if (m_nModelId >= 0)
    {
        if (CheckForBlockage(m_nModelId))
        {
            m_nNextGenTime += 4;
            return;
        }

        CStreaming::RequestModel(m_nModelId, STREAMING_KEEP_IN_MEMORY);
        actualModelId = m_nModelId;
        if (m_nModelId == MODEL_HOTDOG)
            CStreaming::RequestModel(MODEL_BMOCHIL, STREAMING_KEEP_IN_MEMORY);
    }
    else
    {
        if (m_nModelId == MODEL_INVALID || !CStreaming::ms_aInfoForModel[-m_nModelId].IsLoaded()) // todo: Figure out what they wanted with negative indexing
        {
            CTheZones::GetZoneInfo(FindPlayerCoors(-1), nullptr);
            actualModelId = CPopulation::m_AppropriateLoadedCars.PickRandomCar(true, true);

            if (actualModelId < 0)
                return;

            auto mi = CModelInfo::GetModelInfo(actualModelId)->AsVehicleModelInfoPtr();
            if (mi->IsBoat())
                return;

            if (mi->GetBoundingBox()->GetLength() > 8.0f)
                return;

            m_nModelId = -actualModelId;
            m_nPrimaryColor = -1;
            m_nSecondaryColor = -1;
        }
        else
        {
            actualModelId = -m_nModelId;
        }

        if (CheckForBlockage(actualModelId))
        {
            m_nNextGenTime += 4;
            return;
        }
    }

    if (!CStreaming::GetInfo(actualModelId).IsLoaded() ||
        actualModelId == MODEL_HOTDOG &&
        m_nModelId == MODEL_HOTDOG &&
        !CStreaming::GetInfo(MODEL_BMOCHIL).IsLoaded()
    )
        return;

    if (CModelInfo::IsBoatModel(actualModelId) ||
        actualModelId == MODEL_LEVIATHN ||
        actualModelId == MODEL_SEASPAR ||
        actualModelId == MODEL_SKIMMER)
    {
        switch (CModelInfo::GetModelInfo(actualModelId)->AsVehicleModelInfoPtr()->m_nVehicleType)
        {
        case VEHICLE_TYPE_HELI:
            vehicle = new CHeli(actualModelId, PARKED_VEHICLE);
            break;
        case VEHICLE_TYPE_PLANE:
            vehicle = new CPlane(actualModelId, PARKED_VEHICLE);
            break;
        case VEHICLE_TYPE_BOAT:
            vehicle = new CBoat(actualModelId, PARKED_VEHICLE);
            break;
        }

        CVector posn = UncompressLargeVector(m_vecPosn);
        baseZ = posn.z;
        if (baseZ <= MAP_Z_LOW_LIMIT)
            baseZ = CWorld::FindGroundZForCoord(posn.x, posn.y);
        vehicle->m_nExtendedRemovalRange = 255;
    }
    else
    {
        CVector posn = UncompressLargeVector(m_vecPosn);
        if (posn.z > MAP_Z_LOW_LIMIT)
            posn.z += 1.0f;
        else
            posn.z = 1000.0f;

        if (!CWorld::ProcessVerticalLine(posn, -1000.0f, colPoint, entity, true, false, false, false, false, false, nullptr))
            return;

        baseZ = colPoint.m_vecPoint.z;
        plate.m_nCarGenId = CTheCarGenerators::GetIndex(this);
        CTheCarGenerators::m_SpecialPlateHandler.Find(plate.m_nCarGenId, plate.m_szPlateText);
        if (plate.m_szPlateText[0])
        {
            auto mi = CModelInfo::GetModelInfo(actualModelId)->AsVehicleModelInfoPtr();
            if (mi->m_pPlateMaterial)
                mi->SetCustomCarPlateText(plate.m_szPlateText);
        }

        switch (CModelInfo::GetModelInfo(actualModelId)->AsVehicleModelInfoPtr()->m_nVehicleType)
        {
        case VEHICLE_TYPE_MTRUCK:
            vehicle = new CMonsterTruck(actualModelId, PARKED_VEHICLE);
            break;
        case VEHICLE_TYPE_QUAD:
            vehicle = new CQuadBike(actualModelId, PARKED_VEHICLE);
            break;
        case VEHICLE_TYPE_HELI:
            vehicle = new CHeli(actualModelId, PARKED_VEHICLE);
            break;
        case VEHICLE_TYPE_PLANE:
            vehicle = new CPlane(actualModelId, PARKED_VEHICLE);
            break;
        case VEHICLE_TYPE_TRAIN:
            vehicle = new CTrain(actualModelId, PARKED_VEHICLE);
            break;
        case VEHICLE_TYPE_BIKE:
            vehicle = new CBike(actualModelId, PARKED_VEHICLE);
            vehicle->AsBike()->bikeFlags.bOnSideStand = true;
            break;
        case VEHICLE_TYPE_BMX:
            vehicle = new CBmx(actualModelId, PARKED_VEHICLE);
            vehicle->AsBike()->bikeFlags.bOnSideStand = true;
            break;
        case VEHICLE_TYPE_TRAILER:
            vehicle = new CTrailer(actualModelId, PARKED_VEHICLE);
            break;
        default:
            vehicle = new CAutomobile(actualModelId, PARKED_VEHICLE, true);
            break;
        }

        vehicle->GetDistanceFromCentreOfMassToBaseOfModel();
        vehicle->vehicleFlags.bLightsOn = false;
        vehicle->m_nOverrideLights = NO_CAR_LIGHT_OVERRIDE;
        vehicle->SetCollisionLighting(colPoint.m_nLightingB);
    }

    vehicle->SetIsStatic(false);
    vehicle->vehicleFlags.bEngineOn = false;
    CVector posn = UncompressLargeVector(m_vecPosn);
    posn.z = vehicle->GetDistanceFromCentreOfMassToBaseOfModel() + baseZ;
    vehicle->SetPosn(posn);
    vehicle->SetOrientation(0.0f, 0.0f, m_nAngle * (TWO_PI / 256));
    vehicle->m_nStatus = eEntityStatus::STATUS_ABANDONED;
    vehicle->m_nDoorLock = eCarLock::CARLOCK_UNLOCKED;
    vehicle->vehicleFlags.bHasBeenOwnedByPlayer = bPlayerHasAlreadyOwnedCar;
    tractorDriverPedType = -1;

    if (!nightTime &&
        (vehicle->m_nModelIndex == MODEL_TRACTOR || vehicle->m_nModelIndex == MODEL_COMBINE))
    {
        // 0x6F3BF4

        CVector vehPosn = vehicle->GetPosition();
        CNodeAddress pathLink = ThePaths.FindNodeClosestToCoors(vehPosn, PATH_TYPE_VEH, 20.0F, 0, 0, 0, 0, 1);
        if (pathLink.IsValid())
        {
            assert(pathLink.IsValid());

            CPathNode& pathNode = ThePaths.m_pPathNodes[pathLink.m_wAreaId][pathLink.m_wNodeId];
            if (pathNode.m_nNumLinks != 0)
            {
                CNodeAddress baseLink = ThePaths.m_pNodeLinks[pathLink.m_wAreaId][pathNode.m_wBaseLinkId];
                if (ThePaths.m_pPathNodes[baseLink.m_wAreaId])
                {
                    vehicle->SetVehicleCreatedBy(RANDOM_VEHICLE);
                    vehicle->m_autoPilot.m_currentAddress = pathLink;
                    vehicle->m_autoPilot.m_nCruiseSpeed = 7;
                    vehicle->m_autoPilot.m_nCarMission = eCarMission::MISSION_CRUISE;
                    vehicle->m_autoPilot.m_startingRouteNode = baseLink;
                    vehicle->m_nStatus = eEntityStatus::STATUS_PHYSICS;
                    vehicle->vehicleFlags.bNeverUseSmallerRemovalRange = true;
                    bWaitUntilFarFromPlayer = true;
                    tractorDriverPedType = PEDTYPE_CIVMALE;
                }
            }
        }
    }

    CWorld::Add(vehicle);
    if (tractorDriverPedType != -1)
        CCarCtrl::SetUpDriverAndPassengersForVehicle(vehicle, tractorDriverPedType, 0, false, false, 99);

    if (actualModelId == MODEL_HOTDOG && m_nModelId == MODEL_HOTDOG && CStreaming::GetInfo(MODEL_BMOCHIL).IsLoaded())
    {
        CPed* ped = CPopulation::AddPed(PEDTYPE_CIVMALE, MODEL_BMOCHIL, vehicle->GetPosition() - vehicle->GetRight() * 3.0f, false);
        if (ped)
        {
            if (!m_bHotdogVendorPositionOffsetInitialized)
            {
                m_bHotdogVendorPositionOffsetInitialized = true;
                m_HotdogVendorPositionOffset.Set(0.0f, 0.0f, 0.6f);
            }
            ped->AttachPedToEntity(vehicle, m_HotdogVendorPositionOffset, 3, 0.0f, eWeaponType::WEAPON_UNARMED);
            CTheScripts::ScriptsForBrains.CheckIfNewEntityNeedsScript(ped, 0, nullptr);
        }
    }

    if (CGeneral::GetRandomNumberInRange(0, 100) < m_nAlarmChance)
        vehicle->m_nAlarmState = -1;

    if (CGeneral::GetRandomNumberInRange(0, 100) < m_nDoorLockChance)
        vehicle->m_nDoorLock = eCarLock::CARLOCK_LOCKED;

    if (m_nPrimaryColor != -1 && m_nSecondaryColor != -1)
    {
        vehicle->m_nPrimaryColor = m_nPrimaryColor;
        vehicle->m_nSecondaryColor = m_nSecondaryColor;
    }
    else if (m_nModelId < -1)
    {
        m_nPrimaryColor = vehicle->m_nPrimaryColor;
        m_nSecondaryColor = vehicle->m_nSecondaryColor;
    }
    CVisibilityPlugins::SetClumpAlpha(vehicle->m_pRwClump, 0);
    m_nVehicleHandle = GetVehiclePool()->GetRef(vehicle);

    // Originally, R* did a signed comparison between unsigned \r m_nGenerateCount and signed 32bit constant -1.
    // This made the generated code to always skip the decrementation.
    // However, this bug does not affect the game at all because all cargens created by the script
    // are either disabled or infinite.
#ifdef FIX_BUGS
    if (m_nGenerateCount < (uint16)-1)
#else
    if (m_nGenerateCount < -1)
#endif
        m_nGenerateCount--;

    m_nNextGenTime = CalcNextGen();
}

// 0x6F3E90
void CCarGenerator::Process()
{
    if (m_nVehicleHandle == -1 &&
        (
            CTheCarGenerators::GenerateEvenIfPlayerIsCloseCounter
            || m_nNextGenTime <= CTimer::GetTimeInMS()
        )
            && m_nGenerateCount != 0
            && CheckIfWithinRangeOfAnyPlayers()
    ) {
        DoInternalProcessing();
    }

    if (m_nVehicleHandle != -1)
    {
        auto vehicle = GetVehiclePool()->GetAtRef(m_nVehicleHandle);
        if (!vehicle)
            m_nVehicleHandle = -1;
        else if (vehicle->m_nStatus == eEntityStatus::STATUS_PLAYER)
        {
            m_nNextGenTime += 60000;
            m_nVehicleHandle = -1;
            bWaitUntilFarFromPlayer = true;
            vehicle->m_nExtendedRemovalRange = 0;
            if (m_nModelId < 0)
                m_nModelId = -1;
        }
    }
}

// 0x6F2E50
void CCarGenerator::Setup(CVector posn, float angle, int32 modelId, int16 color1, int16 color2, uint8 bForceSpawn,
                          uint8 alarmChance, uint8 doorLockChance, uint16 minDelay, uint16 maxDelay,
                          uint8 iplId, bool ignorePopulationLimit)
{
    constexpr float magic = 256.0f / 360.0f; // 0x8722E8 original expression 128.0f / 180.0f

    m_vecPosn = CompressLargeVector(posn);
    m_nAngle = (char)(angle * magic);
    m_nModelId = modelId;
    m_nPrimaryColor = (uint8)(color1);
    m_nSecondaryColor = (uint8)(color2);

    bWaitUntilFarFromPlayer = false;
    bIgnorePopulationLimit = ignorePopulationLimit;
    bHighPriority = bForceSpawn;
    bPlayerHasAlreadyOwnedCar = false;

    m_nAlarmChance = alarmChance;
    m_nDoorLockChance = doorLockChance;
    m_nMinDelay = minDelay;
    m_nMaxDelay = maxDelay;
    m_nIplId = iplId;

    m_nVehicleHandle = -1;
    m_nNextGenTime = CTimer::GetTimeInMS() + 1;
    m_nGenerateCount = 0;
    m_bIsUsed = true;
}

// 0x6F2E30
void CCarGenerator::SwitchOff()
{
    m_nGenerateCount = 0;
}

// 0x6F32C0
void CCarGenerator::SwitchOn()
{
    m_nGenerateCount = (uint16)-1;
    m_nNextGenTime = CalcNextGen();
}

// 0x6F2E40
uint32 CCarGenerator::CalcNextGen()
{
    return CTimer::GetTimeInMS() + 4;
}
