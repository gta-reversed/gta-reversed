#pragma once

class CMouseControllerState {
public:
    bool isMouseLeftButtonPressed; // LMB
    bool isMouseRightButtonPressed; // RMB
    bool isMouseMiddleButtonPressed; // MMB
    bool isMouseWheelMovedUp; // Wheel up
    bool isMouseWheelMovedDown; // Wheel down
    bool isMouseFirstXPressed; // BMX1
    bool isMouseSecondXPressed; // BMX2
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