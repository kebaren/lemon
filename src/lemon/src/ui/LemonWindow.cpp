//
// Created by janto on 25-5-27.
//

#include "LemonWindow.h"

#include <gtkmm/accelmap.h>
#include <gtkmm/combobox.h>

namespace GUI {
    LemonMenu::LemonMenu()
    {
        set_file_menu();
        m_Edit = new Gtk::MenuItem("Edit");
        m_View = new Gtk::MenuItem("View");
        m_Go = new Gtk::MenuItem("Go");
        m_Project = new Gtk::MenuItem("Terminal");
        m_Help = new Gtk::MenuItem("Help");

        append(*m_Edit);
        append(*m_View);
        append(*m_Go);
        append(*m_Project);
        append(*m_Help);


    }

    LemonMenu::~LemonMenu()
    {
        delete m_File;
        delete m_Edit;
        delete m_View;
        delete m_Go;
        delete m_Project;
        delete m_Help;
        delete m_File_Menu;
        delete m_f_new_file;
        delete m_f_open_file;
        delete m_f_open_project;

        m_File = nullptr;
        m_Edit = nullptr;
        m_View = nullptr;
        m_Go = nullptr;
        m_Project = nullptr;
        m_Help = nullptr;
        m_File_Menu = nullptr;
        m_f_new_file = nullptr;
        m_f_open_file = nullptr;
        m_f_open_project = nullptr;

    }

    bool LemonMenu::set_file_menu()
    {

        //setting file menu
        m_File = new Gtk::MenuItem("File");
        this->append(*m_File);
        m_File->set_margin_right(5);


        m_File_Menu = new Gtk::Menu();
        m_f_seperator = new Gtk::SeparatorMenuItem();
        m_f_seperator2 = new Gtk::SeparatorMenuItem();
        m_f_new_file = new Gtk::MenuItem("New File");
        m_f_new_project = new Gtk::MenuItem("New Project");
        m_f_open_file = new Gtk::MenuItem("Open File");
        m_f_open_project = new Gtk::MenuItem("Open Project");
        m_f_save_file = new Gtk::MenuItem("Save File");
        m_f_save_all = new Gtk::MenuItem("Save All");
        m_f_close_project = new Gtk::MenuItem("Close Project");

        m_File_Menu->append(*m_f_new_file);
        m_File_Menu->append(*m_f_open_file);
        m_File_Menu->append(*m_f_new_project);
        m_File_Menu->append(*m_f_open_project);
        m_File_Menu->append(*m_f_seperator);
        m_File_Menu->append(*m_f_save_file);
        m_File_Menu->append(*m_f_save_all);
        m_File_Menu->append(*m_f_close_project);
        m_File_Menu->append(*m_f_seperator2);
        m_File->set_submenu(*m_File_Menu);

        return true;

    }

    bool LemonMenu::set_edit_menu()
    {
        m_builder = Gtk::Builder::create_from_file("./glade/menu_edit.glade");
        m_builder->get_widget("menu_edit",m_Edit);
        this->append(*m_Edit);

        return true;

    }

    LemonHeaderBar::LemonHeaderBar()
    {
        m_HBox = std::make_unique<Gtk::HBox>();
        m_Button_run = std::make_unique<Gtk::Button>("Run");
        m_Button_debug = std::make_unique<Gtk::Button>("Debug");
        m_Button_build = std::make_unique<Gtk::Button>("Build");
        m_ComBox = std::make_unique<Gtk::ComboBoxText>();
        m_Menu = std::make_unique<LemonMenu>();

        m_HBox->pack_start(*m_ComBox);
        m_HBox->pack_start(*m_Button_build);
        m_HBox->pack_start(*m_Button_run);
        m_HBox->pack_start(*m_Button_debug);

        set_title("Lemon");
        set_show_close_button(true);
        set_decoration_layout("menu:minimize,maximize,close");
        pack_start(*m_Menu);
        pack_end(*m_HBox);

    }

    LemonHeaderBar::~LemonHeaderBar()
    {
    }

    void LemonHeaderBar::initComboBox()
    {
        m_ComBox->append("Edit","Config Project Build");
        m_ComBox->set_active(0);
    }

    LemonWindow::LemonWindow()
    {

        set_default_size(1000,800);
        set_title("Lemon");
        set_border_width(3);

        m_headerbar = Gtk::manage(new LemonHeaderBar());

        set_titlebar(*m_headerbar);

        show_all_children();

    }

    LemonWindow::~LemonWindow()
    {
    }
} // GUI