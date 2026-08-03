/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Rect.h"
#include "RGBA.h"
#include "Vector2D.h"
#include "RenderWare.h"

#define NUM_SPRITE_VERTICES 8

class CSprite2d {
public:
    RwTexture* m_pTexture;

    static inline auto& nextBufferIndex = StaticRef<int32>(0xC80458);
    static inline auto& nextBufferVertex = StaticRef<int32>(0xC8045C);
    static inline auto& NearScreenZ = StaticRef<float>(0xC80460);
    static inline auto& RecipNearClip = StaticRef<float>(0xC80464);
    static inline auto& maVertices = StaticRef<std::array<RwIm2DVertex, 8>>(0xC80468);

public:
    static void InjectHooks();

    CSprite2d();
    CSprite2d* Constructor();

    ~CSprite2d();
    CSprite2d* Destructor();

    void Delete();

    void SetTexture(Const char* name);
    void SetTexture(Const char* name, Const char* maskName);
    void SetAddressingUV(RwTextureAddressMode modeU, RwTextureAddressMode modeV);
    void SetAddressing(RwTextureAddressMode modeUV);
    void SetRenderState();

    void Draw(float x, float y, float width, float height, const CRGBA& color);
    void Draw(const CRect& posn, const CRGBA& color);
    void DrawWithBilinearOffset(const CRect& posn, const CRGBA& color);
    void Draw(const CRect& posn, const CRGBA& color, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4);
    void Draw(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4);
    void Draw(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, const CRGBA& color);

    static void SetRecipNearClip();
    static void InitPerFrame();
    static bool IsVertexBufferEmpty();
    static bool IsVertexBufferFull();
    static void RenderVertexBuffer();
    static void SetVertices(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4);
    static void SetVertices(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4);
    static void SetVertices(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4);
    static void SetVertices(int32 numVerts, const CVector2D* posn, const CVector2D* texCoors, const CRGBA& color);
    static void SetVertices(int32 numVerts, const CVector2D* posn, const CRGBA& color);
    static void SetMaskVertices(int32 numVerts, const CVector2D* posn, float depth);
    static void SetVertices(RwD3D9Vertex* vertices, const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4);
    static void DrawRect(const CRect& posn, const CRGBA& color);
    static void DrawTxRect(const CRect& posn, const CRGBA& color);
    static void DrawRect(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4);
    static void DrawRectXLU(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4);
    static void DrawAnyRect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4);
    static void DrawCircleAtNearClip(const CVector2D& posn, float size, const CRGBA& color, int32 angle);
    static void SetVerticesForSniper(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4);
    static void OffsetTexCoordForBilinearFiltering(float width, float height);
    static void AddToBuffer(const CRect& posn, const CRGBA& color, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4);
    static void Draw2DPolygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, const CRGBA& color);
    static void DrawBarChart(float x, float y, uint16 width, uint8 height, float progress, int8 progressAdd, uint8 drawPercentage, uint8 drawBlackBorder, CRGBA color, CRGBA addColor);

    static auto* GetVertices()      { return maVertices.data(); }
    static auto& GetNearScreenZ()   { return NearScreenZ; }
    static auto& GetRecipNearClip() { return RecipNearClip; }
};

VALIDATE_SIZE(CSprite2d, 4);
