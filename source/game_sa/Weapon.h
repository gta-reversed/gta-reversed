/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "eWeaponType.h"
#include "Vector2D.h"
#include "eWeaponSkill.h"

class FxSystem_c;
class CColPoint;

enum ePedPieceTypes;

enum eWeaponState : uint32 {
    WEAPONSTATE_READY = 0,
    WEAPONSTATE_FIRING,
    WEAPONSTATE_RELOADING,
    WEAPONSTATE_OUT_OF_AMMO,
    WEAPONSTATE_MELEE_MADECONTACT
};

/* Source: https://wiki.multitheftauto.com/wiki/GetPedWeaponSlot */
enum class eWeaponSlot : uint32 {
    UNARMED,
    MELEE,
    HANDGUN,
    SHOTGUN,
    SMG,        // Used for drive-by's
    RIFLE,
    SNIPER,
    HEAVY,
    THROWN,
    SPECIAL,
    GIFT,
    PARACHUTE,
    DETONATOR,
};
constexpr auto NUM_WEAPON_SLOTS = static_cast<size_t>(eWeaponSlot::DETONATOR) + 1u;

class CPed;
class CVehicle;
class CColModel;
class CWeaponInfo;

class CWeapon {
public:
    static inline auto& ms_fExtinguisherAimAngle = StaticRef<float>(0x8D610C); // default -0.34907 rad. (-pi/8)
    static inline auto& bPhotographHasBeenTaken  = StaticRef<bool>(0xC8A7C0);
    static inline auto& ms_bTakePhoto            = StaticRef<bool>(0xC8A7C1);
    static inline auto& ms_PelletTestCol         = StaticRef<CColModel>(0xC8A7DC);

    static inline struct DebugSettings {
        bool NoShotDelay;
    } s_DebugSettings;

public:
    CWeapon() = default;
    CWeapon(eWeaponType weaponType, uint32 ammo);
    CWeapon(const CWeapon&) = delete;

    void Initialise(eWeaponType weaponType, int32 ammo, CPed* owner);
    static void InitialiseWeapons();
    void Shutdown();
    static void ShutdownWeapons();

    void AddGunshell(CEntity* creator, CVector& position, const CVector2D& direction, float size);
    bool LaserScopeDot(CVector* outCoord, float* outSize);
    bool FireSniper(CPed* shooter, CEntity* victim, CVector* target);
    void Reload(CPed* owner = nullptr);

    bool IsTypeMelee();
    bool IsType2Handed();
    bool IsTypeProjectile();

    bool HasWeaponAmmoToBeUsed();
    void StopWeaponEffect();
    void DoBulletImpact(CEntity* owner, CEntity* victim, const CVector& startPoint, const CVector& endPoint, const CColPoint& colPoint, int32 arg5);
    /*!
    * @addr 0x73C1F0
    * @brief Marks all peds and objects that are in range (125 units) and in frame (on the screen - 0.1 relative border) as photographed.
    *
    * @param owner Camera owner - unused.
    * @param point Pos of the camflash effect
    */
    bool TakePhotograph(CEntity* owner, const CVector* point);
    void SetUpPelletCol(int32 numPellets, CEntity* owner, CEntity* victim, CVector& point, CColPoint& colPoint, CMatrix& outMatrix);
    bool CanBeUsedFor2Player();

    // outX and outY will be placed in [-1;1] ranges
    void DoWeaponEffect(CVector origin, CVector target);

    bool FireAreaEffect(CEntity* firingEntity, const CVector& origin, CEntity* targetEntity, CVector* target);
    bool FireInstantHitFromCar(CVehicle* vehicle, bool leftSide, bool rightSide);
    bool FireFromCar(CVehicle* vehicle, bool leftSide, bool rightSide);
    void FireInstantHitFromCar2(CVector startPoint, CVector endPoint, CVehicle* vehicle, CEntity* owner);
    bool FireInstantHit(CEntity* firingEntity, CVector* origin, CVector* muzzlePosn, CEntity* targetEntity = nullptr, CVector* target = nullptr, CVector* originForDriveBy = nullptr, bool arg6 = false, bool muzzle = false);
    bool FireProjectile(CEntity* firingEntity, const CVector& origin, CEntity* targetEntity = nullptr, const CVector* targetPos = nullptr, float force = 0.f);
    bool FireM16_1stPerson(CPed* owner);
    bool Fire(CEntity* firedBy, CVector* startPosn, CVector* barrelPosn, CEntity* targetEnt, CVector* targetPosn, CVector* altPosn);

    void Update(CPed* owner);
    static void UpdateWeapons();

    static bool GenerateDamageEvent(CPed* victim, CEntity* creator, eWeaponType weaponType, int32 damageFactor, ePedPieceTypes pedPiece, uint8 direction);
    static bool CanBeUsedFor2Player(eWeaponType weaponType);
    static float TargetWeaponRangeMultiplier(CEntity* victim, CEntity* weaponOwner);

    /*!
    * @addr 0x73CDC0
    * @brief Find closest entity in range that is visible to `owner` (Eg.: Is in [-PI/8, PI/8] angle) and modify `end->z` to be pointing at it. idk..
    *
    * @param end out Z axis is modified
    */
    static void DoDoomAiming(CEntity* owner, CVector* start, CVector* end);
    static void DoTankDoomAiming(CEntity* vehicle, CEntity* owner, CVector* startPoint, CVector* endPoint);
    static void DoDriveByAutoAiming(CEntity* owner, CVehicle* vehicle, CVector* startPoint, CVector* endPoint, bool canAimVehicles);
    static CEntity* FindNearestTargetEntityWithScreenCoors(float screenX, float screenY, float range, CVector point, float* outX, float* outY);
    static float EvaluateTargetForHeatSeekingMissile(CEntity* potentialTarget, const CVector& origin, const CVector& aimingDir, float tolerance, bool arePlanesPriority, CEntity* preferredExistingTarget);
    static bool CheckForShootingVehicleOccupant(CEntity** pCarEntity, CColPoint* colPoint, eWeaponType weaponType, const CVector& origin, const CVector& target);
    static CEntity* PickTargetForHeatSeekingMissile(CVector origin, CVector direction, float distanceMultiplier, CEntity* ignoreEntity, bool arePlanesPriority, CEntity* preferredExistingTarget);
    static bool ProcessLineOfSight(const CVector& startPoint, const CVector& endPoint, CColPoint& outColPoint, CEntity*& outEntity, eWeaponType weaponType, CEntity* arg5, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool arg11, bool doIgnoreCameraCheck);

    CWeaponInfo& GetWeaponInfo(CPed* owner = nullptr) const;
    CWeaponInfo& GetWeaponInfo(eWeaponSkill skill) const;

    auto GetType()          const noexcept { return m_Type; }
    auto GetState()         const noexcept { return m_State; }
    auto GetAmmoInClip()    const noexcept { return m_AmmoInClip; }
    auto GetTotalAmmo()     const noexcept { return m_TotalAmmo; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    //! @notsa
    //! @brief Get the projectile type of this weapon - Only valid for weapons that fire a projectile [like rlaunchers, etc], or are itself a projectile [ex.: grenades]
    auto GetProjectileType();

    //! @notsa
    CWeapon* Constructor(eWeaponType weaponType, uint32 ammo) {
        this->CWeapon::CWeapon(weaponType, ammo);
        return this;
    }

public: // TODO: Eventually make this private
    eWeaponType  m_Type{};                            //< Weapon's type
    eWeaponState m_State{};                           //< Current weapon state
    uint32       m_AmmoInClip{};                      //< Count of ammo in the clip currently
    uint32       m_TotalAmmo{};                       //< The total amount of ammo (Anything above 25k is considered infinite in case of player peds)
    uint32       m_TimeForNextShotMs{};               //< When the next shot is allowed to be fired
    bool         m_IsFirstPersonWeaponModeSelected{}; //< Fuck knows, unused
    bool         m_DontPlaceInHand{};                 //< Used in case of goggles (infrared/nightvision) : When they're put on the weapon model isn't [and shouldn't be] loaded.
    FxSystem_c*  m_FxSystem{};                        //< Fx system [flamethrower, spraycan, extinguisher]
};

VALIDATE_SIZE(CWeapon, 0x1C);

void FireOneInstantHitRound(const CVector& startPoint, const CVector& endPoint, int32 intensity);
