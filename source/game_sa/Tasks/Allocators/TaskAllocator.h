#pragma once

class CPed;
class CPedGroup;
class CPedGroupIntelligence;

enum eTaskAllocatorType {
    TASK_ALLOCATOR_ATTACK = 1,
    TASK_ALLOCATOR_RETREAT = 2,

    TASK_ALLOCATOR_KILL_ON_FOOT = 4,

    TASK_ALLOCATOR_PLAYER_COMMAND_ATTACK = 6,
    TASK_ALLOCATOR_PLAYER_COMMAND_RETREAT = 7,

    TASK_ALLOCATOR_KILL_THREATS_BASIC = 9,
    TASK_ALLOCATOR_KILL_THREATS_BASIC_RANDOM_GROUP = 10,
    TASK_ALLOCATOR_KILL_THREATS_DRIVEBY = 11,
};

class NOTSA_EXPORT_VTABLE CTaskAllocator {
public:
    /* ACHTUNG:
    * MAKE SURE YOU VMT DEFINITION IS CORRECT!
    * If not, the order below is the correct one, so copy that.
    */

    static void* operator new(size_t size);
    static void operator delete(void* obj);

    static void InjectHooks();

public:
    CTaskAllocator() = default;
    virtual ~CTaskAllocator() = default;

    virtual void               AllocateTasks(CPedGroupIntelligence* intel) = 0;
    virtual CTaskAllocator*    ProcessGroup(CPedGroupIntelligence* intel) { return nullptr; } // 0x69BB50
    virtual bool               IsFinished(CPedGroupIntelligence* intel); // 0x69C3C0
    virtual void               Abort() { /* nop */ } // 0x5F68E0
    virtual eTaskAllocatorType GetType() = 0;
};
VALIDATE_SIZE(CTaskAllocator, sizeof(void**)); // only vmt
