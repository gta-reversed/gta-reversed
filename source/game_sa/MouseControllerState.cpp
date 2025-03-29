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
    m_AmountMoved.x = 0.0f;
    m_AmountMoved.y   = 0.0f;
    Clear();
}

CMouseControllerState* CMouseControllerState::Constructor() {
    this->CMouseControllerState::CMouseControllerState();
    return this;
}

// 0x53F250
void CMouseControllerState::Clear() {
    m_bLeftButton = 0;
    m_bRightButton = 0;
    m_bMiddleButton = 0;
    m_bWheelMovedUp = 0;
    m_bWheelMovedDown = 0;
    m_bMsFirstXButton = 0;
    m_bMsSecondXButton = 0;
}

// 0x53F270
bool CMouseControllerState::CheckForInput() const {
    return (
        m_bLeftButton
        || m_bRightButton
        || m_bMiddleButton
        || m_bWheelMovedUp
        || m_bWheelMovedDown
        || m_bMsFirstXButton
        || m_bMsSecondXButton
        || m_AmountMoved.x != 0.0f
        || m_AmountMoved.y != 0.0f
    );
}
