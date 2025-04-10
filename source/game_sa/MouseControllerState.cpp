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
    m_AmountMoved.Reset();
    Clear();
}

CMouseControllerState* CMouseControllerState::Constructor() {
    this->CMouseControllerState::CMouseControllerState();
    return this;
}

// 0x53F250
void CMouseControllerState::Clear() {
    isMouseLeftButtonPressed = 0;
    isMouseRightButtonPressed = 0;
    isMouseMiddleButtonPressed = 0;
    isMouseWheelMovedUp = 0;
    isMouseWheelMovedDown = 0;
    isMouseFirstXPressed = 0;
    isMouseSecondXPressed = 0;
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
        || m_AmountMoved.x != 0.0f
        || m_AmountMoved.y != 0.0f
    );
}
