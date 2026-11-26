#include "StdInc.h"
#include "TaskComplexPlayHandSignalAnim.h"
#include "TaskSimplePlayHandSignalAnim.h"
#include "TaskSimpleStandStill.h"
#include "Ragdoll/IKChainManager.h"

void CTaskComplexPlayHandSignalAnim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPlayHandSignalAnim, 0x86d5dc, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x61B2B0, "0", {}, AnimationId, float);
    RH_ScopedDestructorInstall(0x61BDF0);

    RH_ScopedInstall(GetAnimIdForPed, 0x61B460);
    RH_ScopedInstall(CreateSubTask, 0x61B2F0);
    RH_ScopedVMTInstall(Clone, 0x61BA00);
    RH_ScopedVMTInstall(GetTaskType, 0x61B2E0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x61B570);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x61B4F0);
    RH_ScopedVMTInstall(ControlSubTask, 0x61B580);
}

// 0x61B2B0
CTaskComplexPlayHandSignalAnim::CTaskComplexPlayHandSignalAnim(AnimationId animationId, float blendFactor) :
    m_AnimationId{animationId},
    m_AnimBlenDelta{blendFactor}
{
}

// For 0x61BA00
CTaskComplexPlayHandSignalAnim::CTaskComplexPlayHandSignalAnim(const CTaskComplexPlayHandSignalAnim& o) :
    CTaskComplexPlayHandSignalAnim{ o.m_AnimationId, o.m_AnimBlenDelta }
{}

// 0x61BDF0
CTaskComplexPlayHandSignalAnim::~CTaskComplexPlayHandSignalAnim() {
    enum {
        RIGHT,
        LEFT,
    };
    enum {
        NONFAT,
        FAT
    };
    const eModelID handModels[2][2]{
        // nonfat           fat
        { MODEL_SHANDL, MODEL_FHANDL }, // left
        { MODEL_SHANDR, MODEL_FHANDR }  // right
    };

    // Remove hand model refs
    for (const auto i : { LEFT, RIGHT }) {
        CModelInfo::GetModelInfo(handModels[i][m_UseFatHands ? FAT : NONFAT])->RemoveRef();
    }

    // Deal with anim
    if (m_IsAnimationLoaded) { // Remove anim ref
        CAnimManager::RemoveAnimBlockRef(ms_animBlock);
    } else if (ms_animBlock != -1 && !CAnimManager::GetAnimBlocks()[ms_animBlock].RefCnt) { 
        if (!rng::all_of(std::array{ LEFT, RIGHT }, [&, this](auto i) { // Unload anim block if not all of the models has refs
            return CModelInfo::GetModelInfo(handModels[i][m_UseFatHands ? FAT : NONFAT])->m_nRefCount != 0;
        })) {
            CStreaming::RemoveModel(IFPToModelId(ms_animBlock));        
        }
    }
}

// 0x61B460
AnimationId CTaskComplexPlayHandSignalAnim::GetAnimIdForPed(CPed* ped) {
    switch (ped->m_nPedType) {
    case PED_TYPE_GANG1: return ANIM_ID_GSIGN1;
    case PED_TYPE_GANG2: return ANIM_ID_GSIGN2;
    case PED_TYPE_GANG3: return ANIM_ID_GSIGN3;
    case PED_TYPE_GANG5: return ANIM_ID_GSIGN5;
    case PED_TYPE_GANG8: return ANIM_ID_GSIGN4;
    default:             return ANIM_ID_UNDEFINED;
    }
}

// 0x61B2F0
CTask* CTaskComplexPlayHandSignalAnim::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_STAND_STILL: 
        return new CTaskSimpleStandStill{ 0, true, false };
    case TASK_SIMPLE_HANDSIGNAL_ANIM:
        return new CTaskComplexSequence{
            new CTaskSimpleStandStill{ CGeneral::GetRandomNumberInRange(0, 1'500), false, false, 8.f },
            new CTaskSimplePlayHandSignalAnim{ m_AnimationId, m_AnimBlenDelta, m_UseFatHands, false }
        };
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}

// 0x61B4F0
CTask* CTaskComplexPlayHandSignalAnim::CreateFirstSubTask(CPed* ped) {
    if (IKChainManager_c::IsArmPointing(IK_ARM_LEFT, ped)) {
        return nullptr;
    }

    if (m_AnimationId == ANIM_ID_UNDEFINED) {
        m_AnimationId = GetAnimIdForPed(ped);
        if (m_AnimationId == ANIM_ID_UNDEFINED) {
            return nullptr;
        }
    }

    m_UseFatHands = notsa::contains({ MODEL_BALLAS2, MODEL_FAM1, MODEL_FAM3 }, ped->GetModelId());

    return CreateSubTask(TASK_SIMPLE_STAND_STILL);
}

// 0x61B580
CTask* CTaskComplexPlayHandSignalAnim::ControlSubTask(CPed* ped) {
    // 0x61B58C - Stream in the hand models
    const auto StreamHand = [](eModelID model, bool isReferenced) {
        if (!CStreaming::IsModelLoaded(model)) {
            CStreaming::RequestModel(model, STREAMING_KEEP_IN_MEMORY);
            return isReferenced;
        }
        if (!isReferenced) {
            CModelInfo::GetModelInfo(model)->AddRef();
        }
        return true;
    };
    m_IsLeftHandLoaded  = StreamHand(m_UseFatHands ? MODEL_FHANDL : MODEL_SHANDL, m_IsLeftHandLoaded);
    m_IsRightHandLoaded = StreamHand(m_UseFatHands ? MODEL_FHANDR : MODEL_SHANDR, m_IsRightHandLoaded);

    // 0x61B640 - Then the anims
    if (m_IsLeftHandLoaded && m_IsRightHandLoaded) {
        if (ms_animBlock == (uint32)-1) {
            ms_animBlock = CAnimManager::GetAnimationBlockIndex("ghands");
        }
        if (CStreaming::IsModelLoaded(IFPToModelId(ms_animBlock))) {
            if (!m_IsAnimationLoaded) {
                CAnimManager::AddAnimBlockRef(ms_animBlock);
                m_IsAnimationLoaded = true;
            }
        } else {
            CStreaming::RequestModel(IFPToModelId(ms_animBlock), STREAMING_KEEP_IN_MEMORY);
        }
    }

    // 0x61B6AB - Everything is loaded, play the hand signal
    if (m_IsAnimationLoaded && notsa::isa<CTaskSimpleStandStill>(m_pSubTask)) {
        return CreateSubTask(TASK_SIMPLE_HANDSIGNAL_ANIM);
    }

    return m_pSubTask;
}
