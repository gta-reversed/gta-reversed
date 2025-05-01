#pragma once

#include "TaskSimple.h"
#include "Vector.h"

class CTaskUtilityLineUpPedWithCar;
class CAnimBlendAssociation;
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarAlign : public CTaskSimple {

public:
    bool m_animHasFinished = {};                     // 8
    CAnimBlendAssociation* m_anim = {};              // 0xC
    CVehicle* m_veh = {};                            // 0x10
    CVector m_doorPos = {};                          // 0x14
    uint32 m_door = {};                              // 0x20
    CTaskUtilityLineUpPedWithCar* m_lineUpTask = {}; // 0x24

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_CAR_ALIGN;

    CTaskSimpleCarAlign(CVehicle* veh, CVector const& doorPos, uint32 targetDoor, CTaskUtilityLineUpPedWithCar* taskLineUp);
    CTaskSimpleCarAlign(const CTaskSimpleCarAlign&);
    ~CTaskSimpleCarAlign();

    static void FinishAnimCarAlignCB(CAnimBlendAssociation*, void*);

    void FixHeading(CPed* ped);
    void StartAnim(CPed* ped);

    CTask* Clone() const override { return new CTaskSimpleCarAlign{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;
    bool SetPedPosition(CPed* ped) override;

private: // Wrappers for hooks
    // 0x645B40
    CTaskSimpleCarAlign* Constructor(CVehicle* veh, CVector const& doorPos, int32 targetDoor, CTaskUtilityLineUpPedWithCar* taskLineUp) {
        this->CTaskSimpleCarAlign::CTaskSimpleCarAlign(veh, doorPos, targetDoor, taskLineUp);
        return this;
    }

    // 0x645BE0
    CTaskSimpleCarAlign* Destructor() {
        this->CTaskSimpleCarAlign::~CTaskSimpleCarAlign();
        return this;
    }
};
