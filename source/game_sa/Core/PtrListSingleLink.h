/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrList.h"
#include "PtrNodeSingleLink.h"

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

    ~CPtrListSingleLink() { Flush(); }

    /*!
    * @brief Remove all nodes of this list
    * @warning Invalidates all nodes of this list
    */
    void Flush() {
        while (NodeType* node = GetNode()) { // Keep popping from the head until empty
            DeleteNode(node, GetNode());
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
    void DeleteItem(void* item) {
        if (!m_node) {
            return;
        }

        NodeType* prev = nullptr;
        auto*     curr = GetNode();
        while (curr->m_item != item) {
            prev = std::exchange(curr, curr->m_next);
            if (!curr) {
                return;
            }
        }
        DeleteNode(curr, prev);
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
    * @warning Invalidates `node`, make sure to pre-fetch `next` from it beforehands!
    */
    void DeleteNode(NodeType* node, NodeType* prev) {
        if (GetNode() == node) { // Node is the head?
            assert(!prev);
            m_node = node->m_next;
        } else if (prev) { // Node not the tail?
            prev->m_next = node->m_next;
        }
        delete node;
    }
};
VALIDATE_SIZE(CPtrListSingleLink, 4);
