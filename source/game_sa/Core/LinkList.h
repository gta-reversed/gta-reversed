/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Link.h"

template <typename T> class CLinkList {
public:
    CLink<T>  usedListHead{};
    CLink<T>  usedListTail{};
    CLink<T>  freeListHead{};
    CLink<T>  freeListTail{};
    CLink<T>* links{};

    void* operator new(unsigned size) {
        return ((void*(__cdecl*)(uint32))0x821195)(size);
    }

    void operator delete(void* object) {
        ((void(__cdecl*)(void*))0x8213AE)(object);
    }

    void Init(int32 count) {
        usedListHead.next = &usedListTail;
        usedListTail.prev = &usedListHead;
        freeListHead.next = &freeListTail;
        freeListTail.prev = &freeListHead;

        links = new CLink<T>[count];
        for (int32 i = count - 1; i >= 0; i--) {
            links[i].Insert(&freeListHead);
        }
    }

    void Shutdown() {
        delete[] std::exchange(links, nullptr);
    }

    /*!
     * @brief Insert `link` at head
     * @param link The link to insert
    */
    void Insert(CLink<T>& link) {
        link.Remove();
        link.Insert(&usedListHead);
    }

    CLink<T>* Insert(T const& data) {
        CLink<T>* link = freeListHead.next;
        if (link == &freeListTail)
            return nullptr;
        link->data = data;
        Insert(*link);
        return link;
    }

    CLink<T>* InsertSorted(T const& data) {
        CLink<T>* i = nullptr;
        for (i = usedListHead.next; i != &usedListTail; i = i->next) {
            if (i->data.m_distance >= data.m_distance)
                break;
        }
        CLink<T>* link = freeListHead.next;
        if (link == &freeListTail)
            return nullptr;
        link->data = data;
        link->Remove();
        link->Insert(i->prev);
        //i->prev->Insert(link);
        return link;
    }

    void Clear() {
        for (CLink<T>* link = usedListHead.next; link != &usedListTail; link = usedListHead.next) {
            Remove(link);
        }
    }

    auto Remove(CLink<T>* l) {
        l->Remove();
        l->Insert(&freeListHead);
        return l;
    }

    auto GetTail() { return usedListTail.prev; }
    auto& GetTailLink() { return usedListTail; }

    auto GetHead() { return usedListHead.next; }
    auto& GetHeadLink() { return usedListHead; }


    //void SetHead(CLink<T>* l) {
    //    auto& h = usedListHead;
    //
    //    l->next = h.next;
    //    h.next->prev = l;
    //
    //    l->prev = reinterpret_cast<CLink<T>*>(this);
    //    h.next = l;
    //}
};

VALIDATE_SIZE(CLinkList<void*>, 0x34);
