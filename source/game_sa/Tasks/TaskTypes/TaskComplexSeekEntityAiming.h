#pragma once

#include "TaskComplex.h"

class CEntity;
class CTaskComplexSeekEntityAiming;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexSeekEntityAiming : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_SEEK_ENTITY_AIMING;

    static void InjectHooks();

    CTaskComplexSeekEntityAiming(CEntity* entity, float seekRadius, float aimRadius);
    CTaskComplexSeekEntityAiming(const CTaskComplexSeekEntityAiming&);
    ~CTaskComplexSeekEntityAiming();

    CTask*    Clone() const override { return new CTaskComplexSeekEntityAiming{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x694B90
    CTaskComplexSeekEntityAiming* Constructor(CEntity* entity, float a3, float a4) {
        this->CTaskComplexSeekEntityAiming::CTaskComplexSeekEntityAiming(entity, a3, a4);
        return this;
    }

    // 0x694C10
    CTaskComplexSeekEntityAiming* Destructor() {
        this->CTaskComplexSeekEntityAiming::~CTaskComplexSeekEntityAiming();
        return this;
    }

private:
    CEntity* m_ToSeek{};
    float    m_SeekRadius{};
    float    m_AimRadius{};
};
