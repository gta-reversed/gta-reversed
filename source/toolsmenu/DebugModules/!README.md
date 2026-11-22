# How to add a new debug module
- 1. Create a file in this directory (.hpp and .cpp), the name being the name of your module (Ex.: `YourModuleNameDebugModule.cpp`, gotta have the `DebugModule` suffix for now)
- 2. Write code for the module (See templates below)
- 3. Go to "DebugModules::CreateModules" and add your module to there
- 4. Profit

# How to structure code

Header file (.hpp):
```cpp
#pragma once

#include "DebugModule.h"

namespace notsa { 
namespace debugmodules {
class Rendering /*No "DebugModule" suffix needed */ final : public DebugModule {
public:
    void RenderWindow() override;
    void RenderMenuEntry() override; 
    void Render3D() override;
    // There's also `void Update()`, but you'll rarely need that one

private:
    bool m_IsOpen{};        // Main window's state
    bool m_ShowClosest{};
};
}; // namespace debugmodules
}; // namespace notsa
```

Source file (.cpp):
```cpp
#include <StdInc.h>
#include "Rendering.hpp"

namespace notsa { 
namespace debugmodules {

// If you want to render a window (or more), this is the place to do it!
void Rendering::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "Rendering", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }
    // Your window's content here
}

// Render the menu entry of this module here. This function is called witihn `BeginMainMenu`, so you don't need to call it yourself
void Rendering::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Visualization", "Rendering" }, [&] { // Visualization -> Rendering 
        ImGui::Checkbox("Show Closest", &m_ShowClosest);   // Show a checkbox if the "Rendering" sub-menu is open
    });

    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] { // Extra menu
        ImGui::MenuItem("Rendering", nullptr, &m_IsOpen); // Toggles the window
    });
}

void Rendering::Render3D() {
    // Render your 3D stuff here (if any)
}
}; // namespace debugmodules
}; // namespace notsa
```

## Template (To copy for new modules)

Header file (`YourModuleNameDebugModule.hpp`):
```cpp
#pragma once

#include "DebugModule.h"

namespace notsa { 
namespace debugmodules {
class YourModuleName final : public DebugModule {
public:
    void RenderWindow() override;
    void RenderMenuEntry() override; 

private:
    bool m_IsOpen{};
};
}; // namespace debugmodules
}; // namespace notsa
```

Source file (`YourModuleNameDebugModule.cpp`):
```cpp
#include <StdInc.h>
#include "YourModuleName.hpp"

namespace notsa { 
namespace debugmodules {
void YourModuleName::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "YourModuleName", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }
    // Your window's content here
}

void YourModuleName::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "YourModuleCategory" }, [&] {
        ImGui::MenuItem("YourModuleName", nullptr, &m_IsOpen);
    });
}
}; // namespace debugmodules
}; // namespace notsa
```
