#pragma once

#include "RenderWare.h"
#include "RGBA.h"

class CSprite {
public:
    static inline float& m_f2DNearScreenZ      = *(float*)0xC4B8D8;
    static inline float& m_f2DFarScreenZ       = *(float*)0xC4B8D4;
    static inline float& m_fRecipNearClipPlane = *(float*)0xC4B8D0;
    static inline uint32& m_bFlushSpriteBufferSwitchZTest = *(uint32*)0xC6A15C;

public:
    static void InjectHooks();

    static void Initialise();
    static void InitSpriteBuffer();
    static void InitSpriteBuffer2D();
    static void FlushSpriteBuffer();

    static void Draw3DSprite(float, float, float, float, float, float, float, float, float);

    static bool  CalcScreenCoors(const RwV3d& posn, RwV3d* out, float* w, float* h, bool checkMaxVisible, bool checkMinVisible);
    static float CalcHorizonCoors();

    static void Set4Vertices2D(RwIm2DVertex* verts, const CRect& rt, const CRGBA& topLeftColor, const CRGBA& topRightColor, const CRGBA& bottomLeftColor, const CRGBA& bottomRightColor);
    static void Set4Vertices2D(RwIm2DVertex* verts, float, float, float, float, float, float, float, float, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&);

    static void RenderOneXLUSprite(CVector pos, CVector2D halfSize, uint8 r, uint8 g, uint8 b, int16 intensity, float rhw, uint8 a, uint8 udir, uint8 vdir);
    static void RenderOneXLUSprite_Triangle(float, float, float, float, float, float, float, uint8, uint8, uint8, int16, float, uint8);
    static void RenderOneXLUSprite_Rotate_Aspect(CVector pos, CVector2D size, uint8 r, uint8 g, uint8 b, int16 intensity, float rz, float rotation, uint8 alpha);
    static void RenderOneXLUSprite_Rotate_Dimension(float, float, float, float, float, uint8, uint8, uint8, int16, float, float, uint8);
    static void RenderOneXLUSprite_Rotate_2Colours(float, float, float, float, float, uint8, uint8, uint8, uint8, uint8, uint8, float, float, float, float, uint8);
    static void RenderOneXLUSprite2D(CVector2D screen, CVector2D size, const CRGBA& color, int16 intensity, uint8 alpha);
    static void RenderOneXLUSprite2D_Rotate_Dimension(float, float, float, float, const RwRGBA&, int16, float, uint8);

    static void RenderBufferedOneXLUSprite(CVector pos, CVector2D size, uint8 r, uint8 g, uint8 b, int16 intensity, float recipNearZ, uint8 a11);
    static void RenderBufferedOneXLUSprite_Rotate_Aspect(float x, float y, float z, float w, float h, uint8 r, uint8 g, uint8 b, int16 intensity, float recipNearZ, float angle, uint8 a12);
    static void RenderBufferedOneXLUSprite_Rotate_Dimension(CVector pos, CVector2D size, uint8 r, uint8 g, uint8 b, int16 intensity, float rz, float rotation, uint8 a);
    static void RenderBufferedOneXLUSprite_Rotate_2Colours(float, float, float, float, float, uint8, uint8, uint8, uint8, uint8, uint8, float, float, float, float, uint8);
    static void RenderBufferedOneXLUSprite2D(CVector2D pos, CVector2D size, const RwRGBA& color, int16 intensity, uint8 alpha);
    static void RenderBufferedOneXLUSprite2D_Rotate_Dimension(float, float, float, float, const RwRGBA&, int16, float, uint8);
};
