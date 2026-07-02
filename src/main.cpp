#include "webview/webview.h"
#include "rest_server.h"
#include "window_utils.h"
#include <memory>

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/,
                   LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
#else
int main() {
#endif
    try {
        // Create and start REST server
        auto rest_server = std::make_shared<RestServer>();
        rest_server->start();

        // Create webview
        webview::webview w(false, nullptr);
        w.set_title("WVUI - Webview UI");
        w.set_size(800, 600, WEBVIEW_HINT_NONE);

        // Get native window handle and remove title bar
        auto window_result = w.window();
        if (window_result.ok()) {
            WindowUtils::remove_title_bar(window_result.value());
        }

        w.navigate(rest_server->get_url());
        w.run();

    } catch (const std::exception& e) {
        // Simple error handling
    }

    return 0;
}
