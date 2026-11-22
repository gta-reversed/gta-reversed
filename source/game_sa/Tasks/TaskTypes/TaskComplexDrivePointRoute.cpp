#include "StdInc.h"
#include "TaskComplexDrivePointRoute.h"
#include "TaskComplexDriveToPoint.h"

void CTaskComplexDrivePointRoute::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDrivePointRoute, 0x86ebf8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6433E0);
    RH_ScopedInstall(Destructor, 0x643490);

    RH_ScopedVMTInstall(Clone, 0x6439C0);
    RH_ScopedVMTInstall(GetTaskType, 0x643480);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x63CF90);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x63D030);
    RH_ScopedVMTInstall(ControlSubTask, 0x63D0C0);
}

// 0x6433E0
CTaskComplexDrivePointRoute::CTaskComplexDrivePointRoute(CVehicle* vehicle, CPointRoute const& route, float cruiseSpeed, uint32 mode, eModelID carModelIdx, float targetRadius, eCarDrivingStyle drivingStyle) :
    m_mode{mode},
    m_veh{vehicle},
    m_route{new CPointRoute{route}},
    m_cruiseSpeed{cruiseSpeed},
    m_desiredCarModel{carModelIdx},
    m_targetRadius{targetRadius},
    m_drivingStyle{drivingStyle}
{
    // CEntity::SafeRegisterRef(m_veh); // NOTE: Missing from OG code
}

CTaskComplexDrivePointRoute::CTaskComplexDrivePointRoute(const CTaskComplexDrivePointRoute& o) :
    CTaskComplexDrivePointRoute{
        o.m_veh,
       *o.m_route,
        o.m_cruiseSpeed,
        o.m_mode,
        o.m_desiredCarModel,
        o.m_targetRadius,
        o.m_drivingStyle
    }
{
}

// 0x643490
CTaskComplexDrivePointRoute::~CTaskComplexDrivePointRoute() {
    delete m_route;
    // Also no unregister ref here.. probably delibarate?
}

CTask* CTaskComplexDrivePointRoute::CreateTaskForCurrentPoint() const {
    return new CTaskComplexDriveToPoint{ m_veh, (*m_route)[m_currPtIdx], m_cruiseSpeed, (int32)m_mode, m_desiredCarModel, m_targetRadius, m_drivingStyle };
}

// 0x63CF90
CTask* CTaskComplexDrivePointRoute::CreateNextSubTask(CPed* ped) {
    if (m_currPtIdx++ == m_route->m_NumEntries) {
        return nullptr;
    }

    return CreateTaskForCurrentPoint();
}
