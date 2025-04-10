#include "StdInc.h"

#include "MemoryHeap.h"
#include "FileMgr.h"

#define CORE_COMMON_SIZE_COUNT 64

static_assert(sizeof(HeapBlockDesc) <= MEMORY_HEAP_ALIGN, "HeapBlockDesc is too big");

uint32 memMoved = 0;

inline uint32 GetCommonSize(uint32 size) {
    return (size >> 4) - 1;
}

inline HeapBlockDesc* GetBlockDesc(void* mem) {
    assert(mem);
    return (HeapBlockDesc*)mem - 1;
}

inline HeapFreeBlockDesc* GetFreeBlockDesc(void* mem) {
    assert(mem);
    return (HeapFreeBlockDesc*)GetBlockDesc(mem);
}

// 0x72E860
CMemoryHeap::CMemoryHeap() {
    m_FirstBlock = nullptr;
}

// 0x72F180
void CMemoryHeap::Init(void* mem, uint32 size, bool bInitSizes) {
    m_nMemId   = -1;
    m_nMemUsed = 0;
    m_bMovingRestricted = true;

    // map free block onto memory
    m_FirstBlock              = (HeapFreeBlockDesc*)mem;
    m_FirstBlock->m_bInUse    = false;
    m_FirstBlock->m_nSize     = size - sizeof(HeapBlockDesc) - sizeof(HeapBlockDesc);
    m_FirstBlock->m_PrevBlock = nullptr;

    // create ending block
    m_LastBlock = m_FirstBlock->_GetNextLocatedBlock();
    m_LastBlock->m_bInUse    = true;
    m_LastBlock->m_nSize     = 0;
    m_LastBlock->m_nMemId    = -1;
    m_LastBlock->m_PrevBlock = m_FirstBlock;

    // update list
    m_FreeListBegin.m_Next = &m_FreeListEnd;
    m_FreeListBegin.m_Prev = &m_FreeListEnd;
    m_FreeListEnd.m_nSize  = std::numeric_limits<int32>::max();
    m_FreeListEnd.m_Prev   = &m_FreeListBegin;
    m_FreeListEnd.m_Next   = &m_FreeListBegin;

#ifdef MEMORY_MGR_USE_HEAP_FLAGS
    m_FreeListBegin.m_Flags.ResetFlags(true);
    m_FreeListEnd.m_Flags.ResetFlags(true);
    m_FirstBlock->m_Flags.ResetFlags(false);
    m_LastBlock->m_Flags.ResetFlags(true);
#endif

    // insert mapped free block
    m_FreeListBegin.InsertHeapFreeBlock((HeapFreeBlockDesc*)m_FirstBlock);

    m_paCommonSizes = nullptr;
    if (bInitSizes) {
        m_paCommonSizes = (CommonSize*)Malloc(sizeof(CommonSize) * MEMORY_HEAP_CSIZES);
        int32 asize = MEMORY_HEAP_ALIGN;
        for (auto i = 0; i < MEMORY_HEAP_CSIZES; ++i) {
            m_paCommonSizes[i].Init(asize);
            asize += MEMORY_HEAP_ALIGN;
        }
    }
}

// 0x72EC60
void CMemoryHeap::Shutdown() {
    PrintMemoryLeaks();

    CMemoryMgr::Free(m_FirstBlock);
    m_FirstBlock = nullptr;
}

// 0x72EB10
void CMemoryHeap::RegisterFree(HeapFreeBlockDesc* desc) {
    m_nMemUsed -= desc->m_nSize + sizeof(HeapBlockDesc);
}

// 0x72EAF0
void CMemoryHeap::RegisterMalloc(HeapBlockDesc* desc) {
#ifdef MEMORY_MGR_USE_HEAP_FLAGS
    assert(!desc->m_Flags.IsBoundBlock);
#endif
    desc->m_bInUse = true;
    desc->m_nMemId = m_nMemId;
    m_nMemUsed += desc->m_nSize + sizeof(HeapBlockDesc);
}

HeapFreeBlockDesc* CMemoryHeap::_JoinFreeBlocks(HeapFreeBlockDesc* desc) {
    // join next located block
    auto* next = (HeapFreeBlockDesc*)desc->_GetNextLocatedBlock();
    if (next && !next->m_bInUse) {
        next->RemoveHeapFreeBlock();
        next = (HeapFreeBlockDesc*)((uint8*)next + next->m_nSize + sizeof(HeapBlockDesc));
    }
    assert(next);

    // join prev located block
    auto* prev = (HeapFreeBlockDesc*)desc->m_PrevBlock;
    if (prev && !prev->m_bInUse) {
        prev->RemoveHeapFreeBlock();
        desc = prev;
    }

    // recount total size
    desc->m_nSize = (uint8*)next - (uint8*)desc - sizeof(HeapBlockDesc);

    // update pointer to previous located block
    next->m_PrevBlock = desc;

    if (m_paCommonSizes && desc->m_nSize < (MEMORY_HEAP_CSIZES * MEMORY_HEAP_ALIGN)) {
        m_paCommonSizes[GetCommonSize(desc->m_nSize)].Free(desc);
    } else {
        m_FreeListBegin.InsertOrderedHeapFreeBlock(desc);
    }

    return (HeapFreeBlockDesc*)desc;
}

// 0x72EE60
void CMemoryHeap::Free(void* memory) {
    if (!memory) {
        return;
    }
    assert(IsHeapPointer(memory));

    HeapFreeBlockDesc* desc = GetFreeBlockDesc(memory);

#ifdef MEMORY_MGR_USE_HEAP_FLAGS
    assert(!desc->m_Flags.bIsBoundBlock && "Cannot free BOUND heap block!");
#endif
    assert(desc->m_bInUse && "Memory block was already deallocated!");

    // free this block
    RegisterFree(desc);
    desc->m_bInUse = false;

    // join prev and next free blocks
    desc = _JoinFreeBlocks(desc);

#ifdef MEMORY_MGR_USE_HEAP_FLAGS
    desc->m_Flags.ResetFlags(false);
#endif

#ifdef _DEBUG
    desc->m_nMemId = 0;
#endif
}

// 0x72EEF0
void* CMemoryHeap::Realloc(void* memory, uint32 size) {
    if (!memory) {
        return Malloc(size);
    }

    auto nBlockSize = size;
    if (size & (MEMORY_HEAP_ALIGN - 1)) {
        nBlockSize = (size & ~(MEMORY_HEAP_ALIGN - 1)) + MEMORY_HEAP_ALIGN;
    }

    assert(IsHeapPointer(memory));
    HeapBlockDesc* desc = GetBlockDesc(memory);

    // extend block
    if (int32(nBlockSize) > GetBlockDesc(memory)->m_nSize) {
        // __DUMMY();

        void* newMem = CMemoryHeap::Malloc(nBlockSize);
        memcpy(newMem, memory, GetBlockDesc(memory)->m_nSize);
        CMemoryHeap::Free(memory);
        return newMem;
    }

    // shrink block
    m_nMemUsed -= desc->m_nSize + sizeof(HeapBlockDesc); // todo: RegisterFree?
    FillInBlockData(GetBlockDesc(memory), desc->_GetNextLocatedBlock(), nBlockSize);
    return memory;
}

// 0x72ED80
void* CMemoryHeap::Malloc(uint32 size) {
    auto blockSize = size;
    if (size & (MEMORY_HEAP_ALIGN - 1)) {
        blockSize = (size & ~(MEMORY_HEAP_ALIGN - 1)) + MEMORY_HEAP_ALIGN;
    }

    auto commonSize = GetCommonSize(blockSize);

    if (m_paCommonSizes && blockSize <= (MEMORY_HEAP_CSIZES * MEMORY_HEAP_ALIGN) && commonSize < MEMORY_HEAP_CSIZES) {
        while (commonSize < MEMORY_HEAP_CSIZES) {
            HeapFreeBlockDesc* block = m_paCommonSizes[commonSize].Malloc();
            if (block) {
                RegisterMalloc(block);
#ifdef MEMORY_MGR_USE_HEAP_FLAGS
                block->m_Flags.ResetFlags();
#endif
                return block->_GetBlockData();
            }
            ++commonSize;
        }
    }

    HeapFreeBlockDesc* block = m_FreeListBegin.m_Next;
    while (blockSize > uint32(block->m_nSize)) {
        block = block->m_Next;
        if (block == &m_FreeListEnd) {
            DEV_LOG("CMemoryHeap[{:02d}]: !!! failed to allocate {} bytes !!!", m_nMemId, block->m_nSize);
            return nullptr;
        }
    }

    // heap corruption check
    assert(IsHeapPointer(block) && "Heap is corrupted!");
    assert((uint32)block->m_nSize < GetHeapSize() && "Heap is corrupted!");

    block->RemoveHeapFreeBlock();
    FillInBlockData(block, block->_GetNextLocatedBlock(), blockSize);
#ifdef MEMORY_MGR_USE_HEAP_FLAGS
    block->m_Flags.ResetFlags();
#endif
    return block->_GetBlockData();
}

// 0x72EC80
void CMemoryHeap::FillInBlockData(HeapBlockDesc* desc, HeapBlockDesc* nextDesc, uint32 size) {
    // __DUMMY();
    // DEV_LOG("CMemoryHeap[MemID:{:02d}]: requesting {} bytes from {}K block", m_nMemId, _nSize, (uint32)(_pDesc->_iSize / 1024.0));

    desc->m_nSize = size; // setup size

    auto* next = (HeapFreeBlockDesc*)desc->_GetNextLocatedBlock();
    if (next < &nextDesc[-1]) {
        RegisterMalloc(desc);

        //  make new next free block
        next->m_nSize = (uint8*)nextDesc - (uint8*)next - sizeof(HeapBlockDesc);
        next->m_PrevBlock = desc;
        next->m_bInUse = false;

#ifdef MEMORY_MGR_USE_HEAP_FLAGS
        next->m_Flags.ResetFlags(false);
#endif

        nextDesc->m_PrevBlock = next;

        assert(next->_GetNextLocatedBlock() == nextDesc);

        _JoinFreeBlocks(next);

        return;
    }

    if (next < nextDesc) {
        desc->m_nSize = (uint8*)nextDesc - (uint8*)desc - sizeof(HeapBlockDesc);
    }
    nextDesc->m_PrevBlock = desc;
    RegisterMalloc(desc);
}

void CMemoryHeap::PrintMemoryLeaks() {
#if defined(CORE_USE_MEMORY_HEAP)
    if (!m_nMemUsed)
        return;

    if (m_paCommonSizes && m_nMemUsed == sizeof(CommonSize) * MEMORY_HEAP_CSIZES) {
        return;
    }

    DEV_LOG("MemoryHeap 0x{:08x} LEAKS (used {} bytes out of {} bytes total)\n", m_FirstBlock, m_nMemUsed, _GetHeapSize());
    DEV_LOG("-----------------------------------------------------------------------------------------------------------\n");

    HeapBlockDesc* p = m_FirstBlock;
    while (p < m_LastBlock) {
        if (p->m_bInUse) {
            p->_DumpBlockInfo();
        }
        p = p->_GetNextLocatedBlock();
    }
#endif
}

void* CMemoryHeap::GetDebugInfo(void* memory) {
    assert(memory);
    return GetBlockDesc(memory)->m_pDebugInfo;
}

void CMemoryHeap::SetDebugInfo(void* memory, void* nDebugInfo) {
    assert(memory);
    GetBlockDesc(memory)->m_pDebugInfo = nDebugInfo;
}

// 0x72E980
uint32 CMemoryHeap::GetSizeOfHoles() {
    uint32 nHolesSize = 0;

    HeapFreeBlockDesc* block;

    for (block = m_FreeListBegin.m_Next; block != &m_FreeListEnd; block = block->m_Next) {
        if (block->m_Next == &m_FreeListEnd)
            break;
        nHolesSize += block->m_nSize;
    }

    if (m_paCommonSizes) {
        for (auto i = 0; i < CORE_COMMON_SIZE_COUNT; ++i) {
            block = m_paCommonSizes[i].m_ListStart.m_Next;
            while (block != &m_paCommonSizes[i].m_ListEnd) {
                nHolesSize += block->m_nSize;
                block = block->m_Next;
            }
        }
    }

    return nHolesSize;
}

// 0x72E8E0
void CMemoryHeap::IntegrityCheck() {
    plugin::CallMethod<0x72E8E0, CMemoryHeap*>(this);
}

void* CMemoryHeap::MoveHeapBlock(HeapFreeBlockDesc* free, HeapBlockDesc* used) {
    assert(free && used);

    auto oldHeapID = m_nMemId;
    m_nMemId = used->m_nMemId;
    free->RemoveHeapFreeBlock();
    FillInBlockData(free, free->_GetNextLocatedBlock(), used->m_nSize);
    m_nMemId = oldHeapID;
    memcpy(free->_GetBlockData(), used->_GetBlockData(), used->m_nSize);
    memMoved += used->m_nSize;
    Free(used->_GetBlockData());
    return nullptr;
}

HeapFreeBlockDesc* CMemoryHeap::WhereShouldMemoryMove(void* memory) {
    assert(true && "NOT IMPLEMENTED");
    return nullptr;
}

void* CMemoryHeap::MoveMemory(void* memory) {
    if (HeapFreeBlockDesc* to = WhereShouldMemoryMove(memory)) {
        return MoveHeapBlock(to, GetBlockDesc(memory));
    }
    return memory;
}

void* CMemoryHeap::MoveMemory(void* memory, void** _ppUnk, int32 _iUnk) {
    assert(true && "NOT IMPLEMENTED");
    return memory;
}

// 0x72E9E0
void CMemoryHeap::ParseHeap() {
    auto* file = CFileMgr::OpenFileForWriting("heap.txt");
    // assert(file && "Failed to open %s for writing!", "heap.txt");

    CTimer::Stop();

    IntegrityCheck();

    HeapBlockDesc* block = m_FirstBlock;
    uint32 k = 0;
    char buffer[16] = {0};

    while (block < m_LastBlock) {
        char cState = '*';
        if (block->m_nMemId & 0xFF) {
            cState = '@' + (block->m_nMemId & 0xFF);
        }

        if ((k & 63) == 0) { // '?'
            sprintf_s(buffer, "\n%5dK:", k / 64);
            CFileMgr::Write(file, buffer, 8);
        }

        CFileMgr::Write(file, "#", 1);
        ++k;

        for (uint32 size = block->m_nSize >> 4; size; --size) {
            if ((k & 63) == 0) {
                sprintf_s(buffer, "\n%5dK:", k / 64);
                CFileMgr::Write(file, buffer, 8);
            }
            CFileMgr::Write(file, &cState, sizeof(char));
            ++k;
        }
        block = block->_GetNextLocatedBlock();
    }

    CTimer::Update();

    CFileMgr::CloseFile(file);
}

void CMemoryHeap::_DumpHeapStatus(bool bCommonSizes) {
    DEV_LOG("MemoryHeap 0x{:08x} STATUS (used {} bytes out of {} bytes total)", LOG_PTR(m_FirstBlock), m_nMemUsed, GetHeapSize());

    DEV_LOG(" - Block map");
    HeapBlockDesc* p = m_FirstBlock;
    while (p != m_LastBlock) {
        p->_DumpBlockInfo();
        p = p->_GetNextLocatedBlock();
    }

    DEV_LOG(" - Free list");
    HeapFreeBlockDesc* pF = &m_FreeListBegin;
    do {
        pF->_DumpBlockInfo();
        pF = pF->m_Next;
    } while (pF != m_FreeListEnd.m_Next);

    if (m_paCommonSizes && bCommonSizes) {
        DEV_LOG(" - CommonSizes");
        for (auto i = 0; i < CORE_COMMON_SIZE_COUNT; ++i) {
            if (m_paCommonSizes[i].m_NumFreeBlocks) {
                m_paCommonSizes[i]._DumpInfo();
            }
        }
    }

    DEV_LOG(" Percentage used:    {:0.1f}%", 100.f * float(m_nMemUsed) / float(GetHeapSize()));
    DEV_LOG(" Largest free block: {} bytes", GetLargestFreeBlock());
    DEV_LOG(" Holes size:         {} bytes", GetSizeOfHoles());
}

HeapBlockDesc* CMemoryHeap::_FindBlockDesc(void* memory) {
    HeapBlockDesc* p = m_FirstBlock;
    while (p < m_LastBlock) {
        if (p->m_nMemId != -1) {
            if (memory >= p->_GetBlockData()) {
                if ((int8*)memory < (int8*)p->_GetBlockData() + p->m_nSize) {
                    return p;
                }
            }
        }
        p = p->_GetNextLocatedBlock();
    }
    return nullptr;
}
