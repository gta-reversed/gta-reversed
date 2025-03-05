#pragma once

#include "common.h"
#include "Vector2D.h"
#include "Vector.h"

class CActiveOccluder;

class COccluder {
public:
    static inline auto& MaxYInOccluder        = StaticRef<float>(0xC73CA0);
    static inline auto& MinYInOccluder        = StaticRef<float>(0xC73CA4);
    static inline auto& MaxXInOccluder        = StaticRef<float>(0xC73CA8);
    static inline auto& MinXInOccluder        = StaticRef<float>(0xC73CAC);

    static inline auto& OccluderCoorsValid    = StaticRef<std::array<bool, 8>>(0xC73CB0);
    static inline auto& OccluderCoors         = StaticRef<std::array<CVector, 8>>(0xC798E0);
    static inline auto& OccluderCoorsOnScreen = StaticRef<std::array<CVector, 8>>(0xC79950);
    static inline auto& CenterOnScreen        = StaticRef<CVector>(0xC79940);

public:
    static void InjectHooks();

    bool ProcessOneOccluder(CActiveOccluder* activeOccluder);
    bool ProcessLineSegment(int32 iInd1, int32 iInd2, CActiveOccluder* activeOccluder);
    bool NearCamera() const;
    int16 SetNext(int16 next);
    int16 GetNext() const { return m_NextIndex; }

public:
    FixedVector<int16, 4.f> m_Center{};

    // As usual, some R* dev from 2003 decided to swap length and width
    // that is, in the structure, it's `m_Length, m_Width, m_Height`
    // so converting it all into one FixedVector would be fucking confusing
    // (as `size.x` would not be `m_Width` but rather `m_Length`
    // TODO(In standalone): Replace this with a vector and fix the abovementioend issue.
    FixedFloat<int16, 4.f> m_Length;
    FixedFloat<int16, 4.f> m_Width;
    FixedFloat<int16, 4.f> m_Height;

    FixedVector<uint8, TWO_PI_OVER_256> m_Rot{};
    
    struct {
        int16 m_NextIndex : 15;
        int16 m_DontStream : 1;
    };
};
VALIDATE_SIZE(COccluder, 0x12);
