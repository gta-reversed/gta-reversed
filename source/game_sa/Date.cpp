#include "StdInc.h"

#include "Date.h"

constexpr std::strong_ordering CDate::operator<=>(const CDate& o) {
    return std::tie(year, month, day, hours, minutes, seconds)
        <=> std::tie(o.year, o.month, o.day, o.hours, o.minutes, o.seconds);
}

constexpr void CDate::PopulateDateFields(const uint8& sec, const uint8& min, const uint8& hour, const uint8& _day, const uint8& mnt, int16 _year) {
    seconds = sec;
    minutes = min;
    hours   = hour;
    day     = _day;
    month   = mnt;
    year    = _year;
}
