#pragma once

#include "RenderWare.h"

#include "Vector.h"
#include "Matrix.h"

class CEscalator {
public:
    CVector  m_vStart;
    CVector  m_vBottom;
    CVector  m_vTop;
    CVector  m_vEnd;
    CMatrix  m_mRotation;
    bool     m_bExist;
    uint8    m_nObjectsCreated; // todo: check offset and size
    bool     m_bMoveDown;
    int32    m_nNumIntermediatePlanes;
    uint32   m_nNumBottomPlanes;
    uint32   m_nNumTopPlanes;
    char     _unused[8];
    RwSphere m_Bounding;
    float    m_nCurrentPosition;
    void*    m_pEntity;
    CObject* m_pObjects[42];

public:
    static void InjectHooks();

    CEscalator();
    ~CEscalator() = default; // 0x717130

    void SwitchOff();
    void AddThisOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecEnd, const CVector& vecTop, bool bMoveDown, CEntity* entity);
    void Update();
};

VALIDATE_SIZE(CEscalator, 0x150);
