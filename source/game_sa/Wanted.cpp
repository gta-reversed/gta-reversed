/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Wanted.h"

void CWanted::InjectHooks() {
    RH_ScopedClass(CWanted);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x562390);
    RH_ScopedInstall(Reset, 0x562400);
    RH_ScopedInstall(InitialiseStaticVariables, 0x561C70);
    RH_ScopedInstall(UpdateWantedLevel, 0x561C90);
    RH_ScopedInstall(SetMaximumWantedLevel, 0x561E70);
    RH_ScopedInstall(AreMiamiViceRequired, 0x561F30);
    RH_ScopedInstall(AreSwatRequired, 0x561F40);
    RH_ScopedInstall(AreFbiRequired, 0x561F60);
    RH_ScopedInstall(AreArmyRequired, 0x561F80);
    RH_ScopedInstall(NumOfHelisRequired, 0x561FA0);
    RH_ScopedInstall(ResetPolicePursuit, 0x561FD0);
    RH_ScopedInstall(UpdateCrimesQ, 0x562760);
    RH_ScopedInstall(ClearQdCrimes, 0x561FE0);
    RH_ScopedInstall(AddCrimeToQ, 0x562000);
    RH_ScopedInstall(ReportCrimeNow, 0x562120);
    RH_ScopedInstall(IsInPursuit, 0x562330);
    RH_ScopedInstall(UpdateEachFrame, 0x562360);
    RH_ScopedInstall(RegisterCrime, 0x562410);
    RH_ScopedInstall(RegisterCrime_Immediately, 0x562430);
    RH_ScopedInstall(SetWantedLevel, 0x562470);
    RH_ScopedInstall(CheatWantedLevel, 0x562540);
    RH_ScopedInstall(SetWantedLevelNoDrop, 0x562570);
    RH_ScopedInstall(ClearWantedLevelAndGoOnParole, 0x5625A0);
    RH_ScopedInstall(WorkOutPolicePresence, 0x5625F0);
    RH_ScopedInstall(IsClosestCop, 0x5627D0);
    RH_ScopedInstall(ComputePursuitCopToDisplace, 0x562B00);
    RH_ScopedOverloadedInstall(RemovePursuitCop, "func", 0x562300, void (*)(CCopPed*, CCopPed**, uint8&));
    RH_ScopedOverloadedInstall(RemovePursuitCop, "method", 0x562C10, void(CWanted::*)(CCopPed*));
    RH_ScopedInstall(RemoveExcessPursuitCops, 0x562C40);
    RH_ScopedInstall(Update, 0x562C90);
    RH_ScopedOverloadedInstall(CanCopJoinPursuit, "func", 0x562F60, bool (*)(CCopPed*, uint8, CCopPed**, uint8&));
    RH_ScopedOverloadedInstall(CanCopJoinPursuit, "method", 0x562FB0, bool(CWanted::*)(CCopPed*));
    RH_ScopedInstall(SetPursuitCop, 0x563060);
}

// 0x562390
void CWanted::Initialise() {
    m_bPoliceBackOff = false;
    m_bPoliceBackOffGarage = false;
    m_bEverybodyBackOff = false;
    SetSwatRequired(false);
    SetFbiRequired(false);
    SetArmyRequired(false);

    m_ChaosLevel = 0;
    m_ChaosLevelBeforeParole = 0;
    m_LastTimeWantedDecreased = 0;
    m_LastTimeWantedLevelChanged = 0;
    m_TimeOfParole = 0;
    m_NumCopsInPursuit = 0;
    m_MaxCopsInPursuit = 0;
    m_MaxCopCarsInPursuit = 0;
    m_ChanceOnRoadBlock = 0;
    m_Multiplier = 1.0f;
    m_TimeCounting = false;
    m_StoredPoliceBackOff = false;
    m_WantedLevel = eWantedLevel::WANTED_CLEAN;
    m_WantedLevelBeforeParole = eWantedLevel::WANTED_CLEAN;
    m_CopsBeatingSuspect = 0;

    rng::fill(m_CopsInPursuit, nullptr);
    ClearQdCrimes();
}

// 0x562400
void CWanted::Reset() {
    ResetPolicePursuit();
    Initialise();
}

// Initialize Static Variables
// 0x561C70
void CWanted::InitialiseStaticVariables() {
    MaximumWantedLevel = eWantedLevel::WANTED_LEVEL_6;
    MaximumChaosLevel = 9200;
    UseNewsHeliInAdditionToPolice = false;
}

// 0x561C90
void CWanted::UpdateWantedLevel() {
    m_ChaosLevel         = std::min(m_ChaosLevel, MaximumChaosLevel);
    const auto oldWanted = m_WantedLevel;

    if (m_ChaosLevel >= 4600) {
        CStats::IncrementStat(eStats::STAT_TOTAL_NUMBER_OF_WANTED_STARS_ATTAINED, (float)(+eWantedLevel::WANTED_LEVEL_6 - +oldWanted));
        m_WantedLevel         = eWantedLevel::WANTED_LEVEL_6;
        m_ChanceOnRoadBlock   = 30;
        m_MaxCopsInPursuit    = 10;
        m_MaxCopCarsInPursuit = 3;
    } else if (m_ChaosLevel >= 2400) {
        CStats::IncrementStat(eStats::STAT_TOTAL_NUMBER_OF_WANTED_STARS_ATTAINED, (float)(+eWantedLevel::WANTED_LEVEL_5 - +oldWanted));
        m_WantedLevel         = eWantedLevel::WANTED_LEVEL_5;
        m_ChanceOnRoadBlock   = 24;
        m_MaxCopsInPursuit    = 8;
        m_MaxCopCarsInPursuit = 3;
    } else if (m_ChaosLevel >= 1200) {
        CStats::IncrementStat(eStats::STAT_TOTAL_NUMBER_OF_WANTED_STARS_ATTAINED, (float)(+eWantedLevel::WANTED_LEVEL_4 - +oldWanted));
        m_WantedLevel         = eWantedLevel::WANTED_LEVEL_4;
        m_ChanceOnRoadBlock   = 18;
        m_MaxCopsInPursuit    = 6;
        m_MaxCopCarsInPursuit = 2;
    } else if (m_ChaosLevel >= 550) {
        CStats::IncrementStat(eStats::STAT_TOTAL_NUMBER_OF_WANTED_STARS_ATTAINED, (float)(+eWantedLevel::WANTED_LEVEL_3 - +oldWanted));
        m_WantedLevel         = eWantedLevel::WANTED_LEVEL_3;
        m_ChanceOnRoadBlock   = 12;
        m_MaxCopsInPursuit    = 4;
        m_MaxCopCarsInPursuit = 2;
    } else if (m_ChaosLevel >= 180) {
        CStats::IncrementStat(eStats::STAT_TOTAL_NUMBER_OF_WANTED_STARS_ATTAINED, (float)(+eWantedLevel::WANTED_LEVEL_2 - +oldWanted));
        m_WantedLevel         = eWantedLevel::WANTED_LEVEL_2;
        m_ChanceOnRoadBlock   = 0;
        m_MaxCopsInPursuit    = 3;
        m_MaxCopCarsInPursuit = 2;
    } else if (m_ChaosLevel >= 50) {
        CStats::IncrementStat(eStats::STAT_TOTAL_NUMBER_OF_WANTED_STARS_ATTAINED, (float)(+eWantedLevel::WANTED_LEVEL_1 - +oldWanted));
        m_WantedLevel         = eWantedLevel::WANTED_LEVEL_1;
        m_ChanceOnRoadBlock   = 0;
        m_MaxCopsInPursuit    = 1;
        m_MaxCopCarsInPursuit = 1;
    } else {
        if (m_WantedLevel == eWantedLevel::WANTED_LEVEL_1) {
            CStats::IncrementStat(STAT_TOTAL_NUMBER_OF_WANTED_STARS_EVADED);
        }
        m_WantedLevel         = eWantedLevel::WANTED_CLEAN;
        m_ChanceOnRoadBlock   = 0;
        m_MaxCopsInPursuit    = 0;
        m_MaxCopCarsInPursuit = 0;
    }

    if (oldWanted != m_WantedLevel) {
        m_LastTimeWantedLevelChanged = CTimer::GetTimeInMS();
    }

    if (PoliceBackOff()) {
        m_MaxCopsInPursuit    = 0;
        m_MaxCopCarsInPursuit = 0;
        m_ChanceOnRoadBlock   = 0;
    }
}

// Set Maximum Wanted Level
// 0x561E70
void CWanted::SetMaximumWantedLevel(eWantedLevel level) {
    assert(level <= eWantedLevel::WANTED_LEVEL_6);

    MaximumWantedLevel = level;
    switch (level) {
    case eWantedLevel::WANTED_CLEAN:   MaximumChaosLevel = 0; break;
    case eWantedLevel::WANTED_LEVEL_1: MaximumChaosLevel = 115; break;
    case eWantedLevel::WANTED_LEVEL_2: MaximumChaosLevel = 365; break;
    case eWantedLevel::WANTED_LEVEL_3: MaximumChaosLevel = 875; break;
    case eWantedLevel::WANTED_LEVEL_4: MaximumChaosLevel = 1800; break;
    case eWantedLevel::WANTED_LEVEL_5: MaximumChaosLevel = 3500; break;
    case eWantedLevel::WANTED_LEVEL_6: MaximumChaosLevel = 6900; break;
    default:                           NOTSA_UNREACHABLE(); break;
    }
}

// 0x561F30, Leftover from VC
bool CWanted::AreMiamiViceRequired() const {
    return m_WantedLevel > eWantedLevel::WANTED_LEVEL_2;
}

// Checks if SWAT is needed after four wanted level stars
// 0x561F40
bool CWanted::AreSwatRequired() const {
    return m_WantedLevel == eWantedLevel::WANTED_LEVEL_4 || m_bSwatRequired;
}

// Checks if FBI is needed after five wanted level stars
// 0x561F60
bool CWanted::AreFbiRequired() const {
    return m_WantedLevel == eWantedLevel::WANTED_LEVEL_5 || m_bFbiRequired;
}

// Checks if Army is needed after six wanted level stars
// 0x561F80
bool CWanted::AreArmyRequired() const {
    return m_WantedLevel == eWantedLevel::WANTED_LEVEL_6 || m_bArmyRequired;
}

// 0x561FA0
int32 CWanted::NumOfHelisRequired() const {
    if (PoliceBackOff()) {
        return 0;
    }

    switch (m_WantedLevel) {
    case eWantedLevel::WANTED_CLEAN:
    case eWantedLevel::WANTED_LEVEL_1:
    case eWantedLevel::WANTED_LEVEL_2: return 0;
    case eWantedLevel::WANTED_LEVEL_3: return 1;
    case eWantedLevel::WANTED_LEVEL_4:
    case eWantedLevel::WANTED_LEVEL_5:
    case eWantedLevel::WANTED_LEVEL_6: return 2;
    default:      NOTSA_UNREACHABLE(); return 0;
    }
}

// In III
// 0x561FD0
void CWanted::ResetPolicePursuit() {
    // NOP
}

// 0x562760
void CWanted::UpdateCrimesQ() {
    for (auto& crime : m_CrimesBeingQd) {
        if (crime.m_nCrimeType == CRIME_NONE) {
            continue;
        }

        if (CTimer::GetTimeInMS() - crime.m_nStartTime > 500 && !crime.m_bAlreadyReported) {
            ReportCrimeNow(crime.m_nCrimeType, crime.m_vecCoors, crime.m_bPoliceDontReallyCare);

            crime.m_bAlreadyReported = true;
        }

        if (CTimer::GetTimeInMS() > crime.m_nStartTime + 10'000) {
            crime.m_nCrimeType = CRIME_NONE;
        }
    }
}

// 0x561FE0
void CWanted::ClearQdCrimes() {
    for (auto& crime : m_CrimesBeingQd) {
        crime.m_nCrimeType = eCrimeType::CRIME_NONE;
    }
}

// 0x562000
bool CWanted::AddCrimeToQ(eCrimeType crimeType, uint32 crimeId, const CVector& posn, bool alreadyReported, bool policeDontReallyCare) {
    assert(crimeType != eCrimeType::CRIME_NONE); // IV

    const auto ReportCrimeIfPossible = [alreadyReported](CCrimeBeingQd& c) {
        const auto old       = c.m_bAlreadyReported;

        NOTSA_LOG_DEBUG("New crime! Type: {}, made by: {}, reported already?: {}", c.m_nCrimeType, c.m_nCrimeId, old ? "yes" : "no");

        c.m_bAlreadyReported = notsa::coalesce(c.m_bAlreadyReported, alreadyReported);
        return old;
    };

    if (auto c = rng::find_if(m_CrimesBeingQd, [&](const auto& c) { return c.m_nCrimeType == crimeType && c.m_nCrimeId == crimeId; }); c != std::end(m_CrimesBeingQd)) {
        // If already queued, early return
        return ReportCrimeIfPossible(*c);
    }

    auto free = rng::find_if(m_CrimesBeingQd, [](const auto& c) { return c.m_nCrimeType == eCrimeType::CRIME_NONE; });
    if (free == std::end(m_CrimesBeingQd)) {
        return false;
    }

    free->m_nCrimeType            = crimeType;
    free->m_nCrimeId              = crimeId;
    free->m_nStartTime            = CTimer::GetTimeInMS();
    free->m_vecCoors              = posn;
    free->m_bAlreadyReported      = alreadyReported;
    free->m_bPoliceDontReallyCare = policeDontReallyCare;
    return ReportCrimeIfPossible(*free);
}

// 0x562120
void CWanted::ReportCrimeNow(eCrimeType crimeType, const CVector& posn, bool bPoliceDontReallyCare) {
    if (CCheat::IsActive(CHEAT_I_DO_AS_I_PLEASE)) {
        return;
    }

    auto wantedLevel = m_WantedLevel;
    auto mul = m_Multiplier;

    if (CDarkel::ReadStatus() == eDarkelStatus::FRENZY_ON_GOING) {
        mul *= 0.3f;
    }

    mul = std::max(mul, 0.0f);

    if (CGangWars::GangWarFightingGoingOn()) {
        mul = 0.0f;
    }

    if (bPoliceDontReallyCare) {
        mul /= 3.0f;
    }

    if (CGangWars::GangWarFightingGoingOn()) {
        mul = 0.0f;
    }

    switch (crimeType) {
    case CRIME_DAMAGED_PED:
    case CRIME_VEHICLE_DAMAGE:
    case CRIME_SPEEDING:
        mul *= 5.0f;
        break;
    case CRIME_DAMAGED_COP:
        mul *= 45.0f;
        break;
    case CRIME_DAMAGE_CAR:
        mul *= 30.0f;
        break;
    case CRIME_DAMAGE_COP_CAR:
    case CRIME_KILL_COP_PED_WITH_CAR:
    case CRIME_SET_COP_PED_ON_FIRE:
        mul *= 80.0f;
        break;
    case CRIME_CAR_STEAL:
        mul *= 15.0f;
        break;
    case CRIME_RUN_REDLIGHT:
        mul *= 10.0f;
        break;
    case CRIME_KILL_PED_WITH_CAR:
        mul *= 18.0f;
        break;
    case CRIME_DESTROY_HELI:
    case CRIME_DESTROY_PLANE:
        mul *= 400.0f;
        break;
    case CRIME_SET_PED_ON_FIRE:
    case CRIME_SET_CAR_ON_FIRE:
        mul *= 20.0f;
        break;
    case CRIME_EXPLOSION:
        mul *= 25.0f;
        break;
    case CRIME_STAB_PED:
        mul *= 35.0f;
        break;
    case CRIME_STAB_COP:
        mul *= 100.0f;
        break;
    case CRIME_DESTROY_VEHICLE:
        mul *= 70.0f;
        break;
    case CRIME_HIT_CAR:
    case CRIME_AIM_GUN:
        mul *= 2.0f;
        break;
    default:
        break;
    }

    NOTSA_LOG_DEBUG("{} : {} (Mult:{})", crimeType, m_WantedLevel, mul); // IV

    if (crimeType != CRIME_NONE && crimeType != CRIME_FIRE_WEAPON) {
        m_ChaosLevel += static_cast<uint32>(mul);
    }

    m_ChaosLevel = std::max(m_ChaosLevel, m_ChaosLevelBeforeParole);
    UpdateWantedLevel();
    if (m_WantedLevel > wantedLevel) {
        m_PoliceScannerAudioEntity.AddAudioEvent(AE_CRIME_COMMITTED, crimeType, posn);
    }
}

// 0x562330
bool CWanted::IsInPursuit(CCopPed* cop) {
    for (auto& copInPursuit : m_CopsInPursuit) {
        if (copInPursuit == cop) {
            return true;
        }
    }

    return false;
}

// 0x562360
void CWanted::UpdateEachFrame() {
    ZoneScoped;

    auto playerWanted = FindPlayerWanted();
    auto wantedLevel = playerWanted->GetWantedLevel();

    if (playerWanted->PoliceBackOff() || (wantedLevel != eWantedLevel::WANTED_LEVEL_3) && (wantedLevel <= eWantedLevel::WANTED_LEVEL_3 || wantedLevel > eWantedLevel::WANTED_LEVEL_6)) { // wantedLevel condition looks inlined or common, see NumOfHelisRequired
        UseNewsHeliInAdditionToPolice = true;
    }
}

// 0x562410
void CWanted::RegisterCrime(eCrimeType crimeType, const CVector& posn, uint32 IDKey, bool bPoliceDontReallyCare) {
    AddCrimeToQ(crimeType, IDKey, posn, false, bPoliceDontReallyCare);
}

// 0x562430
void CWanted::RegisterCrime_Immediately(eCrimeType crimeType, const CVector& posn, uint32 IDKey, bool bPoliceDontReallyCare) {
    if (!AddCrimeToQ(crimeType, IDKey, posn, true, bPoliceDontReallyCare)) {
        ReportCrimeNow(crimeType, posn, bPoliceDontReallyCare);
    }
}

// 0x562470
void CWanted::SetWantedLevel(eWantedLevel level) {
    NOTSA_LOG_DEBUG("New:{}", level); // IV

    if (CCheat::IsActive(CHEAT_I_DO_AS_I_PLEASE)) {
        return;
    }

    assert(level <= eWantedLevel::WANTED_LEVEL_6);

    eWantedLevel newLevel = std::min(level, MaximumWantedLevel);
    ClearQdCrimes();

    switch (newLevel) {
    case eWantedLevel::WANTED_CLEAN:   m_ChaosLevel = 0; break;
    case eWantedLevel::WANTED_LEVEL_1: m_ChaosLevel = 70; break;
    case eWantedLevel::WANTED_LEVEL_2: m_ChaosLevel = 200; break;
    case eWantedLevel::WANTED_LEVEL_3: m_ChaosLevel = 570; break;
    case eWantedLevel::WANTED_LEVEL_4: m_ChaosLevel = 1220; break;
    case eWantedLevel::WANTED_LEVEL_5: m_ChaosLevel = 2420; break;
    case eWantedLevel::WANTED_LEVEL_6: m_ChaosLevel = 4620; break;
    default:                           NOTSA_UNREACHABLE(); break;
    }
    UpdateWantedLevel();
}

// 0x562540
void CWanted::CheatWantedLevel(eWantedLevel level) {
    assert(level <= eWantedLevel::WANTED_LEVEL_6);
    if (level > MaximumWantedLevel) {
        SetMaximumWantedLevel(level);
    }

    SetWantedLevel(level);
    UpdateWantedLevel();
}

// 0x562570
void CWanted::SetWantedLevelNoDrop(eWantedLevel level) {
    NOTSA_LOG_DEBUG("Wanted: SetWantedLevelNoDrop: {}", level); // IV
    assert(level <= eWantedLevel::WANTED_LEVEL_6);
    if (m_WantedLevel < m_WantedLevelBeforeParole) {
        SetWantedLevel(m_WantedLevelBeforeParole);
    }

    if (level > m_WantedLevel) {
        SetWantedLevel(level);
    }
}

// 0x5625A0
void CWanted::ClearWantedLevelAndGoOnParole() {
    CStats::IncrementStat(STAT_TOTAL_NUMBER_OF_WANTED_STARS_EVADED, static_cast<float>(m_WantedLevel));

    auto playerWanted = FindPlayerWanted();
    m_ChaosLevelBeforeParole = playerWanted->m_ChaosLevel;
    m_WantedLevelBeforeParole = playerWanted->m_WantedLevel;
    m_TimeOfParole = CTimer::GetTimeInMS();
    m_ChaosLevel = 0;
    m_WantedLevel = eWantedLevel::WANTED_CLEAN;
    ResetPolicePursuit();
}

// 0x5625F0
int32 CWanted::WorkOutPolicePresence(CVector posn, float radius) {
    auto numCops = 0;

    auto pedPool = GetPedPool();
    for (auto i = pedPool->GetSize(); i; i--) {
        if (auto ped = pedPool->GetAt(i - 1)) {
            if (ped->m_nPedType != PED_TYPE_COP || !ped->IsAlive())
                continue;

            if (DistanceBetweenPoints(ped->GetPosition(), posn) < radius)
                numCops++;
        }
    }

    auto vehPool = GetVehiclePool();
    for (auto i = vehPool->GetSize(); i; i--) {
        if (auto veh = vehPool->GetAt(i - 1)) {
            bool isCopVehicle = veh->vehicleFlags.bIsLawEnforcer || veh->m_nModelIndex == MODEL_POLMAV;

            if (!isCopVehicle || veh == FindPlayerVehicle()) {
                continue;
            }

            if (veh->GetStatus() == STATUS_ABANDONED || veh->GetStatus() == STATUS_WRECKED) {
                continue;
            }

            if (DistanceBetweenPoints(veh->GetPosition(), posn) < radius) {
                numCops++;
            }
        }
    }

    return numCops;
}

// 0x5627D0
// Returns true if the specified ped is one of the closest to the player.
bool CWanted::IsClosestCop(CPed* ped, const int32 numCopsToCheck) const {
    // NOTSA: A bit different but should have the same behavior.
    std::pair<CCopPed*, float /* distance */> closestCops[MAX_COPS_IN_PURSUIT];

    for (const auto&& [i, cop] : rngv::enumerate(m_CopsInPursuit)) {
        closestCops[i].first = cop;
        closestCops[i].second = cop ? DistanceBetweenPointsSquared(FindPlayerCoors(), cop->GetPosition()) : FLT_MAX;
    }
    rng::sort(closestCops, [](const auto& a, const auto& b) { return a.second < b.second; });

    for (const auto& [cop, _] : closestCops | rngv::take(numCopsToCheck)) {
        if (cop == ped->AsCop()) {
            return true;
        }
    }
    return false;
}

// 0x562B00
CCopPed* CWanted::ComputePursuitCopToDisplace(CCopPed* cop, CCopPed** copsArray) {
    const auto& playerPos = FindPlayerPed()->GetPosition();
    CCopPed* displacedCop = nullptr;
    auto distTargetCop = 1.0f;

    if (cop) {
        distTargetCop = std::max(DistanceBetweenPointsSquared(playerPos, cop->GetPosition()), 1.0f);
    }

    for (auto i = 0u; i < MAX_COPS_IN_PURSUIT; i++) {
        const auto& copInPursuit = copsArray[i];

        if (!copInPursuit) {
            continue;
        }

        if (!copInPursuit->IsAlive()) {
            return copInPursuit;
        }

        const auto distPursuitCop = DistanceBetweenPointsSquared(playerPos, copInPursuit->GetPosition());

        if (distPursuitCop > distTargetCop) {
            displacedCop  = copInPursuit;
            distTargetCop = distPursuitCop;
        }
    }

    return displacedCop;
}

// 0x562300
void CWanted::RemovePursuitCop(CCopPed* cop, CCopPed** copsArray, uint8& copsCounter) {
    for (auto i = 0u; i < MAX_COPS_IN_PURSUIT; i++) {
        if (copsArray[i] != cop) {
            continue;
        }

        // R*: delete?
        copsArray[i] = nullptr;
        copsCounter--;
        break;
    }
}

// 0x562C10
void CWanted::RemovePursuitCop(CCopPed* cop) {
    for (auto& copInPursuit : m_CopsInPursuit) {
        if (copInPursuit != cop) {
            continue;
        }

        copInPursuit = nullptr;
        m_NumCopsInPursuit--;
        break;
    }
}

// 0x562C40
void CWanted::RemoveExcessPursuitCops() {
    while (m_NumCopsInPursuit > m_MaxCopsInPursuit) {
        RemovePursuitCop(ComputePursuitCopToDisplace(nullptr, m_CopsInPursuit));
    }
}

// 0x562C90
void CWanted::Update() {
    if (m_WantedLevel < eWantedLevel::WANTED_LEVEL_5) {
        if (m_TimeCounting) {
            auto newStatValue = m_CurrentChaseTime;
            CStats::SetNewRecordStat(STAT_LONGEST_CHASE_TIME_WITH_5_OR_MORE_STARS, static_cast<float>(newStatValue));
            CStats::SetStatValue(STAT_LONGEST_CHASE_TIME_WITH_5_OR_MORE_STARS, static_cast<float>(newStatValue));
            m_TimeCounting = false;
        }
    } else {
        if (!m_TimeCounting) {
            m_CurrentChaseTime = 0;
            m_CurrentChaseTimeCounter = CTimer::GetTimeInMS();
            m_TimeCounting = true;
        }
        if (m_TimeCounting && CTimer::GetTimeInMS() - m_CurrentChaseTimeCounter > 1000) {
            m_CurrentChaseTime++;
            m_CurrentChaseTimeCounter = CTimer::GetTimeInMS();
        }
    }

    if (CTimer::GetTimeInMS() - m_TimeOfParole > 20000) {
        m_ChaosLevelBeforeParole = 0;
        m_WantedLevelBeforeParole = eWantedLevel::WANTED_CLEAN;
    }

    if (CTimer::GetTimeInMS() - m_LastTimeWantedDecreased > 1000) {
        bool inElusiveZone = CWeather::WeatherRegion == WEATHER_REGION_DEFAULT
                          || CWeather::WeatherRegion == WEATHER_REGION_DESERT
                          || !CGame::CanSeeOutSideFromCurrArea();

        auto vehicle = FindPlayerVehicle();
        bool hasElusiveVehicle = vehicle && (vehicle->IsLawEnforcementVehicle() || vehicle->IsSubHeli() || vehicle->IsSubPlane());

        if (m_WantedLevel > eWantedLevel::WANTED_LEVEL_1 && inElusiveZone && hasElusiveVehicle) {
            m_LastTimeWantedDecreased = CTimer::GetTimeInMS();
        }
        else {
            auto playerCoors = FindPlayerCoors();

            if (!WorkOutPolicePresence(playerCoors, 18.0f)) {
                int32 chaosLevel = m_ChaosLevel;
                m_LastTimeWantedDecreased = CTimer::GetTimeInMS();

                chaosLevel -= (inElusiveZone) ? 2 : 1;
                m_ChaosLevel = std::max(chaosLevel, 0);

                UpdateWantedLevel();
                CGameLogic::SetPlayerWantedLevelForForbiddenTerritories(true);
            }
        }

        UpdateCrimesQ();

        int cops = 0;
        bool nilEncountered = false;
        bool listMessedUp = false;

        for (auto& copInPursuit : m_CopsInPursuit) {
            if (copInPursuit != nullptr) {
                ++cops;

                if (nilEncountered) {
                    listMessedUp = true;
                }
            }
            else {
                nilEncountered = true;
            }
        }

        if (cops != m_NumCopsInPursuit) {
            NOTSA_LOG_DEBUG("CopPursuit total messed up: re-setting!"); // leftover debug shit, R* log
            m_NumCopsInPursuit = cops;
        }
        if (listMessedUp) {
            NOTSA_LOG_DEBUG("CopPursuit pointer list messed up: re-sorting!"); // R* log
            bool notFixed = true;

            for (auto i = 0u; i < MAX_COPS_IN_PURSUIT; i++) {
                auto& cop = m_CopsInPursuit[i];

                if (!cop) {
                    for (auto j = i; j < MAX_COPS_IN_PURSUIT; j++) {
                        if (m_CopsInPursuit[j]) {
                            cop = m_CopsInPursuit[j];
                            m_CopsInPursuit[j] = nullptr;
                            notFixed = false;

                            break;
                        }
                    }

                    if (notFixed) {
                        break;
                    }
                }
            }
        }
    }

    if (m_StoredPoliceBackOff != PoliceBackOff()) {
        UpdateWantedLevel();
        m_StoredPoliceBackOff = PoliceBackOff();
    }
}

// 0x562F60
bool CWanted::CanCopJoinPursuit(CCopPed* target, uint8 maxCopsCount, CCopPed** copsArray, uint8& copsCounter) {
    if (!maxCopsCount) {
        return false;
    }

    while (true) {
        if (copsCounter < maxCopsCount) {
            return true;
        }

        auto cop = ComputePursuitCopToDisplace(target, copsArray);
        if (!cop) {
            break;
        }

        RemovePursuitCop(cop, copsArray, copsCounter);
    }
    return false;
}

// 0x562FB0
bool CWanted::CanCopJoinPursuit(CCopPed* cop) {
    if (PoliceBackOff()) {
        return false;
    }
    std::array<CCopPed*, MAX_COPS_IN_PURSUIT> cops{};
    rng::copy(m_CopsInPursuit, cops.begin());
    auto copsCount = m_NumCopsInPursuit;
    return CanCopJoinPursuit(cop, m_MaxCopsInPursuit, cops.data(), copsCount);
}

// 0x563060
bool CWanted::SetPursuitCop(CCopPed* cop) {
    if (!CanCopJoinPursuit(cop)) {
        return false;
    }

    while (m_NumCopsInPursuit >= m_MaxCopsInPursuit) {
        RemovePursuitCop(ComputePursuitCopToDisplace(cop, m_CopsInPursuit));
    }

    for (auto& copInPursuit : m_CopsInPursuit) {
        if (copInPursuit != nullptr) {
            continue;
        }

        copInPursuit = cop;
        m_NumCopsInPursuit++;
        break;
    }

    return true;
}
