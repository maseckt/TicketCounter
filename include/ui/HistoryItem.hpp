#pragma once

#include <glibmm.h>
#include <string>

class HistoryItem : public Glib::Object {
public:
    using Ptr = Glib::RefPtr<HistoryItem>;
    static Ptr create(const std::string& date, int count) {
        return Glib::make_refptr_for_instance<HistoryItem>(new HistoryItem(date, count));
    }
    const std::string& date() const { return date_; }
    int count() const { return count_; }
private:
    HistoryItem(const std::string& d, int c) : date_(d), count_(c) {}
    std::string date_;
    int count_{};
};


