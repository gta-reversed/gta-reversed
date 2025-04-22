#include "StdInc.h"
#include "WaterLevel.h"
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
    RH_ScopedGlobalInstall(FindNearestWaterAndItsFlow, 0x6E9D70, { .reversed = false });
    RH_ScopedGlobalInstall(GetWaterLevelNoWaves, 0x6E8580, { .reversed = false });
    RH_ScopedGlobalInstall(RenderWaterFog, 0x6E7760, { .reversed = false });
    RH_ScopedGlobalInstall(CalculateWavesOnlyForCoordinate, 0x6E6EF0);
    RH_ScopedGlobalInstall(RenderWater, 0x6EF650, { .reversed = false });
    RH_ScopedGlobalInstall(AddWaveToResult, 0x6E81E0, { .reversed = false });
    RH_ScopedGlobalInstall(SetCameraRange, 0x6E9C80);
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
    NumWaterTriangles = 0;
    NumWaterQuads = 0;
    NumWaterVertices = 0;
    NumWaterZonePolys = 0;
    
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
void CWaterLevel::AddWaveToResult(float x, float y, float* pfWaterLevel, float fUnkn1, float fUnkn2, CVector* pVecNormal)
{
    plugin::Call<0x6E81E0, float, float, float*, float, float, CVector*>(x, y, pfWaterLevel, fUnkn1, fUnkn2, pVecNormal);
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

// 0x6E9D70
void CWaterLevel::FindNearestWaterAndItsFlow() {
    plugin::Call<0x6E9D70>();
}

// 0x6E8580
bool CWaterLevel::GetWaterLevelNoWaves(CVector pos, float * pOutWaterLevel, float * pOutBigWaves, float * pOutSmallWaves) {
    return plugin::CallAndReturn<bool, 0x6E8580, CVector, float *, float *, float *>(pos, pOutWaterLevel, pOutBigWaves, pOutSmallWaves);
}

bool CWaterLevel::GetWaterDepth(const CVector& vecPos, float* pOutWaterDepth, float* pOutWaterLevel, float* pOutGroundLevel)
{
    return plugin::CallAndReturn<bool, 0x6EA960, const CVector&, float*, float*, float*>
        (vecPos, pOutWaterDepth, pOutWaterLevel, pOutGroundLevel);
}

// 0x6E7760
void CWaterLevel::RenderWaterFog() {
    ZoneScoped;

    plugin::Call<0x6E7760>();
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
void CWaterLevel::CalculateWavesOnlyForCoordinate2( // TODO: Original name didn't have a 2 in it... I'm just lazy!
    int32 x, int32 y,
    float* pResultHeight,
    float bigWavesAmpl,
    float smallWavesAmpl
) {
    plugin::Call<0x6E7210>(x, y, bigWavesAmpl, smallWavesAmpl, pResultHeight);
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

// 0x6EF650
void CWaterLevel::RenderWater() {
    plugin::Call<0x6EF650>();
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

void CWaterLevel::FillQuadsAndTrianglesList() {
    plugin::Call<0x6E7B30>();
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
