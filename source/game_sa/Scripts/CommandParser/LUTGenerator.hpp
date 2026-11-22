#pragma once

#include <utility>
#include <array>
#include <functional>

#include "extensions/utility.hpp"
#include "Parser.hpp"
#include "eScriptCommands.h"
#include "RunningScript.h"

#if 0
namespace notsa {
namespace script {
/// Type of the function used in the LUT
using T_LUTFunction = OpcodeResult(*)(CRunningScript*);


//! Whenever command has a handler made by us (or uses the default GTA one)
template<eScriptCommands Command>
static constexpr auto CommandHasCustomHandler() {
    return CommandHandler<Command>::value;
}

namespace detail {

/// Wrap `CRunningScript::ProcessCommand` into a type function with type `T_LUTFunction`
template<eScriptCommands Command>
static auto ProcessCommandMemFnWrapper(CRunningScript* S) {
    return std::invoke(&CRunningScript::ProcessCommand<Command>, S);
}
}; // namespace detail

/*!
* Wrapper for command that is disptched to the original GTA command process handler.
* NOTE: We can't yet use this inplace of `CRunningScript::ProcessCommand` as some commands
*       just specialize `ProcessCommand` instead of using `CommandHandler`.
*/
template<eScriptCommands Command>
static auto GTAProcessCommand(CRunningScript* S) {
    return std::invoke(CRunningScript::CommandHandlerTable[(size_t)Command / 100], S, Command);
}

/// Get the function for the command that is to the `CommandParser
template<eScriptCommands Command>
static constexpr auto GetHandlerOfCommand() {
    // Check if there's a registered command handler
    using RegisteredCommandHandler = CommandHandler<Command>;
    if constexpr (RegisteredCommandHandler::value) {
        return &CommandParser<Command, RegisteredCommandHandler::Function>;
    } else { // Otherwise use base `ProcessCommand`, eventually (ideally) we'll get rid of these commands (by converting them to parsed commands instead)
        return &CommandParser<Command, detail::ProcessCommandMemFnWrapper<Command>>;
    }
}

constexpr void IterateCommandIDs(auto&& functor) {
    ::notsa::IterateFunction<0, (int)COMMAND_HIGHEST_ID_TO_HOOK>(functor);
}

/// Generate the script command handler look-up-table (LUT)
static constexpr auto GenerateLUT() {
    std::array<T_LUTFunction, (size_t)COMMAND_HIGHEST_ID_TO_HOOK> lut{};

    IterateCommandIDs([&]<size_t Idx>() {
        lut[Idx] = GetHandlerOfCommand<(eScriptCommands)Idx>();
    });

    return lut;
}

}; // namespace script
}; // namespace notsa 
#endif
