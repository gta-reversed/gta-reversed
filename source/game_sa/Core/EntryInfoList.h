#pragma once

#include "EntryInfoNode.h"

class CEntryInfoList {
public:
    CEntryInfoNode* m_node;

public:
    static void InjectHooks();

    void Flush();                          // 0x536E10
    void DeleteNode(CEntryInfoNode* node); // Most likely inlined in final exe
    bool IsEmpty() const noexcept { return !m_node; }

    // inlined
    CEntryInfoNode* GetNodePtr() { return m_node; } // AKA = GetHeadPtr
};

VALIDATE_SIZE(CEntryInfoList, 0x4);
