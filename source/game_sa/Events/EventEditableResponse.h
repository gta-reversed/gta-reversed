#pragma once

#include "Event.h"
#include <extensions/WEnum.hpp>

#include "TaskSimpleFacial.h"

class NOTSA_EXPORT_VTABLE CEventEditableResponse : public CEvent {
public:
    CEventEditableResponse(eTaskType taskType = TASK_NONE/*notsa*/);
    ~CEventEditableResponse() override = default; // 0x4AC480

    CEvent* Clone() const noexcept override;
    bool HasEditableResponse() const override { return true; } // 0x420EF0;
    virtual CEventEditableResponse* CloneEditable() = 0;

    bool WillRespond() const;
    void InformVehicleOccupants(CPed* ped);
    void InformRespectedFriends(CPed* ped);
    void InformGroup(CPed* ped);
    void TriggerLookAt(CPed* ped) const;
    void ComputeResponseTaskType(CPed* ped, bool bDecisionMakerTypeInGroup);
    void ComputeResponseTaskType(CPedGroup* pedGroup);
    bool ComputeResponseTaskOfType(CPed* ped, eTaskType taskId);

    auto GetFacialExpressionType() const { return (eFacialExpression)m_FacialExpressionType; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventEditableResponse* Constructor();

public:
    bool                               m_bAddToEventGroup;
    notsa::WEnumS16<eTaskType>         m_TaskId;
    notsa::WEnumS16<eFacialExpression> m_FacialExpressionType;
};
VALIDATE_SIZE(CEventEditableResponse, 0x14);
