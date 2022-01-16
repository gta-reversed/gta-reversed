    /*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "Vector.h"
#include "Vector2D.h"
#include "WeaponInfo.h"
#include "AnimBlendAssociation.h"
#include "Entity.h"

class CTaskSimpleUseGun : public CTaskSimple {
public:
    bool m_bIsFinished;
    bool m_bIsInControl;
    bool m_bMoveControl;
    bool m_bFiredGun;
    bool m_bBlockedLOS;
    union {
        uint8 m_nFireGunThisFrame;
        struct {
            uint8 bRightHand : 1;
            uint8 bLefttHand : 1;
        };
    };
    bool m_bSkipAim;

    uint8 m_nNextCommand; // 0x1 reloading - 0x2 firing
    uint8 m_nLastCommand; // active command - 0x1 reloading - 0x2 firing
    char _pad[3];
    CVector2D m_vecMoveCommand;
    CEntity* m_pTarget;
    CVector m_vecTarget;

    CAnimBlendAssociation* m_pAnim;

    CWeaponInfo* m_pWeaponInfo;
    uint16 m_nBurstLength;
    uint16 m_nBurstShots;

    uint8 m_nCountDownFrames;
    bool m_ArmIKInUse;
    bool m_LookIKInUse;
    bool m_bAimImmediate;

public:
    CTaskSimpleUseGun(CEntity* targetEntity, CVector vecTarget, uint8 nCommand, uint16 nBurstLength = 1, bool bAimImmediate = false);
    ~CTaskSimpleUseGun() override;

    CTask* Clone() override { return new CTaskSimpleUseGun(m_pTarget, m_vecTarget, m_nLastCommand, m_nBurstLength, m_bAimImmediate); }
    eTaskType GetTaskType() override { return TASK_SIMPLE_USE_GUN; };
    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    bool ProcessPed(CPed* ped) override;
    bool SetPedPosition(CPed* ped) override;

    void AbortIK(CPed* ped);
    void AimGun(CPed* ped);

    int32 SkipAim(CPed* ped);
    int32 ClearAnim(CPed* ped);

    bool ControlGun(CPed* ped, CEntity* target, int8 nCount);
    bool ControlGunMove(CVector2D* moveSpeed);

    void FinishGunAnimCB(CAnimBlendAssociation* anim, void* data);

    void FireGun(CPed* ped, bool);
    bool PlayerPassiveControlGun();
    void RemoveStanceAnims(CPed* ped, float);
    static bool RequirePistolWhip(CPed* ped, CEntity* entity);
    void Reset(CPed* ped, CEntity* entity, CVector posn, int8, short);

    void SetMoveAnim(CPed* ped);
    void StartAnim(CPed* ped);
    void StartCountDown(uint8, bool);

#if ANDROID
    void CreateTask();
    void Serialize();
#endif

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleUseGun* Constructor(CEntity* targetEntity, CVector vecTarget, uint8 nCommand, uint16 nBurstLength = 1, bool bAimImmediate = false);
};

VALIDATE_SIZE(CTaskSimpleUseGun, 0x3C);

