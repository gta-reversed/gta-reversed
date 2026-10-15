# GTA San Andreas Reversing & Modernization Knowledge Base

This monolith info file acts as a central knowledge base for the modern C++ reversal and modernization project of GTA San Andreas. It has been aggregated and formatted directly from the reference file `tricks.txt`. 

AI agents and contributors should use this document to understand engine quirks, modern C++ equivalents of legacy code, correct memory addresses, and properly reversed enumerations and structures.

---

## 1. Code Simplifications & Modern C++ Idioms

When reverse-engineering and rewriting functions, use these improved, modernized equivalents to clean up the codebase.

### String Comparisons
Use `std::string_view` instead of legacy C-string functions:
*   **Exact Match:** `strcmp(v1, v2) == 0` $\rightarrow$ `std::string_view{v1} == v2`
*   **Prefix Match:** `strncmp(v1, v2, len) == 0` $\rightarrow$ `std::string_view{v1}.starts_with(v2)` *(Ensure the original `strncmp` was indeed being used for a prefix check).*

### RenderWare & Drawing States
Use proper macros for RenderWare states instead of casting raw pointers/integers:
**Bad:**
```cpp
RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)0);
RwRenderStateSet(rwRENDERSTATESRCBLEND,          (void*)rwBLENDSRCALPHA);
RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     nullptr);
```
**Good:**
```cpp
RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
```

**Texture Filtering Shortcut:**
```cpp
// Instead of setting wrap and linear filter mode explicitly via functions:
RwTextureSetAddressing(texture, rwTEXTUREADDRESSWRAP);
RwTextureSetFilterMode(texture, rwFILTERLINEAR);

// You can use the engine's internal representation directly:
LOWORD(this->filterAddressing) = 0x1102;
```

### Inlined Engine Logic
*   **Area Checks:** `!CGame::currArea` is an inlined equivalent of `CGame::CanSeeOutSideFromCurrArea()`.
*   **Entity States:** `m_bIsStatic || m_bIsStaticWaitingForCollision` is usually just `IsStatic()` inlined.
*   **Ped Models:** Checking if a ped has a valid model clump can be simplified:
    ```cpp
    // Legacy / Raw Reversal:
    clump = ped->object.m_pRuClump;
    if (!clump) return 0;
    if (clump->object.parent == (RuFrame *)-16) return 0;

    // Modern / Clean:
    if (!ped->GetModellingMatrix()) return false;
    ```
*   **Atomic Destruction:** 
    `RwFrameDestroy(m_pAtomic->object.object.parent);` is functionally identical to:
    ```cpp
    auto* frame = RpAtomicGetFrame(m_pAtomic);
    RpAtomicDestroy(m_pAtomic);
    RwFrameDestroy(frame);
    ```

---

## 2. Math & Float Equivalents

*   **Float Equality:** 
    `std::fabs(a) < 0.001f` $\Leftrightarrow$ `approxEqual(a, 0.0f, 0.001f)`
    `std::fabs(a - b) < 0.001f` $\Leftrightarrow$ `approxEqual(a, b, 0.001f)`
*   **Logarithms (x86 FPU):** 
    `__FYL2X__(x, 0.30102999566398119802)` is equivalent to `log10(x)`.

---

## 3. IDA Pro Quirks & Analysis Tips

*   **Modulo vs. Bitwise AND:** IDA often converts `& 255` to `LOBYTE(...)`. This can confuse developers when the mathematical intent of the original code was actually a modulo operation `% 256`. Keep this in mind when translating logic.
*   **FPU Comparisons (Floating Point Branching):**
    When IDA generates confusing FPU comparisons referencing flags like `c0` or `c3`:
    *   `st(0)` is the top of the FPU stack (the value loaded with `fld`).
    *   `source` is the operand given to `fcomp`.
    *   If you see `if (!(c0 | c3))`, it translates to a check evaluating if `st(0) < source`.

---

## 4. Addresses, Fixes & Hook Corrections

*   **Safehouse Cars:** The address for `aCarsInSafeHouse` is historically mapped incorrectly as `0x96ABD4`. **The correct address is `0x96ABD8`.**
*   **Radar Trace Colors:** `CRadar::ShowRadarTraceWithHeight` (`0x584070`) uses 4 bytes per channel for RGBA (i.e., `uint32 r, uint32 g, uint32 b, uint32 a`), *not* a standard 1-byte-per-channel struct.
*   **CPed Operator New:** Change the signature of `0x5E4730` in your database from taking one argument to: 
    `CPed *__cdecl CPed__operator_new(unsigned int size, int32 poolRef)`
*   **Beat Info Struct:** The audio hardware struct arrays should use `tBeatInfo`, not `tBeatInfo2` (which is improperly reversed despite matching fields).

---

## 5. Extracted Enums

### Entities & Physics
```cpp
enum eEntityStatusShifted : uint8 {
    SHIFTED_STATUS_PLAYER = 0 << 3,
    SHIFTED_STATUS_PLAYER_PLAYBACK_FROM_BUFFER = 1 << 3,
    SHIFTED_STATUS_SIMPLE = 2 << 3,
    SHIFTED_STATUS_PHYSICS = 3 << 3,
    SHIFTED_STATUS_ABANDONED = 4 << 3,
    SHIFTED_STATUS_WRECKED = 5 << 3,
    SHIFTED_STATUS_TRAIN_MOVING = 6 << 3,
    SHIFTED_STATUS_TRAIN_NOT_MOVING = 7 << 3,
    SHIFTED_STATUS_REMOTE_CONTROLLED = 8 << 3,
    SHIFTED_STATUS_FORCED_STOP = 9 << 3,
    SHIFTED_STATUS_IS_TOWED = 10 << 3,
    SHIFTED_STATUS_IS_SIMPLE_TOWED = 11 << 3,
    SHIFTED_STATUS_GHOS = 12 << 3,
};

enum eEntityFlags {
  ENTITY_m_bUsesCollision = 0x1,
  ENTITY_m_bCollisionProcessed = 0x2,
  ENTITY_m_bIsStatic = 0x4,
  ENTITY_m_bHasContacted = 0x8,
  ENTITY_m_bIsStuck = 0x10,
  ENTITY_m_bIsInSafePosition = 0x20,
  ENTITY_m_bWasPostponed = 0x40,
  ENTITY_m_bIsVisible = 0x80,
  ENTITY_m_bIsBIGBuilding = 0x100,
  ENTITY_m_bRenderDamaged = 0x200,
  ENTITY_m_bStreamingDontDelete = 0x400,
  ENTITY_m_bRemoveFromWorld = 0x800,
  ENTITY_m_bHasHitWall = 0x1000,
  ENTITY_m_bImBeingRendered = 0x2000,
  ENTITY_m_bDrawLast = 0x4000,
  ENTITY_m_bDistanceFade = 0x8000,
  ENTITY_m_bDontCastShadowsOn = 0x10000,
  ENTITY_m_bOffscreen = 0x20000,
  ENTITY_m_bIsStaticWaitingForCollision = 0x40000,
  ENTITY_m_bDontStream = 0x80000,
  ENTITY_m_bUnderwater = 0x100000,
  ENTITY_m_bHasPreRenderEffects = 0x200000,
  ENTITY_m_bIsTempBuilding = 0x400000,
  ENTITY_m_bDontUpdateHierarchy = 0x800000,
  ENTITY_m_bHasRoadsignText = 0x1000000,
  ENTITY_m_bDisplayedSuperLowLOD = 0x2000000,
  ENTITY_m_bIsProcObject = 0x4000000,
  ENTITY_m_bBackfaceCulled = 0x8000000,
  ENTITY_m_bLightObject = 0x10000000,
  ENTITY_m_bUnimportantStream = 0x20000000,
  ENTITY_m_bTunnel = 0x40000000,
  ENTITY_m_bTunnelTransition = 0x80000000,
};

enum PhysicalFlags {
  PHYSICAL_bMakeMassTwiceAsBig = 0x1,
  PHYSICAL_bApplyGravity = 0x2,
  PHYSICAL_bDisableCollisionForce = 0x4,
  PHYSICAL_bCollidable = 0x8,
  PHYSICAL_bDisableTurnForce = 0x10,
  PHYSICAL_bDisableMoveForce = 0x20,
  PHYSICAL_bInfiniteMass = 0x40,
  PHYSICAL_bDisableZ = 0x80,
  PHYSICAL_bSubmergedInWater = 0x100,
  PHYSICAL_bOnSolidSurface = 0x200,
  PHYSICAL_bBroken = 0x400,
  PHYSICAL_bProcessCollisionEvenIfStationary = 0x800,
  PHYSICAL_b13 = 0x1000,
  PHYSICAL_bDontApplySpeed = 0x2000,
  PHYSICAL_b15 = 0x4000,
  PHYSICAL_bProcessingShift = 0x8000,
  PHYSICAL_b17 = 0x10000,
  PHYSICAL_bDisableSimpleCollision = 0x20000,
  PHYSICAL_bBulletProof = 0x40000,
  PHYSICAL_bFireProof = 0x80000,
  PHYSICAL_bCollisionProof = 0x100000,
  PHYSICAL_bMeleeProof = 0x200000,
  PHYSICAL_bInvulnerable = 0x400000,
  PHYSICAL_bExplosionProof = 0x800000,
  PHYSICAL_bDontCollideWithFlyers = 0x1000000,
  PHYSICAL_bAttachedToEntity = 0x2000000,
  PHYSICAL_bAddMovingCollisionSpeed = 0x4000000,
  PHYSICAL_bTouchingWater = 0x8000000,
  PHYSICAL_bCanBeCollidedWith = 0x10000000,
  PHYSICAL_bDestroyed = 0x20000000,
  PHYSICAL_b31 = 0x40000000,
  PHYSICAL_b32 = 0x80000000,
};
```

### Peds & Players
```cpp
enum ePlayerFlags {
  m_bStoppedMoving = 0x1,
  m_bAdrenaline = 0x2,
  m_bHaveTargetSelected = 0x4,
  m_bFreeAiming = 0x8,
  m_bCanBeDamaged = 0x10,
  m_bAllMeleeAttackPtsBlocked = 0x20,
  m_bJustBeenSnacking = 0x40,
  m_bRequireHandleBreath = 0x80,
  m_bGroupStuffDisabled = 0x100,
  m_bGroupAlwaysFollow = 0x200,
  m_bGroupNeverFollow = 0x400,
  m_bInVehicleDontAllowWeaponChange = 0x800,
  m_bRenderWeapon = 0x1000,
};

enum ePedIKFlags {
  PED_IK_bGunReachedTarget = 0x1,
  PED_IK_bTorsoUsed = 0x2,
  PED_IK_bUseArm = 0x4,
  PED_IK_bSlopePitch = 0x8,
  PED_IK_bUnk = 0x10,
};

enum ePedFlags1 {
  PED_FLAGS_1_bIsStanding = 0x1,
  PED_FLAGS_1_bWasStanding = 0x2,
  PED_FLAGS_1_bIsLooking = 0x4,
  PED_FLAGS_1_bIsRestoringLook = 0x8,
  PED_FLAGS_1_bIsAimingGun = 0x10,
  PED_FLAGS_1_bIsRestoringGun = 0x20,
  PED_FLAGS_1_bCanPointGunAtTarget = 0x40,
  PED_FLAGS_1_bIsTalking = 0x80,
  PED_FLAGS_1_bInVehicle = 0x100,
  PED_FLAGS_1_bIsInTheAir = 0x200,
  PED_FLAGS_1_bIsLanding = 0x400,
  PED_FLAGS_1_bHitSomethingLastFrame = 0x800,
  PED_FLAGS_1_bIsNearCar = 0x1000,
  PED_FLAGS_1_bRenderPedInCar = 0x2000,
  PED_FLAGS_1_bUpdateAnimHeading = 0x4000,
  PED_FLAGS_1_bRemoveHead = 0x8000,
  PED_FLAGS_1_bFiringWeapon = 0x10000,
  PED_FLAGS_1_bHasACamera = 0x20000,
  PED_FLAGS_1_bPedIsBleeding = 0x40000,
  PED_FLAGS_1_bStopAndShoot = 0x80000,
  PED_FLAGS_1_bIsPedDieAnimPlaying = 0x100000,
  PED_FLAGS_1_bStayInSamePlace = 0x200000,
  PED_FLAGS_1_bKindaStayInSamePlace = 0x400000,
  PED_FLAGS_1_bBeingChasedByPolice = 0x800000,
  PED_FLAGS_1_bNotAllowedToDuck = 0x1000000,
  PED_FLAGS_1_bCrouchWhenShooting = 0x2000000,
  PED_FLAGS_1_bIsDucking = 0x4000000,
  PED_FLAGS_1_bGetUpAnimStarted = 0x8000000,
  PED_FLAGS_1_bDoBloodyFootprints = 0x10000000,
  PED_FLAGS_1_bDontDragMeOutCar = 0x20000000,
  PED_FLAGS_1_bStillOnValidPoly = 0x40000000,
  PED_FLAGS_1_bAllowMedicsToReviveMe = 0x80000000,
};

enum ePedFlags2 {
  PED_FLAGS_2_bResetWalkAnims = 0x1,
  PED_FLAGS_2_bOnBoat = 0x2,
  PED_FLAGS_2_bBusJacked = 0x4,
  PED_FLAGS_2_bFadeOut = 0x8,
  PED_FLAGS_2_bKnockedUpIntoAir = 0x10,
  PED_FLAGS_2_bHitSteepSlope = 0x20,
  PED_FLAGS_2_bCullExtraFarAway = 0x40,
  PED_FLAGS_2_bTryingToReachDryLand = 0x80,
  PED_FLAGS_2_bCollidedWithMyVehicle = 0x100,
  PED_FLAGS_2_bRichFromMugging = 0x200,
  PED_FLAGS_2_bChrisCriminal = 0x400,
  PED_FLAGS_2_bShakeFist = 0x800,
  PED_FLAGS_2_bNoCriticalHits = 0x1000,
  PED_FLAGS_2_bHasAlreadyBeenRecorded = 0x2000,
  PED_FLAGS_2_bUpdateMatricesRequired = 0x4000,
  PED_FLAGS_2_bFleeWhenStanding = 0x8000,
  PED_FLAGS_2_bMiamiViceCop = 0x10000,
  PED_FLAGS_2_bMoneyHasBeenGivenByScript = 0x20000,
  PED_FLAGS_2_bHasBeenPhotographed = 0x40000,
  PED_FLAGS_2_bIsDrowning = 0x80000,
  PED_FLAGS_2_bDrownsInWater = 0x100000,
  PED_FLAGS_2_bHeadStuckInCollision = 0x200000,
  PED_FLAGS_2_bDeadPedInFrontOfCar = 0x400000,
  PED_FLAGS_2_bStayInCarOnJack = 0x800000,
  PED_FLAGS_2_bDontFight = 0x1000000,
  PED_FLAGS_2_bDoomAim = 0x2000000,
  PED_FLAGS_2_bCanBeShotInVehicle = 0x4000000,
  PED_FLAGS_2_bPushedAlongByCar = 0x8000000,
  PED_FLAGS_2_bNeverEverTargetThisPed = 0x10000000,
  PED_FLAGS_2_bThisPedIsATargetPriority = 0x20000000,
  PED_FLAGS_2_bCrouchWhenScared = 0x40000000,
  PED_FLAGS_2_bKnockedOffBike = 0x80000000,
};

enum ePedFlags3 {
  PED_FLAGS_3_bDonePositionOutOfCollision = 0x1,
  PED_FLAGS_3_bDontRender = 0x2,
  PED_FLAGS_3_bHasBeenAddedToPopulation = 0x4,
  PED_FLAGS_3_bHasJustLeftCar = 0x8,
  PED_FLAGS_3_bIsInDisguise = 0x10,
  PED_FLAGS_3_bDoesntListenToPlayerGroupCommands = 0x20,
  PED_FLAGS_3_bIsBeingArrested = 0x40,
  PED_FLAGS_3_bHasJustSoughtCover = 0x80,
  PED_FLAGS_3_bKilledByStealth = 0x100,
  PED_FLAGS_3_bDoesntDropWeaponsWhenDead = 0x200,
  PED_FLAGS_3_bCalledPreRender = 0x400,
  PED_FLAGS_3_bBloodPuddleCreated = 0x800,
  PED_FLAGS_3_bPartOfAttackWave = 0x1000,
  PED_FLAGS_3_bClearRadarBlipOnDeath = 0x2000,
  PED_FLAGS_3_bNeverLeavesGroup = 0x4000,
  PED_FLAGS_3_bTestForBlockedPositions = 0x8000,
  PED_FLAGS_3_bRightArmBlocked = 0x10000,
  PED_FLAGS_3_bLeftArmBlocked = 0x20000,
  PED_FLAGS_3_bDuckRightArmBlocked = 0x40000,
  PED_FLAGS_3_bMidriffBlockedForJump = 0x80000,
  PED_FLAGS_3_bFallenDown = 0x100000,
  PED_FLAGS_3_bUseAttractorInstantly = 0x200000,
  PED_FLAGS_3_bDontAcceptIKLookAts = 0x400000,
  PED_FLAGS_3_bHasAScriptBrain = 0x800000,
  PED_FLAGS_3_bWaitingForScriptBrainToLoad = 0x1000000,
  PED_FLAGS_3_bHasGroupDriveTask = 0x2000000,
  PED_FLAGS_3_bCanExitCar = 0x4000000,
  PED_FLAGS_3_CantBeKnockedOffBike = 0x8000000,
  PED_FLAGS_3_bHasBeenRendered = 0x10000000,
  PED_FLAGS_3_bIsCached = 0x20000000,
  PED_FLAGS_3_bPushOtherPeds = 0x40000000,
  PED_FLAGS_3_bPedThirdFlags32 = 0x80000000,
};

enum ePedFlags4 {
  PED_FLAGS_4_bHasBulletProofVest = 0x1,
  PED_FLAGS_4_bUsingMobilePhone = 0x2,
  PED_FLAGS_4_bUpperBodyDamageAnimsOnly = 0x4,
  PED_FLAGS_4_bStuckUnderCar = 0x8,
  PED_FLAGS_4_bKeepTasksAfterCleanUp = 0x10,
  PED_FLAGS_4_bIsDyingStuck = 0x20,
  PED_FLAGS_4_bIgnoreHeightCheckOnGotoPointTask = 0x40,
  PED_FLAGS_4_bForceDieInCar = 0x80,
  PED_FLAGS_4_bCheckColAboveHead = 0x100,
  PED_FLAGS_4_bIgnoreWeaponRange = 0x200,
  PED_FLAGS_4_bDruggedUp = 0x400,
  PED_FLAGS_4_bWantedByPolice = 0x800,
  PED_FLAGS_4_bSignalAfterKill = 0x1000,
  PED_FLAGS_4_bCanClimbOntoBoat = 0x2000,
  PED_FLAGS_4_bPedHitWallLastFrame = 0x4000,
  PED_FLAGS_4_bIgnoreHeightDifferenceFollowingNodes = 0x8000,
  PED_FLAGS_4_bMoveAnimSpeedHasBeenSetByTask = 0x10000,
  PED_FLAGS_4_bGetOutUpsideDownCar = 0x20000,
  PED_FLAGS_4_bJustGotOffTrain = 0x40000,
  PED_FLAGS_4_bDeathPickupsPersist = 0x80000,
  PED_FLAGS_4_bTestForShotInVehicle = 0x100000,
  PED_FLAGS_4_bUsedForReplay = 0x200000,
};
```

### Vehicles & Tasks
```cpp
enum eAutomobileFlags {
  AUTOM_FLAG_bTaxiLight = 1 << 0,
  AUTOM_FLAG_bShouldNotChangeColour = 1 << 1,
  AUTOM_FLAG_bWaterTight = 1 << 2,
  AUTOM_FLAG_bDoesNotGetDamagedUpsideDown = 1 << 3,
  AUTOM_FLAG_bCanBeVisiblyDamaged = 1 << 4,
  AUTOM_FLAG_bTankExplodesCars = 1 << 5,
  AUTOM_FLAG_bIsBoggedDownInSand = 1 << 6,
  AUTOM_FLAG_bIsMonsterTruck = 1 << 7
};
typedef eAutomobileFlags eAutomobileNPCFlags; // AKA NPC flags

// In DWARF android this is represented as a bitfield:
struct CAutomobileFlags {
  UInt8 bTaxiLight : 1;
  UInt8 bShouldNotChangeColour : 1;
  UInt8 bWaterTight : 1;
  UInt8 bDoesNotGetDamagedUpsideDown : 1;
  UInt8 bCanBeVisiblyDamaged : 1;
  UInt8 bTankExplodesCars : 1;
  UInt8 bIsBoggedDownInSand : 1;
  UInt8 bIsMonsterTruck : 1;
};

enum eBikeFlags {
  BIKEFLAG_bShouldNotChangeColour = 0x1,
  BIKEFLAG_bPanelsAreThougher = 0x2,
  BIKEFLAG_bWaterTight = 0x4,
  BIKEFLAG_bGettingPickedUp = 0x8,
  BIKEFLAG_bOnSideStand = 0x10,
  BIKEFLAG_bPlayerBoost = 0x20,
  BIKEFLAG_bEngineOnFire = 0x40,
  BIKEFLAG_bWheelieForCamera = 0x80,
};

enum VehAnimGroup {
  VEH_ANIM_GROUP_STDCARAMIMS = 0x0,
  VEH_ANIM_GROUP_LOWCARAMIMS = 0x1,
  VEH_ANIM_GROUP_TRKCARANIMS = 0x2,
  VEH_ANIM_GROUP_STDBIKEANIMS = 0x3,
  VEH_ANIM_GROUP_SPORTBIKEANIMS = 0x4,
  VEH_ANIM_GROUP_VESPABIKEANIMS = 0x5,
  VEH_ANIM_GROUP_HARLEYBIKEANIMS = 0x6,
  VEH_ANIM_GROUP_DIRTBIKEANIMS = 0x7,
  VEH_ANIM_GROUP_WAYFBIKEANIMS = 0x8,
  VEH_ANIM_GROUP_BMXBIKEANIMS = 0x9,
  VEH_ANIM_GROUP_MTBBIKEANIMS = 0xA,
  VEH_ANIM_GROUP_CHOPPABIKEANIMS = 0xB,
  VEH_ANIM_GROUP_QUADBIKEANIMS = 0xC,
  VEH_ANIM_GROUP_VANCARANIMS = 0xD,
  VEH_ANIM_GROUP_RUSTPLANEANIMS = 0xE,
  VEH_ANIM_GROUP_COACHCARANIMS = 0xF,
  VEH_ANIM_GROUP_BUSCARANIMS = 0x10,
  VEH_ANIM_GROUP_DOZERCARANIMS = 0x11,
  VEH_ANIM_GROUP_KARTCARANIMS = 0x12,
  VEH_ANIM_GROUP_CONVCARANIMS = 0x13,
  VEH_ANIM_GROUP_MTRKCARANIMS = 0x14,
  VEH_ANIM_GROUP_TRAINCARRANIMS = 0x15,
}; // Used for tHandlingData::m_nAnimGroup

enum eCarMission {
  MISSION_NONE = 0x0,
  MISSION_CRUISE = 0x1,
  // ... (Full listing truncated for brevity, see reference file for up to 0x44)
  MISSION_ESCORT_FRONT_FARAWAY = 0x44,
};

enum TaskComplexEnterCar_Flags {
    TaskComplexEnterCar_bAsDriver = 1 << 0,
    TaskComplexEnterCar_bQuitAfterOpeningDoor = 1 << 1,
    TaskComplexEnterCar_bQuitAfterDraggingPedOut = 1 << 2,
    TaskComplexEnterCar_bCarryOnAfterFallingOff = 1 << 3,
}; 
// NOTE: Some of these bits (likely OpeningDoor vs DraggingPedOut) are known to be swapped in original behavior.

enum eTaskComplexGangFollowerFlags {
  TC_GANG_FOLLOWER_m_animsReferenced = 1 << 0,
  TC_GANG_FOLLOWER_m_leaveGroup = 1 << 1,
  TC_GANG_FOLLOWER_m_followLeader = 1 << 2 ,
  TC_GANG_FOLLOWER_m_inPlayersGroup = 1 << 3,
  TC_GANG_FOLLOWER_m_usingStandingStillOffsets = 1 << 4,
};
```

### Weapons & Melee Combat
```cpp
enum eWeaponInfoFlag {
    WEPINFO_CAN_AIM          = 0x1, // Or WI_CAN_AIM
    WEPINFO_AIM_WITH_ARM     = 0x2,
    WEPINFO_1ST_PERSON       = 0x4,
    WEPINFO_ONLY_FREEAIM     = 0x8,
    WEPINFO_MOVE_AIM         = 0x10,
    WEPINFO_MOVE_FIRE        = 0x20,
    WEPINFO_UNK1             = 0x40,
    WEPINFO_UNK2             = 0x80,
    WEPINFO_THROW            = 0x100,
    WEPINFO_HEAVY            = 0x200,
    WEPINFO_CONTINUOUS_FIRE  = 0x400,
    WEPINFO_TWIN_PISTOL      = 0x800,
    WEPINFO_RELOAD           = 0x1000,
    WEPINFO_CROUCH_FIRE      = 0x2000,
    WEPINFO_RELOAD2START     = 0x4000,
    WEPINFO_LONG_RELOAD      = 0x8000,
    WEPINFO_SLOWDOWN         = 0x10000,
    WEPINFO_RAND_SPEED       = 0x20000,
    WEPINFO_EXPANDS          = 0x40000,
};

enum eMeleeCommand : int8 {
    MCMD_IDLE = 0, MCMD_END_SLOW = 1, MCMD_BLOCK = 2, MCMD_MOVE_FWD = 3,
    MCMD_MOVE_LEFT = 4, MCMD_MOVE_BACK = 5, MCMD_MOVE_RIGHT = 6,
    MCMD_SHUFFLE_FWD = 7, MCMD_SHUFFLE_LEFT = 8, MCMD_SHUFFLE_BACK = 9,
    MCMD_SHUFFLE_RIGHT = 10, MCMD_ATTACK_1 = 11, MCMD_ATTACK_2 = 12,
    MCMD_ATTACK_3 = 13, MCMD_ATTACK_4 = 14, MCMD_END_QUICK = 15,
    MCMD_END_RUNAWAY = 16, MCMD_END_SPRINTAWAY = 17, MCMD_END_DUCK = 18,
    MCMD_STEALTH_KILL = 19,
};

enum eMeleeAttack : int8 {
    MATK_ATTACK1 = 0, MATK_ATTACK2 = 1, MATK_ATTACK3 = 2,
    MATK_GROUND = 3, MATK_MOVING = 4, MATK_BLOCK = 5
};

enum eMeleeHitLevel {
    MHIT_LEVEL_HIGH = 0, MHIT_LEVEL_LOW = 1, MHIT_LEVEL_GROUND = 2,
    MHIT_LEVEL_BEHIND = 3, MHIT_LEVEL_HIGH_LONG = 4, MHIT_LEVEL_LOW_LONG = 5,
    MHIT_LEVEL_GROUND_LONG = 6, MHIT_LEVEL_NUM = 7,
};

enum eMeleeCombo : int32 {
    MCOMBO_IDLE = 0, MCOMBO_MOVE = 1, MCOMBO_BLOCK = 2, MCOMBO_END = 3,
    MCOMBO_UNARMED_1 = 4, MCOMBO_UNARMED_2 = 5, MCOMBO_UNARMED_3 = 6,
    MCOMBO_UNARMED_4 = 7, MCOMBO_BBALLBAT = 8, MCOMBO_KNIFE = 9,
    MCOMBO_GOLFCLUB = 10, MCOMBO_SWORD = 11, MCOMBO_CHAINSAW = 12,
    MCOMBO_DILDO = 13, MCOMBO_FLOWERS = 14, MCOMBO_KICK_STD = 15,
    MCOMBO_PISTOL_WHIP = 16, MCOMBO_NUM = 17,
};
typedef int8 eMeleeComboS8;
typedef int32 eMeleeComboS32;
```

### Environment & Audio
```cpp
enum eGrgFlags {
    GRGE_FLG_m_b0x1 = 1 << 0,
    GRGE_FLG_m_bInactive = 1 << 1,
    GRGE_FLG_m_bUsedRespray = 1 << 2,
    GRGE_FLG_m_bDoorOpensUp = 1 << 3,
    GRGE_FLG_m_bDoorGoesIn = 1 << 4,
    GRGE_FLG_m_bCameraFollowsPlayer = 1 << 5,
    GRGE_FLG_m_bDoorClosed = 1 << 6,
    GRGE_FLG_m_bRespraysAlwaysFree = 1 << 7,
};

enum eSpeechSlotState : __int32 {
  SPEECH_SLOT_FREE = 0x0,
  SPEECH_SLOT_LOADING = 0x1,
  SPEECH_SLOT_WAITING = 0x2,
  SPEECH_SLOT_REQUESTED = 0x3,
  SPEECH_SLOT_RESERVED = 0x4,
  SPEECH_SLOT_PLAYING = 0x5,
};

enum GlobalSpeechContexts : __int32 {
  CONTEXT_GLOBAL_NO_SPEECH = 0x0,
  CONTEXT_GLOBAL_ABUSE_GANG_BALLAS = 0x1,
  CONTEXT_GLOBAL_ABUSE_GANG_LSV = 0x2,
  CONTEXT_GLOBAL_ABUSE_GANG_VLA = 0x3,
  // ...
};
```

---

## 6. Structural Signatures & Classes

```cpp
union tScriptParam {
  uint8 u8Param;
  int8 i8Param;
  uint16 u16Param;
  int16 i16Param;
  uint32 uParam;
  int32 iParam;
  float fParam;
  void *pParam;
  char *szParam;
  bool bParam;
};

struct TInterestingEvent {
  uint32 Type;
  int32 Time;
  CEntity *Entity;
};

struct CInterestingEvents {
  TInterestingEvent m_Events[8];
  uint8 m_ChancesXrandom[30];
  uint16 m_Times[32];
  _DWORD dwordB8;
  int32 gap[26];
  uint8 m_Flags;
  int32 m_FrameCounter;
  uint32 m_Time;
  float f138;
  CVector m_vec13C;
  CVector m_vec148;
  int8 m_CurrentEventIndex;
};

struct CAnimBlendAssocGroup {
    CAnimBlock*                  m_AnimBlock;
    CAnimBlendStaticAssociation* m_Anims;
    uint32                       m_NumAnims;
    uint32                       m_IdOffset;
    AssocGroupId                 m_GroupID;
};

struct CAnimBlendStaticAssociation {
    void*                vfptr;
    uint16               m_NumBlendNodes;
    int16                m_AnimId;
    int16                m_AnimGroupId;
    uint16               m_Flags;
    CAnimBlendSequence** m_BlendSeqs;
    CAnimBlendHierarchy* m_BlendHier;
};

struct tMeleeComboSet {
    AssocGroupId AnimGroup;
    float        GroupRange;
    float        FireTime[5];
    float        ComboTime[5];
    float        Radius[5];
    float        GroundLoopStart;
    float        BlockLoopStart;
    float        BlockLoopEnd;
    uint8        HitLevel[5];
    uint8        Damage[5];
    tAudioEvent  HitSound[5];
    tAudioEvent  AltHitSound[5];
    uint16       Flags;
};

class CTaskSimpleFight : CTaskSimple {
    bool                   m_IsFinished;
    bool                   m_IsInControl;
    bool                   m_AreAnimsReferenced;
    AssocGroupId           m_RequiredAnimGroup;
    uint16                 m_IdlePeriod;
    uint16                 m_IdleCounter;
    int8                   m_ContinueStrike;
    int8                   m_ChainCounter;
    CEntity*               m_Target;
    CAnimBlendAssociation* m_Anim;
    CAnimBlendAssociation* m_IdleAnim;
    int8                   m_ComboSet;
    eMeleeHitLevel         m_CurrentMove;
    eMeleeCommand          m_NextCmd;
    eMeleeCommand          m_LastCmd;
};

class CAEPedSpeechAudioEntity : public CAEAudioEntity {
public:
    CAESound*               m_Sounds[5];
    bool                    m_IsInitialized;
    eAudioPedType           m_PedAudioType;
    int16                   m_VoiceID;
    int16                   m_IsFemale;
    bool                    m_IsPlayingSpeech;
    bool                    m_IsSpeechDisabled;
    bool                    m_IsSpeechForScriptsDisabled;
    bool                    m_IsFrontend;
    bool                    m_IsForcedAudible;
    CAESound*               m_Sound;
    int16                   m_nSoundId;
    int16                   m_BankID;
    int16                   m_PedSpeechSlotID;
    float                   m_EventVolume;
    int16                   m_LastGCtx;
    uint32                  m_NextTimeCanSayPain[19]; 
};

struct CCoverPoint {
    CCoverPoint__Type  m_Type;
    CCoverPoint__Usage m_Usage;
    char               m_CoverDir;
    CVector            m_Pos;
    CEntity*           m_CoverEntity;
    CPed*              m_CoveredPeds[2];
};