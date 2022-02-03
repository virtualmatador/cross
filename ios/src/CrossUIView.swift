//
//  SwiftUIView.swift
//  cross
//
//  Created by Ali Asadpoor on 6/5/21.
//  Copyright © 2021 shaidin. All rights reserved.
//

import SwiftUI
import WebKit

struct WebViewWrapper : UIViewRepresentable
{
    func updateUIView(_ uiView: WebView, context: Context)
    {
        uiView.setNeedsLayout()
        let url = Bundle.main.url(
            forResource: "index",
            withExtension: "html",
            subdirectory: "html")!
        loadFileURL(url, allowingReadAccessTo: url)
    }

    func makeUIView(context: Context) -> WebView
    {
        let wv = WebView()
        return wv
    }
}

struct CrossUIView: View
{
    var body: some View
    {
        WebViewWrapper()
    }
}
