package com.shaidin.cross;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.AssetManager;
import android.content.res.AssetFileDescriptor;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;
import java.util.StringTokenizer;

public class MainActivity extends Activity
{
    WebView web_view_;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        web_view_ = findViewById(R.id.webView);
        web_view_.getSettings().setJavaScriptEnabled(true);
        web_view_.getSettings().setAllowUniversalAccessFromFileURLs(true);
        web_view_.addJavascriptInterface(this, "Handler_");
        web_view_.loadUrl("file:///android_asset/index.htm");
    }

    @Override public void onBackPressed()
    {
    }

    public void CallFunction(String function)
    {
        web_view_.evaluateJavascript(function, null);
    }
}
