/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "ZoneInfo.h"
class CZone;

enum ePedType : uint32;

// used in CPopCycle::m_nPercTypeGroup
enum ePopcycleGroupPerc {
    POPCYCLE_GROUP_PERC_WORKERS = 0,
    POPCYCLE_GROUP_PERC_BUSINESS,
    POPCYCLE_GROUP_PERC_CLUBBERS,
    POPCYCLE_GROUP_PERC_FARMERS,
    POPCYCLE_GROUP_PERC_BEACHFOLK,
    POPCYCLE_GROUP_PERC_PARKFOLK,
    POPCYCLE_GROUP_PERC_CASUAL_RICH,
    POPCYCLE_GROUP_PERC_CASUAL_AVERAGE,
    POPCYCLE_GROUP_PERC_CASUAL_POOR,
    POPCYCLE_GROUP_PERC_PROZZIES,
    POPCYCLE_GROUP_PERC_CRIMIMALS,
    POPCYCLE_GROUP_PERC_GOLFERS,
    POPCYCLE_GROUP_PERC_SERVANTS,
    POPCYCLE_GROUP_PERC_AIRCREW,
    POPCYCLE_GROUP_PERC_ENTERTAINERS,
    POPCYCLE_GROUP_PERC_OOT_FACT, // means out of town factory
    POPCYCLE_GROUP_PERC_DESERTFOLK,
    POPCYCLE_GROUP_PERC_AIRCREW_RUNWAY,
    POPCYCLE_TOTAL_GROUP_PERCS
};

// used in CPopulation::m_TranslationArray
enum ePopcycleGroup {
    POPCYCLE_GROUP_WORKERS = 0,
    POPCYCLE_GROUP_BUSINESS,
    POPCYCLE_GROUP_CLUBBERS,
    POPCYCLE_GROUP_FARMERS,
    POPCYCLE_GROUP_BEACHFOLK,
    POPCYCLE_GROUP_PARKFOLK,
    POPCYCLE_GROUP_CASUAL_RICH,
    POPCYCLE_GROUP_CASUAL_AVERAGE,
    POPCYCLE_GROUP_CASUAL_POOR,
    POPCYCLE_GROUP_PROSTITUTES,
    POPCYCLE_GROUP_CRIMINALS,
    POPCYCLE_GROUP_GOLFERS,
    POPCYCLE_GROUP_SERVANTS,
    POPCYCLE_GROUP_AIRCREW,
    POPCYCLE_GROUP_ENTERTAINERS,
    POPCYCLE_GROUP_OUT_OF_TOWN_FACTORY_WORKERS,
    POPCYCLE_GROUP_DESERT_FOLK,
    POPCYCLE_GROUP_AIRCREW_RUNWAY,

    POPCYCLE_GROUP_BALLAS,
    POPCYCLE_GROUP_GROVE,
    POPCYCLE_GROUP_VAGOS,
    POPCYCLE_GROUP_SF_RIFA,
    POPCYCLE_GROUP_DA_NANG_BOYS,
    POPCYCLE_GROUP_ITALIAN_MAFIA,
    POPCYCLE_GROUP_TRIADS,
    POPCYCLE_GROUP_VARRIO_LOS_AZTECAZ,
    POPCYCLE_GROUP_UNUSED_1,
    POPCYCLE_GROUP_UNUSED_2,
    POPCYCLE_GROUP_DEALERS,
    POPCYCLE_GROUP_SHOPKEEPERS,
    POPCYCLE_GROUP_OFFICE_WORKERS,
    POPCYCLE_GROUP_HUSBANDS,
    POPCYCLE_GROUP_WIVES,
    POPCYCLE_TOTAL_GROUPS
};

// used in CPopulation::m_PedGroups
enum ePopcyclePedGroup {
    POPCYCLE_PEDGROUP_WORKERS_LA,
    POPCYCLE_PEDGROUP_WORKERS_SF,
    POPCYCLE_PEDGROUP_WORKERS_VG,
    POPCYCLE_PEDGROUP_BUSINESS_LA,
    POPCYCLE_PEDGROUP_BUSINESS_SF,
    POPCYCLE_PEDGROUP_BUSINESS_VG,
    POPCYCLE_PEDGROUP_CLUBBERS_LA,
    POPCYCLE_PEDGROUP_CLUBBERS_SF,
    POPCYCLE_PEDGROUP_CLUBBERS_VG,
    POPCYCLE_PEDGROUP_FARMERS,
    POPCYCLE_PEDGROUP_BEACHFOLK,
    POPCYCLE_PEDGROUP_PARKFOLK_LA,
    POPCYCLE_PEDGROUP_PARKFOLK_SF,
    POPCYCLE_PEDGROUP_PARKFOLK_VG,
    POPCYCLE_PEDGROUP_CASUAL_RICH_LA,
    POPCYCLE_PEDGROUP_CASUAL_RICH_SF,
    POPCYCLE_PEDGROUP_CASUAL_RICH_VG,
    POPCYCLE_PEDGROUP_CASUAL_AVERAGE_LA,
    POPCYCLE_PEDGROUP_CASUAL_AVERAGE_SF,
    POPCYCLE_PEDGROUP_CASUAL_AVERAGE_VG,
    POPCYCLE_PEDGROUP_CASUAL_POOR_LA,
    POPCYCLE_PEDGROUP_CASUAL_POOR_SF,
    POPCYCLE_PEDGROUP_CASUAL_POOR_VG,
    POPCYCLE_PEDGROUP_PROSTITUTES_LA,
    POPCYCLE_PEDGROUP_PROSTITUTES_SF,
    POPCYCLE_PEDGROUP_PROSTITUTES_VG,
    POPCYCLE_PEDGROUP_CRIMINALS_LA,
    POPCYCLE_PEDGROUP_CRIMINALS_SF,
    POPCYCLE_PEDGROUP_CRIMINALS_VG,
    POPCYCLE_PEDGROUP_GOLFERS,
    POPCYCLE_PEDGROUP_SERVANTS_LA,
    POPCYCLE_PEDGROUP_SERVANTS_SF,
    POPCYCLE_PEDGROUP_SERVANTS_VG,
    POPCYCLE_PEDGROUP_AIRCREW_LA,
    POPCYCLE_PEDGROUP_AIRCREW_SF,
    POPCYCLE_PEDGROUP_AIRCREW_VG,
    POPCYCLE_PEDGROUP_ENTERTAINERS_LA,
    POPCYCLE_PEDGROUP_ENTERTAINERS_SF,
    POPCYCLE_PEDGROUP_ENTERTAINERS_VG,
    POPCYCLE_PEDGROUP_OUT_OF_TOWN_FACTORY_WORKERS,
    POPCYCLE_PEDGROUP_DESERT_FOLK,
    POPCYCLE_PEDGROUP_AIRCREW_RUNWAY,
    POPCYCLE_PEDGROUP_BALLAS,
    POPCYCLE_PEDGROUP_GROVE,
    POPCYCLE_PEDGROUP_VAGOS,
    POPCYCLE_PEDGROUP_SF_RIFA,
    POPCYCLE_PEDGROUP_DA_NANG_BOYS,
    POPCYCLE_PEDGROUP_ITALIAN_MAFIA,
    POPCYCLE_PEDGROUP_TRIADS,
    POPCYCLE_PEDGROUP_VARRIO_LOS_AZTECAZ,
    POPCYCLE_PEDGROUP_DEALERS = 52,
    POPCYCLE_PEDGROUP_SHOPKEEPERS,
    POPCYCLE_PEDGROUP_OFFICE_WORKERS,
    POPCYCLE_PEDGROUP_HUSBANDS,
    POPCYCLE_PEDGROUP_WIVES,
    POPCYCLE_TOTAL_PEDGROUPS
};

// used in CPopulation::m_nNumCarsInGroup and CPopulation::m_CarGroups
enum ePopcycleCarGroup {
    POPCYCLE_CARGROUP_WORKERS,
    POPCYCLE_CARGROUP_BUSINESS,
    POPCYCLE_CARGROUP_CLUBBERS,
    POPCYCLE_CARGROUP_FARMERS,
    POPCYCLE_CARGROUP_BEACHFOLK,
    POPCYCLE_CARGROUP_PARKFOLK,
    POPCYCLE_CARGROUP_CASUAL_RICH,
    POPCYCLE_CARGROUP_CASUAL_AVERAGE,
    POPCYCLE_CARGROUP_CASUAL_POOR,
    POPCYCLE_CARGROUP_PROSTITUTES,
    POPCYCLE_CARGROUP_CRIMINALS,
    POPCYCLE_CARGROUP_GOLFERS,
    POPCYCLE_CARGROUP_SERVANTS,
    POPCYCLE_CARGROUP_AIRCREW,
    POPCYCLE_CARGROUP_ENTERTAINERS,
    POPCYCLE_CARGROUP_OUT_OF_TOWN_FACTORY,
    POPCYCLE_CARGROUP_DESERT_FOLK,
    POPCYCLE_CARGROUP_AIRCREW_RUNWAY,
    POPCYCLE_CARGROUP_BALLAS,
    POPCYCLE_CARGROUP_GROVE,
    POPCYCLE_CARGROUP_VAGOS,
    POPCYCLE_CARGROUP_SF_RIFA,
    POPCYCLE_CARGROUP_DA_NANG_BOYS,
    POPCYCLE_CARGROUP_ITALIAN_MAFIA,
    POPCYCLE_CARGROUP_TRIADS,
    POPCYCLE_CARGROUP_VARRIO_LOS_AZTECAZ,
    POPCYCLE_CARGROUP_DEALERS = 28,
    POPCYCLE_CARGROUP_BOATS,
    POPCYCLE_CARGROUP_CHEAT1,
    POPCYCLE_CARGROUP_CHEAT2,
    POPCYCLE_CARGROUP_CHEAT3,
    POPCYCLE_CARGROUP_CHEAT4,
    POPCYCLE_TOTAL_CARGROUPS
};

class CPopCycle {
public:
    // Based on `popcycle.dat`
    enum class ZoneType {
        BUSINESS,
        DESERT,
        ENTERTAINMENT,
        COUNTRYSIDE,
        RESIDENTIAL_RICH,
        RESIDENTIAL_AVERAGE,
        RESIDENTIAL_POOR,
        GANGLAND,
        BEACH,
        SHOPPING,
        PARK,
        INDUSTRY,
        ENTERTAINMENT_BUSY,
        SHOPPING_BUSY,
        SHOPPING_POSH,
        RESIDENTIAL_RICH_SECLUDED,
        AIRPORT,
        GOLF_CLUB,
        OUT_OF_TOWN_FACTORY,
        AIRPORT_RUNWAY,

        // Add above this
        COUNT,
    };
public:
    static float&      m_NumOther_Cars;
    static float&      m_NumCops_Cars;
    static float&      m_NumGangs_Cars;
    static float&      m_NumDealers_Cars;
    static float&      m_NumOther_Peds;
    static float&      m_NumCops_Peds;
    static float&      m_NumGangs_Peds;
    static float&      m_fPercOther;
    static float&      m_fPercCops;
    static float&      m_fPercGangs;
    static float&      m_fPercDealers;
    static bool&       m_bCurrentZoneIsGangArea;
    static float&      m_fCurrentZoneDodgyness;
    static CZone*&     m_pCurrZone;
    static CZoneInfo*& m_pCurrZoneInfo;
    static int32&      m_nCurrentZoneType;
    static int32&      m_nCurrentTimeOfWeek;
    static int32&      m_nCurrentTimeIndex;
    static float&      m_NumDealers_Peds;

    static constexpr auto PERC_DATA_TIME_RESOLUTION_HR = 2;

    // TODO: Eventually use an array of structs instead of 7x 4 dimensional arrays....
    using PercDataArray = uint8[24 / PERC_DATA_TIME_RESOLUTION_HR /*time resolution (hr)*/][2 /*weekday and weekend*/][(uint32)(ZoneType::COUNT)];
    static inline PercDataArray& m_nPercOther = *(PercDataArray*)0xC0DE38;
    static inline PercDataArray& m_nPercCops = *(PercDataArray*)0xC0E018;
    static inline PercDataArray& m_nPercGang = *(PercDataArray*)0xC0E1F8;
    static inline PercDataArray& m_nPercDealers = *(PercDataArray*)0xC0E3D8;
    static inline PercDataArray& m_nMaxNumCars = *(PercDataArray*)0xC0E5B8;
    static inline PercDataArray& m_nMaxNumPeds = *(PercDataArray*)0xC0E798;

    static inline uint8 (&m_nPercTypeGroup)[24 / PERC_DATA_TIME_RESOLUTION_HR][2 /*weekday or weekend*/][(uint32)(ZoneType::COUNT)][POPCYCLE_TOTAL_GROUP_PERCS] = *(uint8(*)[12][2][20][18])0xC0BC78;

    static void InjectHooks();

    static void  Initialise();
    static bool  FindNewPedType(ePedType& outPedType, int32& outPedMI, bool noGangs, bool noCops);
    static float GetCurrentPercOther_Peds();

    /*
    * @addr 0x610150
    * @brief Check if any currently active group in the current world zone contains the given model
    */
    static bool IsPedAppropriateForCurrentZone(int32 modelIndex);

    /*!
    * @addr 0x610210
    * @brief Check if group contains the specified model (In any world zone)
    */
    static bool IsPedInGroup(int32 modelIndex, ePopcycleGroup PopCycle_Group);
    static bool  PedIsAcceptableInCurrentZone(int32 modelIndex);
    static int32 PickARandomGroupOfOtherPeds();
    static int32 PickPedMIToStreamInForCurrentZone();
    static void  PlayerKilledADealer();
    static void  Update();
    static void  UpdateAreaDodgyness();
    static void  UpdateDealerStrengths();
    static void  UpdatePercentages();
    static ePedType PickGangToCreateMembersOf();


    static uint8 GetCurrentPercTypeGroup(int32 groupId) {
        return m_nPercTypeGroup[m_nCurrentTimeIndex][m_nCurrentTimeOfWeek][m_pCurrZoneInfo->zonePopulationType][groupId];
    }

    static bool IsPedInGroupTheseGroups(int32 modelIndex, std::initializer_list<ePopcycleGroup> groups) {
        return rng::any_of(groups, [=](auto group) { return IsPedInGroup(modelIndex, group); });
    }
};
