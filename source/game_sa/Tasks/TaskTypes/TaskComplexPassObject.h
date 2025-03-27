#pragma once

#include "TaskTimer.h"
#include "TaskComplex.h"
#include "Vector.h"

class CEvent;
class CPed;
class CEntity;
class CTaskComplexPassObject;

class NOTSA_EXPORT_VTABLE CTaskComplexPassObject : public CTaskComplex {

public:
    CPed*      m_ped{};             
    bool       m_isPasser{};        
    CVector    m_targetPos{};     
    CTaskTimer m_timer{};      
    CEntity*   m_entityToPass{}; 

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_PASS_OBJECT;

    CTaskComplexPassObject(CPed* ped, bool isPasser);
    CTaskComplexPassObject(const CTaskComplexPassObject&);
    ~CTaskComplexPassObject();

    void AbortIK(CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexPassObject{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6611B0
    CTaskComplexPassObject* Constructor(CPed* ped, uint8 isPasser) {
        this->CTaskComplexPassObject::CTaskComplexPassObject(ped, isPasser);
        return this;
    }
    // 0x661230
    CTaskComplexPassObject* Destructor() {
        this->CTaskComplexPassObject::~CTaskComplexPassObject();
        return this;
    }
};
