#include "StdInc.h"

#include "EventEditableResponse.h"

#include "PedType.h"
#include "IKChainManager_c.h"

void CEventEditableResponse::InjectHooks() {
    RH_ScopedVirtualClass(CEventEditableResponse, 0x85AB80, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AC450);
    RH_ScopedVMTInstall(Clone, 0x420ED0);
    RH_ScopedVMTInstall(HasEditableResponse, 0x420EF0);
    RH_ScopedInstall(WillRespond, 0x4AC490);
    RH_ScopedInstall(InformVehicleOccupants, 0x4AC4A0);
    RH_ScopedInstall(InformRespectedFriends, 0x4B2B00);
    RH_ScopedInstall(InformGroup, 0x4B7DF0);
    RH_ScopedInstall(TriggerLookAt, 0x4AC5A0);
    RH_ScopedOverloadedInstall(ComputeResponseTaskType, "ped", 0x4B56C0, void(CEventEditableResponse::*)(CPed * ped, bool));
    RH_ScopedOverloadedInstall(ComputeResponseTaskType, "group", 0x4B57A0, void(CEventEditableResponse::*)(CPedGroup*));
    RH_ScopedInstall(ComputeResponseTaskOfType, 0x4B5730);
}

// 0x4AC450
CEventEditableResponse::CEventEditableResponse(eTaskType taskType) :
    CEvent()
{
    m_bAddToEventGroup = true;
    m_taskId = (int16)taskType;
    m_FacialExpressionType = -1;
}

CEventEditableResponse* CEventEditableResponse::Constructor() {
    this->CEventEditableResponse::CEventEditableResponse();
    return this;
}

// 0x420ED0
CEvent* CEventEditableResponse::Clone() {
    CEventEditableResponse* clonedEvent = CloneEditable();
    clonedEvent->m_taskId = m_taskId;
    clonedEvent->m_FacialExpressionType = m_FacialExpressionType;
    clonedEvent->m_bAddToEventGroup = m_bAddToEventGroup;
    return clonedEvent;
}

// 0x4AC490
bool CEventEditableResponse::WillRespond() const {
    return m_taskId != TASK_NONE;
}

// 0x4AC4A0
void CEventEditableResponse::InformVehicleOccupants(CPed* ped) {
    CVehicle* vehicle = ped->m_pVehicle;
    if (ped->bInVehicle && DoInformVehicleOccupants(ped) && m_bAddToEventGroup) {
        CPed* driver = vehicle->m_pDriver;
        if (driver && driver != ped) {
            auto clonedEvent = static_cast<CEventEditableResponse*>(Clone());
            clonedEvent->m_taskId = TASK_NONE;
            clonedEvent->m_bAddToEventGroup = false;
            driver->GetEventGroup().Add(clonedEvent, false);
            delete clonedEvent;
        }
        for (const auto passenger : vehicle->GetPassengers()) {
            if (passenger && passenger != ped) {
                auto clonedEvent = static_cast<CEventEditableResponse*>(Clone());
                clonedEvent->m_taskId = TASK_NONE;
                clonedEvent->m_bAddToEventGroup = false;
                passenger->GetEventGroup().Add(clonedEvent, false);
                delete clonedEvent;
            }
        }
    }
}

// 0x4B2B00
void CEventEditableResponse::InformRespectedFriends(CPed* ped) {
    if (!m_bAddToEventGroup)
        return;

    uint32 numPedsToScan = ped->GetIntelligence()->m_nDmNumPedsToScan;
    if (!numPedsToScan)
        return;

    CEntity** entities = ped->GetIntelligence()->GetPedEntities();
    for (size_t entityIndex = 0; entityIndex < numPedsToScan; entityIndex++) {
        CEntity* entity = entities[entityIndex];
        if (!entity)
            continue;

        CPed* entityPed = entity->AsPed();
        if (ped->m_nPedType == PED_TYPE_COP) {
            if (entityPed->m_nPedType != PED_TYPE_COP)
                continue;

            CPlayerPedData* playerData = FindPlayerPed(0)->m_pPlayerData;
            if (playerData->m_pWanted && playerData->m_pWanted->m_nWantedLevel) {
                continue;
            }
        } else {
            uint32 respect = ped->GetAcquaintance().GetAcquaintances(ACQUAINTANCE_RESPECT);
            bool bFlagSet = respect & CPedType::GetPedFlag(entityPed->m_nPedType);
            if (entityPed->IsPlayer()) {
                uint32 entityAcquaintances = entityPed->GetAcquaintance().GetAcquaintances(ACQUAINTANCE_RESPECT);
                bFlagSet = entityAcquaintances & CPedType::GetPedFlag(ped->m_nPedType);
            }
            if (!bFlagSet)
                continue;
        }

        float fDmRadius = ped->GetIntelligence()->m_fDmRadius;
        if (sq(fDmRadius) > DistanceBetweenPointsSquared(entity->GetPosition(), ped->GetPosition())) {
            auto clonedEvent = static_cast<CEventEditableResponse*>(Clone());
            clonedEvent->m_taskId = TASK_NONE;
            clonedEvent->m_bAddToEventGroup = false;
            if (!CInformFriendsEventQueue::Add(entity, clonedEvent))
                delete clonedEvent;
        }
    }
}

// 0x4B7DF0
void CEventEditableResponse::InformGroup(CPed* ped) {
    CPedGroup* pedGroup = CPedGroups::GetPedsGroup(ped);
    if (!pedGroup)
        return;

    auto clonedEvent = static_cast<CEventEditableResponse*>(Clone());
    clonedEvent->m_taskId = TASK_NONE;
    clonedEvent->m_bAddToEventGroup = false;
    if (!CInformGroupEventQueue::Add(ped, pedGroup, clonedEvent))
        delete clonedEvent;
}

// 0x4AC5A0
void CEventEditableResponse::TriggerLookAt(CPed* ped) {
    CEntity* sourceEntity = GetSourceEntity();
    if (sourceEntity) {
        if (sourceEntity->IsPed()) {
            g_ikChainMan.LookAt("CEventEditableResponse", ped, sourceEntity->AsPed(), 2000, BONE_HEAD, nullptr, true, 0.25f, 500, 3, false);
            return;
        }
        g_ikChainMan.LookAt("CEventEditableResponse", ped, sourceEntity, 2000, BONE_UNKNOWN, nullptr, true, 0.25f, 500, 3, false);
    }
}

// 0x4B56C0
void CEventEditableResponse::ComputeResponseTaskType(CPed* ped, bool bDecisionMakerTypeInGroup) {
    if (m_taskId != TASK_NONE) {
        return;
    }
    CDecisionMakerTypes::GetInstance()->MakeDecision(
        ped,
        GetEventType(),
        CEventSource::ComputeEventSourceType(*this, *ped),
        ped->bInVehicle,
        TASK_SIMPLE_INFORM_RESPECTED_FRIENDS,
        TASK_SIMPLE_INFORM_GROUP,
        TASK_SIMPLE_LOOK_AT_ENTITY_OR_COORD,
        TASK_INVALID,
        bDecisionMakerTypeInGroup,
        m_taskId,
        m_FacialExpressionType
    );
}

// 0x4B57A0
void CEventEditableResponse::ComputeResponseTaskType(CPedGroup* pedGroup) {
    if (m_taskId != TASK_NONE) {
        return;
    }
    CPed* groupLeader = pedGroup->GetMembership().GetLeader();
    CPed* member = groupLeader;
    if (groupLeader && groupLeader->IsPlayer())
        member = nullptr;
    if (!member) {
        for (size_t memberId = 0; memberId < TOTAL_PED_GROUP_FOLLOWERS; memberId++) {
            member = pedGroup->GetMembership().GetMember(memberId);
            if (member)
                break;
        }
    }
    if (member) {
        int32 eventSourceType = CEventSource::ComputeEventSourceType(*this, *member);
        m_taskId = CDecisionMakerTypes::GetInstance()->MakeDecision(
            pedGroup,
            GetEventType(),
            eventSourceType,
            member->bInVehicle,
            TASK_SIMPLE_INFORM_GROUP,
            TASK_SIMPLE_INFORM_RESPECTED_FRIENDS,
            TASK_SIMPLE_LOOK_AT_ENTITY_OR_COORD,
            TASK_INVALID
        );
    } else {
        m_taskId = TASK_NONE;
    }
    groupLeader = pedGroup->GetMembership().GetLeader();
    if (m_taskId == TASK_NONE && groupLeader) {
        if (groupLeader->IsPlayer()) {
            int32 eventSourceType = CEventSource::ComputeEventSourceType(*this, *groupLeader);
            m_taskId = CDecisionMakerTypes::GetInstance()->MakeDecision(
                pedGroup,
                GetEventType(),
                eventSourceType, groupLeader->bInVehicle,
                TASK_SIMPLE_INFORM_GROUP,
                TASK_SIMPLE_INFORM_RESPECTED_FRIENDS,
                TASK_SIMPLE_LOOK_AT_ENTITY_OR_COORD,
                TASK_INVALID
            );
        }
    }
}

// 0x4B5730
bool CEventEditableResponse::ComputeResponseTaskOfType(CPed* ped, int32 taskId) {
    int16 outTaskId = TASK_INVALID;
    int16 facialTaskType = TASK_INVALID;
    int32 eventSourceType = CEventSource::ComputeEventSourceType(*this, *ped);
    CDecisionMakerTypes::GetInstance()->MakeDecision(
        ped,
        GetEventType(),
        eventSourceType,
        ped->bInVehicle,
        TASK_INVALID,
        TASK_INVALID,
        TASK_INVALID,
        taskId,
        false,
        outTaskId,
        facialTaskType
    );
    return taskId == outTaskId;
}
