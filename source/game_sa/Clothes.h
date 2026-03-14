/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AnimationEnums.h"
#include "eClothesModelPart.h"
#include "eClothesTexturePart.h"

class CPedClothesDesc;
class CPlayerPed;

class CClothes {
public:
    static inline auto& ms_clothesImageId = StaticRef<int32>(0xBC12F8);
    static inline auto& ms_numRuleTags = StaticRef<uint32>(0xBC12FC);
    static inline auto& ms_clothesRules = StaticRef<uint32[600]>(0xBC1300);

public:
    static void InjectHooks();

    static void Init();
    static void LoadClothesFile();

    static void ConstructPedModel(uint32 modelId, CPedClothesDesc& newClothes, const CPedClothesDesc* oldClothes, bool bCutscenePlayer);
    static void RequestMotionGroupAnims();
    static void RebuildPlayerIfNeeded(CPlayerPed* player);
    static void RebuildPlayer(CPlayerPed* player, bool bIgnoreFatAndMuscle);
    static void RebuildCutscenePlayer(CPlayerPed* player, int32 modelId);
    static eClothesModelPart GetTextureDependency(eClothesTexturePart eClothesTexturePart);
    static eClothesTexturePart GetDependentTexture(eClothesModelPart eClothesModelPart);
    static AssocGroupId GetPlayerMotionGroupToLoad();
    static AssocGroupId GetDefaultPlayerMotionGroup();

    // NOTSA
    static void AddRule(uint32 rule) {
        ms_clothesRules[ms_numRuleTags] = rule;
        ms_numRuleTags += 1;
    }
};
