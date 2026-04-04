/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "ePedType.h"

class CPed;
class GangInfo;

class CGangs {
public:
    static inline auto& GangAttackWithCops = StaticRef<std::array<bool, 10>>(0xC091D2);
    static inline auto& Gang = StaticRef<std::array<CGangInfo, 10>>(0xC091F0);

public:
    static void InjectHooks();

    static void Initialise();

    static bool Load();
    static bool Save();
    static void SetGangWeapons(int16 gangId, eWeaponType weapId1, eWeaponType weapId2, eWeaponType weapId3);
    static eModelID ChooseGangPedModel(eGangID gangId);

    static bool GetWillAttackPlayerWithCops(ePedType pedType);
    static void SetWillAttackPlayerWithCops(ePedType pedType, bool bAttackPlayerWithCops);
    static void SetGangPedModelOverride(int16 gangId, int8 PedModelOverride);
};
