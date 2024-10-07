#pragma once

#include "Event.h"

#include "TaskSimpleFacial.h"

class NOTSA_EXPORT_VTABLE CEventEditableResponse : public CEvent {
public:
    bool   m_bAddToEventGroup;
    int16  m_taskId; // see eTaskType
    int16  m_FacialExpressionType; // eFacialExpression
   
public:
    CEventEditableResponse(eTaskType taskType = TASK_NONE/*notsa*/);
    ~CEventEditableResponse() override = default; // 0x4AC480

    CEvent* Clone() override;
    bool HasEditableResponse() const override { return true; } // 0x420EF0;
    virtual CEventEditableResponse* CloneEditable() = 0;

    bool WillRespond() const;
    void InformVehicleOccupants(CPed* ped);
    void InformRespectedFriends(CPed* ped);
    void InformGroup(CPed* ped);
    void TriggerLookAt(CPed* ped);
    void ComputeResponseTaskType(CPed* ped, bool bDecisionMakerTypeInGroup);
    void ComputeResponseTaskType(CPedGroup* pedGroup);
    bool ComputeResponseTaskOfType(CPed* ped, int32 taskId);

    auto GetFacialExpressionType() const { return (eFacialExpression)m_FacialExpressionType; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventEditableResponse* Constructor();
    };
VALIDATE_SIZE(CEventEditableResponse, 0x14);
