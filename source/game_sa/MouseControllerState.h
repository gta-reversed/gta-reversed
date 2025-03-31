#pragma once

class CMouseControllerState {
public:
    bool m_bLeftButton; // LMB
    bool m_bRightButton; // RMB
    bool m_bMiddleButton; // MMB
    bool m_bWheelMovedUp; // Wheel up
    bool m_bWheelMovedDown; // Wheel down
    bool m_bMsFirstXButton; // BMX1
    bool m_bMsSecondXButton; // BMX2
    float m_fWheelMoved; // Wheel movement
    CVector2D m_AmountMoved; // Mouse movement
  
public:
    static void InjectHooks();

    CMouseControllerState();
    CMouseControllerState* Constructor();

    void Clear();
    [[nodiscard]] bool CheckForInput() const;
    [[nodiscard]] auto GetAmountMouseMoved() const { return m_AmountMoved; }

};
VALIDATE_SIZE(CMouseControllerState, 0x14);