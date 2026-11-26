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

    static void InjectHooks();

    CTaskComplexGoPickUpEntity(CEntity* pEntity, AssocGroupId animGroupId);
    CTaskComplexGoPickUpEntity(const CTaskComplexGoPickUpEntity& o);
    ~CTaskComplexGoPickUpEntity();

    CTask*    Clone() const override { return new CTaskComplexGoPickUpEntity{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    CEntity* GetEntityToPickUp() const { return m_EntityToPickUp; }

private:
public:
    notsa::EntityRef<> m_EntityToPickUp{};
    CVector            m_Position{ -999.f, 0.f, 0.f };
    CVector            m_PickupPosition{ -999.f, 0.f, 0.f };
    int32              m_TimePassedSinceLastSubTaskCreatedInMs{};
    AssocGroupId       m_AnimGroupId{};
    bool               m_IsAnimBlockReferenced{};
};
