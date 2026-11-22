#pragma once
#include "extensions/FixedVector.hpp"

using CompressedVector = FixedVector<int16, 128.0f>;
using CompressedLargeVector = FixedVector<int16, 8.0f>;
using CompressedUnitVector = FixedVector<int16, 4096.0f>;
using CompressedFxVector = FixedVector<int16, 32767.0f>;

using CompressedUnitFloat = FixedFloat<int16, 4096.0f>;

// Use CompressedVector as a type instead.
[[deprecated]] auto CompressVector(const CVector& v) {
    return CompressedVector{ v };
}
// Use CompressedVector as a type instead.
[[deprecated]] auto UncompressVector(const CompressedVector& v) {
    return CVector{ v };
}

// Use CompressedLargeVector as a type instead.
[[deprecated]] auto CompressLargeVector(const CVector& v) {
    return CompressedLargeVector{ v };
}
// Use CompressedLargeVector as a type instead.
[[deprecated]] auto UncompressLargeVector(const CompressedLargeVector& v) {
    return CVector{ v };
}

// Use CompressedUnitVector as a type instead.
[[deprecated]] auto CompressUnitVector(const CVector& v) {
    return CompressedUnitVector{ v };
}
// Use CompressedUnitVector as a type instead.
[[deprecated]] auto UncompressUnitVector(const CompressedUnitVector& v) {
    return CVector{ v };
}

// Use CompressedFxVector as a type instead.
[[deprecated]] auto CompressFxVector(const CVector& v) {
    return CompressedFxVector{ v };
}
// Use CompressedFxVector as a type instead.
[[deprecated]] auto UncompressFxVector(const CompressedFxVector& v) {
    return CVector{ v };
}
