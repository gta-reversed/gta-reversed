/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <PtrNode.h>

class CPtrNodeSingleLink : public CPtrNode<CPtrNodeSingleLink> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CPtrNodeSingleLink);
        RH_ScopedCategory("Core");

        RH_ScopedInstall(AddToList, 0x532960);
    }

    static void* operator new(unsigned size) {
        return GetPtrNodeSingleLinkPool()->New();
    }

    static void operator delete(void* data) {
        GetPtrNodeSingleLinkPool()->Delete(static_cast<CPtrNodeSingleLink*>(data));
    }

public:
    using CPtrNode::CPtrNode;

    //void AddToList(CPtrListSingleLink* list) {
    //    m_next       = list->GetNode();
    //    list->m_node = static_cast<CPtrNode*>(this);
    //}
};
VALIDATE_SIZE(CPtrNodeSingleLink, 8);
