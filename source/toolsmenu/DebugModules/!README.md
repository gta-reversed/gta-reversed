# DebugModules/ — the F7 debug menu modules

Guide: [../../../docs/Architecture.md](../../../docs/Architecture.md) §6 "Debug UI". Modules are NOTSA tools (ImGui),
not reversed game code.

## Layout
- `DebugModule.h` — `DebugModule` base class. Virtuals: `RenderMenuEntry()` (pure), `RenderWindow()`, `Render3D()`,
  `Update()` (once per frame, pre-render), `OnImGuiInitialised(ctx)`, plus serialization (`Serialize`, `Deserialize`,
  `OnDeserialized`, `GetID`). `DebugModuleSingleWindow` is a helper base for a module with one window.
- `DebugModules.h/.cpp` — `DebugModules`: owns the modules, `CreateModules()` adds them (grouped by menu: Tools,
  Settings, Visualization, Stats, Extra), drives update/render and saves their state.
- `../Utility.h` — `notsa::ui` helpers: `ScopedWindow`, `ScopedChild`, `ScopedID`, `ScopedDisable`, `DoNestedMenuIL`.
- One `XxxDebugModule.h/.hpp` + `.cpp` pair per module; bigger ones have a subfolder (`Audio/`, `Script/`,
  `HooksDebugModule/`).

## Conventions
1. Create `YourNameDebugModule.hpp` and `.cpp` here (the CMake glob picks them up; re-run the configure step).
2. Derive from `DebugModule` in `namespace notsa::debugmodules`, as in the template below.
3. Add `Add<notsa::debugmodules::YourNameDebugModule>();` in `DebugModules::CreateModules()` under the right menu,
   and include its header at the top of `DebugModules.cpp`.

```cpp
// YourNameDebugModule.hpp
#pragma once
#include "DebugModule.h"

namespace notsa {
namespace debugmodules {
class YourNameDebugModule final : public DebugModule {
public:
    void RenderWindow() override;
    void RenderMenuEntry() override;
    void Render3D() override; // Only if you draw in the world (lines, etc.)

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(YourNameDebugModule, m_IsOpen); // Optional: persists the listed members

private:
    bool m_IsOpen{};
};
}; // namespace debugmodules
}; // namespace notsa

// YourNameDebugModule.cpp
#include <StdInc.h>
#include "YourNameDebugModule.hpp"

namespace notsa {
namespace debugmodules {
void YourNameDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "Your name", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }
    // Window content
}

void YourNameDebugModule::RenderMenuEntry() { // Called inside the main menu bar; don't begin it yourself
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Your name", nullptr, &m_IsOpen);
    });
}

void YourNameDebugModule::Render3D() {
}
}; // namespace debugmodules
}; // namespace notsa
```

## Pitfalls
- Draw world-space things only in `Render3D()`; elsewhere they (probably) won't be visible.
- Serialized state goes to `DebugModules.json` in the working directory (`<game>/scripts/` under the ASI loader) every
  5 s. Modules without `GetID()` (no serialization macro) are not saved. A corrupt file is backed up and ignored.
- Keep members listed in `NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION` JSON-serializable (nlohmann).
- Modules run every frame: don't call game functions with side effects from `Update()` / `Render*()` unless the tool
  is meant to change the game.
