#pragma once

#include <reversiblehooks/VMTInfo.h>
#include <reversiblehooks/HooksUtility.hpp>

#include "VirtualHook.h"

namespace ReversibleHooks {
namespace ReversibleHook {
/*!
 * @brief 2-way hook that handles hooking of virtual destructors (scalar and base, vector is not hooked), including both direct calls and calls that use the VMT. 
 * @tparam T The class type that has the virtual destructor, used to create a wrapper for calling the destructor.
 */
template<typename T>
    requires std::is_class_v<T>
struct VirtualDestructorHook final : public VirtualHook {
public:
    /*!
     * @brief Constructor for hooking virtual destructors where the direct call and virtual functions aren't the same (For example destructors vs deleting virtual destructor)
     * @param vmtInfoOur Our VMT's info
     * @param vmtInfoGTA GTA VMT's info
     * @param fnAddressGTA Address of GTA base destructor (eg.: &Class::VirtualFunction)
     */
    VirtualDestructorHook(
        Utility::VMTInfo vmtInfoOur,
        Utility::VMTInfo vmtInfoGTA,
        void*            fnAddressGTA
    ) :     
        VirtualHook{  
            "Destructor",
            vmtInfoOur.GetEntryAddressAt(Utility::VMTInfo::DESTRUCTOR_VMT_INDEX),
            Utility::GetScalarDestructorAddress<T>(),
            vmtInfoGTA.GetEntryAddressAt(Utility::VMTInfo::DESTRUCTOR_VMT_INDEX),
            fnAddressGTA
        }
    {
    }

    HookType Type() const noexcept override { return HookType::VirtualDestructor; }
};
}; // namespace ReversibleHook
}; // namespace ReversibleHooks
