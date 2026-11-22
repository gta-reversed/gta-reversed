#include <StdInc.h>

#include "Commands.hpp"
#include <CommandParser/Parser.hpp>
#include "PathFind.h"
using namespace notsa::script;

/*!
* Various path commands
*/

namespace {

/// SWITCH_ROADS_ON(01E7)
void SwitchRoadsOn(float xA, float yA, float zA, float xB, float yB, float zB) {
    const auto xMin = std::min(xA, xB);
    const auto xMax = std::max(xA, xB);
    const auto yMin = std::min(yA, yB);
    const auto yMax = std::max(yA, yB);
    const auto zMin = std::min(zA, zB);
    const auto zMax = std::max(zA, zB);
    ThePaths.SwitchRoadsOffInArea(xMin, xMax, yMin, yMax, zMin, zMax, false, true, false);
}

/// SWITCH_ROADS_OFF(01E8)
void SwitchRoadsOff(float xA, float yA, float zA, float xB, float yB, float zB) {
    const auto xMin = std::min(xA, xB);
    const auto xMax = std::max(xA, xB);
    const auto yMin = std::min(yA, yB);
    const auto yMax = std::max(yA, yB);
    const auto zMin = std::min(zA, zB);
    const auto zMax = std::max(zA, zB);
    ThePaths.SwitchRoadsOffInArea(xMin, xMax, yMin, yMax, zMin, zMax, true, true, false);
}

/// SWITCH_ROADS_BACK_TO_ORIGINAL(091D)
void SwitchRoadsBackToOriginal(float xA, float yA, float zA, float xB, float yB, float zB) {
    const auto xMin = std::min(xA, xB);
    const auto xMax = std::max(xA, xB);
    const auto yMin = std::min(yA, yB);
    const auto yMax = std::max(yA, yB);
    const auto zMin = std::min(zA, zB);
    const auto zMax = std::max(zA, zB);
    ThePaths.SwitchRoadsOffInArea(xMin, xMax, yMin, yMax, zMin, zMax, false, true, true);
}

/// SWITCH_PED_ROADS_ON(022A)
void SwitchPedRoadsOn(float xA, float yA, float zA, float xB, float yB, float zB) {
    const auto xMin = std::min(xA, xB);
    const auto xMax = std::max(xA, xB);
    const auto yMin = std::min(yA, yB);
    const auto yMax = std::max(yA, yB);
    const auto zMin = std::min(zA, zB);
    const auto zMax = std::max(zA, zB);
    ThePaths.SwitchPedRoadsOffInArea(xMin, xMax, yMin, yMax, zMin, zMax, false, false);
}

/// SWITCH_PED_ROADS_OFF(022B)
void SwitchPedRoadsOff(float xA, float yA, float zA, float xB, float yB, float zB) {
    const auto xMin = std::min(xA, xB);
    const auto xMax = std::max(xA, xB);
    const auto yMin = std::min(yA, yB);
    const auto yMax = std::max(yA, yB);
    const auto zMin = std::min(zA, zB);
    const auto zMax = std::max(zA, zB);
    ThePaths.SwitchPedRoadsOffInArea(xMin, xMax, yMin, yMax, zMin, zMax, true, false);
}

/// SWITCH_PED_ROADS_BACK_TO_ORIGINAL(091E)
void SwitchPedRoadsBackToOriginal(float xA, float yA, float zA, float xB, float yB, float zB) {
    const auto xMin = std::min(xA, xB);
    const auto xMax = std::max(xA, xB);
    const auto yMin = std::min(yA, yB);
    const auto yMax = std::max(yA, yB);
    const auto zMin = std::min(zA, zB);
    const auto zMax = std::max(zA, zB);
    ThePaths.SwitchPedRoadsOffInArea(xMin, xMax, yMin, yMax, zMin, zMax, false, true);
}

};

void notsa::script::commands::path::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Path");

    REGISTER_COMMAND_HANDLER(COMMAND_SWITCH_ROADS_ON, SwitchRoadsOn);
    REGISTER_COMMAND_HANDLER(COMMAND_SWITCH_ROADS_OFF, SwitchRoadsOff);
    REGISTER_COMMAND_HANDLER(COMMAND_SWITCH_ROADS_BACK_TO_ORIGINAL, SwitchRoadsBackToOriginal);
    REGISTER_COMMAND_HANDLER(COMMAND_SWITCH_PED_ROADS_ON, SwitchPedRoadsOn);
    REGISTER_COMMAND_HANDLER(COMMAND_SWITCH_PED_ROADS_OFF, SwitchPedRoadsOff);
    REGISTER_COMMAND_HANDLER(COMMAND_SWITCH_PED_ROADS_BACK_TO_ORIGINAL, SwitchPedRoadsBackToOriginal);
}
