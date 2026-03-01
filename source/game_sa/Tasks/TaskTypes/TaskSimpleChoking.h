/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "AnimBlendAssociation.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleChoking : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_CHOKING;

    static void InjectHooks();

    CTaskSimpleChoking(CPed* attacker, bool bIsTeargas);
    CTaskSimpleChoking(const CTaskSimpleChoking&);

    CTask*    Clone() const override { return new CTaskSimpleChoking{*this}; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

    void UpdateChoke(CPed* victim, CPed* attacker, bool bIsTeargas);

private: // Wrappers for hooks
    // 0x6202C0
    CTaskSimpleChoking* Constructor(CPed* attacker, uint8 isTeargas) {
        this->CTaskSimpleChoking::CTaskSimpleChoking(attacker, isTeargas);
        return this;
    }

    // 0x620370
    CTaskSimpleChoking* Destructor() {
        this->CTaskSimpleChoking::~CTaskSimpleChoking();
        return this;
    }

public:
    CPed*                  m_pAttacker;
    CAnimBlendAssociation* m_pAnim;
    uint32                 m_nTimeRemaining;
    uint32                 m_nTimeStarted;
    bool                   m_bIsTeargas;
    bool                   m_bIsFinished;
};
VALIDATE_SIZE(CTaskSimpleChoking, 0x1C);
