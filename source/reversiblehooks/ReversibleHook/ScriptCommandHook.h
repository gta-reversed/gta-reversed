#pragma once 

#ifdef NOTSA_WITH_SCRIPT_COMMAND_HOOKS
#include "eScriptCommands.h"
#include "RunningScript.h"

#include "TwoWayHook.h"

namespace ReversibleHooks {
namespace ReversibleHook {
/*!
 * @brief Script commands hook
 * @note  There's no real `unhooked` state, because either all calls go to our handler or to the gta one, no in-between
 */
struct ScriptCommandHook final : public TwoWayHook {
    ScriptCommandHook(eScriptCommands command) :
        TwoWayHook{ std::string{::notsa::script::GetScriptCommandName(command)} },
        m_Command{command},
        m_OriginalHandler{CRunningScript::CustomCommandHandlerOf(command)}
    {
    }

    ~ScriptCommandHook() override {
        State(TwoWayHookState::Unhooked);
    }

    HookType Type() const noexcept override { return HookType::ScriptCommand; }
    void     Check() override { /* nop */ }
    void     Serialize(json& j) const override { to_json(j, *this); }

    void* GetHookAddressGTA() const noexcept override { return nullptr; }
    void* GetHookAddressOur() const noexcept override { return m_OriginalHandler; }

protected:
    void ApplyNewState(TwoWayHookState state, TwoWayHookState oldState) override {
        CRunningScript::CustomCommandHandlerOf(m_Command) = state == TwoWayHookState::RedirectToOurs
            ? m_OriginalHandler 
            : nullptr;
    }

public:
    friend void to_json(json& j, const ScriptCommandHook& hook) {
        to_json(j, static_cast<const TwoWayHook&>(hook));
        j["Command"] = ::notsa::script::GetScriptCommandName(hook.m_Command);
    }

private:
    eScriptCommands                         m_Command{};
    ::notsa::script::CommandHandlerFunction m_OriginalHandler{};
};
};
};
#endif
