#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"

class CTaskComplexWaitForBackup;
class CPed;
class CTaskSimpleGunControl;

class NOTSA_EXPORT_VTABLE CTaskComplexWaitForBackup : public CTaskComplex {
public:
    float      m_waitDistance{};
    CPed*      m_aggressor{};
    CPed*      m_backup{};
    CTaskTimer m_timer{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_WAIT_FOR_BACKUP;

    CTaskComplexWaitForBackup(CPed* attacker, CPed* backup, float waitDist);
    CTaskComplexWaitForBackup(const CTaskComplexWaitForBackup&);
    ~CTaskComplexWaitForBackup();

    CTask* CreateSubTask(eTaskType taskType, CPed* a3);

    CTask*    Clone() const override { return new CTaskComplexWaitForBackup{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x697290
    CTaskComplexWaitForBackup* Constructor(CPed* aggressor, CPed* backup, float waitDist) {
        this->CTaskComplexWaitForBackup::CTaskComplexWaitForBackup(aggressor, backup, waitDist);
        return this;
    }
    // 0x697330
    CTaskComplexWaitForBackup* Destructor() {
        this->CTaskComplexWaitForBackup::~CTaskComplexWaitForBackup();
        return this;
    }
};
