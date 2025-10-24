#include "ui/GraphPage.hpp"
#include "AppLogic.hpp"
#include <cairomm/context.h>
#include <algorithm>
#include <cmath>

GraphPage::GraphPage() : Gtk::Box(Gtk::Orientation::VERTICAL, 12), drawing_width_(600), drawing_height_(400) {
    set_margin(12);
    auto label = Gtk::make_managed<Gtk::Label>("Граф (работает криво)");
    label->set_margin(12);
    append(*label);

    auto frame = Gtk::make_managed<Gtk::Frame>();
    frame->set_margin(8);
    auto inner = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
    inner->set_margin(8);
    frame->set_child(*inner);
    append(*frame);

    drawing_area_.set_content_height(400);
    drawing_area_.set_draw_func(sigc::mem_fun(*this, &GraphPage::on_draw));

    auto controller = Gtk::EventControllerMotion::create();
    controller->signal_motion().connect(sigc::mem_fun(*this, &GraphPage::on_motion));
    drawing_area_.add_controller(controller);

    scrolled_window_.set_child(drawing_area_);
    scrolled_window_.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::NEVER);
    inner->append(scrolled_window_);

    tooltip_label_.set_text("Наведите мышь на точку");
    tooltip_label_.set_margin(6);
    inner->append(tooltip_label_);
}

std::string GraphPage::format_date(const std::string& iso) const {
    if (iso.size() != 10) return iso;
    return iso.substr(8, 2) + "-" + iso.substr(5, 2) + "-" + iso.substr(0, 4);
}

void GraphPage::bind(std::shared_ptr<AppLogic> logic) {
    logic_ = logic;
    refresh();
}

void GraphPage::refresh() {
    if (logic_) {
        drawing_area_.queue_draw();
    }
}

void GraphPage::on_motion(double x, double y) {
    if (!logic_) return;

    auto history = logic_->get_history_with_goals();
    if (history.empty()) {
        tooltip_label_.set_text("Нет данных");
        return;
    }

    std::sort(history.begin(), history.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) < std::get<0>(b);
    });

    int num_days = history.size();
    int step_per_day = 50;
    int content_width = std::max(600, num_days * step_per_day);
    double height = drawing_height_;
    int max_count = 0;
    int max_goal = 0;
    for (const auto& entry : history) {
        max_count = std::max(max_count, std::get<1>(entry));
        max_goal = std::max(max_goal, std::get<2>(entry));
    }
    int overall_max = std::max(max_count, max_goal);
    if (overall_max == 0) overall_max = 1;
    double y_scale = static_cast<double>(height - 80) / overall_max;

    auto hadj = scrolled_window_.get_hadjustment();
    double scroll_x = hadj ? hadj->get_value() : 0.0;
    double rel_x = x + scroll_x;

    double min_dist = 1e9;
    size_t closest_idx = static_cast<size_t>(-1);
    for (size_t i = 0; i < history.size(); ++i) {
        double px = 80 + step_per_day * i;
        double py = height - 50 - y_scale * std::get<1>(history[i]);
        double dist = std::hypot(rel_x - px, y - py);
        if (dist < min_dist) {
            min_dist = dist;
            closest_idx = i;
        }
    }

    if (closest_idx != static_cast<size_t>(-1) && min_dist < 15.0) {
        std::string date = std::get<0>(history[closest_idx]);
        int count = std::get<1>(history[closest_idx]);
        int goal = std::get<2>(history[closest_idx]);
        tooltip_label_.set_text("Дата: " + format_date(date) + ", Тикетов: " + std::to_string(count) + ", Цель: " + std::to_string(goal));
    } else {
        tooltip_label_.set_text("Наведите мышь на точку");
    }
}



void GraphPage::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
    drawing_width_ = width;
    drawing_height_ = height;
    if (!logic_) return;

    cr->set_source_rgb(0.95, 0.95, 0.95);
    cr->paint();

    auto history = logic_->get_history_with_goals();
    if (history.empty()) {
        cr->set_source_rgb(0.0, 0.0, 0.0);
        cr->move_to(50, static_cast<double>(height) / 2);
        cr->show_text("Нет данных для графика");
        return;
    }

    std::sort(history.begin(), history.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) < std::get<0>(b);
    });

    int num_days = history.size();
    int step_per_day = 50;
    drawing_width_ = std::max(600, num_days * step_per_day);
    drawing_area_.set_content_width(drawing_width_);

    int max_count = 0;
    int max_goal = 0;
    for (const auto& entry : history) {
        max_count = std::max(max_count, std::get<1>(entry));
        max_goal = std::max(max_goal, std::get<2>(entry));
    }
    int overall_max = std::max(max_count, max_goal);
    if (overall_max == 0) overall_max = 1;
    double y_scale = static_cast<double>(height - 80) / overall_max;

    cr->set_source_rgb(0.7, 0.7, 0.7);
    cr->set_line_width(0.5);
    for (int i = 1; i < overall_max; i += std::max(1, overall_max / 10)) {
        double y = height - 50 - y_scale * i;
        cr->move_to(80, y);
        cr->line_to(drawing_area_.get_content_width() - 10, y);
        cr->stroke();
    }

    cr->set_source_rgba(0.0, 0.0, 0.0, 0.8);
    cr->set_line_width(1.5);
    cr->rectangle(80, 30, drawing_area_.get_content_width() - 90, height - 80);
    cr->stroke();

    int step = 1;
    if (overall_max >= 500) step = 100;
    else if (overall_max >= 100) step = 50;
    else if (overall_max >= 50) step = 10;
    else if (overall_max >= 10) step = 5;

    cr->set_font_size(10);
    cr->set_source_rgb(0.0, 0.0, 0.0);
    for (int i = 0; i <= overall_max; i += step) {
        double y = height - 50 - y_scale * i;
        cr->move_to(50, y + 4);
        cr->show_text(std::to_string(i));
    }

    cr->set_font_size(9);
    for (size_t i = 0; i < history.size(); ++i) {
        double x = 80 + step_per_day * i;
        cr->move_to(x - 10, height - 25);
        std::string date = std::get<0>(history[i]);
        std::string short_date = format_date(date);
        cr->show_text(short_date);
    }


    cr->set_source_rgb(0.0, 0.5, 1.0);
    cr->set_line_width(2.0);
    for (size_t i = 0; i < history.size(); ++i) {
        double x = 80 + step_per_day * i;
        double y = height - 50 - y_scale * std::get<1>(history[i]);
        if (i == 0) {
            cr->move_to(x, y);
        } else {
            cr->line_to(x, y);
        }
    }
    cr->stroke();

    cr->set_source_rgb(0.0, 0.5, 1.0);
    for (size_t i = 0; i < history.size(); ++i) {
        double x = 80 + step_per_day * i;
        double y = height - 50 - y_scale * std::get<1>(history[i]);
        cr->arc(x, y, 5, 0, 2 * M_PI);
        cr->fill();
    }
}


