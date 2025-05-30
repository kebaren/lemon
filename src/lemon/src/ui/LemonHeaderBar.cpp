//
// Created by janto on 25-5-30.
//

#include "LemonHeaderBar.h"
namespace GUI
{
    LemonHeaderBar::LemonHeaderBar()
    {
        set_show_close_button(true);
        set_title("lemon");
        set_decoration_layout("menu:minimize,maximize,close");
        pack_start(m_menu);
        pack_end(m_hbox);
        init_tool_btn();

    }

    LemonHeaderBar::~LemonHeaderBar()
    {
    }

    void LemonHeaderBar::init_tool_btn()
    {
        m_btn_build.set_label("build");
        m_btn_debug.set_label("debug");
        m_btn_run.set_label("run");

        m_hbox.pack_start(m_comboBox,false,false,2);
        m_hbox.pack_start(m_btn_build,false,false,2);
        m_hbox.pack_start(m_btn_run,false,false,2);
        m_hbox.pack_start(m_btn_debug,false,false,2);

    }
}
