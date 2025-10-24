#pragma once

#include <gtkmm.h>
#include <memory>

class AppLogic;
class TimeService;

class MainPage : public Gtk::Box {
public:
    MainPage();
    void bind(std::shared_ptr<AppLogic> logic, std::shared_ptr<TimeService> timeService);
private:
    void on_plus();
    void on_minus();
    void on_apply_custom();
    void on_goal_changed();
    void update_ui();
    bool update_timer();
    bool on_key_press(guint keyval, guint keycode, Gdk::ModifierType state);
    void on_gesture_pressed(int n_press, double x, double y);

    std::shared_ptr<AppLogic> logic_;
    std::shared_ptr<TimeService> timeService_;

    Gtk::Label* labelToday_{};
    Gtk::Label* labelTotal_{};
    Gtk::Entry* goalEntry_{};
    Gtk::Label* labelPercent_{};
    Gtk::Label* labelCountdown_{};
    Gtk::Entry* customEntry_{};
    sigc::connection timer_conn_;
};


