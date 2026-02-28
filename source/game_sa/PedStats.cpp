#include "StdInc.h"

#include "PedStats.h"

#include "DecisionMakers/DecisionMakerTypesFileLoader.h"

constexpr uint16 PED_STATS_COUNT = 43;

CPedStat*& CPedStats::ms_apPedStats = *(CPedStat**)0xC0BBEC;

void CPedStats::InjectHooks() {
    RH_ScopedClass(CPedStats);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5BF9D0);
    RH_ScopedInstall(Shutdown, 0x608850);
    RH_ScopedInstall(LoadPedStats, 0x5BB890);
    RH_ScopedInstall(GetPedStatType, 0x6088D0);
}

// 0x5BF9D0
void CPedStats::Initialise() {
    ms_apPedStats = new CPedStat[PED_STATS_COUNT]();
    for (uint16 statIndex = 0; statIndex < PED_STATS_COUNT; statIndex++) {
        ms_apPedStats[statIndex].m_nId = 0;
        ms_apPedStats[statIndex].m_fFleeDistance = 20.0f;
        ms_apPedStats[statIndex].m_fHeadingChangeRate = 15.0f;
        ms_apPedStats[statIndex].m_nFear = 50;
        ms_apPedStats[statIndex].m_nTemper = 50;
        ms_apPedStats[statIndex].m_nLawfulness = 50;
        ms_apPedStats[statIndex].m_nSexiness = 50;
        ms_apPedStats[statIndex].m_fAttackStrength = 1.0f;
        ms_apPedStats[statIndex].m_fDefendWeakness = 1.0f;
        ms_apPedStats[statIndex].m_flags = 0;
        ms_apPedStats[statIndex].m_nDefaultDecisionMaker = eDecisionMakerType::PED_GROUPMEMBER;
    }
    LoadPedStats();
    CDecisionMakerTypesFileLoader::LoadDefaultDecisionMaker();
}

// 0x608850
void CPedStats::Shutdown() {
    delete[] ms_apPedStats;
}

// 0x5BB890
void CPedStats::LoadPedStats() {
    uint16 statIndex = 0;
    auto file = CFileMgr::OpenFile("DATA\\PEDSTATS.DAT", "r");
    for (char* line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (!*line || *line == '#') {
            continue;
        }

        char  name[24];
        float fleeDistance;
        float headingChangeRate;
        int32 fear;
        int32 temper;
        int32 lawfulness;
        int32 sexiness;
        float attackStrength;
        float defendWeakness;
        int32 shootingRate;
        int32 defaultDecisionMaker;

        VERIFY(sscanf_s(
            line,
            "%s %f %f %d %d %d %d %f %f %d %d",
            SCANF_S_STR(name),
            &fleeDistance,
            &headingChangeRate,
            &fear,
            &temper,
            &lawfulness,
            &sexiness,
            &attackStrength,
            &defendWeakness,
            &shootingRate,
            &defaultDecisionMaker
        ) == 11);

        ms_apPedStats[statIndex].m_nId = statIndex;
        rng::copy(name, ms_apPedStats[statIndex].m_acName);
        ms_apPedStats[statIndex].m_fFleeDistance = fleeDistance;
        ms_apPedStats[statIndex].m_fHeadingChangeRate = headingChangeRate;
        ms_apPedStats[statIndex].m_nFear = fear;
        ms_apPedStats[statIndex].m_nTemper = temper;
        ms_apPedStats[statIndex].m_nLawfulness = lawfulness;
        ms_apPedStats[statIndex].m_nSexiness = sexiness;
        ms_apPedStats[statIndex].m_fAttackStrength = attackStrength;
        ms_apPedStats[statIndex].m_fDefendWeakness = defendWeakness;
        ms_apPedStats[statIndex].m_flags = shootingRate;
        ms_apPedStats[statIndex].m_nDefaultDecisionMaker = (eDecisionMakerType)(defaultDecisionMaker);

        statIndex++;
    }
    CFileMgr::CloseFile(file);
}

// 0x6088D0
ePedStats CPedStats::GetPedStatType(const char* statName) {
  uint16 statIndex = 0;
  while (strcmp(ms_apPedStats[statIndex].m_acName, statName) != 0) {
    if (++statIndex >= PED_STATS_COUNT)
      return ePedStats::SENSIBLE_GUY;
  }
  return static_cast<ePedStats>(statIndex);
}

// unused
// 0x608860
CPedStat* CPedStats::GetPedStatInfo(const char* statName) {
    for (uint16 statIndex = 0; statIndex < PED_STATS_COUNT; statIndex++) {
        if (strcmp(ms_apPedStats[statIndex].m_acName, statName) != 0) {
            return &ms_apPedStats[statIndex];
        }
    }
    return nullptr;
}

// unused
// 0x5DE7C0
CPedStat* CPedStats::GetPedStatByArrayIndex(uint32 statIndex) {
    return &ms_apPedStats[statIndex];
}

// unused
// 0x608940
void CPedStats::FindIndexWithPedStat(void* a1) {
    plugin::Call<0x608940>();
}
