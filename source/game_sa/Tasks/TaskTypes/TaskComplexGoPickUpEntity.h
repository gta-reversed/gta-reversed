#pragma once

#include "Base.h"
#include "TaskComplex.h"
#include "Vector.h"
#include "Vehicle.h"
#include <Enums/AnimationEnums.h>

class CPed;
class CTaskComplexGoPickUpEntity;
class CEntity;

class NOTSA_EXPORT_VTABLE CTaskComplexGoPickUpEntity : public CTaskComplex {
public:
    static inline auto& MAX_GOTO_TIME   = StaticRef<int32>(0x8D2FF8);
    static inline auto& MAX_PICKUP_TIME = StaticRef<int32>(0x8D2FFC);

public:
    static constexpr auto Type = TASK_COMPLEX_GO_PICKUP_ENTITY;

    CTaskComplexGoPickUpEntity(CEntity* entity, AssocGroupId animGroupId);
    CTaskComplexGoPickUpEntity(const CTaskComplexGoPickUpEntity& o);
    ~CTaskComplexGoPickUpEntity() override;

    CTask*    Clone() const override { return new CTaskComplexGoPickUpEntity{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    CEntity* GetEntityToPickUp() const { return m_EntityToPickUp; }

    static void InjectHooks();

private:
    notsa::EntityRef<> m_EntityToPickUp{};
    CVector            m_HoldPosition{ -999.f, 0.f, 0.f };   //!< Position of the entity relative to the ped's hand, passed to the hold/pick up tasks
    CVector            m_PickUpPosition{ -999.f, 0.f, 0.f }; //!< Point (in the entity's space) the ped walks to before picking it up
    uint32             m_SubTaskStartTimeMs{};               //!< Time the go-to/pick up sub task was started (for `MAX_GOTO_TIME`/`MAX_PICKUP_TIME`)
    AssocGroupId       m_AnimGroupId{};
    bool               m_IsAnimBlockReferenced{};
};
VALIDATE_SIZE(CTaskComplexGoPickUpEntity, 0x34);
