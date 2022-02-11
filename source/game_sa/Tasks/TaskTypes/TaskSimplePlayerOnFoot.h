/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "AnimBlendAssociation.h"

extern int32& gLastRandomNumberForIdleAnimationID;
extern uint32& gLastTouchTimeDelta;
extern float& gDuckAnimBlendData;
extern bool& gbUnknown_8D2FE8;

class CPed;

class CTaskSimplePlayerOnFoot : public CTaskSimple {
public:
    uint32   m_nAnimationBlockIndex;
    uint32   m_nFrameCounter;
    int32    m_nTimer;
    int32    dword_14;           // always 0
    CEntity* m_pLookingAtEntity; // always 0

public:
    CTaskSimplePlayerOnFoot();
    ~CTaskSimplePlayerOnFoot() override = default;

    eTaskType GetTaskType() override { return TASK_SIMPLE_PLAYER_ON_FOOT; }               // 0x6857C0
    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    CTask* Clone() override { return new CTaskSimplePlayerOnFoot(); }                     // 0x68AFF0
    bool ProcessPed(CPed* ped) override;

    void ProcessPlayerWeapon(CPlayerPed* player);
    void PlayIdleAnimations(CPlayerPed* player);
    void PlayerControlFighter(CPlayerPed* player);
    static void PlayerControlZeldaWeapon(CPlayerPed* player);
    static void PlayerControlDucked(CPlayerPed* player);
    int32 PlayerControlZelda(CPlayerPed* player, bool bAvoidJumpingAndDucking);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimplePlayerOnFoot* Constructor();
    CTaskSimplePlayerOnFoot* Destructor();

    bool ProcessPed_Reversed(CPed* ped);
    bool MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event);
};

VALIDATE_SIZE(CTaskSimplePlayerOnFoot, 0x1C);
