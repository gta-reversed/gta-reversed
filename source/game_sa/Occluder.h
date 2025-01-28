#pragma once

#include "Vector2D.h"
#include "Vector.h"

class CActiveOccluder;

class COccluder {
public:
    static void InjectHooks();

    bool ProcessOneOccluder(CActiveOccluder* activeOccluder);
    bool ProcessLineSegment(int32 iInd1, int32 iInd2, CActiveOccluder* activeOccluder);
    bool NearCamera() const;
    int16 SetNext(int16 next);
    int16 GetNext() const { return m_NextIndex; }

public:
    int16 m_MidX;
    int16 m_MidY;
    int16 m_MidZ;
    int16 m_Length;
    int16 m_Width;
    int16 m_Height;
    uint8 m_RotZ;
    uint8 m_RotY;
    uint8 m_RotX;
    char  _pad;
    struct {
        int16 m_NextIndex : 15;
        int16 m_DontStream : 1;
    };
};
VALIDATE_SIZE(COccluder, 0x12);
