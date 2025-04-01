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
    CPtrNode(TItemType item) :
        Item(item)
    { assert(item); }

    TItemType Item{};
    LinkType* Next{};
};
