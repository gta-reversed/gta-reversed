#pragma once

#include <tuple> // tuple

class CRunningScript;
class CPhysical;

namespace notsa {
namespace script {
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
