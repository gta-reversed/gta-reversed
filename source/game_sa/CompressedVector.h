/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CompressedVector {
public:
    int16 x;
    int16 y;
    int16 z;
};

VALIDATE_SIZE(CompressedVector, 0x6);

CVector UncompressVector(const CompressedVector& compressedVec);
CompressedVector CompressVector(const CVector& vec);
float UncompressUnitFloat(int16 val);
CVector UncompressUnitVector(const CompressedVector& compressedVec);
CompressedVector CompressUnitVector(CVector const& vec);
CVector UncompressLargeVector(const CompressedVector& compressedVec);
CompressedVector CompressLargeVector(CVector const& vec);
