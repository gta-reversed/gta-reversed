#pragma once

class CMouseControllerState {
public:
    uint8 lmb;
    uint8 rmb;
    uint8 mmb;
    uint8 wheelUp;
    uint8 wheelDown;
    uint8 bmx1;
    uint8 bmx2;
    char  __align;
    float Z;
    float X;
    float Y;

public:
    static void InjectHooks();

    CMouseControllerState();
    CMouseControllerState* Constructor();

    void Clear();
    bool CheckForInput();
};

VALIDATE_SIZE(CMouseControllerState, 0x14);
