#pragma once

class CMouseControllerState {
public:
    bool LeftButton; // LMB
    bool RightButton; // RMB
    bool MiddleButton; // MMB
    bool WheelMovedUp; // Wheel up
    bool WheelMovedDown; // Wheel down
    bool MsFirstXButton; // BMX1
    bool MsSecondXButton; // BMX2
    float WheelMoved; // Wheel movement
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
