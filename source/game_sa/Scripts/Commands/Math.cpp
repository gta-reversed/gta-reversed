#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>
#include "CommandParser/Parser.hpp"

/*!
* Various utility commands
*/
namespace {
auto GetDistanceBetweenCoords2d(CVector2D a, CVector2D b) {
    return DistanceBetweenPoints2D(a, b);
}

auto GetDistanceBetweenCoords3d(CVector a, CVector b) {
    return DistanceBetweenPoints(a, b);
}

auto GenerateRandomInt(int32& var) {
    var = CGeneral::GetRandomNumber();
}

auto GenerateRandomIntInRange(int32 a, int32 b) {
    return CGeneral::GetRandomNumberInRange(a, b);
}

auto GenerateRandomFloatInRange(float a, float b) {
    return CGeneral::GetRandomNumberInRange(a, b);
}

auto Sin(float deg) {
    return std::sinf(DegreesToRadians(deg));
}

auto Cos(float deg) {
    return std::cosf(DegreesToRadians(deg));
}
};

void notsa::script::commands::math::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Math");

    REGISTER_COMMAND_HANDLER(COMMAND_GET_DISTANCE_BETWEEN_COORDS_2D, GetDistanceBetweenCoords2d);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_DISTANCE_BETWEEN_COORDS_3D, GetDistanceBetweenCoords3d);
    REGISTER_COMMAND_HANDLER(COMMAND_GENERATE_RANDOM_INT, GenerateRandomInt);
    REGISTER_COMMAND_HANDLER(COMMAND_GENERATE_RANDOM_INT_IN_RANGE, GenerateRandomIntInRange);
    REGISTER_COMMAND_HANDLER(COMMAND_GENERATE_RANDOM_FLOAT_IN_RANGE, GenerateRandomFloatInRange);
    REGISTER_COMMAND_HANDLER(COMMAND_SIN, Sin);
    REGISTER_COMMAND_HANDLER(COMMAND_COS, Cos);
}
