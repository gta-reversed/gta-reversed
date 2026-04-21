#pragma once

#include <cstddef>
#include "MemoryHeap.h"

#define MEMORY_MGR_XBOX_HEAP_SIZE       20656640 // from XBOX
#define MEMORY_MGR_PC_HEAP_SIZE         64 << 20
#define MEMORY_MGR_PC_SCRATCH_SIZE      0x4000
#define MEMORY_MGR_XBOX_SCRATCH_SIZE    0x5000
#define MEMORY_MGR_TRACKING_NUM_IDS     33 // from XBOX
#define MEMORY_MGR_HEAP_SIZE            MEMORY_MGR_PC_HEAP_SIZE

#ifdef MEMORY_MGR_USE_MEMORY_HEAP
#define MEMORY_MGR_OVERLOAD_NEW_DELETE
#define MEMORY_MGR_USE_HEAP_FLAGS
#endif

#ifdef MoveMemory
#undef MoveMemory
#endif

enum eHeapType {
    HEAP_PRIMARY,
    HEAP_SECONDARY,
    HEAP_SCRATCH,
};

enum eMemoryId {
    MEM_INTERNAL_OR_SYSTEM   = -1,
    MEM_0                    = 0,
    MEM_GAME                 = 1,
    MEM_WORLD                = 2,
    MEM_ANIMATION            = 3,
    MEM_4                    = 4,
    MEM_5                    = 5,
    MEM_DEFAULT_MODELS       = 6,
    MEM_STREAMING            = 7,
    MEM_8                    = 8,
    MEM_9                    = 9,
    MEM_10                   = 10,
    MEM_STREAMED_TEXTURES    = 11,
    MEM_STREAMED_COLLISION   = 12,
    MEM_STREAMED_ANIMATION   = 13,
    MEM_STREAMABLE_SCM       = 14,
    MEM_TEXTURES             = 15,
    MEM_COLLISION            = 16,
    MEM_PRE_ALLOC            = 17,
    MEM_GAME_PROCESS         = 18,
    MEM_SCRIPT               = 19,
    MEM_CARS                 = 20,
    MEM_RENDER               = 21,
    MEM_22                   = 22,
    MEM_FX                   = 23,
    MEM_24                   = 24,
    MEM_25                   = 25,
    MEM_26                   = 26,
    MEM_27                   = 27,
    MEM_28                   = 28,
    MEM_PATHS                = 29,
    MEM_30                   = 30,
    MEM_31                   = 31,
    MEM_32                   = 32,
};

static inline auto& s_MemoryHeapBuffer = StaticRef<char[16384]>(0xC8E0C8);
static inline auto& g_Heaps = StaticRef<std::array<CMemoryHeap, 3>>(0xC87B40);

// todo fixed stack
template <typename _Type, uint32 _nSize> class CStack {
protected:
    _Type  _aItems[_nSize];
    uint32 _nItemCount;

public:
    CStack() : _nItemCount(0) {}

    _Type pop() {
        assert(_nItemCount > 0);
        --_nItemCount;
        return _aItems[_nItemCount];
    }

    void push(_Type const& _refVal) {
        assert(_nItemCount + 1 < _nSize);
        _aItems[_nItemCount] = _refVal;
        ++_nItemCount;
    }

    _Type _top() const {
        assert(_nItemCount > 0);
        return _aItems[_nItemCount - 1];
    }
};

class CMemoryMgr {
public:
    static int8   m_idCapture;
    static inline int32* m_pBlockArray = *(int32**)0xC87B14;
    static inline int32  m_blocksUsed = *(int32*)0xC87B30;
    static inline int32* m_pMemUsedArray = *(int32**)0xC87B18;
    static inline int32  m_id = *(int32*)0xC87B1C;
    static inline auto& m_idStack = StaticRef<CStack<int32, 16>>(0xC87C18);
    static int8  m_largeAllocation;
    static int8  m_minLargeAllocation;
    static bool  m_bMallocHintDebugging;
    static inline int32 m_memUsed = *(int32*)0xC87B34;

public:
    static void InjectHooks();

    static void Init();
    static void InitMemoryTracking(int32 numIDs);
    static void Shutdown();

    static void  InitScratchPad();
    static void  LockScratchPad();
    static void* MallocAlignFromScratchPad(uint32, uint32);
    static void* MallocFromScratchPad(uint32);
    static void  ReleaseScratchPad();

    /** Allocates memory blocks.
     *  @param  size Number of bytes to allocate. Should be greater then zero.
     *  @param  hint A RwUInt32 value representing a memory hint.
     *  @return A void pointer to the allocated space, or NULL if
     *          there is insufficient memory available.
     */
    static void* Malloc(uint32 size, uint32 hint);

    /** Allocate aligned memory
     * NOTE: Use CMemoryMgr::FreeAlign to free memory
     *
     * @param   size   size in bytes
     * @param   align  memory align
     * @param   hint   hint to debugger (optional)
     * @return         pointer to memory or NULL if memory was not allocated
     */
    static uint8* MallocAlign(uint32 size, uint32 align, uint32 hint);

    /** Deallocates or frees a memory block.
     *  @param memory Previously allocated memory block to be freed.
     *         Shouldn't be NULL pointer.
     */
    static void Free(void* memory);

    /** Free previously allocated memory
     * @param memory pointer to previously allocated memory (can be NULL)
     */
    static void FreeAlign(void* memory);

    /** Reallocate memory blocks.
     *  @param mem  Pointer to previously allocated memory block.
     *  @param size New size in bytes. Should be greater then zero.
     *  @param hint A RwUInt32 value representing a memory hint.
     *  @return A void pointer to the allocated space, or NULL if
     *          there is insufficient memory available.
     */
    static uint8* Realloc(void* memory, uint32 size, uint32 hint);

    /** Allocates an array in memory with elements initialized to 0.
     *  @param numObj  Non-zero number of elements.
     *  @param sizeObj Non-zero length in bytes of each element.
     *  @param hint    A RwUInt32 value representing a memory hint.
     *  @return A void pointer to the allocated space, or NULL if
     *          there is insufficient memory available.
     */
    static uint8* Calloc(uint32 numObj, uint32 sizeObj, uint32 hint);

    static uint32 GetLargestFreeBlock();
    static uint32 GetSizeOfHoles();
    static uint32 GetMemoryUsed(eMemoryId id);
    static uint32 GetBlocksUsed(eMemoryId id);
    static void   SetRestrictMemoryMove(bool restrict);

    /** Push memory ID to internal ID stack. All allocated memory will have this ID.
     * @param id memory id
     */
    static void PushMemId(eMemoryId id);

    /*
     * Pop memory ID from internal ID stack
     */
    static void PopMemId();


    /**
     * no-hint shorthands
     * (used on mobile)
     */

    static void* MallocAlign(uint32 size, uint32 align);
    static void* Malloc(uint32 size);
    static uint8* Realloc(void* memory, uint32 size);
    static void* Calloc(uint32 num, uint32 size);

    /**
     * memory moving stuff
     */

    static void* MoveMemory(void* memory);
    static void* MoveMemory(void* memory, void** a2, int32 a3);

protected:
    /**
     * debug stuff
     */

    static void* InternalMalloc(uint32 size);
    static void* InternalRealloc(void* memory, uint32 size);
    static void RegisterFree(void* memory);
    static void RegisterMalloc(void* memory);

    static void PrintFrequencies();

public:
    static CMemoryHeap* GetHeapPtr(eHeapType heap) { return &g_Heaps[heap]; }
    static void SetHint(void* memory, const char* hint);
};
