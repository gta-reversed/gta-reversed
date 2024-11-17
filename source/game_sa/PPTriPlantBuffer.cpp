#include "StdInc.h"

#include "PPTriPlantBuffer.h"
#include "GrassRenderer.h"

RwTexture*& RenderGrassTexture = *(RwTexture**)0xC02DC0;

void CPPTriPlantBuffer::InjectHooks() {
    RH_ScopedClass(CPPTriPlantBuffer);
    RH_ScopedCategory("Plant");

    RH_ScopedInstall(Flush, 0x5DB0C0);
    RH_ScopedInstall(GetPPTriPlantPtr, 0x5DB140);
    RH_ScopedInstall(ChangeCurrentPlantModelsSet, 0x5DB170);
    RH_ScopedInstall(IncreaseBufferIndex, 0x5DB1A0);
    RH_ScopedInstall(GetPlantModelsTab, 0x5DACA0);
    RH_ScopedInstall(SetPlantModelsTab, 0x5DAC80);
    RH_ScopedInstall(SetGrassMaterialCB, 0x5DAC10);
}

// 0x5DB090
CPPTriPlantBuffer::CPPTriPlantBuffer() {
    m_CurrentIndex = 0;
    m_PlantModelsSet = 0;
    std::ranges::fill(m_pPlantModelsTab, nullptr);
}

// 0x5DB0C0
void CPPTriPlantBuffer::Flush() {
    if (m_CurrentIndex <= 0)
        return;

    // FIX_BUGS: m_pPlantModelsTab[m_PlantModelsSet] can be null. With std::array we check it.
    // While DrawTriPlants does not check for null.
    auto random = CGeneral::GetRandomNumber();
    CGrassRenderer::DrawTriPlants(m_Buffer, m_CurrentIndex, GetPlantModelsTab(m_PlantModelsSet));
    m_CurrentIndex = 0;
    srand(random);
}

// 0x5DB140
PPTriPlant* CPPTriPlantBuffer::GetPPTriPlantPtr(int32 amountToAdd) {
    if (m_CurrentIndex + amountToAdd > MAX_PLANTS)
        Flush();

    return &m_Buffer[m_CurrentIndex];
}

// 0x5DB170
void CPPTriPlantBuffer::ChangeCurrentPlantModelsSet(int32 newSet) {
    if (m_PlantModelsSet == newSet)
        return;

    Flush();
    m_PlantModelsSet = newSet;
}

// 0x5DB1A0
void CPPTriPlantBuffer::IncreaseBufferIndex(int32 pipeMode, int32 amount) {
    if (m_PlantModelsSet == pipeMode) {
        m_CurrentIndex += amount;
        if (m_CurrentIndex >= MAX_PLANTS) {
            Flush();
        }
    }
}

// 0x5DACA0
RpAtomic** CPPTriPlantBuffer::GetPlantModelsTab(uint32 index) {
    return index < std::size(m_pPlantModelsTab) ? m_pPlantModelsTab[index] : nullptr;
}

// 0x5DAC80
void CPPTriPlantBuffer::SetPlantModelsTab(uint32 index, RpAtomic** pPlantModels) {
    if (index < std::size(m_pPlantModelsTab)) {
        m_pPlantModelsTab[index] = pPlantModels;
    }
}

// 'data' is a pointer to RwRGBA
// 0x5DAC10
RpMaterial* CPPTriPlantBuffer::SetGrassMaterialCB(RpMaterial* material, void* data) {
    const auto color = *reinterpret_cast<RwRGBA*>(data);
    RpMaterialSetColor(material, &color);
    if (RpMaterialGetTexture(material) != RenderGrassTexture) {
        RpMaterialSetTexture(material, RenderGrassTexture);
    }
    return material;
}
