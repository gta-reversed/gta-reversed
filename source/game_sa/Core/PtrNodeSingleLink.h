/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <PtrNode.h>

template<typename TItemType>
class CPtrNodeSingleLink : public CPtrNode<TItemType, CPtrNodeSingleLink<TItemType>> {
public:
    using ItemType = TItemType;

public:
    static void* operator new(unsigned size);
    static void operator delete(void* data);

public:
    using CPtrNode<TItemType, CPtrNodeSingleLink<TItemType>>::CPtrNode;

    //void AddToList(CPtrListSingleLink* list) {
    //    m_next       = list->GetNode();
    //    list->m_node = static_cast<CPtrNode*>(this);
    //}
};
VALIDATE_SIZE(CPtrNodeSingleLink<void*>, 0x8);
