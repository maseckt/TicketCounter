#pragma once

#include <gtkmm.h>
#include <memory>
#include <functional>

class AppLogic;

class HotkeyBinder {
public:
    HotkeyBinder();
    void bind(std::shared_ptr<AppLogic> logic);
    void set_entry(Gtk::Entry* entry);
    void set_callback(std::function<void(unsigned int)> callback);
    void set_get_current_keyval(std::function<unsigned int()> f);
    std::string keyval_to_name(guint keyval) const;

private:
    bool on_key_press(guint keyval, guint keycode, Gdk::ModifierType state);
    void on_gesture_pressed(int n_press, double x, double y);

    std::shared_ptr<AppLogic> logic_;
    Gtk::Entry* entry_;
    std::function<void(unsigned int)> callback_;
    std::function<unsigned int()> get_current_keyval_;
};