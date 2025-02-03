#pragma once

#include "DebugModule.h"

namespace notsa { 
namespace debugmodules {
class VehicleAudioEntityDebugModule final : public DebugModule {
public:
    void RenderWindow() override;
    void RenderMenuEntry() override;
    void Update() override;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(VehicleAudioEntityDebugModule, m_IsOpen);

private:
    void RenderMemberVars();
    void RenderGlobalVars();

private:
    bool          m_IsOpen{};
    CVehicle::Ref m_Veh{};
};
}; // namespace debugmodules
}; // namespace notsa
