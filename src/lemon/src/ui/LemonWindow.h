//
// Created by janto on 25-5-27.
//

#ifndef LEMONWINDOW_H
#define LEMONWINDOW_H
#include <gtkmm/headerbar.h>
#include <gtkmm/window.h>

namespace GUI {

class LemonWindow : public Gtk::Window{
public:
    LemonWindow();
    ~LemonWindow();
    private:
    std::unique_ptr<Gtk::HeaderBar> headerBar;
};

} // GUI

#endif //LEMONWINDOW_H
