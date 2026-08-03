/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrNode.h"
#include "PtrNodeDoubleLink.h"
#include "PtrList.h"

namespace notsa {
namespace details {
template<typename ItemType>
struct PtrListDoubleLinkTraits {
    using NodeType = CPtrNodeDoubleLink<ItemType>;

    static NodeType* AddNode(NodeType*& head, NodeType* node) {
        assert(node);
        assert(!head || IsNodeValid(*head));
        assert(IsNodeValid(*node));

        NodeType* next = node->Next = std::exchange(head, node);
        if (next) {
            assert(next->Prev == nullptr && "Head node must have no `prev`");
            next->Prev = node;
        }
        node->Prev = nullptr;
        return node;
    }

    static NodeType* UnlinkNode(NodeType*& head, NodeType* node, NodeType* prev) {
        assert(node);
        assert(node->Prev == prev && "Incorrect `prev` value");
        assert(head && "Can't remove node from empty list");
        assert(!prev || IsNodeValid(*prev));
        assert(IsNodeValid(*head));
        assert(IsNodeValid(*node));

        NodeType* next = node->Next;

        if (next) {
            assert(next->Prev == node);
            next->Prev = prev;
        }

        if (head == node) {
            assert(!prev && "Head node must have no `prev`");
            head = next;
        } else {
            assert(prev && "All nodes other than the `head` must have a valid `prev`!");
            assert(prev->Next == node);
            prev->Next = next;
        }

        return next;
    }

    static bool IsNodeValid(const NodeType& node) requires std::is_pointer_v<ItemType> {
        if (!GetPtrNodeDoubleLinkPool()->IsObjectValid(reinterpret_cast<const CPtrNodeDoubleLink<void*>*>(&node))) {
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
* @brief A list of double-linked nodes
*/
template<typename ItemType>
class CPtrListDoubleLink : public CPtrList<notsa::details::PtrListDoubleLinkTraits<ItemType>> {
    using Base = CPtrList<notsa::details::PtrListDoubleLinkTraits<ItemType>>;

public:
    using NodeType = typename Base::NodeType;

public:
    using Base::CPtrList;

    /*!
    * @brief Delete the specified node from the list
    * @warning Invalidates `node`, make sure to pre-fetch `next` from it beforehand!
    * @return Node following the removed node (that is `node->next`)
    */
    NodeType* DeleteNode(NodeType* node) {
        return Base::DeleteNode(node, node->Prev);
    }

    /*!
    * @brief Remove the specified node from the list
    * @note Doesn't invalidate (delete) the node, only removes it from the list
    * @return Node following the removed node (that is `node->next`)
    */
    NodeType* UnlinkNode(NodeType* node) {
        return Base::Traits::UnlinkNode(this->m_Head, node, node->Prev);
    }
};
