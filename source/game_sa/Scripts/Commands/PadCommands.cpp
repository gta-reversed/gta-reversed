#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>
#include "MenuManager.h"
#include "Pad.h"
#include "CommandParser/Parser.hpp"
using namespace notsa::script;
/*!
* Various player pad commands
*/

namespace {
bool IsButtonPressed(CRunningScript* S, int16 playerIndex, eButtonId button) {
    return S->GetPadState(playerIndex, button) && !CPad::GetPad(0)->JustOutOfFrontEnd;
}

MultiRet<float, float> GetPCMouseMovement() {
    // TODO(izzotop): check ASM
    return {CPad::NewMouseControllerState.m_AmountMoved.x, CPad::NewMouseControllerState.m_AmountMoved.y};
}

bool IsPCUsingJoyPad() {
    return FrontEndMenuManager.m_ControlMethod;
}

bool IsMouseUsingVerticalInversion() {
    return CMenuManager::bInvertMouseY != 0;
}

bool HasGameJustReturnedFromFrontend() {
    return CPad::GetPad(0)->JustOutOfFrontEnd;
}
};

void notsa::script::commands::pad::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Pad");

    REGISTER_COMMAND_HANDLER(COMMAND_IS_BUTTON_PRESSED, IsButtonPressed);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_PC_MOUSE_MOVEMENT, GetPCMouseMovement);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PC_USING_JOYPAD, IsPCUsingJoyPad);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_MOUSE_USING_VERTICAL_INVERSION, IsMouseUsingVerticalInversion);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_GAME_JUST_RETURNED_FROM_FRONTEND, HasGameJustReturnedFromFrontend);
}
