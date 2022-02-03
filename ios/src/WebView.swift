//
//  WebView.swift
//  cross
//
//  Created by Null on 7/19/19.
//  Copyright © 2020 shaidin. All rights reserved.
//

import WebKit

class WebView: WKWebView, WKScriptMessageHandler, WKNavigationDelegate
{
    override init(frame: CGRect, configuration: WKWebViewConfiguration)
    {
        super.init(frame: frame, configuration: configuration)
        self.configuration.userContentController.add(self, name: "Handler_")
        navigationDelegate = self
        scrollView.bounces = false
    }

    required init?(coder: NSCoder)
    {
        fatalError("init(coder:) has not been implemented")
    }

    func userContentController(_ userContentController: WKUserContentController,
        didReceive message: WKScriptMessage)
    {
        do
        {
            let message_data = (message.body as! String).data(using: String.Encoding.utf8)!
            let message_dictionary = try JSONSerialization.jsonObject(with: message_data,
                options: JSONSerialization.ReadingOptions.init()) as! [String : Any]
            DispatchQueue.main.async
            {
                // TODO
            }
        }
        catch
        {
        }
    }

    func webView(_ webView: WKWebView, decidePolicyFor navigationAction: WKNavigationAction,
                 decisionHandler: @escaping (WKNavigationActionPolicy) -> Void)
    {
        if (navigationAction.navigationType != .linkActivated || navigationAction.sourceFrame.webView?.url?.path == navigationAction.request.url?.path)
        {
            decisionHandler(.allow)
        }
        else
        {
            decisionHandler(.cancel)
            let url = navigationAction.request.url
            if (url != nil)
            {
                UIApplication.shared.open(url!)
            }
        }
    }
}
