# Hook registration

How to register reversed code with the reversible-hook system. The macros live in
`source/reversiblehooks/ReversibleHooks.h`, the options in `RHManager::HookInstallOptions`
(`source/reversiblehooks/RHManager.h`). How hooks work internally: [Architecture.md](Architecture.md) §4. The rules
behind this page, with their reasons: [ReversingRules.md](ReversingRules.md) R8-R12.

## 1. Every class gets an `InjectHooks()`

For every reversed **game** class under `source/game_sa/**`:

1. Declare `static void InjectHooks();` in the class.
2. Call `ClassName::InjectHooks();` from `InjectHooksMain()` in `source/InjectHooksMain.cpp`. Calls are grouped by
   subsystem (some groups are local lambdas such as `Tasks`, `Events`, `Audio`).
3. If `InjectHooks` (or the wrapper functions it installs) is private, add `friend void InjectHooksMain();`. Don't add
   the `friend` when everything is public.

A class whose `InjectHooks()` is never called installs nothing: its functions are "DORMANT" (see
[Glossary.md](Glossary.md)).

```cpp
// TaskSimplePause.h
class NOTSA_EXPORT_VTABLE CTaskSimplePause : public CTaskSimple {
    ...
private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimplePause* Constructor(int32 time);
};

// TaskSimplePause.cpp
void CTaskSimplePause::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePause, 0x85A278, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48E750);
    RH_ScopedVMTInstall(ProcessPed, 0x48E830);
    RH_ScopedVMTInstall(Clone, 0x48E780);
    RH_ScopedVMTInstall(MakeAbortable, 0x48E810);
}
```

## 2. Open a scope

Each `InjectHooks()` starts with one scope macro and one category macro. Together they give the hook path
`<Category>/<Scope>/<Fn>`, shown in F7 → Settings → Hooks and used by `--use-gta-for=` and the difftests.

| Macro | Use for |
|---|---|
| `RH_ScopedClass(Cls)` | A class without virtual functions to hook |
| `RH_ScopedVirtualClass(Cls, 0xVTBL, nVirtFns)` | A class with virtual functions to hook. Needed by every `RH_ScopedVMT*Install` |
| `RH_ScopedNamespace(ns)` | A namespace of free functions (`CGeneral`, `RenderBuffer`, `ModelIndices`) |
| `RH_ScopedNamespaceName("Name")` | Only sets the scope name. Works with the `*Global*` install macros only |
| `RH_ScopedNamedClass(Cls, "Name")` | Like `RH_ScopedClass` with a custom scope name (currently unused) |
| `RH_ScopedCategory("A/B")` | The category. Follows the directory: a file in `game_sa/Entity/Ped/` uses `"Entity/Ped"` |
| `RH_ScopedCategoryGlobal()` | The category for files directly in `source/game_sa/` (`"Global"`) |

For `RH_ScopedVirtualClass`:
- `0xVTBL` is GTA's vtable address and `nVirtFns` the number of slots to search. Take both from
  [ai/Vtables.md](ai/Vtables.md) (or IDA).
- Our class must be declared `class NOTSA_EXPORT_VTABLE Cls`. Our vtable is found by its exported symbol
  `??_7Cls@@6B@`; without the export, installation throws.
- The GTA address is looked up in GTA's vtable, and the **same slot** of our vtable is hooked. So our virtual
  functions must be declared in GTA's order, and `nVirtFns` must cover the slot.

## 3. Install functions

| Macro | Installs |
|---|---|
| `RH_ScopedInstall(Fn, 0xADDR[, opts])` | A member or static function of the scope class/namespace (`&Cls::Fn`) |
| `RH_ScopedGlobalInstall(Fn, 0xADDR[, opts])` | A free function (`&Fn`) |
| `RH_ScopedVMTInstall(Fn, 0xADDR[, opts])` | A virtual function: patches GTA's function **and** swaps the vtable slot. Needs `RH_ScopedVirtualClass` |
| `RH_ScopedOverloadedInstall(Fn, "suffix", 0xADDR, Signature[, opts])` | One overload of a member/static function. The hook is named `Fn-suffix` |
| `RH_ScopedGlobalOverloadedInstall(Fn, "suffix", 0xADDR, Signature[, opts])` | One overload of a free function |
| `RH_ScopedVMTOverloadedInstall(Fn, "suffix", 0xADDR, Signature[, opts])` | One overload of a virtual function |
| `RH_ScopedNamedInstall(Fn, "Name", 0xADDR[, opts])` | A member under another hook name (e.g. a `_Hooked` wrapper shown as the real name) |
| `RH_ScopedNamedGlobalInstall(Fn, "Name", 0xADDR[, opts])` | A free function under another hook name |
| `RH_ScopedInstallScriptCommand(cmd)` | Used by `REGISTER_COMMAND_HANDLER`; a no-op unless `NOTSA_WITH_SCRIPT_COMMAND_HOOKS` |

`Signature` is the function pointer type. Member functions use `Ret(Cls::*)(Args...)`; add `const` after the argument
list for const members:

```cpp
RH_ScopedOverloadedInstall(ReportMissionAudioEvent, "vec", 0x507340, void(CAudioEngine::*)(uint16, CVector&));
RH_ScopedOverloadedInstall(ReportMissionAudioEvent, "ped", 0x507370, void(CAudioEngine::*)(uint16, CPed*));
RH_ScopedOverloadedInstall(GetRelativePosition, "orginal", 0x4EF350, void(CAESound::*)(CVector*) const);
RH_ScopedGlobalOverloadedInstall(MakeUpperCase, "in", 0x718710, char*(*)(char*));
```

Every suffix must be unique within the scope; the same GTA address must not be installed twice.

### Constructors and destructors

Hook a member wrapper, not the constructor itself:

```cpp
CAnimBlendClumpData* Constructor() { this->CAnimBlendClumpData::CAnimBlendClumpData(); return this; }
CAnimBlendClumpData* Destructor()  { this->CAnimBlendClumpData::~CAnimBlendClumpData(); return this; }
...
RH_ScopedInstall(Constructor, 0x4CF0E0);
RH_ScopedInstall(Destructor, 0x4CF100);
```

Several constructors use numbered wrappers with `RH_ScopedOverloadedInstall` (see `CAnimBlendAssociation::InjectHooks`).
`RH_ScopedConstructorInstall`, `RH_ScopedDestructorInstall` and `RH_ScopedVMTDestructorInstall` exist but aren't used.

### Functions that can't be hooked directly

- **Changed signature.** If our C++ signature differs from the original ABI, hook a wrapper with the original
  signature (`Fn_ToHook` / `Fn_Hooked`, e.g. `CTaskSimpleCarJumpOut::ComputeAnimID_ToHook`).
- **`__usercall`** (arguments in arbitrary registers). Leave the install commented out with the reason, as in
  `CPlantMgr::InjectHooks` (`LoadModels`).
- **Callers that rely on preserved registers or stack.** `{ .StackArgumentsToPreserve = n, .PreserveRegisters = true }`
  wraps the call in a `pushad`/`popad` stub (see `CRunningScript::InjectHooks`). `PreserveRegisters` without
  `StackArgumentsToPreserve` throws at install time.

## 4. Install options

The optional last argument is a `HookInstallOptions`, written with designated initializers. Fields must appear in
declaration order:

| Field | Default | Meaning |
|---|---|---|
| `Reversed` | `true` | `false` = our function isn't finished; the hook starts as `RedirectToGTA` |
| `Unhooked` | `false` | Start as `Unhooked` (nothing patched). Ignored when `Reversed = false` |
| `State` | from `Reversed`/`Unhooked` | Explicit start state: `HS::RedirectToOurs`, `HS::RedirectToGTA`, `HS::Unhooked` |
| `Locked` | `!Reversed` | The state can't be changed from the F7 menu or by `--use-gta-for=` |
| `Overrides` | `true` | Virtual only. `false` when GTA has its own copy of a method our class only inherits (installs a vtable swap only) |
| `StackArgumentsToPreserve` | none | See above |
| `PreserveRegisters` | `false` | See above |

Common forms:

```cpp
RH_ScopedInstall(Fn, 0xADDR, { .Reversed = false });                       // still a plugin::Call stub
RH_ScopedVMTInstall(MakeAbortable, 0x632C00, { .Overrides = false });     // inherited in our code, own copy in GTA
RH_ScopedGlobalInstall(NOTSA_WinMain, 0x748710, { .Locked = true });      // reversed, but must never be switched back
RH_ScopedInstall(UnloadDecisionMaker, 0x607A70, { .State = HS::RedirectToGTA, .Locked = true }); // ours has a known problem: keep GTA's
```

### `.Reversed = false` is mandatory for stubs

A function whose body still calls `plugin::Call*<its own address>` **must** be `{ .Reversed = false }`. Installed as
reversed, GTA's address jumps to our function, which calls GTA's address, which jumps back: infinite recursion.
Remove the flag once the body is fully reversed. A `Reversed = false` hook can only be in `RedirectToGTA`
(`HookCategoryItem::SetState` asserts it).

## 5. Check before a PR

```bash
python .claude/skills/review-reversal/scripts/check_hooks.py            # files changed vs master (+ uncommitted)
```

It runs after every Edit/Write in Claude Code and in CI (`.github/workflows/hook-registration.yml`). Then start the
game once: `RH_ScopedVMTInstall` throws at startup if the address isn't in the vtable.

Review checklist:
- [ ] The reversed class declares `static void InjectHooks();`.
- [ ] `InjectHooksMain()` in `source/InjectHooksMain.cpp` calls `ClassName::InjectHooks();`.
- [ ] `friend void InjectHooksMain();` is present only when `InjectHooks` or a hooked wrapper is private.
- [ ] Every function of the class that has a GTA address has a matching `RH_Scoped*Install`.
- [ ] Virtual functions use `RH_ScopedVMTInstall` under `RH_ScopedVirtualClass`, and the class is `NOTSA_EXPORT_VTABLE`.
- [ ] Functions not yet reversed (body calls `plugin::Call*` or otherwise redirects to GTA) have `{ .Reversed = false }`.
