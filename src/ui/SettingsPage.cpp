#include "ui/SettingsPage.hpp"
#include "ui/HotkeyBinder.hpp"
#include "AppLogic.hpp"
#include "TimeService.hpp"
#include <iostream>
#include <fstream>

SettingsPage::SettingsPage() : Gtk::Box(Gtk::Orientation::VERTICAL, 12) {
    set_margin(12);
    auto label = Gtk::make_managed<Gtk::Label>("Настройки");
    label->set_margin(12);
    append(*label);

    auto frame = Gtk::make_managed<Gtk::Frame>();
    frame->set_margin(8);
    auto grid = Gtk::make_managed<Gtk::Grid>();
    grid->set_row_spacing(12);
    grid->set_column_spacing(12);
    grid->set_margin(16);
    frame->set_child(*grid);
    append(*frame);

    auto theme_label = Gtk::make_managed<Gtk::Label>("Тема:");
    theme_label->set_halign(Gtk::Align::START);
    grid->attach(*theme_label, 0, 0);

    theme_combo_ = Gtk::make_managed<Gtk::ComboBoxText>();
    theme_combo_->append("Светлая");
    theme_combo_->append("Темная");
    theme_combo_->set_active(0);
    theme_combo_->signal_changed().connect(sigc::mem_fun(*this, &SettingsPage::on_theme_changed));
    grid->attach(*theme_combo_, 1, 0);

    auto reset_label = Gtk::make_managed<Gtk::Label>("Время сброса (HH:MM):");
    reset_label->set_halign(Gtk::Align::START);
    grid->attach(*reset_label, 0, 1);

    reset_time_entry_ = Gtk::make_managed<Gtk::Entry>();
    reset_time_entry_->signal_changed().connect(sigc::mem_fun(*this, &SettingsPage::on_reset_time_changed));
    grid->attach(*reset_time_entry_, 1, 1);

    auto offset_label = Gtk::make_managed<Gtk::Label>("Смещение UTC (часы):");
    offset_label->set_halign(Gtk::Align::START);
    grid->attach(*offset_label, 0, 2);

    offset_entry_ = Gtk::make_managed<Gtk::Entry>();
    offset_entry_->signal_changed().connect(sigc::mem_fun(*this, &SettingsPage::on_offset_changed));
    grid->attach(*offset_entry_, 1, 2);

    auto hotkey_label = Gtk::make_managed<Gtk::Label>("Клавиша для +1:");
    hotkey_label->set_halign(Gtk::Align::START);
    grid->attach(*hotkey_label, 0, 3);

    hotkey_entry_ = Gtk::make_managed<Gtk::Entry>();
    hotkey_entry_->set_placeholder_text("Нажмите клавишу для бинда...");
    grid->attach(*hotkey_entry_, 1, 3);

    auto hotkey_minus_label = Gtk::make_managed<Gtk::Label>("Клавиша для -1:");
    hotkey_minus_label->set_halign(Gtk::Align::START);
    grid->attach(*hotkey_minus_label, 0, 4);

    hotkey_minus_entry_ = Gtk::make_managed<Gtk::Entry>();
    hotkey_minus_entry_->set_placeholder_text("Нажмите клавишу для бинда...");
    grid->attach(*hotkey_minus_entry_, 1, 4);

    auto spacer = Gtk::make_managed<Gtk::Separator>();
    grid->attach(*spacer, 0, 6, 2, 1);

    backup_btn_ = Gtk::make_managed<Gtk::Button>("Бекап данных");
    backup_btn_->signal_clicked().connect(sigc::mem_fun(*this, &SettingsPage::on_backup_clicked));
    backup_btn_->set_size_request(200, 48);
    grid->attach(*backup_btn_, 0, 7, 2, 1);

    clear_btn_ = Gtk::make_managed<Gtk::Button>("Очистить все данные");
    clear_btn_->signal_clicked().connect(sigc::mem_fun(*this, &SettingsPage::on_clear_data_clicked));
    clear_btn_->add_css_class("destructive-action");
    clear_btn_->set_size_request(200, 48);
    grid->attach(*clear_btn_, 0, 8, 2, 1);
}

void SettingsPage::bind(std::shared_ptr<AppLogic> logic) {
    logic_ = logic;
    binder_plus_ = new HotkeyBinder();
    binder_plus_->bind(logic);
    binder_plus_->set_entry(hotkey_entry_);
    binder_plus_->set_callback([logic](unsigned int keyval) {
        logic->set_hotkey_keyval(keyval);
    });
    binder_plus_->set_get_current_keyval([logic]() { return logic->hotkey_keyval(); });

    binder_minus_ = new HotkeyBinder();
    binder_minus_->bind(logic);
    binder_minus_->set_entry(hotkey_minus_entry_);
    binder_minus_->set_callback([logic](unsigned int keyval) {
        logic->set_hotkey_minus_keyval(keyval);
    });
    binder_minus_->set_get_current_keyval([logic]() { return logic->hotkey_minus_keyval(); });

    update_ui();
}


void SettingsPage::update_ui() {
    if (!logic_) return;
    reset_time_entry_->set_text(logic_->reset_time_hhmm());
    int hours = logic_->tz_offset_hours();
    std::string sign = hours >= 0 ? "+" : "";
    offset_entry_->set_text(sign + std::to_string(hours));
    unsigned int key = logic_->hotkey_keyval();
    if (key != 0) {
        hotkey_entry_->set_text(binder_plus_->keyval_to_name(key));
    } else {
        hotkey_entry_->set_text("");
    }
    unsigned int key_minus = logic_->hotkey_minus_keyval();
    if (key_minus != 0) {
        hotkey_minus_entry_->set_text(binder_minus_->keyval_to_name(key_minus));
    } else {
        hotkey_minus_entry_->set_text("");
    }
    // Update theme combo
    if (logic_->theme() == "light") theme_combo_->set_active(0);
    else if (logic_->theme() == "dark") theme_combo_->set_active(1);
    else theme_combo_->set_active(0); // default to light
}

void SettingsPage::on_theme_changed() {
    if (!logic_) return;
    std::string theme = theme_combo_->get_active_text();
    if (theme == "Светлая") logic_->set_theme("light");
    else if (theme == "Темная") logic_->set_theme("dark");
}

void SettingsPage::on_reset_time_changed() {
    if (!logic_) return;
    std::string time = reset_time_entry_->get_text();
    logic_->set_reset_time_hhmm(time);
    int h, m;
    if (TimeService::parse_hhmm(time, h, m)) {
        logic_->set_reset_hour(h);
        logic_->set_reset_minute(m);
    }
}

void SettingsPage::on_offset_changed() {
    if (!logic_) return;
    std::string text = offset_entry_->get_text();
    if (text.empty()) return;
    try {
        int hours = std::stoi(text);
        logic_->set_tz_offset_hours(hours);
    } catch (...) {
        // Invalid input, ignore
    }
}


void SettingsPage::on_backup_clicked() {
    if (!logic_) return;
    std::ifstream check("data/tickets.json.backup");
    bool exists = check.good();
    check.close();
    if (exists) {
        auto dialog = Gtk::MessageDialog("Вы точно хотите перезаписать файл tickets.json.backup?", false, Gtk::MessageType::QUESTION, Gtk::ButtonsType::YES_NO);
        dialog.set_transient_for(*dynamic_cast<Gtk::Window*>(get_root()));
        dialog.signal_response().connect([this, &dialog](int response) {
            if (response == Gtk::ResponseType::YES) {
                if (logic_->create_backup()) {
                    std::cout << "Backup created" << std::endl;
                } else {
                    std::cout << "Backup failed" << std::endl;
                }
            }
            dialog.hide();
        });
        dialog.show();
    } else {
        if (logic_->create_backup()) {
            std::cout << "Backup created" << std::endl;
        } else {
            std::cout << "Backup failed" << std::endl;
        }
    }
}

void SettingsPage::on_clear_data_clicked() {
    if (!logic_) return;
    auto dialog = Gtk::MessageDialog("Вы уверены, что хотите очистить все данные? Это действие нельзя отменить.", false, Gtk::MessageType::QUESTION, Gtk::ButtonsType::YES_NO);
    dialog.set_transient_for(*dynamic_cast<Gtk::Window*>(get_root()));
    dialog.signal_response().connect([this, &dialog](int response) {
        if (response == Gtk::ResponseType::YES) {
            logic_->clear_all_data();
            update_ui();
            std::cout << "Data cleared" << std::endl;
        }
        dialog.hide();
    });
    dialog.show();
}


