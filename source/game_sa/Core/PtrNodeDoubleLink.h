/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <PtrNode.h>

class CPtrNodeDoubleLink : public CPtrNode<CPtrNodeDoubleLink> {
public:
    //static void InjectHooks();

    static void* operator new(unsigned size);
    static void operator delete(void* ptr, size_t sz);

public:
    using CPtrNode::CPtrNode;

    //void AddToList(CPtrListDoubleLink* list) {
    //    m_prev = nullptr;
    //    m_next = list->GetNode();
    //
    //    if (GetNext()) {
    //        GetNext()->m_prev = this;
    //    }
    //
    //    list->m_node = this;
    //}

public:
    CPtrNodeDoubleLink* m_prev;

};
VALIDATE_SIZE(CPtrNodeDoubleLink, 0xC);
