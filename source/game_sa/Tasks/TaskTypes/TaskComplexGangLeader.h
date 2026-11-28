#pragma once

#include "TaskTimer.h"
#include "TaskComplex.h"

class CEvent;
class CPedGroup;
class CPed;
class CEntity;

class NOTSA_EXPORT_VTABLE CTaskComplexGangLeader : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_GANG_LEADER;

    explicit CTaskComplexGangLeader(CPedGroup* gang);
    CTaskComplexGangLeader(const CTaskComplexGangLeader&);
    ~CTaskComplexGangLeader() override;

    CTask*    Clone() const override { return new CTaskComplexGangLeader{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    virtual void ScanForStuff(CPed* ped);

    static AnimationId GetRandomGangAmbientAnim(CPed* ped, CEntity* entity);
    static bool        ShouldLoadGangAnims();
    static void        DoGangAbuseSpeech(CPed* talker, CPed* sayTo);
    static CPed*       TryToPassObject(CPed* ped, CPedGroup* group);

    static void InjectHooks();

private:
    void UnrefAnimBlock(); // NOTSA

private:
    CPedGroup* m_Gang{};
    CTaskTimer m_WanderTimer{};
    CTaskTimer m_ScanTimer{};
    CTaskTimer m_ExhaleTimer{}; //!< Creates the exhale FX when smoking
    bool       m_AnimsReferenced{};
};
VALIDATE_SIZE(CTaskComplexGangLeader, 0x38);
