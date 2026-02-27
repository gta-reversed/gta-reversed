#pragma once
#include "extensions/FixedVector.hpp"

// Use this type inplace of CompressVector/UncompressVector
using CompressedVector = FixedVector<int16, 128.0f>;

// Use this type inplace of CompressLargeVector/UncompressLargeVector
using CompressedLargeVector = FixedVector<int16, 8.0f>;

// Use this type inplace of CompressLargeVector/UncompressLargeVector
using CompressedUnitVector = FixedVector<int16, 4096.0f>;

// Use this type inplace of CompressUnitFloat/UncompressUnitFloat
using CompressedUnitFloat = FixedFloat<int16, 4096.0f>;

// Use this type inplace of CompressFxVector/UncompressFxVector
using CompressedFxVector = FixedVector<int16, 32767.0f>;
