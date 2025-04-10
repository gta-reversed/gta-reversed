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

class CQuadTreeNode;

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

typedef void(*CQuadTreeNodeRectCallBack) (const CRect& rect, void* item);
typedef void(*CQuadTreeNodeVec2DCallBack) (const CVector2D& rect, void* item);
typedef CPool<CQuadTreeNode> CQuadTreeNodePool;

class CQuadTreeNode {
public:
    using ListType = CPtrListSingleLink<void*>;

public:
    CRect          m_Rect;
    ListType       m_ItemList;
    CQuadTreeNode* m_apChildren[4];
    uint32         m_nLevel; // 0 - last level

    static CQuadTreeNodePool*& ms_pQuadTreeNodePool;

public:
    CQuadTreeNode(const CRect& size, int32 startLevel);
    ~CQuadTreeNode();

    static void* operator new(unsigned size);
    static void operator delete(void* data);

public:
    static void InjectHooks();

    static void InitPool();

    void AddItem(void* item, const CRect& rect);
    void DeleteItem(void* item);
    void DeleteItem(void* item, const CRect& rect);
    int32 FindSector(const CRect& rect);
    int32 FindSector(const CVector2D& posn);
    void ForAllMatching(const CRect& rect, CQuadTreeNodeRectCallBack callback);
    void ForAllMatching(const CVector2D& posn, CQuadTreeNodeVec2DCallBack callback);
    void GetAll(ListType& list);
    void GetAllMatching(const CRect& rect, ListType& list);
    void GetAllMatching(const CVector2D& posn, ListType& list);
    bool InSector(const CRect& rect, int32 sector) const;

// Helpers
public:
    CRect GetSectorRect(int32 sector) const;
    bool LiesInside(const CRect& rect) const {
        return    m_Rect.left <= rect.right
               && m_Rect.right >= rect.left
               && m_Rect.bottom <= rect.top
               && m_Rect.top >= rect.bottom;
    };
};

VALIDATE_SIZE(CQuadTreeNode, 0x28);
