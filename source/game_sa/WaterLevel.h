#pragma once

#include "Base.h"
#include "Vector.h"
#include "RGBA.h"
#include "Vector.h"
#include "Vector2D.h"

#include <extensions/utility.hpp>

struct CRenPar {
    float z{};                        // Z pos of this thing. x, y can be found in the containing vertex, see `CWaterVertex`.
    float bigWaves{}, smallWaves{};   // Height of waves
    int8  flowX{}, flowY{};           // Fixed-point float. Divide by 64
};

// 0x6E5280
static CRenPar lerp(CRenPar rp1, CRenPar rp2, float t) {
    return {
        lerp(rp1.z, rp2.z, t),
        lerp(rp1.bigWaves, rp2.bigWaves, t),
        lerp(rp1.smallWaves, rp2.smallWaves, t),
        0, 0
    };
}

struct CWaterVertex {
    int16  x;
    int16  y;
    CRenPar rp;
};

template<size_t NumVerts>
struct CWaterPolygon { // NOTSA - I hate duplicate code
    CWaterPolygon(bool isInvisible, bool isLimitedDepth, auto vertIdxs) :
        bInvisible{isInvisible},
        bLimitedDepth{isLimitedDepth}
    {
        rng::copy(vertIdxs, verts);
    }

    uint16 verts[NumVerts];

    bool bToBeRendered : 1{};
    bool bInvisible : 1{};
    bool bLimitedDepth : 1{};

    auto GetVertex(uint16 idx) const -> CWaterVertex;
    auto IsInInterior()        const { return GetVertex(0).rp.z > 950.f; }

    auto DoMarkToBeRendered(bool isInInterior) {
        if (!bInvisible && isInInterior == IsInInterior()) {
            bToBeRendered = true;
        }
    }
};
using CWaterTriangle = CWaterPolygon<3>;
VALIDATE_SIZE(CWaterTriangle, 0x8);

using CWaterQuad = CWaterPolygon<4>;
VALIDATE_SIZE(CWaterQuad, 0xA);

class CWaterLevel {
 public:
    /* Missing (in no particular order):
    m_ElementsOnQuadsAndTrianglesList
    m_NumBlocksOutsideWorldToBeRendered
    m_QuadsAndTrianglesInEachBlock
    m_WaterFogCol
    m_WaterFogDensity
    m_WaterFogHourOfDayEnd
    m_WaterFogHourOfDayStart
    m_WaterFogInsideCol
    m_fWaterFogHeight
    m_fWaterFogInsideFadeSpeed
    m_nNumOfWaterQuads
    m_nNumOfWaterTriangles
    */
    static inline uint32& m_nWaterConfiguration = *(uint32*)0xC228A0;
    static inline uint32& m_nWaterTimeOffset = *(uint32*)0xC228A4;
    static inline bool& m_bWaterFog = *(bool*)0x8D37D4;
    static inline bool& m_bWaterFogScript = *(bool*)0x8D37D5;
    static inline int32& m_WaterFogDensity = *(int32*)0x8D37E0;

    static inline float (&faWaveMultipliersX)[8] = *(float(*)[8])0x8D38C8;
    static inline float (&faWaveMultipliersY)[8] = *(float(*)[8])0x8D38E8;
    static inline float (&ms_WakePointAlphaMults)[4] = *(float(*)[4])0x8D390C;

    static inline RwRaster*&   waterclear256Raster = *(RwRaster**)0xC228A8;
    static inline RwTexture*&  texWaterclear256 = *(RwTexture**)0xC228AC;

    static inline RwRaster*&   seabd32Raster = *(RwRaster**)0xC228B0;
    static inline RwTexture*&  texSeabd32 = *(RwTexture**)0xC228B4;

    static inline RwRaster*&   waterwakeRaster = *(RwRaster**)0xC228B8;
    static inline RwTexture*&  texWaterwake = *(RwTexture**)0xC228BC;

    static inline uint32& NumWaterTriangles = *(uint32*)0xC22884;
    static inline uint32& NumWaterQuads = *(uint32*)0xC22888;
    static inline uint32& NumWaterVertices = *(uint32*)0xC2288C;
    static inline uint32& NumWaterZonePolys = *(uint32*)0xC215F0;

    static inline auto& m_aVertices             = *(std::array<CWaterVertex, 1021>*)0xC22910;

    static inline int32& CameraRangeMaxY = *(int32*)0xC1F950;
    static inline int32& CameraRangeMinY = *(int32*)0xC1F954;

    static inline int32& CameraRangeMaxX = *(int32*)0xC1F958;
    static inline int32& CameraRangeMinX = *(int32*)0xC1F95C;

    static inline auto& DETAILEDWATERDIST = *(int32*)0x8D37D0; // Default: 48
    static inline auto& bSplitBigPolys = *(bool*)0x8D37F4;     // Default: true
    static inline auto  BigPolySize = 168; // NOTSA variable, but value is OG.

    static inline auto& TextureShiftFirstV = *(float*)0xC21178;
    static inline auto& TextureShiftFirstU = *(float*)0xC2117C;

    static inline auto& TextureShiftSecondV = *(float*)0xC21180;
    static inline auto& TextureShiftSecondU = *(float*)0xC21184;

    struct WaterFog {
        std::array<int16, 70> minX;
        std::array<int16, 70> minY;
        std::array<int16, 70> maxX;
        std::array<int16, 70> maxY;
        std::array<float, 70> z;
    };
    static inline auto& ms_WaterFog              = *(WaterFog*)0xC21188;
    static inline auto& gWaterFogIndex           = *(uint32*)0xC228E4;
    static inline auto& m_fWaterFogHeight        = *(float*)0x8D37E4;
    static inline auto& gbPlayerIsInsideWaterFog = *(bool*)0xC228E8;


    // In reality the alpha component isn't used and instead `WaterLayerAlpha` is used
    static inline auto& WaterColorTriangle = *(CRGBA*)0xC21168;
    static inline auto& WaterColor         = *(CRGBA*)0xC2116C;

    static inline auto& WaterLayerAlpha = *(std::array<uint32, 2>*)0x8D3808;

    static inline auto& m_CurrentFlow        = *(CVector2D*)0xC22890;
    static inline auto& m_CurrentDesiredFlow = *(CVector2D*)0xC22898;

    static constexpr int32 WATER_BLOCK_SIZE                       = 500;
    static constexpr int32 NUM_WATER_BLOCKS_ROWCOL                = 6000 / WATER_BLOCK_SIZE; // 6000 => map size
    static inline    int32  m_MaxNumBlocksOutsideWorldToBeRendered = 70; // NOTSA: We just want a variable for the debug tool. Value may never be higher than the below array's size.
    static inline    auto&  m_NumBlocksOutsideWorldToBeRendered    = *(uint32*)0xC215EC;
    static inline    auto&  m_BlocksToBeRenderedOutsideWorldX      = *(std::array<int16, 70>*)0xC21560;
    static inline    auto&  m_BlocksToBeRenderedOutsideWorldY      = *(std::array<int16, 70>*)0xC214D0;

    // NOTSA: Color of wake segment parts (So we can make rainbows)
    static inline struct {
        float r{ 1.f }, g{ 1.f }, b{ 1.f };
    } WakeSegmentPartColors[4]{};

    //! Used to describe a block/combo's content
    struct PolyInfo {
    public:
        enum class PType {
            NONE,        //< Nothingness
            SINGLE_QUAD, //< A single quad (ID => Index into `WaterQuads`)
            SINGLE_TRI,  //< A single tri (ID => Index into `WaterTriangles`)
            COMBO        //< A combination of both (ID => Index of first entry in `m_PolyCombos`)
        };

    public:
        auto Id()   const { return m_id; }
        auto Type() const { return (PType)(m_type); }

    private:
        uint16 m_id   : 14{};
        uint16 m_type : 2{};  //< See PType
    };

    //! A block might have a simple shape (tri/quad) or a combo (That is just a combination of quads and triangles)
    //! AKA `m_QuadsAndTrianglesInEachBlock`
    static inline auto& m_BlockPolyInfo = *(notsa::mdarray<PolyInfo, NUM_WATER_BLOCKS_ROWCOL, NUM_WATER_BLOCKS_ROWCOL>*)0xC21B70;

    //! Triangles used for blocks (Or poly combos)
    static inline auto& WaterTriangles = *(std::array<CWaterTriangle, 6>*)0xC22854;

    //! Quads used for blocks (Or poly combos)
    static inline auto& WaterQuads = *(std::array<CWaterQuad, 301>*)0xC21C90;

    //! Contains sequences of poly's, each seq. is used for a block. The end of a sequence is indicated by a `PolyInfo` with it's type set to `NONE`.
    //! AKA `m_QuadsAndTrianglesList`
    static inline auto& m_PolyCombos = *(std::array<PolyInfo, 700>*)0xC215F8;

    // NOTSA Section - Used for debugging //
    
    //! Stop `SetCameraRange()` from running
    static inline bool DontUpdateCameraRange = false;

    static inline struct DebugWaterColor {
        enum {
            TRI,
            RECT
        };

        bool  active{};
        CRGBA color{};
    } DebugWaterColors[2];

    static inline bool DontRenderYSplitTri = false;

public:
    static void InjectHooks();

    static void Shutdown();
    static void AddWaveToResult(float x, float y, float* pfWaterLevel, float fUnkn1, float fUnkn2, CVector* pVecNormal);
    static void RenderWaterTriangle(int32 a1, int32 a2, CRenPar a3, int32 a4, int32 a5, CRenPar a6, int32 a7, int32 a8, CRenPar a9);

    // NOTSA
    struct WaterLayerTexInfo {
        CVector2D shift;
        float     size;
    };
    /*!
    * @addr notsa
    * @brief Get texture info for a given layer
    */
    static auto GetWaterLayerTexInfo(int32 WaterLayer) -> WaterLayerTexInfo;

    // NOTSA
    struct TexUV {
        float     size;      // Texture size
        CVector2D pos;       // Pos of the 0th vertex (top left) on the texture 
        CVector2D baseShift; // Shift from the 0th vertex
    };
    /*!
    * @addr notsa
    * @brief Calculate texture UV mapping coords at a given position for a given layer
    */
    static auto GetTextureUV(int32 X1, int32 Y1, int32 Y3, int32 WaterLayer) -> TexUV;

    /*!
    * @addr notsa
    * @brief Choose color to use for rendering
    *
    * @param real       The real color (As it should be originally)
    * @param debug      Debug color
    * @param WaterLayer The water layer for which this color is. If the choosen color is the `real` the associated alpha will be applied.
    *
    * @return The color that should be used for rendering
    */
    static CRGBA GetWaterColorForRendering(CRGBA real, DebugWaterColor debug, int32 WaterLayer);

    static void RenderFlatWaterTriangle_OneLayer(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3, int32 WaterLayer);
    static void RenderFlatWaterTriangle(int32 a1, int32 a2, CRenPar a3, int32 a4, int32 a5, CRenPar a6, int32 a7, int32 a8, CRenPar a9);
    static void RenderBoatWakes();
    static void SplitWaterTriangleAlongXLine(int32 a7, int32 a1, int32 a2, CRenPar a4, int32 a5, int32 a6, CRenPar arg18, int32 a8, int32 a9, CRenPar a10);

    static void RenderWaterRectangle(int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4);
    static void RenderFlatWaterRectangle(int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4);
    static void RenderFlatWaterRectangle_OneLayer(int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4, int32 WaterLayer);
    static void RenderHighDetailWaterRectangle(int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4);
    static void SplitWaterRectangleAlongXLine(int32 a1, int32 a2, int32 a3, int32 a4, int32 a5, CRenPar a6, CRenPar a7, CRenPar a8, CRenPar a9);
    static void SplitWaterRectangleAlongYLine(int32 splitAtY, int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4);

    static void PreRenderWater();
    static bool GetWaterDepth(const CVector& vecPos, float* pOutWaterDepth, float* pOutWaterLevel, float* pOutGroundLevel);
    static bool GetWaterLevel(float x, float y, float z, float& pOutWaterLevel, uint8 bTouchingWater, CVector* pVecNormals);
    static bool LoadDataFile();
    static void LoadTextures();
    static void WaterLevelInitialise();
    static void SetUpWaterFog(int32 a1, int32 a2, int32 a3, int32 a4);
    static void RenderWakeSegment(const CVector2D& a1, const CVector2D& a2, const CVector2D& a3, const CVector2D& a4, const float& widthA, const float& widthB, const float& alphaA, const float& alphaB, const float& wakeZ);
    static void FindNearestWaterAndItsFlow();
    static bool GetWaterLevelNoWaves(CVector pos, float * pOutWaterLevel, float * fUnkn1 = nullptr, float * fUnkn2 = nullptr);
    static void RenderWaterFog();
    static void CalculateWavesOnlyForCoordinate(int32 x, int32 y, float bigWavesAmplitude, float smallWavesAmplitude, float& outWave, float& colorMult, float& glare, CVector& vecNormal);
    static void MarkQuadsAndPolysToBeRendered(int32 blockX, int32 blockY, bool isInInterior);
    static void BlockHit(int32 X, int32 Y);

    static void CalculateWavesOnlyForCoordinate2(
        int32 x, int32 y,
        float* pResultHeight, // in/out variable => in is the "water level" from `GetWaterLevelNoWaves`/out is the Z coordinate of the wave
        float bigWavesAmpl,
        float smallWavesAmpl
    );

    static float CalculateWavesOnlyForCoordinate2_Direct( // TODO: Once the OG function is reversed, we should use this instead of it (once we've verified that the reversed version works as expected)
        int32 x, int32 y,
        float waterLevel, // "water level" from `GetWaterLevelNoWaves`
        float bigWavesAmpl,
        float smallWavesAmpl
    ) {
        CalculateWavesOnlyForCoordinate2(x, y, &waterLevel, bigWavesAmpl, smallWavesAmpl);
        return waterLevel; // Result of the above function is stored in this variable.
    }

    static void ScanThroughBlocks();
    static void SplitWaterTriangleAlongYLine(int32 a0, int32 a1, int32 a2, CRenPar a3, int32 a4, int32 a5, CRenPar a6, int32 a7, int32 a8, CRenPar a9);
    static void RenderHighDetailWaterTriangle(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3);
    static void RenderWater();
    static void SyncWater();

    static bool IsPointUnderwaterNoWaves(const CVector& point);
    static bool GetWaterLevel(const CVector& pos, float& outWaterLevel, bool touchingWater, CVector* normals = nullptr);

    static uint32 AddWaterLevelVertex(int32 X, int32 Y, CRenPar P);

    static void AddWaterLevelQuad(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3, int32 X4, int32 Y4, CRenPar P4, uint32 Flags);
    static void AddWaterLevelTriangle(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3, uint32 Flags);

    static void FillQuadsAndTrianglesList();

    static void SetCameraRange();
    static void HandleBeachToysStuff();
    static void UpdateFlow();

    /* Missing (In no particular order):
    static void AddWaveToResult(float x, float y, float z, float* pLevel, uint8 bTouchingWater, CVector* normalVec);
    AddWaveToResult(int32, int32, float*, float, float)
    BlockHit(int32, int32)
    CalculateWavesForCoordinate(int32, int32, float, float, float*, float*, float*, CVector*)
    ChangeWaterConfiguration(int32)
    CreateBeachToy(const CVector&, eBeachToy)
    FindNearestWaterAndItsFlow()
    FixVertexOnToLine(CWaterVertex*, CWaterVertex*, CWaterVertex*, float*)
    GetGroundLevel(const CVector&, float*, ColData*, float)
    GetGroundLevel_WS(const CVector&, float*, ColData*, float)
    RenderAndEmptyRenderBuffer()
    AddToQuadsAndTrianglesList(int32, int32, int32, uint32)
    AddWaterLevelQuad(int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar, uint32)
    AddWaterLevelTriangle(int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar, uint32)
    IsLocationOutOfWorldBounds_WS(const CVector&, int32)
    MarkQuadsAndPolysToBeRendered(int32, int32, bool)
    RenderBoatWakes()
    RenderDetailedSeaBedSegment(int32, int32, float, float, float, float)

    RenderFlatWaterTriangle(int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar)

    RenderFlatWaterRectangle(int32, int32, int32, int32, CRenPar, CRenPar, CRenPar, CRenPar)
    RenderFlatWaterRectangle_OneLayer(int32, int32, int32, int32, CRenPar, CRenPar, CRenPar, CRenPar, int32)

    RenderHighDetailWaterRectangle_OneLayer(int32, int32, int32, int32, CRenPar, CRenPar, CRenPar, CRenPar, int32, int32, int32, int32, int32)

    RenderHighDetailWaterTriangle(int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar)
    RenderHighDetailWaterTriangle_OneLayer(int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, int32, int32)

    RenderSeaBedSegment(int32, int32, float, float, float, float)
    RenderShipsOnHorizon()
    RenderTransparentWater()
    RenderWakeSegment(CVector2D&, CVector2D&, CVector2D&, CVector2D&, float&, float&, float&, float&, float&)
    HandleBeachToysStuff()
    IsLocationOutOfWorldBounds_WS(const CVector&, int32) // WS = World Space
    MarkQuadsAndPolysToBeRendered(int32, int32, bool)
    RenderWaterFog()
    RenderWaterRectangle(int32, int32, int32, int32, CRenPar, CRenPar, CRenPar, CRenPar)
    RenderWaterTriangle(int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar)
    ScanThroughBlocks()
    SetUpWaterFog(int32, int32, int32, int32)
    SplitWaterRectangleAlongXLine(int32, int32, int32, int32, int32, CRenPar, CRenPar, CRenPar, CRenPar)
    SplitWaterRectangleAlongYLine(int32, int32, int32, int32, int32, CRenPar, CRenPar, CRenPar, CRenPar)
    TestLineAgainstWater(CVector, CVector, CVector*)
    TestQuadToGetWaterLevel(CWaterQuad*, float, float, float, float*, float*, float*)
    TestTriangleToGetWaterLevel(CWaterTriangle*, float, float, float, float*, float*, float*)
    */
};
