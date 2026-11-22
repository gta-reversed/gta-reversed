#pragma once

#include "DebugModule.h"

namespace notsa { 
namespace debugmodules {
class CloudsDebugModule final : public DebugModule {
public:
    void RenderWindow() override;
    void RenderMenuEntry() override; 

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(CloudsDebugModule, m_IsOpen);
private:
    bool m_IsOpen{};
};
}; // namespace debugmodules
}; // namespace notsa
