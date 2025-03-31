#include <StdInc.h>

#include "Commands.hpp"
#include <CommandParser/Parser.hpp>
#include "CommandParser/Parser.hpp"

/*!
* Various player pad commands
*/

bool IsKeyPressed(int32 virtualKey) {
    return GetKeyState(virtualKey) & 0x8000;
}

void notsa::script::commands::cleo::pad::RegisterHandlers() {
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_KEY_PRESSED, IsKeyPressed);
}
