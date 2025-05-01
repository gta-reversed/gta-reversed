/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "Vector.h"

struct RpClump;
class  FxSystem_c;
class  CAnimBlendAssociation;
class  CEntity;
class  CPed;
class  CPlayerPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleJetPack : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_JETPACK;

    static void InjectHooks();

    CTaskSimpleJetPack(const CVector* pVecTargetPos = nullptr, float fCruiseHeight = 10.0f, int32 nHoverTime = 0, CEntity* entity = nullptr);
    CTaskSimpleJetPack(const CTaskSimpleJetPack&);
    ~CTaskSimpleJetPack() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskSimpleJetPack{ *this }; }
    bool      MakeAbortable(class CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

    void RenderJetPack(CPed* ped);
    void ProcessThrust(CPed* ped);
    bool ProcessAnims(CPed* ped);
    void ProcessControlInput(CPlayerPed* player);
    void ApplyRollAndPitch(CPed* ped);
    void DropJetPack(CPed* ped);
    void DoJetPackEffect(CPed* ped);
    void StopJetPackEffect();

private:
    bool m_bIsFinished{};
    bool m_bAddedIdleAnim{};
    bool m_bAnimsReferenced{};
    bool m_bAttackButtonPressed{};
    bool m_bSwitchedWeapons{};

    bool  m_ThrustStop{};
    char  m_ThrustFwd{};    // -1 = downwards, 0 = nothing, 1 = accelerate
    float m_ThrustStrafe{};
    float m_ThrustAngle{};

    float m_LegSwingFwd{};
    float m_LegSwingSide{};
    float m_LegTwist{};

    float m_LegSwingFwdSpeed{};
    float m_LegSwingSideSpeed{};
    float m_LegTwistSpeed{};

    CVector m_PrevVelocity{};
    float   m_PrevHeading{};

    RpClump*               m_JetPackClump{};
    CAnimBlendAssociation* m_Anim{};

    CVector  m_TargetPos{};
    float    m_CruiseHeight{};
    int32    m_HoverTime{};
    uint32   m_StartHover{};
    CEntity* m_TargetEntity{};

    FxSystem_c* m_ThrusterFX[2]{}; // left and right thruster FX
    float       m_FxKeyTime{};

private: // Wrappers for hooks
    // 0x67B4E0
    CTaskSimpleJetPack* Constructor(CVector const* pVecTargetPos, float fCruiseHeight, int32 nHoverTime, CEntity * entity) {
        this->CTaskSimpleJetPack::CTaskSimpleJetPack(pVecTargetPos, fCruiseHeight, nHoverTime, entity);
        return this;
    }

    // 0x67E600
    CTaskSimpleJetPack* Destructor() {
        this->CTaskSimpleJetPack::~CTaskSimpleJetPack();
        return this;
    }
};
VALIDATE_SIZE(CTaskSimpleJetPack, 0x70);
