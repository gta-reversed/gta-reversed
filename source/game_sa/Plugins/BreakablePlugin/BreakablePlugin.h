#pragma once

/**
 * Breakable plugin unique rwID
 */
#define rwID_BREAKABLEPLUGIN MAKECHUNKID(rwVENDORID_DEVELOPER, 0xFD)

/*
 * Breakable plugin static offset
 */
static inline auto& g_BreakablePlugin = StaticRef<int32>(0xBB4238);

enum eBreakablePluginPositionRule : uint32 {
    OBJECT_ORIGIN    = 0,
    COLLISION_ORIGIN = 1,
};

struct BreakInfoTriangle {
    std::array<uint16, 3> vertIndex;
};

struct BreakInfoColor {
    float red;
    float green;
    float blue;
};
VALIDATE_SIZE(BreakInfoColor, 0xC);

// RW PLUGIN
struct BreakInfo_t {
    eBreakablePluginPositionRule m_uiPosRule;
    uint16                       m_usNumVertices;
    struct RwV3d*                m_pVertexPos;
    struct RwTexCoords*          m_pTexCoors;
    struct RwRGBA*               m_pVertexColors;

    uint16                       m_usNumTriangles;
    struct BreakInfoTriangle*    m_pTriangles;
    uint16*                      m_pTrianglesMaterialIndices;

    uint16                       m_usNumMaterials;
    struct RwTexture**           m_pTextures;
    char*                        m_pTextureNames;
    char*                        m_pMaskNames;
    struct BreakInfoColor*       m_pMaterialProperties;
};
VALIDATE_SIZE(BreakInfo_t, 0x34);

struct BreakablePlugin {
    union {
        uint32       m_SectionHeader;
        BreakInfo_t* m_pBreakableInfo;
    };

    static void InjectHooks();

    auto GetTextures() { return std::span { m_pBreakableInfo->m_pTextures, m_pBreakableInfo->m_usNumMaterials }; }
};
VALIDATE_SIZE(BreakablePlugin, 0x4);

struct BreakablePluginData {
    BreakInfo_t m_Info;

    // Dynamically allocated size, depending on loaded model sizes]
    // RwV3d               m_aVertexPos[m_usNumVertices];
    // RwTexCoords         m_aTexCoors[m_usNumVertices];
    // RwRGBA              m_aVertexColors[m_usNumVertices];
    // BreakInfoTriangle   m_aTriangleVertIndices[m_usNumTriangles];
    // uint16              m_aTriangleMaterialIndices[m_usNumTriangles];
    // char                m_aTextureNames[m_usNumMaterials][32];
    // char                m_aMaskNames[m_usNumMaterials][32];
    // BreakInfoColor      m_aMaterialProperties[m_usNumMaterial];
    // RwTexture*          m_aTexturePointers[m_usNumMaterial];

// NOTSA: Helper functions
    static size_t GetRequiredAllocationSize(const BreakInfo_t& baseInfo) {
        auto size = sizeof(BreakInfo_t);
        size += sizeof(RwV3d) * baseInfo.m_usNumVertices;
        size += sizeof(RwTexCoords) * baseInfo.m_usNumVertices;
        size += sizeof(RwRGBA) * baseInfo.m_usNumVertices;
        size += sizeof(BreakInfoTriangle) * baseInfo.m_usNumTriangles;
        size += sizeof(uint16) * baseInfo.m_usNumTriangles;
        size += sizeof(char[32]) * baseInfo.m_usNumMaterials;
        size += sizeof(char[32]) * baseInfo.m_usNumMaterials;
        size += sizeof(BreakInfoColor) * baseInfo.m_usNumMaterials;
        size += sizeof(RwTexture*) * baseInfo.m_usNumMaterials;
        return size;
    };

    auto GetVertexPosOffset()       { return sizeof(BreakInfo_t); };
    auto GetTexCoordsOffset()       { return GetVertexPosOffset()       + sizeof(RwV3d)             * m_Info.m_usNumVertices; };
    auto GetVertColorsOffset()      { return GetTexCoordsOffset()       + sizeof(RwTexCoords)       * m_Info.m_usNumVertices; };
    auto GetTrianglesOffset()       { return GetVertColorsOffset()      + sizeof(RwRGBA)            * m_Info.m_usNumVertices; };
    auto GetMaterialIndicesOffset() { return GetTrianglesOffset()       + sizeof(BreakInfoTriangle) * m_Info.m_usNumTriangles; };
    auto GetTextureNamesOffset()    { return GetMaterialIndicesOffset() + sizeof(uint16)            * m_Info.m_usNumTriangles; };
    auto GetMaskNamesOffset()       { return GetTextureNamesOffset()    + sizeof(char[32])          * m_Info.m_usNumMaterials; };
    auto GetSurfacePropsOffset()    { return GetMaskNamesOffset()       + sizeof(char[32])          * m_Info.m_usNumMaterials; };
    auto GetTexturesOffset()        { return GetSurfacePropsOffset()    + sizeof(BreakInfoColor)    * m_Info.m_usNumMaterials; };

    template <typename T>
    auto As(auto byteOffset) {
        return reinterpret_cast<T>(&reinterpret_cast<uint8*>(this)[byteOffset]);
    }

    auto GetVertexPosPtr()       { return As<RwV3d*>(GetVertexPosOffset()); }
    auto GetTexCoordsPtr()       { return As<RwTexCoords*>(GetTexCoordsOffset()); }
    auto GetVertColorsPtr()      { return As<RwRGBA*>(GetVertColorsOffset()); }
    auto GetTrianglesPtr()       { return As<BreakInfoTriangle*>(GetTrianglesOffset()); }
    auto GetMaterialIndicesPtr() { return As<uint16*>(GetMaterialIndicesOffset()); }
    auto GetTextureNamesPtr()    { return As<char*>(GetTextureNamesOffset()); }
    auto GetMaskNamesPtr()       { return As<char*>(GetMaskNamesOffset()); }
    auto GetSurfacePropsPtr()    { return As<BreakInfoColor*>(GetSurfacePropsOffset()); }
    auto GetTexturesPtr()        { return As<RwTexture**>(GetTexturesOffset()); }
};

#define BREAKABLEPLG(geometry, var) \
    (RWPLUGINOFFSET(BreakablePlugin, geometry, g_BreakablePlugin)->var)

#define BREAKABLECONSTPLG(geometry, var) \
    (RWPLUGINOFFSETCONST(BreakablePlugin, geometry, g_BreakablePlugin)->var)

bool BreakablePluginAttach();

void* BreakableConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
void* BreakableDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);

RwStream* BreakableStreamRead(RwStream* stream, int binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
RwStream* BreakableStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
RwInt32 BreakableStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
