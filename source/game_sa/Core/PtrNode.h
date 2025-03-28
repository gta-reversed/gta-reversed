/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrList.h"

template<typename LinkType>
class CPtrNode {
    friend CPtrList<CPtrNode<LinkType>>;
public:
    CPtrNode(void* item) : m_item(item) {}

    template<typename T>
    auto GetItem() const { return static_cast<T*>(m_item); }
    
    auto GetNext() const noexcept { return m_next; }

    void*     m_item;
    LinkType* m_next;
};
VALIDATE_SIZE(CPtrNode<void>, 8);
