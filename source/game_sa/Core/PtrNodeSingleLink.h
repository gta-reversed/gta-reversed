/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <PtrNode.h>

// Doing it like this because including `Pools.h` here is a suicide
namespace details {
void* CPtrNodeSingleLink__operator_new(size_t sz);
void  CPtrNodeSingleLink__operator_delete(void* data, size_t sz);
};

template<typename TItemType>
class CPtrNodeSingleLink : public CPtrNode<TItemType, CPtrNodeSingleLink<TItemType>> {
public:
    using ItemType = TItemType;

public:
    static void* operator new(size_t sz) {
        return details::CPtrNodeSingleLink__operator_new(sz);
    }

    static void operator delete(void* data, size_t sz) {
        return details::CPtrNodeSingleLink__operator_delete(data, sz);
    }

public:
    using CPtrNode<TItemType, CPtrNodeSingleLink<TItemType>>::CPtrNode;

    //void AddToList(CPtrListSingleLink* list) {
    //    m_next       = list->GetNode();
    //    list->m_node = static_cast<CPtrNode*>(this);
    //}
};
VALIDATE_SIZE(CPtrNodeSingleLink<void*>, 0x8);
