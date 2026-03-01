#include "StdInc.h"

#include "TaskSimpleTired.h"

void CTaskSimpleTired::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleTired, 0x86DED0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x630F20);
    RH_ScopedInstall(Destructor, 0x630F60);
    RH_ScopedInstall(StartAnim, 0x630F70);
}

// 0x630F20
CTaskSimpleTired::CTaskSimpleTired(uint32 tiredDurationMs) :
    CTaskSimple(),
    m_TiredDurationMs{ tiredDurationMs }
{
    m_AnimStartMs = 0;
    m_TiredDurationMs_Copy = 0;
    m_AnimHasStarted = false;
    m_NeedsToStartAnim = false;
    m_TiredAnim = nullptr;
}

// 0x630F70
void CTaskSimpleTired::StartAnim(CPed* ped) {
    ped->SetMoveState(PEDMOVE_STILL);
    ped->SetMoveAnim();
    m_AnimStartMs = CTimer::GetTimeInMS();
    m_TiredDurationMs_Copy = m_TiredDurationMs;
    m_AnimHasStarted = true;
    m_TiredAnim = CAnimManager::BlendAnimation(
        ped->m_pRwClump,
        FindPlayerPed() == ped && CClothes::GetDefaultPlayerMotionGroup() == ANIM_GROUP_FAT ? ANIM_GROUP_FAT_TIRED : ANIM_GROUP_DEFAULT,
        ANIM_ID_IDLE_TIRED,
        4.f
    );
}

// 0x630FF0
bool CTaskSimpleTired::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (m_TiredAnim && m_TiredAnim == RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_IDLE_TIRED)) {
        m_TiredAnim->m_BlendDelta = -4.f;
    }
    return true;
}

// 0x637970
bool CTaskSimpleTired::ProcessPed(CPed* ped) {
    if (!m_TiredAnim) {
        StartAnim(ped);
    }

    if (!m_AnimHasStarted) {
        return false;
    }

    if (m_NeedsToStartAnim) {
        m_AnimStartMs = CTimer::GetTimeInMS();
        m_NeedsToStartAnim = false;
    }

    if (CTimer::GetTimeInMS() < m_AnimStartMs + m_TiredDurationMs_Copy) {
        return false;
    }

    MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
    return true;
}
