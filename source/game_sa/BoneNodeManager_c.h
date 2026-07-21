#pragma once

#include "Base.h"

#include "List_c.h"
#include "ListItem_c.h"
#include "BoneNode_c.h"
#include "tBoneInfo.h"

class BoneNodeManager_c {
public:
    static inline auto& ms_boneInfos = StaticRef<std::array<tBoneInfo, MAX_BONE_NUM>>(0x8D26D0);

public:
    std::array<BoneNode_c, 128> m_Space;
    TList_c<BoneNode_c>         m_Bones;

public:
    static void InjectHooks();

    BoneNodeManager_c() = default;  // 0x617330
    ~BoneNodeManager_c() = default; // 0x617390

    bool Init();
    void Exit();
    void Reset();
    BoneNode_c* GetBoneNode();
    void ReturnBoneNode(BoneNode_c* bone);
    uint32 GetNumBoneNodesLeft() const;
};
VALIDATE_SIZE(BoneNodeManager_c, 0x4C0C);

static inline auto& g_boneNodeMan = StaticRef<BoneNodeManager_c>(0xC10820);
