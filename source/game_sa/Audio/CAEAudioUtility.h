#pragma once

class CVehicle;

class CAEAudioUtility {
public:
    static int32     GetRandomNumberInRange(const int32 min, const int32 max);
    static float     GetRandomNumberInRange(float a, float b);
    static bool      ResolveProbability(float prob);
    static bool      GetBankAndSoundFromScriptSlotAudioEvent(int32* a1, int32* a2, int32* a3, int32 a4);
    static float     GetPiecewiseLinear(float x, int16 dataCount, float (*data)[2]);
    static CVehicle* FindVehicleOfPlayer();
    static float     AudioLog10(float p);
    static int64     GetCurrentTimeInMilliseconds();
    static uint32    ConvertFromBytesToMS(uint32 a, uint32 frequency, uint16 frequencyMult);
    static uint32    ConvertFromMSToBytes(uint32 a, uint32 frequency, uint16 frequencyMult);
    static void      StaticInitialise();

private:
    static int64& startTimeMs;
    static float (&m_sfLogLookup)[50][2];

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};
