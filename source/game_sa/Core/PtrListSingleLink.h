/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrNodeSingleLink.h"
#include "PtrList.h"

namespace details {
struct PtrListSingleLinkTraits {
    using NodeType = CPtrNodeSingleLink;

    static NodeType* AddNode(NodeType*& head, NodeType* node) {
        node->m_next = std::exchange(head, node);
        return node;
    }

    static NodeType* UnlinkNode(NodeType*& head, NodeType* node, NodeType* prev) {
        assert(node);
        assert(head && "Can't remove node from empty list");

        NodeType* next = node->m_next;
        if (head == node) { // Node is the head?
            assert(!prev && "Head node must have no `prev`"); 
            head = next;
        } else {
            assert(prev && "All nodes other than the `head` must have a valid `prev`!");
            assert(prev->m_next == node && "`prev->next` must point to `node`");
            prev->m_next = next;
        }
        return next;
    }
};
};

/*!
* @brief A list of single-linked nodes (forward list)
*/
class CPtrListSingleLink : public CPtrList<details::PtrListSingleLinkTraits> {
public:
    using CPtrList::CPtrList;
};
