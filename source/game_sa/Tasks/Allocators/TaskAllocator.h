#pragma once

class CPed;
class CPedGroup;
class CPedGroupIntelligence;

enum class eTaskAllocatorType : int32 {
    NONE                            = 0,  // 0x0
    ATTACK                          = 1,  // 0x1
    RETREAT                         = 2,  // 0x2
    COVER_SEEK                      = 3,  // 0x3
    KILL_ON_FOOT                    = 4,  // 0x4
    KILL_ON_FOOT_USING_COVER        = 5,  // 0x5
    PLAYER_COMMAND_ATTACK           = 6,  // 0x6
    PLAYER_COMMAND_RETREAT          = 7,  // 0x7
    ADVANCE_IN_FORMATION            = 8,  // 0x8
    KILL_THREATS_BASIC              = 9,  // 0x9
    KILL_THREATS_BASIC_RANDOM_GROUP = 10, // 0xA
    KILL_THREATS_DRIVEBY            = 11, // 0xB
};

class NOTSA_EXPORT_VTABLE CTaskAllocator {
public:
    /* ACHTUNG:
    * MAKE SURE YOU VMT DEFINITION IS CORRECT (in the database)!
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

    template<typename From, typename To>
    constexpr bool classof(const From* f) { return f->GetType() == To::Type; }
};
VALIDATE_SIZE(CTaskAllocator, sizeof(void**)); // only vmt
