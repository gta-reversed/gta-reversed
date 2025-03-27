#pragma once
#include <memory>
#include <vector>
#include <cassert>

// Dynamic array that frees all pointers on destruction.
//
// TODO: Make sure all instances of this to use new/delete
// allocators and remove the `UseCFree` flag.
template<typename T, bool UseCFree = false>
class HeapPtrArray : public std::vector<T*> {
    HeapPtrArray() = default;
    ~HeapPtrArray() {
        for (auto& ptr : *this) {
            if constexpr (!UseCFree) {
                delete ptr;
            } else {
                free(ptr);
            }
        }
    }

    constexpr void push_back(const T*& value) {
        assert(_CrtIsValidHeapPointer(value));
        std::vector<T*>::push_back(value);
    }

    constexpr void push_back(T*&& value) {
        assert(_CrtIsValidHeapPointer(value));
        std::vector<T*>::push_back(value);
    }
};
