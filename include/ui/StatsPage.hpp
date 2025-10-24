#pragma once

#include <gtkmm.h>
#include <memory>

class AppLogic;

class StatsPage : public Gtk::Box {
public:
    StatsPage();
    void bind(std::shared_ptr<AppLogic> logic);
private:
    void refresh();

    std::shared_ptr<AppLogic> logic_;
    Gtk::ScrolledWindow scroller_;
    Gtk::TreeView table_;

    struct Columns : public Gtk::TreeModel::ColumnRecord {
        Gtk::TreeModelColumn<Glib::ustring> date;
        Gtk::TreeModelColumn<int> count;
        Gtk::TreeModelColumn<int> goal;
        Gtk::TreeModelColumn<int> progress;
        Columns() { add(date); add(count); add(goal); add(progress); }
    } columns_;

    Glib::RefPtr<Gtk::ListStore> store_;
};
