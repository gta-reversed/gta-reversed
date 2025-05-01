/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <Base.h>

template <typename T>
struct SArray /* : public std::vector<T> */ {
    /* allocator<_Ty>*/ void* allocator{}; // MSVC 6.0 STL vector
    /* iterator      */ T*    _First{};    // ptr to array (=> `begin()`)
    /* iterator      */ T*    _Last{};     // ptr to past-the-end of all active elements (=> `end()`)
    /* iterator      */ T*    _End{};      // ptr to past-the-end of array (=> `begin() + capacity()`)
public:
    using value_type = T;

    SArray() noexcept = default;
    SArray(const SArray& o) :
        SArray{}
    {
        if (o.empty() || &o == this) {
            return;
        }
        _First = new T[o.capacity()];
        rng::copy(o, _First);
        _Last = _First + o.size();
        _End  = _First + o.capacity();
    }
    SArray(SArray&& other) noexcept :
        SArray()
    {
        swap(*this, other);
    }

    ~SArray() {
        clear();
    }

    SArray& operator=(SArray o) {
        swap(*this, o);
        return *this;
    }

    friend void swap(SArray<T>& a, SArray<T>& b) {
        using std::swap;

        swap(a._First, b._First);
        swap(a._Last, b._Last);
        swap(a._End, b._End);
    }

    auto size() const { return _First ? (size_t)(_Last - _First) : 0u; }
    auto capacity() const { return _First ? (size_t)(_End - _First) : 0u; }
    auto begin() const { return _First; }
    auto end() const { return _Last; }
    auto empty() const { return size() == 0; }
    auto front() const { return *_First; }
    auto back() const { return *(_Last - 1); }
    void clear() { // in this implementation for the sake of simplicity `clear` actually deallocates all memory and destructs all items
        delete[] std::exchange(_First, nullptr);
        _Last = nullptr;
        _End  = nullptr;
    }
    void reserve(size_t newCap) {
        const auto oldCap = capacity();
        if (newCap <= oldCap) {
            return;
        }
        const auto currSize = size();

        // Alloc new storage
        T* const dst = new T[newCap];
        assert(dst);
        
        // Move items over (If any)
        rng::move(*this, dst);

        // De-allocate old storage
        delete[] std::exchange(_First, dst);

        // Re-calculate pointers
        _First = dst;
        _Last  = dst + currSize;
        _End   = dst + newCap;
    }
    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (_Last == _End) {
            reserve(size() * 150 / 100); // Geometric growth with K=1.5
        }
        std::construct_at(_Last++, std::forward<Args>(args)...);
    }
    void erase(T* iter) {
        assert(iter >= _First && iter < _Last);
        std::destroy_at(iter);
        rng::shift_left(iter + 1, _Last, 1);
        _Last--;
    }
};
VALIDATE_SIZE(SArray<int32>, 0x10);
