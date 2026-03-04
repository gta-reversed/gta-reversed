#pragma once

#include <stdint.h>

class CTimeInfo {
protected:
    uint8 m_On, m_Off;
    int16 m_OtherModel;

public:
    CTimeInfo() = default;

    void SetTimes(uint8 timeOn, uint8 timeOff) { m_On = timeOn; m_Off = timeOff; }
    int32 GetTimeOn() const { return m_On; }
    int32 GetTimeOff() const { return m_Off; }

    CTimeInfo* FindOtherTimeModel(const char* name);

    void SetOtherTimeModel(int32 index) { m_OtherModel = index; }
    int32 GetOtherTimeModel() const { return m_OtherModel; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

public:
    bool IsVisibleNow() const noexcept { return CClock::GetIsTimeInRange(GetTimeOn(), GetTimeOff()); }
};

VALIDATE_SIZE(CTimeInfo, 0x4);
