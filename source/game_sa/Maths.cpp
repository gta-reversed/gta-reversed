#include "StdInc.h"

#include "Maths.h"

// We're going to generate the lookup table here instead of using the one from the game
// Original table is located at 0xBB3E00 (NOT `0xBB3DFC`) and is of size 256 (so don't change the size below)
constexpr size_t SIN_LUT_SIZE = 256;
constexpr float  SIN_LUT_STEP = TWO_PI / (float)(SIN_LUT_SIZE);
const auto SIN_LUT = StaticRef<std::array<float, SIN_LUT_SIZE>>(0xBB3E00) = []() {
    std::array<float, SIN_LUT_SIZE> lut{};
    for (size_t i = 0; i < SIN_LUT_SIZE; ++i) {
        lut[i] = std::sin((float)(i) * SIN_LUT_STEP);
    }
    return lut;
}();

float CMaths::GetSinFast(float rad) {
    return SIN_LUT[(uint32)(rad / SIN_LUT_STEP) % SIN_LUT_SIZE];
}

float CMaths::GetCosFast(float rad) {
    return GetSinFast(rad + PI / 2.f); // cos(x) == sin(x + PI/2)
}

void CMaths::InitMathsTables() {
    ZoneScoped;

    /* No-op because the LUT is already initialized statically (originally it was initialized here) */
}

void CMaths::InjectHooks() {
    RH_ScopedClass(CMaths);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(InitMathsTables, 0x59AC90);
    RH_ScopedInstall(GetSinFast, 0x4A1340); // No callers in the game, inlined in every single instance
    RH_ScopedInstall(GetCosFast, 0x4A1360); // No callers in the game, inlined in every single instance
}
