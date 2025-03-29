/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

template<typename TTraits>
class CPtrList {
public:
    using Traits   = TTraits;
    using NodeType = typename Traits::NodeType;

public:
    CPtrList() = default;
    ~CPtrList() { Flush(); }

    /*!
    * @brief Remove all nodes of this list
    * @warning `delete`s all nodes of this list
    */
    void Flush() {
        // Keep popping from the head until empty
        while (m_node) {
            DeleteNode(m_node, nullptr);
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
    * @return The added node
    */
    NodeType* AddNode(NodeType* node) {
        return Traits::AddNode(m_node, node);
    }

    /*!
    * @brief Remove the specified node from the list
    * @note Doesn't invalidate (delete) the node, only removes it from the list
    * @return Node following the removed node (that is `node->next`)
    */
    NodeType* UnlinkNode(NodeType* node, NodeType* prev) {
        return Traits::UnlinkNode(m_node, node, prev);
    }

    /*!
    * @copydoc Traits::DeleteNode
    */
    NodeType* DeleteNode(NodeType* node, NodeType* prev) {
        auto* const next = UnlinkNode(node, prev);
        delete node;
        return next;
    }

    bool IsMemberOfList(void* data) const {
        for (NodeType* node = GetNode(); node; node = node->m_next) {
            if (node->m_item == data) {
                return true;
            }
        }
        return false;
    }

    uint32 CountElements() const {
        uint32 counter;
        for (NodeType* node = GetNode(); node; node = node->m_next) {
            ++counter;
        }
        return counter;
    }

    bool IsEmpty() const { return !m_node; }
    NodeType* GetNode() const { return m_node; }

public:
    NodeType* m_node{};
};
