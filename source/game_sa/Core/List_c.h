/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

template <typename T>
class ListItem_c;

//! Simple (and shit) linked list
//! When Insert-ing an item, you have to make sure not to insert the same item twice. (Error checks are not possible due to game code and performance)
template <typename T>
class TList_c {
    template<typename Y>
    class BaseIterator {
    public:
        using iterator_category = std::forward_iterator_tag; // Actually it's bidirectional, but there are quirks, so let's pretend like its not
        using difference_type   = std::ptrdiff_t;
        using value_type        = Y;
        using pointer           = Y*;
        using reference         = Y&;

        BaseIterator() = default;
        BaseIterator(pointer ptr) : m_ptr{ ptr } {}

        reference operator*() const { return *m_ptr; }
        pointer operator->() { return m_ptr; }

        auto& operator++() { assert(m_ptr); m_ptr = m_ptr->m_pNext; return *this; }
        auto  operator++(int) { const auto tmp{ *this }; ++(*this); return tmp; }

        // NOTE: Won't work properly in case `list.end() == *this` [Because `m_ptr` will be null]
        auto& operator--() { assert(m_ptr); m_ptr = m_ptr->m_pPrev; return *this; }
        auto  operator--(int) { const auto tmp{ *this }; --(*this); return tmp; }

        friend bool operator==(const BaseIterator<Y>& lhs, const BaseIterator<Y>& rhs) { return lhs.m_ptr == rhs.m_ptr; }
        friend bool operator!=(const BaseIterator<Y>& lhs, const BaseIterator<Y>& rhs) { return !(lhs == rhs); }
    private:
        pointer m_ptr;
    };
public:
    using iterator       = BaseIterator<T>;
    using const_iterator = BaseIterator<const T>;

    using value_type     = T;
    using pointer        = T*;
    using reference      = T&;

public:
    //static void Test() {
    //    struct Item : ListItem_c<Item> {
    //        int i;
    //    };
    //    using List = TList_c<Item>;
    //
    //    Item items[10];
    //
    //    List l{};
    //    l.AddItem(&items[0]);
    //    assert(l.m_cnt == 0);
    //    assert(l.m_tail == &items[0]);
    //    assert(l.m_head == &items[0]);
    //    assert(items[0].m_pPrev == nullptr);
    //    assert(items[0].m_pNext == nullptr);
    //}

    TList_c() = default;
    TList_c(const TList_c&) = delete;
    TList_c(TList_c&& other) {
        *this = std::move(other);
    }

    TList_c& operator=(const TList_c&) = delete;
    TList_c& operator=(TList_c&& other) {
        if (this != &other) {
            m_head = std::exchange(other.m_head, nullptr);
            m_tail = std::exchange(other.m_tail, nullptr);
            m_cnt  = std::exchange(other.m_cnt, 0);
        }
        return *this;
    }

    //! Add item to the beginning (head) of the list
    void AddItem(T* item) { // TODO: This really should take `T&`
        assert(item);
        assert(item != m_tail); // Double insertion
        assert(item != m_head); // Double insertion

        const auto oldHead = std::exchange(m_head, item);
        item->m_pPrev      = nullptr;
        item->m_pNext      = oldHead;

        if (oldHead) {
            oldHead->m_pPrev = item;
        } else {
            assert(m_cnt == 0); // If there was no head, the list must've been empty
            assert(!m_tail);     // And obviously the there mustn't be a tail either
            m_tail = item;
        }

        ++m_cnt;
    }

    //! For `std` compatibility
    void push_back(T& item) {
        AppendItem(&item);
    }

    //! @brief Add item to the end (tail) of the list
    T* AppendItem(T* item) {
        assert(item);
        assert(item != m_tail); // Double insertion
        assert(item != m_head); // Double insertion

        const auto oldTail = std::exchange(m_tail, item);
        item->m_pPrev      = oldTail;
        item->m_pNext      = nullptr;

        if (oldTail) {
            oldTail->m_pNext = item;
        } else {
            assert(m_cnt == 0); // If there was no tail, the list must've been empty
            assert(!m_head);    // And obviously the there mustn't be a head either
            m_head = item;
        }

        ++m_cnt;
    
        return item;
    }

    //! @brief Insert `item` after `after`
    auto InsertAfterItem(T* item, iterator after) {
        assert(m_tail && m_head && m_cnt > 0 && "`after` can't be part of an empty list");

        m_cnt++;

        item->m_pPrev = after.m_ptr;

        auto* const next = item->m_pNext = after.m_ptr->m_pNext;
        if (next) {
            next->m_pPrev = item;
        } else {
            m_tail = item;
        }
        
        return item;
    }

    void InsertBeforeItem(T* addedItem, T* pExistingItem) {
        ++m_cnt; // BUG: We increment count even though the item wasn't added to table, and there's no certainity that it will

        if (!m_head)
            return;

        auto curItem = GetHead();
        while (curItem && curItem != pExistingItem)
            curItem = GetNext(curItem);

        if (!curItem)
            return;

        addedItem->m_pPrev = curItem->m_pPrev;
        addedItem->m_pNext = curItem;
        auto* oldPrev = curItem->m_pPrev;
        curItem->m_pPrev = addedItem;
        if (oldPrev)
            oldPrev->m_pNext = addedItem;
        else
            m_head = addedItem;
    }
    
    T* RemoveItem(T* item) {
        assert(item);
        assert(item->m_pPrev != m_tail);
        assert(item->m_pNext != m_head);
        assert(m_cnt > 0);

        if (item->m_pNext) {
            item->m_pNext->m_pPrev = item->m_pPrev;
        } else {
            m_tail = item->m_pPrev;
        }

        if (item->m_pPrev) {
            item->m_pPrev->m_pNext = item->m_pNext;
        } else {
            m_head = item->m_pNext;
        }

        m_cnt--;

        return item;
    }

    void RemoveAll() {
        m_head = nullptr;
        m_tail = nullptr;
        m_cnt  = 0;
    }

    /*!
    * @brief  Remove the head
    * @return The old head
    */
    T* RemoveHead() {
        if (!m_head) {
            return nullptr;
        }

        assert(m_cnt > 0);
        --m_cnt;

        const auto pOldHead = m_head;
        if (m_head == m_tail) { // 1 items
            m_tail = nullptr;
            m_head = nullptr;
            return pOldHead;
        }

        if (m_head->m_pNext) {
            m_head->m_pNext->m_pPrev = nullptr;
        }

        m_head = m_head->m_pNext;

        return pOldHead;
    }

    /*!
    * @brief  Remove the tail
    * @return The old tail
    */
    T* RemoveTail() {
        if (!m_tail) {
            return nullptr;
        }

        assert(m_cnt > 0);
        --m_cnt;

        const auto oldTail = m_tail;
        m_tail->m_pPrev->m_pNext = nullptr;
        m_tail = m_tail->m_pPrev;

        return oldTail;
    }

    T* GetItemOffset(bool bFromHead, int32 iOffset) {
        if (bFromHead) {
            auto* result = GetHead();
            if (iOffset > 0 && result) {
                int32 iCounter = 0;
                while (iCounter < iOffset && result) {
                    ++iCounter;
                    result = GetNext(result);
                }
            }
            return result;
        } else {
            auto* result = GetTail();
            if (iOffset > 0 && result) {
                int32 iCounter = 0;
                while (iCounter < iOffset && result) {
                    ++iCounter;
                    result = GetPrev(result);
                }
            }
            return result;
        }
    }

    /*!
     * @brief Filter items in-place
     */
    template<std::predicate<T&> Pred>
    void Filter(Pred&& pred) {
        auto* item = GetHead();
        while (item) {
            auto* next = GetNext(item);
            if (!std::invoke(pred, *item)) {
                RemoveItem(item);
            }
            item = next;
        }
    }

    /*!
     * @brief Sort list using predicate
     */
    void Sort(std::predicate<T&, T&> auto&& pred) {
        if (m_cnt < 2) {
            return;
        }

        const auto Merge = [&](T* left, T* right) -> T* {
            T* head = nullptr;
            T* tail = nullptr;

            const auto Append = [&head, &tail](T* item) {
                item->m_pPrev = tail;
                item->m_pNext = nullptr;
                if (tail) {
                    tail->m_pNext = item;
                } else {
                    head = item;
                }
                tail = item;
            };

            while (left && right) {
                if (std::invoke(pred, *right, *left)) {
                    auto* const item = right;
                    right = right->m_pNext;
                    Append(item);
                } else {
                    auto* const item = left;
                    left = left->m_pNext;
                    Append(item);
                }
            }

            for (auto* remaining = left ? left : right; remaining;) {
                auto* const item = remaining;
                remaining = remaining->m_pNext;
                Append(item);
            }

            return head;
        };

        const auto MergeSort = [&] (this auto&& MergeSort, T* head) -> T* {
            if (!head || !head->m_pNext) {
                return head;
            }

            // Split a list in two, returning the head of the latter half.
            const auto Split = [] (T* head) -> T* {
                auto* slow = head;
                auto* fast = head->m_pNext;

                while (fast && fast->m_pNext) {
                    slow = slow->m_pNext;
                    fast = fast->m_pNext->m_pNext;
                }

                auto* const second = slow->m_pNext;
                slow->m_pNext = nullptr;
                second->m_pPrev = nullptr;
                return second;
            };

            return Merge(MergeSort(head), MergeSort(Split(head)));
        };

        // Sort list
        m_head = MergeSort(m_head);

        // Find new tail
        for (m_tail = m_head; m_tail->m_pNext;) {
            m_tail = m_tail->m_pNext;
        }
    }

    T*   GetNext(T* item) const { assert(item); return item->m_pNext; }
    T*   GetPrev(T* item) const { assert(item); return item->m_pPrev; }
    T*   GetHead()        const { return m_head; }
    T*   GetTail()        const { return m_tail; }

    auto GetNumItems()    const { return m_cnt; }
    auto GetSize()        const { return m_cnt; }

    auto cbegin()         const { return const_iterator{ GetHead() }; }
    auto begin()          const { return cbegin(); }
    auto begin()                { return iterator{ GetHead() }; }

    // Past the end is always `nullptr` - Not really std comforting (because `end() != --(++end())`), but oh well
    auto cend()           const { return const_iterator{ nullptr }; }
    auto end()            const { return cend(); }
    auto end()                  { return iterator{ nullptr }; }

    auto IsEmpty()        const { return m_head == nullptr; }

private:
    T*     m_head{};
    T*     m_tail{};
    size_t m_cnt{};
};
using List_c = TList_c<int>; // using int so the compiler doesn't complain about void&
