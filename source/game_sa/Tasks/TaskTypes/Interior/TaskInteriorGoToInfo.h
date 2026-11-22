#pragma once

#include "TaskComplex.h"

class CPed;
struct InteriorInfo_t;
class Interior_c;

class NOTSA_EXPORT_VTABLE CTaskInteriorGoToInfo final : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_INTERIOR_GOTO_INFO;

    static void InjectHooks();

    CTaskInteriorGoToInfo(InteriorInfo_t* interiorInfo, Interior_c* interior, bool bDoInstantly);
    CTaskInteriorGoToInfo(const CTaskInteriorGoToInfo&);
    ~CTaskInteriorGoToInfo() override = default;

    CTask*    Clone() const override { return new CTaskInteriorGoToInfo{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6757D0
    CTaskInteriorGoToInfo* Constructor(InteriorInfo_t* interiorInfo, Interior_c* interior, uint8 unk) {
        this->CTaskInteriorGoToInfo::CTaskInteriorGoToInfo(interiorInfo, interior, unk);
        return this;
    }

    // 0x675810
    CTaskInteriorGoToInfo* Destructor() {
        this->CTaskInteriorGoToInfo::~CTaskInteriorGoToInfo();
        return this;
    }

private:
    InteriorInfo_t* m_IntInfo      = {};
    Interior_c*     m_Int          = {};
    bool            m_bDoInstantly = {};
};
