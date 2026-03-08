#include "StdInc.h"
#include "TaskComplexSunbathe.h"

void CTaskComplexSunbathe::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSunbathe, 0x86e0ac, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x631F80);
    RH_ScopedInstall(Destructor, 0x632050);

    //RH_ScopedGlobalInstall(CanSunbathe, 0x632140, { .reversed = false });
    //RH_ScopedGlobalInstall(ShouldLoadSunbatheAnims, 0x632190, { .reversed = false });

    //RH_ScopedInstall(CreateSubTask, 0x638290, { .reversed = false });

    RH_ScopedVMTInstall(Clone, 0x6366A0);
    RH_ScopedVMTInstall(GetTaskType, 0x632040);
    RH_ScopedVMTInstall(MakeAbortable, 0x6320F0, { .reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6399F0, { .reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x639CB0, { .reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x6381A0, { .reversed = false });

}

// 0x631f80
CTaskComplexSunbathe::CTaskComplexSunbathe(CObject* towel, bool bStartStanding) :
    m_pTowel{towel},
    m_bStartStanding{bStartStanding}
{
    if (m_pTowel) {
        CEntity::SafeRegisterRef(m_pTowel);
        m_pTowel->m_nObjectType = OBJECT_TYPE_DECORATION;
    }

    const auto LoadAnim = [](int32& idx, CAnimBlock*& blk, const char* name) {
        idx = CAnimManager::GetAnimationBlockIndex(name);
        blk = CAnimManager::GetAnimationBlock(name);
    };
    LoadAnim(m_BeachAnimBlockIndex, m_pBeachAnimBlock, "beach");
    LoadAnim(m_SunbatheAnimBlockIndex, m_pSunbatheAnimBlock, "sunbathe");

    // Refs added later in another function
}

CTaskComplexSunbathe::CTaskComplexSunbathe(const CTaskComplexSunbathe& o) :
    CTaskComplexSunbathe{ o.m_pTowel, o.m_bStartStanding }
{
}

// 0x632050
CTaskComplexSunbathe::~CTaskComplexSunbathe() {
    if (m_pTowel) {
        m_pTowel->m_nObjectType = OBJECT_TEMPORARY;
        CEntity::SafeCleanUpRef(m_pTowel);
    }

    const auto UnrefAnimBlock = [](bool& refed, int32 blkIdx) {
        if (refed) {
            CAnimManager::RemoveAnimBlockRef(blkIdx);
            refed = false;
        }
    };
    UnrefAnimBlock(m_bBeachAnimsReferenced, m_BeachAnimBlockIndex);
    UnrefAnimBlock(m_bSunbatheAnimsReferenced, m_SunbatheAnimBlockIndex);
}

// 0x6320F0
bool CTaskComplexSunbathe::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return plugin::CallMethodAndReturn<bool, 0x6320F0, CTaskComplexSunbathe*, CPed*, eAbortPriority, CEvent const*>(this, ped, priority, event);
}

// 0x6399F0
CTask* CTaskComplexSunbathe::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6399F0, CTaskComplexSunbathe*, CPed*>(this, ped);
}

// 0x639CB0
CTask* CTaskComplexSunbathe::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x639CB0, CTaskComplexSunbathe*, CPed*>(this, ped);
}

// 0x6381A0
CTask* CTaskComplexSunbathe::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6381A0, CTaskComplexSunbathe*, CPed*>(this, ped);
}
