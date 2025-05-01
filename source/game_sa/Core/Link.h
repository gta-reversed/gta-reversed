/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

template <typename T>
class CLink {
public:
    T         data;
    CLink<T>* prev;
    CLink<T>* next;

    void Remove() {
        next->prev = prev;
        prev->next = next;
    }

    /*!
     * @brief Insert `this` into a list.
     * @brief If `this` is already in another list, `Remove()` must first be called! (Not doing so will result in the list (`this` is in) getting corrupted)
     * @param after The link to insert `this` after.
     */
    void Insert(CLink<T>* after) {
        assert(after);

        next = after->next;
        next->prev = this;

        prev = after;
        prev->next = this;
    }
};
VALIDATE_SIZE(CLink<void*>, 0xC);
