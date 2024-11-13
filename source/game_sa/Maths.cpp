#include "StdInc.h"

#include "Maths.h"

float (&CMaths::ms_SinTable)[256] = *(float(*)[256])0xBB3DFC;

void CMaths::InjectHooks()
{
    RH_ScopedClass(CMaths);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(InitMathsTables, 0x59AC90);
    RH_ScopedInstall(GetSinFast, 0x4A1340); // No callers in the game, inlined in every single instance
    RH_ScopedInstall(GetCosFast, 0x4A1360); // No callers in the game, inlined in every single instance
}

void CMaths::InitMathsTables() {
    ZoneScoped;

    for (int32 i = 0; i < 256; ++i)
        ms_SinTable[i] = sin(static_cast<float>(i) * PI / 128.0F);
}

float CMaths::GetSinFast(float rad) {
    return ms_SinTable[static_cast<uint8>(rad * RadToSinTableIndex) + 1];
}

float CMaths::GetCosFast(float rad) {
    // Table has 256 elements, and spans [0:2PI), 64 index move equals PI/2 move on the X axis, and cos(x) == sin(x + PI/2)
    return ms_SinTable[static_cast<uint8>(rad * RadToSinTableIndex + 64.f) + 1];
}
