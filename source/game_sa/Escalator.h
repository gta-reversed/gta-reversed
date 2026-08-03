#pragma once

#include "RenderWare.h"

#include "Vector.h"
#include "Matrix.h"
#include "extensions/EntityRef.hpp"

class CEscalator {
public:
    CEscalator() = default; // 0x717110
    ~CEscalator() = default; // 0x717130

    void SwitchOff();
    void AddThisOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecTop, const CVector& vecEnd, bool moveDown, CEntity* entity);
    void Update();

    // Helpers
    CEntity* GetEntity() const {
        return m_Entity.Get();
    }

    void Remove() {
        SwitchOff();
        m_Exist = false;
    }

private:
    friend class CEscalators;

    CVector                  m_Start;
    CVector                  m_Bottom;
    CVector                  m_Top;
    CVector                  m_End;
    CMatrix                  m_Rotation{};
    bool                     m_Exist;
    uint8                    m_StepObjectsCreated{};
    bool                     m_MoveDown;
    uint32                   m_NumIntermediatePlanes;
    uint32                   m_NumBottomPlanes;
    uint32                   m_NumTopPlanes;
    char                     _unused[8];
    CSphere                  m_Bounding;
    float                    m_CurrentPosition;
    notsa::EntityRef<>       m_Entity;
    std::array<CObject*, 42> m_StepObjects{};

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    auto Constructor() { this->CEscalator::CEscalator(); return this; }
    auto Destructor() { this->CEscalator::~CEscalator(); return this; }
};

VALIDATE_SIZE(CEscalator, 0x150);
