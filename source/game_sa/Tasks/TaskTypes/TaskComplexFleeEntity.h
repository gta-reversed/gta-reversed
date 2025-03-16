#pragma once

#include "TaskTimer.h"
#include "Vector.h"
#include "TaskComplex.h"

class CTaskComplexFleeEntity;
class CEntity;
class CTaskComplexFleePoint;
class CPed;

/*!
* This PoS is 95% the same as `CTaskComplexFleeEntity` with the exception of
* missing `m_moveState`, that's it, other than that I literally copy-pasted the code.
*
* TODO: At some point in the future, get rid of this useless shit, replace it with `CTaskComplexSmartFleeEntity`
*/
class NOTSA_EXPORT_VTABLE CTaskComplexFleeEntity : public CTaskComplex {
public:
    CEntity*   m_entity = {};                   /// @copydoc CTaskComplexSmartFleeEntity::m_entity
    CVector    m_pos = {};                      /// @copydoc CTaskComplexSmartFleeEntity::m_pos
    int32      m_time = {};                     /// @copydoc CTaskComplexSmartFleeEntity::m_time
    CTaskTimer m_posCheckTimer{};               /// @copydoc CTaskComplexSmartFleeEntity::m_posCheckTimer
    float      m_safeDistance = {};             /// @copydoc CTaskComplexSmartFleeEntity::m_safeDistance
    bool       m_pedScream = {};                /// @copydoc CTaskComplexSmartFleeEntity::m_pedScream
    int32      m_posCheckPeriod = {};           /// @copydoc CTaskComplexSmartFleeEntity::m_posCheckPeriod
    float      m_posChangeTolerance = {};       /// @copydoc CTaskComplexSmartFleeEntity::m_posChangeTolerance

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_FLEE_ENTITY;

    CTaskComplexFleeEntity(
        CEntity* fleeEntity,
        bool scream,
        float safeDistance = 60.f,
        int32 fleeTime = 1'000'000,
        int32 posCheckPeriod = 1'000,
        float posChangeTolerance = 1.f
    );
    CTaskComplexFleeEntity(const CTaskComplexFleeEntity&);
    ~CTaskComplexFleeEntity();

    CTask* CreateSubTask(eTaskType taskType);

    CTask*    Clone() const override { return new CTaskComplexFleeEntity{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x65B930
    CTaskComplexFleeEntity* Constructor(CEntity* fleeEntity, bool scream, float safeDistance, int32 durationMs, int32 posCheckInterval, float posChangeThreshold) {
        this->CTaskComplexFleeEntity::CTaskComplexFleeEntity(fleeEntity, scream, safeDistance, durationMs, posCheckInterval, posChangeThreshold);
        return this;
    }
    // 0x65B9D0
    CTaskComplexFleeEntity* Destructor() {
        this->CTaskComplexFleeEntity::~CTaskComplexFleeEntity();
        return this;
    }
};
