#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

struct InteriorInfo_t;
class CAnimBlendAssociation;
class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskInteriorSitInChair : public CTaskSimple {
public:
    static constexpr auto Type = TASK_INTERIOR_SIT_ON_CHAIR;

    static void InjectHooks();

    CTaskInteriorSitInChair(int32 getOutOfChairInterval, InteriorInfo_t* interiorInfo, bool sitInChairInstantly);
    CTaskInteriorSitInChair(const CTaskInteriorSitInChair&);
    ~CTaskInteriorSitInChair();

    static CTaskInteriorSitInChair* FinishAnimCB(CAnimBlendAssociation* anim, void* self);

    CTask*    Clone() const override { return new CTaskInteriorSitInChair{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

private:
public:
    int32                  m_getOutOfChairInterval{};
    InteriorInfo_t*        m_pInteriorInfo{};
    bool                   m_sitInChairInstantly{};
    CAnimBlendAssociation* m_pAnim{};
    AnimationId            m_prevAnimId{};
    CTaskTimer             m_getOutOfChairTimer{};
    bool                   m_updatePedPos{};
    bool                   m_lastAnimHasFinished{};
    bool                   m_taskAborting{};
};
