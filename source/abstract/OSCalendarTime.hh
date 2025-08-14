#pragma once

#include "Common.hh"

#include <ctime>

namespace Abstract {

struct OSCalendarTime {
    OSCalendarTime() {
        time_t currentTime = std::time(nullptr);
        tm *localTime = std::localtime(&currentTime);
        sec = localTime->tm_sec;
        min = localTime->tm_min;
        hour = localTime->tm_hour;
        day = localTime->tm_mday;
        month = localTime->tm_mon;
        year = 1900 + localTime->tm_year;
    }

    u32 sec;
    u32 min;
    u32 hour;
    u32 day;
    u32 month;
    u32 year;
};

} // namespace Abstract
