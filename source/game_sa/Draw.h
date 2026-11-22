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
    static inline auto& ms_fFOV = StaticRef<float>(0x8D5038); // 45.0
    static inline auto& ms_fLODDistance = StaticRef<float>(0xC3EF98);
    static inline auto& ms_fNearClipZ = StaticRef<float>(0xC3EFA0);
    static inline auto& ms_fFarClipZ = StaticRef<float>(0xC3EF9C);
    static inline auto& ms_fAspectRatio = StaticRef<float>(0xC3EFA4);

    static inline auto& FadeRed = StaticRef<uint8>(0xC3EFA8);
    static inline auto& FadeGreen = StaticRef<uint8>(0xC3EFA9);
    static inline auto& FadeBlue = StaticRef<uint8>(0xC3EFAA);
    static inline auto& FadeValue = StaticRef<uint8>(0xC3EFAB);

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
