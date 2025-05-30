//
// Created by janto on 25-5-27.
//

#ifndef LEMONWINDOW_H
#define LEMONWINDOW_H
#include <memory>
#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/hvbox.h>
#include <gtkmm/menubar.h>
#include <gtkmm/window.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/separatormenuitem.h>
#include <glibmm/i18n.h>
#include <gtkmm/frame.h>
#include <gtkmm/notebook.h>
#include <gtkmm/paned.h>
#include <gtkmm/stack.h>
#include <gtkmm/statusbar.h>

#include "LemonHeaderBar.h"
#include "LemonActivityBar.h"


namespace GUI
{
    class LemonFileMenu : public Gtk::MenuItem
    {
    public:
        explicit  LemonFileMenu();
        ~LemonFileMenu();
    private:
        Gtk::Menu m_new_menu,m_file_menu,m_recent_menu;
        Gtk::SeparatorMenuItem m_separator,m_separator2,m_separator3,m_separator4;
        Gtk::MenuItem m_new,m_open_file,m_open_project,m_recent_project,m_close_project,
        m_save_file,m_save_all,m_save_as,m_close,m_close_all,m_exit;;
        Gtk::MenuItem m_new_file,m_new_project;
        Gtk::MenuItem m_recent_config;
    };



    //activity bar


    class LemonEditRange : public Gtk::Paned{
    public:
        LemonEditRange();
        ~LemonEditRange();
    private:
        LemonActivityBar m_ActivityBar;
        Gtk::Notebook m_noteBook;

    };

    class LemonStatusBar : public Gtk::Statusbar
    {
    public:
        LemonStatusBar();
        ~LemonStatusBar();
    private:
        Gtk::Label m_Label;
    };


    //lemon main widnow
    class LemonWindow : public Gtk::Window
    {
    public:
        LemonWindow();
        ~LemonWindow();

    private:


    private:
        LemonHeaderBar m_headerbar;
        Gtk::VBox m_VBox;
        LemonEditRange m_EditRange;
        LemonStatusBar m_StatusBar;
        Gtk::Frame m_Frame;
    };
} // GUI
#endif //LEMONWINDOW_H
