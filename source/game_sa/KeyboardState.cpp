#include "StdInc.h"

#include "KeyboardState.h"

void CKeyboardState::InjectHooks() {
    RH_ScopedClass(CKeyboardState);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Clear, 0x53F090);
}

// 0x53F090
void CKeyboardState::Clear() {
    *this = CKeyboardState{};
}
