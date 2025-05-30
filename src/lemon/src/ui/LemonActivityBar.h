//
// Created by janto on 25-5-30.
//

#ifndef LEMONACTIVITYBAR_H
#define LEMONACTIVITYBAR_H
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/stack.h>
#include <glibmm/i18n.h>
#include <gtkmm/label.h>

namespace GUI {

    class LemonActivityBar : public Gtk::HBox
    {
    public:
        LemonActivityBar();
        ~LemonActivityBar();
        void init_activity_button();
        void init_activity_stack();
        void on_explore_clicked();
    private:
        Gtk::Button m_Explore;
        Gtk::Button m_Git;
        Gtk::HBox m_HBox;
        Gtk::Stack m_Stack;
        Gtk::Label m_label;
        gboolean m_stack_visible=true;
    };
} // GUI

#endif //LEMONACTIVITYBAR_H
