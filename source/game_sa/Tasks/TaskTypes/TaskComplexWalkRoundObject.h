#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"
#include "Vector.h"

class CPointRoute;

class NOTSA_EXPORT_VTABLE CTaskComplexWalkRoundObject : public CTaskComplex {
public:
    int32        m_moveState;
    CVector      m_targetPoint;
    CEntity*     m_object;
    CPointRoute* m_pointRoute;
    CTaskTimer   m_timer;         //!< Gives up following the route around the object when out of time
    CVector      m_objectPos;     //!< Object's position when the route was computed (if it moves, the route is invalid)
    CVector      m_objectForward; //!< Object's forward vector when the route was computed
    CVector      m_objectRight;   //!< Object's right vector when the route was computed

public:
    static constexpr auto Type = TASK_COMPLEX_WALK_ROUND_OBJECT;

    CTaskComplexWalkRoundObject(int32 moveState, const CVector& targetPoint, CEntity* object);
    ~CTaskComplexWalkRoundObject() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexWalkRoundObject(m_moveState, m_targetPoint, m_object); }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    CTask* CreateRouteTask(CPed*) const;
    CTask* CreateSubTask(eTaskType taskType, CPed* ped) const;
    float  ComputeRouteAndDistToHitSide(CPed* ped);

public:
    static void InjectHooks();
};

VALIDATE_SIZE(CTaskComplexWalkRoundObject, 0x54);
