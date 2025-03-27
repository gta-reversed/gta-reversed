#pragma once

#include <tuple> // tuple

#include "Pools.h"

class CRunningScript;
class CPhysical;

namespace notsa {
namespace script {
namespace detail {
template<typename T>
auto& PoolOf() = delete;

template<>
auto& PoolOf<CColModel>() { return *GetColModelPool(); }

template<>
auto& PoolOf<CPedIntelligence>() { return *GetPedIntelligencePool(); }

template<>
auto& PoolOf<CPtrNodeSingleLink>() { return *GetPtrNodeSingleLinkPool(); }

template<>
auto& PoolOf<CPtrNodeDoubleLink>() { return *GetPtrNodeDoubleLinkPool(); }

template<>
auto& PoolOf<CEntryInfoNode>() { return *GetEntryInfoNodePool(); }

template<>
auto& PoolOf<CPointRoute>() { return *GetPointRoutePool(); }

template<>
auto& PoolOf<CPatrolRoute>() { return *GetPatrolRoutePool(); }

template<>
auto& PoolOf<CNodeRoute>() { return *GetNodeRoutePool(); }

template<>
auto& PoolOf<CTaskAllocator>() { return *GetTaskAllocatorPool(); }

template<>
auto& PoolOf<CPedAttractor>() { return *GetPedAttractorPool(); }

/*
* Pools of derived types
*/
template<typename T>
    requires std::is_base_of_v<CPed, T>
auto& PoolOf() { return *GetPedPool(); }

template<typename T>
    requires std::is_base_of_v<CVehicle, T>
auto& PoolOf() { return *GetVehiclePool(); }

template<typename T>
    requires std::is_base_of_v<CBuilding, T>
auto& PoolOf() { return *GetBuildingPool(); }

template<typename T>
    requires std::is_base_of_v<CObject, T>
auto& PoolOf() { return *GetObjectPool(); }

template<typename T>
    requires std::is_base_of_v<CDummy, T>
auto& PoolOf() { return *GetDummyPool(); }

template<typename T>
    requires std::is_base_of_v<CTask, T>
auto& PoolOf() { return *GetTaskPool(); }

template<typename T>
    requires std::is_base_of_v<CEvent, T>
auto& PoolOf() { return *GetEventPool(); }
}; // detail

/*!
* Struct used to return multiple values from a script function.
* Eg.: MultiRet<int, int> => Should push 2 ints (Using `StoreArg`)
* Don't use this directly, use `return_multiple` instead (Unless you have to fwd. declare the function or smth)
*/
template<typename... Ts>
using MultiRet = std::tuple<Ts...>;

//! Used to return multiple values from a script function, see `MultiReturn`
template<typename... Ts>
constexpr auto return_multiple(Ts&&... args) {
    return MultiRet<Ts...>{args...};
}

//struct MultiRet : public std::tuple<Ts...> {
//    using std::tuple::tuple;
//};

/*!
* Special return value used to update the compare flag.
*/
struct CompareFlagUpdate {
    bool state{};
};

using CommandHandlerFunction = OpcodeResult(*)(CRunningScript*);

//! Fix angles (in degrees) - heaviliy used in scripts.
inline float FixAngleDegrees(float deg) {
    if (deg < 0.f) {
        return deg + 360.f;
    }
    if (deg > 360.f) {
        return deg - 360.f;
    }
    return deg;
}

//! Get the ped or it's vehicle (if in one)
inline auto GetPedOrItsVehicle(CPed& ped) -> CPhysical& {
    if (ped.IsInVehicle()) {
        return *ped.m_pVehicle;
    }
    return ped;
}
}; // script


// Credit goes to https://github.com/sbx320
// Thank you!
template<std::size_t I, typename... Ts>
using nth_element_t = typename std::tuple_element<I, std::tuple<Ts...>>::type;

}; // notsa
