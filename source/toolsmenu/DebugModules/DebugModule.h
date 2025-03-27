#pragma once

// NOTE: Ideally we'd use imconfig.h, but it's too finnicky
#define IM_VEC2_CLASS_EXTRA \
    operator CVector2D() const { return {x, y}; } \
    ImVec2(const CVector2D& v) : x{v.x}, y{v.y} {} \

#include <imgui.h>
#include "../Utility.h" // TODO Remove this and add it individually to all places this headear is included in

class DebugModule {
public:
    virtual ~DebugModule() = default;

    //! Called once on ImGUI initialization
    virtual void OnImGuiInitialised(ImGuiContext* ctx) { /*nothing*/ }
    
    //! Called once every frame (pre-render)
    virtual void Update() { /*nothing*/ }

    //! Render 3D stuff here (lines, etc...). If rendered elsewhere they (probably) won't be visible.
    virtual void Render3D() { /*nothing*/ };

    //! Module's window should be rendered here
    virtual void RenderWindow() { /*nothing*/ }

    //! Module's entry in the main menu should be rendered here
    virtual void RenderMenuEntry() = 0;

    //! Serialize the state of this tool
    virtual json Serialize() const { return {}; };

    //! Restore state from serialization
    virtual void Deserialize(const json&) { /*nothing*/ };

    //! Get the ID of this module (Used for serialization)
    virtual std::string_view GetID() const { return ""; }
};

//! You can use this macro for implementing the boilerplate needed for
//! serialization support
//! Example call: `NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(TeleportDebugModule, m_IsOpen);`
#define NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(_cls, ...) \
    json Serialize() const final override           { return *this; } \
    void Deserialize(const json& j) final override  { from_json(j, *this); } \
    std::string_view GetID() const final override   { return #_cls; }  \
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(_cls, __VA_ARGS__) \

//! Class representing a debug module with a single window
class DebugModuleSingleWindow : public DebugModule {
public:
    DebugModuleSingleWindow(const char* wndName, ImVec2 wndDefaultSize) :
        m_wndName{ wndName },
        m_wndDefaultSize{ wndDefaultSize }
    {
    }

    //! Override this for your single main window
    virtual void RenderMainWindow() = 0;

    void RenderWindow() override final;

protected:
    void SetMainWindowOpen(bool open) { m_wndIsOpen = open; }

private:
    ImVec2      m_wndDefaultSize{};
    const char* m_wndName{};
    bool        m_wndIsOpen{};
};
