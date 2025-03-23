#include "StdInc.h"

#include "PedClothesDesc.h"

void CPedClothesDesc::InjectHooks() {
    RH_ScopedClass(CPedClothesDesc);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x5A8020);
    RH_ScopedInstall(Initialise, 0x5A78F0);
    RH_ScopedInstall(GetIsWearingBalaclava, 0x5A7950);
    RH_ScopedInstall(HasVisibleNewHairCut, 0x5A7970, { .reversed = false });
    RH_ScopedInstall(HasVisibleTattoo, 0x5A79D0, { .reversed = false });
}

CPedClothesDesc::CPedClothesDesc() {
    Initialise();
}

CPedClothesDesc* CPedClothesDesc::Constructor() {
    this->CPedClothesDesc::CPedClothesDesc();
    return this;
}

// 0x5A78F0
void CPedClothesDesc::Initialise() {
    std::ranges::fill(m_anModelKeys, 0);
    std::ranges::fill(m_anTextureKeys, 0);
    m_fFatStat = 0.0f;
    m_fMuscleStat = 0.0f;
}

// 0x5A7910
void CPedClothesDesc::SetModel(uint32 modelId, eClothesModelPart modelPart) {
    m_anModelKeys[modelPart] = modelId;
}

// 0x5A7920
void CPedClothesDesc::SetModel(const char* model, eClothesModelPart modelPart) {
    if (model)
        m_anModelKeys[modelPart] = CKeyGen::GetUppercaseKey(model);
    else
        m_anModelKeys[modelPart] = 0;
}

// 0x5A7950
bool CPedClothesDesc::GetIsWearingBalaclava() {
    return IsWearingModel("balaclava", CLOTHES_MODEL_SPECIAL);
}

// 0x5A7970
bool CPedClothesDesc::HasVisibleNewHairCut(int32 arg1) {
    return plugin::CallMethodAndReturn<bool, 0x5A7970, CPedClothesDesc*, int32>(this, arg1);
}

// 0x5A79D0
bool CPedClothesDesc::HasVisibleTattoo() {
    return plugin::CallMethodAndReturn<bool, 0x5A79D0, CPedClothesDesc*>(this);
}

// 0x5A8050
void CPedClothesDesc::SetTextureAndModel(uint32 texture, uint32 model, eClothesTexturePart texturePart) {
    plugin::CallMethod<0x5A8050, CPedClothesDesc*, uint32, uint32, eClothesTexturePart>(this, texture, model, texturePart);
}

// 0x5A8080
void CPedClothesDesc::SetTextureAndModel(const char* textureName, const char* modelName, eClothesTexturePart texturePart) {
    plugin::CallMethod<0x5A8080, CPedClothesDesc*, const char*, const char*, eClothesTexturePart>(this, textureName, modelName, texturePart);
}

