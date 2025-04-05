#pragma once

#include "RenderWare.h"

#include "Vector.h"
#include "Matrix.h"

class CEscalator {
public:
    CEscalator() = default; // 0x717110
    ~CEscalator() = default; // 0x717130

    void SwitchOff();
    void AddThisOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecTop, const CVector& vecEnd, bool moveDown, CEntity* entity);
    void Update();

public:
    CVector                  m_vStart;
    CVector                  m_vBottom;
    CVector                  m_vTop;
    CVector                  m_vEnd;
    CMatrix                  m_mRotation{};
    bool                     m_bExist;
    uint8                    m_nStepObjectsCreated{}; // todo: check offset and size
    bool                     m_bMoveDown;
    uint32                   m_nNumIntermediatePlanes;
    uint32                   m_nNumBottomPlanes;
    uint32                   m_nNumTopPlanes;
    char                     _unused[8];
    CSphere                  m_Bounding;
    float                    m_nCurrentPosition;
    CEntity*                 m_pEntity;
    std::array<CObject*, 42> m_pStepObjects{};

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    auto Constructor() { this->CEscalator::CEscalator(); return this; }
    auto Destructor() { this->CEscalator::~CEscalator(); return this; }
};

VALIDATE_SIZE(CEscalator, 0x150);
