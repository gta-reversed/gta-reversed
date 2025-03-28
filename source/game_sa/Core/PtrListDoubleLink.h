/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrList.h"
#include "PtrNodeDoubleLink.h"

class CPtrListDoubleLink : public CPtrList<CPtrNodeDoubleLink> {
public:
    static void InjectHooks();

    CPtrListDoubleLink() = default;
    ~CPtrListDoubleLink() { Flush(); }

    /*!
    * @brief Remove all nodes of this list
    * @warning Invalidates all nodes of this list
    */
    void Flush() {
        while (NodeType* node = GetNode()) {
            DeleteNode(node);
        }
    }

    /*!
    * @brief Add item to the head (front) of the list
    **/
    NodeType* AddItem(void* item) {
        auto node = new NodeType{ item };
        AddNode(node);
        return node;
    }

    /*!
    * @brief Delete an item from the list.
    * @warning Invalidates `item`'s node (iterator), please make sure you pre-fetch `next` before deleting!
    **/
    void DeleteItem(void* item) {
        if (!m_node) {
            return;
        }

        auto* node = GetNode();
        while (node->m_item != item) {
            node = reinterpret_cast<NodeType*>(node->m_next);
            if (!node) {
                return;
            }
        }
        DeleteNode(node);
    }

    /*!
    * @brief Add a node to the head of the list
    */
    void AddNode(NodeType* node) {
        if (m_node) {
            m_node->m_prev = node;
        }
        node->m_prev = nullptr;
        node->m_next = std::exchange(m_node, node);
    }

    /*!
    * @brief Delete the specified node from the list
    * @warning Invalidates `node`, make sure to pre-fetch `next` from it beforehands!
    */
    void DeleteNode(NodeType* node) {
        if (GetNode() == node) { // Head node?
            assert(!node->m_prev);
            m_node = m_node->m_next;
        } else {
            assert(node->m_prev);
            node->m_prev->m_next = node->m_next;
        }

        if (node->m_next) {
            node->m_next->m_prev = node->m_prev;
        }

        delete node;    
    }
};

VALIDATE_SIZE(CPtrListDoubleLink, 0x4);
