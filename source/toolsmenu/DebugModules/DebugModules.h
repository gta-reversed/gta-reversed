#pragma once

#include <chrono>
#include <imgui.h>
#include "DebugModule.h"

class DebugModules {
    static constexpr time_t MODULE_SERIALIZATION_INTERVAL_MS = 5000; //!< How often to serialize the debug module states and save them to the disk

public:
    DebugModules(ImGuiContext* ctx);
    ~DebugModules();

    //! Pre-Render updates
    void PreRenderUpdate();

    //! Process stuff (Including rendering)
    void Render2D();

    //! Process 3D rendering
    void Render3D();

private:
    //! Creates all modules
    void CreateModules();

    //! Render menu bar stuff (FPS, etc)
    void RenderMenuBarInfo();

    //! Add a new module
    template<std::derived_from<DebugModule> T>
    void Add() {
        auto& m = m_Modules.emplace_back(std::make_unique<T>());
        m->OnImGuiInitialised(m_ImCtx);
    }

    //! Serialize the state of all debug modules
    void DoSerializeModules();

    //! Restore state of modules from serialized state
    void DoDeserializeModules();

private:
    std::vector<std::unique_ptr<DebugModule>> m_Modules{};
    ImGuiContext*                             m_ImCtx{};
    time_t                                    m_LastSerializationTimeMs{};
};
