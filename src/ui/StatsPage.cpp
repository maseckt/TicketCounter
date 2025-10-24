#include "ui/StatsPage.hpp"
#include "AppLogic.hpp"

StatsPage::StatsPage() : Gtk::Box(Gtk::Orientation::VERTICAL, 12) {
    set_margin(12);
    auto label = Gtk::make_managed<Gtk::Label>("Статистика");
    label->set_margin(12);
    append(*label);

    auto frame = Gtk::make_managed<Gtk::Frame>();
    frame->set_margin(8);
    auto inner = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    inner->set_margin(16);
    frame->set_child(*inner);
    append(*frame);


    store_ = Gtk::ListStore::create(columns_);
    table_.set_model(store_);

    // создаем колонки
    table_.append_column("Дата", columns_.date);
    table_.append_column("Тикетов", columns_.count);
    table_.append_column("Цель  ", columns_.goal);
    table_.append_column("Прогресс (%)", columns_.progress);

    // делаем все колонки ресайзабельными
    for (int i = 0; i < table_.get_columns().size(); ++i)
        table_.get_column(i)->set_resizable(true);

    // включаем сортировку
    table_.get_column(0)->set_sort_column(columns_.date);
    table_.get_column(1)->set_sort_column(columns_.count);
    table_.get_column(2)->set_sort_column(columns_.goal);
    table_.get_column(3)->set_sort_column(columns_.progress);

    scroller_.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scroller_.set_child(table_);
    scroller_.set_hexpand();
    scroller_.set_vexpand();
    inner->append(scroller_);


    table_.set_grid_lines(Gtk::TreeView::GridLines::BOTH);
}

void StatsPage::bind(std::shared_ptr<AppLogic> logic) {
    logic_ = std::move(logic);
    refresh();
}

void StatsPage::refresh() {
    if (!logic_) return;
    store_->clear();
    auto hist = logic_->get_history_with_goals();
    for (const auto& t : hist) {
        auto row = *(store_->append());
        const auto& date = std::get<0>(t);
        int count = std::get<1>(t);
        int goal = std::get<2>(t);
        row[columns_.date] = date;
        row[columns_.count] = count;
        row[columns_.goal] = goal;
        int percent = goal > 0 ? (count * 100 / goal) : 0;
        row[columns_.progress] = percent;
    }
}


