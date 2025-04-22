#include "StdInc.h"

#include "OctTree.h"

bool& COctTree::ms_bFailed = *(bool*)0xBC12DC;
uint32& COctTree::ms_level = *(uint32*)0xBC12E0;
COctTreePool& COctTree::ms_octTreePool = *(COctTreePool*)0xBC12E4;
COctTree*& gpTmpOctTree = *(COctTree**)0xBC12D8;

// 0x5A6DB0
COctTree::COctTree() {
    m_nLevel = 0;
    m_nRedComponent = 0;
    m_nGreenComponent = 0;
    m_nBlueComponent = 0;
    std::ranges::fill(m_aChildrens, -1);
    m_bLastStep = false;
}

// 0x5A7490
COctTree::~COctTree() {
    empty();
}

// 0x5A7410
void* COctTree::operator new(size_t size) {
    return ms_octTreePool.New();
}

// 0x5A7420
void COctTree::operator delete(void* data) {
    ms_octTreePool.Delete(static_cast<COctTree*>(data));
}

// 0x5A7460
void COctTree::InitPool(void* data, int32 dataSize) {
    const auto cap = dataSize / (sizeof(COctTree) + 1);
    ms_octTreePool = {
        cap,
        data,
        (uint8*)(data) + sizeof(COctTree) * cap
    };
    ms_octTreePool.SetDealWithNoMemory(true);
}

// 0x5A6F70
void COctTree::ShutdownPool() {
    ms_octTreePool.Flush();
}

// 0x5A75B0
bool COctTree::InsertTree(uint8 red, uint8 green, uint8 blue) {
    const auto poolIndex = ( // todo: magic
        ((red   << ms_level >> 5) & 4) +
        ((green << ms_level >> 6) & 2) +
        ((blue  << ms_level >> 7) & 1)
    );
    ++ms_level;

    m_nRedComponent   += red;
    m_nGreenComponent += green;
    m_nBlueComponent  += blue;
    m_nLevel++;

    if (ms_level == std::size(m_aChildrens) || m_bLastStep) {
        ms_level = 0;
        m_bLastStep = true;
        RemoveChildren();
        return m_nLevel == 1;
    }

    COctTree* treeElement{};
    if (m_aChildrens[poolIndex] >= 0) {
        treeElement = ms_octTreePool.GetAt(poolIndex);
    } else {
        treeElement = ms_octTreePool.New();
        if (!treeElement) {
            ms_bFailed = true;
            return false;
        }
        m_aChildrens[poolIndex] = (int16)ms_octTreePool.GetIndex(treeElement);
    }

// todo (Izzotop): review fix_bugs
#ifdef FIX_BUGS
    if (!treeElement)
        return false;
#endif

    bool bTreeInserted = treeElement->InsertTree(red, green, blue);

    if (ms_bFailed && treeElement->m_nLevel < 2) {
        delete treeElement;
        m_aChildrens[poolIndex] = -1;
        return false;
    }

    return bTreeInserted;
}

// 0x5A70F0
void COctTree::FillPalette(uint8* data) {
    if (m_bLastStep) {
        auto* colors = *reinterpret_cast<RwRGBA**>(data);
        colors[ms_level].red   = m_nRedComponent   / m_nLevel;
        colors[ms_level].green = m_nGreenComponent / m_nLevel;
        colors[ms_level].blue  = m_nBlueComponent  / m_nLevel;
        colors[ms_level].alpha = 128;
        m_nLevel = ms_level++;
        return;
    }

    for (auto& children : m_aChildrens) {
        if (auto elem = ms_octTreePool.GetAt(children)) {
            elem->FillPalette(data);
        }
    }
}

// 0x5A71E0
uint32 COctTree::FindNearestColour(uint8 red, uint8 green, uint8 blue) {
    if (m_bLastStep) {
        return m_nLevel;
    }

    COctTree* treeElement = this;
    do {
        auto child = ((red >> 5) & 4) + ((green >> 6) & 2) + (blue >> 7); // todo: magic
        auto treeIndex = treeElement->m_aChildrens[child];
        treeElement = ms_octTreePool.GetAt(treeIndex);
        // useless permutation
        red   *= 2;
        green *= 2;
        blue  *= 2;
    } while (!treeElement->m_bLastStep);

    return treeElement->m_nLevel;
}

// 0x5A6DE0
uint32 COctTree::NoOfChildren() {
    uint32 numOfChildren = 0;
    for (auto& m_aChildren : m_aChildrens) {
        if (m_aChildren >= 0) {
            numOfChildren++;
        }
    }
    return numOfChildren;
}

// 0x5A7040
void COctTree::ReduceTree() {
    if (m_bLastStep) {
        return;
    }

    ms_level++;

    uint32 currentLevel = 0;
    uint32 totalLevels = 0;
    static uint32 lastLevel;
    for (auto& children : m_aChildrens) {
        if (children < 0) {
            continue;
        }
        currentLevel++;
        if (COctTree* tree = ms_octTreePool.GetAt(children)) {
            tree->ReduceTree();
            totalLevels += tree->m_nLevel;
        }
    }

    if (currentLevel >= 2 && (totalLevels < lastLevel || !gpTmpOctTree)) {
        lastLevel = totalLevels;
        gpTmpOctTree = this;
    }

    ms_level--;
}

// 0x5A74F0
void COctTree::RemoveChildren() {
    for (auto& children : m_aChildrens) {
        if (children < 0) {
            continue;
        }
        COctTree* elem = ms_octTreePool.GetAt(children);
        delete elem;
        children = -1;
    }
}

// 0x5A6FC0
void COctTree::empty() {
    m_nLevel = 0;
    m_nRedComponent = 0;
    m_nGreenComponent = 0;
    m_nBlueComponent = 0;
    RemoveChildren();
}
