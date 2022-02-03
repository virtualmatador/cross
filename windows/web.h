#ifndef WINDOWS_WEB_H
#define WINDOWS_WEB_H

#include <mutex>
#include <queue>

#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>

#include "framework.h"

class WebWidget
{
public:
    WebWidget();
    ~WebWidget();
    void evaluate(const char* function);
    void destroy();
    void resize();

private:
    HRESULT OnCreateEnvironmentCompleted(HRESULT errorCode,
        ICoreWebView2Environment* createdEnvironment);
    HRESULT OnCreateCoreWebView2ControllerCompleted(HRESULT errorCode,
        ICoreWebView2Controller* webView);
    HRESULT OnMessageReceived(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args);

private:
    HWND hwnd_;
    int width_;
    int height_;
    Microsoft::WRL::ComPtr<ICoreWebView2Environment> m_webViewEnvironment;
    Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controller;
    Microsoft::WRL::ComPtr<ICoreWebView2_3> m_webView;
};

#endif // WINDOWS_WEB_H
