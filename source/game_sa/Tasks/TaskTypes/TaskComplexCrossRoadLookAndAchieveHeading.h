#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexCrossRoadLookAndAchieveHeading : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_CROSS_ROAD_LOOK_AND_ACHIEVE_HEADING;

    /*!
     * @param  durationInMs Duration of the task [ms]
     * @param  targetHeadingRads Heading to achieve [radians]
     */
    CTaskComplexCrossRoadLookAndAchieveHeading(int32 durationInMs, float targetHeadingRads);
    CTaskComplexCrossRoadLookAndAchieveHeading(const CTaskComplexCrossRoadLookAndAchieveHeading&);
    ~CTaskComplexCrossRoadLookAndAchieveHeading() override = default;

    CTask*    Clone() const override { return new CTaskComplexCrossRoadLookAndAchieveHeading{ *this }; }         // 0x636500
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }                                          // 0x631CB0
    CTask*    CreateFirstSubTask(CPed* ped) override { return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING, ped); } // 0x631C90
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateSubTask(eTaskType taskType, CPed* ped);
    eTaskType GetTaskType() const override { return Type; } // 0x631B10
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;

public:
    static void InjectHooks();

private:
    int32 m_Duration{};          //!< Duration of the task [ms]
    float m_TargetHeadingRads{}; //!< Heading to achieve [radians]
};
