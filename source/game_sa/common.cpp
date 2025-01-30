/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "Garages.h"
#include "common.h"
#include "GxtChar.h"
#include "UIRenderer.h"
#include "CarCtrl.h"
#include "UserDisplay.h"
#include "PostEffects.h"
#include "SpecialFX.h"

#include "Hud.h"
#include "app.h"

void InjectCommonHooks() {
    RH_ScopedNamespaceName("Common");
    RH_ScopedCategory("Common");

    RH_ScopedGlobalInstall(MakeUpperCase, 0x7186E0);
    RH_ScopedGlobalInstall(AsciiToGxtChar, 0x718600);
    RH_ScopedGlobalInstall(WriteRaster, 0x005A4150);
    RH_ScopedGlobalOverloadedInstall(CalcScreenCoors, "VVff", 0x71DA00, bool(*)(const CVector&, CVector&, float&, float&), { .reversed = true });
    RH_ScopedGlobalOverloadedInstall(CalcScreenCoors, "VV", 0x71DAB0, bool(*)(const CVector&, CVector&), { .reversed = true });
    RH_ScopedGlobalInstall(LittleTest, 0x541330);
    RH_ScopedGlobalInstall(DoesInfiniteLineTouchScreen, 0x71DB80);
    RH_ScopedGlobalInstall(IsPointInsideLine, 0x71E050);
}

// 0x54ECE0
void TransformPoint(RwV3d& point, const CSimpleTransform& placement, const RwV3d& vecPos) {
    const auto cos = std::cos(placement.m_fHeading), sin = std::sin(placement.m_fHeading);

    point.x = cos * vecPos.x - sin * vecPos.y + placement.m_vPosn.x;
    point.y = sin * vecPos.x + cos * vecPos.y + placement.m_vPosn.y;
    point.z = vecPos.z + placement.m_vPosn.z;
}

// 0x54EEA0
void TransformVectors(RwV3d* vecsOut, int32 numVectors, const CMatrix& matrix, const RwV3d* vecsIn) {
    matrix.CopyToRwMatrix(CGame::m_pWorkingMatrix1);
    RwMatrixUpdate(CGame::m_pWorkingMatrix1);

    for (auto i = 0; i < numVectors; i++) {
        RwV3dTransformVector(vecsOut++, vecsIn++, CGame::m_pWorkingMatrix1);
    }
}

// 0x54EE30
void TransformVectors(RwV3d* vecsOut, int32 numVectors, const CSimpleTransform& transform, const RwV3d* vecsIn) {
    for (auto i = 0; i < numVectors; i++) {
        TransformPoint(*vecsOut++, transform, *vecsIn++);
    }
}

// 0x54EEF0
void TransformPoints(RwV3d* pointOut, int count, const CMatrix& transformMatrix, RwV3d* pointIn) {
    transformMatrix.CopyToRwMatrix(CGame::m_pWorkingMatrix1);
    RwMatrixUpdate(CGame::m_pWorkingMatrix1);

    for (auto i = 0; i < count; i++) {
        RwV3dTransformPoint(pointOut++, pointIn++, CGame::m_pWorkingMatrix1);
    }
}

// 0x7186E0
char* MakeUpperCase(char* dest, const char* src) {
    for (; *src; src++, dest++)
        *dest = std::toupper(*src);
    *dest = '\0';
    return dest;
}

// NOTSA
bool EndsWith(const char* str, const char* with, bool caseSensitive) {
    const auto strsz = strlen(str), withsz = strlen(with);
    assert(strsz >= withsz);
    return (caseSensitive ? strncmp : _strnicmp)(str + strsz - withsz, with, withsz) == 0;
}

// 0x70F9B0
bool GraphicsHighQuality() {
    if (g_fx.GetFxQuality() < FX_QUALITY_MEDIUM)
        return false;
    if (RwRasterGetDepth(RwCameraGetRaster(Scene.m_pRwCamera)) < 32)
        return false;
    return true;
}

// 0x5A4150
void WriteRaster(RwRaster* raster, const char* filename) {
    assert(raster);
    assert(filename && filename[0]);

    RwImage* img = RwImageCreate(RwRasterGetWidth(raster), RwRasterGetHeight(raster), RwRasterGetDepth(raster));
    RwImageAllocatePixels(img);
    RwImageSetFromRaster(img, raster);
    RtPNGImageWrite(img, filename);
    RwImageDestroy(img);
}

// 0x71DA00
bool CalcScreenCoors(const CVector& in, CVector& out, float& screenX, float& screenY) {
    if (!CalcScreenCoors(in, out)) {
        return false;
    }

    const float recpZ = 1.0f / out.z;
    screenX = SCREEN_WIDTH * recpZ / CDraw::ms_fFOV * 70.0f;
    screenY = SCREEN_HEIGHT * recpZ / CDraw::ms_fFOV * 70.0f;

    return true;
}

/*!
* @0x71DAB0
* @brief Calculate a 3D position on the screen
* @param in  in  The 3D to get the screen position of
* @param out out The 2D screen position (Also includes the depth in the `z` component)
* @returns False if the depth was <= 1 (in which case the `x, y` positions are not not calculated, but `z` is)
*/
bool CalcScreenCoors(const CVector& in, CVector& out) {
    out = TheCamera.GetViewMatrix().TransformPoint(in);
    if (out.z <= 1.0f) {
        return false;
    }

    const auto recpZ = 1.0f / out.z;
    out.x *= SCREEN_WIDTH * recpZ;
    out.y *= SCREEN_HEIGHT * recpZ;

    return true;
}

// 0x541330
void LittleTest() {
    ++g_nNumIm3dDrawCalls;
}

// @notsa
// Similar to `CCollision::DistAlongLine2D`, but instead of calcualting the dot product
// on the line itself, it's actually calculated on a perpendicular line on the right
float DistAlongPerpRightLine2D(CVector2D origin, CVector2D dir, CVector2D pt) {
    // return (pt.x - origin.x) * dir.y - (pt.y - origin.y) * dir.x
    return (pt - origin).Dot(dir.GetPerpRight());
}

// @unk
bool DoesInfiniteLineCrossFiniteLine(
    CVector2D finiteA,
    CVector2D finiteB,
    CVector2D origin,
    CVector2D dir
) {
    //return (((finiteA.x - origin.x) * dir.y) - ((finiteA.y - origin.y) * dir.x))
    //    * (((finiteB.x - origin.x) * dir.y) - ((finiteB.y - origin.y) * dir.x)) < 0.0;
    return DistAlongPerpRightLine2D(origin, dir, finiteA) * DistAlongPerpRightLine2D(origin, dir, finiteB) < 0.f;
}

// used only in COccluder::ProcessLineSegment
// 0x71DB80
bool DoesInfiniteLineTouchScreen(
    //float oX, float oY, float dX, float dY
    CVector2D origin,
    CVector2D dir
) {
    // Point is on screen
    if (IsPointInRect2D(origin, { 0.f, 0.f }, { SCREEN_WIDTH, SCREEN_HEIGHT })) {
        return true;
    }

    // Or line touches any of the screen's bezzles (?)
    return DoesInfiniteLineCrossFiniteLine({0.f, 0.f},           {SCREEN_WIDTH, 0.f},           origin, dir)
        || DoesInfiniteLineCrossFiniteLine({0.f, 0.f},           {0.f, SCREEN_HEIGHT},          origin, dir)
        || DoesInfiniteLineCrossFiniteLine({SCREEN_WIDTH, 0.f},  {SCREEN_WIDTH, SCREEN_HEIGHT}, origin, dir)
        || DoesInfiniteLineCrossFiniteLine({0.f, SCREEN_HEIGHT}, {SCREEN_WIDTH, SCREEN_HEIGHT}, origin, dir);
}

// Used only in COcclusion, COccluder, CActiveOccluder
// 0x71E050
bool IsPointInsideLine(
    CVector2D origin,
    CVector2D dir,
    CVector2D pt,
    float     radius
) {
    return DistAlongPerpRightLine2D(origin, dir, pt) >= radius;
}

// Convert UTF-8 string to Windows Unicode. Free pointer using delete[]
// NOTSA
std::wstring UTF8ToUnicode(const std::string &str)
{
    std::wstring out;

    int32 size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.data(), str.length(), nullptr, 0);
    if (size) {
        std::vector<wchar_t> temp;
        temp.resize(size, L'\0');

        if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.data(), str.length(), temp.data(), size)) {
            out.resize(size);
            std::copy(temp.begin(), temp.end(), out.begin());
        }
    }

    return out;
}

// Convert Windows Unicode to UTF-8. Free pointer using delete[]
// NOTSA
std::string UnicodeToUTF8(const std::wstring& str) {
    std::string out;

    int32 size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, str.data(), str.length(), nullptr, 0, nullptr, nullptr);
    if (size) {
        std::vector<char> temp;
        temp.resize(size, 0);

        if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, str.data(), str.length(), temp.data(), size, nullptr, nullptr)) {
            out.resize(size);
            std::copy(temp.begin(), temp.end(), out.begin());
        }
    }

    return out;
}

/*!
* @notsa
* @return The anim first found from `ids`
*/
CAnimBlendAssociation* RpAnimBlendClumpGetAssociation(RpClump* clump, std::initializer_list<AnimationId> ids) {
    for (const auto id : ids) {
        if (const auto anim = RpAnimBlendClumpGetAssociation(clump, (int32)id)) {
            return anim;
        }
    }
    return nullptr;
}
