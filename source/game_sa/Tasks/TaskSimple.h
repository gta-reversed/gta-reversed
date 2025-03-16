/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Task.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimple : public CTask {
public:
    CTaskSimple() = default;
    ~CTaskSimple() override = default;

    CTask* GetSubTask() const final override { return nullptr; }
    bool IsSimple() const final override { return true; }

    /*!
    * @brief Process this task.
    *
    * @param ped The ped this task belongs to
    * 
    * @return `true` if the task has finished, `false` otheriwse.
    */
    virtual bool ProcessPed(CPed* ped) = 0;

    /*!
    * @brief Set the ped's position
    * 
    * @param ped The ped this task belongs to
    *
    * @return If the ped's posiiton was changed
    */
    virtual bool SetPedPosition(CPed* ped) { return false; }
};
VALIDATE_SIZE(CTaskSimple, 0x8);
