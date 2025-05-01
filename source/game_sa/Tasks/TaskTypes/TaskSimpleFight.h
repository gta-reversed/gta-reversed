/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
class CAnimBlendAssociation;
class CEntity;
class CPed;
class CPlayerPed;

enum eFightAttackType : int8 {
    FIGHT_ATTACK_HIT_1 = 0,
    FIGHT_ATTACK_HIT_2 = 1,
    FIGHT_ATTACK_HIT_3 = 2,
    FIGHT_ATTACK_FIGHT_BLOCK = 3,
    FIGHT_ATTACK_FIGHTIDLE = 4,
};

class NOTSA_EXPORT_VTABLE CMeleeInfo {
public:
    AssocGroupId m_nAnimGroup;
    float  m_fRanges;
    float  m_fHit[5];
    float  m_fChain[5];
    float  m_fRadius[5];
    float  m_fGroundLoop;
    int32  ABlockHit;
    int32  ABlockChain;
    uint8  m_nHitLevel;
    int32  m_nDamage;
    int32  field_58;
    int32  m_Hit[5];
    int32  m_AltHit[5];
    uint16 m_wFlags;
};
VALIDATE_SIZE(CMeleeInfo, 0x88);

class NOTSA_EXPORT_VTABLE CTaskSimpleFight : public CTaskSimple {
public:
    bool                   m_bIsFinished;
    bool                   m_bIsInControl;
    bool                   m_bAnimsReferenced;
    AssocGroupId           m_nRequiredAnimGroup;
    uint16                 m_nIdlePeriod;
    uint16                 m_nIdleCounter;
    int8                   m_nContinueStrike;
    int8                   m_nChainCounter;
    CEntity*               m_pTargetEntity;
    CAnimBlendAssociation* m_pAnim;
    CAnimBlendAssociation* m_pIdleAnim;
    int8                   m_nComboSet;
    eFightAttackType       m_nCurrentMove;
    uint8                  m_nNextCommand;
    uint8                  m_nLastCommand;

    static inline CMeleeInfo (&m_aComboData)[12] = *(CMeleeInfo(*)[12])0xC170D0;

public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_FIGHT;

    CTaskSimpleFight(CEntity* entity, int32 nCommand, uint32 nIdlePeriod = 10000);
    ~CTaskSimpleFight() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskSimpleFight(m_pTargetEntity, m_nLastCommand, m_nIdlePeriod); }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

    static void LoadMeleeData();

    void BeHitWhileBlocking(CPed* ped1, CPed* ped2, int8, int8);
    void ChooseAttackAI(CPed* ped);
    void ChooseAttackPlayer(CPed* ped);
    bool ControlFight(CEntity* entity, uint8 command);

    void FightHitCar(CPed* ped, CVehicle* vehicle, CVector& posn1, CVector& posn2, int16, int8);
    void FightHitObj(CPed* ped, CObject* object, CVector& posn1, CVector& posn2, int16, int8);
    void FightHitPed(CPed* creator, CPed* victim, CVector& posn1, CVector& posn2, int16);
    void FightSetUpCol(float);
    void FightStrike(CPed* ped, CVector& posn);

    void FindTargetOnGround(CPed* ped);
    void FinishMeleeAnimCB(CAnimBlendAssociation*, void*);

    bool IsComboSet();
    bool IsHitComboSet();

    void GetAvailableComboSet(CPed* ped, int8);
    void GetComboType(char*);
    AssocGroupId GetComboAnimGroupID();
    void GetHitLevel(const char*);
    void GetHitSound(int32);
    void GetRange();
    void GetStrikeDamage(CPed* ped);

    void SetPlayerMoveAnim(CPlayerPed* player);
    void StartAnim(CPed* ped, int32);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleFight* Constructor(CEntity* entity, int32 nCommand, uint32 nIdlePeriod);
    CTaskSimpleFight* Destructor();

};
VALIDATE_SIZE(CTaskSimpleFight, 0x28);
