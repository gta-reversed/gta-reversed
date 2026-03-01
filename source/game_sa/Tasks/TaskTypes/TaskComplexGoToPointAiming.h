#pragma once

#include "TaskComplex.h"
class CEntity;

class NOTSA_EXPORT_VTABLE CTaskComplexGoToPointAiming : public CTaskComplex {
public:
    eMoveState m_moveState{};
    CEntity* m_aimAtEntity{};
    CVector m_aimPos{};
    CVector m_movePos{};
    float m_moveTargetRadius{};
    float m_slowDownDistance{};
    bool m_newTargetSet : 1{};

public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_GO_TO_POINT_AIMING;

    CTaskComplexGoToPointAiming(
        eMoveState moveState,
        const CVector& movePos,
        CEntity* aimAtEntity,
        CVector aimPos,
        const float targetRadius,
        const float slowDownDist
    );
    CTaskComplexGoToPointAiming(const CTaskComplexGoToPointAiming&);
    ~CTaskComplexGoToPointAiming() override;

    eTaskType GetTaskType() const override { return Type; } // 0x668860
    CTask* Clone() const override { return new CTaskComplexGoToPointAiming{ *this }; } // 0x66CD80
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateSubTask(eTaskType taskType);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    // 0x668790
    CTaskComplexGoToPointAiming* Constructor(eMoveState moveState, CVector const& movePos, CEntity* aimAtEntity, CVector aimPos, float targetRadius, float slowDownDist) {
        this->CTaskComplexGoToPointAiming::CTaskComplexGoToPointAiming(moveState, movePos, aimAtEntity, aimPos, targetRadius, slowDownDist);
        return this;
    }
    // 0x668870
    CTaskComplexGoToPointAiming* Destructor() {
        this->CTaskComplexGoToPointAiming::~CTaskComplexGoToPointAiming();
        return this;
    }

};
