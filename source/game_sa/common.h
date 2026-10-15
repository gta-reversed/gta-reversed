/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <Base.h>
#include <string>
#include <initializer_list>
#include <RenderWare.h>
#include <GxtChar.h>
#include <numbers>
#include "AnimationEnums.h"
#include "Vector.h"
#include "Vector2D.h"
#include "Matrix.h"
#include "Draw.h"

class CAnimBlendAssociation;
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

// This scales from PS2 pixel coordinates to the real resolution
inline float SCREEN_STRETCH_X(float a)           { return a * SCREEN_WIDTH  / (float)DEFAULT_SCREEN_WIDTH; } // RsGlobal.maximumWidth * 0.0015625 * value
inline float SCREEN_STRETCH_Y(float a)           { return a * SCREEN_HEIGHT / (float)DEFAULT_SCREEN_HEIGHT; }
inline float SCREEN_STRETCH_FROM_RIGHT(float a)  { return SCREEN_WIDTH  - SCREEN_STRETCH_X(a); }
inline float SCREEN_STRETCH_FROM_BOTTOM(float a) { return SCREEN_HEIGHT - SCREEN_STRETCH_Y(a); }

#define ASPECT_RATIO_SCALE
#ifdef ASPECT_RATIO_SCALE
#define SCREEN_SCALE_AR(a) ((a) * DEFAULT_ASPECT_RATIO / SCREEN_ASPECT_RATIO)
#else
#define SCREEN_SCALE_AR(a) (a)
#endif

// This scales from PS2 pixel coordinates while optionally maintaining the aspect ratio
inline float SCREEN_SCALE_X(float a)           { return SCREEN_SCALE_AR(SCREEN_STRETCH_X(a)); }
inline float SCREEN_SCALE_Y(float a)           { return SCREEN_STRETCH_Y(a); } // RsGlobal.maximumHeight * 0.  * value
inline float SCREEN_SCALE_FROM_RIGHT(float a)  { return SCREEN_WIDTH  - SCREEN_SCALE_X(a); }
inline float SCREEN_SCALE_FROM_BOTTOM(float a) { return SCREEN_HEIGHT - SCREEN_SCALE_Y(a); }

constexpr auto BUILD_NAME_FULL = "TEST"; // NOTSA

static inline int32 gDefaultTaskTime = 9'999'999; // or 0x98967F a.k.a (ten million - 1)

static inline auto& gString = StaticRef<char[352]>(0xB71670);
static inline auto& gString2 = StaticRef<char[352]>(0xB71510);

static inline auto& gGxtString = StaticRef<GxtChar[552]>(0xC1B100);
static inline auto& gGxtString2 = StaticRef<GxtChar[552]>(0xC1AED8);
static inline auto& GxtErrorString = StaticRef<GxtChar[32]>(0xC1AEB8);

static inline auto& g_nNumIm3dDrawCalls = StaticRef<int32>(0xB73708);

static inline auto& PC_Scratch = StaticRef<char[16384]>(0xC8E0C8);

extern RwRGBAReal& AmbientLightColour;
extern RwRGBAReal& AmbientLightColourForFrame;

// taken from rpplugin.h
constexpr auto rwVENDORID_DEVELOPER = 0x0253F2;

#define RpGeometryGetMesh(_geometry, _index) (&((RpMesh*)(((char*)(_geometry)->mesh) + sizeof(RpMeshHeader) + ((_geometry)->mesh->firstMeshOffset)))[_index])

constexpr float LN_2           = std::numbers::ln2_v<float>;               // ln(2)
constexpr float LN_10          = std::numbers::ln10_v<float>;              // ln(10)
constexpr float LOG2_E         = std::numbers::log2e_v<float>;             // log2(e)
constexpr float LOG10_E        = std::numbers::log10e_v<float>;            // log10(e)
constexpr float LOG10_2        = LN_2 * LOG10_E;                           // log10(2) = ln(2) * log10(e) = ln(2) / ln(10) = log10(2)
constexpr float LOG2_10        = LN_10 * LOG2_E;                           // log2(10) = ln(10) * log2(e) = ln(10) / ln(2) = log2(10)
constexpr float SQRT_2         = std::numbers::sqrt2_v<float>;             // √2
constexpr float SQRT_3         = std::numbers::sqrt3_v<float>;             // √3
constexpr float E              = std::numbers::e_v<float>;                 // e
constexpr float E_CONST        = std::numbers::egamma_v<float>;            // Euler-Mascheroni constant
constexpr float PI             = std::numbers::pi_v<float>;                // π
constexpr float HALF_PI        = PI / 2.0f;                                // π / 2
constexpr float PI_6           = PI / 6.0f;                                // π / 6
constexpr float TWO_PI         = 2.0f * PI;                                // τ (TAU) = 2π
constexpr float FRAC_1_TAU     = 1.0f / (2.0f * PI);                       // 1 / τ
constexpr float FRAC_1_PI      = std::numbers::inv_pi_v<float>;            // 1 / π
constexpr float FRAC_2_TAU     = FRAC_1_PI;                                // 2 / τ
constexpr float FRAC_2_PI      = 2.0f * FRAC_1_PI;                         // 2 / π
constexpr float FRAC_2_SQRT_PI = 2.0f * std::numbers::inv_sqrtpi_v<float>; // 2 / √π
constexpr float FRAC_4_TAU     = FRAC_2_PI;                                // 4 / τ
constexpr float FRAC_1_SQRT_2  = 1.0f / SQRT_2;                            // 1 / √2
constexpr float FRAC_PI_2      = HALF_PI;                                  // π / 2
constexpr float FRAC_PI_3      = PI / 3.0f;                                // π / 3
constexpr float FRAC_PI_4      = PI / 4.0f;                                // π / 4
constexpr float FRAC_PI_6      = PI_6;                                     // π / 6
constexpr float FRAC_PI_8      = FRAC_PI_4 / 2.0f;                         // π / 8
constexpr float FRAC_PI_9      = PI / 9.0f;                                // π / 9
constexpr float FRAC_TAU_3     = 2.0f * FRAC_PI_3;                         // τ / 3
constexpr float FRAC_TAU_4     = HALF_PI;                                  // τ / 4
constexpr float FRAC_TAU_6     = FRAC_PI_3;                                // τ / 6
constexpr float FRAC_TAU_8     = FRAC_PI_4;                                // τ / 8
constexpr float FRAC_TAU_12    = PI_6;                                     // τ / 12
constexpr float DEG_TO_RAD     = PI / 180.0f;                              // π / 180
// TODO: use c++26 constexpr math function here later
constexpr float COS_45         = SQRT_2 / 2.f;                             // cos(45°)

template<typename T>
NOTSA_FORCEINLINE constexpr T sq(T x) { return x * x; }

struct SpriteFileName {
    const char* name;
    const char* alpha;
};

void InjectCommonHooks();

void TransformPoint(RwV3d& point, const CSimpleTransform& placement, const RwV3d& vecPos);
void TransformVectors(RwV3d* vecsOut, int32 numVectors, const CMatrix& matrix, const RwV3d* vecsin);
void TransformVectors(RwV3d* vecsOut, int32 numVectors, const CSimpleTransform& transform, const RwV3d* vecsin);
void TransformPoints(RwV3d* pointOut, int count, const CMatrix& transformMatrix, RwV3d* pointIn);

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
    return angleInDegrees * DEG_TO_RAD;
}

//! @notsa
inline RwTexCoords operator*(RwTexCoords lhs, float rhs) {
    return { lhs.u * rhs, lhs.v * rhs };
}

//! @notsa
inline RwTexCoords operator+(RwTexCoords lhs, RwTexCoords rhs) {
    return { lhs.u + rhs.u, lhs.v + rhs.v };
}

template<typename T, typename Y = float>
struct WeightedValue {
    using value_type = T;

    T v;
    Y w;
};

template<rng::input_range R> // Range of WeightedValue`s
auto multiply_weighted(R&& r) {
    using T = rng::range_value_t<R>::value_type;

    T a{};
    for (const auto& vw : r) {
        a = a + (T)(vw.v * vw.w);
    }
    return a;
}

template<typename T, typename Y = float, size_t N>
auto multiply_weighted(WeightedValue<T, Y> (&&values)[N]) {
    return multiply_weighted(values);
}

// Converts radians to degrees
// 57.295826
constexpr float RadiansToDegrees(float angleInRadians) {
    return angleInRadians * 180.0F / PI;
}

template<typename T>
T lerp(const T& from, const T& to, float t) {
    // Same as `from + (to - from) * t` (Or `from + t * (to - from)`
    return static_cast<T>(to * t + from * (1.f - t));
}

template<>
inline RwRGBA lerp<RwRGBA>(const RwRGBA& from, const RwRGBA& to, float t) {
    return RwRGBA{
        .red   = lerp(from.red, to.red, t),
        .green = lerp(from.green, to.green, t),
        .blue  = lerp(from.blue, to.blue, t),
        .alpha = lerp(from.alpha, to.alpha, t),
    };
}

constexpr float invLerp(float fMin, float fMax, float fVal) {
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
constexpr bool make_fourcc3(const char* line, const char abc[3]) {
    return line[0] == abc[0] && line[1] == abc[1] && line[2] == abc[2];
}

// shit
constexpr bool make_fourcc4(const char* line, const char abcd[4]) {
    return line[0] == abcd[0] && line[1] == abcd[1] && line[2] == abcd[2] && line[3] == abcd[3];
}

// shit
constexpr uint32 MakeFourCC(const char fourcc[4]) {
    return fourcc[0] << 0 | fourcc[1] << 8 | fourcc[2] << 16 | fourcc[3] << 24;
}

char* MakeUpperCase(char *dest, const char *src);
char* MakeUpperCase(char* dest);
bool EndsWith(const char* str, const char* with, bool caseSensitive = true);

RpAtomic* RemoveRefsCB(RpAtomic* atomic, void* _IGNORED_ data);
void RemoveRefsForAtomic(RpClump* clump);

bool GraphicsHighQuality();

/**
 * Writes given raster to PNG file using RtPNGImageWrite
 */
void WriteRaster(RwRaster* raster, const char* filename);
bool CalcScreenCoors(const CVector& in, CVector& out, float& screenX, float& screenY);
bool CalcScreenCoors(const CVector& in, CVector& out);
bool DoesInfiniteLineTouchScreen(CVector2D origin, CVector2D dir);
bool IsPointInsideLine(
    CVector2D origin,
    CVector2D dir,
    CVector2D pt,
    float     radius
);

void LittleTest();

CAnimBlendAssociation* RpAnimBlendClumpGetAssociation(RpClump* clump, std::initializer_list<enum AnimationId> ids);

std::wstring UTF8ToUnicode(const std::string& str);
std::string UnicodeToUTF8(const std::wstring& str);

constexpr int32 TOTAL_TEMP_BUFFER_INDICES = 4096;
constexpr int32 TOTAL_TEMP_BUFFER_3DVERTICES = 2048;
constexpr int32 TOTAL_TEMP_BUFFER_2DVERTICES = 1024;
constexpr int32 TOTAL_RADIOSITY_VERTEX_BUFFER = 1532;

static inline int32 WindowsCharset = static_cast<int32>(GetACP());

struct TempVertexBuffer {
    RwIm3DVertex m_3d[TOTAL_TEMP_BUFFER_3DVERTICES]; // For Im3D rendering
    RwIm2DVertex m_2d[TOTAL_TEMP_BUFFER_2DVERTICES]; // For Im2D rendering
};

static inline auto& uiTempBufferIndicesStored = StaticRef<uint16>(0xC4B954);
static inline auto& uiTempBufferVerticesStored = StaticRef<uint16>(0xC4B950);
static inline auto& aTempBufferIndices = StaticRef<RxVertexIndex[TOTAL_TEMP_BUFFER_INDICES]>(0xC4B958);
static inline auto& TempBufferVertices = StaticRef<TempVertexBuffer>(0xC4D958);
static inline auto& aRadiosityVertexBuffer = StaticRef<RwD3D9Vertex[TOTAL_RADIOSITY_VERTEX_BUFFER]>(0xC5F958);
