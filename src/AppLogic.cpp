#include "AppLogic.hpp"
#include "TicketStorage.hpp"
#include "TimeService.hpp"
#include <nlohmann/json.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

AppLogic::AppLogic(std::shared_ptr<TicketStorage> storage, std::shared_ptr<TimeService> timeService)
    : storage_(std::move(storage)), timeService_(std::move(timeService)) {
    data_ = std::make_unique<StorageData>();
    StorageData loaded{};
    if (storage_->load(loaded)) {
        *data_ = loaded;
    } else {
        data_->total = 0;
        data_->daily_goal = 0;
        data_->history.clear();
        data_->theme = "light";
        data_->reset_hour = 0;
        data_->reset_minute = 0;
        data_->tz_offset_hours = 3;
        data_->hotkey_keyval = 0;
        data_->hotkey_minus_keyval = 0;
    }
    for (auto &kv : data_->history) {
        if (kv.second.goal == 0) kv.second.goal = data_->daily_goal;
    }
    tz_offset_minutes_ = data_->tz_offset_hours * 60;
    timeService_->set_timezone_offset_minutes(tz_offset_minutes_);
    reset_hour_ = data_->reset_hour;
    reset_minute_ = data_->reset_minute;
    hotkey_keyval_ = data_->hotkey_keyval;
    hotkey_minus_keyval_ = data_->hotkey_minus_keyval;
    current_date_ = today_date_iso();
}

void AppLogic::set_timezone_offset_minutes(int minutes) {
    tz_offset_minutes_ = minutes;
    timeService_->set_timezone_offset_minutes(minutes);
    ensure_date_up_to_date();
}

void AppLogic::set_reset_time_hhmm(const std::string& hhmm) {
    int h = 0, m = 0;
    if (TimeService::parse_hhmm(hhmm, h, m)) {
        reset_hour_ = h;
        reset_minute_ = m;
        ensure_date_up_to_date();
    }
}

int AppLogic::timezone_offset_minutes() const { return tz_offset_minutes_; }

std::string AppLogic::reset_time_hhmm() const {
    char buf[6];
    std::snprintf(buf, sizeof(buf), "%02d:%02d", reset_hour_, reset_minute_);
    return std::string(buf);
}

void AppLogic::set_hotkey_keyval(unsigned int keyval) {
    hotkey_keyval_ = keyval;
    data_->hotkey_keyval = keyval;
    save();
}

void AppLogic::set_hotkey_minus_keyval(unsigned int keyval) {
    hotkey_minus_keyval_ = keyval;
    data_->hotkey_minus_keyval = keyval;
    save();
}

unsigned int AppLogic::hotkey_keyval() const {
    return hotkey_keyval_;
}

unsigned int AppLogic::hotkey_minus_keyval() const {
    return hotkey_minus_keyval_;
}

void AppLogic::set_theme(const std::string& theme) {
    data_->theme = theme;
    save();
}

std::string AppLogic::theme() const {
    return data_->theme;
}

void AppLogic::set_reset_hour(int h) {
    data_->reset_hour = h;
    reset_hour_ = h;
    ensure_date_up_to_date();
    save();
}

void AppLogic::set_reset_minute(int m) {
    data_->reset_minute = m;
    reset_minute_ = m;
    ensure_date_up_to_date();
    save();
}

int AppLogic::reset_hour() const {
    return data_->reset_hour;
}

int AppLogic::reset_minute() const {
    return data_->reset_minute;
}

void AppLogic::set_tz_offset_hours(int hours) {
    data_->tz_offset_hours = hours;
    tz_offset_minutes_ = hours * 60;
    timeService_->set_timezone_offset_minutes(tz_offset_minutes_);
    ensure_date_up_to_date();
    save();
}

int AppLogic::tz_offset_hours() const {
    return data_->tz_offset_hours;
}

int AppLogic::get_today_count() {
    ensure_date_up_to_date();
    auto it = data_->history.find(current_date_);
    if (it == data_->history.end()) return 0;
    return it->second.count;
}

int AppLogic::get_total() { return data_->total; }
int AppLogic::get_goal() { return data_->daily_goal; }
void AppLogic::set_goal(int goal) { data_->daily_goal = goal; }

int AppLogic::get_progress_percent() {
    int goal = get_goal();
    if (goal <= 0) return 0;
    int today = get_today_count();
    int p = (today * 100) / goal;
    if (p < 0) p = 0;
    if (p > 999) p = 999;
    return p;
}

bool AppLogic::apply_delta(int delta) {
    ensure_date_up_to_date();
    auto& entry = data_->history[current_date_];
    int before = entry.count;
    entry.count = before + delta;
    if (entry.count < 0) entry.count = 0;
    int delta_applied = entry.count - before;
    data_->total += delta_applied;
    if (data_->total < 0) data_->total = 0;
    return true;
}

bool AppLogic::apply_delta_string(const std::string& s) {
    if (s.empty()) return false;
    int sign = 1;
    size_t idx = 0;
    if (s[0] == '+') { sign = 1; idx = 1; }
    else if (s[0] == '-') { sign = -1; idx = 1; }
    int value = 0;
    try {
        value = std::stoi(s.substr(idx));
    } catch (...) { return false; }
    return apply_delta(sign * value);
}

bool AppLogic::save() {
    return storage_->save(*data_);
}

void AppLogic::ensure_date_up_to_date() {
    auto now = timeService_->now_utc();
    auto next_reset = timeService_->next_reset_from(now, reset_hour_, reset_minute_, tz_offset_minutes_);
    auto today = today_date_iso();
    if (current_date_.empty()) current_date_ = today;
    if (now >= next_reset) {
        current_date_ = today;
    } else {
        current_date_ = today;
    }
}

std::string AppLogic::today_date_iso() const {
    return timeService_->format_date_iso(timeService_->now_utc(), tz_offset_minutes_);
}

void AppLogic::tick() {
    ensure_date_up_to_date();
}

std::vector<std::pair<std::string,int>> AppLogic::get_history() const {
    std::vector<std::pair<std::string,int>> out;
    out.reserve(data_->history.size());
    for (const auto& kv : data_->history) out.emplace_back(kv.first, kv.second.count);
    std::sort(out.begin(), out.end(), [](const auto& a, const auto& b){ return a.first > b.first; });
    return out;
}

std::vector<std::tuple<std::string,int,int>> AppLogic::get_history_with_goals() const {
    std::vector<std::tuple<std::string,int,int>> out;
    out.reserve(data_->history.size());
    for (const auto& kv : data_->history) out.emplace_back(kv.first, kv.second.count, kv.second.goal);
    std::sort(out.begin(), out.end(), [](const auto& a, const auto& b){ return std::get<0>(a) > std::get<0>(b); });
    return out;
}

int AppLogic::get_goal_for_date(const std::string& date) const {
    auto it = data_->history.find(date);
    if (it != data_->history.end()) return it->second.goal;
    return data_->daily_goal;
}

void AppLogic::set_goal_for_date(const std::string& date, int goal) {
    data_->history[date].goal = goal;
    save();
}

bool AppLogic::export_json(const std::string& path) {
    return storage_->export_to(path);
}

bool AppLogic::import_json(const std::string& path) {
    if (!storage_->import_from(path)) return false;
    StorageData loaded{};
    if (storage_->load(loaded)) {
        *data_ = loaded;
        for (auto &kv : data_->history) {
            if (kv.second.goal == 0) kv.second.goal = data_->daily_goal;
        }
        recalculate_total();
        return true;
    }
    return false;
}


bool AppLogic::create_backup() {
    return storage_->create_backup();
}

void AppLogic::clear_all_data() {
    data_->total = 0;
    data_->daily_goal = 0;
    data_->history.clear();
    data_->reset_hour = 0;
    data_->reset_minute = 0;
    data_->tz_offset_hours = 3;
    data_->hotkey_keyval = 0;
    data_->hotkey_minus_keyval = 0;
    save();
    std::cout << "Data cleared successfully" << std::endl;
}

void AppLogic::clear_cached_history() {
}

void AppLogic::recalculate_total() {
    int sum = 0;
    for (const auto& kv : data_->history) {
        sum += kv.second.count;
    }
    data_->total = sum;
}
