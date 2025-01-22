#pragma once

#include "Base.h"
#include <rwplcore.h>
#include "TaskSimple.h"
#include <extensions/WEnum.hpp>
#include "Enums/eBoneTag.h"

class CEntity;
class CPed;
class CTaskSimpleIKChain;
class IKChain_c;

class NOTSA_EXPORT_VTABLE CTaskSimpleIKChain : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_IK_CHAIN;

    CTaskSimpleIKChain(const char* name, eBoneTag32 effectorBoneTag, CVector effectorPos, eBoneTag32 pivotBoneTag, CEntity* entity, eBoneTag32 offsetBoneTag, CVector offsetPos,
                       float speed, int32 time, int32 blendTime);
    ~CTaskSimpleIKChain() override;


    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override; // 0x62EC30
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

    virtual bool CreateIKChain(CPed* ped);

    void BlendOut(int32 blendOutTime = 250);
    IKChain_c* GetIKChain() const;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleIKChain* Constructor(char* name, eBoneTag32 effectorBoneTag, RwV3d effectorVec, eBoneTag32 pivotBoneTag, CEntity* a6, eBoneTag32 offsetBoneTag, RwV3d offsetPos, float speed, int32 time, int32 blendTime) { this->CTaskSimpleIKChain::CTaskSimpleIKChain(name, effectorBoneTag, effectorVec, pivotBoneTag, a6, offsetBoneTag, offsetPos, speed, time, blendTime); return this; }
    CTaskSimpleIKChain* Destructor() { this->CTaskSimpleIKChain::~CTaskSimpleIKChain(); return this; }

protected:
    int32        m_Duration{};
    int32        m_BlendDuration{};
    IKChain_c*   m_IKChain{};
    int16        m_SlotId{};
    eBoneTag16   m_PivotBone{}; //! Bone we're moving bones up to
    eBoneTag16   m_EffectorBone{}; //! Bone we're moving
    CVector      m_EffectorOffset{};
    CEntity::Ref m_TargetEntity{};
    eBoneTag16   m_OffsetBone{};
    CVector      m_OffsetPos{};
    float        m_Speed{};
    bool         m_TargetEntityExisted{};
    float        m_Blend{};
    int32        m_EndTime{};
    float        m_TargetBlend{};
    int32        m_TargetTime{};
    int32        m_IsBlendingOut{};
};
VALIDATE_SIZE(CTaskSimpleIKChain, 0x58);
