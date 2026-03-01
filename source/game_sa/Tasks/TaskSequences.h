#pragma once

#include "TaskComplexSequence.h"

class CTaskSequences {
public:
    static constexpr auto NUM_SEQUENCES = 64;

    static int32& ms_iActiveSequence;
    static CTaskComplexSequence (&ms_taskSequence)[NUM_SEQUENCES];
    static bool (&ms_bIsOpened)[NUM_SEQUENCES];

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
