#pragma once

#include "PtrListDoubleLink.h"
#include "PtrNodeDoubleLink.h"

class CRepeatSector;
class CPhysical;

class CEntryInfoNode {
public:
    CPtrListDoubleLink<CPhysical*>* m_doubleLinkList;
    CPtrNodeDoubleLink<CPhysical*>* m_doubleLink;
    CRepeatSector*                  m_repeatSector;
    CEntryInfoNode*                 m_previous;
    CEntryInfoNode*                 m_next;

public:
    static void InjectHooks();

    static void* operator new(unsigned size);
    static void  operator delete(void* ptr, size_t sz);

    void         AddToList(CEntryInfoNode* pNext); // Fully inlined in final game
};
VALIDATE_SIZE(CEntryInfoNode, 0x14);
