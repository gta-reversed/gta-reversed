#include "StdInc.h"
#include "TaskComplexSunbathe.h"

#include "AnimManager.h"
#include "Clock.h"
#include "General.h"
#include "InterestingEvents.h"
#include "Population.h"
#include "Streaming.h"
#include "TaskSimplePause.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleRunTimedAnim.h"
#include "Timer.h"
#include "Weather.h"
#include "World.h"

void CTaskComplexSunbathe::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSunbathe, 0x86e0ac, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x631F80);
    RH_ScopedInstall(Destructor, 0x632050);

    RH_ScopedInstall(CreateSubTask, 0x638290);
    RH_ScopedGlobalInstall(CanSunbathe, 0x632140);
    RH_ScopedGlobalInstall(ShouldLoadSunbatheAnims, 0x632190);

    RH_ScopedVMTInstall(Clone, 0x6366A0);
    RH_ScopedVMTInstall(GetTaskType, 0x632040);
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
        const auto eventType = event->GetEventType();
        if (eventType == EVENT_DAMAGE || eventType == EVENT_DEATH) {
            return false;
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
    if (CClock::ms_nGameClockHours < 10 || CClock::ms_nGameClockHours > 17) {
        return false;
    }
    switch (CWeather::NewWeatherType) {
    case WEATHER_EXTRASUNNY_LA:
    case WEATHER_SUNNY_SMOG_LA:
    case WEATHER_SUNNY_SF:
    case WEATHER_SUNNY_COUNTRYSIDE:
    case WEATHER_SUNNY_VEGAS:
    case WEATHER_EXTRASUNNY_DESERT:
        return true;
    default:
        break;
    }
    switch (CWeather::NewWeatherType) {
    case WEATHER_EXTRASUNNY_LA:
    case WEATHER_SUNNY_LA:
    case WEATHER_CLOUDY_LA:
    case WEATHER_SUNNY_SF:
    case WEATHER_CLOUDY_COUNTRYSIDE:
    case WEATHER_EXTRASUNNY_DESERT:
        return true;
    default:
        return false;
    }
}

// 0x632190
bool CTaskComplexSunbathe::ShouldLoadSunbatheAnims() {
    const auto& player = CWorld::Players[CWorld::PlayerInFocus];
    if ((player.m_pPed->physicalFlags.bSubmergedInWater || !player.m_pPed->m_pVehicle)
        && player.m_pPed->GetMoveSpeed().SquaredMagnitude() < 0.04f
    ) {
        return false;
    }
    return true;
}

// 0x638290
CTask* CTaskComplexSunbathe::CreateSubTask(eTaskType taskType, CPed* ped) {
    if (m_pSubTask && m_pSubTask->GetTaskType() == taskType) {
        return m_pSubTask;
    }
    switch (taskType) {
    case TASK_SIMPLE_START_SUNBATHING: { // 0x1AC
        return new CTaskSimpleRunAnim{
            ANIM_GROUP_SUNBATHE,
            static_cast<AnimationId>(static_cast<int32>(m_SunbatherType) + ANIM_ID_PARKSIT_M_IN),
            4.0f,
            TASK_SIMPLE_START_SUNBATHING,
            "start sunbathing",
            true,
        };
    }
    case TASK_SIMPLE_SUNBATHE: { // 0x1A2
        const auto animId = static_cast<AnimationId>(static_cast<int32>(m_SunbatherType) + ANIM_ID_PARKSIT_M_LOOP);
        const auto GetRemainingPlus1s = [this] {
            const auto& t = m_BathingTimer;
            return t.m_bStarted ? static_cast<int32>(t.m_nStartTime + t.m_nInterval - CTimer::GetTimeInMS()) + 1000 : 1000;
        };
        int32 durationMs = GetRemainingPlus1s();
        if (m_pSunbatheAnimBlock->IsLoaded && (m_SunbatherType == SUNBATHER_MALE_1 || m_SunbatherType == SUNBATHER_FEMALE_2)) {
            if (durationMs > 12000) {
                durationMs = 12000;
            } else {
                durationMs = CGeneral::GetRandomNumberInRange(3000, durationMs);
            }
        }
        return new CTaskSimpleRunTimedAnim{
            ANIM_GROUP_BEACH, // Loop anims (239-243) live in the beach block
            animId,
            4.0f,
            -4.0f,
            static_cast<uint32>(durationMs),
            TASK_SIMPLE_SUNBATHE,
            "sunbathe",
            true,
        };
    }
    case TASK_SIMPLE_PAUSE: { // 0xCA
        if ((!m_pSunbatheAnimBlock->IsLoaded && ShouldLoadSunbatheAnims()) || !m_pBeachAnimBlock->IsLoaded) {
            return new CTaskSimplePause{ 10000 };
        }
        return new CTaskSimplePause{ CGeneral::GetRandomNumberInRange(1000, 5000) };
    }
    case TASK_SIMPLE_IDLE_SUNBATHING: { // 0x1AD
        AnimationId animId;
        if (m_SunbatherType == SUNBATHER_MALE_1) {
            animId = static_cast<AnimationId>(CGeneral::GetRandomNumberInRange(0, 3) + ANIM_ID_PARKSIT_M_IDLEA);
        } else if (m_SunbatherType == SUNBATHER_FEMALE_2) {
            animId = static_cast<AnimationId>(CGeneral::GetRandomNumberInRange(0, 3) + ANIM_ID_PARKSIT_M_IDLEA + 3);
        } else {
            animId = static_cast<AnimationId>(TASK_SIMPLE_IDLE_SUNBATHING);
        }
        return new CTaskSimpleRunAnim{
            ANIM_GROUP_SUNBATHE,
            animId,
            4.0f,
            TASK_SIMPLE_IDLE_SUNBATHING,
            "idle sunbathing",
            true,
        };
    }
    case TASK_SIMPLE_STOP_SUNBATHING: { // 0x1AE
        return new CTaskSimpleRunAnim{
            ANIM_GROUP_SUNBATHE,
            static_cast<AnimationId>(static_cast<int32>(m_SunbatherType) + ANIM_ID_PARKSIT_M_OUT),
            4.0f,
            TASK_SIMPLE_STOP_SUNBATHING,
            "stop sunbathing",
            false,
        };
    }
    case TASK_FINISHED: // 0x516
    default:
        return nullptr;
    }
}

// 0x6381A0
CTask* CTaskComplexSunbathe::ControlSubTask(CPed* ped) {
    if (m_bAborted) {
        return nullptr;
    }
    if (!m_bBeachAnimsReferenced) {
        if (m_pBeachAnimBlock->IsLoaded) {
            CAnimManager::AddAnimBlockRef(m_BeachAnimBlockIndex);
            m_bBeachAnimsReferenced = true;
        } else {
            CStreaming::RequestModel(IFPToModelId(m_BeachAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
        }
    }
    if (!m_bSunbatheAnimsReferenced) {
        if (ShouldLoadSunbatheAnims()) {
            if (m_pSunbatheAnimBlock->IsLoaded) {
                CAnimManager::AddAnimBlockRef(m_SunbatheAnimBlockIndex);
                m_bSunbatheAnimsReferenced = true;
            } else {
                CStreaming::RequestModel(IFPToModelId(m_SunbatheAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
            }
        }
    }
    if (m_bBathing) {
        ped->physicalFlags.bCollidable = true;
        ped->bFallenDown = true;
    }
    if (!CanSunbathe()) {
        if (m_BathingTimer.IsStarted()) {
            m_BathingTimer.Pause();
        }
        if (m_pSubTask->GetTaskType() == TASK_SIMPLE_SUNBATHE) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
        }
    }
    return m_pSubTask;
}

// 0x6399F0
CTask* CTaskComplexSunbathe::CreateNextSubTask(CPed* ped) {
    if (ped->m_pVehicle && (ped->m_pVehicle->m_nVehicleType == VEHICLE_TYPE_BOAT || ped->m_nPedType == PED_TYPE_COP)) {
        g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_2, ped);
    }
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_PAUSE: { // 0xCA
        if (!m_bBathing) {
            if (m_pBeachAnimBlock->IsLoaded && m_pSunbatheAnimBlock->IsLoaded) {
                return CreateSubTask(TASK_SIMPLE_START_SUNBATHING, ped);
            }
        } else if (ShouldLoadSunbatheAnims() && m_pSunbatheAnimBlock->IsLoaded) {
            return CreateSubTask(TASK_SIMPLE_STOP_SUNBATHING, ped);
        }
        break;
    }
    case TASK_SIMPLE_SUNBATHE: { // 0x1A2
        if (!m_pSunbatheAnimBlock->IsLoaded) {
            return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
        }
        if (CanSunbathe() && m_BathingTimer.IsOutOfTime() && (m_SunbatherType == SUNBATHER_MALE_1 || m_SunbatherType == SUNBATHER_FEMALE_2)) {
            return CreateSubTask(TASK_SIMPLE_IDLE_SUNBATHING, ped);
        }
        return CreateSubTask(TASK_SIMPLE_STOP_SUNBATHING, ped);
    }
    case TASK_SIMPLE_START_SUNBATHING: { // 0x1AC
        if (m_pBeachAnimBlock->IsLoaded) {
            m_BathingTimer.Start(CGeneral::GetRandomNumberInRange(20000, 100000));
            m_bBathing = true;
            return CreateSubTask(TASK_SIMPLE_SUNBATHE, ped);
        }
        break;
    }
    case TASK_SIMPLE_IDLE_SUNBATHING: { // 0x1AD
        if (!m_pBeachAnimBlock->IsLoaded) {
            return CreateSubTask(TASK_FINISHED, ped);
        }
        return CreateSubTask(TASK_SIMPLE_SUNBATHE, ped);
    }
    case TASK_SIMPLE_STOP_SUNBATHING: { // 0x1AE
        m_bBathing = false;
        return CreateSubTask(TASK_FINISHED, ped);
    }
    default:
        return nullptr;
    }
    return CreateSubTask(TASK_FINISHED, ped);
}

// 0x639CB0
CTask* CTaskComplexSunbathe::CreateFirstSubTask(CPed* ped) {
    rand();
    if (ped->m_nPedType == PED_TYPE_COP) {
        rand();
        m_SunbatherType = static_cast<eSunbatherType>(2 - static_cast<int32>(CGeneral::GetRandomNumberInRange(0.0f, 1.0f)));
    } else if (!CPopulation::IsSunbather(ped->GetModelId())) {
        m_SunbatherType = SUNBATHER_MALE_2;
    } else {
        m_SunbatherType = rand() < 0x3FFF ? SUNBATHER_MALE_1 : SUNBATHER_FEMALE_1;
    }
    if (!m_pBeachAnimBlock->IsLoaded) {
        CStreaming::RequestModel(IFPToModelId(m_BeachAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
    }
    if (!m_pSunbatheAnimBlock->IsLoaded) {
        CStreaming::RequestModel(IFPToModelId(m_SunbatheAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
    }
    m_bBathing = false;
    if (m_bStartStanding ? !ShouldLoadSunbatheAnims() : ShouldLoadSunbatheAnims()) {
        if (!m_bStartStanding || (m_pTowel || m_pSunbatheAnimBlock->IsLoaded)) {
            if (m_bStartStanding || m_pBeachAnimBlock->IsLoaded) {
                rand();
                m_BathingTimer.Start(20000 - static_cast<int32>(CGeneral::GetRandomNumberInRange(0.0f, 1.0f)));
                m_bBathing = true;
                return CreateSubTask(TASK_SIMPLE_SUNBATHE, ped);
            }
            if (ShouldLoadSunbatheAnims()) {
                return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
            }
            return nullptr;
        }
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
    }
    if (!m_pBeachAnimBlock->IsLoaded) {
        return nullptr;
    }
    return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
}
