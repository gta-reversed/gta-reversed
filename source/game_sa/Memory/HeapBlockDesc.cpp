#include "StdInc.h"

#include "HeapBlockDesc.h"
#include "HeapFreeBlockDesc.h"

HeapBlockDesc::HeapBlockDesc() {
    m_nSize      = 0;
    m_bInUse     = false;
    m_nMemId     = 0;
    m_pDebugInfo = nullptr;
    m_PrevBlock  = nullptr;
#ifdef MEMORY_MGR_USE_HEAP_FLAGS
    m_Flags.value = 0;
#endif
}

void HeapBlockDesc::InsertHeapBlock(HeapBlockDesc* desc) {
    // missing
}

HeapBlockDesc* HeapBlockDesc::_GetNextLocatedBlock() const {
    return (HeapBlockDesc*)((uint8*)this + sizeof(HeapBlockDesc) + m_nSize);
}

void* HeapBlockDesc::_GetBlockData() const {
    return (uint8*)this + sizeof(HeapBlockDesc);
}

void HeapBlockDesc::_DumpBlockInfo() const {
    auto const* self = reinterpret_cast<HeapFreeBlockDesc const*>(this);

    const char* info = "";
#ifdef MEMORY_MGR_USE_HEAP_FLAGS
    char szInfo[128] = { 0 };
    info = szInfo;

    if (self->m_Flags.AllocatedUsingNew) {
        if (self->m_Flags.IsArray)
            strcpy_s(szInfo, "operator new[]");
        else
            strcpy_s(szInfo, "operator new  ");
        info = szInfo;
    }

    if (!self->m_Flags.NoDebugHint) {
        if (szInfo[0])
            strcat_s(szInfo, " ");

        if (self->m_Flags.StringDebugInfo)
            sprintf_s(szInfo + strlen(szInfo), "[Hint: %s]", self->m_upDebugInfo); // TODO: fix
        else
            sprintf_s(szInfo + strlen(szInfo), "[Hint: 0x%08x]", self->m_pDebugInfo); // TODO: fix

        info = szInfo;
    }
#endif

    /*char* type = "";
    if (!p->m_bInUse) {
        type = "FREE";
    }*/
#ifdef MEMORY_MGR_USE_HEAP_FLAGS
    if (m_Flags.IsBoundBlock) {
        type = "BOUND";
    } else if (m_nMemId == -1) {
        type = "SYS";
    }
#endif

    char szMem[64] = { 0 };
    for (int32 i = 0; i < 16; ++i) { // todo: magic number (related to m_idStack size?)
        sprintf_s(szMem + strlen(szMem), 3u, "%02x", ((uint8*)self->_GetBlockData())[i]);
        if (i + 1 != 16) {
            strcat_s(szMem, " ");
        }
    }

    // NOTSA_LOG_DEBUG("  {:6s} [{:03d}]: 0x{:08x} {:11d} bytes   ^ 0x{:08x}   < {} >   {}\n", type, p->m_nMemId & 0xFF, p, p->NumBytes, p->m_PrevBlock, szMem, info);
}
