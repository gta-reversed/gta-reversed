#pragma once

class CPed;
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
    CPed*  m_Ped0;
    int32  m_GroupId;
    CPed*  m_Ped1;
    uint32 m_Time;

public:
    CTaskAllocator() = default;
    CTaskAllocator(CPed* ped);
    virtual ~CTaskAllocator() = default;

    static void* operator new(size_t size);
    static void operator delete(void* obj);

    virtual void AllocateTasks(CPedGroupIntelligence* intel) = 0;
    virtual void ProcessGroup(CPedGroupIntelligence* intel);
    virtual bool IsFinished(CPedGroupIntelligence* intel);
    virtual void Abort();
    virtual eTaskAllocatorType GetType() = 0;
};
VALIDATE_SIZE(CTaskAllocator, 0x14);
