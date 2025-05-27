//
// Created by janto on 25-5-27.
//

#include "LemonWindow.h"

#include <gtkmm/combobox.h>

namespace GUI {
    LemonWindow::LemonWindow()
    {
        set_title("Lemon");
        set_default_size(800,600);
        set_decorated(false);

        set_titlebar(headerBar);

        header_bar_sttings();

    }

    LemonWindow::~LemonWindow()
    {
    }

    bool LemonWindow::header_bar_sttings()
    {
        //run build debug config button
        auto hBox = Gtk::HBox();
        auto btn_run = Gtk::Button("Run");
        auto btn_build = Gtk::Button("Build");
        auto btn_debug = Gtk::Button("Debug");
        auto commobox = Gtk::ComboBox("Config");

        hBox.add(btn_run);
        hBox.add(btn_build);
        hBox.add(btn_debug);
        hBox.add(commobox);


        headerBar.set_title("Lemon");
        headerBar.set_show_close_button(true);
        headerBar.set_has_subtitle(true);
        headerBar.pack_start(menuBar);
        headerBar.pack_end(hBox);

        return true;
    }
} // GUI