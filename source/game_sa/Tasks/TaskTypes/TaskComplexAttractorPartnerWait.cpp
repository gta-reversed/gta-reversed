#include "StdInc.h"

#include "TaskComplexAttractorPartnerWait.h"
#include "Scripted2dEffects.h"

// 0x633250
CTaskComplexAttractorPartnerWait::CTaskComplexAttractorPartnerWait(bool a2, const CScriptedEffectPair* pair) : CTaskComplex() {
    byteC = a2;
    m_Pair = pair;
}

// 0x6332A0
CTask* CTaskComplexAttractorPartnerWait::CreateNextSubTask(CPed* ped) {
    return nullptr;
}

// 0x6332B0
CTask* CTaskComplexAttractorPartnerWait::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}

// 0x638F20
CTask* CTaskComplexAttractorPartnerWait::CreateFirstSubTask(CPed* ped) {
    auto sequenceIndex = byteC ? m_Pair->dword4 : m_Pair->dword14;
    auto task = new CTaskComplexUseSequence(sequenceIndex);
    ++CTaskSequences::ms_taskSequence[sequenceIndex].m_RefCnt;
    return task;
}
