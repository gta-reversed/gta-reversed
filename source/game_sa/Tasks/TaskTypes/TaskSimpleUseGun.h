/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <utility>
#include "TaskSimple.h"
#include "Vector.h"
#include "Vector2D.h"
#include "TaskSimpleGunControl.h"

class CWeaponInfo;
class CAnimBlendAssociation;
class CEntity;

class NOTSA_EXPORT_VTABLE CTaskSimpleUseGun : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_USE_GUN;

    CTaskSimpleUseGun(CEntity* targetEntity, CVector vecTarget = {}, eGunCommand nCommand = eGunCommand::AIM, uint16 nBurstLength = 1, bool bAimImmediate = false);
    ~CTaskSimpleUseGun() override;

    CTask* Clone() const override { return new CTaskSimpleUseGun(m_TargetEntity, m_TargetPos, m_LastCmd, m_BurstLength, m_IsAimImmediate); }
    eTaskType GetTaskType() const override { return Type; };
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;
    bool SetPedPosition(CPed* ped) override;

    void AbortIK(CPed* ped);
    void AimGun(CPed* ped);

    void ClearAnim(CPed* ped);

    bool ControlGun(CPed* ped, CEntity* target, eGunCommand cmd);
    bool ControlGunMove(const CVector2D& moveSpeed);

    static void FinishGunAnimCB(CAnimBlendAssociation* anim, void* data);

    void FireGun(CPed* ped, bool); // Originally returned a bool, but since the return value isn't used anywhere, we're going to make it void...
    bool PlayerPassiveControlGun();
    void RemoveStanceAnims(CPed* ped, float x);
    static bool RequirePistolWhip(CPed* ped, CEntity* entity);
    void Reset(CPed* ped, CEntity* targetEntity, CVector targetPos, eGunCommand, int16 burstLength);

    void SetMoveAnim(CPed* ped);
    void StartAnim(CPed* ped);
    void StartCountDown(uint8 numIdleFrames, char isMax);

    auto GetLastGunCommand() const { return m_LastCmd; }

#if ANDROID
    void CreateTask();
    void Serialize();
#endif

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleUseGun* Constructor(CEntity* targetEntity, CVector vecTarget, eGunCommand nCommand, uint16 nBurstLength, bool bAimImmediate) {
        this->CTaskSimpleUseGun::CTaskSimpleUseGun(targetEntity, vecTarget, nCommand, nBurstLength, bAimImmediate);
        return this;
    }

    CVector                      GetAimTargetPosition(CPed* ped) const;
    std::pair<CVector, eBoneTag> GetAimLookAtInfo() const;

public:
    bool                   m_IsFinished{};
    bool                   m_IsInControl{ true };
    bool                   m_HasMoveControl{};
    bool                   m_HasFiredGun{};
    bool                   m_IsLOSBlocked{};
    bool                   m_IsFiringGunRightHandThisFrame : 1{};
    bool                   m_IsFiringGunLeftHandThisFrame : 1{};
    bool                   m_SkipAim{};                           //!< Don't aim (So no crosshair on HUD, etc)
    eGunCommand            m_NextCmd{};                           //!< Active command
    eGunCommand            m_LastCmd{ eGunCommand::UNKNOWN };     //!< Previous command
    CVector2D              m_MoveCmd{ 0.f, 0.f };                 //!< Where we should be moving towards
    CEntity::Ref           m_TargetEntity{};                      //!< Entity we're aiming for (If set, `m_TargetPos` is ignored)
    CVector                m_TargetPos{};                         //!< Either a position, or a direction vector (?) - Only used if `m_TargetEntity` is not set.
    CAnimBlendAssociation* m_Anim{};                              //!< Animation for the current command (Reloading, Firing, etc)
    CWeaponInfo*           m_WeaponInfo{};                        //!< Ped active weapon's info
    uint16                 m_BurstLength{};                       //!< How many bullets to fire in a burst (For eGunCommand::FIREBURST)
    uint16                 m_BurstShots{};                        //!< How many bullets have been fired this burst (For eGunCommand::FIREBURST)
    uint8                  m_CountDownFrames{ 0xFF };             //!< Some countdown (Maybe for burst?)
    bool                   m_IsArmIKInUse{};                      //!< Are we using arm IK (PointArmAt)
    bool                   m_IsLookIKInUse{};                     //!< Are we using look at IK (LookAt)
    bool                   m_IsAimImmediate{};                    //!< How fast the aim animation should be (?)
};
VALIDATE_SIZE(CTaskSimpleUseGun, 0x3C);
