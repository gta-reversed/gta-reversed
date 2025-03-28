/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

template<typename NodeType>
class CPtrList {
public:
    using NodeType = NodeType;

public:
    static void InjectHooks();

    bool      IsMemberOfList(void* data) const;
    bool      IsEmpty() const { return !m_node; }
    uint32    CountElements() const;
    NodeType* GetNode() const { return m_node; }

public:
    NodeType* m_node{};
};
