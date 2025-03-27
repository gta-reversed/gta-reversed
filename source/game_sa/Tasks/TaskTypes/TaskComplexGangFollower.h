#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CPedGroup;

class NOTSA_EXPORT_VTABLE CTaskComplexGangFollower : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_GANG_FOLLOWER;

    static constexpr bool ms_bUseClimbing = true; // 0x8D2EDC

    CTaskComplexGangFollower(CPedGroup* pedGroup, CPed* ped, uint8 a4, CVector pos, float a6);
    CTaskComplexGangFollower(const CTaskComplexGangFollower&);
    ~CTaskComplexGangFollower() override;

    CTask*    Clone() const override { return new CTaskComplexGangFollower{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    CVector   CalculateOffsetPosition();

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskComplexGangFollower* Constructor(CPedGroup* pedGroup, CPed* ped, uint8 uint8, CVector pos, float a6) { this->CTaskComplexGangFollower::CTaskComplexGangFollower(pedGroup, ped, uint8, pos, a6); return this; }
    CTaskComplexGangFollower* Destructor() { this->CTaskComplexGangFollower::~CTaskComplexGangFollower(); return this; }

public:
    CPedGroup* m_PedGroup{};
    CPed*      m_Leader{};
    CVector    m_LeaderInitialPos{};
    CVector    m_OffsetPos{};
    CVector    m_InitialOffsetPos{};
    float      m_TargetRadius{};
    uint8      m_GrpMemIdx{};
    bool       m_AnimsRef : 1{false};
    bool       m_LeaveGroup : 1{false};
    bool       m_FollowLeader : 1{true};
    bool       m_IsInPlayersGroup : 1{m_Leader == FindPlayerPed()};
    bool       m_IsUsingStandingStillOffsets : 1{true};
    CTaskTimer m_ExhaleTimer{};
};
VALIDATE_SIZE(CTaskComplexGangFollower, 0x4C);
