#pragma once

#include "TaskComplex.h"

class CEvent;
class CPed;
struct InteriorInfo_t;
class Interior_c;

class NOTSA_EXPORT_VTABLE CTaskInteriorUseInfo : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_INTERIOR_USE_INFO;

    static void InjectHooks();

    CTaskInteriorUseInfo(InteriorInfo_t* interiorInfo, Interior_c* interior, int32 duration = 0, bool bDoInstantly = false);
    CTaskInteriorUseInfo(const CTaskInteriorUseInfo&);
    ~CTaskInteriorUseInfo();

    CTask*    Clone() const override { return new CTaskInteriorUseInfo{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;
private: // Wrappers for hooks
    // 0x675A50
    CTaskInteriorUseInfo* Constructor(InteriorInfo_t* interiorInfo, Interior_c* interior, int32 duration, bool bDoInstantly) {
        this->CTaskInteriorUseInfo::CTaskInteriorUseInfo(interiorInfo, interior, duration, bDoInstantly);
        return this;
    }

    // 0x675A90
    CTaskInteriorUseInfo* Destructor() {
        this->CTaskInteriorUseInfo::~CTaskInteriorUseInfo();
        return this;
    }

private:
    InteriorInfo_t* m_IntInfo{};
    Interior_c*     m_Int{};
    int32           m_Dur{};
    bool            m_bDoInstantly{};
};
