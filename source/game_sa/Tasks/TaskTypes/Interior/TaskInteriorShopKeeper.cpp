#include "StdInc.h"

#include "TaskInteriorShopKeeper.h"
#include "TaskInteriorUseInfo.h"
#include "TaskSimpleRunAnim.h"

#include "InteriorGroup_c.h"
#include "InteriorInfo_t.h"
#include "InteriorManager_c.h"

void CTaskInteriorShopKeeper::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorShopKeeper, 0x870290, 11);
    RH_ScopedCategory("Tasks/TaskTypes/Interior");

    RH_ScopedInstall(Constructor, 0x675660);
    RH_ScopedInstall(Destructor, 0x6756A0);

    RH_ScopedVMTInstall(Clone, 0x6756C0);
    RH_ScopedVMTInstall(GetTaskType, 0x675690);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6764C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x676550);
    RH_ScopedVMTInstall(ControlSubTask, 0x675740);
}

// 0x675660
CTaskInteriorShopKeeper::CTaskInteriorShopKeeper(InteriorGroup_c* intGrp, bool bIsSetup) :
    m_bIsSetup{bIsSetup},
    m_IntGrp{intGrp}
{
}

// 0x6756C0
CTaskInteriorShopKeeper::CTaskInteriorShopKeeper(const CTaskInteriorShopKeeper& o) :
    CTaskInteriorShopKeeper{o.m_IntGrp, o.m_bIsSetup}
{
}

// 0x6756A0
CTaskInteriorShopKeeper::~CTaskInteriorShopKeeper() {
    if (m_IntInfo) {
        m_IntInfo->IsInUse = false;
    }
}

// 0x6764C0
CTask* CTaskInteriorShopKeeper::CreateNextSubTask(CPed* ped) {
    return new CTaskInteriorUseInfo{ m_IntInfo, m_Int };
}

// 0x676550
CTask* CTaskInteriorShopKeeper::CreateFirstSubTask(CPed* ped) {
    if (!m_IntInfo) {
        VERIFY(m_IntGrp->FindInteriorInfo(eInteriorInfoType::UNK_10, &m_IntInfo, &m_Int));
    }
    m_IntInfo->IsInUse = true;
    return new CTaskInteriorUseInfo{ m_IntInfo, m_Int, 0, m_bIsSetup };
}

// 0x675740
CTask* CTaskInteriorShopKeeper::ControlSubTask(CPed* ped) {
    if (m_bStartServeAnim) {
        return new CTaskSimpleRunAnim{ ANIM_GROUP_INT_SHOP, ANIM_ID_SHOP_CASHIER, 4.f, 0 };
    }
    return m_pSubTask;
}
