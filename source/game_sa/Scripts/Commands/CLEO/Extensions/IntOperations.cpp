#include <StdInc.h>

#include "Commands.hpp"
#include <CommandParser/Parser.hpp>
#include "CommandParser/Parser.hpp"
/*!
 * Various int operations (bitwise and modulo)
 */

auto BitAnd(uint32 a, uint32 b) {
    return a & b; 
}
auto BitwiseAndThingWithThing(uint32& a, uint32 b) {
    a &= b; 
}

auto BitOr(uint32 a, uint32 b) {
    return a | b; 
}
auto BitwiseOrThingWithThing(uint32& a, uint32 b) {
    a |= b; 
}

auto BitXor(uint32 a, uint32 b) {
    return a ^ b; 
}
auto BitwiseXorThingWithThing(uint32& a, uint32 b) {
    a ^= b; 
}

auto BitNot(uint32 a) {
    return ~a; 
}

auto BitShl(uint32 a, uint32 b) {
    return a << b; 
}
auto BitwiseShlThingByThing(uint32& a, uint32 b) {
    a <<= b; 
}

auto BitShr(uint32 a, uint32 b) {
    return a >> b; 
}
auto BitwiseShrThingByThing(uint32& a, uint32 b) {
    a >>= b; 
}

auto BitMod(uint32 a, uint32 b) {
    return a % b; 
}
auto BitwiseModThingByThing(uint32& a, uint32 b) {
    a %= b; 
}

void notsa::script::commands::cleo::extensions::intoperations::RegisterHandlers() {
    /*
    REGISTER_COMMAND_HANDLER(COMMAND_BIT_AND, BitAnd);
    REGISTER_COMMAND_HANDLER(COMMAND_BITWISE_AND_THING_WITH_THING, BitwiseAndThingWithThing);
    REGISTER_COMMAND_HANDLER(COMMAND_BIT_OR, BitOr);
    REGISTER_COMMAND_HANDLER(COMMAND_BITWISE_OR_THING_WITH_THING, BitwiseOrThingWithThing);
    REGISTER_COMMAND_HANDLER(COMMAND_BIT_XOR, BitXor);
    REGISTER_COMMAND_HANDLER(COMMAND_BITWISE_XOR_THING_WITH_THING, BitwiseXorThingWithThing);
    REGISTER_COMMAND_HANDLER(COMMAND_BIT_NOT, BitNot);
    REGISTER_COMMAND_HANDLER(COMMAND_BIT_SHL, BitShl);
    REGISTER_COMMAND_HANDLER(COMMAND_BITWISE_SHL_THING_BY_THING, BitwiseShlThingByThing);
    REGISTER_COMMAND_HANDLER(COMMAND_BIT_SHR, BitShr);
    REGISTER_COMMAND_HANDLER(COMMAND_BITWISE_SHR_THING_BY_THING, BitwiseShrThingByThing);
    REGISTER_COMMAND_HANDLER(COMMAND_BIT_MOD, BitMod);
    REGISTER_COMMAND_HANDLER(COMMAND_BITWISE_MOD_THING_BY_THING, BitwiseModThingByThing);
    */
}
