#include "ui/HotkeyBinder.hpp"
#include "AppLogic.hpp"

HotkeyBinder::HotkeyBinder() = default;

void HotkeyBinder::bind(std::shared_ptr<AppLogic> logic) {
    logic_ = logic;
}

void HotkeyBinder::set_entry(Gtk::Entry* entry) {
    entry_ = entry;
    entry_->set_editable(false);

    auto key_controller = Gtk::EventControllerKey::create();
    key_controller->signal_key_pressed().connect(sigc::mem_fun(*this, &HotkeyBinder::on_key_press), false);
    entry_->add_controller(key_controller);

    auto gesture = Gtk::GestureClick::create();
    gesture->signal_pressed().connect(sigc::mem_fun(*this, &HotkeyBinder::on_gesture_pressed));
    entry_->add_controller(gesture);
}

void HotkeyBinder::set_callback(std::function<void(unsigned int)> callback) {
    callback_ = callback;
}

void HotkeyBinder::set_get_current_keyval(std::function<unsigned int()> f) {
    get_current_keyval_ = f;
}

bool HotkeyBinder::on_key_press(guint keyval, guint keycode, Gdk::ModifierType state) {
    if (keyval == 65307) { // ESC
        if (get_current_keyval_) {
            unsigned int key = get_current_keyval_();
            entry_->set_text(keyval_to_name(key));
        } else {
            entry_->set_text("");
        }
        return true;
    }

    if (keyval == 65505 || keyval == 65507 || keyval == 65513) return true;

    if (keyval >= 0x10000000 && keyval <= 0x10000009) {
        // Allow mouse buttons
    }

    if (keyval == 65293) { // Enter
        if (logic_) logic_->save();
        return true;
    }

    std::string combo;
    if ((state & Gdk::ModifierType::SHIFT_MASK) != Gdk::ModifierType{}) combo += "Shift + ";
    if ((state & Gdk::ModifierType::CONTROL_MASK) != Gdk::ModifierType{}) combo += "Ctrl + ";
    if ((state & Gdk::ModifierType::ALT_MASK) != Gdk::ModifierType{}) combo += "Alt + ";
    combo += keyval_to_name(keyval);

    if (keyval >= 0x10000000 && keyval <= 0x10000009) {
        int button = keyval - 0x10000000;
        if (button == 8) combo = "Back Mouse";
        else if (button == 9) combo = "Forward Mouse";
        else combo = "Mouse " + std::to_string(button);
    }

    entry_->set_text(combo);
    if (callback_) callback_(keyval);
    if (logic_) logic_->save();
    return true;
}

void HotkeyBinder::on_gesture_pressed(int n_press, double x, double y) {
    if (n_press == 8) { // Back mouse button
        unsigned int keyval = 0x10000008;
        std::string combo = "Back Mouse";
        entry_->set_text(combo);
        if (callback_) callback_(keyval);
        if (logic_) logic_->save();
    } else if (n_press == 9) { // Forward mouse button
        unsigned int keyval = 0x10000009;
        std::string combo = "Forward Mouse";
        entry_->set_text(combo);
        if (callback_) callback_(keyval);
        if (logic_) logic_->save();
    }
}

std::string HotkeyBinder::keyval_to_name(guint keyval) const {
    if (keyval >= 0x10000001 && keyval <= 0x10000009) {
        int button = keyval - 0x10000000;
        if (button == 1) return "Left Mouse";
        else if (button == 2) return "Middle Mouse";
        else if (button == 3) return "Right Mouse";
        else if (button == 8) return "Back Mouse";
        else if (button == 9) return "Forward Mouse";
        else return "Mouse " + std::to_string(button);
    }

    switch (keyval) {
        case 65505: return "Shift";
        case 65507: return "Ctrl";
        case 65513: return "Alt";
        case 65288: return "Backspace";
        case 65289: return "Tab";
        case 65293: return "Enter";
        case 65307: return "Esc";
        case 32: return "Space";
        default: {
            if (keyval >= 97 && keyval <= 122) return std::string(1, keyval - 32);
            if (keyval >= 65 && keyval <= 90) return std::string(1, keyval);
            if (keyval >= 48 && keyval <= 57) return std::string(1, keyval);
            // Map Russian letters to English equivalents
            if (keyval >= 0x410 && keyval <= 0x42F) { // А-Я
                int offset = keyval - 0x410;
                const char* russian_to_english = "QWERTYUIOPASDFGHJKLZXCVBNM";
                if (offset < 26) return std::string(1, russian_to_english[offset]);
            }
            if (keyval >= 0x430 && keyval <= 0x44F) { // а-я
                int offset = keyval - 0x430;
                const char* russian_to_english_lower = "qwertyuiopasdfghjklzxcvbnm";
                if (offset < 26) return std::string(1, russian_to_english_lower[offset]);
            }
            if (keyval >= 1761 && keyval <= 1791) { // А-Я (alternative)
                int offset = keyval - 1761;
                const char* russian_to_english = "QWERTYUIOPASDFGHJKLZXCVBNM";
                if (offset < 26) return std::string(1, russian_to_english[offset]);
            }
            if (keyval >= 1792 && keyval <= 1822) { // а-я (alternative)
                int offset = keyval - 1792;
                const char* russian_to_english_lower = "qwertyuiopasdfghjklzxcvbnm";
                if (offset < 26) return std::string(1, russian_to_english_lower[offset]);
            }
            return "Key " + std::to_string(keyval);
        }
    }
}