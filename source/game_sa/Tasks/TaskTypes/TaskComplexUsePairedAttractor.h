#pragma once

#include "TaskComplex.h"

class CTaskComplexUsePairedAttractor;
class CTask;
class CPed;
class CPedAttractor;
class CScriptedEffectPair;


class NOTSA_EXPORT_VTABLE CTaskComplexUsePairedAttractor : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_PAIRED_ATTRACTOR;

    static void InjectHooks();

    CTaskComplexUsePairedAttractor(CPedAttractor*);
    CTaskComplexUsePairedAttractor(const CTaskComplexUsePairedAttractor&);
    ~CTaskComplexUsePairedAttractor() override = default;

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexUsePairedAttractor{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6331A0
    CTaskComplexUsePairedAttractor* Constructor(CPedAttractor* a) {
        this->CTaskComplexUsePairedAttractor::CTaskComplexUsePairedAttractor(a);
        return this;
    }

    // 0x6331E0
    CTaskComplexUsePairedAttractor* Destructor() {
        this->CTaskComplexUsePairedAttractor::~CTaskComplexUsePairedAttractor();
        return this;
    }

public:
    CPedAttractor*             m_Attractor{};
    CPed*                      m_Partner{};
    bool                       m_IsLeader{};
    const CScriptedEffectPair* m_CurrentFxPair{};
    int32                      m_PartnerProgress{};
    int32                      m_SoloProgress{};
};
