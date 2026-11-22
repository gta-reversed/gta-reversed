#pragma once

#include "rwcore.h"

// III/VC Leftover
class CMBlur {
public:
    static RwRaster* pFrontBuffer;
    static bool ms_bJustInitialised;
    static bool ms_bScaledBlur;
    static bool BlurOn;
    static inline auto& Drunkness = StaticRef<float>(0xC73C58);

    static int32 pBufVertCount;

public:
    CMBlur();

    static bool MotionBlurOpen(RwCamera* cam);
    static bool MotionBlurClose();
    static void MotionBlurRender(RwCamera* cam, uint32 red, uint32 green, uint32 blue, uint32 blur, int32 type, uint32 blurAlpha);

    static void CreateImmediateModeData(RwCamera* cam, RwRect* rect);
    static void CreateImmediateModeData(RwCamera* cam, RwRect* rect, RwIm2DVertex* verts, RwRGBA color, float u1Off, float v1Off, float u2Off, float v2Off, float z, int32 fullTexture);
    static void OverlayRender(RwCamera* cam, RwRaster* raster, RwRGBA color, int32 type, int32 blurAlpha);

    static void SetDrunkBlur(float drunkness);
    static void ClearDrunkBlur();

    static bool PosInside(RwRect* rect, float x1, float y1, float x2, float y2);
    static bool AddRenderFx(RwCamera* cam, RwRect* rect, float z, int type);
    static void OverlayRenderFx(RwCamera* cam, RwRaster* frontBuf);
};
