#pragma once

constexpr auto MAX_SURFACE_PTR_PROPERTIES = 178u;
constexpr auto MAX_SURFACE_PROPERTIES = 57u;

enum class ePlantField {
    NAME,                   // Name
    PCD_ID,                 // PCDid
    SLOT_ID,                // SlotID
    MODEL_ID,               // ModelID
    UV_OFFSET,              // UVoff
    COLOR_R,                // R
    COLOR_G,                // G
    COLOR_B,                // B
    INTENSITY,              // I
    INTENSITY_VARIATION,    // VarI
    COLOR_ALPHA,            // A
    SCALE_XY,               // SclXY
    SCALE_Z,                // SclZ
    SCALE_VARIATION_XY,     // SclVarXY
    SCALE_VARIATION_Z,      // SclVarZ
    WIND_BENDING_SCALE,     // WBendScl
    WIND_BENDING_VARIATION, // WBendVar
    DENSITY,                // Density

    ALL_FIELDS_READED
};

struct CPlantSurfPropPlantData {
    uint16 model_id = uint16(-1);
    uint16 uv_offset = 0;
    CRGBA  color{ 255, 255, 255, 255 };
    uint8  intensity = 255;
    uint8  intensity_variation = 0;
    float  scale_xy = 1.0f;
    float  scale_z = 1.0f;
    float  scale_variation_xy = 0.0f;
    float  scale_variation_z  = 0.0f;
    float  density = 0.0f;
    float  wind_blending_scale = 0.0f;
    float  wind_blending_variation = 0.0f;
};
VALIDATE_SIZE(CPlantSurfPropPlantData, 0x28);

struct CPlantSurfProp {
    uint16 m_SlotId;
    CPlantSurfPropPlantData  m_Plants[3];
};

class CPlantSurfPropMgr {
public:
    static inline std::array<CPlantSurfProp*, 178>& m_SurfPropPtrTab = *(std::array<CPlantSurfProp*, 178>*)0xC38070;
    static inline uint32& m_countSurfPropsAllocated = *(uint32*)0xC39ED4;
    static inline CPlantSurfProp (&m_SurfPropTab)[MAX_SURFACE_PROPERTIES] = *(CPlantSurfProp(*)[57])0xC38338;

public:
    static void InjectHooks();

    static bool Initialise();
    static void Shutdown();

    static bool LoadPlantsDat(const char* filename);
    static CPlantSurfProp* AllocSurfProperties(uint16 surfaceId, bool clearAllocCount);
    static CPlantSurfProp* GetSurfProperties(uint16 index);
};
