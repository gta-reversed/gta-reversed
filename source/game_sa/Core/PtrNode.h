/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

template<typename TItemType, typename TLinkType>
class CPtrNode {
public:
    using LinkType = TLinkType;

public:
    CPtrNode(void* item) :
        m_item(item)
    { assert(item); }

    template<typename T>
    auto GetItem() const { return static_cast<T*>(m_item); }

    template<typename T>
    auto ItemAs() const { return static_cast<T*>(m_item); }

    auto GetNext() const noexcept { return m_next; }

    TItemType m_item{};
    LinkType* m_next{};
};
