/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "CollisionData.h"
#include "ColModel.h"

class CAnimBlendAssociation;
class CEntity;
class CPed;
class CPlayerPed;

enum class eMeleeMove : int32  { // AKA eMeleeAttack
    ATTACK1 = 0,
    ATTACK2 = 1,
    ATTACK3 = 2,
    GROUND  = 3,
    MOVING  = 4,

    //
    // Add above
    //
    NUM
};
NOTSA_WENUM_DEFS_FOR(eMeleeMove);

enum class eMeleeCommand : int8 {
    NONE           = -1,
    IDLE           = 0,
    END_SLOW       = 1, //!< Used when `MakeAbortable` is called with `LEISURE` priority
    BLOCK          = 2,
    MOVE_FWD       = 3,
    MOVE_LEFT      = 4,
    MOVE_BACK      = 5,
    MOVE_RIGHT     = 6,
    SHUFFLE_FWD    = 7,
    SHUFFLE_LEFT   = 8,
    SHUFFLE_BACK   = 9,
    SHUFFLE_RIGHT  = 10,
    ATTACK_1       = 11,
    ATTACK_2       = 12,
    ATTACK_3       = 13,
    ATTACK_4       = 14,
    END_QUICK      = 15,
    END_RUNAWAY    = 16,
    END_SPRINTAWAY = 17,
    END_DUCK       = 18,
    STEALTH_KILL   = 19,
};
NOTSA_WENUM_DEFS_FOR(eMeleeCommand);

inline bool IsMeleeCommandAttack(eMeleeCommand cmd) {
    switch (cmd) {
    case eMeleeCommand::ATTACK_1:
    case eMeleeCommand::ATTACK_2:
    case eMeleeCommand::ATTACK_3:
    case eMeleeCommand::ATTACK_4:
        return true;
    }
    return false;

}

enum class eMeleeHitLevel : int32 {
    NONE        = -1,
    HIGH        = 0, // `H`
    LOW         = 1, // `L`
    GROUND      = 2, // `G`
    BEHIND      = 3, // `B`
    HIGH_LONG   = 4, // `HL`
    LOW_LONG    = 5, // `LL`
    GROUND_LONG = 6, // `GL`

    //
    // Add above
    //
    NUM
};
NOTSA_WENUM_DEFS_FOR(eMeleeHitLevel);

struct tMeleeComboSet {
    //! Stores data per melee move type
    template<typename T>
    using MeleeMoveInfo = std::array<T, +eMeleeMove::NUM>;

    AssocGroupId                    AnimGroup;
    float                           GroupRange;
    MeleeMoveInfo<float>            FireTime;
    MeleeMoveInfo<float>            ComboTime;
    MeleeMoveInfo<float>            Radius;
    float                           GroundLoopStart;
    float                           BlockLoopStart;
    float                           BlockLoopEnd;
    MeleeMoveInfo<eMeleeHitLevelS8> HitLevel;
    MeleeMoveInfo<uint8>            Damage;
    MeleeMoveInfo<eAudioEvents>     HitSound;
    MeleeMoveInfo<eAudioEvents>     AltHitSound;
    union {
        struct {
            // Moves available (?)
            bool Attack2 : 1; // 0x1
            bool Attack3 : 1; // 0x2
            bool Ground : 1;  // 0x4
            bool Running : 1; // 0x8

            // Ways to fall (?)
            bool Fall1 : 1;      // 0x10
            bool Fall2 : 1;      // 0x20
            bool Fall3 : 1;      // 0x40
            bool FallGround : 1; // 0x80
            bool FallRun : 1;    // 0x100

            // Other flags
            bool Block : 1;   // 0x200
            bool OwnIdle : 1; // 0x400
            bool : 1;         // 0x800
            bool NoFall1 : 1; // 0x1000
            bool NoFall2 : 1; // 0x2000
            bool NoFall3 : 1; // 0x4000
            bool : 1;         // 0x8000
        };
        uint16 Flags;
    };
};
VALIDATE_SIZE(tMeleeComboSet, 0x88);

class NOTSA_EXPORT_VTABLE CTaskSimpleFight final : public CTaskSimple {
public:
    static inline auto& m_aComboData      = StaticRef<std::array<tMeleeComboSet, +eMeleeCombo::NUM_SETS>>(0xC170D0);
    static inline auto& m_aHitOffset      = StaticRef<std::array<CVector, 7>>(0xC177D0);
    static inline auto& m_sStrikeColModel = StaticRef<CColModel>(0xC17824);
    static inline auto& m_sStrikeColData  = StaticRef<CCollisionData>(0xC17854);
    static inline auto& m_sStrikeSpheres  = StaticRef<std::array<CColSphere, 1>>(0xC17884);

public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_FIGHT;

    CTaskSimpleFight(CEntity* target, eMeleeCommandS32 firstCmd, uint32 idlePeriod = 10'000);
    CTaskSimpleFight(const CTaskSimpleFight&);
    ~CTaskSimpleFight() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskSimpleFight{*this}; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

    static void LoadMeleeData();

    bool BeHitWhileBlocking(CPed* ped, CPed* attacker, eMeleeCombo atkCombo, eMeleeMove atkMove);
    void ChooseAttackAI(CPed* ped);
    void ChooseAttackPlayer(CPed* ped);
    bool FindTargetOnGround(CPed* ped);

    bool          ControlFight(CEntity* entity, eMeleeCommandS8 command);
    void          FightHitObj(CPed* attacker, CObject* victim, CVector& hitPt, CVector& hitDelta, int16 hitPieceType, uint8 hitSurfaceType);
    void          FightHitCar(CPed* attacker, CVehicle* victim, CVector& hitPt, CVector& hitDelta, int16 hitPieceType, uint8 hitSurfaceType);
    CPed*         FightHitPed(CPed* attacker, CPed* victim, CVector& hitPt, CVector& hitDelta, int16 hitPieceType);
    eMeleeCommand FightStrike(CPed* ped, CVector& posn);
    void          FightSetUpCol(float radius);

    static auto& GetComboData(eMeleeCombo c) { return m_aComboData[MeleeCombo2MeleeDataIdx(c)]; }
    auto&        GetCurrentComboData() const { return GetComboData(m_ComboSet); }
    auto         GetCurrentComboSet() const { return m_ComboSet; }
    bool         IsComboSet() const;
    bool         IsHitComboSet() const;
    eMeleeCombo  GetAvailableComboSet(CPed* ped, eMeleeCommandS8 nextCmd);
    AssocGroupId GetComboAnimGroupID() const;

    static eMeleeCombo    GetComboType(const char* comboName);
    static eMeleeHitLevel GetHitLevel(const char*);
    static eAudioEvents   GetHitSound(int32);
    
    auto GetMove() const { return m_CurrentMove; }
    auto GetMoveAnimID() const { return (AnimationId)(+GetMove() + +ANIM_ID_FIGHT_HIT_1); }

    float GetRange() const;
    float GetStrikeDamage(CPed* ped);

    static void FinishMeleeAnimCB(CAnimBlendAssociation*, void*);
    void        SetPlayerMoveAnim(CPlayerPed* player);
    void        StartAnim(CPed* ped, int32);

private:
    void BlendOutIdleAnim(CPed* ped, float blendDelta);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    // 0x61C470
    CTaskSimpleFight* Constructor(CEntity * pTargetEntity, eMeleeCommandS32 nCommand, uint32 nIdlePeriod) {
        this->CTaskSimpleFight::CTaskSimpleFight(pTargetEntity, nCommand, nIdlePeriod);
        return this;
    }

    // 0x61C530
    CTaskSimpleFight* Destructor() {
        this->CTaskSimpleFight::~CTaskSimpleFight();
        return this;
    }

private:
    bool                   m_IsFinished{false};
    bool                   m_IsInControl{true};
    bool                   m_AreAnimsReferenced{false};
    AssocGroupId           m_RequiredAnimGroup{ANIM_GROUP_MELEE_1};
    uint16                 m_IdlePeriod{0};
    uint16                 m_IdleCounter{0};
    int8                   m_ContinueStrike{0};
    int8                   m_ChainCounter{0};
    notsa::EntityRef<>     m_Target{ nullptr };
    CAnimBlendAssociation* m_Anim{nullptr};
    CAnimBlendAssociation* m_IdleAnim{nullptr};
    eMeleeComboS8          m_ComboSet{eMeleeCombo::IDLE};
    eMeleeMoveS8           m_CurrentMove{eMeleeMove::ATTACK1};
    eMeleeCommandS8        m_NextCmd{eMeleeCommand::IDLE};
    eMeleeCommandS8        m_LastCmd{eMeleeCommand::IDLE};
};
VALIDATE_SIZE(CTaskSimpleFight, 0x28);
