/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

static inline auto& JustLoadedDontFadeInYet = StaticRef<bool>(0xC16EDC);
static inline auto& StillToFadeOut = StaticRef<bool>(0xC16EDD);

class CDraw {
public:
    static float& ms_fFOV; // default 45.0
    static float& ms_fLODDistance;
    static float& ms_fNearClipZ;
    static float& ms_fFarClipZ;
    static float& ms_fAspectRatio;

    static uint8& FadeRed;
    static uint8& FadeGreen;
    static uint8& FadeBlue;
    static uint8& FadeValue;

public:
    static void InjectHooks();

    static void SetFOV(float fov);
    static float GetFOV() { return ms_fFOV; }

    static void SetNearClipZ(float nearClip) { ms_fNearClipZ = nearClip; }
    static float GetNearClipZ() { return ms_fNearClipZ; }

    static void SetFarClipZ(float farClip) { ms_fFarClipZ = farClip; }
    static float GetFarClipZ() { return ms_fFarClipZ; }

    static float GetAspectRatio() { return ms_fAspectRatio; }
    static void SetAspectRatio(float ratio) { ms_fAspectRatio = ratio; }

    static void CalculateAspectRatio();

    // @notsa
    static bool IsFading() { return FadeValue != 0u; }
};

extern void DoFade();
