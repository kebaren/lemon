//
// Created by janto on 25-5-27.
//

#ifndef LEMONWINDOW_H
#define LEMONWINDOW_H
#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/hvbox.h>
#include <gtkmm/menubar.h>
#include <gtkmm/window.h>

namespace GUI
{
    class LemonMenu : public Gtk::MenuBar
    {
    public:
        LemonMenu();
        ~LemonMenu();
    };

    class LemonHeaderBar:  public Gtk::HeaderBar
    {
    public:
        LemonHeaderBar();
        ~LemonHeaderBar();

    };


    class LemonWindow : public Gtk::Window
    {
    public:
        LemonWindow();
        ~LemonWindow();

    private:
        bool header_bar_sttings();

    private:
        Gtk::HeaderBar headerBar;
        Gtk::MenuBar menuBar;
    };
} // GUI

#endif //LEMONWINDOW_H
