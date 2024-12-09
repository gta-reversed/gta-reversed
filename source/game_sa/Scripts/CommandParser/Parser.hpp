#pragma once

#include <utility>
#include "StoreArg.hpp"
#include "ReadArg.hpp"
#include "RunningScript.h"

#ifdef ENABLE_SCRIPT_COMMAND_HOOKS
#include <ScriptCommand.h>
#endif

class CRunningScript;

namespace notsa {
namespace script {
namespace detail {
template<typename T_FnRet, typename... T_FnArgs, typename... T_CollectedArgs>
inline OpcodeResult CollectArgsAndCall(CRunningScript* S, eScriptCommands command, T_FnRet(*CommandFn)(T_FnArgs...), T_CollectedArgs&&... args) {
    if constexpr (sizeof...(T_CollectedArgs) == sizeof...(T_FnArgs)) { // Has it collected enough args?
        const auto CallCommandFn = [&]() -> T_FnRet {
            return std::invoke(CommandFn, std::forward<T_CollectedArgs>(args)...);
        };

        if constexpr (std::is_same_v<T_FnRet, void>) {
            CallCommandFn(); // Returns void, nothing to push
        } else {
            T_FnRet ret = CallCommandFn();
            if constexpr (std::is_same_v<T_FnRet, OpcodeResult>) { 
                return ret;
            } else {
                StoreArg(S, std::forward<T_FnRet>(ret)); // Store result to script
            }
        }

        return OR_CONTINUE;
    } else { // Not enough args, read more
        using T_ToRead = nth_element_t<sizeof...(T_CollectedArgs), T_FnArgs...>;
        const auto ContinueWithArg = [&]<typename T>(T&& arg) {
            return CollectArgsAndCall(
                S,
                command,
                CommandFn,
                std::forward<T_CollectedArgs>(args)...,
                std::forward<T>(arg)
            );
        };

        if constexpr (std::is_same_v<T_ToRead, eScriptCommands>) { // Special case for reading the current script comamnd
            return ContinueWithArg(command);
        } else {
            return ContinueWithArg(notsa::script::Read<T_ToRead>(S)); // Read next parameter and continue
        }
    
        // Don't remove this, we might need it!
        //try {
        //} catch (const std::exception&) {
            // In cases the argument parser has errored out we still
            // have to read all values before returning.
            // Reason: The IP has to be increased
            //((void)(notsa::script::Read<T_FnArgs>(S)), ...);
            //return OR_CONTINUE;
        //}
    }
}

//! That is, ones that aren't used anywhere.
//! If this ever gets called, that means that the command is used after all, and shouldn't be hooked as unimplemented.
inline auto NotImplemented(CRunningScript& S, eScriptCommands cmd) {
    DEV_LOG("[{}][IP: {:#x} + {:#x}]: Unimplemented command has been called! [ID: {:04X}; Name: {}]", S.m_szName, LOG_PTR(S.m_pBaseIP), LOG_PTR(S.m_IP - S.m_pBaseIP), (unsigned)(cmd), GetScriptCommandName(cmd));
    NOTSA_DEBUGBREAK(); // Something went horribly wrong here, and the game will crash after this [if the function is supposed to take/return any arguments], so better stop here.
    return OR_INTERRUPT; // Vanilla SA behavior
}

template<eScriptCommands Command, auto* CommandFn>
inline OpcodeResult CommandParser(CRunningScript* S) {
    CRunningScript::ScriptArgCharNextFreeBuffer = 0;
    return detail::CollectArgsAndCall(S, Command, CommandFn);
} 

template<eScriptCommands Command, auto* CommandFn>
inline void AddCommandHandler() {
    auto& entry = CRunningScript::CustomCommandHandlerOf(Command);
    // Make sure it doesn't already have a handler
    if (entry) {
        NOTSA_UNREACHABLE("Command already handled!\nOpcode: {:04X} ({})", (unsigned)(Command), GetScriptCommandName(Command));
    }
    entry = &CommandParser<Command, CommandFn>; // Register handler
}
}; // namespace detail
}; // namespace script
}; // namespace notsa

template<eScriptCommands Command>
struct CommandHandler : std::false_type {};

namespace notsa {
namespace detail {
//! Returns raw function pointer of a function or lambda.
//! The `+` operator is a hack to get lambda's function pointer.
//! The `+` is a no-op in this case (As in, it won't increment the value, or anything like that, is just merely forces a decay to a pointer [from a lambda object instance])
//! See: https://stackoverflow.com/questions/18889028
template<typename T>
constexpr inline auto AddressOfFunction(T fn) {
    return +(fn);
}
};
};

//! Register a custom command handler
#ifdef ENABLE_SCRIPT_COMMAND_HOOKS
//! Use this before calling any of the 
#define REGISTER_COMMAND_HANDLER_BEGIN(_namespace) \
    RH_ScopedCategory("Scripts/Commands") \
    RH_ScopedNamespaceName(_namespace)
#define REGISTER_COMMAND_HANDLER(cmd, fn) \
    do { \
        ::notsa::script::detail::AddCommandHandler<cmd, ::notsa::detail::AddressOfFunction(fn)>(); \
        RH_ScopedInstallScriptCommand(cmd); \
    } while (0)
#else
#define REGISTER_COMMAND_HANDLER_BEGIN(_namespace) \
    ((void)(_namespace))
#define REGISTER_COMMAND_HANDLER(cmd, fn) \
    ::notsa::script::detail::AddCommandHandler<cmd, ::notsa::detail::AddressOfFunction(fn)>()
#endif

//! Register a command handler for an unimplemented command (That is, a command that wasn't implemented in the game either)
#define REGISTER_COMMAND_UNIMPLEMENTED(cmd) \
    REGISTER_COMMAND_HANDLER(cmd, ::notsa::script::detail::NotImplemented)

#ifdef IMPLEMENT_UNSUPPORTED_OPCODES
//! Register a custom command handler for an un-implemented command. (Won't be implemented if IMPLEMENT_UNSUPPORTED_OPCODES is not defined.)
#define REGISTER_UNSUPPORTED_COMMAND_HANDLER(cmd, fn) \
    REGISTER_COMMAND_HANDLER(cmd, fn)
#else
//! Register a custom command handler for an unimplmented command. (Not implemented if IMPLEMENT_UNSUPPORTED_OPCODES is not defined.)
#define REGISTER_UNSUPPORTED_COMMAND_HANDLER(cmd, fn) \
    REGISTER_COMMAND_HANDLER(cmd, ::notsa::script::detail::NotImplemented)
#endif
