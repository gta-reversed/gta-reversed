#pragma once 

#include "Base.h"
#include "eScriptCommands.h"
#include "RunningScript.h"

#ifdef ENABLE_SCRIPT_COMMAND_HOOKS
namespace ReversibleHooks {
namespace ReversibleHook {
struct ScriptCommand : Base {
    ScriptCommand(eScriptCommands command, bool locked = false, bool enabledByDefault = true) :
        Base{ std::string{::notsa::script::GetScriptCommandName(command)}, Base::HookType::ScriptCommand, locked },
        m_cmd{command},
        m_originalHandler{CRunningScript::CustomCommandHandlerOf(command)}
    {
        m_bIsHooked = true; // Enabled by default
        if (m_bIsHooked && !enabledByDefault) {
            Switch(); // Uninstall it
        }
    }

    ~ScriptCommand() override = default;

    void Switch() override {
        using namespace ::notsa::script;

        m_bIsHooked = !m_bIsHooked;
        CRunningScript::CustomCommandHandlerOf(m_cmd) = m_bIsHooked ? m_originalHandler : nullptr;
    }

    void        Check() override { /* nop */ }
    const char* Symbol() const override { return "C"; }
private:
    eScriptCommands                         m_cmd{};
    ::notsa::script::CommandHandlerFunction m_originalHandler{};
};
};
};
#endif
