/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Sprite2d.h"
#include "GxtChar.h"

void CSprite2d::InjectHooks() {
    RH_ScopedClass(CSprite2d);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x727230);
    RH_ScopedInstall(Destructor, 0x7281E0);

    RH_ScopedInstall(Delete, 0x727240);
    RH_ScopedOverloadedInstall(SetTexture, "", 0x727270, void(CSprite2d::*)(const char*));
    RH_ScopedOverloadedInstall(SetTexture, "mask", 0x7272B0, void(CSprite2d::*)(const char*, const char*));
    RH_ScopedInstall(SetAddressingUV, 0x7272E0);
    RH_ScopedInstall(SetAddressing, 0x727320);
    RH_ScopedInstall(SetRenderState, 0x727B30);
    RH_ScopedOverloadedInstall(Draw, "ffffCRGBA", 0x7282C0, void(CSprite2d::*)(float, float, float, float, const CRGBA&));
    RH_ScopedOverloadedInstall(Draw, "CRectCRGBA", 0x728350, void(CSprite2d::*)(const CRect&, const CRGBA&));
    RH_ScopedInstall(DrawWithBilinearOffset, 0x7283B0);
    RH_ScopedOverloadedInstall(Draw, "CRectCRGBA4", 0x7284B0, void(CSprite2d::*)(const CRect&, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&));
    RH_ScopedOverloadedInstall(Draw, "ffffffffCRGBA", 0x728520, void(CSprite2d::*)(float, float, float, float, float, float, float, float, const CRGBA&));

    RH_ScopedInstall(SetRecipNearClip, 0x727260);
    RH_ScopedInstall(InitPerFrame, 0x727350);
    RH_ScopedInstall(IsVertexBufferEmpty, 0x727390);
    RH_ScopedInstall(IsVertexBufferFull, 0x7273A0);
    RH_ScopedInstall(RenderVertexBuffer, 0x7273D0);
    RH_ScopedOverloadedInstall(SetVertices, "CRectCRGBA4", 0x727420, void(*)(const CRect&, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&));
    RH_ScopedOverloadedInstall(SetVertices, "ffffffffCRGBA", 0x727590, void(*)(float, float, float, float, float, float, float, float, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&));
    RH_ScopedOverloadedInstall(SetVertices, "CRectCRGBA4ffffffff", 0x727710, void(*)(const CRect&, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&, float, float, float, float, float, float, float, float));
    RH_ScopedOverloadedInstall(SetVertices, "iffCRGBA", 0x727890, void(*)(int32, const CVector2D*, const CVector2D*, const CRGBA&));
    RH_ScopedOverloadedInstall(SetVertices, "ifCRGBA", 0x727920, void(*)(int32, const CVector2D*, const CRGBA&));
    RH_ScopedInstall(SetMaskVertices, 0x7279B0);
    RH_ScopedOverloadedInstall(SetVertices, "RwD3D9Vertex", 0x727A00, void(*)(RwD3D9Vertex*, const CRect&, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&, float, float, float, float, float, float, float, float));
    RH_ScopedOverloadedInstall(DrawRect, "CRGBA", 0x727B60, void(*)(const CRect&, const CRGBA&));
    RH_ScopedInstall(DrawTxRect, 0x727BE0);
    RH_ScopedOverloadedInstall(DrawRect, "CRGBA4", 0x727C10, void(*)(const CRect&, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&));
    RH_ScopedInstall(DrawRectXLU, 0x727C50);
    RH_ScopedInstall(DrawAnyRect, 0x727CC0);
    RH_ScopedInstall(Draw2DPolygon, 0x7285B0);
    RH_ScopedInstall(DrawBarChart, 0x728640);
    RH_ScopedInstall(DrawCircleAtNearClip, 0x727D60, { .reversed = false });
}

CSprite2d::CSprite2d()
{
    m_pTexture = nullptr;
}
CSprite2d* CSprite2d::Constructor()
{
    this->CSprite2d::CSprite2d();
    return this;
}

CSprite2d::~CSprite2d() {
    Delete();
}
CSprite2d* CSprite2d::Destructor()
{
    this->CSprite2d::~CSprite2d();
    return this;
}

// delete this sprite (similar to destructor)
// 0x727240
void CSprite2d::Delete()
{
    if (m_pTexture) {
        RwTextureDestroy(m_pTexture);
        m_pTexture = nullptr;
    }
}

// set texture by name from current txd
// 0x727270
void CSprite2d::SetTexture(Const char* name)
{
    Delete();
    if (name)
        m_pTexture = RwTextureRead(name, nullptr);
}

// set texture by name from current txd (+alpha mask name)
// 0x7272B0
void CSprite2d::SetTexture(Const char* name, Const char* maskName)
{
    if (name && maskName)
        m_pTexture = RwTextureRead(name, maskName);
}

// 0x7272E0
void CSprite2d::SetAddressingUV(RwTextureAddressMode modeU, RwTextureAddressMode modeV)
{
    if (m_pTexture) {
        RwTextureSetAddressingU(m_pTexture, modeU);
        RwTextureSetAddressingV(m_pTexture, modeV);
    }
}

// 0x727320
void CSprite2d::SetAddressing(RwTextureAddressMode modeUV)
{
    if (m_pTexture)
        RwTextureSetAddressing(m_pTexture, modeUV);
}

// Sets sprite texture as current for device rendering
// 0x727B30
void CSprite2d::SetRenderState()
{
    if (m_pTexture)
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(m_pTexture)));
    else
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
}

// Draw this sprite
void CSprite2d::Draw(float x, float y, float width, float height, const CRGBA& color)
{
    CRect posn(x, y, x + width, y + height);
    Draw(posn, color);
}

void CSprite2d::Draw(const CRect& posn, const CRGBA& color)
{
    Draw(posn, color, color, color, color);
}

// 0x7283B0
void CSprite2d::DrawWithBilinearOffset(const CRect& posn, const CRGBA& color)
{
    SetVertices(posn, color, color, color, color);
    RwRaster* raster = RwTextureGetRaster(m_pTexture);
    OffsetTexCoordForBilinearFiltering(static_cast<float>(RwRasterGetWidth(raster)), static_cast<float>(RwRasterGetHeight(raster)));
    SetRenderState();
    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, maVertices, 4);
}

void CSprite2d::Draw(const CRect& posn, const CRGBA& color, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4)
{
    SetVertices(posn, color, color, color, color, u1, v1, u2, v2, u3, v3, u4, v4);
    SetRenderState();
    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, maVertices, 4);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
}

void CSprite2d::Draw(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4)
{
    SetVertices(posn, color1, color2, color3, color4);
    SetRenderState();
    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, maVertices, 4);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
}

void CSprite2d::Draw(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, const CRGBA& color)
{
    SetVertices(x1, y1, x2, y2, x3, y3, x4, y4, color, color, color, color);
    SetRenderState();
    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, maVertices, 4);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
}

// 0x727260
void CSprite2d::SetRecipNearClip() {
    ZoneScoped;
    // NOP
}

void CSprite2d::InitPerFrame() {
    ZoneScoped;

    nextBufferVertex = 0;
    nextBufferIndex = 0;
    RecipNearClip = 1.0f / RwCameraGetNearClipPlane(Scene.m_pRwCamera);
    NearScreenZ = RWSRCGLOBAL(dOpenDevice).zBufferNear;
}

bool CSprite2d::IsVertexBufferEmpty()
{
    return nextBufferVertex == 0;
}

bool CSprite2d::IsVertexBufferFull()
{
    return nextBufferVertex > TOTAL_RADIOSITY_VERTEX_BUFFER || nextBufferIndex > TOTAL_TEMP_BUFFER_INDICES;
}

void CSprite2d::RenderVertexBuffer()
{
    if (nextBufferVertex <= 0)
        return;

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));
    RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aRadiosityVertexBuffer, nextBufferVertex, aTempBufferIndices, nextBufferIndex);
    nextBufferVertex = 0;
    nextBufferIndex = 0;
}

// different kinds of vertices' defining
void CSprite2d::SetVertices(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4)
{
    float offset = 1.0f / 1024.0f;
    SetVertices(posn, color1, color2, color3, color4,
              offset,       offset,
        1.f + offset,       offset,
              offset, 1.f + offset,
        1.f + offset, 1.f + offset);
}

void CSprite2d::SetVertices(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4)
{
    RwIm2DVertexSetScreenX(&maVertices[0], x3);
    RwIm2DVertexSetScreenY(&maVertices[0], y3);
    RwIm2DVertexSetScreenZ(&maVertices[0], NearScreenZ);
    RwIm2DVertexSetRecipCameraZ(&maVertices[0], RecipNearClip);
    RwIm2DVertexSetU(&maVertices[0], 0.f, RecipNearClip);
    RwIm2DVertexSetV(&maVertices[0], 0.f, RecipNearClip);
    RwIm2DVertexSetIntRGBA(&maVertices[0], color3.r, color3.g, color3.b, color3.a);

    RwIm2DVertexSetScreenX(&maVertices[1], x4);
    RwIm2DVertexSetScreenY(&maVertices[1], y4);
    RwIm2DVertexSetScreenZ(&maVertices[1], NearScreenZ);
    RwIm2DVertexSetRecipCameraZ(&maVertices[1], RecipNearClip);
    RwIm2DVertexSetU(&maVertices[1], 1.f, RecipNearClip);
    RwIm2DVertexSetV(&maVertices[1], 0.f, RecipNearClip);
    RwIm2DVertexSetIntRGBA(&maVertices[1], color4.r, color4.g, color4.b, color4.a);

    RwIm2DVertexSetScreenX(&maVertices[2], x2);
    RwIm2DVertexSetScreenY(&maVertices[2], y2);
    RwIm2DVertexSetScreenZ(&maVertices[2], NearScreenZ);
    RwIm2DVertexSetRecipCameraZ(&maVertices[2], RecipNearClip);
    RwIm2DVertexSetU(&maVertices[2], 1.f, RecipNearClip);
    RwIm2DVertexSetV(&maVertices[2], 1.f, RecipNearClip);
    RwIm2DVertexSetIntRGBA(&maVertices[2], color2.r, color2.g, color2.b, color2.a);

    RwIm2DVertexSetScreenX(&maVertices[3], x1);
    RwIm2DVertexSetScreenY(&maVertices[3], y1);
    RwIm2DVertexSetScreenZ(&maVertices[3], NearScreenZ);
    RwIm2DVertexSetRecipCameraZ(&maVertices[3], RecipNearClip);
    RwIm2DVertexSetU(&maVertices[3], 0.f, RecipNearClip);
    RwIm2DVertexSetV(&maVertices[3], 1.f, RecipNearClip);
    RwIm2DVertexSetIntRGBA(&maVertices[3], color1.r, color1.g, color1.b, color1.a);
}

void CSprite2d::SetVertices(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4,
    float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4)
{
    SetVertices(maVertices, posn, color1, color2, color3, color4, u1, v1, u2, v2, u3, v3, u4, v4);
}

void CSprite2d::SetVertices(int32 numVerts, const CVector2D* posn, const CVector2D* texCoors, const CRGBA& color)
{
    for (int32 i = 0; i < numVerts; ++i) {
        RwIm2DVertexSetScreenX(&maVertices[i], posn[i].x);
        RwIm2DVertexSetScreenY(&maVertices[i], posn[i].y);
        RwIm2DVertexSetScreenZ(&maVertices[i], NearScreenZ + 0.0001f);
        RwIm2DVertexSetRecipCameraZ(&maVertices[i], RecipNearClip);
        RwIm2DVertexSetU(&maVertices[i], texCoors[i].x, RecipNearClip);
        RwIm2DVertexSetV(&maVertices[i], texCoors[i].y, RecipNearClip);
        RwIm2DVertexSetIntRGBA(&maVertices[i], color.r, color.g, color.b, color.a);
    }
}

void CSprite2d::SetVertices(int32 numVerts, const CVector2D* posn, const CRGBA& color)
{
    for (int32 i = 0; i < numVerts; ++i) {
        RwIm2DVertexSetScreenX(&maVertices[i], posn[i].x);
        RwIm2DVertexSetScreenY(&maVertices[i], posn[i].y);
        RwIm2DVertexSetScreenZ(&maVertices[i], NearScreenZ);
        RwIm2DVertexSetRecipCameraZ(&maVertices[i], RecipNearClip);
        RwIm2DVertexSetU(&maVertices[i], 1.f, RecipNearClip);
        RwIm2DVertexSetV(&maVertices[i], 1.f, RecipNearClip);
        RwIm2DVertexSetIntRGBA(&maVertices[i], color.r, color.g, color.b, color.a);
    }
}

void CSprite2d::SetMaskVertices(int32 numVerts, const CVector2D* posn, float depth)
{
    for (int32 i = 0; i < numVerts; ++i) {
        RwIm2DVertexSetScreenX(&maVertices[i], posn[i].x);
        RwIm2DVertexSetScreenY(&maVertices[i], posn[i].y);
        RwIm2DVertexSetScreenZ(&maVertices[i], depth);
        RwIm2DVertexSetRecipCameraZ(&maVertices[i], RecipNearClip);
        RwIm2DVertexSetIntRGBA(&maVertices[i], 0, 0, 0, 0);
    }
}

void CSprite2d::SetVertices(RwIm2DVertex* vertices, const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4,
    float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4)
{
    RwIm2DVertexSetScreenX(&vertices[0], posn.left);
    RwIm2DVertexSetScreenY(&vertices[0], posn.bottom);
    RwIm2DVertexSetScreenZ(&vertices[0], NearScreenZ);
    RwIm2DVertexSetRecipCameraZ(&vertices[0], RecipNearClip);
    RwIm2DVertexSetU(&vertices[0], u1, RecipNearClip);
    RwIm2DVertexSetV(&vertices[0], v1, RecipNearClip);
    RwIm2DVertexSetIntRGBA(&vertices[0], color3.r, color3.g, color3.b, color3.a);

    RwIm2DVertexSetScreenX(&vertices[1], posn.right);
    RwIm2DVertexSetScreenY(&vertices[1], posn.bottom);
    RwIm2DVertexSetScreenZ(&vertices[1], NearScreenZ);
    RwIm2DVertexSetRecipCameraZ(&vertices[1], RecipNearClip);
    RwIm2DVertexSetU(&vertices[1], u2, RecipNearClip);
    RwIm2DVertexSetV(&vertices[1], v2, RecipNearClip);
    RwIm2DVertexSetIntRGBA(&vertices[1], color4.r, color4.g, color4.b, color4.a);

    RwIm2DVertexSetScreenX(&vertices[2], posn.right);
    RwIm2DVertexSetScreenY(&vertices[2], posn.top);
    RwIm2DVertexSetScreenZ(&vertices[2], NearScreenZ);
    RwIm2DVertexSetRecipCameraZ(&vertices[2], RecipNearClip);
    RwIm2DVertexSetU(&vertices[2], u4, RecipNearClip);
    RwIm2DVertexSetV(&vertices[2], v4, RecipNearClip);
    RwIm2DVertexSetIntRGBA(&vertices[2], color2.r, color2.g, color2.b, color2.a);

    RwIm2DVertexSetScreenX(&vertices[3], posn.left);
    RwIm2DVertexSetScreenY(&vertices[3], posn.top);
    RwIm2DVertexSetScreenZ(&vertices[3], NearScreenZ);
    RwIm2DVertexSetRecipCameraZ(&vertices[3], RecipNearClip);
    RwIm2DVertexSetU(&vertices[3], u3, RecipNearClip);
    RwIm2DVertexSetV(&vertices[3], v3, RecipNearClip);
    RwIm2DVertexSetIntRGBA(&vertices[3], color1.r, color1.g, color1.b, color1.a);
}

// draws non-textured rectangle
void CSprite2d::DrawRect(const CRect& posn, const CRGBA& color) {
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
    SetVertices(posn, color, color, color, color);
    if (color.a == 255)
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    else
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, maVertices, 4);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
}

// this could be used for drawing textured rectangle (use SetRenderState() before this)
void CSprite2d::DrawTxRect(const CRect& posn, const CRGBA& color) {
    CSprite2d::SetVertices(posn, color, color, color, color);
    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, maVertices, 4);
}

// draw non-textured rectangle, with setupable corners' colors.
void CSprite2d::DrawRect(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4) {
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
    SetVertices(posn, color1, color2, color3, color4);
    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, maVertices, 4);
}

// draws non-textured rectangle with default blending states
void CSprite2d::DrawRectXLU(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    DrawRect(posn, color1, color2, color3, color4);
}

// draws rectangle, texture could be set with SetRenderState().
void CSprite2d::DrawAnyRect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4) {
    SetVertices(x1, y1, x2, y2, x3, y3, x4, y4, color1, color2, color3, color4);
    if (color1.a == 255 && color2.a == 255 && color3.a == 255 && color4.a == 255)
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    else
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, maVertices, 4);
}

// draws a triangle with rotation (degrees)
void CSprite2d::DrawCircleAtNearClip(const CVector2D& posn, float size, const CRGBA& color, int32 angle)
{
    ((void(__cdecl*)(const CVector2D&, float, const CRGBA&, int32))0x727D60)(posn, size, color, angle);

    /* NOT TESTED
    RwIm2DVertexSetScreenX(&maVertices[0], posn.x);
    RwIm2DVertexSetScreenY(&maVertices[0], posn.y);
    RwIm2DVertexSetScreenZ(&maVertices[0], NearScreenZ);
    RwIm2DVertexSetRecipCameraZ(&maVertices[0], RecipNearClip);
    RwIm2DVertexSetU(&maVertices[0], 0.5f, RecipNearClip);
    RwIm2DVertexSetV(&maVertices[0], 0.5f, RecipNearClip);
    RwIm2DVertexSetIntRGBA(&maVertices[0], color.r, color.g, color.b, color.a);

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));

    float posna = 360.f / static_cast<float>(angle);
    float step = posna * DegreesToRadians(1.f) * (256.f / DegreesToRadians(360.f)); // posna * 35 / 45
    for (int32 i = 0; i < angle; ++i)
    {
        for (int32 l = 1; l <= 2; ++l) {
            uint8 idx = static_cast<uint8>(static_cast<float>(i + l - 1) * step);
            RwIm2DVertexSetScreenX(&maVertices[l], size * CMaths::ms_SinTable[idx + 64] + posn.x);
            RwIm2DVertexSetScreenY(&maVertices[l], size * CMaths::ms_SinTable[idx] + posn.y);
            RwIm2DVertexSetScreenZ(&maVertices[l], NearScreenZ);
            RwIm2DVertexSetRecipCameraZ(&maVertices[l], RecipNearClip);
            RwIm2DVertexSetIntRGBA(&maVertices[l], color.r, color.g, color.b, color.a);
            RwIm2DVertexSetU(&maVertices[l], (CMaths::ms_SinTable[idx + 64] + 1.f) * 0.5f, RecipNearClip);
            RwIm2DVertexSetV(&maVertices[l], (CMaths::ms_SinTable[idx] + 1.f) * 0.5f, RecipNearClip);
        }
        RWSRCGLOBAL(dOpenDevice).fpIm2DRenderTriangle(maVertices, 3, 2, 1, 0);
    } */
}

// this makes some trick with sprite z position (z = NearScreenZ + 0.000001).
void CSprite2d::SetVerticesForSniper(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4)
{
    ((void(__cdecl*)(const CRect&, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&))0x727FD0)(posn, color1, color2, color3, color4);
}

void CSprite2d::OffsetTexCoordForBilinearFiltering(float width, float height)
{
    ((void(__cdecl*)(float, float))0x728150)(width, height);
}

// add vertices to buffer
void CSprite2d::AddToBuffer(const CRect& posn, const CRGBA& color, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4)
{
    ((void(__cdecl*)(const CRect&, const CRGBA&, float, float, float, float, float, float, float, float))0x728200)(posn, color, u1, v1, u2, v2, u3, v3, u4, v4);
}

// non-textured polygon
// 0x7285B0
void CSprite2d::Draw2DPolygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, const CRGBA& color)
{
    SetVertices(x1, y1, x2, y2, x3, y3, x4, y4, color, color, color, color);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,     RWRSTATE(rwSHADEMODENASHADEMODE));

    if (color.a == 255)
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    else
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));

    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, maVertices, 4);
}

// draws progress line. Progress is a value in ranges 0 - 100.
// ----------------------
// | +++++++++++--------|
// ----------------------
// 0x728640
void CSprite2d::DrawBarChart(float x, float y, uint16 width, uint8 height, float progress,
    int8 progressAdd, uint8 drawPercentage, uint8 drawBlackBorder,
    CRGBA color, CRGBA addColor)
{
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,     RWRSTATE(rwSHADEMODEFLAT));

    progress = std::clamp(progress, 0.0f, 100.0f);

    const float endX = x + (float)width;
    const float unclampedCurrX = x + (float)width * progress / 100.0f;
    const float currX = std::min(unclampedCurrX, endX);
    const auto fheight = (float)height;

    // Progress rect
    DrawRect({ x, y, currX, y + fheight }, color);
    // Background (from currX to endX)
    DrawRect({ currX, y, endX, y + fheight }, { uint8(color.r / 2.0f), uint8(color.g / 2.0f), uint8(color.b / 2.0f), color.a });

    if (progressAdd) {
        addColor.a = color.a;
        DrawRect({
            std::max<float>(x - 1.0f, currX - progressAdd < 0 ? 127.0f : progressAdd), // FIX_BUGS: Because of the progressAdd overflow, the green bar is out of bounds.
            y,
            currX,
            y + fheight
        }, addColor);
    }

    if (drawBlackBorder) {
        const float w = SCREEN_STRETCH_X(2.0f), h = SCREEN_SCALE_Y(2.0f);
        const CRect rects[] = {
            //left,     top,              right,    bottom
            { x,        y,                endX,     y + h       },       // Top
            { x,        y + fheight - h,  endX,     y + fheight },       // Bottom
            { x,        y,                x + w,    y + fheight },       // Left
            { endX - w, y,                endX,     y + fheight }        // Right
        };

        const auto black = CRGBA{ 0, 0, 0, color.a };
        for (const CRect& rect : rects) {
            DrawRect(rect, black);
        }
    }

    // unused
    if (drawPercentage) {
        char text[12];
        sprintf_s(text, "%d%%", (int)progress);

        GxtChar gxtText[12];
        AsciiToGxtChar(text, gxtText);

        CFont::SetWrapx(x);
        CFont::SetRightJustifyWrap(endX);
        CFont::SetColor({ 0, 0, 0, color.a });
        CFont::SetEdge(0);
        CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
        CFont::SetScale(fheight * 0.03f, fheight / 0.04f);

        auto textX = (uint16)unclampedCurrX;
        if (x + 50.0f <= (float)textX) {
            CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
        }
        else {
            CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
            textX += 5;
        }
        CFont::PrintString((float)textX, y + 2.0f, gxtText);
    }
}
