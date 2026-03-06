#pragma once

#include "TaskComplexSequence.h"

class CTaskSequences {
public:
    static constexpr auto NUM_SEQUENCES = 64;

    static inline auto& ms_iActiveSequence = StaticRef<int32>(0x8D2E98);
    static inline auto (&ms_taskSequence)[NUM_SEQUENCES] = StaticRef<CTaskComplexSequence[NUM_SEQUENCES]>(0xC178F0);
    static inline auto (&ms_bIsOpened)[NUM_SEQUENCES] = StaticRef<bool[NUM_SEQUENCES]>(0xC17898);

public:
    static void InjectHooks();

    static void Init();
    static void CleanUpForShutdown();
    static int32 GetAvailableSlot(uint8 slot);

    static CTaskComplexSequence& GetActiveSequence() { return ms_taskSequence[ms_iActiveSequence]; }
    static inline void AddTaskToActiveSequence(CTask* task) {
        ms_taskSequence[ms_iActiveSequence].AddTask(task);
    }
    static auto Get(uint32 slot) { return &ms_taskSequence[slot]; }
};
