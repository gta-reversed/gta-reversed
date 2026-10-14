# Copilot review & code instructions

## Docs
Read the docs under the `docs/` directory and apply them. Start with `docs/ReversingRules.md` (the rules every
reversed function must follow) and `docs/CodingGuidelines.md`.

## Hook registration review
For pull requests that add or modify reversed code under `source/game_sa/**`, review `docs/HookRegistration.md` and
enforce its checklist (section 5). In short, for each reversed class:

1. The class declaration contains `static void InjectHooks();`.
2. `source/InjectHooksMain.cpp` calls `ClassName::InjectHooks();` inside `InjectHooksMain()`.
3. `friend void InjectHooksMain();` appears only when `InjectHooks` or a hooked wrapper is private.
4. Every function with a GTA address has an `RH_Scoped*Install`; virtual functions use `RH_ScopedVMTInstall` under
   `RH_ScopedVirtualClass`.
5. A function whose body still calls `plugin::Call*` on its own address is installed with `{ .Reversed = false }`.

The `Hook registration check` workflow (`check_hooks.py`) checks most of this automatically.

## Coding guidelines and formatting review
For all pull requests ensure the coding guidelines in `docs/CodingGuidelines.md` are followed and that the code is
formatted according to the repo's `.clang-format` and `.editorconfig`.

## Script command review
For pull requests that add or modify script commands (`source/game_sa/Scripts/Commands/**`), review
`docs/ScriptCommandReversing.md` and enforce its conventions.
