//
// Created by janto on 25-5-30.
//

#include "LemonActivityBar.h"

namespace GUI {
    LemonActivityBar::LemonActivityBar()
    {
        set_border_width(5);
        init_activity_button();
        init_activity_stack();
        pack_start(m_HBox,false,false,2);
        pack_end(m_Stack);
    }

    LemonActivityBar::~LemonActivityBar()
    {
    }

    void LemonActivityBar::init_activity_button()
    {
        m_HBox.set_size_request(32,-1);
        m_Explore = Gtk::Button(_("Exp"));
        m_Explore.set_relief(Gtk::RELIEF_NONE);
        m_Explore.set_size_request(32,32);
        m_Explore.set_hexpand(false);
        m_Explore.set_vexpand(false);
        m_HBox.pack_start(m_Explore,false,false,0);
        m_Explore.signal_clicked().connect(sigc::mem_fun(*this,&LemonActivityBar::on_explore_clicked));
    }

    void LemonActivityBar::init_activity_stack()
    {
        m_Stack.set_transition_type(Gtk::StackTransitionType::STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
        m_Stack.set_transition_duration(200);
        m_Stack.set_vexpand(true);
        m_Stack.set_hexpand(true);
        m_Stack.set_halign(Gtk::ALIGN_CENTER);
        m_Stack.set_valign(Gtk::ALIGN_CENTER);
        m_label.set_label("test");
        m_Stack.add(m_label);
    }

    void LemonActivityBar::on_explore_clicked()
    {
        if(m_stack_visible)
        {
            m_stack_visible=false;
            m_Stack.hide();
            // m_Stack.set_size_request(0,-1);
        }
        else
        {
            m_stack_visible=true;
            m_Stack.show();
            // m_Stack.set_size_request(60,-1);
        }


    }
} // GUI