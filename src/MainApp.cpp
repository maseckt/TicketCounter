#include "MainApp.hpp"
#include "AppLogic.hpp"
#include "TicketStorage.hpp"
#include "TimeService.hpp"
#include "ui/MainPage.hpp"
#include "ui/StatsPage.hpp"
#include "ui/GraphPage.hpp"
#include "ui/SettingsPage.hpp"
#include <gtkmm.h>
#include <iostream>
#include <fstream>
#include <filesystem>

Glib::RefPtr<MainApp> MainApp::create() {
    return Glib::RefPtr<MainApp>(new MainApp());
}

int main(int argc, char** argv) {
    auto app = MainApp::create();
    return app->run(argc, argv);
}

MainApp::MainApp() : Gtk::Application("app.ticketcounter") {}

void MainApp::on_activate() {
    auto window = new Gtk::ApplicationWindow();
    window->set_title("TicketCounter");
    window->set_default_size(800, 600);

    auto storage = std::make_shared<TicketStorage>("data/tickets.json");
    timeService_ = std::make_shared<TimeService>();

    std::ifstream check("data/tickets.json");
    if (!check.good()) {
        check.close();
        std::ifstream backup("data/tickets.json.backup");
        if (backup.good()) {
            std::cout << "Restoring data from backup..." << std::endl;
            std::filesystem::copy("data/tickets.json.backup", "data/tickets.json");
        }
    }

    logic_ = std::make_shared<AppLogic>(storage, timeService_);

    auto stack = Gtk::make_managed<Gtk::Stack>();
    stack->set_transition_type(Gtk::StackTransitionType::SLIDE_LEFT_RIGHT);

    auto mainPage = Gtk::make_managed<MainPage>();
    mainPage->bind(logic_, timeService_);
    auto statsPage = Gtk::make_managed<StatsPage>();
    statsPage->bind(logic_);
    auto graphPage = Gtk::make_managed<GraphPage>();
    graphPage->bind(logic_);
    auto settingsPage = Gtk::make_managed<SettingsPage>();
    settingsPage->bind(logic_);

    stack->add(*mainPage, "main", "Главная");
    stack->add(*statsPage, "stats", "Статистика");
    stack->add(*graphPage, "graph", "Граф");
    stack->add(*settingsPage, "settings", "Настройки");

    auto switcher = Gtk::make_managed<Gtk::StackSwitcher>();
    switcher->set_stack(*stack);

    auto header = Gtk::make_managed<Gtk::HeaderBar>();
    header->set_title_widget(*switcher);
    window->set_titlebar(*header);

    window->set_child(*stack);
    window->present();
    add_window(*window);

    window->signal_close_request().connect([this, logic = logic_]() {
        if (logic) logic->save();
        return false;
    }, false);

    if (!autosave_conn_.connected()) {
        autosave_conn_ = Glib::signal_timeout().connect([this]() {
            if (logic_) {
                logic_->tick();
                logic_->save();
            }
            return true;
        }, 3000);
    }
}


