/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrList.h"
#include "PtrNodeDoubleLink.h"

/*!
* @brief A list of double-linked nodes
*/
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
    * @return The `node` of the added item
    **/
    NodeType* AddItem(void* item) {
        return AddNode(new NodeType{ item });
    }

    /*!
    * @brief Delete an item from the list.
    * @warning Invalidates `item`'s node (iterator), please make sure you pre-fetch `next` before deleting!
    * @return Node following the removed node (that is `node->next`)
    **/
    NodeType* DeleteItem(void* item) {
        assert(item);

        for (NodeType* node = m_node; node; node = node->m_next) {
            if (node->m_item == item) {
                return DeleteNode(node);
            }
        }
        return nullptr;
    }

    /*!
    * @brief Add a node to the head of the list
    * @return The added node
    */
    NodeType* AddNode(NodeType* node) {
        assert(node);

        NodeType* next = node->m_next = std::exchange(m_node, node);
        if (next) {
            assert(next->m_prev == nullptr && "Head node must have no `prev`");
            next->m_prev = node;
        }
        node->m_prev = nullptr;
        return node;
    }

    /*!
    * @brief Delete the specified node from the list
    * @warning Invalidates `node`, make sure to pre-fetch `next` from it beforehand!
    * @return Node following the removed node (that is `node->next`)
    */
    NodeType* DeleteNode(NodeType* node) {
        assert(node);
        assert(m_node && "Can't remove node from empty list");

        NodeType *next = node->m_next,
                 *prev = node->m_prev;

        if (next) {
            assert(next->m_prev == node);
            next->m_prev = prev;
        }

        if (m_node == node) { // Node is the head?
            assert(!prev && "Head node must have no `prev`");
            m_node = next;
        } else {
            assert(prev && "All nodes other than the `head` must have a valid `prev`!");
            assert(prev->m_next == node);
            prev->m_next = next;
        }

        delete node;

        return next;
    }
};

VALIDATE_SIZE(CPtrListDoubleLink, 0x4);
