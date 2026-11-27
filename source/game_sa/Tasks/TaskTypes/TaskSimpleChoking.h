/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "AnimBlendAssociation.h"
#include <extensions/EntityRef.hpp>

class NOTSA_EXPORT_VTABLE CTaskSimpleChoking : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_CHOKING;

    CTaskSimpleChoking(CPed* attacker, bool isTeargas);
    CTaskSimpleChoking(const CTaskSimpleChoking&);
    ~CTaskSimpleChoking() override;

    CTask*    Clone() const override { return new CTaskSimpleChoking{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

    void UpdateChoke(CPed* victim, CPed* attacker, bool isTeargas);

    static void InjectHooks();

private:
    static void DeleteAnimChokeCB(CAnimBlendAssociation* anim, void* data);

private:
    notsa::EntityRef<CPed> m_Attacker{};
    CAnimBlendAssociation* m_Anim{};
    uint32                 m_TimeRemaining{};
    uint32                 m_TimeStarted{};
    bool                   m_IsTeargas{};
    bool                   m_IsFinished{};
};
VALIDATE_SIZE(CTaskSimpleChoking, 0x1C);
