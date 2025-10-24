#pragma once

#include <memory>
#include <string>
#include <vector>
#include <tuple>

class TicketStorage;
class TimeService;

struct StorageData;

class AppLogic {
public:
    AppLogic(std::shared_ptr<TicketStorage> storage, std::shared_ptr<TimeService> timeService);
    void set_timezone_offset_minutes(int minutes);
    void set_reset_time_hhmm(const std::string& hhmm);
    int timezone_offset_minutes() const;
    std::string reset_time_hhmm() const;
    void set_hotkey_keyval(unsigned int keyval);
    void set_hotkey_minus_keyval(unsigned int keyval);
    unsigned int hotkey_keyval() const;
    unsigned int hotkey_minus_keyval() const;

    void set_theme(const std::string& theme);
    std::string theme() const;
    void set_reset_hour(int h);
    void set_reset_minute(int m);
    int reset_hour() const;
    int reset_minute() const;
    void set_tz_offset_hours(int hours);
    int tz_offset_hours() const;

    int get_today_count();
    int get_total();
    int get_goal();
    void set_goal(int goal);
    int get_progress_percent();

    bool apply_delta(int delta);
    bool apply_delta_string(const std::string& s);
    bool save();
    std::vector<std::pair<std::string,int>> get_history() const;
    void clear_cached_history();
    void recalculate_total();

    std::vector<std::tuple<std::string,int,int>> get_history_with_goals() const;
    int get_goal_for_date(const std::string& date) const;
    void set_goal_for_date(const std::string& date, int goal);

    bool export_json(const std::string& path);
    bool import_json(const std::string& path);
    bool create_backup();
    void clear_all_data();
    void tick();

private:
    void ensure_date_up_to_date();
    std::string today_date_iso() const;

    std::shared_ptr<TicketStorage> storage_;
    std::shared_ptr<TimeService> timeService_;
    std::unique_ptr<StorageData> data_;
    std::string current_date_;
    int reset_hour_{};
    int reset_minute_{};
    int tz_offset_minutes_{};
    unsigned int hotkey_keyval_{0};
    unsigned int hotkey_minus_keyval_{0};
};
