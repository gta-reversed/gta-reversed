/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrNodeDoubleLink.h"
#include "PtrList.h"

namespace details {
struct PtrListDoubleLinkTraits {
    using NodeType = CPtrNodeDoubleLink;

    static NodeType* AddNode(NodeType*& head, NodeType* node) {
        NodeType* next = node->m_next = std::exchange(head, node);
        if (next) {
            assert(next->m_prev == nullptr && "Head node must have no `prev`");
            next->m_prev = node;
        }
        node->m_prev = nullptr;
        return node;
    }

    static NodeType* UnlinkNode(NodeType*& head, NodeType* node, NodeType* prev) {
        assert(node);
        assert(node->m_prev == prev && "Incorrect `prev` value");
        assert(head && "Can't remove node from empty list");

        NodeType* next = node->m_next;

        if (next) {
            assert(next->m_prev == node);
            next->m_prev = prev;
        }

        if (head == node) {
            assert(!prev && "Head node must have no `prev`");
            head = next;
        } else {
            assert(prev && "All nodes other than the `head` must have a valid `prev`!");
            assert(prev->m_next == node);
            prev->m_next = next;
        }

        return next;
    }
};
};

/*!
* @brief A list of double-linked nodes
*/
class CPtrListDoubleLink : public CPtrList<details::PtrListDoubleLinkTraits> {
public:
    using CPtrList::CPtrList;

    /*!
    * @brief Delete the specified node from the list
    * @warning Invalidates `node`, make sure to pre-fetch `next` from it beforehand!
    * @return Node following the removed node (that is `node->next`)
    */
    NodeType* DeleteNode(NodeType* node) {
        return CPtrList::DeleteNode(node, node->m_prev);
    }

    /*!
    * @brief Remove the specified node from the list
    * @note Doesn't invalidate (delete) the node, only removes it from the list
    * @return Node following the removed node (that is `node->next`)
    */
    NodeType* UnlinkNode(NodeType* node) {
        return Traits::UnlinkNode(m_node, node, node->m_prev);
    }
};
