#pragma once
#include "eRadioID.h"

class CVehicle;

class CAEAudioUtility {
public:
    static void      StaticInitialise();

    template<typename T>
    static inline T GetRandomNumberInRange(T min, T max) {
        if constexpr (std::is_integral_v<T>) {
            return CGeneral::GetRandomNumberInRange<T>(min, max + 1);
        } else {
            return CGeneral::GetRandomNumberInRange<T>(min, max);
        }

    }

    static CVehicle* FindVehicleOfPlayer();
    static bool      ResolveProbability(float prob);
    static float     AudioLog10(float p);

    static uint32    ConvertFromBytesToMS(uint32 lengthInBytes, uint32 frequency, uint16 frequencyMult);
    static uint32    ConvertFromMSToBytes(uint32 a, uint32 frequency, uint16 frequencyMult);

    static bool      GetBankAndSoundFromScriptSlotAudioEvent(int32& slot, int32& outBank, int32& outSound, int32 a4);
    static float     GetPiecewiseLinear(float x, int16 dataCount, float (*data)[2]);
    static uint64    GetCurrentTimeInMS();

    // NOTSA
    static eRadioID GetRandomRadioStation() {
        return static_cast<eRadioID>(GetRandomNumberInRange(1, RADIO_COUNT - 1));
    }

private:
    static uint64& startTimeMs;
    static float (&m_sfLogLookup)[50][2];

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};
