#pragma once

#include "TaskComplex.h"
#include "Ped.h" // eMoveState
#include "PedGeometryAnalyser.h" // CPointRoute

class CVector;
class CTaskSimpleStandStill;
class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskComplexFollowPointRoute : public CTaskComplex {
public:
    enum class Mode : uint32 {
        ONE_WAY         = 0, ///< Do the route only once
        RETURN          = 1, ///< Follow route once, then follow it in reverse
        BACK_AND_FORTH  = 2, ///< Same as `RETURN`, but looped forever.
        LOOP            = 3, ///< Loop follow route, but always in the same direction (when end is reached, ped goes back to the first point, but unlike `BACK_AND_FORTH` it won't follow the original route in reverse)
    };

public:
    Mode         m_mode{};
    int32        m_nRouteTraversals{};
    eMoveState   m_moveState{};
    float        m_targetRadius{};
    float        m_slowDownDistance{};
    CPointRoute* m_route{};
    uint32       m_currPointIdx{};

    bool         m_bMustOvershootTarget : 1{};
    bool         m_bNewRoute : 1{true};
    bool         m_bStandStillAfterMove : 1{};
    bool         m_bUseBlending : 1{};
    bool         m_bWillSlowDown : 1{};
    bool         m_bSlowingDown : 1{};
    bool         m_bSpeedingUp : 1{};

    // For the below values see `CalcBlendRatio`
    float        m_speedDecreaseDist{};
    float        m_speedIncreaseDist{};

    float        m_speedDecreaseAmt{};
    float        m_speedIncreaseAmt{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_FOLLOW_POINT_ROUTE;

    CTaskComplexFollowPointRoute(
        eMoveState         moveState,
        CPointRoute const& route,
        Mode               mode                 = Mode::ONE_WAY,
        float              targetRadius         = 0.5f,
        float              slowDownDist         = 0.f,
        bool               overshootTarget      = true,
        bool               useBlending          = false,
        bool               standStillAfterMove  = false
    );
    CTaskComplexFollowPointRoute(const CTaskComplexFollowPointRoute&);
    ~CTaskComplexFollowPointRoute() override;

    eTaskType GetSubTaskType();
    CTask*    CreateTask(eTaskType taskType, CPed* ped);
    CVector   GetLastWaypoint(CPed* ped);
    CVector   GetNextWaypoint(CPed* ped);
    eTaskType CalcGoToTaskType(CPed* ped, eTaskType subTaskType);
    float     CalcBlendRatio(CPed* ped, bool slowing);

    CTask*    Clone() const override { return new CTaskComplexFollowPointRoute{*this}; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x671510
    CTaskComplexFollowPointRoute* Constructor(eMoveState moveState, CPointRoute const& route, Mode mode, float targetRadius, float slowDownDist, bool overshootTarget, bool useBlending,
                                              bool standStillAfterMove) {
        this->CTaskComplexFollowPointRoute::CTaskComplexFollowPointRoute(moveState, route, mode, targetRadius, slowDownDist, overshootTarget, useBlending, standStillAfterMove);
        return this;
    }
};
