#pragma once

#include "TaskComplex.h"
#include "Enums/eMoveState.h"

class C2dEffectPedAttractor;
class CEntity;
class CPedAttractor;
class CEvent;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexUseEffect : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_EFFECT;

    static void InjectHooks();

    CTaskComplexUseEffect(C2dEffectPedAttractor* fx, CEntity* entity);
    CTaskComplexUseEffect(const CTaskComplexUseEffect&);
    ~CTaskComplexUseEffect() override;

    CTask*    Clone() const override { return new CTaskComplexUseEffect{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) final override;
    CTask*    CreateNextSubTask(CPed* ped) final override;
    CTask*    CreateFirstSubTask(CPed* ped) final override;
    CTask*    ControlSubTask(CPed* ped) final override;

private: // Wrappers for hooks
    // 0x6321F0
    CTaskComplexUseEffect* Constructor(C2dEffectPedAttractor* fx, CEntity* entity) {
        this->CTaskComplexUseEffect::CTaskComplexUseEffect(fx, entity);
        return this;
    }

    // 0x632270
    CTaskComplexUseEffect* Destructor() {
        this->CTaskComplexUseEffect::~CTaskComplexUseEffect();
        return this;
    }

public:
    C2dEffectPedAttractor* m_2dFx{};
    CEntity*               m_Entity{};
    CPedAttractor*         m_PedAttractor{};
    CPed*                  m_Ped{};
    eMoveState             m_MoveState{ eMoveState::PEDMOVE_WALK };
    bool                   m_bAbort{};
};
