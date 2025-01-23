#pragma once
#include <common.h>

class CMaths {
public:
    static void InjectHooks();
    static void InitMathsTables();

    // Originally named `SinTabel` with argument named `Arg` which sucks, so replaced with custom name.
    static float GetSinFast(float rad);

    // Originally named `CosTabel` with argument named `Arg`, same story as above
    static float GetCosFast(float rad);
};
