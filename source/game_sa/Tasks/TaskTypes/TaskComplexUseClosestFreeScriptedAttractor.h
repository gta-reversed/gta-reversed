#pragma once

#include "TaskComplex.h"

#include "Ped.h"

class CPed;
class C2dEffect;

class NOTSA_EXPORT_VTABLE CTaskComplexUseClosestFreeScriptedAttractor : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR;

    static void InjectHooks();


    CTaskComplexUseClosestFreeScriptedAttractor(/*bool unused = false*/) = default;

    /*!
     * @notsa
    */
    CTaskComplexUseClosestFreeScriptedAttractor(eMoveState ms);

    CTaskComplexUseClosestFreeScriptedAttractor(const CTaskComplexUseClosestFreeScriptedAttractor&);
    ~CTaskComplexUseClosestFreeScriptedAttractor() = default;

    static C2dEffect* ComputeClosestFreeScriptedEffect(CPed const& ped);

    CTask*    Clone()                       const override { return new CTaskComplexUseClosestFreeScriptedAttractor{ *this }; } // 0x636F70
    eTaskType GetTaskType()                 const override { return Type; } // 0x634710
    CTask*    CreateNextSubTask(CPed* ped)        override { return nullptr; } // 0x634730
    CTask*    CreateFirstSubTask(CPed* ped)       override;
    CTask*    ControlSubTask(CPed* ped)           override { return m_pSubTask; } // 0x634890

private: // Wrappers for hooks
    // 0x6346F0
    CTaskComplexUseClosestFreeScriptedAttractor* Constructor(bool) {
        this->CTaskComplexUseClosestFreeScriptedAttractor::CTaskComplexUseClosestFreeScriptedAttractor();
        return this;
    }

    // 0x634720
    CTaskComplexUseClosestFreeScriptedAttractor* Destructor() {
        this->CTaskComplexUseClosestFreeScriptedAttractor::~CTaskComplexUseClosestFreeScriptedAttractor();
        return this;
    }

private:
    eMoveState m_MoveState{PEDMOVE_WALK};
};
