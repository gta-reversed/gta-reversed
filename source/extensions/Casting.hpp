#pragma once

class CPedAttractor;

namespace notsa {
    //
    // Temporary implementation for casting stuff.. Eventually all custom casting related stuff will be here
    // And the code from llvm/Support/Casting.hpp will be used instead of this custom shit...
    //

    /*!
     * @brief Works like `dynamic_cast` => Checks if `p` is of the required type, if so, returns it, otherwise nullptr
     */
    template<typename T>
    inline T* DynCast(CPedAttractor* p) {
        if (p && p->GetType() == T::Type) {
            return static_cast<T*>(p);
        }
        return nullptr;
    }

    /*!
     * @brief Check if this task is any of the given types
     */
    template<ePedAttractorType... Types>
    inline bool IsA(CPedAttractor* p) {
        if (!p) {
            return false;
        }
        const auto type = p->GetType();
        return ((type == Types) || ...);
    }

    template<typename... Ts>
    inline bool IsA(CPedAttractor* p) {
        return IsA<Ts::Type...>(p);
    }

    /*!
     * @brief Works like `static_cast` + in debug mode asserts the type to be as expected.
     */
    template<typename T>
    inline T* Cast(CPedAttractor* p) {
        if constexpr (std::is_same_v<CPedAttractor, T>) {
            return p;
        } else {
            assert(!p || p->GetType() == T::Type);
            return static_cast<T*>(p);
        }
    }
};
