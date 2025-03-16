#pragma once

#include "TaskSimple.h"
#include "Enums/eMoveState.h"
#include "Vector.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleGoTo : public CTaskSimple {
public:
    eMoveState m_moveState;
    CVector    m_vecTargetPoint;
    float      m_fRadius;
    union {
        struct 
        {
            uint8 m_targetCircledFlags : 4;
            uint8 m_bIsIKChainSet : 1;
            uint8 m_bTargetPointUpdated : 1;
        } gotoFlags;
        uint8 m_GoToFlags;
    };

    static float& ms_fLookAtThresholdDotProduct;

public:
    CTaskSimpleGoTo(eMoveState moveState, const CVector& targetPoint, float fRadius);
    ~CTaskSimpleGoTo() override = default; // 0x667A00

    bool HasCircledTarget(CPed* ped);
    void SetUpIK(CPed* ped);
    void QuitIK(CPed* ped);

    auto GetTargetPt()           const { return m_vecTargetPoint; }
    void SetTargetPt(CVector pt)       { m_vecTargetPoint = pt;   }

    auto GetTargetPtRadius()        const { return m_fRadius; }
    void SetTargetPtRadius(float r)       { m_fRadius = r;    }


private:
    friend void InjectHooksMain();
    static void InjectHooks();

    // 0x6679C0
    auto Constructor(eMoveState moveState, const CVector& targetPoint, float fRadius) {
        this->CTaskSimpleGoTo::CTaskSimpleGoTo(moveState, targetPoint, fRadius);
        return this;
    }
};
VALIDATE_SIZE(CTaskSimpleGoTo, 0x20);
