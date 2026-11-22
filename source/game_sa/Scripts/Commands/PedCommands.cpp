#include <StdInc.h>

#include "Commands.hpp"
#include <CommandParser/Parser.hpp>
#include "Ped.h"
#include "CommandParser/Parser.hpp"

/*!
* Various ped commands (TODO: Isn't this the same as Chars.cpp?)
*/
namespace {
void SetDeathWeaponPersist(CPed& ped, bool enable) {
    ped.bDeathPickupsPersist = enable;
}

void IgnoreHeightDifferenceFollowingNodes(CPed& ped, bool ignore) {
    ped.bIgnoreHeightDifferenceFollowingNodes = ignore;
}

void ShutAllCharsUp(bool disable) {
    (disable ? CAEPedSpeechAudioEntity::DisableAllPedSpeech : CAEPedSpeechAudioEntity::EnableAllPedSpeech)();
}

void SetCharGetOutUpsideDownCar(CPed& ped, bool enable) {
    ped.bGetOutUpsideDownCar = enable;
}
};

void notsa::script::commands::ped::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Ped");

    REGISTER_COMMAND_HANDLER(COMMAND_SET_DEATH_WEAPONS_PERSIST, SetDeathWeaponPersist);
    REGISTER_COMMAND_HANDLER(COMMAND_IGNORE_HEIGHT_DIFFERENCE_FOLLOWING_NODES, IgnoreHeightDifferenceFollowingNodes);
    REGISTER_COMMAND_HANDLER(COMMAND_SHUT_ALL_CHARS_UP, ShutAllCharsUp);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_GET_OUT_UPSIDE_DOWN_CAR, SetCharGetOutUpsideDownCar);
}
