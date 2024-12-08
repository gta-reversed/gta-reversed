#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>
#include "TaskSimplePlayerOnFoot.h"
#include "eScriptCommands.h"
#include "RunningScript.h"
#include "OpcodeResult.h"
#include "RunningScript.h"
#include "Cheat.h"
#include "CarGenerator.h"
#include "TheCarGenerators.h"
#include "FireManager.h"
#include "CommandParser/Parser.hpp"

void notsa::script::commands::generic::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Generic");
}
