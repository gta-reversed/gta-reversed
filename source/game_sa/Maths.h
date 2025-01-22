#pragma once

constexpr float RadToSinTableIndex = 256.0f / TWO_PI;

class CMaths {
public:
    static float (&ms_SinTable)[256];

    static void InjectHooks();
    static void InitMathsTables();

    // Originally named `SinTabel` with argument named `Arg` which sucks, so replaced with custom name.
    static float GetSinFast(float rad);

    // Originally named `CosTabel` with argument named `Arg`, same story as above
    static float GetCosFast(float rad);

};
