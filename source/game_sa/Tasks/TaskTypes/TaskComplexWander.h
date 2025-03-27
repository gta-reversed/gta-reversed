/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplex.h"
#include "NodeAddress.h"

enum eWanderType : int32 {
    WANDER_TYPE_STANDARD = 0,
    WANDER_TYPE_COP,
    WANDER_TYPE_MEDIC,
    WANDER_TYPE_CRIMINAL,
    WANDER_TYPE_GANG,
    WANDER_TYPE_SHOP,
    WANDER_TYPE_FLEE,
    WANDER_TYPE_PROSTITUTE
};

class CPed;
class CNodeAddress;
class CVector;

class NOTSA_EXPORT_VTABLE CTaskComplexWander : public CTaskComplex {
public:
    eMoveState   m_nMoveState;
    uint8        m_nDir;
    float        m_fTargetRadius;
    CNodeAddress m_LastNode;
    CNodeAddress m_NextNode;
    uint32       m_nLastUpdateDirFrameCount;
    union
    {
        uint8 m_nFlags;
        struct {
            uint8 m_bWanderSensibly : 1;
            uint8 m_bNewDir : 1;
            uint8 m_bNewNodes : 1;
            uint8 m_bAllNodesBlocked : 1;
        };
    };

public:
    static constexpr auto Type = TASK_COMPLEX_WANDER;

    CTaskComplexWander(eMoveState moveState, uint8 dir, bool bWanderSensibly = true, float fTargetRadius = 0.5f);
    ~CTaskComplexWander() override = default;

    eTaskType GetTaskType() const override { return Type; } // 0x460CD0
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    virtual eWanderType GetWanderType() = 0;
    virtual void ScanForStuff(CPed* ped) = 0;
    virtual void UpdateDir(CPed* ped);
    virtual void UpdatePathNodes(const CPed* ped, uint8 dir, CNodeAddress& originNode, CNodeAddress& targetNode, uint8& outDir);

    CTask* CreateSubTask(CPed* ped, eTaskType taskType);
    void ComputeTargetPos(const CPed* ped, CVector& outTargetPos, const CNodeAddress& targetNodeAddress);
    float ComputeTargetHeading(CPed* ped);
    [[nodiscard]] bool ValidNodes() const;
    void ScanForBlockedNodes(CPed* ped);
    bool ScanForBlockedNode(CPed* ped, const CNodeAddress& targetNodeAddress);
    bool ScanForBlockedNode(const CVector& position, class CEntity* entity);

    /// Get sq. distance of the path node (either `m_LastNode` or `m_NextNode`) closest to the ped
    float GetDistSqOfClosestPathNodeToPed(CPed* ped);

    static CTaskComplexWander* GetWanderTaskByPedType(CPed* ped);
private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskComplexWander* Constructor(eMoveState moveState, uint8 dir, bool bWanderSensibly = true, float fTargetRadius = 0.5f);
};
VALIDATE_SIZE(CTaskComplexWander, 0x28);
