#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

#include "Enums/AnimationEnums.h"

struct InteriorInfo_t;
class  CPed;
class  CAnimBlendAssociation;
class  CEvent;

class NOTSA_EXPORT_VTABLE CTaskInteriorSitAtDesk : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_INTERIOR_SIT_AT_DESK;

    static void InjectHooks();

    CTaskInteriorSitAtDesk(int32 actionAnimTime, InteriorInfo_t* interiorInfo, bool bDoInstantly);
    CTaskInteriorSitAtDesk(const CTaskInteriorSitAtDesk&);
    ~CTaskInteriorSitAtDesk();

    static void FinishAnimCB(CAnimBlendAssociation* anim, void* data);

    CTask*    Clone() const override { return new CTaskInteriorSitAtDesk{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

    void StartRandomLoopAnim(CPed* ped, float blendDelta);
    void StartRandomOneOffAnim(CPed* ped);

private:
    int32                  m_Duration{};
    InteriorInfo_t*        m_InteriorInfo{};
    bool                   m_bDoInstantly{};
    CAnimBlendAssociation* m_Anim{};
    AnimationId            m_PrevAnimId{ ANIM_ID_UNDEFINED };
    CTaskTimer             m_TaskTimer{};
    CTaskTimer             m_AnimTimer{};
    bool                   m_bUpdatePedPos{};
    bool                   m_bTaskFinished{};
    bool                   m_bTaskAborting{};

private: // Wrappers for hooks
    // 0x676010
    CTaskInteriorSitAtDesk* Constructor(int32 actionAnimTime, InteriorInfo_t* interiorInfo, uint8 a4) {
        this->CTaskInteriorSitAtDesk::CTaskInteriorSitAtDesk(actionAnimTime, interiorInfo, a4);
        return this;
    }

    // 0x676080
    CTaskInteriorSitAtDesk* Destructor() {
        this->CTaskInteriorSitAtDesk::~CTaskInteriorSitAtDesk();
        return this;
    }
};
