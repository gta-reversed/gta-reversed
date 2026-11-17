#pragma once

#include <extensions/EntityRef.hpp>
#include "TaskComplex.h"
#include "Vector.h"

class CPointRoute;

class NOTSA_EXPORT_VTABLE CTaskComplexWalkRoundObject : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_WALK_ROUND_OBJECT;

    CTaskComplexWalkRoundObject(int32 moveState, const CVector& targetPoint, CEntity* object);
    ~CTaskComplexWalkRoundObject() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexWalkRoundObject(m_moveState, m_targetPoint, m_object); }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    CTask* CreateRouteTask(CPed* ped) const;
    float  ComputeRoute(CPed* ped) const;

    CEntity* GetObject() const { return m_object; }

public:
    static void InjectHooks();

private:
    int32              m_moveState{};
    CVector            m_targetPoint{};
    notsa::EntityRef<> m_object{};
    CPointRoute*       m_pointRoute{};
    CTaskTimer         m_Timer{};
    CVector            m_ObjectPos{};
    CVector            m_ObjectForwardsVec{};
    CVector            m_ObjectRightVec{};
};

VALIDATE_SIZE(CTaskComplexWalkRoundObject, 0x54);
