#pragma once

class CVector;
class CRGBA;
struct RwTexture;

class PPTriPlant {
public:
    CVector V1;
    CVector V2;
    CVector V3;
    CVector Center;

    uint16 model_id;
    uint16 num_plants;

    CVector2D scale;
    RwTexture* texture;

    CRGBA color;
    uint8 intensity;
    uint8 intensity_var;

    float seed;
    float scale_var_xy;
    float scale_var_z;
    float wind_bend_scale;
    float wind_bend_var;
};
VALIDATE_SIZE(PPTriPlant, 0x5C);

constexpr auto MAX_PLANTS = 32u;

class CPPTriPlantBuffer {
public:
    int32      m_CurrentIndex;
    std::array<PPTriPlant, MAX_PLANTS> m_Buffer;
    int32      m_PlantModelsSet;
    std::array<RpAtomic**, 4> m_pPlantModelsTab;

public:
    static void InjectHooks();

    CPPTriPlantBuffer();
    ~CPPTriPlantBuffer() = default;

    void Flush();
    PPTriPlant* GetPPTriPlantPtr(int32 amountToAdd);
    void ChangeCurrentPlantModelsSet(int32 newSet);
    void IncreaseBufferIndex(int32 pipeMode, int32 amount);

    RpAtomic** GetPlantModelsTab(uint32 index);
    void SetPlantModelsTab(uint32 index, RpAtomic** pPlantModels);

    static RpMaterial* SetGrassMaterialCB(RpMaterial* material, void* data);

    int32 GetType() const    { return m_PlantModelsSet; }; // 0x5DAAF0
    void SetType(int32 type) { m_PlantModelsSet = type; }; // 0x5DAAE0

    // NOTSA
    void CopyToActive(PPTriPlant* plant) {
        memcpy(&m_Buffer[m_CurrentIndex], plant, sizeof(PPTriPlant));
    }
};

extern RwTexture*& RenderGrassTexture;
