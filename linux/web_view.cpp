#include <cstring>
#include <filesystem>
#include <string>
#include <sstream>

#include "web_view.h"

void web_view_script_message_received(WebKitUserContentManager* manager,
    WebKitJavascriptResult* js_result, gpointer user_data)
{
    auto value = webkit_javascript_result_get_js_value(js_result);
    std::istringstream is{ std::string(jsc_value_to_string(value)) };
    std::int32_t sender;
    std::string id, command, info;
    is >> sender;
    is >> id;
    is >> command;
    is.ignore(1);
    std::getline(is, info);
    // TODO run command
    webkit_javascript_result_unref(js_result);
}

void web_view_run_javascript_finished(
    GObject* sourceObject, GAsyncResult* res, gpointer userData)
{
}

void web_view_load_changed(WebKitWebView* web_view, WebKitLoadEvent load_event,
    gpointer user_data)
{
    if (load_event == WEBKIT_LOAD_STARTED)
    {
        webkit_web_view_run_javascript(web_view,
            "var handler = window.webkit.messageHandlers.Handler_;"
            "var platform_ = 'Linux';"
            "function callHandler(id, command, info)"
            "{"
            "    handler.postMessage(id + \" \" + command + \" \" + info);"
            "}"
            , nullptr, web_view_run_javascript_finished, nullptr);
    }
}

WebWidget::WebWidget()
    : std::reference_wrapper<WebKitWebView>{
        *(WebKitWebView*)webkit_web_view_new() }
{
    WebKitUserContentManager *manager =
        webkit_web_view_get_user_content_manager(&get());
    g_signal_connect(manager, "script-message-received::Handler_",
        G_CALLBACK(web_view_script_message_received), nullptr);
    webkit_user_content_manager_register_script_message_handler(
        manager, "Handler_");
    web_widget_ = Glib::wrap((GtkWidget*)&get());
    web_widget_->show();
    g_signal_connect(
        &get(), "load-changed", GCallback(web_view_load_changed), nullptr);
    webkit_web_view_load_uri(
        &get(), "https://" PROJECT_DOMAIN "/" PROJECT_LANDING_PAGE);
}

WebWidget::~WebWidget()
{
    Glib::unwrap(web_widget_);
}

void WebWidget::evaluate(const char* function)
{
    webkit_web_view_run_javascript(&get(), function, nullptr,
        web_view_run_javascript_finished, nullptr);
}
