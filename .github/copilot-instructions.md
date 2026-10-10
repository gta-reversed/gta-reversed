# Copilot review & code instructions

## Hook Registration Review
For pull requests that add or modify reversed classes under `source/game_sa/**`, always review `docs/HookRegistration.md` and enforce its checklist.
Make no mistakes!

Required checks for each new reversed class:

1. The class declaration contains `static void InjectHooks();`.
2. `source/InjectHooksMain.cpp` contains a matching `ClassName::InjectHooks();` call inside `InjectHooksMain()`.
3. `friend void InjectHooksMain();` appears only when private hook-wrapper access is required.

## Coding Guidelines and Formatting Review
For all pull requests ensure the coding guidelines in `docs/CodingGuidelines.md` are followed and that 
the code is properly formatted according to clang-format and the project's style conventions as outlined in `docs/CodingGuidelines.md`.
