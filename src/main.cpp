#include "webview/webview.h"

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/,
                   LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
#else
int main() {
#endif
    try {
        webview::webview w(false, nullptr);
        w.set_title("WVUI - Webview UI");
        w.set_size(800, 600, WEBVIEW_HINT_NONE);
        w.set_html(R"html(<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
            margin: 0;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        }
        .container {
            text-align: center;
            color: white;
            padding: 40px;
        }
        h1 {
            font-size: 3em;
            margin-bottom: 20px;
        }
        p {
            font-size: 1.2em;
            opacity: 0.9;
        }
        button {
            background: white;
            color: #667eea;
            border: none;
            padding: 15px 30px;
            font-size: 1.1em;
            border-radius: 8px;
            cursor: pointer;
            margin-top: 20px;
            transition: transform 0.2s;
        }
        button:hover {
            transform: scale(1.05);
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Hello WVUI!</h1>
        <p>Welcome to your webview-based UI framework.</p>
        <button onclick="sayHello()">Click me</button>
    </div>
    <script>
        function sayHello() {
            alert('Hello from JavaScript!');
        }
    </script>
</body>
</html>
)html");
        w.run();
        return 0;
    } catch (const webview::exception &e) {
        return 1;
    }
}
