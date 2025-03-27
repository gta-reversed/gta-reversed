#pragma once

#include "TaskTimer.h"
#include "TaskComplex.h"

class CTaskComplexStareAtPed;
class CPed;
class CPedGroup;

class NOTSA_EXPORT_VTABLE CTaskComplexStareAtPed : public CTaskComplex {

public:
    CPedGroup*  m_pPedGroup{};
    CPed*       m_pPed{};
    int32       m_timeout{};
    CTaskTimer  m_timer{};
    bool        m_lookInitialised{};
    bool        m_animsReferenced{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_STARE_AT_PED;

    CTaskComplexStareAtPed(CPedGroup* pedGroip, CPed* pedToStareAt, int32 timeout);
    CTaskComplexStareAtPed(const CTaskComplexStareAtPed&);
    ~CTaskComplexStareAtPed();

    CTask*    Clone() const override { return new CTaskComplexStareAtPed{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

protected:
    void UnrefAnimBlock();
    void AbortIK(CPed* ped);
private: // Wrappers for hooks
    // 0x660270
    CTaskComplexStareAtPed* Constructor(CPedGroup* pedGroip, CPed* pedToStareAt, int32 timeout) {
        this->CTaskComplexStareAtPed::CTaskComplexStareAtPed(pedGroip, pedToStareAt, timeout);
        return this;
    }
    // 0x660310
    CTaskComplexStareAtPed* Destructor() {
        this->CTaskComplexStareAtPed::~CTaskComplexStareAtPed();
        return this;
    }
};
