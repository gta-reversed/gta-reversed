#include "StdInc.h"
#include "TaskComplexSunbathe.h"
#include "InterestingEvents.h"
#include "Event.h"

void CTaskComplexSunbathe::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSunbathe, 0x86e0ac, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x631F80);
    RH_ScopedInstall(Destructor, 0x632050);

    //RH_ScopedGlobalInstall(CanSunbathe, 0x632140, { .Reversed = false });
    //RH_ScopedGlobalInstall(ShouldLoadSunbatheAnims, 0x632190, { .Reversed = false });

    //RH_ScopedInstall(CreateSubTask, 0x638290, { .Reversed = false });

    RH_ScopedVMTInstall(Clone, 0x6366A0, { .Reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x632040, { .Reversed = false });
    RH_ScopedVMTInstall(MakeAbortable, 0x6320F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6399F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x639CB0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6381A0);

}

// 0x631f80
CTaskComplexSunbathe::CTaskComplexSunbathe(CObject* towel, bool bStartStanding) :
    m_pTowel{towel},
    m_bStartStanding{bStartStanding}
{
    if (m_pTowel) {
        CEntity::SafeRegisterRef(m_pTowel);
        m_pTowel->m_nObjectType = OBJECT_TYPE_DECORATION;
    }

    const auto LoadAnim = [](int32& idx, CAnimBlock*& blk, const char* name) {
        idx = CAnimManager::GetAnimationBlockIndex(name);
        blk = CAnimManager::GetAnimationBlock(name);
    };
    LoadAnim(m_BeachAnimBlockIndex, m_pBeachAnimBlock, "beach");
    LoadAnim(m_SunbatheAnimBlockIndex, m_pSunbatheAnimBlock, "sunbathe");

    // Refs added later in another function
}

CTaskComplexSunbathe::CTaskComplexSunbathe(const CTaskComplexSunbathe& o) :
    CTaskComplexSunbathe{ o.m_pTowel, o.m_bStartStanding }
{
}

// 0x632050
CTaskComplexSunbathe::~CTaskComplexSunbathe() {
    if (m_pTowel) {
        m_pTowel->m_nObjectType = OBJECT_TEMPORARY;
        CEntity::SafeCleanUpRef(m_pTowel);
    }

    const auto UnrefAnimBlock = [](bool& refed, int32 blkIdx) {
        if (refed) {
            CAnimManager::RemoveAnimBlockRef(blkIdx);
            refed = false;
        }
    };
    UnrefAnimBlock(m_bBeachAnimsReferenced, m_BeachAnimBlockIndex);
    UnrefAnimBlock(m_bSunbatheAnimsReferenced, m_SunbatheAnimBlockIndex);
}

// 0x6320F0
bool CTaskComplexSunbathe::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (event) {
        switch (event->GetEventType()) {
        case EVENT_PED_COLLISION_WITH_PED:
        case EVENT_PED_COLLISION_WITH_PLAYER:
            return false;
        default:
            break;
        }
    }
    if (!m_pSubTask->MakeAbortable(ped, priority, event)) {
        return false;
    }
    m_bAborted = true;
    return true;
}

// 0x632140
bool CTaskComplexSunbathe::CanSunbathe() {
    return plugin::CallAndReturn<bool, 0x632140>();
}

// 0x632190
bool CTaskComplexSunbathe::ShouldLoadSunbatheAnims() {
    return plugin::CallAndReturn<bool, 0x632190>();
}

// 0x638290
CTask* CTaskComplexSunbathe::CreateSubTask(eTaskType taskType, CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x638290, CTaskComplexSunbathe*, eTaskType, CPed*>(this, taskType, ped);
}

// 0x6399F0
CTask* CTaskComplexSunbathe::CreateNextSubTask(CPed* ped) {
    if (ped->m_nPedType == PED_TYPE_PROSTITUTE || ped->m_nPedType == PED_TYPE_CIVFEMALE) {
        g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_2, ped);
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_PAUSE:
        if (m_bBathing) {
            // Bathing: anims still needed and loaded -> stand up, otherwise we're done
            if (!ShouldLoadSunbatheAnims() || !m_pSunbatheAnimBlock->IsLoaded) {
                return CreateSubTask(TASK_FINISHED, ped);
            }
            return CreateSubTask(TASK_SIMPLE_STOP_SUNBATHING, ped);
        } else {
            // Not bathed yet: both anims present -> start sunbathing
            if (!m_pSunbatheAnimBlock->IsLoaded || !m_pBeachAnimBlock->IsLoaded) {
                return CreateSubTask(TASK_FINISHED, ped);
            }
            return CreateSubTask(TASK_SIMPLE_START_SUNBATHING, ped);
        }

    case TASK_SIMPLE_SUNBATHE:
        if (!m_pSunbatheAnimBlock->IsLoaded) {
            return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
        }
        if (!CanSunbathe()
            || m_BathingTimer.IsOutOfTime()
            || (m_SunbatherType != SUNBATHER_MALE_1 && m_SunbatherType != SUNBATHER_FEMALE_1)) {
            return CreateSubTask(TASK_SIMPLE_STOP_SUNBATHING, ped);
        }
        return CreateSubTask(TASK_SIMPLE_IDLE_SUNBATHING, ped);

    case TASK_SIMPLE_START_SUNBATHING:
        if (!m_pBeachAnimBlock->IsLoaded) {
            return CreateSubTask(TASK_FINISHED, ped);
        }
        RET_IGNORED(m_BathingTimer.Start(20'000 + static_cast<int32>(static_cast<float>(CGeneral::GetRandomNumber()) * (1.f / 32768.f) * 80000.f)));
        m_bBathing = true;
        return CreateSubTask(TASK_SIMPLE_SUNBATHE, ped);

    case TASK_SIMPLE_IDLE_SUNBATHING:
        return CreateSubTask(m_pBeachAnimBlock->IsLoaded ? TASK_SIMPLE_SUNBATHE : TASK_FINISHED, ped);

    case TASK_SIMPLE_STOP_SUNBATHING: {
        auto* const task = CreateSubTask(TASK_FINISHED, ped);
        m_bBathing       = false;
        return task;
    }

    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x639CB0
CTask* CTaskComplexSunbathe::CreateFirstSubTask(CPed* ped) {
    RET_IGNORED(CGeneral::GetRandomNumber());

    // Pick the sunbather type
    if (ped->m_nPedType == PED_TYPE_CIVFEMALE) {
        m_SunbatherType = static_cast<eSunbatherType>(2 - static_cast<int32>(static_cast<float>(CGeneral::GetRandomNumber()) * 0.000030517578125f * -3.0f));
    } else if (CPopulation::IsSunbather(ped->GetModelId())) {
        m_SunbatherType = CGeneral::GetRandomNumber() < 0x3FFF ? SUNBATHER_MALE_2 : SUNBATHER_MALE_1;
    } else {
        m_SunbatherType = SUNBATHER_MALE_2;
    }

    if (!m_pBeachAnimBlock->IsLoaded) {
        CStreaming::RequestModel(IFPToModelId(m_BeachAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
    }
    if (!m_pSunbatheAnimBlock->IsLoaded) {
        CStreaming::RequestModel(IFPToModelId(m_SunbatheAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
    }

    m_bBathing                         = false;

    const bool shouldLoadSunbatheAnims = ShouldLoadSunbatheAnims();

    if (m_bStartStanding && shouldLoadSunbatheAnims) {
        return CreateSubTask(!m_pTowel && m_pSunbatheAnimBlock->IsLoaded ? TASK_SIMPLE_START_SUNBATHING : TASK_SIMPLE_PAUSE, ped);
    }
    if (!shouldLoadSunbatheAnims && !m_pBeachAnimBlock->IsLoaded) {
        return nullptr;
    }

    // 0x639DA5: "beach" loaded -> bathing with a timer; otherwise (0x639DAF) TASK_NONE/nullptr
    if (m_pBeachAnimBlock->IsLoaded) {
        RET_IGNORED(m_BathingTimer.Start(20'000 - static_cast<int32>(static_cast<float>(CGeneral::GetRandomNumber()) * (1.f / 32768.f) * -80000.f)));
        m_bBathing                  = true;
        return CreateSubTask(TASK_SIMPLE_SUNBATHE, ped);
    }
    if (shouldLoadSunbatheAnims) {
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
    }
    return nullptr;
}

// 0x6381A0
CTask* CTaskComplexSunbathe::ControlSubTask(CPed* ped) {
    if (m_bAborted) {
        return nullptr;
    }

    if (!m_bBeachAnimsReferenced) {
        CAnimManager::StreamAnimBlock("beach", true, m_bBeachAnimsReferenced);
    }
    if (!m_bSunbatheAnimsReferenced && ShouldLoadSunbatheAnims()) {
        CAnimManager::StreamAnimBlock("sunbathe", true, m_bSunbatheAnimsReferenced);
    }

    if (m_bBathing) {
        ped->m_pedIK.bSlopePitch = true;
        ped->bFallenDown         = true;
    }

    if (!CanSunbathe()) {
        m_BathingTimer.Pause();
        if (m_pSubTask->GetTaskType() == TASK_SIMPLE_SUNBATHE) {
            RET_IGNORED(m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE));
        }
    }

    return m_pSubTask;
}
