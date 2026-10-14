---
name: reverse-class
description: >
  Create or complete a whole reversed GTA:SA class/struct in gta-reversed - memory layout with VALIDATE_SIZE /
  VALIDATE_OFFSET, vtable, static members via StaticRef, ctor/dtor wrappers, InjectHooks with every function
  registered, and registration in source/InjectHooksMain.cpp. Use when a class does not exist yet, when a class
  has many `.Reversed = false` hooks to finish, or when a struct layout must be recovered. For a single function
  use reverse-function.
argument-hint: "<ClassName> [vtable address] [size]"
---

# Reverse / complete a class

Target: `$ARGUMENTS`

Rules: `docs/ReversingRules.md` (layout R13-R15, hooks R8-R12), `docs/HookRegistration.md`, `docs/CodingGuidelines.md`.

## 1. Inventory

1. Run `python .claude/skills/find-reversing-targets/scripts/class_dossier.py <Class> --ida` (drop `--ida` without
   IDA). It prints the repo files, every method with status/size/address, the vtable, layout checks, dependencies
   with a leaf-first order and callers.
2. The class exists → reverse its functions one by one with **reverse-function**, smallest/leaf first.
   `list_unreversed.py --filter <File> --details` lists the remaining hooks.
3. The class is new → pick the directory by subsystem. It sets the hook category: files in
   `source/game_sa/Entity/Ped/` use `RH_ScopedCategory("Entity/Ped")`; files directly in `source/game_sa/` use
   `RH_ScopedCategoryGlobal()`.
4. Virtual class → grep `docs/ai/Vtables.md` for `### <Class>`: vtable address, slot count, each slot's status.

Many functions at once: the named workflow `.claude/workflows/reverse-class.js` (args `{cls, max, maxBytes}`) runs one
`gta-reverser` per function in its own worktree, then reviews each. Start it only when the user asks.

Done when: you have the list of functions to write, in order, and know whether the class is new.

## 2. Recover the layout (IDA)

- **Size**: `operator new` call sites (`push 0xNN; call operator new`), pool sizes, `memset` in the ctor, array
  strides (`imul reg, 0xNN`).
- **Members**: every `[this+0xNN]` access in ctor, dtor and methods; note width (byte/word/dword/float) and use.
  `xrefs_to_field`, `read_struct`, `search_structs`, `type_inspect` help.
- **Vtable**: the ctor writes it (`mov dword ptr [esi], offset off_XXXXXX`). `nVirtFns` counts all entries,
  inherited ones included; compare with the base class's `RH_ScopedVirtualClass` count.
- **Cross-check**: `struct_check.py --type <Class>` (repo asserts vs IDB), `docs/ai/TipsAndTricks.md` §5-§6
  (recovered enums and structs). The IDB is community-made and can be wrong: a mismatch means "look at both".

Done when: every member has an offset you can justify and the total size is known.

## 3. Header template

Addresses below are placeholders; take the real ones from IDA.

```cpp
#pragma once

#include "FooBase.h"       // base class header

class CEntity;             // forward declare instead of including when possible

class NOTSA_EXPORT_VTABLE CFoo : public CFooBase { // NOTSA_EXPORT_VTABLE only for polymorphic classes
public:
    static inline auto& ms_Count = StaticRef<uint32>(0xADDR); // statics reference the original

    int16    m_Id;         // 0x4
    uint8    m_Flags;      // 0x6
    uint8    _pad7;        // keep unknown bytes as padding
    CVector  m_Pos;        // 0x8
    CEntity* m_Target;     // 0x14

public:
    static void InjectHooks();

    CFoo(int16 id);                      // 0xADDR
    ~CFoo() override;                    // 0xADDR (plain ~CFoo() if not virtual)

    void Update();                       // 0xADDR
    bool IsActive() const;               // 0xADDR

private: // Wrappers for hooks
    // 0xADDR
    CFoo* Constructor(int16 id) {
        this->CFoo::CFoo(id);
        return this;
    }

    // 0xADDR
    CFoo* Destructor() {
        this->CFoo::~CFoo();
        return this;
    }

    friend void InjectHooksMain(); // only because InjectHooks/wrappers are private
};
VALIDATE_SIZE(CFoo, 0x18);
VALIDATE_OFFSET(CFoo, m_Pos, 0x8); // for every member you rely on
```

Layout conventions (details in `docs/CodingGuidelines.md`): classes use `m_`/`ms_`, plain structs bare
`PascalCase`; fixed-width ints; `std::array` for arrays; SA types over RW types (except `RwMatrix`); bitfields for
flag bytes (`uint8 bFoo : 1;`); enums with an explicit underlying type matching the member width
(`enum eFooState : uint8`).

## 4. Source template

```cpp
#include "StdInc.h"

#include "Foo.h"

void CFoo::InjectHooks() {
    RH_ScopedVirtualClass(CFoo, 0xVTBL, 6); // RH_ScopedClass(CFoo) if not polymorphic
    RH_ScopedCategory("Entity/Object");     // follows the directory

    RH_ScopedInstall(Constructor, 0xADDR);
    RH_ScopedVMTDestructorInstall(0xADDR);  // or RH_ScopedInstall(Destructor, 0xADDR) for non-virtual dtors

    RH_ScopedInstall(Update, 0xADDR);
    RH_ScopedInstall(IsActive, 0xADDR, { .Reversed = false }); // still a stub
    RH_ScopedVMTInstall(Process, 0xADDR);
}

// 0xADDR
CFoo::CFoo(int16 id) :
    CFooBase{},
    m_Id{ id }
{
}

// 0xADDR
bool CFoo::IsActive() const {
    return plugin::CallMethodAndReturn<bool, 0xADDR, const CFoo*>(this);
}
```

Stubs for functions you are not reversing now (always installed with `{ .Reversed = false }`):
- member: `plugin::CallMethod<0xADDR, CFoo*, Args...>(this, args...);`
- member with return: `plugin::CallMethodAndReturn<Ret, 0xADDR, CFoo*, Args...>(this, args...);`
- static/free: `plugin::Call<0xADDR, Args...>(args...);` / `plugin::CallAndReturn<Ret, 0xADDR, Args...>(args...);`

Install macros for every case: **reverse-function** Phase 4.

## 5. Register the class

1. `#include "Foo.h"` in `source/InjectHooksMain.cpp`, near related includes.
2. `CFoo::InjectHooks();` inside `InjectHooksMain()`, in the matching subsystem group (base before derived).
3. `friend void InjectHooksMain();` only if `InjectHooks` or its wrappers are private.
4. New `.cpp` files are picked up by the `GLOB_RECURSE` in `source/CMakeLists.txt`; re-run the CMake configure step.

## 6. Verify

Done when:
- `python .claude/skills/review-reversal/scripts/check_hooks.py <Foo.cpp> <Foo.h>` → no `[E]` (it also checks
  `RH_ScopedVMTInstall` addresses against the GTA vtable: a wrong one throws at startup, R10-R11);
- every reversed function passes **validate-against-binary**;
- the build passes (`cmake --build build --config Debug --target gta_reversed`); `VALIDATE_SIZE/OFFSET` failures show
  up as `static_assert`s;
- the **gta-reversal-reviewer** agent has reviewed it.
