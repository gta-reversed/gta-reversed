/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CCarGenerator;
class CSpecialPlateHandler;

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

    static int32 CreateCarGenerator(
        CVector posn,
        float   angle,
        int32   modelId,
        uint8   primaryColor,
        uint8   secondaryColor,
        bool    isHighPriority,
        uint8   chanceOfAlarm,
        uint8   chanceOfDoorLock,
        uint16  minDelay,
        uint16  maxDelay,
        uint8   iplId,
        bool    ignorePopulationLimit
    );
    static void Init();
    static void Load();
    static void Process();
    static void RemoveCarGenerators(uint8 IplID);
    static void Save();

    static CCarGenerator* Get(uint16 index);
    static int32 GetIndex(CCarGenerator* pCarGen);
};
