#include "StdInc.h"

#include "BoneNodeManager.h"

void BoneNodeManager_c::InjectHooks() {
    RH_ScopedClass(BoneNodeManager_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x617330);
    RH_ScopedInstall(Destructor, 0x617390);

    RH_ScopedInstall(Init, 0x6173F0);
    RH_ScopedInstall(Exit, 0x617420);
    RH_ScopedInstall(Reset, 0x617430);
    RH_ScopedInstall(GetBoneNode, 0x617460);
    RH_ScopedInstall(ReturnBoneNode, 0x617470);
    RH_ScopedInstall(GetNumBoneNodesLeft, 0x617480);
}

// 0x6173F0
bool BoneNodeManager_c::Init() {
    for (auto & item : m_Space) {
        m_Bones.AddItem(&item);
    }
    return true;
}

// 0x617420
void BoneNodeManager_c::Exit() {
    m_Bones.RemoveAll();
}

// 0x617430
void BoneNodeManager_c::Reset() {
    Exit();
    Init();
}

// 0x617460
BoneNode_c* BoneNodeManager_c::GetBoneNode() {
    return m_Bones.RemoveHead();
}

// 0x617470
void BoneNodeManager_c::ReturnBoneNode(BoneNode_c* boneNode) {
    m_Bones.AddItem(boneNode);
}

// 0x617480
uint32 BoneNodeManager_c::GetNumBoneNodesLeft() const {
    return m_Bones.GetNumItems();
}
