#pragma once

class CScriptedEffectPair;
class CPed;
class CTaskComplexUseAttractorPartner;

class CTaskComplexUseAttractorPartner final : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_ATTRACTOR_PARTNER;

    static void InjectHooks();

    CTaskComplexUseAttractorPartner(bool isLeader, const CScriptedEffectPair& pair);
    CTaskComplexUseAttractorPartner(const CTaskComplexUseAttractorPartner&);
    ~CTaskComplexUseAttractorPartner() override = default;

    CTask* Clone() const override { return new CTaskComplexUseAttractorPartner{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask* CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x6332C0
    CTaskComplexUseAttractorPartner* Constructor(bool a, const CScriptedEffectPair& b) {
        this->CTaskComplexUseAttractorPartner::CTaskComplexUseAttractorPartner(a, b);
        return this;
    }

    // 0x633300
    CTaskComplexUseAttractorPartner* Destructor() {
        this->CTaskComplexUseAttractorPartner::~CTaskComplexUseAttractorPartner();
        return this;
    }

protected:
    bool                       m_IsLeader;
    const CScriptedEffectPair* m_CurrentFxPair;
};
