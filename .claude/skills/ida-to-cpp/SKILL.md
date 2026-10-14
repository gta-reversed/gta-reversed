---
name: ida-to-cpp
description: >
  Translation reference from IDA/Hex-Rays pseudocode and x86/x87 disassembly of GTA:SA to idiomatic gta-reversed
  C++. Covers calling conventions, FPU compares, LOBYTE/bitfields, inlined engine helpers (GetPosition, GetIsStatic,
  CanSeeOutSideFromCurrArea...), RenderWare state macros, pools, strings, math idioms, magic constants and
  common decompiler traps. Use whenever converting decompiled code into C++ or reviewing such a conversion.
---

# IDA pseudocode → gta-reversed C++

This file covers *mechanical translation patterns*. The rules they serve are in `docs/ReversingRules.md` (R1-R7);
style is in `docs/CodingGuidelines.md`; engine quirks, known wrong addresses and recovered enums/structs are in
`docs/ai/TipsAndTricks.md`; functions MSVC inlined are in `docs/ai/InlineCatalogue.md`.

## 1. Calling conventions & signatures

| IDA shows | C++ in repo | Notes |
|---|---|---|
| `int __thiscall CFoo::Bar(CFoo *this, int a)` | `int32 CFoo::Bar(int32 a)` | `this` in `ecx` |
| `int __cdecl CFoo::Baz(int a)` | `static int32 CFoo::Baz(int32 a)` | static member or free function |
| `__stdcall` | RW plugin callbacks, Win32 API, some `CB` functions | keep `__stdcall` if used as a callback pointer |
| `__fastcall` with unused `edx` | usually a `__thiscall` mis-detected by IDA | treat as member |
| `__usercall X@<eax>(a@<esi>, ...)` | needs `{ .StackArgumentsToPreserve = N, .PreserveRegisters = true }` or a naked wrapper | ask before inventing one |
| returns struct by hidden pointer (`CVector *__thiscall f(CFoo *this, CVector *result, ...)`) | `CVector CFoo::f(...)` | `plugin::CallMethodAndReturn<CVector, ...>` handles it |
| `char` return used as boolean | `bool` | |
| `_DWORD`, `int` | `uint32`/`int32` (from usage) | never `DWORD` |
| `__int16`, `_WORD` | `int16`/`uint16` | |
| `_BYTE`, `char` used as number | `uint8`/`int8` | |

Overloads: `RH_ScopedOverloadedInstall(Fn, "Suffix", 0xADDR, Ret(CFoo::*)(Args) const)`. If our signature differs
from the original ABI, hook a `Fn_ToHook` wrapper (R12).

## 2. Inlined engine helpers - collapse them

| Decompiled pattern | Replace with |
|---|---|
| `m_matrix ? &m_matrix->pos : &m_placement.m_vPosn` | `GetPosition()` |
| same for right/forward/up | `GetRight()`, `GetForward()`, `GetUp()` |
| `m_bIsStatic \|\| m_bIsStaticWaitingForCollision` | `GetIsStatic()` |
| `!CGame::currArea` | `CGame::CanSeeOutSideFromCurrArea()` |
| `clump->parent == (RwFrame*)-16` / clump/frame null checks | `GetModellingMatrix()` |
| `CWorld::Players[CWorld::PlayerInFocus].m_pPed` | `FindPlayerPed()` (also `FindPlayerVehicle()`, `FindPlayerWanted()`, `FindPlayerCoors()`) |
| `(handle >> 8)` / pool `m_byteMap[i] & 0x80` loops | pool helpers: `GetPedPool()->GetAt(handle)`, `GetAtRef`, `GetAllValid()` (grep `Pool.h`) |
| `RwFrameDestroy(atomic->object.object.parent)` after destroy | `RpAtomicGetFrame` + `RpAtomicDestroy` + `RwFrameDestroy` |
| `RwRenderStateSet(x, (void*)1)` | `RwRenderStateSet(x, RWRSTATE(TRUE))` (`contrib/clean-srs-calls.py` for bulk) |
| `LOWORD(tex->filterAddressing) = 0x1102` | equivalent to `RwTextureSetAddressing` + `RwTextureSetFilterMode`; prefer the calls |
| `CTimer::ms_fTimeStep` & co. | `CTimer` accessors (`GetTimeStep()`, `GetTimeInMS()`...); the variables are private |
| hand-rolled `x*x + y*y + z*z` | `MagnitudeSquared()` (`Magnitude()` for the `sqrt`) |
| hand-rolled dot/cross | `.Dot()`, `.Cross()` |
| `fabs(a - b) < eps` | `approxEqual(a, b, eps)` |
| `__FYL2X__(x, 0.30102999566398119802)` | `std::log10(x)` |

Before translating a block that looks generic, grep for an existing helper (`extensions/`, `General.h`, `Vector.h`,
`Matrix.h`, `Entity.h`) and `docs/ai/InlineCatalogue.md` §1. If an inlined block matches a reversed function, call it.

## 3. x87 floating point

`python .claude/skills/validate-against-binary/scripts/asm_validate.py 0xADDR --dump --no-check` decodes compare
sequences for you (`jne 0x53cb24 ; jump if st(0) < 180.0f`) and prints the exact constants. Trust the disassembly
over the pseudocode: Hex-Rays often gets the direction wrong.

Compares: `fcom/fcomp X; fnstsw ax; test ah, MASK; jcc`. C0 (`0x01`) = `st0 < X`, C2 (`0x04`) = unordered,
C3 (`0x40`) = `st0 == X`.

| `test ah,` | `jnz` | `jz` | `jp` | `jnp` |
|---|---|---|---|---|
| `0x41` | `st0 <= X` | `st0 > X` | | |
| `0x01` | `st0 < X` | `st0 >= X` | | |
| `0x40` | `st0 == X` | `st0 != X` | | |
| `0x05` | | | `st0 >= X` | `st0 < X` |
| `0x44` | | | `st0 != X` | `st0 == X` |

- `st0` is the value on top of the stack: in `fld A; fld B; fcomp st(1)` it is **B**, compared against A.
  `fcompp` compares `st0` with `st(1)` and pops both.
- Pseudocode `if (!(c0 | c3))` means `st0 > X`.
- **Precision** (R4): MSVC 2003 keeps intermediates in 80-bit registers. A `double` constant (`fld qword ptr`,
  `dbl_` in IDA) means the math is in double; match it when results could differ (`static_cast<float>(x * 0.5)`).
  A value accumulated across a loop without being stored needs `double` in our code (`CGeneral::LimitRadianAngle`).
  Keep the operation order: `(1 / a) * 0.5` and `1 / (2a)` round differently.
- **Constants** (R3): use the value the binary loads, bit for bit. `PI`, `TWO_PI`, `HALF_PI`, `FRAC_*` in
  `common.h` are exact (`std::numbers`: `3.1415927f`, `6.2831855f`). Where the binary loads a rounded value
  (`FxEmitter_c::CreateParticle` loads `6.28318f`), write that literal with a `// NOTE:` comment. Keep
  `1.0f / 50.0f` vs `0.02f` as the binary has it. Use `DegreesToRadians`/`RadiansToDegrees` only when the constant
  matches.
- `_ftol`/`_ftol2` → truncating `static_cast<int32>(f)`.
- Time-step dependent code stays as is; framerate fixes go behind `notsa::bugfixes::GenericFrameRate`.

## 4. Integer / bit idioms

| Pattern | Meaning |
|---|---|
| `LOBYTE(x) = ...`, `BYTE1(x)`, `HIBYTE` on a struct field | separate `uint8` members or a bitfield; fix the struct, don't keep `LOBYTE` |
| `x & 0xFF` shown as `LOBYTE(x)` | may originally be `% 256` (TipsAndTricks §3) |
| `flags & 0x40` on entity/ped/physical flags | named bitfield members (`m_bIsVisible`, `bInVehicle`...), TipsAndTricks §5 |
| `(x >> 3) & 0x1F` on a status-like byte | packed bitfields: `m_nType : 3`, `m_nStatus : 5` |
| `(unsigned int)(x - a) <= b - a` | range check `a <= x && x <= b` |
| `x - 10 * (x / 10)` / `__ROL4__` | `x % 10` / `std::rotl` |
| `sar/shr` after `imul` with a magic constant | division by a constant |
| `v = (cond) ? -1 : 0; v &= ...` (`sbb`) | branchless ternary → write the ternary |
| `rand() * 4.656613e-10f` / `CGeneral::GetRandomNumber() ...` | `CGeneral::GetRandomNumberInRange(...)` **only if it is exactly the same formula**; keep the call count (R2) |

## 5. Memory, strings, containers

| Pattern | C++ |
|---|---|
| `qmemcpy(&a, &b, sizeof)` of a struct | `a = b;` |
| `memset(this, 0, N)` in ctor | member initialisers / `= {}` |
| `strcmp(a, b) == 0` | `std::string_view{a} == b` |
| `strncmp(a, b, len) == 0` with `len == strlen(b)` | `std::string_view{a}.starts_with(b)` |
| `_strcmpi`/`_stricmp` | existing case-insensitive helper (grep `ci_string_view`), else keep |
| `sprintf(buf, ...)` | keep semantics; `std::format_to` only if the output is identical |
| fixed array + `m_nCount` loops | `std::span{ m_Arr, m_nCount }` or `m_Arr \| rngv::take(m_nCount)` |
| `for (i = 0; i < N; ++i) arr[i]` | range-for / `rngv::enumerate` |
| linked list `for (n = list.first; n; n = n->next)` | existing iterator helpers (`CPtrListSingleLink`/`CLink` ranges) if present |
| GXT strings (`GxtChar*`) | CodingGuidelines "Handling translated (GXT) text" (`AsciiToGxtChar`, `""_gxt`) |

## 6. Globals and statics

- IDA `dword_B7CB64` → find the existing `StaticRef` by address (grep `-i "0xB7CB64"`, or `docs/ai/Globals.md`).
- New global: `static inline auto& ms_Name = StaticRef<T>(0xADDR);` in the owning class; file-local:
  `static auto& s_Name = StaticRef<T>(0xADDR);`; arrays: `StaticRef<std::array<T, N>>(0xADDR)` (R14).
- Constant `.rdata` tables never written by the game: may become a `constexpr std::array`.
- Function-local `static` with a guard byte (`if (!(byte_C1234 & 1)) { byte_C1234 |= 1; ... }`) → `ScopedStaticRef`
  or a `static` local referencing the original (grep `ScopedStaticRef` for examples).

## 7. Virtual calls & type checks

- `(*(void (__thiscall **)(CEntity *))(*(_DWORD *)ent + 0x2C))(ent)` → slot `0x2C / 4 = 11`. Declaration order in the
  base class header is vtable order; `docs/ai/Vtables.md` lists the slots. Call the virtual by name.
- `ent->m_nType == 3` → `ent->GetIsTypePed()` (`GetIsTypeVehicle()`, `GetIsTypeObject()`, `GetIsTypeBuilding()`), then
  `AsPed()`/`AsVehicle()`.
- Vehicles: `m_nVehicleSubType` → `IsSubAutomobile()`, `IsSubBike()`, ...; `m_nVehicleType` only distinguishes
  automobile/bike/boat/train (R5).
- `task->GetTaskType() == 0x2BC` → the `TASK_...` value of `eTaskType`.

## 8. Things the decompiler hides or invents

- **Dead stores / duplicated loads**: drop them.
- **One stack slot, two meanings**: split into two named variables.
- **Wrong argument count** for unknown prototypes: check `add esp, N` after the call or `retn N` in the callee.
- **`this` adjusted for multiple inheritance** (`this - 4`, `this + 0x34`): cast to the right base class.
- **Tail calls** shown as `return f(...)` with a mismatched return type: plain call, then return.
- **SEH / `_EH_prolog`** in ctors: ignore; translate the member initialisation.
- **`switch` default falling through**: read the jump table, write explicit `case`s; `NOTSA_UNREACHABLE_CASE(v)` only
  when the original also can't reach the default.

## 9. Annotations

- Original bug: keep it with `// BUG: <explanation>`; fix only behind `notsa::bugfixes::<Name>` or a generic one
  (`GenericCrashing`, `GenericOOB`, `GenericUB`, `GenericFrameRate`) (R6).
- Code that does not exist in the original: `// NOTSA` (helpers and asserts don't need it).
- Inlined function called by name: `// NOTE: inlined in original` when it isn't obvious.
- `MagnitudeSquared` where the original used `Magnitude`: `// NOTE: Using MagSq here`.
- Long functions: `// 0xADDR` comments from the decompiler's `/*0x...*/` markers.
