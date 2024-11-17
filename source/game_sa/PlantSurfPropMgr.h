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
    uint16 m_nModelID = uint16(-1);
    uint16 m_nTextureID = 0;
    CRGBA  m_rgbaColor{ 255, 255, 255, 255 };
    uint8  m_nIntensity = 255;
    uint8  m_nIntensityVar = 0;
    float  m_fScaleXY = 1.0f;
    float  m_fScaleZ = 1.0f;
    float  m_fScaleVarXY = 0.0f;
    float  m_fScaleVarZ  = 0.0f;
    float  m_fDensity = 0.0f;
    float  m_fWindBendScale = 0.0f;
    float  m_fWindBendVar = 0.0f;
};
VALIDATE_SIZE(CPlantSurfPropPlantData, 0x28);

struct CPlantSurfProp {
    uint16 m_nPlantSlotID;
    CPlantSurfPropPlantData  m_PlantData[3];
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
