#pragma once

#include "TaskComplex.h"

class InteriorGroup_c;
class Interior_c;
struct InteriorInfo_t;
class  CPed;

class NOTSA_EXPORT_VTABLE CTaskInteriorBeInOffice : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_INTERIOR_BE_IN_OFFICE;

    static void InjectHooks();

    CTaskInteriorBeInOffice(InteriorGroup_c* ig);
    CTaskInteriorBeInOffice(const CTaskInteriorBeInOffice&);
    ~CTaskInteriorBeInOffice() override = default;

    CTask*    Clone() const override { return new CTaskInteriorBeInOffice{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override { return CreateUseInfoTask(ped, false); }
    CTask*    CreateFirstSubTask(CPed* ped) override { return CreateUseInfoTask(ped, true); }
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private:
    void   GetInfoForPedToUse(CPed* ped, int32* outDur);
    CTask* CreateUseInfoTask(CPed* ped, bool bDoInstantly); // notsa

private: // Wrappers for hooks
    // 0x675220
    CTaskInteriorBeInOffice* Constructor(InteriorGroup_c* ig) {
        this->CTaskInteriorBeInOffice::CTaskInteriorBeInOffice(ig);
        return this;
    }

    // 0x675260
    CTaskInteriorBeInOffice* Destructor() {
        this->CTaskInteriorBeInOffice::~CTaskInteriorBeInOffice();
        return this;
    }

private:
    InteriorGroup_c* m_IntGrp{};
    Interior_c*      m_CurrInt{};
    InteriorInfo_t*  m_CurrIntInfo{};
};
