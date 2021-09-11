/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"

class CSprite {
public:
    static float& m_f2DNearScreenZ;
    static float& m_f2DFarScreenZ;
    static float& m_fRecipNearClipPlane;

public:
    static void InjectHooks();

    static void Initialise();
    static void InitSpriteBuffer();
    static void InitSpriteBuffer2D();
    static void FlushSpriteBuffer();

    static void Draw3DSprite(float, float, float, float, float, float, float, float, float);

    static bool  CalcScreenCoors(const RwV3d& posn, RwV3d* out, float* w, float* h, bool checkMaxVisible, bool checkMinVisible);
    static float CalcHorizonCoors();

    static void Set4Vertices2D(RwD3D9Vertex*, const CRect&, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&);
    static void Set4Vertices2D(RwD3D9Vertex*, float, float, float, float, float, float, float, float, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&);

    static void RenderOneXLUSprite(float x, float y, float z, float halfWidth, float halfHeight, uint8 r, uint8 g, uint8 b, int16 a, float rhw, uint8 intensity, uint8 udir, uint8 vdir);
    static void RenderOneXLUSprite_Triangle(float, float, float, float, float, float, float, uint8, uint8, uint8, int16, float, uint8);
    static void RenderOneXLUSprite_Rotate_Aspect(float, float, float, float, float, uint8, uint8, uint8, int16, float, float, uint8);
    static void RenderOneXLUSprite_Rotate_Dimension(float, float, float, float, float, uint8, uint8, uint8, int16, float, float, uint8);
    static void RenderOneXLUSprite_Rotate_2Colours(float, float, float, float, float, uint8, uint8, uint8, uint8, uint8, uint8, float, float, float, float, uint8);
    static void RenderOneXLUSprite2D(float, float, float, float, const RwRGBA&, int16, uint8);
    static void RenderOneXLUSprite2D_Rotate_Dimension(float, float, float, float, const RwRGBA&, int16, float, uint8);

    static void RenderBufferedOneXLUSprite(float x, float y, float z, float w, float h, uint8 r, uint8 g, uint8 b, int16 intensity, float recipNearZ, uint8 a11);
    static void RenderBufferedOneXLUSprite_Rotate_Aspect(float x, float y, float z, float w, float h, uint8 r, uint8 g, uint8 b, int16 intensity, float recipNearZ, float angle, uint8 a12);
    static void RenderBufferedOneXLUSprite_Rotate_Dimension(float, float, float, float, float, uint8, uint8, uint8, int16, float, float, uint8);
    static void RenderBufferedOneXLUSprite_Rotate_2Colours(float, float, float, float, float, uint8, uint8, uint8, uint8, uint8, uint8, float, float, float, float, uint8);
    static void RenderBufferedOneXLUSprite2D(float, float, float, float, const RwRGBA&, int16, uint8);
    static void RenderBufferedOneXLUSprite2D_Rotate_Dimension(float, float, float, float, const RwRGBA&, int16, float, uint8);
};