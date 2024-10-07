#pragma once

#include "Vector.h"

class ProcObjectListItem;
class CPlantLocTri;

class ProcSurfaceInfo_c {
public:
    uint8  m_SurfaceId;
    uint8  field_1;
    uint8  field_2;
    uint8  field_3;
    int32  m_ModelIndex;
    float  m_fSpacing;
    float  m_fSquaredSpacingRadius;
    float  m_fSquaredMinDistance;
    float  m_fMinRotInRadians;
    float  m_fMaxRotInRadians;
    float  m_fMinScale;
    float  m_fMaxScale;
    float  m_fMinScaleZ;
    float  m_fMaxScaleZ;
    float  m_fOffsetMinZ;
    float  m_fOffsetMaxZ;
    bool   m_Align;
    float  m_fUseGrid;
    TList_c<ProcObjectListItem> m_Objects;

public:
    static void InjectHooks();

    ProcSurfaceInfo_c() = default;  // 0x5A2E90
    ~ProcSurfaceInfo_c() = default; // 0x5A2EA0

    void Init(const char* surfaceType, const char* modelName,
              float spacing, float minDist,
              int32 minRot, int32 maxRot,
              float minScale, float maxScale,
              float minScaleZ, float maxScaleZ,
              float zOffsetMin, float zOffsetMax,
              bool align, uint8 useGrid
    );
    void Exit();

    void AddObject(CVector p1, CVector p2, bool a3);
    int32 AddObjects(CPlantLocTri* plant);

};
VALIDATE_SIZE(ProcSurfaceInfo_c, 0x48);
