/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "OctTree.h"

class NOTSA_EXPORT_VTABLE COctTreeBase : public COctTree {
public:
    uint32 m_nNumBranches;
    bool32 m_bHasTransparentPixels;

public:
    COctTreeBase() = default;  // 0x5A7570
    ~COctTreeBase() = default; // 0x856690

    bool InsertTree(uint8 red, uint8 green, uint8 blue) override;
    void FillPalette(uint8* colors) override;

    void Init(int32 numBranches);
    bool Insert(uint8 red, uint8 green, uint8 blue);
    void ReduceBranches(int32 newBranchesCount);
    void Reduce();
};

extern COctTreeBase& gOctTreeBase;

VALIDATE_SIZE(COctTreeBase, 0x30);
