# Reversal case studies

These are real mistakes found in reversed code, with the original behaviour, the fix and the lesson. Each one passed
review and looked right in the pseudocode. Read them before reversing something similar. The rule numbers (R*n*)
refer to [../ReversingRules.md](../ReversingRules.md).

Cases 1-3 and 5-7 are still **open** in the repository: their fixes change game code and go in separate pull
requests (see [../BinaryMap.md](../BinaryMap.md) §4.2). The code shown as "Fix" is the change that makes them match
the original. Case 4 and cases 8-10 are fixed.

| # | Function | Kind of mistake | Found by | Status |
|---|---|---|---|---|
| 1 | `CCam::ClipBeta` | wrong branch structure | disassembly | open |
| 2 | `CGeneral::SolveQuadratic` | operation order changes rounding | difftest | open |
| 3 | `CGeneral::LimitRadianAngle` | x87 extended precision | difftest | open |
| 4 | `PI`/`TWO_PI` in `common.h` | rounded constants | static check (`asm_validate.py`) | fixed (`std::numbers`) |
| 5 | `CTaskSimpleCarJumpOut::ProcessPed` | vehicle type vs sub-type | disassembly | open |
| 6 | `CPedIntelligence::GetActivePrimaryTask` | loop variable not used | disassembly | open |
| 7 | `CMatrix::ConvertToEulerAngles` | wrong array index | difftest | open |
| 8 | `CTaskInteriorLieInBed::InjectHooks` | VMT install of a static function | the game froze at startup | fixed |
| 9 | `CWorld::InjectHooks` | hook installed at the neighbour's address | IDB names | fixed |
| 10 | 4 hooks (`CDecision::Set`, ...) | stub installed as reversed | `check_hooks.py` | fixed |

---

## 1. `CCam::ClipBeta` (`0x509C50`): wrong branch structure

```cpp
// Current repo code
if (m_fHorizontalAngle < DegreesToRadians(-180.0f)) {
    m_fHorizontalAngle += DegreesToRadians(360.0f);
} else {
    m_fHorizontalAngle -= DegreesToRadians(360.0f);   // runs for EVERY angle >= -pi
}
// Fix (matches the original)
if (m_fHorizontalAngle > PI) {
    m_fHorizontalAngle -= TWO_PI;
} else if (m_fHorizontalAngle < -PI) {
    m_fHorizontalAngle += TWO_PI;
}
```
The original loads `3.1415927f` and `6.2831855f`, which are exactly `PI` and `TWO_PI`. The static check was clean,
because the constants and calls were all right. The mistake was in the branch layout:
an `else` where the original has `else if`.
**Lesson:** read the conditional jumps in `asm_validate.py --dump` and check that every path exists in our code
(R4). A wrap-around helper that changes a value already in range is always suspect.

## 2. `CGeneral::SolveQuadratic` (`0x53CE30`): operation order

```cpp
// Current repo code
const auto denom = 1.f / (2.f * a);
x1 = (-b - discriminantSqrt) * denom;
x2 = (-b + discriminantSqrt) * denom;
// Fix
const auto invA = 1.f / a;
x2 = (discriminantSqrt - b) * invA * 0.5f;
x1 = (-b - discriminantSqrt) * invA * 0.5f;
```
The two versions are equal on paper, but in floating point `1 / (2a)` and `(1 / a) * 0.5` round differently. A
first, bit-exact version of the differential test reported results a few ULP apart. The current test has to allow
8 ULP plus a relative error, because the original also computes the discriminant in x87 precision. It passes with
either version, so the operation order has to be checked in the disassembly.
**Lesson:** keep the original's operation order, even when an algebraic rewrite looks cleaner (R4). Write
`x2` before `x1` if the original does; it doesn't matter here, but it makes comparing with the disassembly easier.

## 3. `CGeneral::LimitRadianAngle` (`0x53CB50`): x87 extended precision

The original loads the angle onto the x87 stack and subtracts or adds 2π in a loop without storing it, so every step
is computed in 80-bit precision. Rounding back to `float` happens once, at the end. Our `float` loop rounds after
every step and drifts by about 1e-6. The fix computes the loop in `double`:
```cpp
double result = std::clamp(angle, -25.0f, 25.0f);
while (result >= (double)PI)  { result -= (double)TWO_PI; }
while (result < -(double)PI)  { result += (double)TWO_PI; }
return (float)result;
```
**Lesson:** when the disassembly keeps a value on the x87 stack across a loop (no `fstp` to memory inside it),
accumulate in `double` (R4). Compare float results in difftests with ULP tolerance (`CompareFloats`), not
byte-for-byte.

## 4. `PI` / `TWO_PI` in `common.h`: rounded constants (fixed)

`PI` was `3.14159f`, which is 11 ULP away from the `3.1415927f` the game loads, and the other constants
(`TWO_PI`, `HALF_PI`, `FRAC_*`, ...) were rounded the same way. Every function written with them computed slightly
different results than the original: `asm_validate.py` reported `PRECISION MISMATCH` in many functions, and the
difftests of `CGeneral::GetATanOfXY`, `GetRadianAngleBetweenPoints` and `GetAngleBetweenPoints` failed.
The constants are now the exact `std::numbers` values. The few functions whose original really loads a rounded value
keep it as a literal with a `// NOTE:` comment: `FxEmitter_c::CreateParticle` (`6.28318f`) and
`CWaterLevel::CalculateWavesOnlyForCoordinate` (`0.577f`).
**Lesson:** use the value the binary loads (R3). A shared constant can't be "close enough".

## 5. `CTaskSimpleCarJumpOut::ProcessPed` (`0x64DD60`): type vs sub-type

```cpp
} else if (m_veh->IsPlane()) {      // repo: m_nVehicleType is AUTOMOBILE for a CPlane, so this is never true
} else if (m_veh->IsSubPlane()) {   // fix: the original reads m_nVehicleSubType (0x64DE90)
```
**Lesson:** check which field the original reads (R5). `IsPlane()`, `IsHeli()`, `IsTrailer()` and `IsBMX()` are
never true on a live vehicle. `CVehicle::IsSubFlyingVehicle` has a related mistake: `IsSubHeli() && IsSubPlane()`
(always false) instead of `||`.

## 6. `CPedIntelligence::GetActivePrimaryTask` (`0x4B85B0`): loop variable not used

```cpp
for (const auto i : { TASK_PRIMARY_PHYSICAL_RESPONSE, TASK_PRIMARY_EVENT_RESPONSE_TEMP, TASK_PRIMARY_EVENT_RESPONSE_NONTEMP }) {
    if (CTask* task = m_TaskMgr.GetTaskPrimary(TASK_PRIMARY_PHYSICAL_RESPONSE)) {  // repo: always the same slot
    if (CTask* task = m_TaskMgr.GetTaskPrimary(i)) {                               // fix
```
**Lesson:** a loop whose variable is not used in the body is almost always a mistake. Our build treats warnings as
errors, but no compiler warning catches this.

## 7. `CMatrix::ConvertToEulerAngles` (`0x59A840`): wrong array index

In the `EULER_ANGLES` branch, `atan2(fArr[iInd2][iInd3], ...)` should be `atan2(fArr[iInd2][iInd1], ...)`.
The two expressions look nearly identical in the pseudocode. The difftest found it by comparing 5,000 random
rotation matrices against the original.
**Lesson:** functions full of index arithmetic need a difftest. Reading them is not enough.

## 8. `CTaskInteriorLieInBed::InjectHooks`: VMT install of a static function

`FinishAnimCB` is a static animation callback, but it was registered with `RH_ScopedVMTInstall`. `VMTInfo` could not
find its address in the class vtable and threw. Because `InjectHooksMain` runs inside `DllMain` with the loader lock
held, the exception led to `exit()`, which waited for a logger thread that could never start. The game froze at
the console with no message. `CLodTimeModelInfo` has the same problem: its `InjectHooks` uses a vtable that doesn't
contain the functions it installs, which is why it stays unregistered.
**Lesson:** use `RH_ScopedVMTInstall` only for addresses that are in that class's vtable (R10, see
[Vtables.md](Vtables.md)). Start the game once after adding hooks (R11). `check_hooks.py` now checks VMT addresses
against the vtable when it can read `gta_sa.exe`.

## 9. `CWorld::InjectHooks`: hook installed at the neighbour's address

`FindPlayerSlotWithVehiclePointer` was installed at `0x563FD0`, which is the address of
`FindPlayerSlotWithRemoteVehiclePointer`; the right address is `0x564000`. Both functions have the same signature,
so nothing crashed: GTA's calls to one function silently ran the other.
**Lesson:** copy addresses from IDA, never from a nearby line (R13). Look up the address in the IDB to confirm that
the name matches.

## 10. Stubs installed as reversed

`CCamera::TakeControlAttachToEntity`, `CConversationForPed::IsPlayerInPositionForConversation`, `CDecision::Set` and
`CTheScripts::ScriptDebugCircle2D` were installed as reversed while their bodies were still
`plugin::Call*<their own address>`. With the hook active, GTA jumps to our function, which calls the original
address, which jumps back to ours, until the stack overflows. They only stayed unnoticed because nothing exercised
them.
**Lesson:** a function that still calls GTA is `{ .Reversed = false }` (R9). `check_hooks.py` reports this as an
error, and the PostToolUse hook and the CI job run it automatically.
