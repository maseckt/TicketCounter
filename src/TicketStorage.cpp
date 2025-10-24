#include "TicketStorage.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
using nlohmann::json;

TicketStorage::TicketStorage(const std::string& dataPath)
    : dataPath_(dataPath) {}

static json to_json(const StorageData& d) {
    json j;
    j["total"] = d.total;
    j["daily_goal"] = d.daily_goal;
    json hist = json::object();
    for (const auto& kv : d.history) {
        hist[kv.first] = { {"count", kv.second.count}, {"goal", kv.second.goal} };
    }
    j["history"] = hist;
    j["settings"]["theme"] = d.theme;
    j["settings"]["reset_hour"] = d.reset_hour;
    j["settings"]["reset_minute"] = d.reset_minute;
    j["settings"]["tz_offset_hours"] = d.tz_offset_hours;
    j["settings"]["hotkey_keyval"] = d.hotkey_keyval;
    j["settings"]["hotkey_minus_keyval"] = d.hotkey_minus_keyval;
    return j;
}

static bool from_json(const json& j, StorageData& d) {
    if (!j.is_object()) return false;
    if (!j.contains("total") || !j.contains("daily_goal") || !j.contains("history")) return false;
    d.total = j.value("total", 0);
    d.daily_goal = j.value("daily_goal", 0);
    d.history.clear();
    const auto& hist = j.at("history");
    if (!hist.is_object()) return false;
    for (auto it = hist.begin(); it != hist.end(); ++it) {
        const auto &val = it.value();
        DayEntry e;
        if (val.is_number()) {
            e.count = val.get<int>();
            e.goal = d.daily_goal;
        } else if (val.is_object()) {
            e.count = val.value("count", 0);
            e.goal = val.value("goal", d.daily_goal);
        } else {
            e.count = 0;
            e.goal = d.daily_goal;
        }
        d.history[it.key()] = e;
    }
    if (j.contains("settings") && j["settings"].is_object()) {
        const auto& sett = j["settings"];
        d.theme = sett.value("theme", "light");
        d.reset_hour = sett.value("reset_hour", 0);
        d.reset_minute = sett.value("reset_minute", 0);
        d.tz_offset_hours = sett.value("tz_offset_hours", 3);
        d.hotkey_keyval = sett.value("hotkey_keyval", 0u);
        d.hotkey_minus_keyval = sett.value("hotkey_minus_keyval", 0u);
    }
    return true;
}

bool TicketStorage::load(StorageData& out) {
    std::ifstream f(dataPath_);
    if (!f.is_open()) return false;
    json j;
    try { f >> j; } catch (...) { return false; }
    return from_json(j, out);
}

bool TicketStorage::save(const StorageData& in) {
    std::filesystem::path p(dataPath_);
    if (p.has_parent_path()) {
        std::error_code ec; std::filesystem::create_directories(p.parent_path(), ec);
    }
    std::string tmpPath = dataPath_ + ".tmp";
    std::ofstream f(tmpPath, std::ios::trunc);
    if (!f.is_open()) return false;
    json j = to_json(in);
    f << j.dump(2);
    f.close();
    std::error_code ec;
    std::filesystem::rename(tmpPath, dataPath_, ec);
    if (ec) {
        std::filesystem::remove(tmpPath, ec);
        return false;
    }
    return true;
}

bool TicketStorage::export_to(const std::string& path) {
    StorageData d;
    if (!load(d)) return false;
    std::ofstream f(path, std::ios::trunc);
    if (!f.is_open()) return false;
    f << to_json(d).dump(2);
    return true;
}

bool TicketStorage::import_from(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return false;
    json j;
    try { f >> j; } catch (...) { return false; }
    StorageData d;
    if (!from_json(j, d)) return false;
    return save(d);
}

bool TicketStorage::create_backup() {
    StorageData d;
    if (!load(d)) return false;
    std::string backupPath = dataPath_ + ".backup";
    std::ofstream f(backupPath, std::ios::trunc);
    if (!f.is_open()) return false;
    f << to_json(d).dump(2);
    return true;
}
