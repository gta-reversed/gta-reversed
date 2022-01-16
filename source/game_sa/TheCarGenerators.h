/*
Plugin-SDK (Grand Theft Auto San Andreas) header file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "SpecialPlateHandler.h"
#include "CarGenerator.h"

static constexpr int32 NUM_CAR_GENERATORS = 500;

class CTheCarGenerators {
public:
    static uint8& GenerateEvenIfPlayerIsCloseCounter;
    static uint8& ProcessCounter;
    static uint32& NumOfCarGenerators;
    static CSpecialPlateHandler& m_SpecialPlateHandler;
    static CCarGenerator(&CarGeneratorArray)[NUM_CAR_GENERATORS];

public:
    static void InjectHooks();

    static signed int CreateCarGenerator(CVector posn, float angle, int32 modelId, int16 color1, int16 color2, uint8 forceSpawn, uint8 alarmChances, uint8 doorLockChances,
                                         uint16 minDelay, uint16 maxDelay, uint8 iplId, bool ignorePopulationLimit);
    static void Init();
    static void Load();
    static void Process();
    static void RemoveCarGenerators(uint8 IplID);
    static void Save();

    static CCarGenerator* Get(uint16 index);
    static int32 GetIndex(CCarGenerator* pCarGen);
};
