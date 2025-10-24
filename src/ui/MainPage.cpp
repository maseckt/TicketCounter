#include "ui/MainPage.hpp"
#include "AppLogic.hpp"
#include "TimeService.hpp"

MainPage::MainPage() : Gtk::Box(Gtk::Orientation::VERTICAL, 12) {
    set_margin(16);
    auto key_controller = Gtk::EventControllerKey::create();
    key_controller->signal_key_pressed().connect(sigc::mem_fun(*this, &MainPage::on_key_press), false);
    add_controller(key_controller);

    auto gesture = Gtk::GestureClick::create();
    gesture->signal_pressed().connect(sigc::mem_fun(*this, &MainPage::on_gesture_pressed));
    add_controller(gesture);

    auto frame = Gtk::make_managed<Gtk::Frame>();
    frame->set_margin(8);
    auto inner = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    inner->set_margin(16);
    frame->set_child(*inner);
    append(*frame);

    auto header = Gtk::make_managed<Gtk::Label>("Счётчик за сегодня");
    header->set_margin_bottom(8);
    header->set_halign(Gtk::Align::CENTER);
    header->set_css_classes({"title-2"});
    inner->append(*header);

    labelToday_ = Gtk::make_managed<Gtk::Label>("0");
    labelToday_->set_margin_bottom(8);
    labelToday_->set_halign(Gtk::Align::CENTER);
    labelToday_->set_css_classes({"title-1"});
    inner->append(*labelToday_);

    auto rowButtons = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 12);
    rowButtons->set_halign(Gtk::Align::CENTER);
    auto btnMinus = Gtk::make_managed<Gtk::Button>("−1");
    auto btnPlus = Gtk::make_managed<Gtk::Button>("+1");
    btnMinus->add_css_class("destructive-action");
    btnPlus->add_css_class("suggested-action");
    btnMinus->set_size_request(120, 48);
    btnPlus->set_size_request(120, 48);
    rowButtons->append(*btnMinus);
    rowButtons->append(*btnPlus);
    inner->append(*rowButtons);

    auto rowTotals = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 12);
    rowTotals->set_halign(Gtk::Align::CENTER);
    auto lblTotalCaption = Gtk::make_managed<Gtk::Label>("Всего:");
    labelTotal_ = Gtk::make_managed<Gtk::Label>("0");
    rowTotals->append(*lblTotalCaption);
    rowTotals->append(*labelTotal_);
    inner->append(*rowTotals);

    auto rowGoal = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 12);
    rowGoal->set_halign(Gtk::Align::CENTER);
    auto lblGoal = Gtk::make_managed<Gtk::Label>("Цель на день:");
    goalEntry_ = Gtk::make_managed<Gtk::Entry>();
    goalEntry_->set_width_chars(6);
    rowGoal->append(*lblGoal);
    rowGoal->append(*goalEntry_);
    inner->append(*rowGoal);

    auto rowPercent = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 12);
    rowPercent->set_halign(Gtk::Align::CENTER);
    auto lblPercent = Gtk::make_managed<Gtk::Label>("Прогресс:");
    labelPercent_ = Gtk::make_managed<Gtk::Label>("0%");
    labelPercent_->set_css_classes({"title-2"});
    rowPercent->append(*lblPercent);
    rowPercent->append(*labelPercent_);
    inner->append(*rowPercent);

    auto rowCountdown = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 12);
    rowCountdown->set_halign(Gtk::Align::CENTER);
    auto lblCountdown = Gtk::make_managed<Gtk::Label>("Осталось:");
    labelCountdown_ = Gtk::make_managed<Gtk::Label>("--:--:--");
    rowCountdown->append(*lblCountdown);
    rowCountdown->append(*labelCountdown_);
    inner->append(*rowCountdown);

    auto rowCustom = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 12);
    rowCustom->set_halign(Gtk::Align::CENTER);
    customEntry_ = Gtk::make_managed<Gtk::Entry>();
    customEntry_->set_placeholder_text("+3, -2 ...");
    customEntry_->set_width_chars(10);
    rowCustom->append(*customEntry_);
    inner->append(*rowCustom);

    btnPlus->signal_clicked().connect(sigc::mem_fun(*this, &MainPage::on_plus));
    btnMinus->signal_clicked().connect(sigc::mem_fun(*this, &MainPage::on_minus));
    customEntry_->signal_activate().connect(sigc::mem_fun(*this, &MainPage::on_apply_custom));
    goalEntry_->signal_activate().connect(sigc::mem_fun(*this, &MainPage::on_goal_changed));
}

void MainPage::bind(std::shared_ptr<AppLogic> logic, std::shared_ptr<TimeService> timeService) {
    logic_ = std::move(logic);
    timeService_ = std::move(timeService);
    update_ui();
    if (!timer_conn_.connected()) {
        timer_conn_ = Glib::signal_timeout().connect(sigc::mem_fun(*this, &MainPage::update_timer), 1000);
    }
}

void MainPage::on_plus() {
    if (!logic_) return;
    logic_->apply_delta(+1);
    logic_->save();
    update_ui();
}

void MainPage::on_minus() {
    if (!logic_) return;
    logic_->apply_delta(-1);
    logic_->save();
    update_ui();
}

void MainPage::on_apply_custom() {
    if (!logic_) return;
    auto s = customEntry_->get_text();
    if (logic_->apply_delta_string(s)) {
        logic_->save();
        customEntry_->set_text("");
        update_ui();
    }
}

void MainPage::on_goal_changed() {
    if (!logic_) return;
    try {
        int g = std::stoi(goalEntry_->get_text());
        logic_->set_goal(g);
        logic_->save();
        update_ui();
    } catch (...) {}
}

bool MainPage::on_key_press(guint keyval, guint keycode, Gdk::ModifierType state) {
    if ((state & Gdk::ModifierType::CONTROL_MASK) != Gdk::ModifierType::CONTROL_MASK) return false;
    if (keyval == GDK_KEY_plus || keyval == GDK_KEY_equal) {
        on_plus();
        return true;
    } else if (keyval == GDK_KEY_minus) {
        on_minus();
        return true;
    }
    if (logic_ && keyval == logic_->hotkey_keyval()) {
        on_plus();
        return true;
    }
    if (logic_ && keyval == logic_->hotkey_minus_keyval()) {
        on_minus();
        return true;
    }

    // Handle mouse buttons
    if (keyval >= 0x10000000 && keyval <= 0x10000009) {
        int button = keyval - 0x10000000;
        if (logic_ && button == 8 && logic_->hotkey_keyval() == 0x10000008) {
            on_plus();
            return true;
        } else if (logic_ && button == 9 && logic_->hotkey_minus_keyval() == 0x10000009) {
            on_minus();
            return true;
        }
    }
    return false;
}

void MainPage::on_gesture_pressed(int n_press, double x, double y) {
    if (!logic_) return;
    if (n_press == 8 && logic_->hotkey_keyval() == 0x10000008) {
        on_plus();
    } else if (n_press == 9 && logic_->hotkey_minus_keyval() == 0x10000009) {
        on_minus();
    }
}

void MainPage::update_ui() {
    if (!logic_) return;
    labelToday_->set_text(std::to_string(logic_->get_today_count()));
    labelTotal_->set_text(std::to_string(logic_->get_total()));
    goalEntry_->set_text(std::to_string(logic_->get_goal()));
    labelPercent_->set_text(std::to_string(logic_->get_progress_percent()) + "%");
    update_timer();
}

bool MainPage::update_timer() {
    if (!logic_ || !timeService_) return true;
    int h = 0, m = 0;
    TimeService::parse_hhmm(logic_->reset_time_hhmm(), h, m);
    auto next = timeService_->next_reset_utc(h, m, logic_->timezone_offset_minutes());
    auto now = timeService_->now_utc();
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(next - now);
    if (diff.count() < 0) diff = std::chrono::seconds(0);
    auto hh = diff.count() / 3600;
    auto mm = (diff.count() % 3600) / 60;
    auto ss = diff.count() % 60;
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%02lld:%02lld:%02lld", (long long)hh, (long long)mm, (long long)ss);
    labelCountdown_->set_text(buf);
    return true;
}


