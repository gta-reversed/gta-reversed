/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
class CAnimBlendAssociation;
class CEntity;

class NOTSA_EXPORT_VTABLE CTaskSimpleThrowProjectile : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_THROW_PROJECTILE;

    static void InjectHooks();

    CTaskSimpleThrowProjectile(CEntity* target, CVector posn);
    CTaskSimpleThrowProjectile(const CTaskSimpleThrowProjectile&);
    ~CTaskSimpleThrowProjectile() override;

    eTaskType   GetTaskType() const override { return Type; } // 0x61F6F0
    CTask*      Clone() const override { return new CTaskSimpleThrowProjectile{ *this }; } // 0x623030
    bool        MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool        ProcessPed(CPed* ped) override;

    bool        ControlThrow(bool bButtonReleased, CEntity* entity, const CVector* pos);

    static void FinishAnimThrowProjectileCB(CAnimBlendAssociation* anim, void* data);
    void        StartAnim(CPed* ped);

public:
    bool                   m_bIsFinished{};
    bool                   m_bStartThrowFinished{};
    bool                   m_bButtonReleased{};
    CAnimBlendAssociation* m_Anim{};
    CEntity*               m_TargetEntity{};
    CVector                m_TargetPos{};
    uint32                 m_ButtonCounter{};

private:
    // 0x61F660
    CTaskSimpleThrowProjectile* Constructor(CEntity * pEntity, CVector firingPoint) {
        this->CTaskSimpleThrowProjectile::CTaskSimpleThrowProjectile(pEntity, firingPoint);
        return this;
    }

    // 0x61F700
    CTaskSimpleThrowProjectile* Destructor() {
        this->CTaskSimpleThrowProjectile::~CTaskSimpleThrowProjectile();
        return this;
    }
};
VALIDATE_SIZE(CTaskSimpleThrowProjectile, 0x24);
