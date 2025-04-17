#include "StdInc.h"

#include "MouseControllerState.h"

void CMouseControllerState::InjectHooks() {
    RH_ScopedClass(CMouseControllerState);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x53F220);
    RH_ScopedInstall(Clear, 0x53F250);
    RH_ScopedInstall(CheckForInput, 0x53F270);
}

// 0x53F220
CMouseControllerState::CMouseControllerState() {
    m_fWheelMoved = 0.f;
    m_AmountMoved.Reset();
    Clear();
}

CMouseControllerState* CMouseControllerState::Constructor() {
    this->CMouseControllerState::CMouseControllerState();
    return this;
}

// 0x53F250
void CMouseControllerState::Clear() {
    isMouseLeftButtonPressed = false;
    isMouseRightButtonPressed = false;
    isMouseMiddleButtonPressed = false;
    isMouseWheelMovedUp = false;
    isMouseWheelMovedDown = false;
    isMouseFirstXPressed = false;
    isMouseSecondXPressed = false;
}

// 0x53F270
bool CMouseControllerState::CheckForInput() const {
    return (
        isMouseLeftButtonPressed
        || isMouseRightButtonPressed
        || isMouseMiddleButtonPressed
        || isMouseWheelMovedUp
        || isMouseWheelMovedDown
        || isMouseFirstXPressed
        || isMouseSecondXPressed
        || !m_AmountMoved.IsZero()
    );
}
