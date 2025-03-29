/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrList.h"
#include "PtrNodeSingleLink.h"

/*!
* @brief A list of single-linked nodes (forward list)
*/
class CPtrListSingleLink : public CPtrList<CPtrNodeSingleLink> {
public:
    //static void InjectHooks() {
    //    RH_ScopedClass(CPtrListSingleLink);
    //    RH_ScopedCategory("Core");
    //
    //    RH_ScopedInstall(Flush, 0x552400);
    //    RH_ScopedInstall(AddItem, 0x5335E0);
    //    RH_ScopedInstall(DeleteItem, 0x533610);
    //}

    CPtrListSingleLink() = default;
    ~CPtrListSingleLink() { Flush(); }

    /*!
    * @brief Remove all nodes of this list
    * @warning Invalidates all nodes of this list
    */
    void Flush() {
        // Keep popping from the head until empty
        while (NodeType* node = GetNode()) {
            DeleteNode(node, nullptr);
        }
    }

    /*!
    * @brief Add item to the head (front) of the list
    **/
    NodeType* AddItem(void* item) {
        return AddNode(new NodeType{item});
    }

    /*!
    * @brief Delete an item from the list.
    * @warning Invalidates `item`'s node (iterator), please make sure you pre-fetch `next` before deleting!
    **/
    NodeType* DeleteItem(void* item) {
        assert(item);
        for (NodeType *curr = m_node, *prev{}; curr; prev = std::exchange(curr, curr->m_next)) {
            if (curr->m_item == item) {
                return DeleteNode(curr, prev);
            }
        }
        return nullptr;
    }

    /*!
    * @brief Add a node to the head of the list
    */
    NodeType* AddNode(NodeType* node) {
        node->m_next = std::exchange(m_node, node);
        return node;
    }

    /*!
    * @brief Delete the specified node from the list
    * @warning Invalidates `node`, make sure to pre-fetch `next` from it beforehand!
    * @return Node following the removed node (that is `node->next`)
    */
    NodeType* DeleteNode(NodeType* node, NodeType* prev) {
        assert(node);
        assert(GetNode() && "Can't remove node from empty list");

        NodeType* next = node->m_next;
        if (GetNode() == node) { // Node is the head?
            assert(!prev && "Head node must have no `prev`"); 
            m_node = next;
        } else {
            assert(prev && "All nodes other than the `head` must have a valid `prev`!");
            assert(prev->m_next == node && "`prev->next` must point to `node`");
            prev->m_next = next;
        }
        delete node;
        return next;
    }
};
VALIDATE_SIZE(CPtrListSingleLink, 4);
