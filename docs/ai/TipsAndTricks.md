# Tips and tricks

Engine quirks, IDA idioms, known wrong addresses and recovered types of GTA:SA. Related pages:
- `.claude/skills/ida-to-cpp/SKILL.md`: the full pseudocode → C++ translation table
- [../ReversingRules.md](../ReversingRules.md): the rules R1-R20 (fidelity, hooks, layout)
- [CaseStudies.md](CaseStudies.md): real reversal mistakes and their lessons
- [Enums.md](Enums.md): members of every IDB enum the repo doesn't have yet (generated)

---

## 1. C++ equivalents of common patterns

### Strings
- `strcmp(a, b) == 0` → `std::string_view{a} == b`
- `strncmp(a, b, len) == 0` → `std::string_view{a}.starts_with(b)`, but only when `len == strlen(b)`, ie. the
  original really checks a prefix.

### RenderWare
Wrap render state values in `RWRSTATE` (`source/game_sa/RenderWare/RenderWare.h`) instead of casting:
```cpp
RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);              // pseudocode
RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));           // ours
RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
```
`contrib/clean-srs-calls.py` does this in bulk.

`LOWORD(tex->filterAddressing) = 0x1102` is the inlined form of:
```cpp
RwTextureSetAddressing(tex, rwTEXTUREADDRESSWRAP); // 0x1100: U wrap (bits 8-11) and V wrap (bits 12-15)
RwTextureSetFilterMode(tex, rwFILTERLINEAR);       // 0x0002: filter mode (bits 0-7)
```
Prefer the readable calls.

`atomic->object.object.parent` is `RpAtomicGetFrame(atomic)`.

### Inlined engine helpers
| Pseudocode | C++ |
|---|---|
| `!CGame::currArea` | `CGame::CanSeeOutSideFromCurrArea()` |
| `m_bIsStatic \|\| m_bIsStaticWaitingForCollision` | `GetIsStatic()` |
| null check of the entity's RwObject, then a read of its frame matrix | `GetModellingMatrix()` (returns `nullptr` without an RwObject) |

---

## 2. Math and floats

- `fabs(a - b) < eps` → `approxEqual(a, b, eps)` (`common.h`, the game's own helper at `0x4EEA80`).
- `__FYL2X__(x, 0.30102999566398119802)` → `std::log10(x)` (x87 `fyl2x` computes `log2(x) * log10(2)`).
- **Constants.** The named constants in `common.h` (`PI`, `TWO_PI`, `HALF_PI`, `FRAC_*`, `DEG_TO_RAD`, `SQRT_2`, ...)
  are the exact float values from `std::numbers`, which is what the game loads (`PI` = `3.1415927f`,
  `TWO_PI` = `6.2831855f`). A few functions load a rounded value instead. Write that literal with a `// NOTE:`
  comment, because the named constant would be wrong there:
  - `FxEmitter_c::CreateParticle`: `6.28318f`
  - `CWaterLevel::CalculateWavesOnlyForCoordinate`: `0.577f` (`E_CONST` is now the exact `0.5772157f`)

  `asm_validate.py` reports the difference as `PRECISION MISMATCH` (rule R3).
- **x87 precision.** The original keeps intermediate results in 80-bit x87 registers. See rule R4 and
  [CaseStudies.md](CaseStudies.md) §2-3.

---

## 3. IDA and disassembly quirks

- **`LOBYTE` may be a modulo.** IDA shows `x & 0xFF` as `LOBYTE(x)`. The original intent may be `% 256`.
- **Entity status is shifted.** `CEntity::m_nStatus` is a 5-bit bitfield above the 3-bit `m_nType`. The pseudocode
  compares `(flags & 0xF8)` against the status shifted left by 3: `== 0x18` is `STATUS_PHYSICS` (3), `== 0x20` is
  `STATUS_ABANDONED` (4). Use `GetStatus()` (`Entity.h`).
- **Bit flags.** `flags & 0x40` on entity, physical, ped or vehicle flags is a named bitfield member in the repo.
  The repo's bitfields are declared in bit order; §5 says where each one lives.

### x87 comparisons
The pattern is `fcom X` / `fnstsw ax` / `test ah, MASK` / `jcc`. `fcom` sets C0 (`0x01`) when `st0 < X`, C2
(`0x04`) when unordered (NaN) and C3 (`0x40`) when `st0 == X`. `st0` is the value loaded last with `fld`; `X` is the
operand of `fcom`/`fcomp`.

| `test ah,` | `jne`/`jnz` | `je`/`jz` | `jp` | `jnp` |
|---|---|---|---|---|
| `0x41` | `st0 <= X` | `st0 > X` | | |
| `0x01` | `st0 < X` | `st0 >= X` | | |
| `0x40` | `st0 == X` | `st0 != X` | | |
| `0x05` | | | `st0 >= X` | `st0 < X` |
| `0x44` | | | `st0 != X` | `st0 == X` |

- `fld A; fld B; fcomp st(1)` compares **B** (`st0`) against **A**.
- Hex-Rays shows these as conditions on `c0`/`c3`, for example `!(c0 | c3)`, which means `st0 > X`. It often gets
  the direction wrong: take the comparison from the disassembly. `asm_validate.py --dump` decodes each jump
  (`jne 0x509c76 ; jump if st(0) <= 3.1415927f`).

---

## 4. Known wrong addresses and IDB fixes

| What | Correct fact |
|---|---|
| `CGarages::aCarsInSafeHouse` | `0x96ABD8` (`CStoredCar[20][4]`). The old `0x96ABD4` is `ModelToTry`. |
| `CWorld::FindPlayerSlotWithVehiclePointer` | `0x564000`. `0x563FD0` is `FindPlayerSlotWithRemoteVehiclePointer` ([CaseStudies.md](CaseStudies.md) §9). |
| `CRadar::ShowRadarTraceWithHeight` (`0x584070`) | The colour arguments are 4 bytes each (`uint32 r, g, b, a`), as in the repo. The IDB declares them as `uint8`. |
| `CPed::operator new` (`0x5E4730`) | Takes a second argument: `void* __cdecl operator new(unsigned size, int32 poolRef)`. The IDB prototype only has `size`. |
| `tBeatInfo` / `tBeatInfo2` | The IDB has both (same size). Use `tBeatInfo`; `tBeatInfo2` is a badly reversed duplicate. |

---

## 5. Recovered enums and flags

These come from the IDB. The repo has most of them as enums or as bitfield members (declared in bit order). Enums that exist only in the IDB are listed with all their members in
[Enums.md](Enums.md).

### Entities, peds and vehicles
| IDB enum | Repo equivalent |
|---|---|
| `eEntityStatusShifted` | `eEntityStatus` (`Enums/eEntityStatus.h`), shifted left by 3 (see §3) |
| `eEntityFlags` | bitfields of `CEntity` (`Entity/Entity.h`) |
| `PhysicalFlags` | bitfields of `CPhysical` (`Entity/Physical.h`) |
| `ePedFlags1`-`ePedFlags4` | bitfields of `CPed` (`Entity/Ped/Ped.h`), `bIsStanding` (`0x1` of `ePedFlags1`) onwards |
| `ePlayerFlags` | bitfields of `CPlayerPedData` (`PlayerPedData.h`) |
| `ePedIKFlags` (not in the IDB) | bitfields of `CPedIK` (`PedIK.h`): `bGunReachedTarget` `0x1`, `bTorsoUsed` `0x2`, `bUseArm` `0x4`, `bSlopePitch` `0x8` |
| `eAutomobileFlags` | bitfields of `CAutomobile` (`Entity/Vehicle/Automobile.h`), `bTaxiLight` onwards |
| `eBikeFlags` | bitfields of `CBike` (`Entity/Vehicle/Bike.h`) |
| `VehAnimGroup` | `AssocGroupId` (`Enums/AnimationEnums.h`): `tHandlingData::m_nAnimGroup` is an offset from `ANIM_GROUP_STDCARAMIMS` (`GetAnimGroupId()`) |
| `eCarMission` | `Enums/eCarMission.h` |
| `TaskComplexEnterCar_Flags` | bitfields of `CTaskComplexEnterCar` (`m_bQuitAfterOpeningDoor`, ...) |
| `eTaskComplexGangFollowerFlags` | IDB only, see [Enums.md](Enums.md) |
| `eGrgFlags` | bitfields of `CGarage` (`Garage.h`) |

### Weapons & Melee Combat
| IDB enum | Repo equivalent |
|---|---|
| `eWeaponInfoFlag` | `eWeaponFlags` (`Enums/eWeaponFlags.h`). Bits `0x40` and `0x80` are unknown and not in the repo |
| `eMeleeCombo` | `Enums/eMeleeCombo.h` |
| `eMeleeCommand`, `eMeleeAttack`, `eMeleeHitLevel` | IDB only, see [Enums.md](Enums.md). `CTaskSimpleFight::m_nNextCommand`/`m_nLastCommand` hold an `eMeleeCommand` |

### Audio
| IDB enum | Repo equivalent |
|---|---|
| `eSpeechSlotStatus` | `CAEPedSpeechSlot::eStatus` (`Audio/Entities/AEPedSpeechAudioEntity.h`) |
| `GlobalSpeechContexts` | `eGlobalSpeechContext` (`Audio/Enums/PedSpeechContexts.h`) |

---

## 6. Recovered structs

These IDB structs are already in the repo; compare field names when the repo has `field_XX` members.

| IDB struct | Repo |
|---|---|
| `tScriptParam` | `Scripts/ScriptParam.h` |
| `CInterestingEvents` (and its `TInterestingEvent` entries) | `InterestingEvents.h` |
| `CAnimBlendAssocGroup` | `Animation/AnimBlendAssocGroup.h` |
| `CAnimBlendStaticAssociation` | `Animation/AnimBlendAssociation.h` |
| `CTaskSimpleFight` | `Tasks/TaskTypes/TaskSimpleFight.h` |
| `tMeleeComboSet` | `CMeleeInfo` (`Tasks/TaskTypes/TaskSimpleFight.h`), see below |
| `CAEPedSpeechAudioEntity` | `Audio/Entities/AEPedSpeechAudioEntity.h` |
| `CCoverPoint` | `CoverPoint.h` |

`tMeleeComboSet` (size `0x88`) is more precise than the repo's `CMeleeInfo` from `0x44` on:
```cpp
float        GroundLoopStart;   // 0x44 (repo: m_fGroundLoop)
float        BlockLoopStart;    // 0x48 (repo: ABlockHit)
float        BlockLoopEnd;      // 0x4C (repo: ABlockChain)
uint8        HitLevel[5];       // 0x50 (repo: uint8 m_nHitLevel)
uint8        Damage[5];         // 0x55 (repo: int32 m_nDamage @ 0x54, field_58)
tAudioEvent  HitSound[5];       // 0x5C (repo: m_Hit)
tAudioEvent  AltHitSound[5];    // 0x70 (repo: m_AltHit)
uint16       Flags;             // 0x84 (repo: m_wFlags)
```
