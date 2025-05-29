//
// Created by janto on 25-5-27.
//

#ifndef LEMONWINDOW_H
#define LEMONWINDOW_H
#include <memory>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/hvbox.h>
#include <gtkmm/menubar.h>
#include <gtkmm/window.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/separatormenuitem.h>

namespace GUI
{
    //editor menubar
    class LemonMenu : public Gtk::MenuBar
    {
    public:
        LemonMenu();
        ~LemonMenu();
        bool set_file_menu();
        bool set_edit_menu();

    private:
        Gtk::MenuItem* m_File;
        Gtk::MenuItem* m_Edit;
        Gtk::MenuItem* m_View;
        Gtk::MenuItem* m_Go;
        Gtk::MenuItem* m_Project;
        Gtk::MenuItem* m_Help;

        Gtk::Menu     * m_File_Menu;
        Gtk::SeparatorMenuItem *m_f_seperator;
        Gtk::SeparatorMenuItem *m_f_seperator2;
        Gtk::MenuItem* m_f_new_file;
        Gtk::MenuItem *m_f_new_project;
        Gtk::MenuItem* m_f_open_file;
        Gtk::MenuItem* m_f_open_project;
        Gtk::MenuItem* m_f_save_file;
        Gtk::MenuItem* m_f_save_all;
        Gtk::MenuItem* m_f_close_project;

        Gtk::Menu *m_Edit_Menu;
        Glib::RefPtr<Gtk::Builder> m_builder;


    };


    //eidtor headerbar
    class LemonHeaderBar : public Gtk::HeaderBar
    {
    public:
        LemonHeaderBar();
        ~LemonHeaderBar();

    private:
        void initComboBox();

    private:
        std::unique_ptr<LemonMenu> m_Menu;
        std::unique_ptr<Gtk::Button> m_Button_run;
        std::unique_ptr<Gtk::Button> m_Button_debug;
        std::unique_ptr<Gtk::Button> m_Button_build;
        std::unique_ptr<Gtk::ComboBoxText> m_ComBox;
        std::unique_ptr<Gtk::HBox> m_HBox;
    };


    //lemon main widnow
    class LemonWindow : public Gtk::Window
    {
    public:
        LemonWindow();
        ~LemonWindow();

    private:


    private:
        LemonHeaderBar* m_headerbar;
    };
} // GUI
#endif //LEMONWINDOW_H
