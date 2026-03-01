#pragma once

#include "TaskComplex.h"
#include "Base.h"
#include "Ped.h"
#include "Vector.h"

class CPed;
class CTaskComplexWalkRoundFire;

class NOTSA_EXPORT_VTABLE CTaskComplexWalkRoundFire : public CTaskComplex {

public:
    eMoveState m_moveState{};          ///< How the ped should move when going around the fire
    CSphere    m_fireBoundSphere{};    ///< Bounding sphere of the fire
    CVector    m_targetPos{};          ///< The current position the ped is "going to" (using the GoToPoint task)
    CVector    m_initialPedPos{};      ///< Ped's initial position (when the task was created)

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_WALK_ROUND_FIRE;

    CTaskComplexWalkRoundFire(eMoveState moveState, CVector const& firePos, float fireRadius, CVector const& targetPos);
    CTaskComplexWalkRoundFire(const CTaskComplexWalkRoundFire&);
    ~CTaskComplexWalkRoundFire() = default;

    bool ComputeDetourTarget(const CPed& ped, CVector& outTarget) const;
    CVector GetDetourTarget(const CPed& ped) const;

    CTask*    Clone() const override { return new CTaskComplexWalkRoundFire{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;
private: // Wrappers for hooks
    // 0x655720
    CTaskComplexWalkRoundFire* Constructor(eMoveState moveState, CVector const& firePos, float fireRadius, CVector const& targetPos) {
        this->CTaskComplexWalkRoundFire::CTaskComplexWalkRoundFire(moveState, firePos, fireRadius, targetPos);
        return this;
    }
    // 0x655780
    CTaskComplexWalkRoundFire* Destructor() {
        this->CTaskComplexWalkRoundFire::~CTaskComplexWalkRoundFire();
        return this;
    }

};
VALIDATE_SIZE(CTaskComplexWalkRoundFire, 0x38);
