/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Rect.h"
#include "PtrListSingleLink.h"
#include "Pool.h"

/*
node level 2
 +----------------------+--
 | node level 1         +
 |  +--------+--------+ +
 |  |node    |node    | +
 |  |level 0 |level 0 | +
 |  |index 0 |index 1 | +
 |  +--------+--------+ +
 |  |node    |node    | +
 |  |level 0 |level 0 | +
 |  |index 2 |index 3 | +
 |  +--------+--------+ +
 +----------------------+--
 |                      |

 Total rectangles = 4^startLevel
*/

template<typename T>
class CQuadTreeNode {
public:
    using ListType                   = CPtrListSingleLink<T>;
    using CQuadTreeNodeRectCallBack  = void (*)(const CRect& rect, T item);
    using CQuadTreeNodeVec2DCallBack = void (*)(const CVector2D& rect, T item);

protected:
    using CQuadTreeNodePool = CPool<CQuadTreeNode<void*>>;
    static inline auto& ms_pQuadTreeNodePool = StaticRef<CQuadTreeNodePool*>(0xB745BC);

public:
    static void* operator new(size_t sz) {
        assert(sz == sizeof(CQuadTreeNode<void*>));
        return ms_pQuadTreeNodePool->New();
    }

    static void operator delete(void* data, size_t sz) {
        assert(sz == sizeof(CQuadTreeNode<void*>));
        ms_pQuadTreeNodePool->Delete(static_cast<CQuadTreeNode<void*>*>(data));
    }

public:
    static void InjectHooks() {
        RH_ScopedClass(CQuadTreeNode<T>);
        RH_ScopedCategory("Core");

        RH_ScopedInstall(InitPool, 0x552C00);
        RH_ScopedOverloadedInstall(FindSector, "rect", 0x5525A0, int32(CQuadTreeNode<T>::*)(const CRect&));
        RH_ScopedOverloadedInstall(FindSector, "vec", 0x552640, int32(CQuadTreeNode<T>::*)(const CVector2D&));
        RH_ScopedInstall(InSector, 0x5526A0);
        RH_ScopedOverloadedInstall(DeleteItem, "", 0x552A40, void(CQuadTreeNode<T>::*)(T));
        RH_ScopedOverloadedInstall(DeleteItem, "rect", 0x552A90, void(CQuadTreeNode<T>::*)(T, const CRect&));
        RH_ScopedInstall(AddItem, 0x552CD0);
        RH_ScopedInstall(GetAll, 0x552870);
        RH_ScopedOverloadedInstall(GetAllMatching, "rect", 0x5528C0, void(CQuadTreeNode<T>::*)(const CRect&, ListType&));
        RH_ScopedOverloadedInstall(GetAllMatching, "vec", 0x552930, void(CQuadTreeNode<T>::*)(const CVector2D&, ListType&));
        RH_ScopedOverloadedInstall(ForAllMatching, "rect", 0x552980, void(CQuadTreeNode<T>::*)(const CRect&, CQuadTreeNodeRectCallBack));
        RH_ScopedOverloadedInstall(ForAllMatching, "vec", 0x5529F0, void(CQuadTreeNode<T>::*)(const CVector2D&, CQuadTreeNodeVec2DCallBack));
    }

    // 0x552C00
    static void InitPool() {
        ZoneScoped;

        if (ms_pQuadTreeNodePool) {
            return;
        }
        ms_pQuadTreeNodePool = new CQuadTreeNodePool(400, "QuadTreeNodes");
    }

    static CQuadTreeNodePool* GetPool() {
        return ms_pQuadTreeNodePool;
    }

public:
    CQuadTreeNode(const CRect& size, int32 startLevel) :
        m_ItemList(),
        m_apChildren{ nullptr } {
        m_Rect   = size;
        m_nLevel = startLevel;
    }

    ~CQuadTreeNode() {
        for (auto& child : m_apChildren) {
            delete child;
        }
        m_ItemList.Flush();
    }

    // 0x552CD0
    void AddItem(T item, const CRect& rect) {
        if (!m_nLevel) {
            m_ItemList.AddItem(item);
            return;
        }

        for (auto sector = 0; sector < 4; ++sector) {
            if (!InSector(rect, sector)) {
                continue;
            }

            if (!m_apChildren[sector]) {
                const CRect sectorRect = GetSectorRect(sector);
                m_apChildren[sector]   = new CQuadTreeNode(sectorRect, m_nLevel - 1);
            }

            m_apChildren[sector]->AddItem(item, rect);
        }
    }

    // 0x552A40
    void DeleteItem(T item) {
        m_ItemList.DeleteItem(item);

        for (auto& sector : m_apChildren) {
            if (sector) {
                sector->DeleteItem(item);
            }
        }
    }

    // 0x552A90
    void DeleteItem(T item, const CRect& rect) {
        m_ItemList.DeleteItem(item);

        for (auto sector = 0; sector < 4; ++sector) {
            if (m_apChildren[sector] && InSector(rect, sector)) {
                m_apChildren[sector]->DeleteItem(item);
            }
        }
    }

    // Returns -1 if not found
    // 0x5525A0
    int32 FindSector(const CRect& rect) {
        const auto center = m_Rect.GetCenter();
        if (!m_nLevel) {
            return -1;
        }

        // This stuff for sure can be simplified, but my attempts break the logic
        if (center.y > rect.top) {
            if (center.x > rect.right) {
                return 2;
            }
            if (center.x >= rect.left) {
                return -1;
            }
            return 3;
        }
        if (center.y >= rect.bottom) {
            return -1;
        }
        if (center.x > rect.right) {
            return 0;
        }
        if (center.x >= rect.left) {
            return -1;
        }

        return 1;
    }

    // Returns -1 if not found
    // 0x552640
    int32 FindSector(const CVector2D& posn) {
        const auto center = m_Rect.GetCenter();
        if (!m_nLevel) {
            return -1;
        }

        // This stuff for sure can be simplified, but my attempts break the logic
        const bool bLeftHalf = center.x > posn.x;
        if (bLeftHalf) {
            if (center.y <= posn.y) {
                return 0;
            } else {
                return 2;
            }
        } else if (center.y <= posn.y) {
            return 1;
        } else {
            return 3;
        }
    }

    void ForAllMatching(const CRect& rect, CQuadTreeNodeRectCallBack callback) {
        for (auto* const item : m_ItemList) {
            callback(rect, item);
        }

        for (auto sector = 0; sector < 4; ++sector) {
            if (m_apChildren[sector] && InSector(rect, sector)) {
                m_apChildren[sector]->ForAllMatching(rect, callback);
            }
        }
    }

    void ForAllMatching(const CVector2D& posn, CQuadTreeNodeVec2DCallBack callback) {
        for (auto* const item : m_ItemList) {
            callback(posn, item);
        }

        const auto sector = FindSector(posn);
        if (sector == -1 || !m_apChildren[sector]) {
            return;
        }

        m_apChildren[sector]->ForAllMatching(posn, callback);
    }

    void GetAll(ListType& list) {
        for (auto* const item : m_ItemList) {
            list.AddItem(item);
        }

        for (auto& sector : m_apChildren) {
            if (sector) {
                sector->GetAll(list);
            }
        }
    }

    // 0x5528C0
    void GetAllMatching(const CRect& rect, ListType& list) {
        for (auto* const item : m_ItemList) {
            list.AddItem(item);
        }

        for (auto sector = 0; sector < 4; ++sector) {
            if (m_apChildren[sector] && InSector(rect, sector)) {
                m_apChildren[sector]->GetAllMatching(rect, list);
            }
        }
    }

    void GetAllMatching(const CVector2D& posn, ListType& list) {
        for (auto* const item : m_ItemList) {
            list.AddItem(item);
        }

        const auto sector = FindSector(posn);
        if (sector == -1 || !m_apChildren[sector]) {
            return;
        }

        m_apChildren[sector]->GetAllMatching(posn, list);
    }

    // 0x5526A0
    bool InSector(const CRect& rect, int32 sector) const {
        if (!m_nLevel) {
            return false;
        }

        const CRect sectorRect = GetSectorRect(sector);
        if (   sectorRect.left <= rect.right // LinesInside???
            && sectorRect.right >= rect.left
            && sectorRect.bottom <= rect.top
            && sectorRect.top >= rect.bottom
        ) {
            return true;
        }

        return false;
    }

    CRect GetSectorRect(int32 sector) const {
        CRect      rect   = m_Rect;
        const auto center = rect.GetCenter();
        switch (sector) {
        case 0:
            rect.right  = center.x;
            rect.bottom = center.y;
            break;
        case 1:
            rect.left   = center.x;
            rect.bottom = center.y;
            break;
        case 2:
            rect.right = center.x;
            rect.top   = center.y;
            break;
        case 3:
            rect.left = center.x;
            rect.top  = center.y;
            break;
        default:
            assert(false); // Shouldn't ever get here
        }
        return rect;
    }

    bool LiesInside(const CRect& rect) const {
        return    m_Rect.left <= rect.right
               && m_Rect.right >= rect.left
               && m_Rect.bottom <= rect.top
               && m_Rect.top >= rect.bottom;
    };

public:
    CRect          m_Rect;
    ListType       m_ItemList;
    CQuadTreeNode* m_apChildren[4];
    uint32         m_nLevel; // 0 - last level
};
VALIDATE_SIZE(CQuadTreeNode<void*>, 0x28);
