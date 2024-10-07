#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleAchieveHeading : public CTaskSimple {
public:
    static void InjectHooks();

    static constexpr auto Type = TASK_SIMPLE_ACHIEVE_HEADING;

    /*!
     * @param desiredHeading The heading to achieve [In Radians]
     * @param changeRate     How fast to change the heading
     * @param tolerance      The tolerance of the heading [In Radians]
    */
    CTaskSimpleAchieveHeading(float desiredHeading, float changeRate = 0.5f, float tolerance = 0.2f);

    /*!
     * @notsa
     * @brief Achieve heading towards an entity
     * 
     * @param looker      The ped that has to achieve the heading
     * @param lookTowards The entity the ped has to achieve the heading towards
     * @param changeRate  How fast to change the heading
     * @param tolerance   The tolerance of the heading [In Radians]
    */
    CTaskSimpleAchieveHeading(CPed* looker, CEntity* lookTowards, float changeRate = 0.5f, float tolerance = 0.2f); // NOTSA
    ~CTaskSimpleAchieveHeading() override = default; // 0x667E70

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskSimpleAchieveHeading(m_DesiredHeadingRad, m_HeadingChangeRate, m_HeadingToleranceRad); } // 0x66CCF0
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(class CPed* ped) override;

    void QuitIK(CPed* ped) const;
    void SetUpIK(CPed* ped);
    void SetHeading(float heading, float maxHeading, float changeRateMult);

private:
    float m_DesiredHeadingRad{};
    float m_HeadingChangeRate{};
    float m_HeadingToleranceRad{};
    bool  m_IsDoingIK : 1{};
};
VALIDATE_SIZE(CTaskSimpleAchieveHeading, 0x18);
