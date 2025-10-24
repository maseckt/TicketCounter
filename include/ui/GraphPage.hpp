#pragma once

#include <gtkmm.h>
#include <memory>

class AppLogic;

class GraphPage : public Gtk::Box {
public:
    GraphPage();
    void bind(std::shared_ptr<AppLogic> logic);
private:
    void refresh();
    void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
    void on_motion(double x, double y);
    std::string format_date(const std::string& iso) const;

    std::shared_ptr<AppLogic> logic_;
    Gtk::ScrolledWindow scrolled_window_;
    Gtk::DrawingArea drawing_area_;
    Gtk::Label tooltip_label_;
    int drawing_width_;
    int drawing_height_;
};


