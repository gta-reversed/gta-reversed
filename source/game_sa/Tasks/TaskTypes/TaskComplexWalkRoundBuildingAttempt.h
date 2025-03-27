#pragma once

#include "Ped.h"
#include "TaskComplex.h"
#include "Vector.h"

class CPointRoute;
class CPed;
class CTaskComplexWalkRoundBuildingAttempt;
class CEntity;
class CEvent;
class CTask;

class NOTSA_EXPORT_VTABLE CTaskComplexWalkRoundBuildingAttempt : public CTaskComplex {

public:
    CPointRoute* m_route = new CPointRoute{};
    CPointRoute* m_crapRoute = new CPointRoute{};
    CVector      m_targetPos{};                     /// = m_targetEntity->GetMatrix() * m_offset if `m_flag0x4` else value from constructor
    CVector      m_pos{};                           ///< Some position
    CVector      m_normal{};                        ///< Some kind of normal (maybe the buildings?)
    CEntity*     m_targetEntity{};                  ///< If set it's used instead `m_target`
    CVector      m_offset{};                        ///< If `m_targetEntity` is set this is used as an offset to the entity's position
    int8         m_moveState{};                     ///< Type is: `eMoveState`
    bool         m_bIsHeadOnCollision : 1{};        ///< todo...
    bool         m_routeHasPoints : 1{};            ///< Whenever `m_route` has points
    bool         m_isWalkingAroundEntity : 1{};     ///< If set `m_targetEntity` should be used as the target, otherise `m_targetPos`
    bool         m_crapRouteHasPoints : 1{};        /// If `m_crapRoute` has any points added to it
    bool         m_justAbort : 1{};                 ///< Whenever the task has to be aborted (In case target entity was deleted, or similar)
    bool         m_isFollowingRouteNow : 1{};       ///< Whenever `CTaskComplexFollowPointRoute` was created
    int8         m_nAttempts{};                     ///< Increased when `ComputeRoute` is called. After 20 attempts the `m_crapRoute` is copied into `m_route`

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_WALK_ROUND_BUILDING_ATTEMPT;

    CTaskComplexWalkRoundBuildingAttempt(eMoveState moveState, CVector const& targetPos, CVector const& pos, CVector const& normal, bool bIsHeadOnCollision);
    CTaskComplexWalkRoundBuildingAttempt(eMoveState moveState, CEntity* target, CVector const& offset, CVector const& pos, CVector const& normal, bool bIsHeadOnCollision);
    CTaskComplexWalkRoundBuildingAttempt(const CTaskComplexWalkRoundBuildingAttempt&);
    ~CTaskComplexWalkRoundBuildingAttempt();

    void ComputeMoveDir(CPed const& ped, CVector& outDir);
    void ComputeCrapRoute(CPed const& ped);
    void ComputeRoute(CPed const& ped);
    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexWalkRoundBuildingAttempt{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x654740
    CTaskComplexWalkRoundBuildingAttempt* Constructor(eMoveState moveState, CVector const& targetPos, CVector const& pos, CVector const& normal, bool flag_0x1) {
        this->CTaskComplexWalkRoundBuildingAttempt::CTaskComplexWalkRoundBuildingAttempt(moveState, targetPos, pos, normal, flag_0x1);
        return this;
    }
    // 0x654950
    CTaskComplexWalkRoundBuildingAttempt* Destructor() {
        this->CTaskComplexWalkRoundBuildingAttempt::~CTaskComplexWalkRoundBuildingAttempt();
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexWalkRoundBuildingAttempt, 0x4C);
