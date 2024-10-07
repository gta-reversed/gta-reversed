#include "StdInc.h"

#include "TaskComplexMedicTreatInjuredPed.h"

#include "TaskComplexTreatAccident.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexDriveToPoint.h"
#include "TaskComplexDriveWander.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskComplexWanderMedic.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "InterestingEvents.h"

void CTaskComplexMedicTreatInjuredPed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexMedicTreatInjuredPed, 0x86F518, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x658BA0);
    RH_ScopedInstall(CreateSubTask, 0x658DB0);
    RH_ScopedInstall(CreateDealWithNextAccidentTask, 0x65A020);
    RH_ScopedInstall(FindNearestAccident, 0x658CC0);
    RH_ScopedInstall(CalcTargetPosWithOffset, 0x658D20);
    RH_ScopedVMTInstall(Clone, 0x659AF0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x659FE0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65A990);
    RH_ScopedVMTInstall(ControlSubTask, 0x65ABF0);
}

CTaskComplexMedicTreatInjuredPed* CTaskComplexMedicTreatInjuredPed::Constructor(CVehicle* vehicle, CPed* ped, bool isDriver) {
    this->CTaskComplexMedicTreatInjuredPed::CTaskComplexMedicTreatInjuredPed(vehicle, ped, isDriver);
    return this;
}

// 0x658BA0
CTaskComplexMedicTreatInjuredPed::CTaskComplexMedicTreatInjuredPed(CVehicle* vehicle, CPed* ped, bool isDriver) : CTaskComplex() {
    m_pVehicle      = vehicle;
    m_pPartnerMedic = ped;
    m_bIsDriver     = isDriver;
    m_pAccident     = nullptr;
    m_bLeftCarOnce  = false;
    CEntity::SafeRegisterRef(m_pVehicle);
    CEntity::SafeRegisterRef(m_pPartnerMedic);
}

// 0x658C30
CTaskComplexMedicTreatInjuredPed::~CTaskComplexMedicTreatInjuredPed() {
    if (m_pAccident && m_pAccident->m_pPed && !m_pAccident->m_bIsRevived)
        m_pAccident->m_bIsTreated = false;

    CEntity::SafeCleanUpRef(m_pVehicle);
    CEntity::SafeCleanUpRef(m_pPartnerMedic);
}

// 0x658DB0
CTask* CTaskComplexMedicTreatInjuredPed::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_TREAT_ACCIDENT:
        return new CTaskComplexTreatAccident(m_pAccident);
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill(10'000, false, false, 8.0F);
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
        return new CTaskComplexEnterCarAsPassenger(m_pVehicle, 11, false);
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
        return new CTaskComplexEnterCarAsDriver(m_pVehicle);
    case TASK_COMPLEX_LEAVE_CAR:
        m_bLeftCarOnce = true;
        return new CTaskComplexLeaveCar(m_pVehicle, 0, 0, true, false);
    case TASK_SIMPLE_CAR_DRIVE:
        return new CTaskSimpleCarDrive(m_pVehicle, nullptr, false);
    case TASK_COMPLEX_CAR_DRIVE_TO_POINT:
        return new CTaskComplexDriveToPoint(m_pVehicle, m_vecAccidentPosition, 30.0F, 0, MODEL_INVALID, -1.0F, DRIVING_STYLE_AVOID_CARS);
    case TASK_COMPLEX_CAR_DRIVE_WANDER:
        return new CTaskComplexCarDriveWander(m_pVehicle, DRIVING_STYLE_AVOID_CARS, 30.0F);
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        return new CTaskComplexGoToPointAndStandStill(PEDMOVE_RUN, m_vecAccidentPosition, 0.125F, 2.0F, false, false);
    case TASK_COMPLEX_WANDER:
        return new CTaskComplexWanderMedic(PEDMOVE_WALK, CGeneral::GetRandomNumberInRange(0, 8), true);
    default:
        return nullptr;
    }
}

// 0x65A020
CTask* CTaskComplexMedicTreatInjuredPed::CreateDealWithNextAccidentTask(CPed* ped, CAccident* accident) {
    m_pAccident = CAccidentManager::GetInstance()->GetNearestFreeAccidentExceptThisOne(ped->GetPosition(), accident, true);
    if (m_pAccident && m_pAccident->m_pPed) {
        m_pAccident->m_bIsTreated = true;
        CalcTargetPosWithOffset(ped, m_pAccident->m_pPed);
        if (m_pSubTask && m_pSubTask->GetTaskType() == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {
            auto subTask = reinterpret_cast<CTaskComplexGoToPointAndStandStill*>(m_pSubTask);
            subTask->GoToPoint(m_vecAccidentPosition, 0.5F, 2.0F, false);
            subTask->m_moveState = PEDMOVE_RUN;
            return subTask;
        } else
            return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL);
    } else
        return CreateSubTask(m_bIsDriver ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER : TASK_COMPLEX_ENTER_CAR_AS_PASSENGER);
}

// 0x658CC0
void CTaskComplexMedicTreatInjuredPed::FindNearestAccident(CVector& posn) {
    m_pAccident = CAccidentManager::GetInstance()->GetNearestFreeAccident(posn, true);
}

// 0x658D20
void CTaskComplexMedicTreatInjuredPed::CalcTargetPosWithOffset(CPed* ped, CPed* injuredPed) {
    m_vecAccidentPosition = (injuredPed->GetBonePosition(BONE_L_THIGH) + injuredPed->GetBonePosition(BONE_R_THIGH)) * 0.5F;
}

// 0x659AF0
CTask* CTaskComplexMedicTreatInjuredPed::Clone() const {
    return new CTaskComplexMedicTreatInjuredPed(m_pVehicle, m_pPartnerMedic, m_bIsDriver);
}

// 0x659FE0
CTask* CTaskComplexMedicTreatInjuredPed::CreateFirstSubTask(CPed* ped) {
    if (!m_pVehicle)
        return CreateSubTask(TASK_COMPLEX_WANDER);

    if (m_bIsDriver)
        return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_WANDER);

    return CreateSubTask(TASK_SIMPLE_CAR_DRIVE);
}

// 0x65A990
CTask* CTaskComplexMedicTreatInjuredPed::CreateNextSubTask(CPed* ped) {
    eTaskType subTaskId = m_pSubTask->GetTaskType();

    if (subTaskId == TASK_COMPLEX_TREAT_ACCIDENT) {
        FindNearestAccident(ped->GetPosition());
        if (!m_pAccident)
            return CreateSubTask(m_bIsDriver ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER : TASK_COMPLEX_ENTER_CAR_AS_PASSENGER);
        else {
            m_pAccident->m_bIsTreated = true;
            CalcTargetPosWithOffset(ped, m_pAccident->m_pPed);
            return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL);
        }
    }

    if (subTaskId == TASK_SIMPLE_STAND_STILL)
        return CreateSubTask(m_bIsDriver ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER : TASK_COMPLEX_ENTER_CAR_AS_PASSENGER);

    if (subTaskId == TASK_COMPLEX_ENTER_CAR_AS_DRIVER || subTaskId == TASK_COMPLEX_ENTER_CAR_AS_PASSENGER)
        return CreateSubTask(ped->bInVehicle ? TASK_SIMPLE_CAR_DRIVE : TASK_COMPLEX_WANDER);

    if (subTaskId == TASK_COMPLEX_LEAVE_CAR) {
        g_InterestingEvents.Add(CInterestingEvents::EType::INTERESTING_EVENT_12, ped);
        if (m_bIsDriver && m_pAccident->m_pPed && m_pAccident->m_bIsTreated) {
            CalcTargetPosWithOffset(ped, m_pAccident->m_pPed);
            return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL);
        } else {
            FindNearestAccident(ped->GetPosition());
            if (m_pAccident) {
                m_pAccident->m_bIsTreated = true;
                CalcTargetPosWithOffset(ped, m_pAccident->m_pPed);
                return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL);
            } else {
                return CreateSubTask(m_bIsDriver ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER : TASK_SIMPLE_STAND_STILL);
            }
        }
    }

    if (subTaskId == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {
        auto relPosn = ped->GetPosition() - m_vecAccidentPosition;
        if (relPosn.Magnitude2D() <= 1.0F && fabs(relPosn.z) <= 2.0F)
            return CreateSubTask(TASK_COMPLEX_TREAT_ACCIDENT);
        else
            return CreateDealWithNextAccidentTask(ped, m_pAccident);
    }

    return nullptr;
}

// 0x65ABF0
CTask* CTaskComplexMedicTreatInjuredPed::ControlSubTask(CPed* ped) {
    eTaskType subTaskId = m_pSubTask->GetTaskType();

    if (subTaskId == TASK_SIMPLE_CAR_DRIVE) {
        if (ped->bInVehicle && m_bIsDriver && m_pVehicle->IsPassenger(m_pPartnerMedic))
            return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_WANDER);

        if (!m_bLeftCarOnce && m_pPartnerMedic) {
            auto partnerTask = m_pPartnerMedic->GetTaskManager().GetActiveTask();
            if (partnerTask && partnerTask->GetTaskType() == TASK_COMPLEX_MEDIC_TREAT_INJURED_PED) {
                auto* taskTreat = reinterpret_cast<CTaskComplexMedicTreatInjuredPed*>(partnerTask);
                if (taskTreat->m_pAccident && taskTreat->m_pAccident->m_pPed) {
                    m_vecAccidentPosition = taskTreat->m_vecAccidentPosition;
                    if (DistanceBetweenPointsSquared(m_vecAccidentPosition, m_pVehicle->GetPosition()) < 100.0f) {
                        ped->Say(CTX_GLOBAL_VAN, 0, 1.0F);
                        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR);
                    }
                }
            }
        }
    }

    if (subTaskId == TASK_COMPLEX_CAR_DRIVE_TO_POINT) {
        if (ped->bInVehicle) {
            if (!m_pAccident || !m_pAccident->m_pPed)
                return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_WANDER);

            if (DistanceBetweenPointsSquared(m_vecAccidentPosition, m_pVehicle->GetPosition()) < 100.0f) {
                ped->Say(CTX_GLOBAL_VAN, 0, 1.0F);
                return CreateSubTask(TASK_COMPLEX_LEAVE_CAR);
            }
        }
    }

    if (subTaskId == TASK_COMPLEX_CAR_DRIVE_WANDER) {
        if (ped->bInVehicle) {
            FindNearestAccident(ped->GetPosition());
            if (m_pAccident) {
                m_pAccident->m_bIsTreated = true;
                CalcTargetPosWithOffset(ped, m_pAccident->m_pPed);
                return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_TO_POINT);
            }
        }
    }

    if (subTaskId == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {
        if (!m_pAccident || !m_pAccident->m_pPed)
            return CreateDealWithNextAccidentTask(ped, nullptr);

        auto subTask = reinterpret_cast<CTaskComplexGoToPointAndStandStill*>(m_pSubTask);
        if (subTask->m_moveState != PEDMOVE_WALK && DistanceBetweenPointsSquared(ped->GetPosition(), m_vecAccidentPosition) < 25.0F) {
            for (CEntity* entity : ped->GetIntelligence()->m_vehicleScanner.m_apEntities) {
                if (entity && CPedGeometryAnalyser::IsEntityBlockingTarget(entity, m_vecAccidentPosition, 0.125F)) {
                    return CreateDealWithNextAccidentTask(ped, nullptr);
                }
            }

            CalcTargetPosWithOffset(ped, m_pAccident->m_pPed);
            subTask->m_moveState = PEDMOVE_WALK;
            subTask->GoToPoint(m_vecAccidentPosition, 0.5F, 2.0F, false);
        }
    }

    return m_pSubTask;
}
