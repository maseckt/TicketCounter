#pragma once

#include <string>
#include <map>

struct DayEntry {
    int count = 0;
    int goal = 0;
};

struct StorageData {
    int total = 0;
    int daily_goal = 0; // глобальная дефолтная цель (fallback / для миграции)
    std::map<std::string, DayEntry> history;
    std::string theme = "light"; // light, dark, system
    int reset_hour = 0;
    int reset_minute = 0;
    int tz_offset_hours = 3; // МСК по умолчанию
    unsigned int hotkey_keyval = 0;
    unsigned int hotkey_minus_keyval = 0;
};

class TicketStorage {
public:
    explicit TicketStorage(const std::string& dataPath);
    bool load(StorageData& out);
    bool save(const StorageData& in);
    bool export_to(const std::string& path);
    bool import_from(const std::string& path);
    bool create_backup();
private:
    std::string dataPath_;
};
