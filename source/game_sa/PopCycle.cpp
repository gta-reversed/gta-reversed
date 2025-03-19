/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "PopCycle.h"
#include <CustomBuildingDNPipeline.h>

void CPopCycle::InjectHooks() {
    RH_ScopedClass(CPopCycle);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(Initialise, 0x5BC090);
    RH_ScopedGlobalInstall(PickGangToCreateMembersOf, 0x60F8D0);
    RH_ScopedGlobalInstall(FindNewPedType, 0x60FBD0);
    RH_ScopedGlobalInstall(PickPedMIToStreamInForCurrentZone, 0x60FFD0, { .reversed = false });
    RH_ScopedGlobalInstall(IsPedAppropriateForCurrentZone, 0x610150);
    RH_ScopedGlobalInstall(IsPedInGroup, 0x610210);
    RH_ScopedGlobalInstall(PickARandomGroupOfOtherPeds, 0x610420);
    RH_ScopedGlobalInstall(PlayerKilledADealer, 0x610490);
    RH_ScopedGlobalInstall(UpdateDealerStrengths, 0x6104B0);
    RH_ScopedGlobalInstall(UpdateAreaDodgyness, 0x610560);
    RH_ScopedGlobalInstall(UpdateIsGangArea, 0x6106D0);
    RH_ScopedGlobalInstall(PedIsAcceptableInCurrentZone, 0x610720);
    RH_ScopedGlobalInstall(UpdatePercentages, 0x610770);
    RH_ScopedGlobalInstall(Update, 0x610BF0);
    RH_ScopedGlobalInstall(GetCurrentPercOther_Peds, 0x610310);
}

// 0x5BC090
void CPopCycle::Initialise() {
    CFileMgr::SetDir("DATA");
    const auto file = CFileMgr::OpenFile("POPCYCLE.DAT", "r");
    CFileMgr::SetDir("");

    const notsa::ScopeGuard autoCloser{ [&] { CFileMgr::CloseFile(file); } };

    auto nline{ 1u };
    for (auto zone = 0; zone < (uint32)ZoneType::COUNT; zone++) {
        for (auto wktime = 0; wktime < 2; wktime++) { // weekday (0) / weekend(1)
            for (auto daytime = 0; daytime < 24 / PERC_DATA_TIME_RESOLUTION_HR; daytime++) {
                // Find next suitable data line
                const char* l{};
                while (true) {
                    l = CFileLoader::LoadLine(file);
                    if (!l) {
                        NOTSA_UNREACHABLE("Expected more data, got EOF!");
                    }
                    nline++;
                    if (l[0] != '/' && l[0]) {
                        break;
                    }
                }

                // Ideally we could/would use a file pointer here (instead of `LoadLine`)
                // and read each number one-by-one.
                // But until then we're stuck with this hardcoded version.

                auto& percs = m_nPercTypeGroup[daytime][wktime][zone];
                const auto nread = sscanf_s(
                    l,
                    "%hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu",

                    &m_nMaxNumPeds[daytime][wktime][zone],
                    &m_nMaxNumCars[daytime][wktime][zone],

                    &m_nPercDealers[daytime][wktime][zone],
                    &m_nPercGang[daytime][wktime][zone],
                    &m_nPercCops[daytime][wktime][zone],
                    &m_nPercOther[daytime][wktime][zone],

                    &percs[0], &percs[1], &percs[2], &percs[3], &percs[4], &percs[5],
                    &percs[6], &percs[7], &percs[8], &percs[9], &percs[10], &percs[11],
                    &percs[12], &percs[13], &percs[14], &percs[15], &percs[16], &percs[17]
                );

                if (nread != 6 + 18) {
                    NOTSA_UNREACHABLE("Failed reading all data!");
                }

                // In the vanilla game the %'s in this array add up to 100% (The original code rescales the values in order to make sure this is the case...but fails sometimes, see below)
                // But we take another route and don't normalize to 100%. This way there's no rounding error involved and everything works perfectly. 
#ifndef FIX_BUGS
                // The percs should always be >= 100 in total (otherwise `PickARandomGroupOfOtherPeds` will fail)
                if (const auto percsSum = notsa::accumulate(percs, (size_t)0); percsSum < 100) {
                    for (auto& p : percs) {
                        p = (size_t)p * 100u / percsSum; // fp math unnecessary here - Rescale to 102% here to make sure we're at 100%
                    }

                    // At this point this must hold - If the value is over 100 that's fine, but it may not be less!
                    // assert(notsa::accumulate(percs, (size_t)0) >= 100u); // In vanilla game this always triggers because of rounding errors... Not much to do.
                }
#endif

            }
        }
    }
    DEV_LOG("POPCYCLE.DAT has been loaded successfully!");
}

// 0x60FBD0
bool CPopCycle::FindNewPedType(ePedType& outPedType, eModelID& outPedMI, bool noGangs, bool noCops) {
    // NOTSA: Bug prevention
    outPedMI = MODEL_INVALID;

    if (!m_pCurrZoneInfo) {
        return false;
    }

    if (CPopulation::bInPoliceStation && CGeneral::RandomBool(70.f)) {
        outPedType = PED_TYPE_COP;
        outPedMI = CPopulation::ChoosePolicePedOccupation();
        return true;
    }

    auto dealersChance = m_NumDealers_Peds - (float)CPopulation::ms_nNumDealers;

    auto gangChance = m_NumGangs_Peds - (float)CPopulation::CalculateTotalNumGangPeds();
    if (CPopulation::m_bOnlyCreateRandomGangMembers) {
        gangChance = 50.f;
    }
    if (CPopulation::m_bDontCreateRandomGangMembers || noGangs) {
        gangChance = -10.f;
    }

    auto copChance = m_NumCops_Peds - (float)CPopulation::ms_nNumCop;
    if (CGangWars::GangWarFightingGoingOn() || CPopulation::m_bDontCreateRandomCops || noCops || m_pCurrZoneInfo->noCops) {
        copChance = -10.f;
    }

    auto civPedsChance = CPopCycle::m_NumOther_Peds - (float)(CPopulation::ms_nNumCivMale + CPopulation::ms_nNumCivFemale);

    for (auto chance : { &civPedsChance, &copChance, &dealersChance, &gangChance }) {
        if (*chance < 2.f) {
            *chance *= CGeneral::GetRandomNumberInRange(0.f, 1.f);
        }
    }

    if (!CGame::CanSeeOutSideFromCurrArea()) {
        dealersChance = -10.f;
    }

    // Pirulax: I had to refactor the code to be acceptable and bugless - sorry
    while (true) {
        const auto highestChance = std::max({ civPedsChance, copChance, dealersChance, gangChance });

        if (highestChance <= 0.f) {
            return false;
        }

        if (highestChance == dealersChance) {
            // 0x60FEF2:
            // Because originally there was no `return` inside the loop itself it always returned at the last viable ID.
            // we reverse the loop and just return at the first viable ID.
            // It seems intentional, but I'm unsure what the purpose was.
            for (auto modelId : CPopulation::GetModelsInPedGroup(CPopulation::GetPedGroupId(POPCYCLE_GROUP_DEALERS)) | rng::views::reverse) {
                if (CStreaming::IsModelLoaded(modelId)) {
                    outPedMI = modelId;
                    assert(outPedMI != MODEL_PLAYER);
                    outPedType = PED_TYPE_DEALER;
                    return true;
                }
            }
            dealersChance = 0.f;
            continue;
        } else if (highestChance == gangChance) { // 0x60FF13
            outPedType = PickGangToCreateMembersOf();
            assert(IsPedTypeGang(outPedType));
            outPedMI = CPopulation::ChooseGangOccupation((eGangID)(outPedType - ePedType::PED_TYPE_GANG1));
            assert(outPedMI != MODEL_PLAYER);
            if (outPedMI >= 0) {
                return true;
            }
            if (CPopulation::m_bOnlyCreateRandomGangMembers) {
                return false;
            }
            gangChance = 0.f;
            continue;
        } else if (highestChance == copChance) { // 0x60FF62
            outPedMI = CPopulation::ChoosePolicePedOccupation();
            outPedType = PED_TYPE_COP;
            return true;
        } else if (highestChance == civPedsChance) { // 0x60FF8F
            outPedMI = CPopulation::ChooseCivilianOccupation();
            switch (outPedMI) {
            case MODEL_INVALID:
            case MODEL_MALE01:
                return false;
            }
            assert(outPedMI != MODEL_PLAYER);
            outPedType = CModelInfo::GetPedModelInfo(outPedMI)->m_nPedType;
            return true;
        } else {
            NOTSA_UNREACHABLE();
        }
    }
}

// 0x610310
float CPopCycle::GetCurrentPercOther_Peds() {
    const auto percOther = (float)m_nPercOther[CPopCycle::m_nCurrentTimeIndex][CPopCycle::m_nCurrentTimeOfWeek][CPopCycle::m_nCurrentZoneType];

    if (CDarkel::FrenzyOnGoing()) {
        return percOther;
    }

    if (CTheScripts::IsPlayerOnAMission()) {
        if (const auto plyr = FindPlayerPed()) {
            if (plyr->IsInVehicle()) {
                switch (plyr->m_pVehicle->m_nModelIndex) {
                case MODEL_TAXI:
                case MODEL_CABBIE:
                    return percOther;
                }
            }
        }
    }

    return percOther * (1.f - std::sqrt(CWeather::Rain) * 0.8f);
}

// 0x610150
bool CPopCycle::IsPedAppropriateForCurrentZone(int32 modelIndex) {
    if (!IsRaceAllowedInCurrentZone((eModelID)modelIndex)) {
        return false;
    }

    // Check if any group active in this zone contains the given model
    for (auto grpId = 0; grpId < POPCYCLE_TOTAL_GROUPS; grpId++) {
        // Check if this group is active now
        if (!m_nPercTypeGroup[m_nCurrentTimeIndex][m_nCurrentTimeOfWeek][m_pCurrZoneInfo->zonePopulationType]) {
            continue;
        }

        // Check if group contains this model
        if (notsa::contains(CPopulation::GetModelsInPedGroup(CPopulation::GetPedGroupId((ePopcycleGroup)(grpId), CPopulation::CurrentWorldZone)), modelIndex)) {
            return true;
        }
    }

    // No group currently active contains the given model
    return false;
}

// 0x610210
bool CPopCycle::IsPedInGroup(int32 modelIndex, ePopcycleGroup group) {
    for (auto pedGroup : CPopulation::GetPedGroupsOfGroup(group)) {
        for (auto mdlId : CPopulation::GetModelsInPedGroup(pedGroup)) {
            if (mdlId == modelIndex) {
                return true;
            }
        }
    }
    return false;
}

// 0x610720
bool CPopCycle::PedIsAcceptableInCurrentZone(int32 modelIndex) {
    if (!m_pCurrZoneInfo) {
        return false;
    }

    if (CCheat::IsZoneStreamingAllowed()) {
        return true;
    }

    if (IsRaceAllowedInCurrentZone((eModelID)modelIndex)) {
        return true;
    }

    return false;
}

// 0x610420
ePopcycleGroup CPopCycle::PickARandomGroupOfOtherPeds() {
    const auto& percs = m_nPercTypeGroup[m_nCurrentTimeIndex][m_nCurrentTimeOfWeek][m_pCurrZoneInfo->zonePopulationType];
    auto rndPerc = CGeneral::GetRandomNumberInRange(
        0,
#ifdef FIX_BUGS // See `Initialise` for an explanation
        (int32)notsa::accumulate(percs, (size_t)0)
#else
        100
#endif
    );
    for (auto [grpIdx, grpPerc] : notsa::enumerate(percs)) {
        if ((int32)(grpPerc) >= rndPerc) {
            return (ePopcycleGroup)grpIdx;
        }
        rndPerc -= (int32)(grpPerc);
    }
    NOTSA_UNREACHABLE();
}

// 0x60FFD0
eModelID CPopCycle::PickPedMIToStreamInForCurrentZone() {
    for (auto tr = 0; tr < 10; tr++) { // 10 tries
        const auto grpId        = PickARandomGroupOfOtherPeds();
        const auto pedGrpId     = CPopulation::GetPedGroupId(grpId, CPopulation::CurrentWorldZone);
        const auto npeds        = CPopulation::GetNumPedsInGroup(pedGrpId);
        auto& nextPedToLoadSlot = CStreaming::ms_NextPedToLoadFromGroup[grpId];
        for (auto p = 0; p < npeds; p++) {
            nextPedToLoadSlot  = (nextPedToLoadSlot + 1) % npeds;
            const auto modelId = (eModelID)CPopulation::GetPedGroupModelId(pedGrpId, nextPedToLoadSlot);
            if (!notsa::contains(CStreaming::ms_pedsLoaded, modelId) && IsRaceAllowedInCurrentZone(modelId)) {
                return modelId;
            }
        }
    }
    return MODEL_INVALID;
}

// 0x610490
void CPopCycle::PlayerKilledADealer() {
    if (m_pCurrZoneInfo && m_pCurrZoneInfo->DrugDealerCounter) {
        m_pCurrZoneInfo->DrugDealerCounter--;
    }
}

// 0x610BF0
void CPopCycle::Update() {
    ZoneScoped;

    m_nCurrentTimeOfWeek = [] {
        switch (CClock::GetGameWeekDay()) {
        case 0: // Not sure (Maybe Sunday)
        case 7: // Sunday
            return 1;
        case 1:  // Monday
            return CClock::GetGameClockHours() >= 20 ? 0 : 1;
        case 2:
        case 3:
        case 4:
        case 5:
            return 0;
        case 6: // Saturday
            return CClock::GetGameClockHours() >= 20 ? 1 : 0;
        }
        NOTSA_UNREACHABLE();
    }();

    m_nCurrentTimeIndex = CClock::GetGameClockHours() / 2;

    if (const auto& pos = FindPlayerCentreOfWorld(); pos.z < 950.f || !m_pCurrZoneInfo) {
        m_pCurrZoneInfo = CTheZones::GetZoneInfo(pos, &m_pCurrZone);
        m_nCurrentZoneType = m_pCurrZoneInfo->zonePopulationType;
    }

    UpdatePercentages();
    UpdateDealerStrengths();
    UpdateAreaDodgyness();
    UpdateIsGangArea();
}

// 0x610560
void CPopCycle::UpdateAreaDodgyness() {
    m_fCurrentZoneDodgyness = std::min((float)m_pCurrZoneInfo->DrugDealerCounter * 0.07f + (float)m_pCurrZoneInfo->GetSumOfGangDensity() / 100.f, 1.0f);
}

// 0x6104B0
void CPopCycle::UpdateDealerStrengths() {
    if (!CGangWars::bGangWarsActive) {
        return;
    }

    if (CTimer::m_snTimeInMilliseconds / 60000 != CTimer::m_snPreviousTimeInMilliseconds / 60000) {
        return;
    }

    if (!CTheZones::TotalNumberOfMapZones) {
        return;
    }

    for (auto& zone : CTheZones::ZoneInfoArray) {
        const auto Chk = [&](eGangID gangId) { return zone.GangDensity[gangId] > 10u; };
        if (!Chk(GANG_BALLAS) && !Chk(GANG_GROVE) && !Chk(GANG_VAGOS)) {
            continue;
        }

        constexpr float chances[]{ 0.05f, 0.2f, 0.3f, 0.35f, 0.4f, 0.5f, 0.55f, 0.6f, 0.65f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f }; // 0x8D24F0
        if (zone.DrugDealerCounter >= std::size(chances)) {
            continue;
        }

        if (CGeneral::GetRandomNumber() >= chances[zone.DrugDealerCounter]) {
            continue;
        }

        zone.DrugDealerCounter++;
    }
}

// 0x610770
void CPopCycle::UpdatePercentages() {
    m_fPercDealers = std::max(0.1f, (float)m_pCurrZoneInfo->DrugDealerCounter / 100.f);

    m_fPercGangs = std::min(0.5f, (float)m_pCurrZoneInfo->GetSumOfGangDensity() / 100.f);
    m_fPercCops = m_fPercGangs >= 0.15f
        ? std::max(0.03f, 0.3f - m_fPercGangs)
        : std::max(0.02f, m_fPercGangs);

    // 0x610881
    m_fPercCops = [] {
        switch (m_pCurrZoneInfo->zonePopulationType) {
        case POPCYCLE_PEDGROUP_BUSINESS_SF:
        case POPCYCLE_PEDGROUP_CASUAL_RICH_LA:
        case POPCYCLE_PEDGROUP_CASUAL_RICH_VG:
            return m_fPercCops <= 0.1f ? 0.1f : m_fPercCops;
        case POPCYCLE_PEDGROUP_BUSINESS_VG:
            return m_fPercCops <= 0.05f ? 0.05f : m_fPercCops;
        case POPCYCLE_PEDGROUP_CLUBBERS_VG:
        case POPCYCLE_PEDGROUP_CASUAL_AVERAGE_LA:
            return 0.f;
        }
        return m_fPercCops;
    }();

    // 0x610922:
    if (const auto sum = m_fPercDealers + m_fPercGangs + m_fPercCops; sum <= 1.f) {
        m_fPercOther = 1.f - sum;
    } else { // Otherwise normalize all values by the sum (This will make their new sum be `1.f`)
        m_fPercOther    = 0.f;
        m_fPercDealers /= sum;
        m_fPercGangs   /= sum;
        m_fPercCops    /= sum;
    }

    // 0x610A7D
    gfLaRiotsLightMult = (CGameLogic::LaRiotsActiveHere() && (1.f - CCustomBuildingDNPipeline::m_fDNBalanceParam) > 0.5f)
        ? std::max(0.6f, gfLaRiotsLightMult - 0.01f) // Decrease
        : std::min(1.f, gfLaRiotsLightMult + 0.01f); // Increase

    // 0x610A41 + 0x610A57
    const auto maxNumPeds = (float)(
        CGameLogic::LaRiotsActiveHere()
            ? std::min<uint8>(20u, GetMaxPedsCurrently())
            : GetMaxPedsCurrently()
    );

    // From all the data above, calculate the actual ped/car numbers for this zone
    const auto Process = [
        maxNumPeds,
        maxNumCars = GetMaxCarsCurrently()
    ](PercDataArray& maxPercLUT, float percPeds, float percCars, float& nOutPeds, float& nOutCars) {
        const auto maxPercOfType = (float)maxPercLUT[m_nCurrentTimeIndex][m_nCurrentTimeOfWeek][m_nCurrentZoneType] / 100.f;

        nOutPeds = maxNumPeds * (maxPercOfType * percPeds);
        nOutCars = maxNumCars * (maxPercOfType * percCars);

        if (CGameLogic::LaRiotsActiveHere()) {
            nOutCars *= 0.75f;
        }
    };
    Process(m_nPercDealers, m_fPercDealers,                                    m_fPercDealers, m_NumDealers_Peds, m_NumDealers_Cars);
    Process(m_nPercGang,    m_fPercGangs,                                      m_fPercGangs,   m_NumGangs_Peds,   m_NumGangs_Cars  );
    Process(m_nPercCops,    m_fPercCops,                                       m_fPercCops,    m_NumCops_Peds,    m_NumCops_Cars   );
    Process(m_nPercOther,   m_fPercOther * GetCurrentPercOther_Peds() / 100.f, m_fPercOther,   m_NumOther_Peds,   m_NumOther_Cars  );
}

// 0x60F8D0
ePedType CPopCycle::PickGangToCreateMembersOf() {
    if (CCheat::IsActive(CHEAT_GANGS_CONTROLS_THE_STREETS)) {
        return CGeneral::RandomChoice(GetAllGangPedTypes());
    }

    const auto dominatingGangId = rng::max(
        rng::iota_view{0u, (size_t)TOTAL_GANGS},
        rng::less{},
        [sumGangDensity = (float)m_pCurrZoneInfo->GetSumOfGangDensity()](auto gangId) {
            return (float)m_pCurrZoneInfo->GangDensity[gangId] / sumGangDensity - (float)CPopulation::ms_nNumGang[gangId] / m_NumGangs_Peds;
        }
    );
    return (ePedType)((size_t)PED_TYPE_GANG1 + dominatingGangId);
}

// notsa
bool CPopCycle::IsRaceAllowedInCurrentZone(ePedRace race) {
    return race == RACE_DEFAULT || m_pCurrZoneInfo->zonePopulationRace & (1 << (race - 1));
}

// notsa
bool CPopCycle::IsRaceAllowedInCurrentZone(eModelID pedModelId) {
    return IsRaceAllowedInCurrentZone(CModelInfo::GetPedModelInfo((int32)pedModelId)->GetRace());
}

void CPopCycle::UpdateIsGangArea() {
    m_bCurrentZoneIsGangArea = m_pCurrZoneInfo->GetSumOfGangDensity() > 20;
}
