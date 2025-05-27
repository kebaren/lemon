#include "LemonApp.h"

int main(int argc, char* argv[]) {
   auto app = std::make_unique<GUI::LemonApp>();

    return app->run(argc, argv);
}