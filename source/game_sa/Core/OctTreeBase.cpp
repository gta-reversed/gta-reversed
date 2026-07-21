#include "StdInc.h"

#include "OctTreeBase.h"

auto& gOctTreeBase = StaticRef<COctTreeBase>(0xBC1290);

// 0x5A7710
bool COctTreeBase::InsertTree(uint8 red, uint8 green, uint8 blue) {
    ms_level = 0;

    if (!COctTree::InsertTree(red, green, blue))
        return false;

    m_nNumBranches--;

    return true;
}

// 0x5A7280
void COctTreeBase::FillPalette(uint8* data) {
    ms_level = 0;

    if (m_bHasTransparentPixels) {
        auto* colors = *reinterpret_cast<RwRGBA**>(data);
        colors->red   = 0;
        colors->green = 0;
        colors->blue  = 0;
        colors->alpha = 0;

        ms_level = 1;
    }

    COctTree::FillPalette(data);
}

// 0x5A7260
void COctTreeBase::Init(int32 numBranches) {
    ms_level = 0;

    m_nNumBranches = numBranches;
    m_bHasTransparentPixels = 0;

    empty();
}

// 0x5A7750
bool COctTreeBase::Insert(uint8 red, uint8 green, uint8 blue) {
    while (true) {
        ms_bFailed = false;

        if (InsertTree(red, green, blue))
            break;

        if (ms_bFailed) {
            Reduce();
            continue;
        }

        return false;
    }

    if (m_nNumBranches <= 0) {
        Reduce();
    }

    return true;
}

// 0x5A7840
void COctTreeBase::ReduceBranches(int32 newBranchesCount) {
    const int32 branchesToCount = m_bHasTransparentPixels ? newBranchesCount + 1 : newBranchesCount;

    // Signed-unsigned conversion check
    // TODO: Replace this with the overflow-check cast in notsa::script.
    assert(m_nNumBranches < static_cast<uint32>(std::numeric_limits<int32>::max()));
    while (static_cast<int32>(m_nNumBranches) < branchesToCount) {
        Reduce();
    }
}

void COctTreeBase::Reduce() {
    gpTmpOctTree = nullptr;
    ReduceTree();
    m_nNumBranches += gpTmpOctTree->NoOfChildren() - 1;

    gpTmpOctTree->m_bLastStep = true;
    gpTmpOctTree->RemoveChildren();
}
