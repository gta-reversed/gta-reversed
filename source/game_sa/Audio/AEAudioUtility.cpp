#include "StdInc.h"

#include <chrono>

#include "AEAudioUtility.h"

uint64& CAEAudioUtility::startTimeMs = *reinterpret_cast<uint64*>(0xb610f8);
float (&CAEAudioUtility::m_sfLogLookup)[50][2] = *reinterpret_cast<float (*)[50][2]>(0xb61100);

// NOTE: Not sure about the size.
static inline auto& gScriptBanksLookup = *reinterpret_cast<std::array<int32, 628>*>(0x8ABC70);

void CAEAudioUtility::InjectHooks() {
    RH_ScopedClass(CAEAudioUtility);
    RH_ScopedCategory("Audio");

    RH_ScopedInstall(GetRandomNumberInRange<int32>, 0x4d9c10);
    RH_ScopedInstall(GetRandomNumberInRange<float>, 0x4d9c50);
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
    return 0.00001f <= p ? std::log10f(p) : -5.0f;
}

// REFACTORED
// 0x4d9e80
uint64 CAEAudioUtility::GetCurrentTimeInMS() {
    using namespace std::chrono;
    auto nowMs = time_point_cast<milliseconds>(high_resolution_clock::now());
    auto value = duration_cast<milliseconds>(nowMs.time_since_epoch());
    return static_cast<uint64>(value.count());
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

    startTimeMs = GetCurrentTimeInMS();
}

// 0x4D9CC0
bool CAEAudioUtility::GetBankAndSoundFromScriptSlotAudioEvent(int32& slot, int32& outBank, int32& outSound, int32 a4) {
    if (slot < 1800)
        return false;

    if (slot < 2000) {
        outBank = gScriptBanksLookup[slot - 1800];
        return true;
    }

    if (slot == 0xFFFF) { // (uint16)-1
        outBank = 291;

        if (a4 > 3)
            outSound = 0;
        else
            outSound = 2 * (a4 % 2);

        return true;
    }

    outBank = static_cast<int32>(std::floor(float(slot - 2000) / 200.0f)) + 147;
    outSound = (slot - 2000) % 200;
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
