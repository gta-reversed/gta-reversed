/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CIniFile {
public:
    static inline auto& PedNumberMultiplier = StaticRef<float>(0x8CDF14);
    static inline auto& CarNumberMultiplier = StaticRef<float>(0x8CDF18);

public:
    static void InjectHooks();

    static void LoadIniFile();
};
