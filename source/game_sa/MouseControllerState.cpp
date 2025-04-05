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
    LeftButton = false;
    RightButton = false;
    MiddleButton = false;
    WheelMovedUp = false;
    WheelMovedDown = false;
    MsFirstXButton = false;
    MsSecondXButton = false;
}

// 0x53F270
bool CMouseControllerState::CheckForInput() const {
    return (
        LeftButton
        || RightButton
        || MiddleButton
        || WheelMovedUp
        || WheelMovedDown
        || MsFirstXButton
        || MsSecondXButton
        || !m_AmountMoved.IsZero()
    );
}
