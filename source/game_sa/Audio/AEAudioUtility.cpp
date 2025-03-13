#include "StdInc.h"

#include <chrono>

#include "AEAudioUtility.h"

auto& Frequency = StaticRef<LARGE_INTEGER>(0xB610F0);
uint64& startTimeMs = *reinterpret_cast<uint64*>(0xb610f8);
float (&CAEAudioUtility::m_sfLogLookup)[50][2] = *reinterpret_cast<float (*)[50][2]>(0xb61100);

// NOTE: For me all values were 0... The below values should be the correct ones:
static constexpr std::array<eSoundBank, AE_SCRIPT_BANK_LAST - AE_SCRIPT_BANK_FIRST> gScriptBanksLookup = {
    SND_BANK_SCRIPT_VIDEO_POKER,
    SND_BANK_SCRIPT_SHOOTING_RANGE,
    SND_BANK_SCRIPT_POOL_MINIGAME,
    SND_BANK_SCRIPT_NULL,
    SND_BANK_SCRIPT_KEYPAD,
    SND_BANK_SCRIPT_NULL,
    SND_BANK_SCRIPT_GOGO,
    SND_BANK_SCRIPT_DUALITY,
    SND_BANK_SCRIPT_NULL,
    SND_BANK_SCRIPT_BLACK_PROJECT,
    SND_BANK_SCRIPT_BEE,
    SND_BANK_SCRIPT_BASKETBALL,
    SND_BANK_SCRIPT_MEAT_BUSINESS,
    SND_BANK_SCRIPT_ROULETTE,
    SND_BANK_SCRIPT_BANDIT,
    SND_BANK_SCRIPT_NULL,
    SND_BANK_SCRIPT_NULL,
    SND_BANK_SCRIPT_OGLOC,
    SND_BANK_SCRIPT_CARGO_PLANE,
    SND_BANK_SCRIPT_DA_NANG,
    SND_BANK_SCRIPT_GYM,
    SND_BANK_SCRIPT_OTB,
    SND_BANK_SCRIPT_STINGER,
    SND_BANK_SCRIPT_UNCLE_SAM,
    SND_BANK_SCRIPT_VERTICAL_BIRD,
    SND_BANK_SCRIPT_MECHANIC,
    SND_BANK_SCRIPT_CAT2_BANK,
    SND_BANK_SCRIPT_AIR_HORN,
    SND_BANK_SCRIPT_RESTAURANT,
    SND_BANK_SCRIPT_TEMPEST
};

void CAEAudioUtility::InjectHooks() {
    RH_ScopedClass(CAEAudioUtility);
    RH_ScopedCategory("Audio");

    RH_ScopedOverloadedInstall(GetRandomNumberInRange<int32>, "int", 0x4d9c10, int32(*)(int32, int32));
    RH_ScopedOverloadedInstall(GetRandomNumberInRange<float>, "float", 0x4d9c50, float(*)(float, float));
    RH_ScopedInstall(ResolveProbability, 0x4d9c80);
    RH_ScopedInstall(GetPiecewiseLinear, 0x4d9d90);
    RH_ScopedInstall(AudioLog10, 0x4d9e50);
    RH_ScopedInstall(ConvertFromBytesToMS, 0x4d9ef0);
    RH_ScopedInstall(ConvertFromMSToBytes, 0x4d9f40);
    RH_ScopedInstall(GetBankAndSoundFromScriptSlotAudioEvent, 0x4D9CC0);
    RH_ScopedInstall(FindVehicleOfPlayer, 0x4D9E10);

    RH_ScopedInstall(GetCurrentTimeInMS, 0x4d9e80);
    RH_ScopedInstall(StaticInitialise, 0x5b97f0);
}

// 0x4d9c80
bool CAEAudioUtility::ResolveProbability(float p) {
    return p >= 1.0f || CGeneral::RandomBool(100.0f * p);
}

// 0x4d9d90
float CAEAudioUtility::GetPiecewiseLinear(float x, int16 dataCount, float (*data)[2]) {
    if (x >= data[dataCount - 1][0])
        return data[dataCount - 1][1];

    if (x <= data[0][0])
        return data[0][1];

    int32 i = 0;
    for (; i < dataCount; i++) {
        if (data[i][0] >= x)
            break;
    }
    // Perform linear interpolation
    float t = (x - data[i - 1][0]) / (data[i][0] - data[i - 1][0]);
    return t * (data[i][1] - data[i - 1][1]) + data[i - 1][1];
}

// 0x4d9e50
float CAEAudioUtility::AudioLog10(float p) {
    return p >= 0.00001f
        ? std::log10f(p)
        : -5.0f;
}

// REFACTORED
// 0x4d9e80
uint64 CAEAudioUtility::GetCurrentTimeInMS() {
    using namespace std::chrono;
    const auto nowMs = time_point_cast<milliseconds>(high_resolution_clock::now());
    const auto value = duration_cast<milliseconds>(nowMs.time_since_epoch());
    return static_cast<uint64>(value.count()) - startTimeMs;

    //For some reason this doesn't work (original code):
    //LARGE_INTEGER counter;
    //QueryPerformanceCounter(&counter);
    //return counter.QuadPart / Frequency.QuadPart * 1000 - startTimeMs;
}

// 0x4d9ef0
uint32 CAEAudioUtility::ConvertFromBytesToMS(uint32 lengthInBytes, uint32 frequency, uint16 frequencyMult) {
    return static_cast<uint32>(std::floorf(lengthInBytes / (float(frequency * frequencyMult) / 500.0f)));
}

// 0x4d9f40
uint32 CAEAudioUtility::ConvertFromMSToBytes(uint32 a, uint32 frequency, uint16 frequencyMult) {
    const auto value = static_cast<uint32>(std::floorf(a * float(frequency * frequencyMult) / 500.0f));
    return value + value % (2 * frequencyMult);
}

// CHANGED?
// 0x5B97F0
void CAEAudioUtility::StaticInitialise() {
    constexpr size_t LOOKUP_SIZE = sizeof(m_sfLogLookup) / sizeof(float) / 2;

    m_sfLogLookup[0][0] = 0.0f;
    m_sfLogLookup[0][1] = -100.0f;
    m_sfLogLookup[1][0] = 0.00001f;

    for (size_t i = 2; i < LOOKUP_SIZE; i++) {
        float v = 10.0f / std::powf(1.2f, static_cast<float>(LOOKUP_SIZE - i - 1));
        m_sfLogLookup[i][0] = v;
        m_sfLogLookup[1][1] = log10f(v);
    }

    VERIFY(QueryPerformanceFrequency(&Frequency));
    startTimeMs = GetCurrentTimeInMS();
}

// 0x4D9CC0
bool CAEAudioUtility::GetBankAndSoundFromScriptSlotAudioEvent(const eAudioEvents& scriptID, eSoundBankS32& outBankID, int32& outSoundID, int32 slot) {
    if (scriptID < AE_SCRIPT_BANK_FIRST) {
        return false;
    }
    if (scriptID < AE_SCRIPT_SLOT_FIRST) {
        outBankID = gScriptBanksLookup[scriptID - AE_SCRIPT_BANK_FIRST];
        outSoundID = -1;
    } else if (scriptID == AE_SCRIPT_SLOT_USE_CUSTOM) {
        outBankID  = SND_BANK_SCRIPT_NULL;
        outSoundID = slot > 3
            ? 0
            : 2 * (slot % 2);
    } else {
        outSoundID = (scriptID - AE_SCRIPT_SLOT_FIRST) % 200;
        outBankID  = (eSoundBank)(SND_BANK_SCRIPT_FIRST + (scriptID - AE_SCRIPT_SLOT_FIRST) / 200u); // Each sound bank contains 200 sounds
    }
    return true;
}

// 0x4D9E10
CVehicle* CAEAudioUtility::FindVehicleOfPlayer() {
    auto* vehicle = FindPlayerVehicle(-1, true);
    if (vehicle)
        return vehicle;

    auto* player = FindPlayerPed();
    if (!player)
        return vehicle;

    auto* attach = player->m_pAttachedTo->AsAutomobile();
    if (attach && attach->IsVehicle())
        return attach;

    return vehicle;
}
