#include "StdInc.h"
#include "TaskComplexUseAttractor.h"
#include "ScriptsForBrains.h"
#include "PedAttractor.h"
#include "TaskSimpleUseAtm.h"
#include "TaskComplexSitDownThenIdleThenStandUp.h"
#include "TaskComplexWaitForBus.h"
#include "TaskComplexWaitForDryWeather.h"
#include "TaskComplexUseScriptedBrain.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexUseScriptedAttractor.h"
#include "TaskComplexSunbathe.h"

void CTaskComplexUseAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseAttractor, 0x86e134, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6326F0);
    RH_ScopedInstall(Destructor, 0x632720);

    RH_ScopedVMTInstall(Clone, 0x6369B0);
    RH_ScopedVMTInstall(GetTaskType, 0x632710);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x632730);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6385D0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6327C0);
}

// 0x6326F0
CTaskComplexUseAttractor::CTaskComplexUseAttractor(CPedAttractor* attractor) :
    m_attractor{attractor}
{
    assert(attractor);
}

CTaskComplexUseAttractor::CTaskComplexUseAttractor(const CTaskComplexUseAttractor& o) :
    CTaskComplexUseAttractor{ o.m_attractor }
{
}

// 0x632730
CTask* CTaskComplexUseAttractor::CreateNextSubTask(CPed* ped) {
    switch (m_attractor->GetType()) {
    case PED_ATTRACTOR_PIZZA: {
        ped->m_nMoneyCount = std::max(0, ped->m_nMoneyCount - 10);
        break;
    }
    case PED_ATTRACTOR_ATM: {
        ped->m_nMoneyCount += 20 + 20 * CGeneral::GetRandomNumberInRange(0, 50);
        break;
    }
    }
    GetPedAttractorManager()->BroadcastDeparture(ped, m_attractor);
    return nullptr;
}

// 0x6385D0
CTask* CTaskComplexUseAttractor::CreateFirstSubTask(CPed* ped) {
    const auto task = [&, this]() -> CTask* {
        const auto type = m_attractor->GetType();
        switch (type) {
        case PED_ATTRACTOR_ATM:
            return new CTaskSimpleUseAtm{};
        case PED_ATTRACTOR_SEAT:
        case PED_ATTRACTOR_STEP:
            return new CTaskComplexSitDownThenIdleThenStandUp{
                CGeneral::RandomBool(40.f) ? CGeneral::GetRandomNumberInRange(15'000, 60'000) : gDefaultTaskTime,
                type == PED_ATTRACTOR_STEP,
                !!ped->bStayInSamePlace
            };
        case PED_ATTRACTOR_STOP:
            return new CTaskComplexWaitForBus{};
        case PED_ATTRACTOR_PIZZA:
            return new CTaskSimpleStandStill{ CGeneral::GetRandomNumberInRange(6000, 12'000) };
        case PED_ATTRACTOR_SHELTER:
            return new CTaskComplexWaitForDryWeather{};
        case PED_ATTRACTOR_TRIGGER_SCRIPT: {
            if (ped->bHasAScriptBrain) {
                return nullptr;
            }

            if (!CTheScripts::ScriptsForBrains.HasAttractorScriptBrainWithThisNameLoaded(m_attractor->m_ScriptName)) {
                return nullptr;
            }

            if (CEntity* const entity = m_attractor->m_Entity) {
                if (!entity->IsObject() || entity->AsObject()->objectFlags.bEnableDisabledAttractors) {
                    CTheScripts::ScriptsForBrains.StartAttractorScriptBrainWithThisName(m_attractor->m_ScriptName, ped, ped->bUseAttractorInstantly);
                    CScriptedBrainTaskStore::Clear(ped);
                    return new CTaskComplexUseScriptedBrain{ m_attractor->m_ScriptName };
                }
            }

            return nullptr;
        }
        case PED_ATTRACTOR_LOOK_AT:
            return new CTaskSimpleStandStill{ CGeneral::GetRandomNumberInRange(3000, 6000) };
        case PED_ATTRACTOR_SCRIPTED:
            return new CTaskComplexUseScriptedAttractor{ *m_attractor };
        case PED_ATTRACTOR_PARK:
            return new CTaskComplexSunbathe{ nullptr, true };
        default:
            NOTSA_UNREACHABLE();
        }
    }();

    if (task) {
        return task;
    } else {
        GetPedAttractorManager()->BroadcastDeparture(ped, m_attractor);
        return nullptr;
    }
}

// 0x6327C0
CTask* CTaskComplexUseAttractor::ControlSubTask(CPed* ped) {
    if (m_attractor->m_Entity) {
        if (CEntity* const entity = m_attractor->m_Entity) {
            if (entity->IsObject() || entity->AsObject()->objectFlags.bIsBroken) {
                m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
            }
        }
    }
    return m_pSubTask;
}
