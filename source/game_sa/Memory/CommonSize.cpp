#include "StdInc.h"

#include "CommonSize.h"

void CommonSize::Init(int32 blockSize) {
    m_ListEnd.m_Prev   = &m_ListStart;
    m_ListStart.m_Next = &m_ListEnd;

    m_BlockSize     = blockSize;
    m_NumFails      = 0;
    m_NumFreeBlocks = 0;

#ifdef MEMORY_MGR_USE_HEAP_FLAGS
    m_ListEnd.m_Flags.ResetFlags(true);
    m_ListStart.m_Flags.ResetFlags(true);
#endif
}

HeapFreeBlockDesc* CommonSize::Malloc() {
    if (m_ListStart.m_Next != &m_ListEnd) {
        HeapFreeBlockDesc* block = m_ListStart.m_Next;
        m_ListStart.m_Next->RemoveHeapFreeBlock();
        --m_NumFreeBlocks;
        return block;
    }
    m_NumFails++;
    return nullptr;
}

void CommonSize::Free(HeapFreeBlockDesc* desc) {
    assert(desc);
    m_ListStart.InsertHeapFreeBlock(desc);
    m_NumFreeBlocks++;
}

void CommonSize::_DumpInfo() {
    DEV_LOG("CommonSize (blockSize {}, failed {}, free {}):\n", m_BlockSize, m_NumFails, m_NumFreeBlocks);
    HeapFreeBlockDesc* block = &m_ListStart;
    while (true) {
        block->_DumpBlockInfo();
        block = block->m_Next;
        if (block == m_ListEnd.m_Next) {
            return;
        }
    }
}
