#pragma once

#include "TaskSimple.h"
#include "Entity.h"
#include <extensions/EntityRef.hpp>

class NOTSA_EXPORT_VTABLE CTaskSimpleTriggerLookAt : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_TRIGGER_LOOK_AT;

    CTaskSimpleTriggerLookAt(CEntity* entity, int32 time, eBoneTag32 offsetBoneTag, RwV3d offsetPos, bool bUseTorso = true, float speed = 0.25f, int32 blendTime = 1'000, int32 priority = 3);
    CTaskSimpleTriggerLookAt(const CTaskSimpleTriggerLookAt&);
    ~CTaskSimpleTriggerLookAt() override = default;

    CTask*    Clone() const override { return new CTaskSimpleTriggerLookAt{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; }
    bool      ProcessPed(CPed* ped) override;

public:
    static void InjectHooks();

private:
    notsa::EntityRef<> m_Entity{};
    int32              m_Time{};
    eBoneTag32         m_OffsetBoneTag{};
    CVector            m_OffsetPos{};
    bool               m_UseTorso{};
    float              m_Speed{};
    int32              m_BlendTime{};
    bool               m_NonNullEntity{};
    int8               m_Priority{};
};
VALIDATE_SIZE(CTaskSimpleTriggerLookAt, 0x30);
