#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

class CTaskInteriorSitOnChair;
class CAnimBlendAssociation;
class CEvent;
class CPed;
class Interior_c;
struct InteriorInfo_t;

class NOTSA_EXPORT_VTABLE CTaskInteriorSitOnChair : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_INTERIOR_SIT_ON_CHAIR;

    static void InjectHooks();

    CTaskInteriorSitOnChair(Interior_c* interior, InteriorInfo_t* interiorInfo, bool bDoInstantly);
    CTaskInteriorSitOnChair(const CTaskInteriorSitOnChair&);
    ~CTaskInteriorSitOnChair();

    static void FinishAnimCB(CAnimBlendAssociation* anim, void* data);

    CTask*    Clone() const override { return new CTaskInteriorSitOnChair{ *this }; } // 0x675CF0
    eTaskType GetTaskType() const override { return Type; } // 0x675C80
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x675C30
    CTaskInteriorSitOnChair* Constructor(Interior_c* interior, InteriorInfo_t* interiorInfo, uint8 bDoInstantly) {
        this->CTaskInteriorSitOnChair::CTaskInteriorSitOnChair(interior, interiorInfo, bDoInstantly);
        return this;
    }

    // 0x675C90
    CTaskInteriorSitOnChair* Destructor() {
        this->CTaskInteriorSitOnChair::~CTaskInteriorSitOnChair();
        return this;
    }

private:
    Interior_c*            m_Interior{};
    InteriorInfo_t*        m_InteriorInfo{};
    bool                   m_bDoInstantly{};
    CAnimBlendAssociation* m_Anim{};
    AnimationId            m_PrevAnimId{ANIM_ID_UNDEFINED};
    CTaskTimer             m_TaskTimer;
    bool                   m_bUpdatePedPos{};
    bool                   m_bTaskFinished{};
    bool                   m_bTaskAborting{};
};
