/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <string>
#include <initializer_list>
#include <RenderWare.h>

#include "Vector.h"
#include "Vector2D.h"
#include "Matrix.h"
#include "Draw.h"

class CAnimBlendClumpData;
class CSimpleTransform;

constexpr float DegreesToRadians(float angleInDegrees); // forward declaration

constexpr auto DEFAULT_SCREEN_WIDTH       = 640;
constexpr auto DEFAULT_SCREEN_HEIGHT      = 448;
constexpr auto DEFAULT_SCREEN_HEIGHT_PAL  = 512.0f;
constexpr auto DEFAULT_SCREEN_HEIGHT_NTSC = 448.0f;
constexpr auto DEFAULT_ASPECT_RATIO       = 4.0f / 3.0f;
constexpr auto DEFAULT_VIEW_WINDOW        = 0.7f;

// game uses maximumWidth/Height, but this probably won't work
// with RW windowed mode
#define SCREEN_WIDTH ((float)RsGlobal.maximumWidth)
#define SCREEN_HEIGHT ((float)RsGlobal.maximumHeight)
#define SCREEN_ASPECT_RATIO (CDraw::ms_fAspectRatio)
#define SCREEN_VIEW_WINDOW (std::tan(DegreesToRadians(CDraw::GetFOV() / (2.0f)))) // todo: GetScaledFov
#define SCREEN_WIDTH_UNIT (SCREEN_WIDTH / 640.0f)
#define SCREEN_HEIGHT_UNIT (SCREEN_HEIGHT / 448.0f)

// This scales from PS2 pixel coordinates to the real resolution
constexpr float SCREEN_STRETCH_X(float a)           { return a * SCREEN_WIDTH  / (float)DEFAULT_SCREEN_WIDTH; } // RsGlobal.maximumWidth * 0.0015625 * value
constexpr float SCREEN_STRETCH_Y(float a)           { return a * SCREEN_HEIGHT / (float)DEFAULT_SCREEN_HEIGHT; }
constexpr float SCREEN_STRETCH_FROM_RIGHT(float a)  { return SCREEN_WIDTH  - SCREEN_STRETCH_X(a); }
constexpr float SCREEN_STRETCH_FROM_BOTTOM(float a) { return SCREEN_HEIGHT - SCREEN_STRETCH_Y(a); }

#define ASPECT_RATIO_SCALE
#ifdef ASPECT_RATIO_SCALE
#define SCREEN_SCALE_AR(a) ((a) * DEFAULT_ASPECT_RATIO / SCREEN_ASPECT_RATIO)
#else
#define SCREEN_SCALE_AR(a) (a)
#endif

// This scales from PS2 pixel coordinates while optionally maintaining the aspect ratio
constexpr float SCREEN_SCALE_X(float a)           { return SCREEN_SCALE_AR(SCREEN_STRETCH_X(a)); }
constexpr float SCREEN_SCALE_Y(float a)           { return SCREEN_STRETCH_Y(a); } // RsGlobal.maximumHeight * 0.  * value
constexpr float SCREEN_SCALE_FROM_RIGHT(float a)  { return SCREEN_WIDTH  - SCREEN_SCALE_X(a); }
constexpr float SCREEN_SCALE_FROM_BOTTOM(float a) { return SCREEN_HEIGHT - SCREEN_SCALE_Y(a); }

constexpr auto BUILD_NAME_FULL = "TEST"; // NOTSA

static inline int32 gDefaultTaskTime = 9'999'999; // or 0x98967F a.k.a (ten million - 1)

static inline char (&gString)[352] = *(char (*)[352])0xB71670;
static inline char (&gString2)[352] = *(char (*)[352])0xB71510;

static inline char (&gGxtString)[552] = *(char (*)[552])0xC1B100;
static inline char (&gGxtString2)[552] = *(char (*)[552])0xC1AED8;
static inline char (&GxtErrorString)[32] = *(char (*)[32])0xC1AEB8;

static inline int32& g_nNumIm3dDrawCalls = *(int32*)0xB73708;

static inline char (&PC_Scratch)[16384] = *(char (*)[16384])0xC8E0C8;

extern RwRGBAReal& AmbientLightColour;
extern RwRGBAReal& AmbientLightColourForFrame;

// taken from rpplugin.h
#define rwVENDORID_DEVELOPER 0x0253F2

#define RpGeometryGetMesh(_geometry, _index) (&((RpMesh*)(((char*)(_geometry)->mesh) + sizeof(RpMeshHeader) + ((_geometry)->mesh->firstMeshOffset)))[_index])

constexpr float E              = 2.71828f;          // e
constexpr float E_CONST        = 0.577f;            // Euler-Mascheroni constant
constexpr float FRAC_1_TAU     = 0.159154f;         // 1 / τ
constexpr float FRAC_1_PI      = 0.318309f;         // 1 / π
constexpr float FRAC_2_TAU     = 0.318309f;         // 2 / τ
constexpr float FRAC_2_PI      = 0.636619f;         // 2 / π
constexpr float FRAC_2_SQRT_PI = 1.12837f;          // 2 / √π
constexpr float FRAC_4_TAU     = 0.636619f;         // 4 / τ
constexpr float FRAC_1_SQRT_2  = 0.707106f;         // 1 / √2
constexpr float FRAC_PI_2      = 1.57079f;          // π / 2
constexpr float FRAC_PI_3      = 1.04719f;          // π / 3
constexpr float FRAC_PI_4      = 0.785398f;         // π / 4
constexpr float FRAC_PI_6      = 0.523598f;         // π / 6
constexpr float FRAC_PI_8      = 0.392699f;         // π / 8
constexpr float FRAC_TAU_2     = 3.14159f;          // τ / 2
constexpr float FRAC_TAU_3     = 2.09439f;          // τ / 3
constexpr float FRAC_TAU_4     = 1.57079f;          // τ / 4
constexpr float FRAC_TAU_6     = 1.04719f;          // τ / 6
constexpr float FRAC_TAU_8     = 0.785398f;         // τ / 8
constexpr float FRAC_TAU_12    = 0.523598f;         // τ / 12
constexpr float LN_2           = 0.693147f;         // ln(2)
constexpr float LN_10          = 2.30258f;          // ln(10)
constexpr float LOG2_E         = 1.44269f;          // log2(e)
constexpr float LOG10_E        = 0.434294f;         // log10(e)
constexpr float LOG10_2        = 0.301029f;         // log10(2)
constexpr float LOG2_10        = 3.32192f;          // log2(10)
constexpr float PI             = 3.14159f;          // π
constexpr float HALF_PI        = PI / 2.0f;         // π / 2
constexpr float SQRT_2         = 1.41421f;          // √2
constexpr float SQRT_3         = 1.73205f;          // √3
constexpr float SIN_PI         = 0.0f;              // sin(π);
constexpr float COS_PI         = -1.0f;             // cos(π);
constexpr float TWO_PI         = 6.28318f;          // τ (TAU)

constexpr float sq(float x) { return x * x; }

struct SpriteFileName {
    const char* name;
    const char* alpha;
};

void InjectCommonHooks();

void TransformPoint(RwV3d& point, const CSimpleTransform& placement, const RwV3d& vecPos);
void TransformVectors(RwV3d* vecsOut, int32 numVectors, const CMatrix& matrix, const RwV3d* vecsin);
void TransformVectors(RwV3d* vecsOut, int32 numVectors, const CSimpleTransform& transform, const RwV3d* vecsin);

// Check point is within 2D rectangle
static bool IsPointInRect2D(const CVector2D& point, const CVector2D& min, const CVector2D& max) {
    return point.x >= min.x && point.x <= max.x &&
           point.y >= min.y && point.y <= max.y;
}

static bool IsPointInCircle2D(CVector2D point, CVector2D center, float r) {
    return DistanceBetweenPointsSquared2D(point, center) <= sq(r);
}

static bool IsPointInSphere(const CVector& point, const CVector& center, float r) {
    return DistanceBetweenPointsSquared(point, center) <= sq(r);
}

// Converts degrees to radians
// keywords: 0.017453292 flt_8595EC
constexpr float DegreesToRadians(float angleInDegrees) {
    return angleInDegrees * PI / 180.0F;
}

// Converts radians to degrees
// 57.295826
constexpr float RadiansToDegrees(float angleInRadians) {
    return angleInRadians * 180.0F / PI;
}

inline const CVector lerp(const CVector& fMin, const CVector& fMax, float fProgress) {
    return fMin * (1.0F - fProgress) + fMax * fProgress;
}

inline const float lerp(float fMin, float fMax, float fProgress) {
    return fMin * (1.0F - fProgress) + fMax * fProgress;
}

inline const float invLerp(float fMin, float fMax, float fVal) {
    return (fVal - fMin) / (fMax - fMin);
}

// 0x4EEA80 - And inlined helluvalot
inline bool approxEqual(float f1, float f2, float epsilon) {
    return fabs(f1 - f2) < epsilon;
}

// Used in some audio functions, mostly CAESmoothFadeThread
inline bool approxEqual2(float f1, float f2, float epsilon = 0.01F)
{
    return f1 == f2 || fabs(f1 - f2) < epsilon;
}

// shit
extern constexpr bool make_fourcc3(const char* line, const char abc[3]) {
    return line[0] == abc[0] &&
           line[1] == abc[1] &&
           line[2] == abc[2];
}

// shit
extern constexpr bool make_fourcc4(const char* line, const char abcd[4]) {
    return line[0] == abcd[0] &&
           line[1] == abcd[1] &&
           line[2] == abcd[2] &&
           line[3] == abcd[3];
}

// shit
extern constexpr uint32 make_fourcc4(const char fourcc[4]) {
    return fourcc[0] << 0 |
           fourcc[1] << 8 |
           fourcc[2] << 16 |
           fourcc[3] << 24;
}

char* MakeUpperCase(char *dest, const char *src);
bool EndsWith(const char* str, const char* with, bool caseSensitive = true);

void Render2dStuff();
void DefinedState();
void DefinedState2d();
RpAtomic* RemoveRefsCB(RpAtomic* atomic, void* _IGNORED_ data);
void RemoveRefsForAtomic(RpClump* clump);

bool GraphicsLowQuality();

/**
 * Writes given raster to PNG file using RtPNGImageWrite
 */
void WriteRaster(RwRaster* raster, const char* filename);
bool CalcScreenCoors(const CVector& in, CVector* out, float* screenX, float* screenY);
bool CalcScreenCoors(const CVector& in, CVector* out);
bool DoesInfiniteLineTouchScreen(float baseX, float baseY, float deltaX, float deltaY);
bool IsPointInsideLine(float fLineBaseX, float fLineBaseY, float fDeltaX, float fDeltaY, float fTestPointX, float fTestPointY, float fRadius);

void LittleTest();

std::wstring UTF8ToUnicode(const std::string& str);
std::string UnicodeToUTF8(const std::wstring& str);

constexpr int32 TOTAL_TEMP_BUFFER_INDICES = 4096;
constexpr int32 TOTAL_TEMP_BUFFER_VERTICES = 2048;
constexpr int32 TOTAL_RADIOSITY_VERTEX_BUFFER = 1532;

static inline int32 WindowsCharset = static_cast<int32>(GetACP());

static inline uint16& uiTempBufferIndicesStored = *(uint16*)0xC4B954;
static inline uint16& uiTempBufferVerticesStored = *(uint16*)0xC4B950;
static inline RxVertexIndex(&aTempBufferIndices)[TOTAL_TEMP_BUFFER_INDICES] = *(RxVertexIndex(*)[TOTAL_TEMP_BUFFER_INDICES])0xC4B958;
static inline RxObjSpace3DVertex(&aTempBufferVertices)[TOTAL_TEMP_BUFFER_VERTICES] = *(RxObjSpace3DVertex(*)[TOTAL_TEMP_BUFFER_VERTICES])0xC4D958; // size 1024 - after this there are 2 more arrays like this, both sized 512
static inline RwD3D9Vertex(&aRadiosityVertexBuffer)[TOTAL_RADIOSITY_VERTEX_BUFFER] = *reinterpret_cast<RwD3D9Vertex(*)[TOTAL_RADIOSITY_VERTEX_BUFFER]>(0xC5F958);
