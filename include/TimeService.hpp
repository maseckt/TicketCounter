#pragma once

#include <chrono>
#include <string>

class TimeService {
public:
    TimeService();
    void set_timezone_offset_minutes(int minutes);
    int timezone_offset_minutes() const;

    std::chrono::system_clock::time_point now_utc() const;
    std::chrono::system_clock::time_point now_local() const;

    std::chrono::system_clock::time_point next_reset_from(
        std::chrono::system_clock::time_point from_utc,
        int reset_hour,
        int reset_minute,
        int offset_minutes) const;

    std::chrono::system_clock::time_point next_reset_utc(
        int reset_hour,
        int reset_minute,
        int offset_minutes) const;

    static bool parse_hhmm(const std::string& s, int& hour, int& minute);
    std::string format_date_iso(std::chrono::system_clock::time_point tp_utc, int offset_minutes) const;

private:
    int offset_minutes_{};
};


