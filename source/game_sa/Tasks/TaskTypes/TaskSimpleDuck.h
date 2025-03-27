/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "Vector2D.h"
class CAnimBlendAssociation;

enum eDuckControlType : uint8 {
    DUCK_STANDALONE = 0,	        // Duck anim removed when task removed
    DUCK_STANDALONE_WEAPON_CROUCH,  // Duck anim removed when task removed
    DUCK_TASK_CONTROLLED,	        // Duck directly linked to a controlling task
    DUCK_ANIM_CONTROLLED,	        // Duck linked to duck anim (make duck partial?)
    DUCK_SCRIPT_CONTROLLED,
};

class NOTSA_EXPORT_VTABLE CTaskSimpleDuck : public CTaskSimple {
public:
    uint32                 m_StartTime{};
    uint16                 m_LengthOfDuck{};
    int16                  m_ShotWhizzingCounter{};
    CAnimBlendAssociation* m_DuckAnim{};
    CAnimBlendAssociation* m_MoveAnim{};
    int8                   m_bIsFinished{};
    int8                   m_bIsAborting{};
    bool                   m_bNeedToSetDuckFlag{ true }; // - In case ped->bIsDucking flag gets cleared elsewhere, so we know to stop duck task
    int8                   m_bIsInControl{ true };       // - If duck task is being controlled by another task then it requires continuous control
    CVector2D              m_MoveCmd{ 0.f, 0.f };
    eDuckControlType       m_DuckControlType{};
    int8                   m_CountDownFrames{ -1 };           

public:
    static constexpr auto Type = TASK_SIMPLE_DUCK;

    static void InjectHooks();

    CTaskSimpleDuck(eDuckControlType duckControlType, uint16 lengthOfDuck, int16 showWhizzingCounter = -1);
    CTaskSimpleDuck(const CTaskSimpleDuck&); // NOTSA: `Clone()` helper
    ~CTaskSimpleDuck();

    static void DeleteDuckAnimCB(CAnimBlendAssociation* assoc, void* task);
    static bool CanPedDuck(CPed* ped);

    bool IsTaskInUseByOtherTasks() const;
    void AbortBecauseOfOtherDuck(CPed* ped); // May've been returning bool originally, unsure.
    void RestartTask(CPed* ped);
    void ControlDuckMove(CVector2D moveSpeed = {});
    void SetMoveAnim(CPed*);
    void ForceStopMove();
    bool StopFireGun() const;
    void SetDuckTimer(uint16 time);

    CTask* Clone() const override { return new CTaskSimpleDuck{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = eAbortPriority::ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;
    void SetPedIsDucking(CPed* ped, bool value);

private: // Wrappers for hooks

// 0x691FC0
    CTaskSimpleDuck* Constructor(eDuckControlType duckControlType, int16 lengthOfDuck, int16 showWhizzingCounter) {
        this->CTaskSimpleDuck::CTaskSimpleDuck(duckControlType, lengthOfDuck, showWhizzingCounter);
        return this;
    }

    // 0x692030
    CTaskSimpleDuck* Destructor() {
        this->CTaskSimpleDuck::~CTaskSimpleDuck();
        return this;
    }
}; 
VALIDATE_SIZE(CTaskSimpleDuck, 0x28);
