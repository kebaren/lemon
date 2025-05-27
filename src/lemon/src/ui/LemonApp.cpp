#include "LemonApp.h"

GUI::LemonApp::~LemonApp()
{
}

GUI::LemonApp::LemonApp()
{

}

void GUI::LemonApp::on_startup()
{
    Application::on_startup();
    add_window(window);
    window.present();
}
