#include "StdInc.h"

/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

// 0x5A81E0
void CClothes::ConstructPedModel(uint32 modelid, CPedClothesDesc& newclothes, CPedClothesDesc const* oldclothes, bool bCutscenePlayer) {
    plugin::Call<0x5A81E0, uint32, CPedClothesDesc&, CPedClothesDesc const*, bool>(modelid, newclothes, oldclothes, bCutscenePlayer);
}

// 0x5A8120 
void CClothes::RequestMotionGroupAnims() {
    plugin::Call<0x5A8120>();
}

// 0x5A8390 
void CClothes::RebuildPlayerIfNeeded(CPlayerPed* player) {
    plugin::Call<0x5A8390, CPlayerPed*>(player);
}

// 0x5A82C0 
void CClothes::RebuildPlayer(CPlayerPed* player, bool bIgnoreFatAndMuscle) {
    plugin::Call<0x5A82C0, CPlayerPed*, bool>(player, bIgnoreFatAndMuscle);
}

// 0x5A8270 
void CClothes::RebuildCutscenePlayer(CPlayerPed* player, int32 modelid) {
    plugin::Call<0x5A8270, CPlayerPed*, int32>(player, modelid);
}

// 0x5A7B30 
void CClothes::LoadClothesFile() {
    plugin::Call<0x5A7B30>();
}

// 0x5A80D0 
void CClothes::Init() {
    plugin::Call<0x5A80D0>();
}

// 0x5A7EA0 
eClothesModelPart CClothes::GetTextureDependency(int32 eClothesTexturePart) {
    eClothesModelPart result;
    plugin::CallAndReturn<eClothesModelPart, 0x5A7EA0, eClothesModelPart*, int32>(&result, eClothesTexturePart);
    return result;
}

// 0x5A7FB0 
int32 CClothes::GetPlayerMotionGroupToLoad() {
    return plugin::CallAndReturn<int32, 0x5A7FB0>();
}

// 0x5A7F30 
eClothesTexturePart CClothes::GetDependentTexture(int32 eClothesModelPart) {
    eClothesTexturePart result;
    plugin::CallAndReturn<eClothesTexturePart, 0x5A7F30, eClothesTexturePart*, int32>(&result, eClothesModelPart);
    return result;
}

// 0x5A81B0
int32 CClothes::GetDefaultPlayerMotionGroup() {
    return plugin::CallAndReturn<int32, 0x5A81B0>();
}
