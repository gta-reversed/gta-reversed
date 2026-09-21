#include "StdInc.h"

#include "TaskComplexTrackEntity.h"
#include "TaskComplexSmartFleeEntity.h"
#include "TaskComplexLeaveCar.h"

void CTaskGangHassleVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CTaskGangHassleVehicle, 0x86F9D4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65FAC0);
    RH_ScopedInstall(Destructor, 0x65FB60);
    RH_ScopedInstall(GetTargetHeading, 0x65FDD0);
    RH_ScopedInstall(CalcTargetOffset, 0x6641A0);
    RH_ScopedInstall(Clone, 0x65FC00);
    RH_ScopedInstall(CreateNextSubTask, 0x65FC80);
    RH_ScopedInstall(CreateFirstSubTask, 0x664BA0);
    RH_ScopedInstall(ControlSubTask, 0x6637C0, { .reversed = false });
}

// 0x65FAC0
CTaskGangHassleVehicle::CTaskGangHassleVehicle(CVehicle* vehicle, int32 a3, uint8 a4, float a5, float a6) : CTaskComplex() {
    m_nTime = 0;
    dword3C = 0;
    byte40 = 0;
    byte41 = 0;
    byte18 = a4;
    dword1C = a5;
    m_Vehicle = vehicle;
    m_nHasslePosId = -1;
    m_fOffsetX = a6;
    m_bRemoveAnim = 0;
    m_pEntity = nullptr;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x65FB60
CTaskGangHassleVehicle::~CTaskGangHassleVehicle() {
    if (m_Vehicle) {
        if (m_nHasslePosId > -1) {
            m_Vehicle->SetHasslePosId(m_nHasslePosId, false);
        }
        CEntity::SafeCleanUpRef(m_Vehicle);
    }

    CEntity::SafeCleanUpRef(m_pEntity);

    if (m_bRemoveAnim) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_bRemoveAnim = false;
    }
}

// 0x65FDD0
float CTaskGangHassleVehicle::GetTargetHeading(CPed* ped) {
    UNUSED(ped);
    const auto& right   = m_Vehicle->GetRight();
    const auto& forward = m_Vehicle->GetForward();
    float x = right.x;
    float y = right.y;
    switch (m_nHasslePosId) {
    case 0:
    case 2:
        x = right.x;
        y = right.y;
        break;
    case 1:
    case 3:
        x = -right.x;
        y = -right.y;
        break;
    case 4:
        x = forward.x;
        y = forward.y;
        break;
    case 5:
        x = -forward.x;
        y = -forward.y;
        break;
    default:
        break;
    }
    return CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(x, y, 0.0f, 0.0f));
}

// 0x6641A0
void CTaskGangHassleVehicle::CalcTargetOffset() {
    m_vecPosn = CVector{};
    const auto& bbox = CModelInfo::ms_modelInfoPtrs[m_Vehicle->m_nModelIndex]->GetColModel()->GetBoundingBox();
    const float minX = bbox.m_vecMin.x;
    const float minY = bbox.m_vecMin.y;
    const float maxX = bbox.m_vecMax.x;
    const float maxY = bbox.m_vecMax.y;
    switch (m_nHasslePosId) {
    case 0:
        m_vecPosn.x = minX - m_fOffsetX;
        m_vecPosn.y = maxY * 0.5f;
        break;
    case 1:
        m_vecPosn.x = maxX + m_fOffsetX;
        m_vecPosn.y = maxY * 0.5f;
        break;
    case 2:
        m_vecPosn.x = minX - m_fOffsetX;
        m_vecPosn.y = minY * 0.5f;
        break;
    case 3:
        m_vecPosn.x = maxX + m_fOffsetX;
        m_vecPosn.y = minY * 0.5f;
        break;
    case 4:
        m_vecPosn.y = minY - m_fOffsetX;
        break;
    case 5:
        m_vecPosn.y = maxY + m_fOffsetX;
        break;
    default:
        break;
    }
}

// 0x65FC80
CTask* CTaskGangHassleVehicle::CreateNextSubTask(CPed* ped) {
    UNUSED(ped);
    if (!m_Vehicle) {
        return nullptr;
    }
    if (m_pSubTask && m_pSubTask->GetTaskType() == TASK_COMPLEX_SMART_FLEE_ENTITY) {
        return nullptr;
    }
    if (m_Vehicle->m_fHealth >= 250.0f) {
        if (m_pSubTask && m_pSubTask->GetTaskType() == TASK_COMPLEX_GANG_HASSLE_PED) {
            return nullptr;
        }
        if (m_pSubTask && m_pSubTask->GetTaskType() == TASK_COMPLEX_TRACK_ENTITY) {
            return nullptr;
        }
        return new CTaskComplexTrackEntity{ m_Vehicle, m_vecPosn, 1, -1, 10.0f, 40.0f, 1 };
    }
    return new CTaskComplexSmartFleeEntity{ m_Vehicle, false, 28.0f, 1'000'000, 1000, StaticRef<float>(0xC18CF0) };
}

// 0x664BA0
CTask* CTaskGangHassleVehicle::CreateFirstSubTask(CPed* ped) {
    if (!m_Vehicle) {
        return nullptr;
    }
    m_pEntity = m_Vehicle->m_pDriver;
    if (m_pEntity) {
        CEntity::RegisterReference(m_pEntity);
    }
    const auto& bbox = CModelInfo::ms_modelInfoPtrs[m_Vehicle->m_nModelIndex]->GetColModel()->GetBoundingBox();
    if (bbox.m_vecMax.x - bbox.m_vecMin.x > 4.0f || bbox.m_vecMax.y - bbox.m_vecMin.y > 8.0f) {
        return nullptr;
    }
    m_nHasslePosId = m_Vehicle->GetSpareHasslePosId();
    if (m_nHasslePosId == -1) {
        return nullptr;
    }
    m_Vehicle->SetHasslePosId(m_nHasslePosId, true);
    CalcTargetOffset();
    m_b31 = false;
    ped->DropEntityThatThisPedIsHolding(true);
    m_nTime = CTimer::GetTimeInMS();
    dword3C = CGeneral::GetRandomNumberInRange(150000, 250000);
    byte40 = true;
    if (!ped->bInVehicle || !ped->m_pVehicle) {
        return CreateNextSubTask(ped);
    }
    return new CTaskComplexLeaveCar{ ped->m_pVehicle, 0, 0, true, false };
}

// 0x6637C0
CTask* CTaskGangHassleVehicle::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6637C0, CTaskGangHassleVehicle*, CPed*>(this, ped);
}
