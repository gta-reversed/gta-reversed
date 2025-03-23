#pragma once

#include "Base.h"
#include "Utility.hpp"
#include "ReadArg.hpp" // TODO: We only use `PooledType` from here, so move that out to somewhere common between the 2 headers (because including this here is ugly)
#include "TheScripts.h"
#include "RunningScript.h" // ScriptParams
#include "Pools.h"

namespace notsa {
namespace script {
/*!
* Bool is a special case, it isn't stored, but rather updates the compare flag.
* TODO: Actually verify this theory.
*/
template<typename T>
    requires std::is_same_v<T, bool> // Must do it like this to avoid erroneous implicit conversions to bool
inline void StoreArg(CRunningScript* S, T arg) {
    S->UpdateCompareFlag(arg);
}

/*!
* @brief Store an argument of type. NOTE: Increments script's IP.
*
* @param S The S to which the values should be stored to
* @param arg    The argument to store
*/
template<typename T>
    requires (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>)
inline void StoreArg(CRunningScript* S, const T& arg) { // Add requirements to filter out possible mistakes (Like returning an unsupported type)
    T& dst = [&]() -> T& {
        switch (const auto t = S->GetAtIPAs<int8>()) {
        case SCRIPT_PARAM_GLOBAL_NUMBER_VARIABLE:
            return S->GetGlobal<T>(S->GetAtIPAs<uint16>());
        case SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE:
            return S->GetLocal<T>(S->GetAtIPAs<uint16>());
        case SCRIPT_PARAM_GLOBAL_NUMBER_ARRAY:
            return S->GetAtIPFromArray<T>(true);
        case SCRIPT_PARAM_LOCAL_NUMBER_ARRAY:
            return S->GetAtIPFromArray<T>(false);
        default:
            NOTSA_UNREACHABLE("Variable type unknown ={:x}", t);
        }
    }();
    static_assert(sizeof(dst) <= sizeof(tScriptParam)); // Otherwise we'd be overwriting the script => bad

    memset((void*)&dst, 0, sizeof(tScriptParam));  // Zero it all out
    memcpy((void*)&dst, (void*)&arg, sizeof(arg)); // Now copy the value to it (Which might be less than 4 bytes)
}

// Vector overloads

inline void StoreArg(CRunningScript* S, const CVector& v3) {
    for (auto&& c : v3.GetComponents()) {
        StoreArg(S, c);
    }
}

inline void StoreArg(CRunningScript* S, const CVector2D& v2) {
    for (auto&& c : v2.GetComponents()) {
        StoreArg(S, c);
    }
}

inline void StoreArg(CRunningScript* S, CompareFlagUpdate flag) {
    S->UpdateCompareFlag(flag.state);
}

// Below must be after the basic overloads, otherwise won't compile

//! Store a pooled type (CPed, CVehicle, etc) - It pushes a handle of the entity to the script
template<typename T, typename U = std::remove_cvref_t<std::remove_pointer_t<T>>>
    requires detail::PooledType<U>
inline void StoreArg(CRunningScript* S, T&& value) {
    const auto StoreEntity = [&](auto ptr) {
        StoreArg(S, detail::PoolOf<U>().GetRef(ptr));
    };
    if constexpr (std::is_pointer_v<T>) {
        if (value) { // As always, pointers might be null, so we have to check.
            StoreEntity(value);
        } else {
            StoreArg(S, -1); // If null, store `-1` (indicates an invalid handle, it is handled properly!)
        }
    } else { // References are never invalid
        StoreEntity(&value);
    }
}

/*!
 * @brief Overload for enum types. They're casted to their underlying type.
 */
template <typename T>
    requires std::is_enum_v<T>
inline void StoreArg(CRunningScript* S, T value) {
    StoreArg(S, static_cast<std::underlying_type_t<T>>(value));
}

/*!
* @brief Overload for MultiReturn => Stores each arg separately, in same order as they appear in the multireturn
*/
template<typename... Ts>
inline void StoreArg(CRunningScript* S, const MultiRet<Ts...>& arg) {
    std::apply([S](const Ts&... args) { (StoreArg(S, args), ...); }, arg);
}

}; // namespace script
}; // namespace notsa
