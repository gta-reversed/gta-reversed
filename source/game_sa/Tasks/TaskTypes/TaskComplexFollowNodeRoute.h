#pragma once

#include "TaskComplex.h"
#include "PathFind.h"

class CPointRoute;

class NOTSA_EXPORT_VTABLE CTaskComplexFollowNodeRoute final : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_FOLLOW_NODE_ROUTE;

    CTaskComplexFollowNodeRoute(
        eMoveState     moveState,
        const CVector& targetPt,
        float          targetPtTolerance = 0.5f,
        float          slowDownDist = 3.f,
        float          followNodeThresholdHeightChange = 2.f,
        bool           bKeepNodesHeadingAwayFromTarget = false,
        int32          time = -1,
        bool           bUseBlending = true
    );
    CTaskComplexFollowNodeRoute(const CTaskComplexFollowNodeRoute&);
    ~CTaskComplexFollowNodeRoute() override;

    static eTaskType GetSubTaskType(uint32 progress, bool bLastRoutePointIsTarget, const CPointRoute& route);

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexFollowNodeRoute{*this}; }
    void      StopTimer(const CEvent* event) override;
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    void SetTarget(CPed* ped, const CVector& target, float targetPtTolerance, float slowDownDistance, float followNodeThresholdHeightChange, bool bForce);
    void SetStartNode(CNodeAddress naddr) { m_StartNode = naddr; }

protected:
    CTask*    CreateSubTask(eTaskType taskType, CPed* ped) const;

    CVector   GetLastWaypoint(CPed* ped) const;
    CVector   GetNextWaypoint(CPed* ped) const;
    eTaskType CalcGoToTaskType(CPed* ped, eTaskType taskType);
    float     CalcBlendRatio(CPed* ped, bool bUsePointRoute) const;
    bool      CanGoStraightThere(const CPed* ped, const CVector& from, const CVector& to, float maxDist) const;
    void      ComputePathNodes(const CPed* ped);
    void      ComputeRoute();
    auto      GetCurrentPt() const { return (*m_PtRoute)[m_CurrPtIdx]; }

public:
    CVector      m_TargetPt{};                            //< Point we're trying to get to
    eMoveState   m_MoveState{};                           //< Move state to use to get to this point
    float        m_TargetTolerance{};                     //< Tolerance [Determinates how close the ped has to get to the point for the task to be considered as finished]
    float        m_SlowDownDist{};                        //< Used for `CTaskComplexGoToPointAndStandStill`
    float        m_FollowNodeThresholdHeightChange{};     //< Determinates if the ped should go straight, or take some extra steps to avoid falling off and shit [I guess]
    CNodeAddress m_StartNode{};                           //< The node the computed route starts from [See `ComputePathNodes`]
    CNodeRoute*  m_NodeRoute{};                           //< The route to follow [See `ComputePathNodes`]
    CPointRoute* m_PtRoute{};                             //< The node route but as points instead [See `ComputeRoute`] - This should be a 1:1 translation, unless a node's area isn't loaded
    CNodeAddress m_CurrNode{};                            //< The node we're currently at
    uint32       m_CurrPtIdx{};                           //< Current point we're at [If using `m_PtRoute`]
    int32        m_TimeToFinishMs{};                      //< The the we have to finish going to the point. If not enough time, the ped is teleported and the task is finished
    CTaskTimer   m_FinishedTimer{};                       //< Timer for finishing the task [See `m_TimeToFinishMs`]
    bool         m_bKeepNodesHeadingAwayFromTarget : 1{}; //< Not sure
    bool         m_LastRoutePointIsTarget : 1{};          //< If last entry in `NodeRoute` corresponds to `m_TargetPt`
    bool         m_bNewTarget : 1{};                      //< If a new target was set using `SetTarget`
    bool         m_bUseBlending : 1{};                    //< If the task should go rough or fine [by using blending] - It takes more time to finish the task if blending is used
    bool         m_bWillSlowDown : 1{};                   //< Used for blending
    bool         m_bSlowingDown : 1{};                    //< Used for blending
    bool         m_bSpeedingUp : 1{};                     //< Used for blending
    float        m_SpeedDecreaseDist{};                   //< Used for blending
    float        m_SpeedIncreaseDist{};                   //< Used for blending
    float        m_SpeedDecreaseAmt{};                    //< Used for blending
    float        m_SpeedIncreaseAmt{};                    //< Used for blending

private: // Hook shit, ignore
    friend void InjectHooksMain();
    static void InjectHooks();

    // 0x66EA30
    CTaskComplexFollowNodeRoute* Constructor(eMoveState moveState, CVector const& targetPoint, float radius, float a5, float a6, bool a7, int32 time, bool a9) {
        this->CTaskComplexFollowNodeRoute::CTaskComplexFollowNodeRoute(moveState, targetPoint, radius, a5, a6, a7, time, a9);
        return this;
    }

    // 0x66EB70
    CTaskComplexFollowNodeRoute* Destructor() {
        this->CTaskComplexFollowNodeRoute::~CTaskComplexFollowNodeRoute();
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexFollowNodeRoute, 0x60);
