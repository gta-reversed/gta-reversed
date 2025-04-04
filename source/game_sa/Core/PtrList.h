/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

namespace details {
/*!
 * @brief A safe iterator that pre-caches the next node
 */
template<typename ItemType, typename NodeType>
struct PtrListIterator {
public:
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag; // Technically we could make this bidirectional for double-linked lists, but who cares (for now anyways)
    using value_type        = ItemType;
    using pointer           = ItemType*;
    using reference         = ItemType&;

public:
    PtrListIterator(NodeType* node) :
        m_curr{ node },
        m_next{ node ? node->Next : nullptr }
    {}

    reference operator*() { return m_curr->Item; }
    pointer   operator->() { return &m_curr->Item; }

    PtrListIterator& operator++() {
        if (m_curr = m_next) {
            m_next = m_curr->Next;
        }
        return *this;
    }

    PtrListIterator operator++(int) {
        PtrListIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator==(const PtrListIterator& a, const PtrListIterator& b) { return a.m_curr == b.m_curr; }
    friend bool operator!=(const PtrListIterator& a, const PtrListIterator& b) = default;

private:
    NodeType *m_curr{}, *m_next{};
};
}; // namespace details

template<typename TTraits>
class CPtrList {
public:
    using Traits   = TTraits;
    using NodeType = typename Traits::NodeType;
    using ItemType = typename NodeType::ItemType;

    using iterator       = details::PtrListIterator<ItemType, NodeType>;
    using const_iterator = details::PtrListIterator<const ItemType, NodeType>;

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
    NodeType* AddItem(ItemType item) {
        return AddNode(new NodeType{item});
    }

    /*!
    * @brief Delete an item from the list.
    * @warning Invalidates `item`'s node (iterator), please make sure you pre-fetch `next` before deleting!
    **/
    NodeType* DeleteItem(ItemType item) {
        assert(item);
        for (NodeType *curr = m_node, *prev{}; curr; prev = std::exchange(curr, curr->Next)) {
            if (curr->Item == item) {
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
    * @brief Remove the specified node from the list
    * @warning Invalidates (deletes) the node!
    * @return Node following the removed node (that is `node->next`)
    */
    NodeType* DeleteNode(NodeType* node, NodeType* prev) {
        auto* const next = UnlinkNode(node, prev);
        delete node;
        return next;
    }

    /*!
    * @return If the specified item is in the list
    */
    bool IsMemberOfList(ItemType item) const {
        for (NodeType* node = m_node; node; node = node->Next) {
            if (node->Item == item) {
                return true;
            }
        }
        return false;
    }

    /*!
     * @warning This function is slow, because it's complexity is O(n)
     * @return The number of nodes/items in the list
     */
    uint32 GetSize() const {
        uint32 counter;
        for (NodeType* node = m_node; node; node = node->Next) {
            ++counter;
        }
        return counter;
    }

    /*!
     * @return If the list is empty
     */
    bool IsEmpty() const { return !m_node; }

    /*!
     * @return The head node of the list
     */
    NodeType* GetNode() const { return m_node; }

    auto begin() { return iterator{ m_node }; }
    auto end() { return iterator{ nullptr }; }

    auto cbegin() const { return const_iterator{ m_node }; }
    auto cend() const { return const_iterator{ nullptr }; }

public:
    NodeType* m_node{};
};
