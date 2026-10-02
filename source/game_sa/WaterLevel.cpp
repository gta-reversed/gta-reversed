#include "StdInc.h"
#include "WaterLevel.h"
#include "Camera.h"
#include "PostEffects.h"
#include "Weather.h"
#include <sstream>

#define TRIANGLE_ARGS_OUT X1, Y1, P1, X2, Y2, P2, X3, Y3, P3

void CWaterLevel::InjectHooks() {
    RH_ScopedClass(CWaterLevel);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(WaterLevelInitialise, 0x6EAE80);
    RH_ScopedGlobalInstall(Shutdown, 0x6E59E0);
    RH_ScopedGlobalInstall(RenderWaterTriangle, 0x6EE240);
    RH_ScopedGlobalInstall(RenderFlatWaterTriangle_OneLayer, 0x6E8ED0);
    RH_ScopedGlobalInstall(RenderFlatWaterTriangle, 0x6EE080);
    RH_ScopedGlobalInstall(SplitWaterTriangleAlongXLine, 0x6ECF00);
    RH_ScopedGlobalInstall(SplitWaterTriangleAlongYLine, 0x6EE5A0);

    RH_ScopedGlobalInstall(RenderWaterRectangle, 0x6EC5D0);
    RH_ScopedGlobalInstall(RenderFlatWaterRectangle_OneLayer, 0x6E9940);
    RH_ScopedGlobalInstall(RenderFlatWaterRectangle, 0x6EBEC0);

    RH_ScopedGlobalInstall(SplitWaterRectangleAlongXLine, 0x6E73A0);
    RH_ScopedGlobalInstall(SplitWaterRectangleAlongYLine, 0x6ED6D0);

    RH_ScopedGlobalInstall(PreRenderWater, 0x6EB710);
    RH_ScopedGlobalInstall(MarkQuadsAndPolysToBeRendered, 0x6E5810);
    RH_ScopedGlobalInstall(ScanThroughBlocks, 0x6E6D10);
    RH_ScopedGlobalInstall(BlockHit, 0x6E6CA0);

    // This one doesn't seem to work properly for whatever reason
    // It works for some quads, not for others... But then it works for that one too if only it's loaded from the file (eg.: you delete all others)
    // no clue what is the issue
    // one quad that doesn't load can be seen from -1610, 168
    // it's at water.dat:252
    RH_ScopedGlobalInstall(AddWaterLevelQuad, 0x6E7EF0);
    RH_ScopedGlobalInstall(AddWaterLevelTriangle, 0x6E7D40);
    RH_ScopedGlobalInstall(AddWaterLevelVertex, 0x6E5A40);

    RH_ScopedGlobalInstall(RenderBoatWakes, 0x6ED9A0);
    RH_ScopedGlobalInstall(RenderWakeSegment, 0x6EA260);

    RH_ScopedOverloadedInstall(GetWaterLevel, "", 0x6EB690, bool(*)(float, float, float, float&, uint8, CVector*));
    RH_ScopedGlobalInstall(SetUpWaterFog, 0x6EA9F0);
    RH_ScopedGlobalInstall(FindNearestWaterAndItsFlow, 0x6E9D70);
    RH_ScopedGlobalInstall(GetWaterLevelNoWaves, 0x6E8580);
    RH_ScopedGlobalInstall(RenderWaterFog, 0x6E7760);
    RH_ScopedGlobalInstall(CalculateWavesOnlyForCoordinate, 0x6E6EF0);
    RH_ScopedGlobalInstall(RenderWater, 0x6EF650);
    RH_ScopedGlobalInstall(RenderSeaBedSegment, 0x6E6870);
    RH_ScopedGlobalInstall(RenderDetailedSeaBedSegment, 0x6E6A10);
    RH_ScopedGlobalInstall(AddWaveToResult, 0x6E81E0);
    RH_ScopedGlobalInstall(SetCameraRange, 0x6E9C80);
    RH_ScopedGlobalInstall(CalculateWavesOnlyForCoordinate2, 0x6E7210);
    RH_ScopedGlobalInstall(GetWaterDepth, 0x6EA960);
    RH_ScopedGlobalInstall(GetGroundLevel, 0x6EA8A0);
}

// NOTSA
bool CWaterLevel::LoadDataFile() {
    const auto file = CFileMgr::OpenFile(m_nWaterConfiguration == 1 ? "DATA//water1.dat" : "DATA//water.dat", "r");

    const notsa::ScopeGuard autoCloser{ [&] { CFileMgr::CloseFile(file); } };

    uint32 nline{}, ntri{}, nquad{};
    for (;; nline++) {
        const auto line = CFileLoader::LoadLine(file);
        if (!line) {
            break;
        }
        std::stringstream liness{ line };

        auto nvertices{0u};

        struct {
            CVector   pos{};
            CVector2D flow{};
            float     bigWaves{}, smallWaves{};
        } vertices[4]{};

        // Helper function to read a vertex from the stream
        const auto ReadNextVertex = [&]() {
            const auto orgpos = liness.tellg();

            auto& vtx = vertices[nvertices];
            liness
                >> vtx.pos.x
                >> vtx.pos.y
                >> vtx.pos.z
                >> vtx.flow.x
                >> vtx.flow.y
                >> vtx.bigWaves
                >> vtx.smallWaves;

            if (liness.good()) {
                nvertices++;
                return true;
            } else {
                liness.clear(); // reset error flags
                liness.seekg(orgpos); // go back to before
                return false;
            }

        };

        // If can't read first vertex just ignore line
        if (!ReadNextVertex()) {
            continue;
        }

        // Read 2/3 more vertices
        while (ReadNextVertex() && nvertices < 4);

        // Check if we have enough vertices
        if (nvertices < 3) {
            NOTSA_LOG_DEBUG("[Warning]: Not enough vertices, got {}, expected 3 or 4. [Line: {}]", nvertices, nline);
            continue;
            //return false; // Just stop here, this parser is way too primitive to be able to recover from errors
        }

        // Optional flag after vertices
        uint32 flags{};
        liness >> flags;

        // I'm sorry, but don't blame me I HAD NO OTHER CHOICE!
        #define VertexUnpack(n) \
            (int32)vertices[n].pos.x, (int32)vertices[n].pos.y, \
            CRenPar{vertices[n].pos.z, vertices[n].bigWaves, vertices[n].smallWaves, (int8)(vertices[n].flow.x * 64.f), (int8)(vertices[n].flow.y * 64.f)}

        // Add quad/triangle
        if (nvertices == 4) {
            CWaterLevel::AddWaterLevelQuad(
                VertexUnpack(0),
                VertexUnpack(1),
                VertexUnpack(2),
                VertexUnpack(3),
                flags
            );
            nquad++;
        } else {
            CWaterLevel::AddWaterLevelTriangle(
                VertexUnpack(0),
                VertexUnpack(1),
                VertexUnpack(2),
                flags
            );
            ntri++;
        }
        #undef ArgUnpack
    }
    NOTSA_LOG_DEBUG("Successfully loaded! [Quads: {}; Tris: {}]", nquad, ntri);
    return true;
}

// NOTSA: Code @ 0x6EB5F4
void CWaterLevel::LoadTextures() {
    CTxdStore::PushCurrentTxd();    
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));

    const auto DoTex = [](auto& inOutTex, auto& outRaster, const char* name) {
        if (!inOutTex) {
            inOutTex = RwTextureRead(name, nullptr);
        }
        outRaster = RwTextureGetRaster(inOutTex);
    };
    DoTex(texWaterclear256, waterclear256Raster, "waterclear256");
    DoTex(texSeabd32,       seabd32Raster,       "seabd32"      );
    DoTex(texWaterwake,     waterwakeRaster,     "waterwake"    );

    CTxdStore::PopCurrentTxd();
}

// 0x6EAE80
void CWaterLevel::WaterLevelInitialise() {
    m_BlockPolyInfo = {};

    NumWaterTriangles = 0;
    NumWaterQuads = 0;
    NumWaterVertices = 0;
    m_ElementsOnQuadsAndTrianglesList = 0;
    
    (void)LoadDataFile();
    FillQuadsAndTrianglesList();

    LoadTextures();
}

// 0x6E59E0
void CWaterLevel::Shutdown() {
    // Unload Textures
    for (auto tex : { &texWaterclear256, &texSeabd32, &texWaterwake }) {
        if (*tex) {
            RwTextureDestroy(*tex);
            *tex = nullptr;
        }
    }
}

// 0x6E81E0
void CWaterLevel::AddWaveToResult(float x, float y, float* pfWaterLevel, float fBigWavesAmpl, float fSmallWavesAmpl, CVector* pVecNormal)
{
    // Waves are calculated on a grid from `(x / 2, y / 2)` (See below why we add 2 to get the next point)
    // The fractional part of the coordinate is used to interpolate between the points of the grid
    const auto fX = x * 0.5f;
    const auto fY = y * 0.5f;

    const auto iX = (int32)std::floor(fX);
    const auto iY = (int32)std::floor(fY);

    const auto fFracX = fX - (float)iX;
    const auto fFracY = fY - (float)iY;

    //! Because we divide the coordinate by 2, the next point of the grid is `2` away (Not 1!)
    constexpr auto GRID_STEP = 2;

    // The heights of the 3 (of the 4) grid points we're going to interpolate between
    // NOTE: These MUST be initialized to 0, as `CalculateWavesOnlyForCoordinate2` accumulates into them
    float h0{}, hX{}, hY{};

    //! The position is in the bottom-left triangle of the grid cell, and not the top-right one
    const auto isInBottomLeftTri = fFracX + fFracY < 1.0f;

    if (isInBottomLeftTri) {
        CalculateWavesOnlyForCoordinate2(iX,               iY,               fBigWavesAmpl, fSmallWavesAmpl, &h0); // (0, 0)
        CalculateWavesOnlyForCoordinate2(iX + GRID_STEP,   iY,               fBigWavesAmpl, fSmallWavesAmpl, &hX); // (1, 0)
        CalculateWavesOnlyForCoordinate2(iX,               iY + GRID_STEP,   fBigWavesAmpl, fSmallWavesAmpl, &hY); // (0, 1)
    } else {
        CalculateWavesOnlyForCoordinate2(iX + GRID_STEP,   iY + GRID_STEP,   fBigWavesAmpl, fSmallWavesAmpl, &h0); // (1, 1)
        CalculateWavesOnlyForCoordinate2(iX + GRID_STEP,   iY,               fBigWavesAmpl, fSmallWavesAmpl, &hX); // (1, 0)
        CalculateWavesOnlyForCoordinate2(iX,               iY + GRID_STEP,   fBigWavesAmpl, fSmallWavesAmpl, &hY); // (0, 1)
    }

    // Bilinearly interpolate the wave height between the 3 (of the 4) grid points
    *pfWaterLevel += isInBottomLeftTri
        ? h0 + fFracX * (hX - h0) + fFracY * (hY - h0)
        : h0 + (1.0f - fFracX) * (hY - h0) + (1.0f - fFracY) * (hX - h0);

    if (!pVecNormal) {
        return;
    }

    // Approximate the normal of the water surface at this position by crossing the
    // 2 (spatial) edge vectors of the triangle the position is in.
    auto vEdgeX = isInBottomLeftTri ? CVector{  2.0f,  0.0f, hX - h0 } : CVector{  0.0f, -2.0f, hX - h0 };
    auto vEdgeY = isInBottomLeftTri ? CVector{  0.0f,  2.0f, hY - h0 } : CVector{ -2.0f,  0.0f, hY - h0 };

    //! Note: In the non-bottom-left case the original crosses the edges in the opposite order
    if (isInBottomLeftTri) {
        CrossProduct(pVecNormal, &vEdgeX, &vEdgeY);
    } else {
        CrossProduct(pVecNormal, &vEdgeY, &vEdgeX);
    }
    pVecNormal->Normalise();
}

// 0x6EE240
void CWaterLevel::RenderWaterTriangle(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3) {
    const auto [minX, maxX] = std::make_pair(X1, X2); // Assumes: Starting in top left vertex with clockwise order
    const auto [minY, maxY] = std::minmax(Y1, Y3);
    if (minX >= CameraRangeMaxX || maxX <= CameraRangeMinX || minY >= CameraRangeMaxY || maxY <= CameraRangeMinY) { // Lies outside (of camera) fully
        RenderFlatWaterTriangle(TRIANGLE_ARGS_OUT);
    } else if (minX < CameraRangeMinX || maxX > CameraRangeMaxX) { // Lies inside on X 
        SplitWaterTriangleAlongXLine(minX < CameraRangeMinX ? CameraRangeMinX : CameraRangeMaxX, TRIANGLE_ARGS_OUT);
    } else if (minY < CameraRangeMinY || maxY > CameraRangeMaxY) { // Lies inside of Y
        SplitWaterTriangleAlongYLine(minY < CameraRangeMinY ? CameraRangeMinY : CameraRangeMaxY, TRIANGLE_ARGS_OUT);
    } else { // Lies inside of camera fully
        RenderHighDetailWaterTriangle(TRIANGLE_ARGS_OUT);
    }
}

// NOTSA
auto CWaterLevel::GetWaterLayerTexInfo(int32 layer) -> WaterLayerTexInfo {
    switch (layer) {
    case 0: return { { TextureShiftFirstU,  TextureShiftFirstV  }, 25.0f };
    case 1: return { { TextureShiftSecondU, TextureShiftSecondV }, 12.5f };
    default: NOTSA_UNREACHABLE();
    }
}

// NOTSA
CRGBA CWaterLevel::GetWaterColorForRendering(CRGBA real, DebugWaterColor debug, int32 WaterLayer) {
    if (debug.active) {
        return debug.color;
    } else {
        real *= 0.577f; // AKA 1/sqrt3 OR E_CONST OR neither, but just a coincidence?
        real.a = WaterLayerAlpha[WaterLayer];
        return real;
    }
}

// notsa
auto CWaterLevel::GetTextureUV(int32 X1, int32 Y1, int32 Y3, int32 WaterLayer) -> TexUV {
    const auto txinfo = GetWaterLayerTexInfo(WaterLayer);
    const auto posUV  = CVector2D{ (float)X1, (float)Y1 } / txinfo.size + txinfo.shift;

    const auto CalcShift = [](float p, bool dir) {
        return p - std::floor(p) + (dir ? 7.f : -7.f);
    };

    return {
        .size      = txinfo.size,
        .pos       = posUV,
        .baseShift = CVector2D{
            CalcShift(posUV.x, false),
            CalcShift(posUV.y, Y3 - Y1 <= 0)
        }
    };
}

// 0x6E8ED0
void CWaterLevel::RenderFlatWaterTriangle_OneLayer(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3, int32 WaterLayer) {
    RenderBuffer::RenderIfDoesntFit(5, 3);

    // First(!) push indices
    RenderBuffer::PushIndices({ 0, 1, 2 }, true);

    // And push vertices into the buffer
    const auto PushVertex = [
        &,
        tex       = GetTextureUV(X1, Y1, Y3, WaterLayer),
        pos2DVtx1 = CVector2D{ (float)X1, (float)Y2 },
        color     = GetWaterColorForRendering(WaterColorTriangle, DebugWaterColors[DebugWaterColor::TRI], WaterLayer)
    ](int32 x, int32 y, CRenPar p) {
        const auto pos2DThis = CVector2D{ (float)x, (float)y };
        RenderBuffer::PushVertex(
            CVector{ pos2DThis, p.z },
            (pos2DThis - pos2DVtx1) / tex.size + tex.baseShift,
            color
        );
    };

    PushVertex(X1, Y1, P1);
    PushVertex(X2, Y2, P2);
    PushVertex(X3, Y3, P3);
}

// 0x6EE080
void CWaterLevel::RenderFlatWaterTriangle(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3) {
    if (bSplitBigPolys && X2 - X1 > BigPolySize) {
        SplitWaterTriangleAlongXLine((X1 + X2) / 2, X1, Y1, P1, X2, Y2, P2, X3, Y3, P3);
    } else {
        RenderFlatWaterTriangle_OneLayer(X1, Y1, P1, X2, Y2, P2, X3, Y3, P3, 0);
        RenderFlatWaterTriangle_OneLayer(X1, Y1, P1, X2, Y2, P2, X3, Y3, P3, 1);
    }
}

// 0x6EA260
void CWaterLevel::RenderWakeSegment(
    const CVector2D& vecA, const CVector2D& vecB,
    const CVector2D& vecC, const CVector2D& vecD,
    const float& widthA, const float& widthB,
    const float& alphaA, const float& alphaB,
    const float& wakeZ
) {
    constexpr auto  NUM_PARTS = 4;
    constexpr float ALPHA_MULTS[]{ 0.4f, 1.f, 0.2f, 1.f, 0.4f }; // 0x8D390C

    const auto angle      = (float)(CTimer::GetTimeInMS() % 4096) / (4096.f / (2.f * PI));
    const auto windRadius = CWeather::WindClipped * 0.4f + 0.2f;

    for (auto partIdx = 0; partIdx < NUM_PARTS; partIdx++) {
        RenderBuffer::RenderIfDoesntFit(6, 4);

        RenderBuffer::PushIndices({ 0, 2, 1, 0, 3, 2 }, true);

        const CVector2D corners[]{
            lerp(vecB, vecA, (float)(partIdx + 0) / (float)(NUM_PARTS)),
            lerp(vecB, vecA, (float)(partIdx + 1) / (float)(NUM_PARTS)),
            lerp(vecC, vecD, (float)(partIdx + 1) / (float)(NUM_PARTS)),
            lerp(vecC, vecD, (float)(partIdx + 0) / (float)(NUM_PARTS)),
        };

        CVector2D uvs[4]{};
        rng::transform(corners, uvs, [](const CVector2D& pos) -> CVector2D {
            return { pos.x / (float)(NUM_PARTS), pos.y / (float)(NUM_PARTS) };
        });
        rng::transform(uvs, uvs, // Isn't it beautiful?
            [
                minUV = CVector2D{
                    std::floor(rng::min(uvs, {}, &CVector2D::x).x),
                    std::floor(rng::min(uvs, {}, &CVector2D::y).y)
                }
            ](auto& uv) {
                return uv - minUV;
            }
        );

        const float alphas[]{
            alphaA * ALPHA_MULTS[partIdx + 0],
            alphaA * ALPHA_MULTS[partIdx + 1],
            alphaB * ALPHA_MULTS[partIdx + 1],
            alphaB * ALPHA_MULTS[partIdx + 0],
        };

        for (auto i = 0; i < 4; i++) {
            const auto CalcAngleOfPos = [&](float p) {
                p += 3072.f; // TODO: Magic number, but I think it's meaningless (as the integer part is discarded below)
                p /= 32.f;   // TODO: Magic number (maybe meaningful this time) 
                return p - std::floor(p); // Extract fractional part
            };
            const auto  z   = wakeZ + std::sin((CalcAngleOfPos(corners[i].x) + CalcAngleOfPos(corners[i].y)) * PI * 2.f + angle) * windRadius;
            const auto& rgb = WakeSegmentPartColors[i];
            RenderBuffer::PushVertex(
                CVector{ corners[i], z },
                uvs[i],
                { (uint8)(rgb.r * 255.f), (uint8)(rgb.g * 255.f), (uint8)(rgb.b * 255.f), (uint8)(alphas[i]) }
            );
        }
    }
}

// 0x6ED9A0
void CWaterLevel::RenderBoatWakes() {
    CBoat::RenderAllWakePointBoats();
}

// 0x6ECF00
void CWaterLevel::SplitWaterTriangleAlongXLine(int32 splitAtX, int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3) {
    assert(Y1 == Y2); 

    // Ease of life
    const auto XS = splitAtX;

    const auto splitWidth = XS - X1;
    const auto triWidth   = X2 - X1;

    // Calculate position of split along Y axis
    const auto CalcSplitPosY = [&](int32 fromY, int32 toY) {
        return fromY + (toY - fromY) * splitWidth / triWidth;
    };

    // Interpolation value
    const auto t = (float)splitWidth / (float)triWidth;

    // New interpolations of RenPar's along a few segments
    const auto P12 = lerp(P1, P2, t);
    const auto P13 = lerp(P1, P3, t);
    const auto P23 = lerp(P2, P3, t);

    // Vertex 1 and 2 are always (top left), (top right)
    // Also the triangles always contain a 90deg corner at either the left or right side.

    if (X1 == X3) { // Vertex 3 => (bottom left)
        const auto YS = CalcSplitPosY(Y3, Y1);

        // Bottom
        RenderWaterTriangle(
            X1, YS, P12,
            XS, YS, P13,
            X3, Y3, P3
        );

        // Left
        RenderWaterRectangle(
            X1, XS,
            Y1, YS,
            P1, P12, P23, P13
        );

        // Right
        RenderWaterTriangle(
            XS, Y1, P12,
            X2, Y1, P2,
            XS, YS, P23
        );
    } else if (X2 == X3) { // Vertex 3 => (bottom right)
        const auto YS = CalcSplitPosY(Y1, Y3);

        // Left
        RenderWaterTriangle(
            X1, Y1, P1,
            XS, Y1, P12,
            XS, YS, P13
        );

        // Right
        RenderWaterRectangle(
            XS, X2,
            Y1, YS,
            P12, P2, P23, P12
        );

        // Bottom
        RenderWaterTriangle(
            XS, YS, P13,
            X2, YS, P23,
            X3, Y3, P3
        );
    } else {
        NOTSA_UNREACHABLE("Triangle has no 90deg corner => Very bad");
    }
}

// 0x6EE5A0
void CWaterLevel::SplitWaterTriangleAlongYLine(int32 splitAtY, int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3) {
    if (DontRenderYSplitTri) { // NOTSA
        return;
    }

    const auto [minY, maxY] = std::minmax(Y1, Y3);
    const auto height = maxY - minY;
    const auto width  = X2 - X1;
    
    // Calulcate the X position where the Y line intersects the hypot
    // and using that we split the triangle. 
    // Same result as original code, but much easier.

    SplitWaterTriangleAlongXLine(
        X1 + (maxY - splitAtY) * width / height,
        X1, Y1, P1,
        X2, Y2, P2,
        X3, Y3, P3
    );
}

// 0x6EC5D0
void CWaterLevel::RenderWaterRectangle(int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4) {
    const auto [minY, maxY] = std::minmax(Y1, Y2);
    if (minX >= CameraRangeMaxX || maxX <= CameraRangeMinX || minY >= CameraRangeMaxY || maxY <= CameraRangeMinY) { // Lies outside (of camera) fully
        RenderFlatWaterRectangle(minX, maxX, Y1, Y2, P1, P2, P3, P4);
    } else if (minX < CameraRangeMinX || maxX > CameraRangeMaxX) { // Lies inside on X
        SplitWaterRectangleAlongXLine(minX < CameraRangeMinX ? CameraRangeMinX : CameraRangeMaxX, minX, maxX, Y1, Y2, P1, P2, P3, P4);
    } else if (minY < CameraRangeMinY || maxY > CameraRangeMaxY) { // Lies inside of Y
        SplitWaterRectangleAlongYLine(minY < CameraRangeMinY ? CameraRangeMinY : CameraRangeMaxY, minX, maxX, Y1, Y2, P1, P2, P3, P4);
    } else { // Lies inside of camera fully
        RenderHighDetailWaterRectangle(minX, maxX, Y1, Y2, P1, P2, P3, P4);
    }
}

// 0x6EBEC0
void CWaterLevel::RenderFlatWaterRectangle(int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4) {
    if (bSplitBigPolys && maxX - minX > BigPolySize) {
        SplitWaterRectangleAlongXLine((minX + maxX) / 2,  minX, maxX, Y1, Y2, P1, P2, P3, P4);
#ifdef FIX_BUGS
    } else if (const auto [minY, maxY] = std::minmax(Y1, Y2); bSplitBigPolys && (maxY - minY) > BigPolySize) {
#else
    } else if (bSplitBigPolys && Y2 - Y1 > BigPolySize) {
#endif
        SplitWaterRectangleAlongYLine((Y2 + Y1) / 2, minX, maxX, Y1, Y2, P1, P2, P3, P4);
    } else {
        for (int32 lyr = 0; lyr < 2; lyr++) {
            RenderFlatWaterRectangle_OneLayer(minX, maxX, Y1, Y2, P1, P2, P3, P4, lyr);
        }
    }
}

// 0x6E9940
void CWaterLevel::RenderFlatWaterRectangle_OneLayer(int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4, int32 WaterLayer) {
    RenderBuffer::RenderIfDoesntFit(6, 4);

    // First(!) push indices
    RenderBuffer::PushIndices({ 0, 1, 2, 2, 3, 0 }, true);

    // Get texture UV stuff
    const auto texuv = GetTextureUV(minX, Y1, Y2, WaterLayer);

    const auto PushVertex = [
        &,
        color = GetWaterColorForRendering(WaterColor, DebugWaterColors[DebugWaterColor::RECT], WaterLayer)
    ](int32 x, int32 y, const CRenPar& p, CVector2D vtxUVOffset) {
        RenderBuffer::PushVertex({ (float)x, (float)y, p.z }, texuv.baseShift + vtxUVOffset, color);
    };

    // Bottom right corner position on texture (In UV coords)
    const auto bruv{ CVector2D{ (float)(maxX - minX), (float)(Y2 - Y1) } / texuv.size };

    PushVertex(minX, Y1, P1, { 0.f,    0.f }); // Top Left
    PushVertex(maxX, Y1, P2, { bruv.x, 0.f    }); // Top Right
    PushVertex(maxX, Y2, P3, { bruv.x, bruv.y }); // Bottom Right
    PushVertex(minX, Y2, P4, { 0.f,    bruv.y }); // Bottom Left
} 

void CWaterLevel::RenderHighDetailWaterRectangle(int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4) {
    plugin::Call<0x6EB810>(minX, maxX, Y1, Y2, P1, P2, P3, P4);
}

// 0x6E73A0
void CWaterLevel::SplitWaterRectangleAlongXLine(int32 splitAtX, int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4) {
    const auto t = (float)(splitAtX - minX) / (float)(maxX - minX);

    const auto P12 = lerp(P1, P2, t);
    const auto P34 = lerp(P3, P4, t);

    // Left
    RenderWaterRectangle(
        minX, splitAtX,
        Y1, Y2,
        P1, P12, P3, P34
    );

    // Right
    RenderWaterRectangle(
        splitAtX, maxX,
        Y1, Y2,
        P12, P2, P34, P4
    );
}

// 0x6ED6D0 - Though fully inlined into `RenderWaterRectangle`
void CWaterLevel::SplitWaterRectangleAlongYLine(int32 splitAtY, int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4) {
    const auto [minY, maxY] = std::minmax(Y1, Y2);

    const auto t = (float)(splitAtY - minY) / (float)(maxY - minY);

    const auto P13 = lerp(P1, P3, t);
    const auto P24 = lerp(P2, P4, t);

    // Top
    RenderWaterRectangle(
        minX, maxX,
        Y1, splitAtY,
        P1, P2, P13, P24
    );

    // Bottom
    RenderWaterRectangle(
        minX,     maxX,
        splitAtY, Y2,
        P13, P24, P3, P4
    );
}

// 0x6EB710
void CWaterLevel::PreRenderWater() {
    ZoneScoped;

    if (CGame::CanSeeWaterFromCurrArea()) {
        ScanThroughBlocks();
        UpdateFlow();
        HandleBeachToysStuff();
    }
}

// NOTSA: From PreRenderWater()
void CWaterLevel::UpdateFlow() {
    if (CTimer::m_FrameCounter % 32 == 29) {
        CWaterLevel::FindNearestWaterAndItsFlow();
    }

    const auto CalculateFlowOnAxis = [
        step = CTimer::GetTimeStep() / 1000.f
    ](float desired, float curr) {
        const auto delta = desired - curr;
        return std::abs(delta) < step
            ? desired
            : curr + std::copysign(step, delta);
    };

    m_CurrentFlow = {
        CalculateFlowOnAxis(m_CurrentDesiredFlow.x, m_CurrentFlow.x),
        CalculateFlowOnAxis(m_CurrentDesiredFlow.y, m_CurrentFlow.y)
    };
}

// 0x6EB690
bool CWaterLevel::GetWaterLevel(float x, float y, float z, float& pOutWaterLevel, uint8 bTouchingWater, CVector* pVecNormals) {
    float smallWaves, bigWaves;
    if (!GetWaterLevelNoWaves({x, y, z}, &pOutWaterLevel, &smallWaves, &bigWaves)) {
        return false;
    }
     
    if ((pOutWaterLevel - z > 3.0F) && !bTouchingWater) {
        pOutWaterLevel = 0.0F;
        return false;
    }

    AddWaveToResult(x, y, &pOutWaterLevel, smallWaves, bigWaves, pVecNormals);

    return true;
}

// 0x6EA9F0
void CWaterLevel::SetUpWaterFog(int32 minX, int32 minY, int32 maxX, int32 maxY) {
    if (!CWaterLevel::m_bWaterFog || gWaterFogIndex >= 70) {
        return;
    }

    const auto fogZ = [&] {
        if (float waterLvl, bigWaves, smallWaves; GetWaterLevelNoWaves({(float)minX, (float)minY, 0.f}, & waterLvl, & bigWaves, & smallWaves)) {
            if (bigWaves != 0.f || smallWaves != 0.f) {
                return waterLvl;
            }
        }
        return 0.f;
    }();

    const auto plyrPos = FindPlayerCoors();
    gbPlayerIsInsideWaterFog = m_fWaterFogHeight + fogZ > plyrPos.z && CRect{ (float)minX, (float)minY, (float)maxX, (float)maxY }.IsPointInside(plyrPos);

    const auto idx = gWaterFogIndex++;
    ms_WaterFog.minX[idx] = minX;
    ms_WaterFog.minY[idx] = minY;
    ms_WaterFog.maxX[idx] = maxX;
    ms_WaterFog.maxY[idx] = maxY;
    ms_WaterFog.z[idx]    = fogZ;
}

// 0x6E5750
void CWaterLevel::AddToQuadsAndTrianglesList(int32 blockX, int32 blockY, int32 polyId, uint32 type) {
    auto& blockInfo = m_BlockPolyInfo[blockX][blockY];

    const auto existingType = blockInfo.Type();
    if (existingType == PolyInfo::PType::NONE) { // Store it directly in the block
        blockInfo = PolyInfo{ (PolyInfo::PType)type, (uint16)polyId };
        return;
    }

    const auto start = m_ElementsOnQuadsAndTrianglesList;
    if (existingType != PolyInfo::PType::SINGLE_QUAD && existingType != PolyInfo::PType::SINGLE_TRI) { // Append to the combo we're currently building
        m_PolyCombos[start - 1] = PolyInfo{ (PolyInfo::PType)type, (uint16)polyId }; // Overwrite the previous terminator
        m_PolyCombos[start]     = PolyInfo{};                                        // New terminator
        m_ElementsOnQuadsAndTrianglesList = start + 1;
        return;
    }

    // Block already has a single poly => Start a new combo
    m_PolyCombos[start + 0] = blockInfo;
    m_PolyCombos[start + 1] = PolyInfo{ (PolyInfo::PType)type, (uint16)polyId };
    m_PolyCombos[start + 2] = PolyInfo{}; // Terminator
    m_ElementsOnQuadsAndTrianglesList = start + 3;

    blockInfo = PolyInfo{ PolyInfo::PType::COMBO, (uint16)start };
}

// 0x6E9D70
void CWaterLevel::FindNearestWaterAndItsFlow() {
    const auto camPos = TheCamera.GetPosition();

    float nearestQuadDist   = 1e7f; // `local_8`
    float nearestWaterDist  = 1e7f; // `local_c`
    float nearestWaterLevel = 0.0f; // `local_4`

    if (camPos.x > -3000.0f && camPos.x < 3000.0f && camPos.y > -3000.0f && camPos.y < 3000.0f) {
        for (auto i = 0u; i < NumWaterQuads; i++) {
            const auto& quad = WaterQuads[i];
            const auto v0 = quad.GetVertex(0);
            const auto v1 = quad.GetVertex(1);
            const auto v2 = quad.GetVertex(2);
            const auto v3 = quad.GetVertex(3);

            // Distance from the camera to the quad's footprint (0 if inside)
            const auto dx = camPos.x < (float)v0.x
                ? (float)v0.x - camPos.x
                : ((float)v1.x < camPos.x ? camPos.x - (float)v1.x : 0.0f);
            const auto dy = camPos.y < (float)v0.y
                ? (float)v0.y - camPos.y
                : ((float)v2.y < camPos.y ? camPos.y - (float)v2.y : 0.0f);
            const auto dist = std::sqrt(dx * dx + dy * dy);

            // Remember the closest water surface that actually has waves
            if (dist < nearestWaterDist && (
                v0.rp.bigWaves != 0.0f || v0.rp.smallWaves != 0.0f ||
                v1.rp.bigWaves != 0.0f || v1.rp.smallWaves != 0.0f ||
                v2.rp.bigWaves != 0.0f || v2.rp.smallWaves != 0.0f ||
                v3.rp.bigWaves != 0.0f || v3.rp.smallWaves != 0.0f
            )) {
                nearestWaterLevel = v0.rp.z;
                nearestWaterDist  = dist;
            }

            // Remember the flow of the closest vertex (Of any quad)
            if (dist < nearestQuadDist) {
                nearestQuadDist = dist;

                const auto SqDistToVtx = [&](const CWaterVertex& v) {
                    return sq(camPos.y - (float)v.y) + sq(camPos.x - (float)v.x);
                };
                const auto sqDistV0 = SqDistToVtx(v0);
                const auto sqDistV1 = SqDistToVtx(v1);
                const auto sqDistV2 = SqDistToVtx(v2);
                const auto sqDistV3 = SqDistToVtx(v3);

                const CWaterVertex* nearestVtx;
                if (sqDistV1 <= sqDistV0 || sqDistV2 <= sqDistV0 || sqDistV3 <= sqDistV0) {
                    if (sqDistV2 <= sqDistV1 || sqDistV3 <= sqDistV1) {
                        nearestVtx = sqDistV3 <= sqDistV2 ? &v3 : &v2;
                    } else {
                        nearestVtx = &v1;
                    }
                } else {
                    nearestVtx = &v0;
                }

                m_CurrentDesiredFlow.x = (float)nearestVtx->rp.flowX * (1.0f / 64.0f);
                m_CurrentDesiredFlow.y = (float)nearestVtx->rp.flowY * (1.0f / 64.0f);
            }
        }

        TheCamera.m_fDistanceToWater     = nearestWaterDist;
        TheCamera.m_fHeightOfNearestWater = nearestWaterLevel;
    } else {
        TheCamera.m_fDistanceToWater     = 0.0f;
        TheCamera.m_fHeightOfNearestWater = 0.0f;
        m_CurrentDesiredFlow = {};
    }
}

// 0x6E5BB0
bool CWaterLevel::TestQuadToGetWaterLevel(CWaterQuad* quad, float x, float y, float z, float* pOutWaterLevel, float* pOutBigWaves, float* pOutSmallWaves) {
    const auto v0 = quad->GetVertex(0);
    const auto v1 = quad->GetVertex(1);
    const auto v2 = quad->GetVertex(2);
    const auto v3 = quad->GetVertex(3);

    if (x < (float)v0.x || x > (float)v1.x || y < (float)v0.y || y > (float)v2.y) {
        return false;
    }

    const auto u = (x - (float)v0.x) / (float)(v1.x - v0.x);
    const auto v = (y - (float)v0.y) / (float)(v2.y - v0.y);

    if (u + v <= 1.0f) { // Inside the quad, in the triangle containing V0 (Uses the V0 vertex as the base)
        *pOutWaterLevel = (v1.rp.z - v0.rp.z) * u + (v2.rp.z - v0.rp.z) * v + v0.rp.z;
        if (pOutBigWaves) {
            *pOutBigWaves   = (v1.rp.bigWaves   - v0.rp.bigWaves)   * u + (v2.rp.bigWaves   - v0.rp.bigWaves)   * v + v0.rp.bigWaves;
            *pOutSmallWaves = (v1.rp.smallWaves - v0.rp.smallWaves) * u + (v2.rp.smallWaves - v0.rp.smallWaves) * v + v0.rp.smallWaves;
        }
    } else { // Inside the quad, in the triangle containing V3 (Uses the V3 vertex as the base)
        const auto uu = 1.0f - u, vv = 1.0f - v; // Save ourselves some math
        *pOutWaterLevel = (v1.rp.z - v3.rp.z) * vv + (v2.rp.z - v3.rp.z) * uu + v3.rp.z;
        if (pOutBigWaves) {
            *pOutBigWaves   = (v2.rp.bigWaves   - v3.rp.bigWaves)   * uu + (v1.rp.bigWaves   - v3.rp.bigWaves)   * vv + v3.rp.bigWaves;
            *pOutSmallWaves = (v2.rp.smallWaves - v3.rp.smallWaves) * uu + (v1.rp.smallWaves - v3.rp.smallWaves) * vv + v3.rp.smallWaves;
        }
    }

    if (!pOutBigWaves) {
        return true;
    }

    if (!(*pOutWaterLevel - 6.0f <= z || !quad->bLimitedDepth)) {
        return false;
    }

    return *pOutWaterLevel + 20.0f >= z;
}

// 0x6E5E90
bool CWaterLevel::TestTriangleToGetWaterLevel(CWaterTriangle* tri, float x, float y, float z, float* pOutWaterLevel, float* pOutBigWaves, float* pOutSmallWaves) {
    const auto v0 = tri->GetVertex(0);
    const auto v1 = tri->GetVertex(1);
    const auto v2 = tri->GetVertex(2);

    if (x < (float)v0.x || x > (float)v1.x) {
        return false;
    }

    if (y < (float)std::min(v0.y, v2.y) || y > (float)std::max(v0.y, v2.y)) {
        return false;
    }

    auto u = (x - (float)v0.x) / (float)(v1.x - v0.x);
    const auto v = (y - (float)v0.y) / (float)(v2.y - v0.y);

    if (v0.x == v2.x) { // Hypotenuse goes from V1 (top right) to V2 (bottom left), so the triangle is around V0 (top left)
        if (u + v > 1.0f) {
            return false;
        }
        *pOutWaterLevel = (v1.rp.z - v0.rp.z) * u + (v2.rp.z - v0.rp.z) * v + v0.rp.z;
        if (pOutBigWaves) {
            *pOutBigWaves   = (v2.rp.bigWaves   - v0.rp.bigWaves)   * v + (v1.rp.bigWaves   - v0.rp.bigWaves)   * u + v0.rp.bigWaves;
            *pOutSmallWaves = (v1.rp.smallWaves - v0.rp.smallWaves) * u + (v2.rp.smallWaves - v0.rp.smallWaves) * v + v0.rp.smallWaves;
        }
    } else {
        if (u < v) {
            return false;
        }
        u = 1.0f - u;
        *pOutWaterLevel = (v0.rp.z - v1.rp.z) * u + (v2.rp.z - v1.rp.z) * v + v1.rp.z;
        if (pOutBigWaves) {
            *pOutBigWaves   = (v2.rp.bigWaves   - v1.rp.bigWaves)   * v + (v0.rp.bigWaves   - v1.rp.bigWaves)   * u + v1.rp.bigWaves;
            *pOutSmallWaves = (v2.rp.smallWaves - v1.rp.smallWaves) * v + (v0.rp.smallWaves - v1.rp.smallWaves) * u + v1.rp.smallWaves;
        }
    }

    if (!(*pOutWaterLevel - 6.0f <= z || !tri->bLimitedDepth)) {
        return false;
    }

    return *pOutWaterLevel + 20.0f >= z;
}

// 0x6E8580
bool CWaterLevel::GetWaterLevelNoWaves(CVector pos, float* pOutWaterLevel, float* pOutBigWaves, float* pOutSmallWaves) {
    // Convert world coords to block index (World is 6000x6000, block size is 500)
    const auto BlockIdx = [](float c) { return (int32)std::floor(c * 0.002f + 6.0f); };

    const auto blockX = BlockIdx(pos.x);
    const auto blockY = BlockIdx(pos.y);

    if (blockX < 0 || blockX >= NUM_WATER_BLOCKS_ROWCOL || blockY < 0 || blockY >= NUM_WATER_BLOCKS_ROWCOL) {
        *pOutWaterLevel = 0.0f;
        if (pOutBigWaves) {
            *pOutBigWaves = 1.0f;
        }
        if (pOutSmallWaves) {
            *pOutSmallWaves = 0.0f;
        }
        return true;
    }

    const auto& blockInfo = m_BlockPolyInfo[blockX][blockY];

    using PType = PolyInfo::PType;
    switch (blockInfo.Type()) {
    case PType::SINGLE_QUAD:
        return TestQuadToGetWaterLevel(&WaterQuads[blockInfo.Id()], pos.x, pos.y, pos.z, pOutWaterLevel, pOutBigWaves, pOutSmallWaves);
    case PType::SINGLE_TRI:
        return TestTriangleToGetWaterLevel(&WaterTriangles[blockInfo.Id()], pos.x, pos.y, pos.z, pOutWaterLevel, pOutBigWaves, pOutSmallWaves);
    case PType::COMBO:
        for (auto& combo : m_PolyCombos | rng::views::drop(blockInfo.Id())) {
            if (combo.Type() == PType::NONE) { // End of sequence
                break;
            }
            if (combo.Type() == PType::SINGLE_QUAD && TestQuadToGetWaterLevel(&WaterQuads[combo.Id()], pos.x, pos.y, pos.z, pOutWaterLevel, pOutBigWaves, pOutSmallWaves)) {
                return true;
            }
            if (combo.Type() == PType::SINGLE_TRI && TestTriangleToGetWaterLevel(&WaterTriangles[combo.Id()], pos.x, pos.y, pos.z, pOutWaterLevel, pOutBigWaves, pOutSmallWaves)) {
                return true;
            }
        }
        return false;
    default: // PType::NONE
        return false;
    }
}

// 0x6EA960
bool CWaterLevel::GetWaterDepth(const CVector& vecPos, float* pOutWaterDepth, float* pOutWaterLevel, float* pOutGroundLevel)
{
    float fWaterLevel;
    if (!GetWaterLevelNoWaves(vecPos, &fWaterLevel, nullptr, nullptr)) {
        return false;
    }

    float fGroundLevel;
    if (!GetGroundLevel(vecPos, &fGroundLevel, nullptr, 30.0f)) {
        fGroundLevel = -100.0f; // 0x859014
    }

    if (pOutWaterDepth) {
        *pOutWaterDepth = fWaterLevel - fGroundLevel;
    }
    if (pOutWaterLevel) {
        *pOutWaterLevel = fWaterLevel;
    }
    if (pOutGroundLevel) {
        *pOutGroundLevel = fGroundLevel;
    }
    return true;
}

// 0x6EA8A0
bool CWaterLevel::GetGroundLevel(const CVector& vecPos, float* pOutGroundZ, ColData* pOutColData, float fMaxDist)
{
    CColPoint colPoint;
    CEntity*  entity{};
    if (!CWorld::ProcessVerticalLine(
        CVector{ vecPos.x, vecPos.y, vecPos.z + fMaxDist }, // Origin
        -fMaxDist,                                          // Distance
        colPoint,
        entity,
        true,   // Buildings
        false,  // Vehicles
        false,  // Peds
        false,  // Objects
        true,   // Dummies
        false,  // See through check
        nullptr
    )) {
        return false;
    }

    *pOutGroundZ = colPoint.m_vecPoint.z;
    if (pOutColData) {
        *pOutColData = ColData{ colPoint.m_nSurfaceTypeB, colPoint.m_nPieceTypeB };
    }
    return true;
}

// 0x6E7760
void CWaterLevel::RenderWaterFog() {
    ZoneScoped;

    if (!m_bWaterFog || !m_bWaterFogScript) {
        return;
    }

    if (CWeather::UnderWaterness >= CPostEffects::m_fWaterFXStartUnderWaterness) {
        gWaterFogIndex = 0;
        return;
    }

    // Amount of vertical "slices" the fog volume is made out of
    const auto numVerticalLayers = (int32)((float)m_WaterFogDensity * CWeather::WaterFogFXControl);
    if (numVerticalLayers == 0) {
        gWaterFogIndex = 0;
        return;
    }

    const auto zStep          = m_fWaterFogHeight / (float)numVerticalLayers;
    const auto numFogRegions  = gWaterFogIndex;
    gWaterFogIndex = 0;

    if (!gbPlayerIsInsideWaterFog) { // Player left the fog => Fade out
        m_fWaterFogTimer -= CTimer::GetTimeStep();
        if (m_fWaterFogTimer <= 0.0f) {
            m_fWaterFogTimer = 0.0f;
            m_fWaterFogInsideFade -= CTimer::GetTimeStep() * m_fWaterFogInsideFadeSpeed;
            if (m_fWaterFogInsideFade <= 0.0f) {
                m_fWaterFogInsideFade = 0.0f;
            }
        }
    } else { // Player entered the fog => Fade in
        m_fWaterFogInsideFade += CTimer::GetTimeStep() * m_fWaterFogInsideFadeSpeed;
        if (m_fWaterFogInsideFade > 1.0f) {
            m_fWaterFogInsideFade = 1.0f;
        }
        m_fWaterFogTimer = 40.0f;
    }
    gbPlayerIsInsideWaterFog = false;

    if (m_fWaterFogInsideFade > 0.0f) {
        // Full-screen color overlay (The "inside" fog)
        const auto insideAlpha = (uint8)((float)(int32)((float)m_WaterFogInsideCol.a * m_fWaterFogInsideFade) * CWeather::WaterFogFXControl);

        CPostEffects::ImmediateModeRenderStatesStore();
        CPostEffects::ImmediateModeRenderStatesSet();
        CPostEffects::DrawQuad(
            0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT,
            m_WaterFogInsideCol.r, m_WaterFogInsideCol.g, m_WaterFogInsideCol.b,
            insideAlpha, nullptr
        );
        CPostEffects::ImmediateModeRenderStatesReStore();

        if (m_fWaterFogInsideFade == 1.0f) { // Fully inside => Only the overlay is drawn
            return;
        }
    }

    CPostEffects::ImmediateModeRenderStatesStore();
    CPostEffects::ImmediateModeRenderStatesSet();
    RwRenderStateSet((RwRenderState)6, (void*)1);
    RwRenderStateSet((RwRenderState)1, (void*)0);

    const auto numLayers = (int32)((1.0f - m_fWaterFogInsideFade) * (float)numVerticalLayers);
    const auto color     = CRGBA{ m_WaterFogCol.r, m_WaterFogCol.g, m_WaterFogCol.b, m_WaterFogCol.a };

    auto nVerticesStored = 0u;
    for (auto i = 0u; i < (uint32)numFogRegions; i++) {
        // The 6 (x, y) pairs that make up a fog volume's quad (In tristrip-ish order)
        const float xs[]{
            (float)ms_WaterFog.minX[i], (float)ms_WaterFog.maxX[i], (float)ms_WaterFog.maxX[i],
            (float)ms_WaterFog.minX[i], (float)ms_WaterFog.maxX[i], (float)ms_WaterFog.minX[i]
        };
        const float ys[]{
            (float)ms_WaterFog.minY[i], (float)ms_WaterFog.minY[i], (float)ms_WaterFog.maxY[i],
            (float)ms_WaterFog.minY[i], (float)ms_WaterFog.maxY[i], (float)ms_WaterFog.maxY[i]
        };

        auto z = ms_WaterFog.z[i];
        for (auto layer = numLayers; layer > 0; layer--) {
            for (auto vtxIdx = 0u; vtxIdx < std::size(xs); vtxIdx++) {
                auto& vtx = TempBufferVertices.m_3d[nVerticesStored];
                RwIm3DVertexSetPos(&vtx, xs[vtxIdx], ys[vtxIdx], z);
                RwIm3DVertexSetRGBA(&vtx, color.r, color.g, color.b, color.a);

                if (++nVerticesStored == 0x7FE) {
                    if (RwIm3DTransform(TempBufferVertices.m_3d, 0x7FE, nullptr, rwIM3D_VERTEXXYZ)) {
                        RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
                        RwIm3DEnd();
                    }
                    nVerticesStored = 0;
                }
            }
            z += zStep;
        }
    }

    if (nVerticesStored > 0 && RwIm3DTransform(TempBufferVertices.m_3d, nVerticesStored, nullptr, rwIM3D_VERTEXXYZ)) {
        RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
        RwIm3DEnd();
    }

    CPostEffects::ImmediateModeRenderStatesReStore();
}

// 0x6E6EF0
void CWaterLevel::CalculateWavesOnlyForCoordinate(
    int32 x, int32 y,
    float bigWavesAmplitude,
    float smallWavesAmplitude,
    float& outWave,
    float& colorMult,
    float& glare,
    CVector& vecNormal
)
{
    x = std::abs(x);
    y = std::abs(y);
    vecNormal = CVector(0.f, 0.f, 1.f);

    const float waveMult = faWaveMultipliersX[(x / 2) % 8] * faWaveMultipliersY[(y / 2) % 8] * CWeather::Wavyness;
    float fX = (float)x, fY = (float)y;

    // literal AIDS code
    const auto CalculateWave = [&](int32 offset, float angularFreqX, float angularFreqY, float amplitude) {
        const float freqOffsetMult = TWO_PI / static_cast<float>(offset);
        const CVector2D waveVector{ TWO_PI * angularFreqX, TWO_PI * angularFreqY }; // w = angular frequency

        const auto step  = (CTimer::GetTimeInMS() - m_nWaterTimeOffset) % offset;
        const auto wavePhase = step * freqOffsetMult + fX * waveVector.x + fY * waveVector.y;

        const auto sinPhase = CMaths::GetSinFast(wavePhase);
        const auto cosPhase = CMaths::GetCosFast(wavePhase);
        outWave += sinPhase * waveMult * amplitude;

        // Wave normal calculation - seems broken but maybe R* just knows something that we don't :D
        // Normal generation is completely skipped on later releases of the game (android / definitive)
        switch (offset) {
        case 5000: {
            const auto normalDerivative = -cosPhase * waveMult * amplitude * waveVector.x;
            vecNormal += { normalDerivative, normalDerivative, 0.0f };
            break;
        }
        case 3500: {
            const auto normalDerivative = cosPhase * waveMult * amplitude * waveVector.x;
            vecNormal += { normalDerivative, normalDerivative, 0.0f };
            break;
        }
        case 3000: {
            const auto normalDerivative = cosPhase * waveMult * amplitude * (PI / 10.0f);
            vecNormal += { normalDerivative, 0.0f, 0.0f };
            break;
        }
        }
    };

    CalculateWave(5000, 1.f / 64.0f, 1.f / 64.0f, 2.0f * bigWavesAmplitude);
    CalculateWave(3500, 1.f / 26.0f, 1.f / 52.0f, 1.0f * smallWavesAmplitude);
    CalculateWave(3000, 0.0f,        1.f / 20.0f, 0.5f * smallWavesAmplitude);

    vecNormal.Normalise();
    const auto glareLevel = (vecNormal.x + vecNormal.y + vecNormal.z) * E_CONST;

    colorMult = std::max(glareLevel, 0.0f) * 0.65f + 0.27f;
    glare = std::clamp(8.0f * glareLevel - 5.0f, 0.0f, 0.99f) * CWeather::SunGlare;
}

// 0x6E5810
void CWaterLevel::MarkQuadsAndPolysToBeRendered(int32 blockX, int32 blockY, bool isInInterior) {
    using PType = PolyInfo::PType;

    // Horrible naming, sorry.
    const auto ProcessPoly = [&](PolyInfo data) {
        switch (data.Type()) {
        case PType::SINGLE_QUAD:
            WaterQuads[data.Id()].DoMarkToBeRendered(isInInterior);
            break;
        case PType::SINGLE_TRI:
            WaterTriangles[data.Id()].DoMarkToBeRendered(isInInterior);
            break;
        }
    };

    auto& blockPolyInfo = m_BlockPolyInfo[blockX][blockY];
    switch (blockPolyInfo.Type()) {
    case PType::SINGLE_QUAD:
    case PType::SINGLE_TRI:
        ProcessPoly(blockPolyInfo);
        break;
    case PType::COMBO: {
        for (auto& comboPoly : m_PolyCombos | rng::views::drop(blockPolyInfo.Id())) {
            if (comboPoly.Type() == PType::NONE) {
                break; // End of sequence
            }
            ProcessPoly(comboPoly);
        }
        break;
    }
    }
}

// 0x6E7210
// 0x6E7210
void CWaterLevel::CalculateWavesOnlyForCoordinate2( // TODO: Original name didn't have a 2 in it... I'm just lazy!
    int32 x, int32 y,
    float bigWavesAmpl,
    float smallWavesAmpl,
    float* pResultHeight
) {
    x = std::abs(x);
    y = std::abs(y);

    const float waveMult = faWaveMultipliersX[(x / 2) % 8] * faWaveMultipliersY[(y / 2) % 8] * CWeather::Wavyness;

    // literal AIDS code (Same as the one in `CalculateWavesOnlyForCoordinate`),
    // but only the height is calculated, the normal is not.
    const auto CalculateWave = [&](int32 offset, CVector2D waveVector, float amplitude) {
        const auto step  = (CTimer::GetTimeInMS() - m_nWaterTimeOffset) % offset;
        const auto phase = step * (TWO_PI / (float)offset) + (float)x * waveVector.x + (float)y * waveVector.y;

        *pResultHeight += CMaths::GetSinFast(phase) * waveMult * amplitude;
    };

    CalculateWave(5000, { TWO_PI * (1.0f / 64.0f), TWO_PI * (1.0f / 64.0f) }, 2.0f * bigWavesAmpl);
    CalculateWave(3500, { TWO_PI * (1.0f / 26.0f), TWO_PI * (1.0f / 52.0f) }, 1.0f * smallWavesAmpl);
    CalculateWave(3000, { 0.0f,                   TWO_PI * (1.0f / 20.0f) }, 0.5f * smallWavesAmpl);
}

// 0x6E6CA0
void CWaterLevel::BlockHit(int32 blockX, int32 blockY) {
    if (blockX >= 0 && blockX < NUM_WATER_BLOCKS_ROWCOL && blockY >= 0 && blockY < NUM_WATER_BLOCKS_ROWCOL) {
        MarkQuadsAndPolysToBeRendered(blockX, blockY, CGame::currArea != AREA_CODE_NORMAL_WORLD);
    }

    // Blocks at the edge of the world (index 0 and 11) need to be handled both ways, the quads and polys are to be rendered, but also the general ocean plane needs to be rendered on them
    if (blockX <= 0 || blockX >= (NUM_WATER_BLOCKS_ROWCOL - 1) || blockY <= 0 || blockY >= (NUM_WATER_BLOCKS_ROWCOL - 1)) {
        if (m_NumBlocksOutsideWorldToBeRendered < (uint32)m_MaxNumBlocksOutsideWorldToBeRendered) {
            const auto idx                         = m_NumBlocksOutsideWorldToBeRendered++;
            m_BlocksToBeRenderedOutsideWorldX[idx] = blockX;
            m_BlocksToBeRenderedOutsideWorldY[idx] = blockY;
        }
    }
}

// 0x6E6D10
void CWaterLevel::ScanThroughBlocks() {
    m_NumBlocksOutsideWorldToBeRendered = 0;

    const auto frustumPts = TheCamera.GetFrustumPoints();
    CVector2D scanPts[5]{};
    for (auto i = 0; i < 5; i++) {
        scanPts[i] = CVector2D{ frustumPts[i] } / (float)WATER_BLOCK_SIZE + CVector2D{6.f, 6.f};
    }
    CWorldScan::ScanWorld(scanPts, 5, BlockHit);
}

void CWaterLevel::RenderHighDetailWaterTriangle(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3) {
    plugin::CallAndReturn<void, 0x6EDDC0, int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar>(X1, Y1, P1, X2, Y2, P2, X3, Y3, P3);
}

/*!
* @addr notsa
* @brief Draw the vertices/indices that were pushed into the temporary Im3D buffers up until now
*
* @note The caller is responsible for clearing `uiTempBufferVerticesStored`/`uiTempBufferIndicesStored`
*/
void CWaterLevel::RenderAndEmptyRenderBuffer() {
    if (uiTempBufferVerticesStored) {
        LittleTest();
        if (RwIm3DTransform(TempBufferVertices.m_3d, uiTempBufferVerticesStored, nullptr, rwIM3D_VERTEXUV)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, uiTempBufferIndicesStored);
            RwIm3DEnd();
        }
    }
}

/*!
* @addr notsa
* @brief Push a sea bed vertex (A textured quad/rectangle at Z = -70)
*/
static void PushSeaBedVertex(uint32 idxInBuffer, float x, float y, float u, float v) {
    static const RwRGBA SEA_BED_COLOR = { 0x50, 0x50, 0x50, 0xFF }; // 0xFF505050

    auto* vtx = &TempBufferVertices.m_3d[idxInBuffer];
    const CVector pos{ x, y, -70.f };
    RxObjSpace3DVertexSetPos(vtx, &pos);
    RxObjSpace3DVertexSetPreLitColor(vtx, &SEA_BED_COLOR);
    RxObjSpace3DVertexSetU(vtx, u);
    RxObjSpace3DVertexSetV(vtx, v);
}

/*!
* @addr notsa
* @brief Push the indices for a quad (2 triangles) made out of 4 vertices starting at `firstVertex`
*/
static void PushSeaBedQuadIndices(RxVertexIndex firstVertex) {
    aTempBufferIndices[uiTempBufferIndicesStored + 0] = firstVertex + 0;
    aTempBufferIndices[uiTempBufferIndicesStored + 1] = firstVertex + 1;
    aTempBufferIndices[uiTempBufferIndicesStored + 2] = firstVertex + 2;
    aTempBufferIndices[uiTempBufferIndicesStored + 3] = firstVertex + 3;
    aTempBufferIndices[uiTempBufferIndicesStored + 4] = firstVertex + 1;
    aTempBufferIndices[uiTempBufferIndicesStored + 5] = firstVertex + 2;
    uiTempBufferVerticesStored += 4;
    uiTempBufferIndicesStored  += 6;
}

// 0x6E6870
void CWaterLevel::RenderSeaBedSegment(int32 blockX, int32 blockY, float x1, float x2, float y1, float y2) {
    // World space coordinates of the rectangle's corners
    const auto posX1 = ((float)blockX + x1) * (float)WATER_BLOCK_SIZE - 3000.f;
    const auto posX2 = ((float)blockX + x2) * (float)WATER_BLOCK_SIZE - 3000.f;
    const auto posY1 = ((float)blockY + y1) * (float)WATER_BLOCK_SIZE - 3000.f;
    const auto posY2 = ((float)blockY + y2) * (float)WATER_BLOCK_SIZE - 3000.f;

    // Texture coordinates (8 repeats per block - 0x859000 is `8.0f`)
    const auto u1 = x1 * 8.f, u2 = x2 * 8.f;
    const auto v1 = y1 * 8.f, v2 = y2 * 8.f;

    const auto startVtx = uiTempBufferVerticesStored;
    PushSeaBedVertex(startVtx + 0, posX1, posY1, u1, v1);
    PushSeaBedVertex(startVtx + 1, posX1, posY2, u1, v2);
    PushSeaBedVertex(startVtx + 2, posX2, posY1, u2, v1);
    PushSeaBedVertex(startVtx + 3, posX2, posY2, u2, v2);
    PushSeaBedQuadIndices((RxVertexIndex)startVtx);
}

// 0x6E6A10
void CWaterLevel::RenderDetailedSeaBedSegment(int32 blockX, int32 blockY, float x1, float x2, float y1, float y2) {
    // Number of cells the rectangle is subdivided into (4 => At most 125 units per cell)
    const auto numCellsX = std::max(1, (int32)((x2 - x1) * 4.f)); // 0x858B90 is `4.0f`
    const auto numCellsY = std::max(1, (int32)((y2 - y1) * 4.f));

    for (int32 ix = 0; ix < numCellsX; ix++) {
        const auto cellX1 = ((float)ix * (x2 - x1)) / (float)numCellsX + x1;
        const auto cellX2 = ((float)(ix + 1) * (x2 - x1)) / (float)numCellsX + x1;
        for (int32 iy = 0; iy < numCellsY; iy++) {
            const auto cellY1 = ((float)iy * (y2 - y1)) / (float)numCellsY + y1;
            const auto cellY2 = ((float)(iy + 1) * (y2 - y1)) / (float)numCellsY + y1;

            const auto posX1 = ((float)blockX + cellX1) * (float)WATER_BLOCK_SIZE - 3000.f;
            const auto posX2 = ((float)blockX + cellX2) * (float)WATER_BLOCK_SIZE - 3000.f;
            const auto posY1 = ((float)blockY + cellY1) * (float)WATER_BLOCK_SIZE - 3000.f;
            const auto posY2 = ((float)blockY + cellY2) * (float)WATER_BLOCK_SIZE - 3000.f;

            const auto u1 = cellX1 * 8.f, u2 = cellX2 * 8.f;
            const auto v1 = cellY1 * 8.f, v2 = cellY2 * 8.f;

            const auto startVtx = uiTempBufferVerticesStored;
            PushSeaBedVertex(startVtx + 0, posX1, posY1, u1, v1);
            PushSeaBedVertex(startVtx + 1, posX1, posY2, u1, v2);
            PushSeaBedVertex(startVtx + 2, posX2, posY1, u2, v1);
            PushSeaBedVertex(startVtx + 3, posX2, posY2, u2, v2);
            PushSeaBedQuadIndices((RxVertexIndex)startVtx);
        }
    }
}

// 0x6EF650
void CWaterLevel::RenderWater() {
    if (!CGame::CanSeeWaterFromCurrArea()) {
        return;
    }

    SetCameraRange();
    DefinedState();

    // Render the sea bed for all blocks outside the world
    {
        uiTempBufferVerticesStored = 0;
        uiTempBufferIndicesStored  = 0;

        RwRenderStateSet(rwRENDERSTATETEXTURERASTER,        RWRSTATE(seabd32Raster));
        RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATESRCBLEND,             RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,            RWRSTATE(rwBLENDINVSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(0));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));

        const auto camPos = CVector2D{ TheCamera.GetPosition() };
        for (uint32 i = 0; i < m_NumBlocksOutsideWorldToBeRendered; i++) {
            const auto blockX = m_BlocksToBeRenderedOutsideWorldX[i];
            const auto blockY = m_BlocksToBeRenderedOutsideWorldY[i];

            // Only blocks close enough to the camera get their sea bed subdivided
            const bool renderDetailed = (camPos - CVector2D{
                ((float)blockX + 0.5f) * (float)WATER_BLOCK_SIZE - 3000.f,
                ((float)blockY + 0.5f) * (float)WATER_BLOCK_SIZE - 3000.f
            }).Magnitude() < m_fSeaBedDetailedDist;

            const auto RenderPart = [&](float fx1, float fx2, float fy1, float fy2) {
                if (renderDetailed) {
                    RenderDetailedSeaBedSegment(blockX, blockY, fx1, fx2, fy1, fy2);
                } else {
                    RenderSeaBedSegment(blockX, blockY, fx1, fx2, fy1, fy2);
                }
            };

            if (blockX < 0 || blockX >= NUM_WATER_BLOCKS_ROWCOL || blockY < 0 || blockY >= NUM_WATER_BLOCKS_ROWCOL) {
                // The whole block lies outside the world
                RenderPart(0.f, 1.f, 0.f, 1.f);
            } else {
                // These blocks are only partially outside the world, so only the outer strip needs its sea bed rendered
                const bool renderX = blockX == 0 || blockX == NUM_WATER_BLOCKS_ROWCOL - 1;
                if (renderX) {
                    RenderPart(
                        blockX == 0 ? 0.f : 0.96f,
                        blockX == 0 ? 0.04f : 1.f,
                        0.f,
                        1.f
                    );
                }
                if (blockY == 0 || blockY == NUM_WATER_BLOCKS_ROWCOL - 1) {
                    RenderPart(
                        0.f,
                        1.f,
                        blockY == 0 ? 0.f : 0.96f,
                        blockY == 0 ? 0.04f : 1.f
                    );
                }
            }
        }
        RenderAndEmptyRenderBuffer();
    }

    // Render the water polygons
    uiTempBufferVerticesStored = 0;
    uiTempBufferIndicesStored  = 0;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,   RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(m_WaterTextureAddressMode));

    // Update the water flow based texture scroll
    const auto flowStepX = CTimer::ms_fTimeStep * m_CurrentFlow.x * m_fWaterFlowShiftScale;
    const auto flowStepY = CTimer::ms_fTimeStep * m_CurrentFlow.y * m_fWaterFlowShiftScale;

    const auto Scroll = [](float& curr, float add) {
        curr += add;
        if (curr >= 1.f) {
            curr -= 1.f;
        }
    };
    Scroll(m_fWaterScrollSecondU, 0.08f * flowStepX); // 0x859018 is `0.08f`
    Scroll(m_fWaterScrollSecondV, 0.08f * flowStepY);
    Scroll(m_fWaterScrollFirstU, flowStepX * m_fWaterFlowShiftScale);
    Scroll(m_fWaterScrollFirstV, flowStepY * m_fWaterFlowShiftScale);

    // Update the texture shifts
    const auto timeInMS = CTimer::m_snTimeInMilliseconds;
    const auto angleSecond = (float)(timeInMS & 0xFFF) * (TWO_PI / 4096.f);  // 0x872174
    TextureShiftSecondU = std::sin(angleSecond) * CWeather::Wavyness * 0.08f + m_fWaterScrollSecondU;
    TextureShiftSecondV = std::cos(angleSecond) * CWeather::Wavyness * 0.08f + m_fWaterScrollSecondV;

    const auto angleFirst = (float)(timeInMS & 0x1FFF) * (TWO_PI / 8192.f); // 0x872170
    TextureShiftFirstU = m_fWaterScrollFirstU;
    TextureShiftFirstV = std::cos(angleFirst) * 0.024f + m_fWaterScrollFirstV; // 0x87216C is `0.024f`

    // The high detail water's texture is additionally jittered (0x858C7C is `1.f / 32768.f`)
    const auto shiftJitter = [] { return (float)rand() * (1.f / 32768.f) * m_fWaterTextureShiftJitter; };
    m_fHighDetailTextureShiftU = std::sin(angleFirst) * m_fHighDetailTextureShiftAmp + shiftJitter();
    m_fHighDetailTextureShiftV = std::cos(angleFirst) * m_fHighDetailTextureShiftAmp + shiftJitter();

    // Set the water color from the current time cycle's water colors
    WaterColor.r = (uint8)CTimeCycle::GetWaterRed();
    WaterColor.g = (uint8)CTimeCycle::GetWaterGreen();
    WaterColor.b = (uint8)CTimeCycle::GetWaterBlue();
    WaterColorTriangle = WaterColor;

    WaterLayerAlpha[1] = (uint32)(int32)(CTimeCycle::GetWaterAlpha() * 0.5f);
    WaterLayerAlpha[0] = std::min<uint32>(0xFF, (WaterLayerAlpha[1] << 8) / (0x100 - WaterLayerAlpha[1]));

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(waterclear256Raster));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,     RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDINVSRCALPHA));

    // Render the marked water triangles
    for (uint32 i = 0; i < NumWaterTriangles; i++) {
        auto& tri = WaterTriangles[i];
        if (!tri.bToBeRendered) {
            continue;
        }
        RenderWaterTriangle(
            m_aVertices[tri.verts[0]].x, m_aVertices[tri.verts[0]].y, m_aVertices[tri.verts[0]].rp,
            m_aVertices[tri.verts[1]].x, m_aVertices[tri.verts[1]].y, m_aVertices[tri.verts[1]].rp,
            m_aVertices[tri.verts[2]].x, m_aVertices[tri.verts[2]].y, m_aVertices[tri.verts[2]].rp
        );
        tri.bToBeRendered = false;
    }

    // Render the marked water quads
    for (uint32 i = 0; i < NumWaterQuads; i++) {
        auto& quad = WaterQuads[i];
        if (!quad.bToBeRendered) {
            continue;
        }

        if (m_bRandomizeWaterColor) {
            WaterColor = CRGBA{
                (uint8)((i & 0xF) * 0.0625f * 255.f),         // 0x858620 is `0.0625f`, 0x859AAC is `255.f`
                (uint8)(((i >> 4) & 0xF) * 0.0625f * 255.f),
                (uint8)(((i >> 8) & 0xF) * 0.0625f * 255.f),
                (uint8)CTimeCycle::GetWaterAlpha()
            };
            WaterColorTriangle = WaterColor;
        }

        RenderWaterRectangle(
            m_aVertices[quad.verts[0]].x, m_aVertices[quad.verts[1]].x,
            m_aVertices[quad.verts[0]].y, m_aVertices[quad.verts[2]].y,
            m_aVertices[quad.verts[0]].rp, m_aVertices[quad.verts[1]].rp,
            m_aVertices[quad.verts[2]].rp, m_aVertices[quad.verts[3]].rp
        );
        quad.bToBeRendered = false;
    }

    // Render the ocean plane for the blocks outside the world
    for (uint32 i = 0; i < m_NumBlocksOutsideWorldToBeRendered; i++) {
        const auto blockX = m_BlocksToBeRenderedOutsideWorldX[i];
        const auto blockY = m_BlocksToBeRenderedOutsideWorldY[i];
        if (blockX < 0 || blockX >= NUM_WATER_BLOCKS_ROWCOL || blockY < 0 || blockY >= NUM_WATER_BLOCKS_ROWCOL) {
            const CRenPar P{ 0.f, 1.f, 0.f, 0, 0 }; // Plain water surface at Z = 0, no waves, no flow
            RenderWaterRectangle(
                blockX * WATER_BLOCK_SIZE - 3000, (blockX + 1) * WATER_BLOCK_SIZE - 3000,
                blockY * WATER_BLOCK_SIZE - 3000, (blockY + 1) * WATER_BLOCK_SIZE - 3000,
                P, P, P, P
            );
        }
    }

    RenderAndEmptyRenderBuffer();
    uiTempBufferVerticesStored = 0;
    uiTempBufferIndicesStored  = 0;

    RenderBoatWakes();
    DefinedState();
}

void CWaterLevel::SyncWater() {
    m_nWaterTimeOffset = CTimer::GetTimeInMS();
}

// NOTSA
bool CWaterLevel::IsPointUnderwaterNoWaves(const CVector& point) {
    float level{};
    if (GetWaterLevelNoWaves(point, &level, nullptr, nullptr))
        return level > point.z;
    return false;
}

bool CWaterLevel::GetWaterLevel(const CVector& pos, float& outWaterLevel, bool touchingWater, CVector* normals) {
    return GetWaterLevel(pos.x, pos.y, pos.z, outWaterLevel, touchingWater, normals);
}

// 0x6E5A40
uint32 CWaterLevel::AddWaterLevelVertex(int32 X, int32 Y, CRenPar P) {
    // Make sure point is inside world bounds
    if (CVector2D pt{ (float)X, (float)Y }; WORLD_BOUNDS.DoConstrainPoint(pt)) {
        X = (int32)pt.x;
        Y = (int32)pt.y;

        P = {};
    }

    // Try finding a vertex with the same coords, and use that
    for (auto&& [id, vtx] : rngv::enumerate(m_aVertices | rng::views::take(NumWaterVertices))) {
        if (vtx.x == X && vtx.y == Y && vtx.rp.z == P.z) {
            return id;
        }
    }

    const auto idx = NumWaterVertices++;
    m_aVertices[idx] = { (int16)X, (int16)Y, P };
    return idx;
}

struct SortableVtx {
    SortableVtx(int32 x, int32 y, const CRenPar& rp) :
        idx{ CWaterLevel::AddWaterLevelVertex(x, y, rp) },
        x{ CWaterLevel::m_aVertices[idx].x },
        y{ CWaterLevel::m_aVertices[idx].y }
    {
    }

    uint32 idx;
    int32  x, y;
};

//! NOTSA
//! Sort vertices in clockwise order (With a few assumptions)
template<size_t N>
auto DoVtxSortAndGetRange(SortableVtx (&verts)[N]) {
    const auto VertexComparator = [&](SortableVtx& a, SortableVtx& b) {
        if (a.y == b.y) {
            return a.x < b.x; // Sort by x if y is the same
        }
        return a.y < b.y; // Otherwise, sort by y
    };

    rng::sort(verts, VertexComparator);

    // Return a range of vertex indices that can be passed to the constructor of `CWaterPolygon`
    return verts | rng::views::transform([](auto& vtx) {
        return vtx.idx;
    });
}

// 0x6E7EF0
void CWaterLevel::AddWaterLevelQuad(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3, int32 X4, int32 Y4, CRenPar P4, uint32 Flags) {
    if ((X1 == X2 && X1 == X3 && X1 == X4) || (Y1 == Y2 && Y1 == Y3 && Y1 == Y4)) {
        return;
    }

    // Seemingly only axis aligned rectangles can be used as quads
    // NOTSA: to verify the above.
    assert(X1 == X2 || X1 == X3 || X1 == X4 || X2 == X3 || X2 == X4 || X3 == X4);
    assert(Y1 == Y2 || Y1 == Y3 || Y1 == Y4 || Y2 == Y3 || Y2 == Y4 || Y3 == Y4);

    SortableVtx verts[]{
        {X1,  Y1, P1},
        { X2, Y2, P2},
        { X3, Y3, P3},
        { X4, Y4, P4},
    };

    // Now actually create the quad
    WaterQuads[NumWaterQuads++] = CWaterQuad{
        (Flags & 1) == 0,
        (Flags & 2) != 0,
        DoVtxSortAndGetRange(verts)
    };
}

// 0x6E7D40
void CWaterLevel::AddWaterLevelTriangle(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3, uint32 Flags) {
    if ((X1 == X2 && X1 == X3) || (Y1 == Y2 && Y1 == Y3)) {
        return;
    }

    // Sorting seemingly always cares about only 2 of 3 vertices, looking at water.dat, in every single case 2 of 3 vertices have the same y coordinate,
    // I assume that's a limitation, and at least 2 of 3 vertices need to fall on the same x/y axis.
    // NOTSA: to verify the above.
    assert(X1 == X2 || X1 == X3 || X2 == X3);
    assert(Y1 == Y2 || Y1 == Y3 || Y2 == Y3);

    SortableVtx verts[]{
        {X1,  Y1, P1},
        { X2, Y2, P2},
        { X3, Y3, P3},
    };

    int16_t indices[3];
    if (verts[0].y == verts[1].y) {
        if (verts[0].x < verts[1].x) {
            indices[0] = verts[0].idx;
            indices[1] = verts[1].idx;
        } else {
            indices[0] = verts[1].idx;
            indices[1] = verts[0].idx;
        }
        indices[2] = verts[2].idx;
    } else if (verts[0].y == verts[2].y) {
        if (verts[0].x >= verts[2].x) {
            indices[0] = verts[2].idx;
            indices[1] = verts[0].idx;
        } else {
            indices[0] = verts[0].idx;
            indices[1] = verts[2].idx;
        }
        indices[2] = verts[1].idx;
    } else {
        if (verts[1].x >= verts[2].x) {
            indices[0] = verts[2].idx;
            indices[1] = verts[1].idx;
        } else {
            indices[0] = verts[1].idx;
            indices[1] = verts[2].idx;
        }
        indices[2] = verts[0].idx;
    }

    // Now actually create the triangle
    WaterTriangles[NumWaterTriangles++] = CWaterTriangle{
        (Flags & 1) == 0,
        (Flags & 2) != 0,
        indices | rng::views::all
    };
}

// 0x6E7B30
void CWaterLevel::FillQuadsAndTrianglesList() {
    for (int32 blockX = 0; blockX < NUM_WATER_BLOCKS_ROWCOL; blockX++) {
        const auto minX = (float)(blockX * WATER_BLOCK_SIZE) - 3000.0f; // Left edge of this block
        const auto maxX = minX + (float)WATER_BLOCK_SIZE;

        for (int32 blockY = 0; blockY < NUM_WATER_BLOCKS_ROWCOL; blockY++) {
            const auto minY = (float)(blockY * WATER_BLOCK_SIZE) - 3000.0f; // Bottom edge of this block
            const auto maxY = minY + (float)WATER_BLOCK_SIZE;

            // Quads
            for (auto i = 0u; i < NumWaterQuads; i++) {
                const auto& quad = WaterQuads[i];
                if (minX < (float)m_aVertices[quad.verts[1]].x && (float)m_aVertices[quad.verts[0]].x < maxX
                 && minY < (float)m_aVertices[quad.verts[2]].y && (float)m_aVertices[quad.verts[0]].y < maxY
                ) {
                    AddToQuadsAndTrianglesList(blockX, blockY, (int32)i, 1);
                }
            }

            // Triangles
            for (auto i = 0u; i < NumWaterTriangles; i++) {
                const auto& tri = WaterTriangles[i];
                const auto  y0  = m_aVertices[tri.verts[0]].y;
                const auto  y1  = m_aVertices[tri.verts[1]].y;
                if (minX < (float)m_aVertices[tri.verts[1]].x && (float)m_aVertices[tri.verts[0]].x < maxX
                 && minY < (float)std::max(y0, y1) && (float)std::min(y0, y1) < maxY
                ) {
                    AddToQuadsAndTrianglesList(blockX, blockY, (int32)i, 2);
                }
            }
        }
    }
}

// 0x6E9C80
void CWaterLevel::SetCameraRange() {
    if (DontUpdateCameraRange) {
        return;
    }

    const auto& cmpos = TheCamera.GetPosition();

    const auto CalcMin = [](float p) { return 2 * (int32)std::floor((p - (float)DETAILEDWATERDIST) / 2.f); };
    const auto CalcMax = [](float p) { return 2 * (int32)std::ceil((p + (float)DETAILEDWATERDIST) / 2.f); };

    CameraRangeMinX = CalcMin(cmpos.x);
    CameraRangeMaxX = CalcMax(cmpos.x);

    CameraRangeMinY = CalcMin(cmpos.y);
    CameraRangeMaxY = CalcMax(cmpos.y);
}

// 0x6EAB50
void CWaterLevel::HandleBeachToysStuff() {
    /* nothing special (10 lines), but it uses 3 static variables, and they aren't used anywhere else, so I won't bother */
}

template<size_t NumVerts>
CWaterVertex CWaterPolygon<NumVerts>::GetVertex(uint16 idx) const {
    return CWaterLevel::m_aVertices[verts[idx]];
}
