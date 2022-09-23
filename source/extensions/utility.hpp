#pragma once

namespace notsa {

/*!
* @brief Call the given function on object destruction.
*/
template<typename Fn>
struct AutoCallOnDestruct {
    AutoCallOnDestruct(Fn fn) :
        m_fn{ std::move(fn) }
    {
    }

    ~AutoCallOnDestruct() {
        std::invoke(m_fn);
    }

private:
    Fn m_fn;
};

/// Predicate to check if `value` is null
template<typename T>
    requires(std::is_pointer_v<T>)
bool IsNull(T value) { return value == nullptr; }

/// Negate another predicate function
template<typename T>
auto Not(bool(*fn)(T)) { return [fn](const T& value) { return !fn(value); }; }

struct NotIsNull {
    template<typename T>
    bool operator()(const T* ptr) {
        return ptr != nullptr;
    }
};

// Find first non-null value in range. If found it's returned, `null` otherwise.
template<rng::input_range R, typename T_Ret = rng::range_value_t<R>>
    requires(std::is_pointer_v<T_Ret>)
T_Ret FirstNonNull(R&& range) {
    const auto it = rng::find_if(range, NotIsNull{});
    return it != rng::end(range)
        ? *it
        : nullptr;
}

/*!
* @tparam Start     The number at which to start the iteration
* @tparam Stop      The number at which to stop the iteration
* @tparam ChunkSize The chunk size, see function description.
*
* @arg functor The functor instance that has a `operator()` with matching `template<size_t>`. Templated lambdas can be used.
*
* This function will iterate thru the sequence of numbers in range [Base, Stop), and
* call the functor's templated `operator()` with the index being the first template argument.
*
* @code{.cpp}
*   constexpr auto Bar = []<size_t Idx>(){
*       std::cout << Idx << ", ";
*   };
*   IterateFunction<0, 5>(Bar); // Prints: 0, 1, 2, 3, 4
* @endcode
*
* About the `ChunkSize` template parameter:
* We have to balance between recursion and index seq. size, because:
* - Fold op. by default maxes out at ~256 arguments (MSVC)
* - contexpr recursion is limited to ~1000 (MSVC)
* So we can't rely on using only 1 method, gotta use both at the same time
* In case you ever run into one of the limits:
* - Fold op. => Decrease `ChunkSize`
* - Recursion => Increase `ChunkSize`
*/
template<int Start, int Stop, int ChunkSize = 128>
static constexpr void IterateFunction(auto&& functor) {
    // Invoke function with current sequence
    [&] <std::size_t... Idx>(std::index_sequence<Idx...>) {
        (functor.template operator()<Start + Idx>(), ...);
    }(std::make_index_sequence<std::min(ChunkSize, Stop - Start)>{});

    // Continue recursing if there's anything left
    if constexpr (Stop - Start > ChunkSize) {
        IterateFunction<Start + ChunkSize, Stop>(functor);
    }
}

};
