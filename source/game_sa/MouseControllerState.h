#pragma once

class CMouseControllerState {
public:
    uint8 lmb{};       //!< Left mouse button
    uint8 rmb{};       //!< Right mouse button
    uint8 mmb{};       //!< Middle mouse button
    uint8 wheelUp{};   //!< Is scroll wheel going up
    uint8 wheelDown{}; //!< Is scroll wheel going down
    uint8 bmx1{};      //!< Side mouse button 1
    uint8 bmx2{};      //!< Side mouse button 2
    float Z{};         //!< Scroll wheel delta
    float X{};         //!< Mouse delta X
    float Y{};         //!< Mouse delta Y

public:
    static void InjectHooks();

    CMouseControllerState();
    CMouseControllerState* Constructor();

    void Clear();
    [[nodiscard]] bool CheckForInput() const;
};
VALIDATE_SIZE(CMouseControllerState, 0x14);
