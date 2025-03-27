#pragma once

// Disables many extensions and non-vanilla features
//#define BAREBONES

#define BUILD_PC
#define USE_BUILD_INFORMATION

#undef TRACY_ENABLE // TODO

#ifndef BAREBONES

#define MORE_LANGUAGES
#define USE_ORIGINAL_CODE
#define USE_ADDITIONAL_CHEATS

// Enables opcodes from III/VC that are not implemented in SA
#define IMPLEMENT_UNSUPPORTED_OPCODES

// Extensions
#define EXT_FAST_LOADER

// do not uncomment -- doesn't work rn
// #define USE_OPENAL

#endif // !BAREBONES

#ifdef BUILD_PC
//#define USE_EU_STUFF
#endif
