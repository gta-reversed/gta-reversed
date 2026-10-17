#pragma once

namespace notsa {
namespace script {
namespace detail {
template<typename T, size_t UniqueTag = 0>
struct StrongAlias {
    T value;

    auto& operator=(const T& o) {
        value = o;
        return *this;
    }

    operator T() const { return value; }
};
};

// eModelID wrapper that is read either from a static int32 value or UsedObjectArray.
using Model = detail::StrongAlias<eModelID>;

// uint32 wrapper for reading an unsigned 32-bit integer that can be out of range for int32
using Hash = detail::StrongAlias<uint32>;

}; // namespace script
}; // namespace notsa
