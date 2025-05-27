//
// Created by janto on 25-5-26.
//

#ifndef LEMONGUI_H
#define LEMONGUI_H

#include <gtkmm.h>
#include <string_view>

#include "LemonWindow.h"

namespace GUI {

class LemonApp : public Gtk::Application{
public:
    ~LemonApp();
    LemonApp();

private:
    void on_startup() override;

private:
LemonWindow window;


};

} // GUI

#endif //LEMONGUI_H
