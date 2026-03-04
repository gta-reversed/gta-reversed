#pragma once

#define GET_HEAP_DESC(mem) ((HeapBlockDesc*)(mem)-1)

#ifdef MEMORYMGR_USE_HEAP_FLAGS

/**
 * Heap block flags
 * NOTE: this is NOT an original Rockstar structure!!!
 */
struct HeapBlockFlags {
    union {
        struct {
            bool Initialised : 1;
            bool IsBoundBlock : 1;
            bool NoDebugHint : 1;
            bool InternalDebugInfo : 1;
            bool StringDebugInfo : 1; // null terminated string
            bool AllocatedUsingNew : 1;
            bool IsArray : 1;
        };
        uint8 value;
    };

    void ResetFlags(bool IsBound = false) {
        value = 0;
        Initialised = true;
        IsBoundBlock = IsBound;
    }
};

#endif

/**
 * Common heap block descriptor
 */
class HeapBlockDesc {
public:
    int32 m_nSize;
    bool  m_bInUse;

#ifdef MEMORYMGR_USE_HEAP_FLAGS
    HeapBlockFlags Flags; //< +0x05  gap (I've used this field for my own flags)
#endif

    int16 m_nMemId; // HeapID ?
    union {
        void*        m_pDebugInfo;   // UIntPtr
        uint8 const* m_upDebugInfo;  // (can be useful, lol)
    };
    HeapBlockDesc* m_PrevBlock; // prev located block ?

public:
    HeapBlockDesc();

    void InsertHeapBlock(HeapBlockDesc* desc);

    HeapBlockDesc* _GetNextLocatedBlock() const;
    void* _GetBlockData() const;
    void _DumpBlockInfo() const;
};

VALIDATE_SIZE(HeapBlockDesc, 0x10);
