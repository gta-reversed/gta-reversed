/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CCredits {
public:
    static inline auto& CreditsStartTime = StaticRef<uint32>(0xC6E978);
    static inline auto& bCreditsGoing = StaticRef<bool>(0xC6E97C);

public:
    static void InjectHooks();

    static void Start();
    static void Stop();

    static void Render();
    static void PrintCreditText(float scaleX, float scaleY, const GxtChar* text, uint32& position, float currentOffset, bool highlighted);
    static inline void PrintCreditSpace(float spaceSize, uint32& position);
    static void RenderCredits();
};
