#pragma once

#include "TaskSimple.h"
#include "Base.h"
#include "AnimationEnums.h"
enum ePedPieceTypes;

class CAnimBlendAssociation;

class NOTSA_EXPORT_VTABLE CTaskSimpleBeHit : public CTaskSimple {
public:
    CPed*          m_Attacker;
    bool           m_bIsFinished;
    bool           m_bAnimAdded;
    AnimationId    m_nAnimId;
    AssocGroupId   m_nAnimGroup;
    ePedPieceTypes m_eHitZone;
    int32          m_nHitPower;
    int32          m_nDirn;
    CAnimBlendAssociation* m_Anim;

public:
    constexpr static auto Type = eTaskType::TASK_SIMPLE_BE_HIT;

    CTaskSimpleBeHit(CPed* ped, ePedPieceTypes pedPieceType, int32 direction, int32 damageHealth);
    ~CTaskSimpleBeHit() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

    void StartAnim(CPed* ped);
    static void FinishAnimBeHitCB(CAnimBlendAssociation* a1, void* task);

    static void InjectHooks();
    CTaskSimpleBeHit* Constructor(CPed* ped, ePedPieceTypes pedPieceType, int32 damageArmor, int32 damageHealth) { this->CTaskSimpleBeHit::CTaskSimpleBeHit(ped, pedPieceType, damageArmor, damageHealth); return this; }
    CTaskSimpleBeHit* Destructor() { this->CTaskSimpleBeHit::~CTaskSimpleBeHit(); return this; }
};
VALIDATE_SIZE(CTaskSimpleBeHit, 0x28);
