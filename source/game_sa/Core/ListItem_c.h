/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "List_c.h"

/**
 * Double linked list item base class
 *
 * You should inherit this class to use it in List_c lists!
 */
template<typename T> // T should be derived from `ListItem_c`
class ListItem_c {
public:
    ListItem_c() = default;  // 0x4A8DB0
    ~ListItem_c() = default; // 0x49EA70

public: // 
    T* m_pPrev{};
    T* m_pNext{};

protected:
    friend class TList_c<T>;
};
