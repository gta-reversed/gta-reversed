#pragma once

#include <type_traits>
#include <assert.h>

#include "RunningScript.h"
#include "app_debug.h"
#include "World.h"
#include "TheScripts.h"
#include "Utility.hpp"
#include "Rect.h"
#include "Scripted2dEffects.h"

namespace notsa {
namespace script {

// eModelID wrapper that is read either from a static
// int32 value or UsedObjectArray.
struct Model {
    eModelID model;

    operator eModelID() const { return model; }
};

namespace detail {

//! Script thing bullshittery
namespace scriptthing {
//! Get script thing at index
template<typename T>
inline auto GetAt(uint32 index) -> T& = delete;
template<>
inline auto GetAt(uint32 idx) -> tScriptSphere& { return CTheScripts::ScriptSphereArray[idx]; }
template<>
inline auto GetAt(uint32 idx) -> tScriptEffectSystem& { return CTheScripts::ScriptEffectSystemArray[idx]; }
template<>
inline auto GetAt(uint32 idx) -> tScriptSearchlight& { return CTheScripts::ScriptSearchLightArray[idx]; }
template<>
inline auto GetAt(uint32 idx) -> tScriptSequence&   { return CTheScripts::ScriptSequenceTaskArray[idx]; }
template<>
inline auto GetAt(uint32 idx) -> tScriptCheckpoint& { return CTheScripts::ScriptCheckpointArray[idx]; }
template<std::derived_from<C2dEffectBase> T>
inline auto GetAt(uint32 idx) -> T& {
    auto& effect = CScripted2dEffects::ms_effects[idx];
    assert(T::Type == effect.m_type);
    return reinterpret_cast<T&>(effect);
}

//! Get ID of a thing at a given index
template<typename T>
inline auto GetId(uint32 idx) -> uint32 { return GetAt<T>(idx).GetId(); }
template<std::derived_from<C2dEffectBase> T>
inline auto GetId(uint32 idx) -> uint32 { return CScripted2dEffects::ScriptReferenceIndex[idx]; }

//! Get if a script thing is active
template<typename T>
inline auto IsActive(uint32 idx) -> bool { return GetAt<T>(idx).IsActive(); }
template<std::derived_from<C2dEffectBase> T>
inline auto IsActive(uint32 idx) -> bool { return CScripted2dEffects::ms_activated[idx]; }

//! Check if `T` is a script thing
template<typename T>
inline constexpr auto is_script_thing_v = requires(uint32 index) { GetAt<T>(index); };
static_assert(is_script_thing_v<C2dEffect> && !is_script_thing_v<int>);
}; // namespace scriptthing
};

namespace detail {
template<typename T>
concept PooledType =
    requires { detail::PoolOf<typename std::remove_cvref_t<T>>(); };
};

namespace detail {

//! Safely cast one arithmetic type to another (Checks for under/overflow in debug mode only), then casts to `T`
template<typename T, typename F>
inline T safe_arithmetic_cast(F value) {
#ifdef NOTSA_DEBUG
    if constexpr (is_standard_integer<T> && is_standard_integer<F>) {
        assert(std::in_range<T>(value));
    }
#endif
    return static_cast<T>(value); // In release mode just a simple, regular cast
}

//! Check if `Derived` is derived from `Base` but isn't `Base`
template<typename Base, typename Derived>
constexpr auto is_derived_from_but_not_v = std::is_base_of_v<Base, Derived> && !std::is_same_v<Base, Derived>;
};

//! Read a value (Possibly from script => increases IP, or return a value (w/o increasing IP)
template<typename T>
inline T Read(CRunningScript* S) {
    using Y = std::remove_pointer_t<std::remove_cvref_t<T>>;

    // First of all, deal with references
    // References are a way to express that a value (non-null) value must be present
    // While simple pointers are a way to express that "it's okay if it's null, I can handle it".
    // This check here also means that all other branches must either return by-value or a pointer (not a refernce)
    if constexpr (std::is_reference_v<T>) {
        const auto ptr = Read<std::remove_reference_t<T>*>(S);
        assert(ptr); // This assert is usually hit if the implementation defines an argument with a different type than the original. Eg.: `CVehicle&` instead of `CPed&`.
        return *ptr;
    } else if constexpr (std::is_same_v<Y, CVector>) {
        return { Read<float>(S), Read<float>(S), Read<float>(S) };
    } else if constexpr (std::is_same_v<Y, CVector2D>) {
        return { Read<float>(S), Read<float>(S) };
    } else if constexpr (std::is_same_v<Y, CRect>) {
        return { Read<CVector2D>(S), Read<CVector2D>(S) }; // Read as (minX, minY)+(maxX, maxY) or top-left+bottom-right
    } else if constexpr (std::is_same_v<Y, std::string_view>) {
        switch (const auto ptype = S->GetAtIPAs<eScriptParameterType>()) {
        case SCRIPT_PARAM_GLOBAL_SHORT_STRING_VARIABLE:
            return S->GetGlobal<scm::ShortString>(S->GetAtIPAs<scm::VarLoc>());
        case SCRIPT_PARAM_LOCAL_SHORT_STRING_VARIABLE:
            return S->GetLocal<scm::ShortString>(S->GetAtIPAs<scm::VarLoc>());

        case SCRIPT_PARAM_GLOBAL_SHORT_STRING_ARRAY:
            return S->GetAtIPFromArray<scm::ShortString>(true);
        case SCRIPT_PARAM_GLOBAL_LONG_STRING_ARRAY:
            return S->GetAtIPFromArray<scm::LongString>(true);

        case SCRIPT_PARAM_LOCAL_SHORT_STRING_ARRAY:
            return S->GetAtIPFromArray<scm::ShortString>(false);
        case SCRIPT_PARAM_LOCAL_LONG_STRING_ARRAY:
            return S->GetAtIPFromArray<scm::LongString>(false);

        case SCRIPT_PARAM_LOCAL_LONG_STRING_VARIABLE:
            return S->GetLocal<scm::LongString>(S->GetAtIPAs<scm::VarLoc>());
        case SCRIPT_PARAM_GLOBAL_LONG_STRING_VARIABLE:
            return S->GetGlobal<scm::LongString>(S->GetAtIPAs<scm::VarLoc>());

        case SCRIPT_PARAM_STATIC_SHORT_STRING:
            return S->GetAtIPAs<scm::ShortString>();
        case SCRIPT_PARAM_STATIC_LONG_STRING:
            return S->GetAtIPAs<scm::LongString>();

        case SCRIPT_PARAM_STATIC_PASCAL_STRING: {
            const auto sSize = S->GetAtIPAs<int8>(); // signed size, max size = 127, not 255
            VERIFY(sSize >= 0);
            const auto size = (size_t)(sSize);
            return { &S->GetAtIPAs<char>(true, size), size};
        }
        default:
            NOTSA_UNREACHABLE("Unknown param type: {}", (int32)(ptype));
        }
    } else if constexpr (std::is_same_v<T, const char*>) { // Read C-style string (Hacky)
        const auto sv = Read<std::string_view>(S);
        assert(sv.size() < COMMANDS_CHAR_BUFFER_SIZE - 1);
        // For explaination of why this is done this way, see the comment at CRunningScript::ScriptArgCharBuffers declaration
        auto& buffer = CRunningScript::ScriptArgCharBuffers[CRunningScript::ScriptArgCharNextFreeBuffer++];
        sv.copy(buffer.data(), sv.size());
        buffer[sv.size()] = '\0';
        return buffer.data();
    } else if constexpr (std::is_arithmetic_v<Y>) { // Simple arithmetic types (After reading a string, because `const char*` with cv and pointer removed is just `char` which is an arithmetic type)
        const auto ptype = S->GetAtIPAs<eScriptParameterType>();
        if constexpr (std::is_pointer_v<T>) { // by-ref (This is a special case, as some basic ops need a reference instead of a value)
            switch (ptype) {
            case SCRIPT_PARAM_GLOBAL_NUMBER_VARIABLE:
                return &S->GetGlobal<Y>(S->GetAtIPAs<scm::VarLoc>());
                //return reinterpret_cast<Y*>(&CTheScripts::ScriptSpace[S->GetAtIPAs<uint16>()]);
            case SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE:
                return &S->GetLocal<Y>(S->GetAtIPAs<scm::VarLoc>());
                //return reinterpret_cast<Y*>(S->GetPointerToLocalVariable(S->GetAtIPAs<uint16>()));
            case SCRIPT_PARAM_GLOBAL_NUMBER_ARRAY: {
                return &S->GetAtIPFromArray<Y>(true);
                //const auto [offset, idx] = detail::ReadArrayInfo(S);
                //return reinterpret_cast<Y*>(&CTheScripts::ScriptSpace[offset + sizeof(tScriptParam) * idx]);
            }
            case SCRIPT_PARAM_LOCAL_NUMBER_ARRAY: {
                return &S->GetAtIPFromArray<Y>(false);
                //const auto [offset, idx] = detail::ReadArrayInfo(S);
                //return reinterpret_cast<Y*>(S->GetPointerToLocalArrayElement(offset, idx, 1));
            }
            }
        } else { // Regular by-value
            switch (ptype) {
            case SCRIPT_PARAM_GLOBAL_NUMBER_VARIABLE:
                return S->GetGlobal<T>(S->GetAtIPAs<scm::VarLoc>());
                //return *reinterpret_cast<T*>(&CTheScripts::ScriptSpace[S->GetAtIPAs<uint16>()]);
            case SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE:
                return S->GetLocal<T>(S->GetAtIPAs<scm::VarLoc>());
                //return *reinterpret_cast<T*>(S->GetPointerToLocalVariable(S->GetAtIPAs<uint16>()));
            case SCRIPT_PARAM_STATIC_INT_8BITS:
                return detail::safe_arithmetic_cast<T>(S->GetAtIPAs<int8>());
            case SCRIPT_PARAM_STATIC_INT_16BITS:
                return detail::safe_arithmetic_cast<T>(S->GetAtIPAs<int16>());
            case SCRIPT_PARAM_STATIC_INT_32BITS:
                return detail::safe_arithmetic_cast<T>(S->GetAtIPAs<int32>());
            case SCRIPT_PARAM_STATIC_FLOAT: {
                if constexpr (!std::is_floating_point_v<T>) {
                    // Check your call stack and change the function argument type to a float.
                    // Possibly unintended truncation of `float` to integeral type!
                    DebugBreak();
                }
                return detail::safe_arithmetic_cast<T>(S->GetAtIPAs<float>());
            }
            case SCRIPT_PARAM_GLOBAL_NUMBER_ARRAY: {
                return S->GetAtIPFromArray<T>(true);
                //const auto [offset, idx] = detail::ReadArrayInfo(S);
                //return *reinterpret_cast<T*>(&CTheScripts::ScriptSpace[offset + sizeof(tScriptParam) * idx]);
            }
            case SCRIPT_PARAM_LOCAL_NUMBER_ARRAY: {
                return S->GetAtIPFromArray<Y>(false);
                //const auto [offset, idx] = detail::ReadArrayInfo(S);
                //return *reinterpret_cast<T*>(S->GetPointerToLocalArrayElement(offset, idx, 1));
            }
            }
        }
        NOTSA_UNREACHABLE("Unknown param type: {}", (int32)(ptype));
    } else if constexpr (std::is_enum_v<Y>) { // Read underlaying arithmetic type and cast (then profit)
        return static_cast<Y>(Read<std::underlying_type_t<Y>>(S));
    } else if constexpr (std::is_same_v<Y, CPlayerPed>) { // Special case for `CPlayerPed` (As the IDs for it aren't from the pool)
        return FindPlayerPed(Read<int32>(S));
    } else if constexpr (detail::PooledType<Y>)  { // Pooled types (CVehicle, CPed, etc)
        T ptr = static_cast<T>(detail::PoolOf<Y>().GetAtRef(Read<int32>(S)));

    #ifdef NOTSA_DEBUG
        if (ptr) {
            if constexpr (detail::is_derived_from_but_not_v<CVehicle, Y>) {
                assert(Y::Type == ptr->m_nVehicleType);
            } else if constexpr (detail::is_derived_from_but_not_v<CTask, Y>) {
                assert(Y::Type == ptr->GetTaskType());
            } // TODO: Eventually add this for `CEvent` too
        }
    #endif

        return ptr;
    } else if constexpr (std::is_same_v<Y, CRunningScript>) { // Just return the script from where this command was invoked from
        return S;
    } else if constexpr (std::is_same_v<Y, CPlayerInfo>) {
        return &FindPlayerInfo(Read<int32>(S));
    } else if constexpr (detail::scriptthing::is_script_thing_v<Y>) {
        // Read information (packed 2x16 int)
        const auto info = Read<uint32>(S);
        if (info == (uint32)(-1)) { // Invalid handle, may happen if a function returned it (and they didn't handle it properly)
            return nullptr; 
        }

        // Extract index and (expected) ID of the object
        const auto index = (uint16)(LOWORD(info)), id = (uint16)(HIWORD(info));

        // Check if the object is active (If not, it has been reused/deleted)
        if (!detail::scriptthing::IsActive<Y>(index)) {
            return nullptr;
        }

        // Check if ID is what we expect (If not, that means that the object has been reused)
        if (detail::scriptthing::GetId<Y>(index) != id) {
            return nullptr;
        }

        return &detail::scriptthing::GetAt<Y>(index);
    } else if constexpr (std::is_same_v<T, script::Model>) {
        const auto value = Read<int32>(S);
        if (value < 0) {
            // we get the model from UsedObjectArray.
            return {static_cast<eModelID>(CTheScripts::UsedObjectArray[-value].nModelIndex)};
        }

        return {static_cast<eModelID>(value)};
    }
    // If there's an error like "function must return a value" here,
    // that means that no suitable branch was found for `T`
    // You have 2 options: Either add a branch for that type,
    // or check and make sure you didn't try taking some object by-value
    // that you shouldn't have (Like `CPed`, `CVehicle`, or such)
}

}; // namespace script
}; // namespace notsa
