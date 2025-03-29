/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

template<typename T>
class CPtrList {
public:
    using NodeType = T;

public:
    static void InjectHooks();

    bool IsMemberOfList(void* data) const {
        for (NodeType* node = GetNode(); node; node = node->m_next) {
            if (node->m_item == data) {
                return true;
            }
        }
        return false;
    }

    uint32 CountElements() const {
        uint32    counter;
        for (NodeType* node = GetNode(); node; node = node->m_next) {
            ++counter;
        }
        return counter;
    }

    bool IsEmpty() const { return !m_node; }
    NodeType* GetNode() const { return m_node; }

public:
    NodeType* m_node{};
};
