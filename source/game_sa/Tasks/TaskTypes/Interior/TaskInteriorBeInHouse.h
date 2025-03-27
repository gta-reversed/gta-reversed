#pragma once

#include "TaskComplex.h"

class CPed;
class InteriorGroup_c;
class Interior_c;
struct InteriorInfo_t;

class NOTSA_EXPORT_VTABLE CTaskInteriorBeInHouse : public CTaskComplex {
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_INTERIOR_BE_IN_HOUSE;

    CTaskInteriorBeInHouse(InteriorGroup_c* intGrp);
    CTaskInteriorBeInHouse(const CTaskInteriorBeInHouse&); // NOTSA
    ~CTaskInteriorBeInHouse() = default;

    void GetInfoForPedToUse(CPed* ped, int32* outDuration);

    CTask*    Clone() const override { return new CTaskInteriorBeInHouse{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override { return CreateFirstSubTask(ped); }
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x674FC0
    CTaskInteriorBeInHouse* Constructor(InteriorGroup_c* a2) {
        this->CTaskInteriorBeInHouse::CTaskInteriorBeInHouse(a2);
        return this;
    }
    // 0x675000
    CTaskInteriorBeInHouse* Destructor() {
        this->CTaskInteriorBeInHouse::~CTaskInteriorBeInHouse();
        return this;
    }

private:
    InteriorGroup_c* m_intGrp{};  //< Current interior's group (Used to find the interior to use for the ped)
    Interior_c*      m_int{};     //< Current inerior
    InteriorInfo_t*  m_intInfo{}; //< Current interior's info
};
