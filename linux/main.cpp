#include <cstring>
#include <iostream>

#include <gtkmm.h>

#include "web_view.h"

int main(int argc, const char* argv[])
{
    if (argc > 1)
    {
        if (argc == 2 && std::strcmp("--version", argv[1]) == 0)
        {
            std::cout << PROJECT_NAME << " " << PROJECT_VERSION << std::endl;
            return 0;
        }
        else
        {
            std::cerr << "Unknown option" << std::endl;
            return -1;
        }
    }
    else
    {
        auto app = Gtk::Application::create(APPLICATION_ID);
        Gtk::Window window;
        WebWidget web_view_;
        window.add(*web_view_.web_widget_);
        web_view_.web_widget_->show();
        window.maximize();
        window.set_title(PROJECT_NAME);
        return app->run(window);
    }
}
