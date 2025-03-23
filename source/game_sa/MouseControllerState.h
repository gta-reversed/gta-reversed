#pragma once

class CMouseControllerState {
public:
    bool8 m_bLeftButton; // LMB
    bool8 m_bRightButton; // RMB
    bool8 m_bMiddleButton; // MMB
    bool8 m_bWheelMovedUp; // Wheel up
    bool8 m_bWheelMovedDown; // Wheel down
    bool8 m_bMsFirstXButton; // BMX1
    bool8 m_bMsSecondXButton; // BMX2
    float m_fWheelMoved; // Wheel movement
    RwV2d m_AmountMoved; // Mouse movement
  
public:
    static void InjectHooks();

    CMouseControllerState();
    CMouseControllerState* Constructor();

    void Clear();
    [[nodiscard]] bool CheckForInput() const;
    [[nodiscard]] auto GetAmountMouseMoved() const { return m_AmountMoved; }

};
VALIDATE_SIZE(CMouseControllerState, 0x14);
