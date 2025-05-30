#include <gtkmm.h>

#include "LemonWindow.h"

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.lemon.org");

    GUI::LemonWindow window;

    return app->run(window);
}