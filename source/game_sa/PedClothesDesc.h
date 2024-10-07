/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "eClothesModelPart.h"
#include "eClothesTexturePart.h"

class CPedClothesDesc {
public:
    std::array<uint32, CLOTHES_MODEL_TOTAL> m_anModelKeys;
    std::array<uint32, CLOTHES_TEXTURE_TOTAL> m_anTextureKeys;
    float  m_fFatStat;
    float  m_fMuscleStat;

public:
    static void InjectHooks();

    CPedClothesDesc();
    CPedClothesDesc* Constructor();

    void Initialise();

    void SetModel(uint32 modelId, eClothesModelPart modelPart);
    void SetModel(const char* model, eClothesModelPart modelPart);
    bool GetIsWearingBalaclava();
    bool HasVisibleNewHairCut(int32 arg1);
    bool HasVisibleTattoo();
    void SetTextureAndModel(uint32 texture, uint32 model, eClothesTexturePart texturePart);
    void SetTextureAndModel(const char* textureName, const char* modelName, eClothesTexturePart texturePart);
};

VALIDATE_SIZE(CPedClothesDesc, 0x78);
