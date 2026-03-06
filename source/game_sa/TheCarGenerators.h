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
    static inline auto& GenerateEvenIfPlayerIsCloseCounter = StaticRef<uint8>(0xC279D0);
    static inline auto& ProcessCounter = StaticRef<uint8>(0xC279D1);
    static inline auto& NumOfCarGenerators = StaticRef<uint32>(0xC279D4);
    static inline auto& m_SpecialPlateHandler = StaticRef<CSpecialPlateHandler>(0xC279D8);
    static inline auto (&CarGeneratorArray)[NUM_CAR_GENERATORS] = StaticRef<CCarGenerator[NUM_CAR_GENERATORS]>(0xC27AD0);

public:
    static void InjectHooks();

    static int32 CreateCarGenerator(CVector posn, float angle, int32 modelId, int16 color1, int16 color2, uint8 forceSpawn, uint8 alarmChances, uint8 doorLockChances,
                                         uint16 minDelay, uint16 maxDelay, uint8 iplId, bool ignorePopulationLimit);
    static void Init();
    static void Load();
    static void Process();
    static void RemoveCarGenerators(uint8 IplID);
    static void Save();

    static CCarGenerator* Get(uint16 index);
    static int32 GetIndex(CCarGenerator* pCarGen);
};
