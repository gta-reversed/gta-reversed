#pragma once
#include <source_location>
#include <string>
#include "KeyGen.h"


namespace notsa::bugfixes {
/*!
* This class is *not* meant to be constructed on the stack.
* Only place it should be defined at is `Bugs.hpp`.
*/
struct ReversibleBugFix {
    static inline ReversibleBugFix* Tail{};

    constexpr operator bool() const { return Enabled; }

public:
    const char*       Name{};                            //!< User-defined name
    const char*       Description{};                     //!< User defined description
    const char*       Credit{};                          //!< Credits for this bugfix
    bool              Enabled{ true };                   //!< Is this bugfix enabled? (If not, the bug is present)
    bool              Locked{ false };                   //!< Can this bugfix's enabled state be changed?
    ReversibleBugFix* Prev{ std::exchange(Tail, this) }; //!< Linked list of bugfixes (Maintained for the debug tool)
};
};
