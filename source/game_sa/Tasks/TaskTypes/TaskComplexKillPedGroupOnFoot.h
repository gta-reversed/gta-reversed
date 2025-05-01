#pragma once

#include "TaskComplex.h"
#include "Ped.h"

class CTaskComplexKillPedGroupOnFoot;
class CPed;

/*!
* Kill all members of a given group identified by it's ID.
*/
class NOTSA_EXPORT_VTABLE CTaskComplexKillPedGroupOnFoot : public CTaskComplex {
public:
    int32 m_grpId{}; //< The group to kill members of
    CPed* m_currPedToKill{}; //< Ped from the group we're currently trying to kill

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_KILL_PED_GROUP_ON_FOOT;

    /*!
    * @param groupId The group to kill all members of
    * @param ped     Ped to kill. If they're part of the same group as the owner of this task the task will finish immidiately.
    * @addr 0x622240
    */
    CTaskComplexKillPedGroupOnFoot(int32 groupId, CPed* ped);
    CTaskComplexKillPedGroupOnFoot(const CTaskComplexKillPedGroupOnFoot&);
    ~CTaskComplexKillPedGroupOnFoot();

    CTask*    CreateSubTask(eTaskType taskType, CPed* ped);
    CPed*     ComputeNearestLivingGroupPed(CPed& ped, bool flag = false);
    eTaskType ComputeNewTargetAndTaskType(CPed* ped); // NOTSA

    CTask*    Clone() const override { return new CTaskComplexKillPedGroupOnFoot{ *this }; } // 0x6237C0
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; } // 0x0x622310
 
private: // Wrappers for hooks
    // 0x622240
    CTaskComplexKillPedGroupOnFoot* Constructor(int32 groupId, CPed* ped) {
        this->CTaskComplexKillPedGroupOnFoot::CTaskComplexKillPedGroupOnFoot(groupId, ped);
        return this;
    }

    // 0x6222B0
    CTaskComplexKillPedGroupOnFoot* Destructor() {
        this->CTaskComplexKillPedGroupOnFoot::~CTaskComplexKillPedGroupOnFoot();
        return this;
    }
};
