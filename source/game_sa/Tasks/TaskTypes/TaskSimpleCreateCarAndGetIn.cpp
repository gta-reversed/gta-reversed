#include "StdInc.h"

#include "TaskSimpleCreateCarAndGetIn.h"
#include "PedPlacement.h"
#include "TaskSimpleStandStill.h"
#include "CarCtrl.h"
#include "TaskSimpleCarSetPedInAsDriver.h"

void CTaskSimpleCreateCarAndGetIn::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCreateCarAndGetIn, 0x86F070, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6493E0);
    RH_ScopedInstall(Destructor, 0x64CEA0);
    RH_ScopedVMTInstall(Clone, 0x64A410);
    RH_ScopedVMTInstall(GetTaskType, 0x649430);
    RH_ScopedVMTInstall(MakeAbortable, 0x649440);
    RH_ScopedVMTInstall(ProcessPed, 0x64CF40);
}

// 0x6493E0
CTaskSimpleCreateCarAndGetIn::CTaskSimpleCreateCarAndGetIn(const CVector& pos, int32 model) : CTaskSimple() {
    m_Pos            = pos;
    m_CreatedVeh     = nullptr;
    m_nModel         = model;
    m_nTimeMs        = 0;
    m_nWaitTime      = 0;
    m_bWaitTimeSet   = false;
    m_bResetWaitTime = false;
}

// 0x64CEA0
CTaskSimpleCreateCarAndGetIn::~CTaskSimpleCreateCarAndGetIn() {
    if (m_CreatedVeh) {
        CleanupCreatedVehicle();
    }
}

// 0x649440
bool CTaskSimpleCreateCarAndGetIn::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return true;
}

// 0x64CF40
bool CTaskSimpleCreateCarAndGetIn::ProcessPed(CPed* ped) {
    CVector nodePos{};
    if (!ThePaths.FindNodeCoorsForScript(nodePos, ThePaths.FindNodeClosestToCoors(m_Pos, PATH_TYPE_VEH, 999999.88f, 1, 1, 0, 0, 0))) {
        if (!m_bWaitTimeSet) {
            m_nTimeMs = CTimer::GetTimeInMS();
            m_nWaitTime = 2000;
            m_bWaitTimeSet = true;
        }
        if (m_bResetWaitTime) {
            m_nTimeMs = CTimer::GetTimeInMS();
            m_bResetWaitTime = false;
        }
        return CTimer::GetTimeInMS() >= m_nTimeMs + m_nWaitTime;
    }

    m_VehCreationPos = nodePos;
    m_bWaitTimeSet = false;

    // If vehicle is not created yet lets check if it can be created at all
    if (!m_CreatedVeh) {
        // Make sure position isn't visible by camera
        if (TheCamera.IsSphereVisible({ m_VehCreationPos, 3.f })) {
            return true;
        }

        // Also make sure peds position isn't visible by the camera
        if (TheCamera.IsSphereVisible({ ped->GetPosition(), ped->GetColModel()->GetBoundRadius() })) {
            return true;
        }

        // And make sure the ped isn't stuck or something
        if (!CPedPlacement::IsPositionClearForPed(ped->GetPosition(), 3.f, -1, 0, true, true, true)) {
            return true;
        }
    }

    // Weird..
    {
        CTaskSimpleStandStill task{ 0, 0, 0, 8.f };
        task.ProcessPed(ped);
    }

    // Make sure model is loaded
    if (!CStreaming::IsModelLoaded(m_nModel)) {
        CStreaming::RequestModel(m_nModel, STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED);
        return false;
    }

    // Create vehicle and set ped in as driver
    if (!m_CreatedVeh) {
        m_CreatedVeh = CCarCtrl::CreateCarForScript(m_nModel, m_VehCreationPos, true);

        CTaskSimpleCarSetPedInAsDriver task{ m_CreatedVeh, nullptr };
        task.ProcessPed(ped);
    }
 
    if (!m_CreatedVeh->IsStatic()) {
        return false;
    }

    // Veh is static now, so remove it, and signal task finish
    CleanupCreatedVehicle();
    return true;
}

// NOTSA
void CTaskSimpleCreateCarAndGetIn::CleanupCreatedVehicle() {
    CTheScripts::CleanUpThisVehicle(m_CreatedVeh);
    CTheScripts::MissionCleanUp.RemoveEntityFromList(GetVehiclePool()->GetRef(m_CreatedVeh), MISSION_CLEANUP_ENTITY_TYPE_VEHICLE);
    m_CreatedVeh = nullptr;
}
