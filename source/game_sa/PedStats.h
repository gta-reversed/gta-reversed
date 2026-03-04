#pragma once

#include "ePedStats.h"

class CPedStat {
public:
    uint32 m_nId;
    char   m_acName[24];
    float  m_fFleeDistance;
    float  m_fHeadingChangeRate;
    uint8  m_nFear;
    uint8  m_nTemper;
    uint8  m_nLawfulness;
    uint8  m_nSexiness;
    float  m_fAttackStrength;
    float  m_fDefendWeakness;
    uint16 m_flags;
    char   m_nDefaultDecisionMaker;
};

VALIDATE_SIZE(CPedStat, 0x34);

class CPedStats {
public:
    static CPedStat*& ms_apPedStats;

public:
    static void InjectHooks();

    static void Initialise();
    static void Shutdown();
    static void LoadPedStats();
    static ePedStats GetPedStatType(const char* statName);
    static CPedStat* GetPedStatInfo(const char* statName);
    static CPedStat* GetPedStatByArrayIndex(uint32 statIndex);
    static void FindIndexWithPedStat(void* a1);
};
