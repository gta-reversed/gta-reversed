#include "StdInc.h"
#include "TaskComplexSunbathe.h"
#include "InterestingEvents.h"

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
    RH_ScopedVMTInstall(MakeAbortable, 0x6320F0, { .Reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6399F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x639CB0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6381A0, { .Reversed = false });

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
    return plugin::CallMethodAndReturn<bool, 0x6320F0, CTaskComplexSunbathe*, CPed*, eAbortPriority, CEvent const*>(this, ped, priority, event);
}

// 0x6399F0
CTask* CTaskComplexSunbathe::CreateNextSubTask(CPed* ped) {
    if (ped->m_nPedType == PED_TYPE_PROSTITUTE || ped->m_nPedType == PED_TYPE_CIVFEMALE) {
        g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_2, ped);
    }

    // Wrapper over the not-yet-reversed createSubTask (0x638290, not part of this batch)
    const auto createSubTask = [this, ped](eTaskType taskType) {
        return plugin::CallMethodAndReturn<CTask*, 0x638290, CTaskComplexSunbathe*, eTaskType, CPed*>(this, taskType, ped);
    };

    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_PAUSE:
        if (m_bBathing) {
            // Bathing: anims still needed and loaded -> stand up, otherwise we're done
            if (!plugin::CallAndReturn<bool, 0x632190>() || !m_pSunbatheAnimBlock->IsLoaded) { // ShouldLoadSunbatheAnims()
                return createSubTask(TASK_FINISHED);
            }
            return createSubTask(TASK_SIMPLE_STOP_SUNBATHING);
        } else {
            // Not bathed yet: both anims present -> start sunbathing
            if (!m_pSunbatheAnimBlock->IsLoaded || !m_pBeachAnimBlock->IsLoaded) {
                return createSubTask(TASK_FINISHED);
            }
            return createSubTask(TASK_SIMPLE_START_SUNBATHING);
        }

    case TASK_SIMPLE_SUNBATHE:
        if (!m_pSunbatheAnimBlock->IsLoaded) {
            return createSubTask(TASK_SIMPLE_PAUSE);
        }
        if (!plugin::CallAndReturn<bool, 0x632140>() // CanSunbathe()
            || m_BathingTimer.IsOutOfTime()
            || (m_SunbatherType != SUNBATHER_MALE_1 && m_SunbatherType != SUNBATHER_FEMALE_1)) {
            return createSubTask(TASK_SIMPLE_STOP_SUNBATHING);
        }
        return createSubTask(TASK_SIMPLE_IDLE_SUNBATHING);

    case TASK_SIMPLE_START_SUNBATHING:
        if (!m_pBeachAnimBlock->IsLoaded) {
            return createSubTask(TASK_FINISHED);
        }
        m_BathingTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_BathingTimer.m_nInterval  = 20'000 + static_cast<int32>(static_cast<float>(CGeneral::GetRandomNumber()) * (1.f / 32768.f) * 80000.f);
        m_BathingTimer.m_bStarted   = true;
        {
            auto* const task = createSubTask(TASK_SIMPLE_SUNBATHE);
            m_bBathing       = true; // NOTE: set AFTER createSubTask in the original
            return task;
        }

    case TASK_SIMPLE_IDLE_SUNBATHING:
        return createSubTask(m_pBeachAnimBlock->IsLoaded ? TASK_SIMPLE_SUNBATHE : TASK_FINISHED);

    case TASK_SIMPLE_STOP_SUNBATHING: {
        auto* const task = createSubTask(TASK_FINISHED);
        m_bBathing       = false;
        return task;
    }

    default:
        return nullptr;
    }
}

// 0x639CB0
CTask* CTaskComplexSunbathe::CreateFirstSubTask(CPed* ped) {
    rand(); // NOTE: called and discarded in the original (CRT rand, 0x821B1E)

    // Pick the sunbather type
    if (ped->m_nPedType == PED_TYPE_CIVFEMALE) {
        m_SunbatherType = static_cast<eSunbatherType>(2 - static_cast<int32>(static_cast<float>(rand() & 0xFFFF) * 0.000030517578125f * -3.0f));
    } else if (CPopulation::IsSunbather(ped->GetModelId())) {
        m_SunbatherType = rand() < 0x3FFF ? SUNBATHER_MALE_2 : SUNBATHER_MALE_1;
    } else {
        m_SunbatherType = SUNBATHER_MALE_2;
    }

    if (!m_pBeachAnimBlock->IsLoaded) {
        CStreaming::RequestModel(IFPToModelId(m_BeachAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
    }
    if (!m_pSunbatheAnimBlock->IsLoaded) {
        CStreaming::RequestModel(IFPToModelId(m_SunbatheAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
    }

    // Wrapper over the not-yet-reversed createSubTask (0x638290, not part of this batch)
    const auto createSubTask = [this, ped](eTaskType taskType) {
        return plugin::CallMethodAndReturn<CTask*, 0x638290, CTaskComplexSunbathe*, eTaskType, CPed*>(this, taskType, ped);
    };

    m_bBathing                         = false;

    const bool shouldLoadSunbatheAnims = plugin::CallAndReturn<bool, 0x632190>(); // ShouldLoadSunbatheAnims()

    if (m_bStartStanding) {
        if (shouldLoadSunbatheAnims) {
            if (!m_pTowel && m_pSunbatheAnimBlock->IsLoaded) {
                return createSubTask(TASK_SIMPLE_START_SUNBATHING);
            }
            return createSubTask(TASK_SIMPLE_PAUSE);
        }
        // Sunbathe anims not needed - only "beach" matters (0x639D9B)
        if (!m_pBeachAnimBlock->IsLoaded) {
            return nullptr;
        }
    } else if (!shouldLoadSunbatheAnims) {
        // (0x639D9B)
        if (!m_pBeachAnimBlock->IsLoaded) {
            return nullptr;
        }
    }

    // 0x639DA5: "beach" loaded -> bathing with a timer; otherwise (0x639DAF) TASK_NONE/nullptr
    if (m_pBeachAnimBlock->IsLoaded) {
        m_BathingTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_BathingTimer.m_nInterval  = 20'000 - static_cast<int32>(static_cast<float>(rand() & 0xFFFF) * 0.000030517578125f * -80000.0f); // [20000..100000)
        m_BathingTimer.m_bStarted   = true;
        m_bBathing                  = true;
        return createSubTask(TASK_SIMPLE_SUNBATHE);
    }
    if (shouldLoadSunbatheAnims) {
        return createSubTask(TASK_SIMPLE_PAUSE);
    }
    return nullptr;
}

// 0x6381A0
CTask* CTaskComplexSunbathe::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6381A0, CTaskComplexSunbathe*, CPed*>(this, ped);
}
