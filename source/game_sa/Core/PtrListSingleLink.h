/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrNodeSingleLink.h"
#include "PtrList.h"
#include <Pools/Pools.h>

namespace notsa {
namespace details {
template<typename ItemType>
struct PtrListSingleLinkTraits {
    using NodeType = CPtrNodeSingleLink<ItemType>;

    static NodeType* AddNode(NodeType*& head, NodeType* node) {
        assert(node);
        assert(!head || IsNodeValid(*head));
        assert(IsNodeValid(*node));

        node->Next = std::exchange(head, node);
        return node;
    }

    static NodeType* UnlinkNode(NodeType*& head, NodeType* node, NodeType* prev) {
        assert(node);
        assert(head && "Can't remove node from empty list");
        assert(!prev || IsNodeValid(*prev));
        assert(IsNodeValid(*head));
        assert(IsNodeValid(*node));

        NodeType* next = node->Next;
        if (head == node) { // Node is the head?
            assert(!prev && "Head node must have no `prev`"); 
            head = next;
        } else {
            assert(prev && "All nodes other than the `head` must have a valid `prev`!");
            assert(prev->Next == node && "`prev->next` must point to `node`");
            prev->Next = next;
        }
        return next;
    }

    static bool IsNodeValid(const NodeType& node) requires std::is_pointer_v<ItemType> {
        if (!GetPtrNodeSingleLinkPool()->IsObjectValid(reinterpret_cast<const CPtrNodeSingleLink<void*>*>(&node))) {
            return false;
        }
        if (!node.IsItemValid()) {
            return false;
        }
        return true;
    }
};
}; // namespace details
}; // namespace notsa

/*!
* @brief A list of single-linked nodes (forward list)
*/
template<typename ItemType>
class CPtrListSingleLink : public CPtrList<notsa::details::PtrListSingleLinkTraits<ItemType>> {
public:
    using CPtrList<notsa::details::PtrListSingleLinkTraits<ItemType>>::CPtrList;
};
