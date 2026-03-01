#pragma once

#include "TaskSimple.h"
#include "AnimationEnums.h"

class CTaskUtilityLineUpPedWithCar;
class CPed;
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskSimpleBikeJacked : public CTaskSimple {
    bool m_animWasPlayed{};
    CAnimBlendAssociation* m_firstAnim{};
    AnimationId m_secondAnimId{ ANIM_ID_NO_ANIMATION_SET }; // Animation that is played after `m_anim` has finished
    CVehicle* m_vehicle{};
    uint32 m_door{};
    uint32 m_time{};
    CPed* m_jacker{};
    bool m_isVictimDriver{};
    CTaskUtilityLineUpPedWithCar* m_taskUtilityLineUpPedWithCar{}; 
public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_BIKE_JACKED;

    CTaskSimpleBikeJacked(CVehicle* vehicle, uint32 door, uint32 time, CPed* carJacker, bool a6);
    CTaskSimpleBikeJacked(const CTaskSimpleBikeJacked&);
    ~CTaskSimpleBikeJacked();

    static void FinishAnimBikeHitCB(CAnimBlendAssociation* anim, void* data);

    CTask* Clone() const override { return new CTaskSimpleBikeJacked{ *this }; }
    eTaskType GetTaskType() const override { return Type;  }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;
    bool SetPedPosition(CPed* ped) override;

private: // Wrappers for hooks
    // 0x648B90
    CTaskSimpleBikeJacked* Constructor(CVehicle* vehicle, uint32 door, uint32 time, CPed* carJacker, bool a6) { this->CTaskSimpleBikeJacked::CTaskSimpleBikeJacked(vehicle, door, time, carJacker, a6); return this; }

    // 0x648C40
    CTaskSimpleBikeJacked* Destructor() { this->CTaskSimpleBikeJacked::~CTaskSimpleBikeJacked(); return this;}
};
