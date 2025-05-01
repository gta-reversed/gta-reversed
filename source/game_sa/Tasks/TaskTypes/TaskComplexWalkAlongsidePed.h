#pragma once

#include "Vector.h"
#include "TaskComplex.h"

class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskComplexWalkAlongsidePed : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_WALK_ALONGSIDE_PED;

    static void InjectHooks();

    CTaskComplexWalkAlongsidePed(CPed* ped, float giveUpDist);
    CTaskComplexWalkAlongsidePed(const CTaskComplexWalkAlongsidePed&);
    ~CTaskComplexWalkAlongsidePed() = default;

    CTask* CreateSubTask(eTaskType tt, CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexWalkAlongsidePed{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x683240
    CTaskComplexWalkAlongsidePed* Constructor(CPed* a, float b) {
        this->CTaskComplexWalkAlongsidePed::CTaskComplexWalkAlongsidePed(a, b);
        return this;
    }

    // 0x6832C0
    CTaskComplexWalkAlongsidePed* Destructor() {
        this->CTaskComplexWalkAlongsidePed::~CTaskComplexWalkAlongsidePed();
        return this;
    }

private:
    CPed::Ref m_TargetPed{};    //< The ped to walk alongside of
    float     m_GiveUpDist{};   //< Distance after which to give up following
    CVector   m_Offset{};       //< Pos offset
};
