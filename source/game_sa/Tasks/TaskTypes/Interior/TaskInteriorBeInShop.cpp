#include "StdInc.h"

#include "TaskInteriorBeInShop.h"

#include "EventInteriorUseInfo.h"

#include "Interior/InteriorInfo_t.h"
#include "Interior/Interior_c.h"
#include "Interior/InteriorGroup_c.h"

#include "IKChainManager_c.h"

void CTaskInteriorBeInShop::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorBeInShop, 0x870244, 15);
    RH_ScopedCategory("Tasks/TaskTypes/Interior");

    RH_ScopedInstall(Constructor, 0x6753E0);
    RH_ScopedInstall(Destructor, 0x675460);

    RH_ScopedVMTInstall(Clone, 0x675470);
    RH_ScopedVMTInstall(GetTaskType, 0x675450);
    RH_ScopedVMTInstall(GetWanderType, 0x675440);
    RH_ScopedVMTInstall(ScanForStuff, 0x6754D0);
}

// 0x6753E0
CTaskInteriorBeInShop::CTaskInteriorBeInShop(InteriorGroup_c* interiorGroup) :
    CTaskComplexWander{PEDMOVE_WALK, CGeneral::RandomNodeHeading(), true, 0.2f},
    m_CurrIntGrp{interiorGroup}
{
}

// 0x675470
CTaskInteriorBeInShop::CTaskInteriorBeInShop(const CTaskInteriorBeInShop& o) :
    CTaskInteriorBeInShop{o.m_CurrIntGrp}
{
}

// 0x6754D0
void CTaskInteriorBeInShop::ScanForStuff(CPed* ped) {
    if (!m_bScanForStuff) {
        return;
    }
    InteriorInfo_t* closestIntInfo{};
    Interior_c* closestInt{};
    float unk{};
    if (!m_CurrIntGrp->FindClosestInteriorInfo(
        -1,
        ped->GetPosition(),
        2.f,
        &closestIntInfo,
        &closestInt,
        &unk
    )) {
        return;
    }
    if (closestIntInfo == m_CurrIntInfo) {
        return;
    }
    const auto PedAddUseInfoEvent = [&]{
        ped->GetEventGroup().Add(CEventInteriorUseInfo{ closestIntInfo, closestInt, 0, false });
    };
    switch (closestIntInfo->Type) {
    case eInteriorInfoType::UNK_8: {
        if (CGeneral::DoCoinFlip()) {
            g_ikChainMan.LookAt(
                "TaskBeInShop",
                ped,
                nullptr,
                3'000,
                eBoneTag::BONE_UNKNOWN,
                &closestIntInfo->Pos,
                true,
                0.25f,
                500,
                3,
                false
            );
        } else {
            PedAddUseInfoEvent();
        }
        break;
    }
    case eInteriorInfoType::UNK_9: {
        if (!CGeneral::DoCoinFlip()) {
            m_bScanForStuff = false;
            PedAddUseInfoEvent();
        }
        break;
    }
    }
    m_CurrIntInfo = closestIntInfo;
}
