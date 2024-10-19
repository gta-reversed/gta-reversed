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
* Bool is a sepcial case, it isn't stored, but rather updates the compare flag.
* TODO: Actually verify this theory.
*/
inline void StoreArg(CRunningScript* S, bool arg) {
    S->UpdateCompareFlag(arg);
}
/*!
* @brief Store an argument of type. NOTE: Increments script's IP.
*
* @param S The S to which the values should be stored to
* @param arg    The argument to store
*/
template<typename T>
    requires (std::is_arithmetic_v<T>)
inline void StoreArg(CRunningScript* S, const T& arg) { // Add requirements to filter out possible mistakes (Like returning an unsupported type)
    tScriptParam* dest = [&] {
        const auto GetFromArray = [S](auto&& GetArrayElement) {
            uint16 base{};
            int32 idx{};
            S->ReadArrayInformation(true, &base, &idx);
            return std::invoke(GetArrayElement, S, base, idx, 1);
        };

        switch (const auto t = S->ReadAtIPAs<int8>()) {
        case SCRIPT_PARAM_GLOBAL_NUMBER_VARIABLE:
            return S->GetPointerToGlobalVariable(S->ReadAtIPAs<uint16>());
        case SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE:
            return S->GetPointerToLocalVariable(S->ReadAtIPAs<uint16>());
        case SCRIPT_PARAM_GLOBAL_NUMBER_ARRAY:
            return GetFromArray(&CRunningScript::GetPointerToGlobalArrayElement);
        case SCRIPT_PARAM_LOCAL_NUMBER_ARRAY:
            return GetFromArray(&CRunningScript::GetPointerToLocalArrayElement);
        default:
            NOTSA_UNREACHABLE("Variable type unknown ={:x}", t);
        }
    }();
    static_assert(sizeof(T) <= sizeof(tScriptParam)); // Otherwise we'd be overwriting the script => bad

    memset((void*)dest, 0, sizeof(tScriptParam));  // Zero it all out
    memcpy((void*)dest, (void*)&arg, sizeof(arg)); // Now copy the value to it (Which might be less than 4 bytes)
}

// Vector overloads

inline void StoreArg(CRunningScript* script, const CVector& v3) {
    for (auto&& c : v3.GetComponents()) {
        StoreArg(script, c);
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
template<detail::PooledType T>
inline void StoreArg(CRunningScript* S, const T& value) {
    const auto StoreEntity = [&](auto ptr) { StoreArg(S, detail::PoolOf<std::remove_cvref_t<T>>().GetRef(ptr)); };
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
