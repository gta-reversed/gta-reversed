#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CPedGroup;

class NOTSA_EXPORT_VTABLE CTaskComplexGangFollower : public CTaskComplex {
public:
    CPedGroup* m_grp{};
    CPed*      m_leader{};
    CVector    m_leaderInitialPos{};
    CVector    m_offsetPos{};
    CVector    m_initialOffsetPos{};
    float      m_targetRadius{};
    uint8      m_grpMemshitIdx{};
    bool       m_animsReferenced : 1{false};
    bool       m_leaveGroup : 1{false};
    bool       m_followLeader : 1{true};
    bool       m_inPlayersGroup : 1{m_leader == FindPlayerPed()};
    bool       m_usingStandingStillOffsets : 1{true};
    CTaskTimer m_exhaleTimer{};

public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_GANG_FOLLOWER;

    static constexpr bool ms_bUseClimbing = true; // 0x8D2EDC

    CTaskComplexGangFollower(CPedGroup* pedGroup, CPed* ped, uint8 a4, CVector pos, float a6);
    CTaskComplexGangFollower(const CTaskComplexGangFollower&);
    ~CTaskComplexGangFollower() override;

    CTask*    Clone() override { return new CTaskComplexGangFollower{ *this }; }
    eTaskType GetTaskType() override { return Type; }
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
};
VALIDATE_SIZE(CTaskComplexGangFollower, 0x4C);
