#pragma once

#include <gtkmm.h>
#include <memory>

class AppLogic;
class TimeService;

class MainApp : public Gtk::Application {
public:
    static Glib::RefPtr<MainApp> create();
protected:
    MainApp();
    void on_activate() override;
private:
    std::shared_ptr<AppLogic> logic_;
    std::shared_ptr<TimeService> timeService_;
    sigc::connection autosave_conn_;
};


