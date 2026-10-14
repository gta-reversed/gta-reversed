# reversiblehooks/ — the hook machinery behind `RH_Scoped*`

Guide: [../../docs/HookRegistration.md](../../docs/HookRegistration.md) (usage rules for reversed classes) and
[../../docs/Architecture.md](../../docs/Architecture.md) §4 (internals). Changes here affect every hook: keep them minimal.

## Layout
- `ReversibleHooks.h` — the macros. `RH_ScopedClass/NamedClass/Namespace/VirtualClass` open a scope;
  `RH_ScopedCategory("A/B")` / `RH_ScopedCategoryGlobal()` set the category. The hook path is
  `<Category>/<Scope>/<Fn>` (difftests use it); overloads are named `"<Fn>-<suffix>"`.
- `RHManager.h/.cpp` — singleton: `InstallStatic`, `InstallVirtual`, `InstallConstructor`, `InstallVirtualDestructor`,
  `InstallScriptCommand`; `HookInstallOptions` (`Reversed`, `Unhooked`, `State`, `Locked`, `Overrides`,
  `StackArgumentsToPreserve`, `PreserveRegisters`). `CheckAll()` re-validates the patches every 500 ms.
- `VMTInfo.h/.cpp` — vtable lookup: `FindByClassName` resolves our vtable from the exported MSVC symbol
  `??_7<Class>@@6B@` (needs `NOTSA_EXPORT_VTABLE`; single-parameter templates are mangled too), `FindIndexOf` finds a
  function's slot in a vtable.
- `ReversibleHook/` — hook kinds: `StaticTwoWayHook` (two `StaticOneWayHook` jumps), `VirtualHook` (vtable slot +
  direct-call jump), `VMTRedirectHook` (`.Overrides = false`), `VirtualDestructorHook`, `ScriptCommandHook`, `NullHook`
  (`NOTSA_STANDALONE` / `NOTSA_STANDALONE_DUMP_HOOKS_ONLY` builds). `HookCategory*`, `RootHookCategory.h` — the tree shown in F7 → Hooks
  (`source/toolsmenu/DebugModules/HooksDebugModule/`).

## Conventions
- States: `RedirectToOurs` = GTA's function jumps to ours. `RedirectToGTA` = **our** function jumps to GTA's (default
  and locked when `.Reversed = false`). `Unhooked` = nothing patched; difftests use it to call both versions.
- Hence a body that still calls `plugin::Call<own address>` must be `.Reversed = false`; otherwise GTA's address jumps
  to ours, which calls GTA's address again: infinite recursion.
- Virtual hooks look the GTA address up in GTA's vtable and patch the **same slot index** in ours, so our virtual
  declaration order must match GTA's and the `nVirtFns` count must cover the slot.
- Command line (`source/extensions/CommandLine.cpp`): `--use-gta-for=`, `--all-use-gta-hooks`,
  `--all-use-gta-hooks-except=` take `Category/Sub` (whole category) or `Category/Sub::Fn` (one hook), comma-separated.

## Pitfalls
- `VMTInfo` throws `std::runtime_error` when our vtable symbol is missing or the GTA address is not in the vtable
  (e.g. `CLodTimeModelInfo`, see `game_sa/Models/CLAUDE.md`). `check_hooks.py` catches most of these statically.
- `InjectHooksMain()` runs from `DllMain` under the loader lock. Nothing catches an escaping exception: the game
  freezes at the console with no message. Never throw, wait on threads or load DLLs on the install path; validate
  with `check_hooks.py` and start the game once.
- `StackArgumentsToPreserve` / `PreserveRegisters` are for callers that rely on non-standard conventions;
  `PreserveRegisters` without `StackArgumentsToPreserve` throws at install time (`InstallStatic`).
- Script-command hooks exist only with `NOTSA_WITH_SCRIPT_COMMAND_HOOKS`; otherwise `RH_ScopedInstallScriptCommand`
  is a no-op.
