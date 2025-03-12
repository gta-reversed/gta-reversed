#pragma once

#include <type_traits>
#include <concepts>

#include "Base.h"
#include "TaskComplex.h"
#include "PosCalculators/EntitySeekPosCalculator.h"
#include "TaskComplexDriveToPoint.h"
#include "TaskComplexLeaveAnyCar.h"
#include "TaskTimer.h"
#include "Entity.h"

class CPed;

/*!
* Follows an entity using a car.
* When the entity is destroyed the ped leaves the car.
* Do not use directly, use the derived classes instead, 
* such as `CTaskComplexSeekEntityAnyMeansXYOffset`
*
* @tparam T The position calculator to use
*/
template<std::derived_from<CEntitySeekPosCalculator> T>
class NOTSA_EXPORT_VTABLE CTaskComplexSeekEntityAnyMeans : public CTaskComplex {
    CEntity*   m_entity{};        /// The entity to seek
    int32      m_scanInterval{};  /// Entity pos seek interval 
    float      m_targetRadius{};  /// The "tolerance"
    CTaskTimer m_scanTimer{};     /// Timer which on expiration updates the subtasks' goto point and is re-started (Uses `m_scanInterval` as the interval)
    T          m_posCalculator{}; /// The position calculator
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_SEEK_ENTITY_ANY_MEANS;

protected:
    CTaskComplexSeekEntityAnyMeans(CEntity* entity, int32 period, float radius, T posCalculator) :
        m_targetRadius{ radius },
        m_entity{ entity },
        m_scanInterval{ period },
        m_posCalculator{ posCalculator }
    {
        CEntity::SafeRegisterRef(m_entity);
    }

public:
    CTaskComplexSeekEntityAnyMeans(const CTaskComplexSeekEntityAnyMeans& o) :
        m_targetRadius{ 8.f },
        m_entity{ o.m_entity },
        m_scanInterval{ 1000 },
        m_posCalculator{ o.m_posCalculator }
    {
    }

    ~CTaskComplexSeekEntityAnyMeans() {
        CEntity::SafeCleanUpRef(m_entity);
    }

    CTask* Clone() const override {
        return new CTaskComplexSeekEntityAnyMeans{ *this };
    }

    eTaskType GetTaskType() const override {
        return Type;
    }

    CTask*    CreateNextSubTask(CPed* ped) override {
        return nullptr;
    }

    CTask* CreateFirstSubTask(CPed* ped) override {
        return new CTaskComplexDriveToPoint{ nullptr, GetSeekPos(ped), 30.f, 0, MODEL_INVALID, m_targetRadius, DRIVING_STYLE_STOP_FOR_CARS };
    }

    CTask* ControlSubTask(CPed* ped) override {
        if (m_entity) {
            if (m_scanTimer.IsOutOfTime()) {
                m_scanTimer.Start(m_scanInterval);
                CTask::Cast<CTaskComplexDriveToPoint>(m_pSubTask)->GoToPoint(GetSeekPos(ped));
            }
            return m_pSubTask;
        }
        return new CTaskComplexLeaveAnyCar{ 0, true, false };
    }

    CVector GetSeekPos(CPed* ped) {
        CVector seekPos{};
        m_posCalculator.ComputeEntitySeekPos(*ped, *m_entity, seekPos);
        return seekPos;
    }
};
