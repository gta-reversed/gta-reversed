# Hook registration requirements for reversed classes

For every new reversed class added under `source/game_sa/**`:

1. Declare `static void InjectHooks();` in the class declaration.
2. Register the class in `/home/runner/work/gta-reversed/gta-reversed/source/InjectHooksMain.cpp` by adding `ClassName::InjectHooks();` inside `InjectHooksMain()`.

`friend void InjectHooksMain();` is only expected when `InjectHooksMain()` must access private hook wrappers. Do not add this friend declaration when it is not needed.

## PR review checklist

- [ ] New class under `source/game_sa/**` declares `static void InjectHooks();`.
- [ ] `InjectHooksMain()` in `source/InjectHooksMain.cpp` contains a matching `ClassName::InjectHooks();` call.
- [ ] `friend void InjectHooksMain();` is present only when private hook-wrapper access is required.
