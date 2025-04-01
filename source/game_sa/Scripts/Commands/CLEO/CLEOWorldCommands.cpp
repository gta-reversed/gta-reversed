#include <StdInc.h>

#include "CLEOCommands.hpp"
#include <CommandParser/Parser.hpp>

#include "Radar.h"
#include "MenuManager.h"
#include "CommandParser/Parser.hpp"

/*!
* Various CLEO world commands
*/

CVector GetCheckpointBlipCoords(CRunningScript* S) {
    S->m_CondResult = false;

    const auto blipIdx = CRadar::GetActualBlipArrayIndex(FrontEndMenuManager.m_nTargetBlipIndex);
    if (blipIdx == -1)
        return {};

    if (const auto& trace = CRadar::ms_RadarTrace[blipIdx]; trace.m_nBlipDisplayFlag != BLIP_DISPLAY_NEITHER) {
        S->m_CondResult = true;
        return trace.m_vPosition;
    }

    return {};
}

void notsa::script::commands::cleo::world::RegisterHandlers() {
    /*
    REGISTER_COMMAND_HANDLER(COMMAND_GET_TARGET_BLIP_COORDS, GetCheckpointBlipCoords);
    */
}
