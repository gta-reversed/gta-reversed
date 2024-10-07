#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CEntity;
class CColSphere;

class NOTSA_EXPORT_VTABLE CTaskComplexAvoidEntity : CTaskComplex {
public:
    CEntity*   m_Entity;
    eMoveState m_moveState;
    CVector    m_PedPos;
    CVector    f20;
    CVector    f2C;
    CVector    f38;
    int32      field_44;
    int32      field_48;
    char       field_4C;
    char       field_4D;
    char       field_4E;
    char       field_4F;
    union {
        struct {
            bool m_b1 : 1; // IK initialized
        };
        uint8 m_Flags;
    };

public:
    static constexpr auto Type = TASK_COMPLEX_AVOID_ENTITY;

    CTaskComplexAvoidEntity(eMoveState, CEntity* entity, const CVector& pos);
    ~CTaskComplexAvoidEntity() override;

    eTaskType GetTaskType() const override { return Type; } // 0x66AAD0
    CTask* Clone() const override { return new CTaskComplexAvoidEntity(m_moveState, m_Entity, f20); } // 0x66D0C0
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* CreateNextSubTask(CPed* ped) override;

    bool ComputeDetourTarget(const CPed& ped);
    bool ComputeDetourTarget(const CPed& ped, const CColSphere& sphere);
    void ComputeAvoidSphere(const CPed& ped, CColSphere& sphere);

    static void ComputeSphere(CColSphere& sphere, CEntity** entities);
    void QuitIK(CPed* ped) const;
    void SetUpIK(CPed *ped);
    uint8 NearbyEntitiesInSphere(const CPed& ped, const CColSphere& sphere, CEntity**, CEntity **);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexAvoidEntity* Constructor(eMoveState moveState, CEntity* entity, const CVector& pos) { this->CTaskComplexAvoidEntity::CTaskComplexAvoidEntity(moveState, entity, pos); return this; };
            };
VALIDATE_SIZE(CTaskComplexAvoidEntity, 0x54);
