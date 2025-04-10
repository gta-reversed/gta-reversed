/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CDate {
public:
    int32 seconds;
    int32 minutes;
    int32 hours;
    int32 day;
    int32 month;
    int32 year;

public:
    CDate();

    bool operator<(const CDate& rhs);
    bool operator==(const CDate& rhs);
    bool operator>(const CDate& rhs);

    void PopulateDateFields(const char& seconds, const char& minutes, const char& hours, const char& day, const char& month, int16 year);
};

VALIDATE_SIZE(CDate, 0x18);
