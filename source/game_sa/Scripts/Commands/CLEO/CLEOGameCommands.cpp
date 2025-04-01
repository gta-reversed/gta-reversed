#include <StdInc.h>

#include "CLEOCommands.hpp"
#include <CommandParser/Parser.hpp>
#include "CommandParser/Parser.hpp"

/*!
* Various CLEO game commands
*/

bool IsGameVersionOriginal() {
    return false;
}

void notsa::script::commands::cleo::game::RegisterHandlers() {
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_GAME_VERSION_ORIGINAL, IsGameVersionOriginal);
}
