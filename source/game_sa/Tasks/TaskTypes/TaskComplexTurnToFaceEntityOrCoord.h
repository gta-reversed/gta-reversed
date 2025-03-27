#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CPed;
class CEntity;

class NOTSA_EXPORT_VTABLE CTaskComplexTurnToFaceEntityOrCoord : public CTaskComplex {
public:
    CEntity* m_EntityToFace{};
    bool     m_bFaceEntity{};
    CVector  m_CoordsToFace{};
    float    m_fChangeRateMult{};
    float    m_fMaxHeading{};

public:
    constexpr static auto Type = TASK_COMPLEX_TURN_TO_FACE_ENTITY;

    static void InjectHooks();

    CTaskComplexTurnToFaceEntityOrCoord(CEntity* entity, float changeRateMult = 0.5f, float maxHeading = 0.2f);
    CTaskComplexTurnToFaceEntityOrCoord(const CVector& coords, float changeRateMult = 0.5f, float maxHeading = 0.2f);
    ~CTaskComplexTurnToFaceEntityOrCoord() override;

    eTaskType GetTaskType() const override { return Type; } // 0x66B900
    CTask* Clone() const override; // 0x66D250
    CTask* CreateNextSubTask(CPed* ped) override { return nullptr; } // 0x66B9C0
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    float   ComputeTargetHeading(CPed* ped) const;
    CVector ComputeTargetPos() const; // NOTSA

private:
    CTaskComplexTurnToFaceEntityOrCoord* Constructor(CEntity* entity, float changeRateMult, float maxHeading) { this->CTaskComplexTurnToFaceEntityOrCoord::CTaskComplexTurnToFaceEntityOrCoord(entity, changeRateMult, maxHeading); return this; }
    CTaskComplexTurnToFaceEntityOrCoord* Constructor(CVector const& coords, float changeRateMult, float maxHeading) { this->CTaskComplexTurnToFaceEntityOrCoord::CTaskComplexTurnToFaceEntityOrCoord(coords, changeRateMult, maxHeading); return this; }
    CTaskComplexTurnToFaceEntityOrCoord* Destructor() { this->CTaskComplexTurnToFaceEntityOrCoord::~CTaskComplexTurnToFaceEntityOrCoord(); return this; }
};
VALIDATE_SIZE(CTaskComplexTurnToFaceEntityOrCoord, 0x28);
