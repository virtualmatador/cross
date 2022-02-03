#ifndef LINUX_WEB_H
#define LINUX_WEB_H

#include <gtkmm.h>
#include <webkit2/webkit2.h>

class WebWidget : public std::reference_wrapper<WebKitWebView>
{
public:
    WebWidget();
    ~WebWidget();
    void evaluate(const char* function);

public:
    Gtk::Widget* web_widget_;
};

#endif // LINUX_WEB_H
