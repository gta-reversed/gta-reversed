/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CReference;

#define MAX_NUM_REFERENCES 3000

class CReferences {
public:
    static inline auto (&aRefs)[MAX_NUM_REFERENCES] = StaticRef<CReference[MAX_NUM_REFERENCES]>(0xB9B9A8);
    static inline auto& pEmptyList = StaticRef<CReference*>(0xBA1768);

public:
    static void InjectHooks();

    static void   Init();
    static uint32 ListSize(CReference* ref);
    static void   RemoveReferencesToPlayer();
    static void   PruneAllReferencesInWorld();
};
