# Hook registration requirements for reversed classes
For every new reversed **in-game** class added under `source/game_sa/**`:

1. There must be `static void InjectHooks();` in the class declaration.
2. `InjectHooksMain()` at `source/InjectHooksMain.cpp` must call `ClassName::InjectHooks();`.

## PR review checklist
- [ ] In-game reversed class has `static void InjectHooks();`.
- [ ] `InjectHooksMain()` in `source/InjectHooksMain.cpp` contains a matching `ClassName::InjectHooks();` call.
- [ ] `friend void InjectHooksMain();` is present only when private hook-wrapper access is required.
- [ ] All functions present in the class must have a matching `RH_Install` call.
- [ ] Functions not yet reversed (eg.: have `plugin::Call` or similar redirect to GTA code) must have `.reversed = false` set in the `RH_Install` options.
