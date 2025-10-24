#pragma once

#include <gtkmm.h>
#include <memory>

class AppLogic;
class HotkeyBinder;

class SettingsPage : public Gtk::Box {
public:
    SettingsPage();
    void bind(std::shared_ptr<AppLogic> logic);
private:
    void on_theme_changed();
    void on_reset_time_changed();
    void on_offset_changed();
    void on_backup_clicked();
    void on_clear_data_clicked();
    void update_ui();

    std::shared_ptr<AppLogic> logic_;
    Gtk::ComboBoxText* theme_combo_{};
    Gtk::Entry* reset_time_entry_{};
    Gtk::Entry* offset_entry_{};
    Gtk::Entry* hotkey_entry_{};
    Gtk::Entry* hotkey_minus_entry_{};
    HotkeyBinder* binder_plus_;
    HotkeyBinder* binder_minus_;
    Gtk::Button* backup_btn_{};
    Gtk::Button* clear_btn_{};
};


