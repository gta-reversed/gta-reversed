#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexGoToAttractor : public CTaskComplex {
public:
    CPedAttractor* m_Attractor;
    CVector        m_vecAttrPosn;
    float          m_fAttrHeading;
    float          m_fAttrTime;
    int32          m_nQueueNumber;
    int32          field_28;

public:
    static constexpr auto Type = TASK_COMPLEX_GO_TO_ATTRACTOR;

    CTaskComplexGoToAttractor(CPedAttractor* attractor, CVector const& pos, float heading, float attrTime, int32 queueNumber, int32 a7);
    ~CTaskComplexGoToAttractor() override = default; // 0x66B6A0

    eTaskType GetTaskType() const override { return Type;  }
    CTask* Clone() const override { return new CTaskComplexGoToAttractor(m_Attractor, m_vecAttrPosn, m_fAttrHeading, m_fAttrTime, m_nQueueNumber, field_28); } // 0x66D130
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskComplexGoToAttractor* Constructor(CPedAttractor* attractor, CVector const& attrPosn, float attrHeading, float attrTime, int32 queueNumber, int32 a7) { this->CTaskComplexGoToAttractor::CTaskComplexGoToAttractor(attractor, attrPosn, attrHeading, attrTime, queueNumber, a7); return this; }
    CTaskComplexGoToAttractor* Destructor() { this->CTaskComplexGoToAttractor::~CTaskComplexGoToAttractor(); return this; }
};
VALIDATE_SIZE(CTaskComplexGoToAttractor, 0x2C);
