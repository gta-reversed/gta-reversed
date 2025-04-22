#pragma once
#include "eRadioID.h"
#include <General.h>

class CVehicle;

class CAEAudioUtility {
public:
    static void      StaticInitialise();

    /*!
    * @brief This and CGeneral differs in that this function returns a number [min, max + 1], while
    * @brief the other [min, max]. To solve this we do `max + 1`
    */
    template<std::integral T>
    static T GetRandomNumberInRange(T min, T max) {
        return CGeneral::GetRandomNumberInRange<T>(min, max + 1);
    }

    template<typename T>
        requires std::is_floating_point_v<T>
    static T GetRandomNumberInRange(T min, T max) {
        return CGeneral::GetRandomNumberInRange<T>(min, max);
    }

    static CVehicle* FindVehicleOfPlayer();
    static bool      ResolveProbability(float prob);
    static float     AudioLog10(float p);

    static uint32    ConvertFromBytesToMS(uint32 lengthInBytes, uint32 sampleRate, uint16 numChannels);
    static uint32    ConvertFromMSToBytes(uint32 a, uint32 frequency, uint16 frequencyMult);

    static bool      GetBankAndSoundFromScriptSlotAudioEvent(const eAudioEvents& ae, eSoundBankS32& outBankID, int32& outSoundID, int32 slot);
    static float     GetPiecewiseLinear(float x, int16 dataCount, float (*data)[2]);
    static uint64    GetCurrentTimeInMS();

    // NOTSA
    static eRadioID GetRandomRadioStation() {
        return static_cast<eRadioID>(GetRandomNumberInRange(1, RADIO_COUNT - 1));
    }

private:
    static float (&m_sfLogLookup)[50][2];

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};
