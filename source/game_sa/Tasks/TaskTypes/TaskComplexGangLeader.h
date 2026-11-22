#pragma once

#include "TaskTimer.h"
#include "TaskComplex.h"

class CEvent;
class CPedGroup;
class CPed;
class CEntity;

class NOTSA_EXPORT_VTABLE CTaskComplexGangLeader : public CTaskComplex {
public:
    CPedGroup* m_gang{};
    CTaskTimer m_wanderTimer{};
    CTaskTimer m_scanTimer{};
    CTaskTimer m_exhaleTimer{};     // Creates exahle FX when smoking cigs
    bool       m_animsReferenced{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_GANG_LEADER;

    CTaskComplexGangLeader(CPedGroup* gang);
    CTaskComplexGangLeader(const CTaskComplexGangLeader&);
    ~CTaskComplexGangLeader();

    static auto GetRandomGangAmbientAnim(CPed* ped, CEntity* entity) ->AnimationId;
    static auto ShouldLoadGangAnims() -> bool;
    static auto DoGangAbuseSpeech(CPed* talker, CPed* sayTo) -> void;

    CPed* TryToPassObject(CPed* ped, CPedGroup* group);
    void  UnrefAnimBlock(); // NOTSA

    CTask*    Clone() const override { return new CTaskComplexGangLeader{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    virtual void ScanForStuff(CPed* ped);

private: // Wrappers for hooks
    // 0x65DED0
    CTaskComplexGangLeader* Constructor(CPedGroup* gang) {
        this->CTaskComplexGangLeader::CTaskComplexGangLeader(gang);
        return this;
    }
    // 0x65DF30
    CTaskComplexGangLeader* Destructor() {
        this->CTaskComplexGangLeader::~CTaskComplexGangLeader();
        return this;
    }
};
