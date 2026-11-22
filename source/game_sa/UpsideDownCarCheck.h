/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CVehicle;

struct UpsideDownCar {
    int32  m_nHandle;
    uint32 m_nTime;

    UpsideDownCar() { // 0x468DE0
        Clear();
    };

    void Clear() {
        m_nHandle = -1;
        m_nTime   = 0;
    }
};

class CUpsideDownCarCheck {
public:
    static constexpr auto UPSIDE_DOWN_CAR_MIN_TIME{ 2000u };
    std::array<UpsideDownCar, 6> m_aUpsideDownCars;

public:
    static void InjectHooks();

    void Init();
    bool IsCarUpsideDown(int32 carHandle);
    bool IsCarUpsideDown(const CVehicle* vehicle);
    bool AreAnyCarsUpsideDown();
    void UpdateTimers();
    void AddCarToCheck(int32 car);
    void RemoveCarFromCheck(int32 carHandle);
    bool HasCarBeenUpsideDownForAWhile(int32 car);
};

VALIDATE_SIZE(CUpsideDownCarCheck, 0x30);
