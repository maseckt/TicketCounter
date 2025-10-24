#include "TimeService.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

TimeService::TimeService() = default;

void TimeService::set_timezone_offset_minutes(int minutes) {
    offset_minutes_ = minutes;
}

int TimeService::timezone_offset_minutes() const {
    return offset_minutes_;
}

std::chrono::system_clock::time_point TimeService::now_utc() const {
    return std::chrono::system_clock::now();
}

std::chrono::system_clock::time_point TimeService::now_local() const {
    return now_utc() + std::chrono::minutes(offset_minutes_);
}

static std::tm break_down_utc(std::chrono::system_clock::time_point tp) {
    auto t = std::chrono::system_clock::to_time_t(tp);
    std::tm out{};
    #ifdef _WIN32
    gmtime_s(&out, &t);
    #else
    gmtime_r(&t, &out);
    #endif
    return out;
}

std::chrono::system_clock::time_point TimeService::next_reset_from(
    std::chrono::system_clock::time_point from_utc,
    int reset_hour,
    int reset_minute,
    int offset_minutes) const {

        auto local_from = from_utc + std::chrono::minutes(offset_minutes);
        auto tm_local = break_down_utc(local_from);

        tm_local.tm_hour = reset_hour;
        tm_local.tm_min = reset_minute;
        tm_local.tm_sec = 0;

        #ifdef _WIN32
        auto local_reset_t = _mkgmtime(&tm_local);
        #else
        auto local_reset_t = timegm(&tm_local);
        #endif
        auto local_reset = std::chrono::system_clock::from_time_t(local_reset_t);

        if (local_reset <= local_from) {
            tm_local.tm_mday += 1;
            #ifdef _WIN32
            local_reset_t = _mkgmtime(&tm_local);
            #else
            local_reset_t = timegm(&tm_local);
            #endif
            local_reset = std::chrono::system_clock::from_time_t(local_reset_t);
        }

        return local_reset - std::chrono::minutes(offset_minutes);
    }

    std::chrono::system_clock::time_point TimeService::next_reset_utc(
        int reset_hour,
        int reset_minute,
        int offset_minutes) const {
            return next_reset_from(now_utc(), reset_hour, reset_minute, offset_minutes);
        }

        bool TimeService::parse_hhmm(const std::string& s, int& hour, int& minute) {
            if (s.size() != 5 || s[2] != ':') return false;
            try {
                hour = std::stoi(s.substr(0,2));
                minute = std::stoi(s.substr(3,2));
            } catch (...) {
                return false;
            }
            if (hour < 0 || hour > 23) return false;
            if (minute < 0 || minute > 59) return false;
            return true;
        }

        std::string TimeService::format_date_iso(std::chrono::system_clock::time_point tp_utc, int offset_minutes) const {
            auto local_tp = tp_utc + std::chrono::minutes(offset_minutes);
            auto t = std::chrono::system_clock::to_time_t(local_tp);
            std::tm tm{};
            #ifdef _WIN32
            gmtime_s(&tm, &t);
            #else
            gmtime_r(&t, &tm);
            #endif
            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%d");
            return oss.str();
        }
