/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "app/app_debug.h"
#include <rw/rwplcore.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define PLUGIN_API

#define VALIDATE_SIZE(struc, size) static_assert(sizeof(struc) == size, "Invalid structure size of " #struc)
#define VALIDATE_OFFSET(struc, member, offset) \
	static_assert(offsetof(struc, member) == offset, "The offset of " #member " in " #struc " is not " #offset "...")

VALIDATE_SIZE(bool, 1);
VALIDATE_SIZE(char, 1);
VALIDATE_SIZE(short, 2);
VALIDATE_SIZE(int, 4);
VALIDATE_SIZE(float, 4);
VALIDATE_SIZE(long long, 8);

// Use this to add const that wasn't there in the original code
#define Const const

// Basic types for structures describing
typedef int8_t    int8;
typedef int16_t   int16;
typedef int32_t   int32;
typedef int64_t   int64;
typedef uint8_t   uint8;
typedef uint16_t  uint16;
typedef uint32_t  uint32;
typedef uint64_t  uint64;
typedef intptr_t  intptr;
typedef uintptr_t uintptr;

typedef uint8     bool8;
typedef uint16    bool16;
typedef uint32    bool32;

#if (defined(__GNUC__) || defined(__GNUG__) || defined(__clang__))
#define UNREACHABLE_INTRINSIC(...) __builtin_unreachable()
#elif (defined(_MSC_VER))
#define UNREACHABLE_INTRINSIC(...) __assume(false)
#else
#define UNREACHABLE_INTRINSIC(...) assert(false)
#endif

// Use the `NOTSA_UNREACHABLE` macro for unreachable code paths.
// In debug mode it will do a DebugBreak() and print a message to the console,
// while in release code it'll be optimized away (by using special compiler directives)
// Also serves as a way to supress compiler warnings (for example, when you don't have need a `default` case in a `switch`)
#if _DEBUG
#include <format>
#include <winuser.h>

namespace notsa {
static const fs::path SOURCE_PATH = fs::path(__FILE__).parent_path();

template<typename... Ts>
[[noreturn]] static void unreachable(std::string_view method, std::string_view file, unsigned line, std::string userDetails = "<None provided>") {
    const auto mbMsg = std::format(
        "File: {}\nIn: {}:{}\n\nDetails:\n{}",
        fs::relative(file, SOURCE_PATH).string(),
        method,
        line,
        userDetails
    );

    spdlog::error(mbMsg);
    spdlog::dump_backtrace();
    spdlog::apply_all([](std::shared_ptr<spdlog::logger> l) { // Flush all sinks immidiately
        l->flush();
    });

    const auto result = MessageBox(
        NULL,
        mbMsg.c_str(),
        "Unreachable code reached! SEND HALP IMMEDIATELY!",
        MB_TASKMODAL | MB_ICONHAND | MB_ABORTRETRYIGNORE | MB_SETFOREGROUND
    );

    switch (result) {
    case IDRETRY: 
        __debugbreak();  // Cause a debug break 
        [[fallthrough]]; // If it's "Continue"'d fall to exit anyways, because function is no-return
    case IDIGNORE:
    case IDABORT:
    default:
        // This is here because:
        // - The compiler will optimize unreachable code away in release builds
        // - No warnings will be generated for missing `return`s
        // - If this function is ever called we're supposed to abort
        exit(3);
    }
}
};
// TODO/NOTE: We might need to manually suppress warnings here?
// Since all the code here is perfectly valid, so the compiler might
// still complain that, for example, the function doesn't return on all code paths, etc
#define IMPL_NOTSA_UNREACHABLE_FMT_ARGS(...) std::format(__VA_ARGS__)
#define NOTSA_UNREACHABLE(...) do { ::notsa::unreachable(__FUNCTION__, __FILE__, __LINE__ __VA_OPT__(,IMPL_NOTSA_UNREACHABLE_FMT_ARGS(__VA_ARGS__))); } while (false)
#else 
#define NOTSA_UNREACHABLE(...) UNREACHABLE_INTRINSIC()
#endif

#ifdef _DEBUG
#define NOTSA_DEBUG_BREAK() __debugbreak()
#else
#define NOTSA_DEBUG_BREAK()
#endif

// In order to be able to get the vtable address using GetProcAddress
// the whole class must be exported. (Along which the vtable is exported as well)
// See `ReversibleHooks::detail::GetClassVTableAddress`
// This should be added to every and all class with a vtable
#define NOTSA_EXPORT_VTABLE __declspec(dllexport)

// Macro for unused function arguments - Use it to avoid compiler warnings of unused arguments
#define UNUSED(x) (void)(x);

// Macro for unused function return values.
// Eventually could instead verify the returned value? In case of `sscanf` etc...
#define RET_IGNORED(x) (void)(x);

//! Cause a debug break
#define NOTSA_DEBUGBREAK() __debugbreak()

//! switch case fallthru
#define NOTSA_SWCFALLTHRU [[fallthrough]]

//! Macro for passing a string var to *scanf_s function.
#define SCANF_S_STR(s) s, std::size(s)

#define NOTSA_FORCEINLINE __forceinline

/*!
* @brief Used for static variable references
*
* @tparam T    The type of the variable
* @param Addr  The address of it
*/
template<typename T>
T& StaticRef(uintptr addr) {
    return *reinterpret_cast<T*>(addr);
}

/*!
 * @brief Use for scoped static variables (That is, static variables that are initialized in functions)
 * @brief See `CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire` for examples)
 * @tparam T The type of the var
 * @param varAddr 
 * @param flagsAddr 
 * @param flagsMask 
 * @param initVal 
 * @return 
 */
template<typename T>
T& ScopedStaticRef(uintptr varAddr, uintptr flagsAddr, uint32 flagsMask, T&& initVal) {
    auto& var   = StaticRef<T>(varAddr);
    auto& flags = StaticRef<uint32>(flagsAddr);
    if (!(flags & flagsMask)) {
        flags |= flagsMask;
        var    = initVal;
    }
    return var;
}

// TODO: Replace this with the one above
template<typename T, uintptr Addr>
T& StaticRef() {
    return StaticRef<T>(Addr);
}

template<typename T>
void SAFE_RELEASE(T*& ptr) { // DirectX stuff `Release()`
    if (ptr) {
        ptr->Release();
        ptr = nullptr;
    }
}

#define _IGNORED_
#define _CAN_BE_NULL_

// TODO: Use premake/cmake for this instead of relaying on `_DEBUG`
#ifdef _DEBUG
#define NOTSA_DEBUG 1
#endif

#if (defined(__GNUC__) || defined(__GNUG__) || defined(__clang__))
#define PLUGIN_SOURCE_FILE
#define PLUGIN_VARIABLE
#define _NOINLINE_
#elif (defined(_MSC_VER))
#define PLUGIN_SOURCE_FILE  __pragma(init_seg(lib))
#define PLUGIN_VARIABLE
#define _NOINLINE_ __declspec(noinline)
#else
#define PLUGIN_SOURCE_FILE
#define PLUGIN_VARIABLE
#define _NOINLINE_
#endif

// III/VC char > wchar_t string conversion
#define _SWSTRING_INIT(str, id) std::wstring my_ws##id; for (size_t i = 0; i < strlen(str); i++) my_ws##id += str[i]
#define _SWSTRING(id) const_cast<wchar_t *>(my_ws##id.c_str())
#define _SWSTRING_STATIC_INIT(id) static wchar_t my_ws##id[512] ; my_ws##id[0] = 0
#define _SWSTRING_STATIC(id) my_ws##id
#define _SWSTRING_STATIC_FROM(id, src) for (size_t i = 0; i < strlen(src); i++) my_ws##id[i] = src[i]
#define _SWSTRING_STATIC_TO(id, dst) for (size_t i = 0; i < wcslen(my_ws##id); i++) dst[i] = static_cast<char>(my_ws##id[i])

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RwRGBAReal, red, blue, green, alpha);
