#pragma once

#include "TaskSimple.h"
#include <extensions/EntityRef.hpp>

class NOTSA_EXPORT_VTABLE CTaskLeanOnVehicle : public CTaskSimple {
public:
    static constexpr auto Type = TASK_COMPLEX_LEAN_ON_VEHICLE;

    CTaskLeanOnVehicle(CVehicle* vehicle, int32 leanAnimDurationInMs, bool skipLeanIn);
    ~CTaskLeanOnVehicle() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskLeanOnVehicle(m_Vehicle, m_LeanAnimDurationInMs, m_SkipLeanIn); } // 0x6610A0
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

    static void InjectHooks();

private:
    static void FinishAnimCB(CAnimBlendAssociation* assoc, void* data);

private:
    notsa::EntityRef<CVehicle> m_Vehicle{};
    int32                      m_LeanAnimDurationInMs{};
    bool                       m_SkipLeanIn{}; //!< Start with the idle anim instead of `ANIM_ID_LEANIN`
    CAnimBlendAssociation*     m_LeanAnim{};
    int32                      m_LastAnimId{ ANIM_ID_UNDEFINED }; //!< Anim id of the last finished anim
    CTaskTimer                 m_LeanTimer{};
    bool                       m_IsFinished{};
    bool                       m_IsLeanOutRequested{}; //!< Set by a non-immediate `MakeAbortable`
    bool                       m_StopLeaning{};
};
VALIDATE_SIZE(CTaskLeanOnVehicle, 0x2C);
