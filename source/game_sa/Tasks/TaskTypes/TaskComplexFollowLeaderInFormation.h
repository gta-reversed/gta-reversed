#pragma once

#include "TaskComplex.h"
#include "Vector.h"
#include "Enums/eMoveState.h"
#include <extensions/EntityRef.hpp>

class CPedGroup;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexFollowLeaderInFormation : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_FOLLOW_LEADER_IN_FORMATION;

    struct Offsets {
        std::array<CVector2D, TOTAL_PED_GROUP_MEMBERS> Offsets;
        std::array<CVector2D, TOTAL_PED_GROUP_MEMBERS> MovingOffsets;
        float                                          Scale;
    };
    static inline auto& ms_offsets = StaticRef<Offsets>(0xC196E8);

    CTaskComplexFollowLeaderInFormation(CPedGroup* pedGroup, CPed* leader, const CVector& offset, float maxDist = -1.f);
    CTaskComplexFollowLeaderInFormation(const CTaskComplexFollowLeaderInFormation&);
    ~CTaskComplexFollowLeaderInFormation() override = default; // 0x694A40

    eTaskType GetTaskType() const override { return Type; }                                       // 0x694A30
    CTask*    Clone() const override { return new CTaskComplexFollowLeaderInFormation{ *this }; } // 0x695740
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    static void InjectHooks();

private:
    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

    CTaskComplexFollowLeaderInFormation* Constructor(CPedGroup* pedGroup, CPed* leader, const CVector& offset, float maxDist) {
        this->CTaskComplexFollowLeaderInFormation::CTaskComplexFollowLeaderInFormation(pedGroup, leader, offset, maxDist);
        return this;
    }

    CTaskComplexFollowLeaderInFormation* Destructor() {
        this->CTaskComplexFollowLeaderInFormation::~CTaskComplexFollowLeaderInFormation();
        return this;
    }

private:
    CPedGroup*             m_Group{};
    notsa::EntityRef<CPed> m_Leader{};
    CVector                m_Offset{};    //!< Offset (from the leader) the ped seeks
    eMoveState             m_MoveState{}; //!< Never read
    float                  m_MaxDist{};   //!< Followers farther than this from the leader stop following it
};
VALIDATE_SIZE(CTaskComplexFollowLeaderInFormation, 0x28);
