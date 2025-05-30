//
// Created by janto on 25-5-27.
//

#include "LemonWindow.h"

#include <gtkmm/accelmap.h>
#include <gtkmm/combobox.h>

namespace GUI {
    LemonFileMenu::LemonFileMenu()
    {
        //
        set_label("File");
        m_file_menu = Gtk::Menu();
        m_new_menu = Gtk::Menu();

        //New Sub menu
        m_new = Gtk::MenuItem(_("New"));
        m_new_file = Gtk::MenuItem(_("New File"));
        m_new_project = Gtk::MenuItem(_("New Project"));
        m_new_menu.append(m_new_file);
        m_new_menu.append(m_new_project);


        //file and project menu
        m_open_file = Gtk::MenuItem(_("Open File"));
        m_open_project = Gtk::MenuItem(_("Open Project"));
        m_close_project = Gtk::MenuItem(_("Close Project"));
        m_recent_project = Gtk::MenuItem(_("Recent Project"));
        m_recent_project.set_submenu(m_recent_menu);
        m_recent_config = Gtk::MenuItem(_("Recent History"));
        m_recent_menu.append(m_separator4);
        m_recent_menu.append(m_recent_config);

        //save
        m_save_file = Gtk::MenuItem(_("Save"));
        m_save_all = Gtk::MenuItem(_("Save All"));
        m_save_as = Gtk::MenuItem(_("Save As..."));

        //close
        m_close = Gtk::MenuItem(_("Close"));
        m_close_all = Gtk::MenuItem(_("Close All"));

        //exit
        m_exit = Gtk::MenuItem(_("Exit"));

        //close

        set_submenu(m_file_menu);
        m_new.set_submenu(m_new_menu);
        m_file_menu.append(m_new);
        m_file_menu.append(m_open_file);
        m_file_menu.append(m_open_project);
        m_file_menu.append(m_close_project);
        m_file_menu.append(m_recent_project);
        m_file_menu.append(m_separator);
        m_file_menu.append(m_save_file);
        m_file_menu.append(m_save_all);
        m_file_menu.append(m_save_as);
        m_file_menu.append(m_separator2);
        m_file_menu.append(m_close);
        m_file_menu.append(m_close_all);
        m_file_menu.append(m_separator3);
        m_file_menu.append(m_exit);

    }

    LemonFileMenu::~LemonFileMenu(){}

    LemonEditRange::LemonEditRange()
    {
        pack1(m_ActivityBar,false,false);
        pack2(m_noteBook);
    }

    LemonEditRange::~LemonEditRange()
    {
    }

    LemonStatusBar::LemonStatusBar()
    {
        set_size_request(-1,20);
        m_Label = Gtk::Label("label");
        add(m_Label);
    }

    LemonStatusBar::~LemonStatusBar()
    {
    }

    LemonWindow::LemonWindow()
    {

        set_default_size(1000,800);
        set_title("Lemon");
        set_border_width(3);


        set_titlebar(m_headerbar);
        m_VBox.pack_start(m_EditRange);
        m_VBox.pack_end(m_StatusBar,false,false,2);
        m_VBox.set_hexpand(true);
        m_VBox.set_vexpand(true);

        add(m_VBox);

        show_all_children();

    }

    LemonWindow::~LemonWindow()
    {
    }
} // GUI