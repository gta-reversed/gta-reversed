#include "StdInc.h"

#include "Maths.h"

constexpr size_t LUT_SIZE = 256;
auto& ms_SinTable = StaticRef<std::array<float, LUT_SIZE>>(0xBB4200); // This is the correct address, not `0xBB3DFC`

constexpr auto RadToSinTableIndex(float rad) {
    assert(rad >= 0.f && rad <= 2.f * PI);
    return (uint32)(rad * ((float)(LUT_SIZE) / TWO_PI)) % LUT_SIZE;
}

float CMaths::GetSinFast(float rad) {
    return ms_SinTable[RadToSinTableIndex(rad)];
}

float CMaths::GetCosFast(float rad) {
    return ms_SinTable[RadToSinTableIndex(rad + (PI / 2.f))]; // cos(x) == sin(x + PI/2)
}

void CMaths::InitMathsTables() {
    ZoneScoped;

    for (size_t i = 0; i < LUT_SIZE; ++i) {
        ms_SinTable[i] = std::sin((float)(i) * ((2.f * PI) / LUT_SIZE));
    }
}

void CMaths::InjectHooks() {
    RH_ScopedClass(CMaths);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(InitMathsTables, 0x59AC90);
    RH_ScopedInstall(GetSinFast, 0x4A1340); // No callers in the game, inlined in every single instance
    RH_ScopedInstall(GetCosFast, 0x4A1360); // No callers in the game, inlined in every single instance
}
