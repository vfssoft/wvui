#ifndef WINDOW_UTILS_H
#define WINDOW_UTILS_H

class WindowUtils {
public:
    /**
     * Removes the default system title bar from a window
     * @param native_window_handle The native window handle (HWND on Windows, NSWindow* on macOS)
     */
    static void remove_title_bar(void* native_window_handle);
};

#endif // WINDOW_UTILS_H
