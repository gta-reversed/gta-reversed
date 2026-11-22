/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CTickCounter {
public:
    CTickCounter() : Count(0), Period(0) {}

    ~CTickCounter() = default;

    void SetPeriod(const int32 period) { Period = period; }
    int32 GetPeriod() { return Period; }

    bool Tick() {
        bool shouldTrigger = Count == 0;
        Count++;
        if (Count > Period) {
            Count = 0;
        }
        return shouldTrigger;
    }

    void SetCount(int32 count) { Count = count; }

private:
    int32 Count;
    int32 Period;
};

VALIDATE_SIZE(CTickCounter, 0x8);
