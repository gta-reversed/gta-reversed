#pragma once

#include "PedDamageResponse.h"
#include "Entity.h"
#include "Enums\eWeaponType.h"
#include "Enums\eEventType.h"
#include "Event.h"

enum ePedPieceTypes;
class CPedDamageResponseCalculator;

class NOTSA_EXPORT_VTABLE CEventDamage : public CEventEditableResponse {
public:
    CEntity*           m_pSourceEntity;
    uint32             m_nStartTime;
    eWeaponType        m_weaponType;
    ePedPieceTypes     m_pedPieceType;
    uint8              m_ucDirection; // See `eFallDir`
    uint8              m_bJumpedOutOfMovingCar : 1;
    uint8              m_bFallDown : 1; // If true, the victim falls on the ground when damaged
    uint8              m_bAnimAdded : 1;
    uint8              m_bWitnessedInVehicle : 1;
    uint8              m_bStealthMode : 1;
    bool               m_unk26;
    bool               m_unk27;
    AssocGroupId       m_nAnimGroup;
    AnimationId        m_nAnimID;
    float              m_fAnimBlend;
    float              m_fAnimSpeed;
    CPedDamageResponse m_damageResponse;

public:
    static constexpr auto Type = eEventType::EVENT_DAMAGE;

    CEventDamage(const CEventDamage& event);
    CEventDamage(CEntity* source, uint32 startTime, eWeaponType weaponType, ePedPieceTypes pieceHit, uint8 direction, bool a7, bool bPedInVehicle);
    ~CEventDamage() override;

    eEventType GetEventType() const override { return EVENT_DAMAGE; } // 0x4AD910;
    int32 GetEventPriority() const override { return 65; } // 0x4AD950;
    int32 GetLifeTime() override { return 0; } // 0x4AD920;
    bool AffectsPed(CPed* ped) override;
    bool AffectsPedGroup(CPedGroup* pedGroup) override;
    bool IsCriminalEvent() override;
    void ReportCriminalEvent(CPed* ped) override;
    CEntity* GetSourceEntity() const override;
    bool TakesPriorityOver(const CEvent& refEvent) override;
    float GetLocalSoundLevel() override { return 55.0f; } // 0x4AD930;
    bool DoInformVehicleOccupants(CPed* ped) override;
    bool CanBeInterruptedBySameEvent() override { return true; } // 0x4AD940;
    CEventEditableResponse* CloneEditable() override;

    void From(const CEventDamage& event);
    void ProcessDamage(CPed* ped);
    void ComputeBodyPartToRemove(int32& boneFrameId);
    void ComputeDeathAnim(CPed* ped, bool bMakeActiveTaskAbortable);
    void ComputeDamageAnim(CPed* ped, bool bMakeActiveTaskAbortable);
    void ComputeAnim(CPed* ped, bool bMakeActiveTaskAbortable = true);

    //! Either computes the damage, or sets it as computed (Without computing it) - Very common logic in the code
    void ComputeDamageResponseIfAffectsPed(CPed* ped, CPedDamageResponseCalculator calculator, bool bSpeak);

    //! @addr 0x4ADBF0
    bool IsSameEventForAI(const CEventDamage& o) const { return o.m_pSourceEntity == m_pSourceEntity && o.m_weaponType == m_weaponType; }

    //! @addr 0x4ABCA0
    bool HasKilledPed() const { return m_damageResponse.m_bHealthZero && m_bAddToEventGroup; }

    //! @notsa
    bool WasHeadShot() const; // Actually SA. Originally called `ComputeHeadShot` [which also took a `bool&` instead returning the result directly]

    bool GetAnimAdded() const { return m_bAnimAdded; }
    auto GetAnimId() const { return m_nAnimID; }
    auto GetAnimGroup() const { return m_nAnimGroup; }

    auto& GetDamageResponse() const { return m_damageResponse; }
private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventDamage* Constructor(const CEventDamage& event);
    CEventDamage* Constructor(CEntity * source, uint32 startTime, eWeaponType weaponType, ePedPieceTypes pieceHit, uint8 direction, bool a7, bool bPedInVehicle);

                };

VALIDATE_SIZE(CEventDamage, 0x44);
