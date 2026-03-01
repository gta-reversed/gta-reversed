#pragma once

#include "TaskSimple.h"

class CVehicle;
class CTask;
class CTaskUtilityLineUpPedWithCar;
class CAnimBlendAssociation;

/*!
* Task resposinble for playing the get-in animation.
* In case of vehicles it is created right after the door opening anim ends.
*/
class NOTSA_EXPORT_VTABLE CTaskSimpleCarGetIn : public CTaskSimple {
public:
    bool m_finished{};                            ///< Has the anim finished (=> meaning task has finished)
    CAnimBlendAssociation* m_anim{};              ///< The animation playing
    int32 m_unk{};                                ///< Unused
    int32 m_unk2{};                               ///< Unused
    CVehicle* m_veh{};                            ///< The vehicle we're getting into
    uint32 m_door{};                              ///< The door we want to enter at
    CTaskUtilityLineUpPedWithCar* m_lineUpTask{}; ///< Utility lineup task


public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_CAR_GET_IN;
    
    CTaskSimpleCarGetIn(CVehicle* vehicle, uint32 door, CTaskUtilityLineUpPedWithCar* task);
    CTaskSimpleCarGetIn(const CTaskSimpleCarGetIn&); // NOTSA
    ~CTaskSimpleCarGetIn();

    static void FinishAnimCarGetInCB(CAnimBlendAssociation* anim, void* data);
    
    void StartAnim(CPed const* ped);

    CTask*      Clone() const override { return new CTaskSimpleCarGetIn{ *this }; }
    eTaskType   GetTaskType() const override { return Type; }
    bool        MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool        ProcessPed(CPed* ped) override;
    bool        SetPedPosition(CPed* ped) override;

private: // Wrappers for hooks
    // 0x646690
    CTaskSimpleCarGetIn* Constructor(CVehicle* vehicle, uint32 door, CTaskUtilityLineUpPedWithCar* task) {
        this->CTaskSimpleCarGetIn::CTaskSimpleCarGetIn(vehicle, door, task);
        return this;
    }

    // 0x646710
    CTaskSimpleCarGetIn* Destructor() {
        this->CTaskSimpleCarGetIn::~CTaskSimpleCarGetIn();
        return this;
    }
};
VALIDATE_SIZE(CTaskSimpleCarGetIn, 0x24);
