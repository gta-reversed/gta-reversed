#pragma once

#include "TaskTimer.h"
#include "TaskComplexSmartFleeEntity.h"

class CEvent;
class CEntity;
class CPed;
class CTaskComplexFleeShooting;

class NOTSA_EXPORT_VTABLE CTaskComplexFleeShooting : public CTaskComplexSmartFleeEntity {

public:
    int32       m_shootTime{};
    int32       m_shootRecoverTime{};
    CTaskTimer  m_gunTimer{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_FLEE_SHOOTING;

    CTaskComplexFleeShooting(CEntity* entityToFleeFrom, bool pedScream, float safeDist, int32 fleeTime, int32 shootTime, int32 shootRecoverTime, int32 entityPosCheckInterval, float entityPosChangeThreshold);
    CTaskComplexFleeShooting(const CTaskComplexFleeShooting&);
    ~CTaskComplexFleeShooting();

    CTask*    Clone() override { return new CTaskComplexFleeShooting{ *this }; }
    eTaskType GetTaskType() override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override; 
private: // Wrappers for hooks
    // 0x65C900
    CTaskComplexFleeShooting* Constructor(CEntity* entityToFleeFrom, bool pedScream, float safeDist, int32 fleeTime, int32 shootTime, int32 shootRecoverTime,
                                          int32 entityPosCheckInterval, float entityPosChangeThreshold) {
        this->CTaskComplexFleeShooting::CTaskComplexFleeShooting(entityToFleeFrom, pedScream, safeDist, fleeTime, shootTime, shootRecoverTime, entityPosCheckInterval,
                                                                 entityPosChangeThreshold);
        return this;
    }
    // 0x65C9A0
    CTaskComplexFleeShooting* Destructor() {
        this->CTaskComplexFleeShooting::~CTaskComplexFleeShooting();
        return this;
    }
};
