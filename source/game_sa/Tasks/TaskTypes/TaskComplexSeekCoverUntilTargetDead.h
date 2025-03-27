#pragma once

#include "TaskComplex.h"

class CPed;
class CTaskComplexSeekCoverUntilTargetDead;

using PedGroupId = int32;

/*!
* This task works 2 ways, depending on which constructor is used:
* - `CTaskComplexSeekCoverUntilTargetDead(CPed*)`
     => The given ped is covered, and all the task owner ped's group members (if any).
* - `CTaskComplexSeekCoverUntilTargetDead(PedGroupId)`
*   => Any member of the ped's group is considered a "cover ped",
*      and every member of the specified group are considered as targets.
*   
*/
class NOTSA_EXPORT_VTABLE CTaskComplexSeekCoverUntilTargetDead : public CTaskComplex {
public:
    PedGroupId m_groupId{-1};
    CPed*      m_targetPed{};
    CPed*      m_coverPed{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SEEK_COVER_UNTIL_TARGET_DEAD;

    explicit CTaskComplexSeekCoverUntilTargetDead(CPed* ped);
    explicit CTaskComplexSeekCoverUntilTargetDead(PedGroupId grpId);
    CTaskComplexSeekCoverUntilTargetDead(const CTaskComplexSeekCoverUntilTargetDead&);
    ~CTaskComplexSeekCoverUntilTargetDead();

    CPed* GetCoverPed(CPed* ped);
    CPed* GetTargetPed();

    CTask*    Clone() const override { return new CTaskComplexSeekCoverUntilTargetDead{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private:
    CTaskComplexSequence* CreateSequence(CPed* ped); // NOTSA

private: // Wrappers for hooks
    // 0x622BC0
    CTaskComplexSeekCoverUntilTargetDead* Constructor_Ped(CPed* ped) {
        this->CTaskComplexSeekCoverUntilTargetDead::CTaskComplexSeekCoverUntilTargetDead(ped);
        return this;
    }
    // 0x622B80
    CTaskComplexSeekCoverUntilTargetDead* Constructor_Grp(PedGroupId grp) {
        this->CTaskComplexSeekCoverUntilTargetDead::CTaskComplexSeekCoverUntilTargetDead(grp);
        return this;
    }
    // 0x622C20
    CTaskComplexSeekCoverUntilTargetDead* Destructor() {
        this->CTaskComplexSeekCoverUntilTargetDead::~CTaskComplexSeekCoverUntilTargetDead();
        return this;
    }
};

