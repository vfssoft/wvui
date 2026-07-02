#include "window_utils.h"

#ifdef _WIN32
#include <windows.h>

void WindowUtils::remove_title_bar(void* native_window_handle) {
    if (!native_window_handle) {
        return;
    }

    HWND hwnd = static_cast<HWND>(native_window_handle);
    if (!IsWindow(hwnd)) {
        return;
    }

    // Get current window style
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    
    // Remove title bar related styles
    style &= ~(WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
    
    // Keep a thin frame for resizing if needed
    style |= WS_POPUP;
    
    // Apply the new style
    SetWindowLongPtrW(hwnd, GWL_STYLE, style);
    
    // Update the window to reflect changes
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

#elif defined(__APPLE__)
#include <objc/objc-runtime.h>

void WindowUtils::remove_title_bar(void* native_window_handle) {
    if (!native_window_handle) {
        return;
    }

    id window = static_cast<id>(native_window_handle);
    
    // Call setStyleMask: with only resizable style (no title bar)
    // NSWindowStyleMaskResizable = 8
    objc_msgSend(window, sel_registerName("setStyleMask:"), 8ULL);
}

#else

void WindowUtils::remove_title_bar(void* /*native_window_handle*/) {
    // Do nothing on unsupported platforms
}

#endif
