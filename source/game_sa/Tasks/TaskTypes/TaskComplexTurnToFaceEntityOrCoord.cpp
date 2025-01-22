#include "StdInc.h"

#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleAchieveHeading.h"

void CTaskComplexTurnToFaceEntityOrCoord::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexTurnToFaceEntityOrCoord, 0x86ff94, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedOverloadedInstall(Constructor, "Coords", 0x66B890, CTaskComplexTurnToFaceEntityOrCoord*(CTaskComplexTurnToFaceEntityOrCoord::*)(CEntity*, float, float));
    RH_ScopedOverloadedInstall(Constructor, "Entity", 0x66B910, CTaskComplexTurnToFaceEntityOrCoord*(CTaskComplexTurnToFaceEntityOrCoord::*)(CVector const&, float, float));
    RH_ScopedInstall(Destructor, 0x66B960);

    RH_ScopedInstall(ComputeTargetHeading, 0x66B9D0);

    RH_ScopedVMTInstall(Clone, 0x66D250);
    RH_ScopedVMTInstall(GetTaskType, 0x66B900);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66B9C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x670850);
    RH_ScopedVMTInstall(ControlSubTask, 0x670920);
}

// 0x66B890
CTaskComplexTurnToFaceEntityOrCoord::CTaskComplexTurnToFaceEntityOrCoord(CEntity* entity, float changeRateMult, float maxHeading) :
    m_EntityToFace{ entity },
    m_fChangeRateMult{ changeRateMult },
    m_fMaxHeading{ maxHeading },
    m_bFaceEntity{ true }
{
    assert((uintptr_t)entity != (uintptr_t)-1);
    CEntity::SafeRegisterRef(m_EntityToFace);
}

// 0x66B910
CTaskComplexTurnToFaceEntityOrCoord::CTaskComplexTurnToFaceEntityOrCoord(CVector const& coords, float changeRateMult, float maxHeading) :
    m_CoordsToFace{ coords },
    m_fChangeRateMult{ changeRateMult },
    m_fMaxHeading{ maxHeading },
    m_bFaceEntity{ false }
{
}

// 0x66B960
CTaskComplexTurnToFaceEntityOrCoord::~CTaskComplexTurnToFaceEntityOrCoord() {
    CEntity::SafeCleanUpRef(m_EntityToFace);
}

// 0x66D250
CTask* CTaskComplexTurnToFaceEntityOrCoord::Clone() const {
    return m_bFaceEntity
        ? new CTaskComplexTurnToFaceEntityOrCoord(m_EntityToFace, m_fChangeRateMult, m_fMaxHeading)
        : new CTaskComplexTurnToFaceEntityOrCoord(m_CoordsToFace, m_fChangeRateMult, m_fMaxHeading);
}

// 0x66B9D0
float CTaskComplexTurnToFaceEntityOrCoord::ComputeTargetHeading(CPed* ped) const {
    return (ComputeTargetPos() - ped->GetPosition()).Heading();
}

CVector CTaskComplexTurnToFaceEntityOrCoord::ComputeTargetPos() const {
    return m_bFaceEntity
        ? m_EntityToFace->GetPosition()
        : m_CoordsToFace;
}

// 0x670850
CTask* CTaskComplexTurnToFaceEntityOrCoord::CreateFirstSubTask(CPed* ped) {
    return m_bFaceEntity && !m_EntityToFace
        ? nullptr
        : new CTaskSimpleAchieveHeading(ComputeTargetHeading(ped), m_fChangeRateMult, m_fMaxHeading);
}

// 0x670920
CTask* CTaskComplexTurnToFaceEntityOrCoord::ControlSubTask(CPed* ped) {
    if (m_bFaceEntity) {
        const auto heading = m_EntityToFace
            ? ComputeTargetHeading(ped)
            : ped->m_fCurrentRotation;
        CTask::Cast<CTaskSimpleAchieveHeading>(m_pSubTask)->SetHeading(heading, m_fMaxHeading, m_fChangeRateMult);
    }
    return m_pSubTask;
}
