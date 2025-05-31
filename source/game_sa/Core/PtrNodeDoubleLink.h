/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrNode.h"

// Doing it like this because including `Pools.h` here is a suicide
namespace details {
void* CPtrNodeDoubleLink__operator_new(size_t sz);
void  CPtrNodeDoubleLink__operator_delete(void* data, size_t sz);
};

template<typename TItemType>
class CPtrNodeDoubleLink : public CPtrNode<TItemType, CPtrNodeDoubleLink<TItemType>> {
public:
    using ItemType = TItemType;

public:
    static void* operator new(size_t sz) {
        return details::CPtrNodeDoubleLink__operator_new(sz);
    }

    static void operator delete(void* data, size_t sz) {
        return details::CPtrNodeDoubleLink__operator_delete(data, sz);
    }

public:
    using CPtrNode<ItemType, CPtrNodeDoubleLink<ItemType>>::CPtrNode;

public:
    CPtrNodeDoubleLink<ItemType>* Prev{};
};
VALIDATE_SIZE(CPtrNodeDoubleLink<void*>, 0xC);
