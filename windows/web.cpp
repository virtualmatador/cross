//
//  web.cpp
//  cross
//
//  Created by Ali Asadpoor on 10/12/20.
//  Copyright © 2020 Shaidin. All rights reserved.
//

#include <codecvt>
#include <string>
#include <sstream>

#include <wrl/event.h>
#include <shlwapi.h>

#include "web.h"


LRESULT CALLBACK wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CHAR:
    {
        Window::window_->handle_key(wParam);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SHOWWINDOW:
        if (wParam)
        {
            cross::Create();
            if (!Window::window_->started_)
            {
                Window::window_->started_ = true;
                cross::Start();
            }
        }
        else
        {
            if (Window::window_->started_)
            {
                Window::window_->started_ = false;
                cross::Stop();
            }
            cross::Destroy();
        }
        break;
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            if (Window::window_->started_)
            {
                Window::window_->started_ = false;
                cross::Stop();
            }
        }
        else
        {
            if (!Window::window_->started_)
            {
                Window::window_->started_ = true;
                cross::Start();
            }
        }
        Window::window_->width_ = LOWORD(lParam);
        Window::window_->height_ = HIWORD(lParam);
        Window::window_->web_view_.resize();
        break;
    case Window::WM_RESTART_:
        Window::window_->on_need_restart();
        break;
    case Window::WM_MESSAGE_:
        Window::window_->on_post_message();
        break;
    case Window::WM_LOAD_:
        Window::window_->web_view_.pop_load();
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

WebWidget::WebWidget(HINSTANCE hInstance)
{
    WNDCLASSEXA cex;
    cex.cbSize = sizeof(WNDCLASSEX);
    cex.style = CS_HREDRAW | CS_VREDRAW;
    cex.lpfnWndProc = wnd_proc;
    cex.cbClsExtra = 0;
    cex.cbWndExtra = 0;
    cex.hInstance = hInstance;
    cex.hIcon = LoadIconA(hInstance, MAKEINTRESOURCEA(108));
    cex.hCursor = nullptr;
    cex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    cex.lpszMenuName = "";
    cex.lpszClassName = PROJECT_NAME;
    cex.hIconSm = LoadIconA(hInstance, MAKEINTRESOURCEA(108));
    RegisterClassExA(&cex);

    hwnd_ = CreateWindowA(PROJECT_NAME, PROJECT_NAME, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hwnd_, nCmdShow);
    UpdateWindow(hwnd_);

    char path[MAX_PATH];
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule != NULL)
    {
        GetModuleFileName(hModule, path, sizeof(path));
    }
    else
    {
        strcpy(path, ".");
    }
    assets_path_ = std::filesystem::path(path).parent_path().parent_path() / "share";

}

WebWidget::~WebWidget()
{
    destroy();
}

void WebWidget::evaluate(const char* function)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wfunction = converter.from_bytes(function);
    m_webView->ExecuteScript(wfunction.c_str(), nullptr);
}

void WebWidget::destroy()
{
    if (m_controller)
    {
        m_controller->Close();
        m_controller = nullptr;
        m_webView = nullptr;
    }
    m_webViewEnvironment = nullptr;
}

void WebWidget::resize()
{
    if (m_controller)
    {
        m_controller->put_Bounds(RECT{ 0, 0, Window::window_->width_, Window::window_->height_ });
    }
}

void WebWidget::on_load(const std::int32_t sender, const std::int32_t view_info, const char* html)
{
    destroy();
    Window::window_->load_view(sender, view_info);
    html_ = html;
    auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
    auto hr = CreateCoreWebView2EnvironmentWithOptions(nullptr, Window::window_->config_path_.wstring().c_str(), options.Get(),
        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this, &WebWidget::OnCreateEnvironmentCompleted).Get());
    if (hr != S_OK)
    {
        throw std::runtime_error("CreateCoreWebView2EnvironmentWithOptions");
    }
}

HRESULT WebWidget::OnCreateEnvironmentCompleted(HRESULT errorCode, ICoreWebView2Environment* createdEnvironment)
{
    m_webViewEnvironment = createdEnvironment;

    if (m_webViewEnvironment->CreateCoreWebView2Controller(
        Window::window_->hwnd_,
        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
            this, &WebWidget::OnCreateCoreWebView2ControllerCompleted)
        .Get()) != S_OK)
    {
        throw std::runtime_error("");
    }
    return S_OK;
}

HRESULT WebWidget::OnCreateCoreWebView2ControllerCompleted(HRESULT errorCode, ICoreWebView2Controller* webView)
{
    auto hr = errorCode;
    if (hr == S_OK)
    {
        m_controller = webView;
        Microsoft::WRL::ComPtr<ICoreWebView2> coreWebView2;
        if (m_controller->get_CoreWebView2(&coreWebView2) != S_OK || coreWebView2.As(&m_webView) != S_OK)
        {
            throw std::runtime_error("");
        }
        resize();
        m_webView->SetVirtualHostNameToFolderMapping(L"asset.cross.com", (Window::window_->assets_path_ / L"assets").wstring().c_str(), COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW);
        m_webView->AddWebResourceRequestedFilter(L"cross://*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
        m_webView->add_WebResourceRequested(Microsoft::WRL::Callback<ICoreWebView2WebResourceRequestedEventHandler>(this, &WebWidget::OnResourceRequested).Get(), nullptr);
        m_webView->add_WebMessageReceived(Microsoft::WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(this, &WebWidget::OnMessageReceived).Get(), nullptr);
        m_webView->add_NavigationCompleted(Microsoft::WRL::Callback<ICoreWebView2NavigationCompletedEventHandler>(this, &WebWidget::OnNavigationCompleted).Get(), nullptr);
        std::wstring path = L"file://" + (Window::window_->assets_path_ / "assets" / (std::string(html_) + ".htm")).wstring();
        auto hr = m_webView->Navigate(path.c_str());
    }
    return hr;
}

HRESULT WebWidget::OnMessageReceived(ICoreWebView2* core_sender, ICoreWebView2WebMessageReceivedEventArgs* args)
{
    wchar_t* buffer = nullptr;
    if (args->TryGetWebMessageAsString(&buffer) != S_OK)
    {
        throw std::runtime_error("");
    }
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string msg = converter.to_bytes(buffer);
    std::istringstream is{ msg };
    std::string id, command, info;
    is >> id;
    is >> command;
    is.ignore(1);
    std::getline(is, info);
    // TODO
    return S_OK;
}
