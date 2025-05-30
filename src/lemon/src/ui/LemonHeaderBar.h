//
// Created by janto on 25-5-30.
//

#ifndef LEMONHEADERBAR_H
#define LEMONHEADERBAR_H

#include <gtkmm/headerbar.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>


#include "LemonMenu.h"

namespace GUI{

class LemonHeaderBar : public Gtk::HeaderBar {
public:
    LemonHeaderBar();
    ~LemonHeaderBar();
    //member
private:
    void init_tool_btn();
private:
    Gtk::Button m_btn_run;
    Gtk::Button m_btn_debug;
    Gtk::Button m_btn_build;
    Gtk::ComboBoxText m_comboBox;
    Gtk::HBox m_hbox;

    LemonMenu m_menu;

};
}


#endif //LEMONHEADERBAR_H
