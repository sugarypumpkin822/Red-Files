#include "../include/rf_window.h"
#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>

namespace RFWindow {

using float32 = float;
using uint32 = uint32_t;
using uint8 = uint8_t;

// Windows-specific window implementation with no external dependencies
class WindowsWindow : public Window {
public:
    WindowsWindow() : window_(nullptr), deviceContext_(nullptr), glContext_(nullptr),
                      shouldClose_(false), focused_(false), visible_(false), 
                      minimized_(false), maximized_(false), borderless_(false),
                      resizable_(true), cursorVisible_(true) {
    }
    
    ~WindowsWindow() {
        destroy();
    }
    
    bool create(const WindowConfig& config) override {
        config_ = config;
        
        // Register window class
        if (!registerWindowClass()) {
            std::cerr << "Failed to register window class" << std::endl;
            return false;
        }
        
        // Create window
        if (!createWindow()) {
            std::cerr << "Failed to create window" << std::endl;
            return false;
        }
        
        // Create device context
        if (!createDeviceContext()) {
            std::cerr << "Failed to create device context" << std::endl;
            return false;
        }
        
        // Create OpenGL context
        if (!createGLContext()) {
            std::cerr << "Failed to create OpenGL context" << std::endl;
            return false;
        }
        
        // Apply initial configuration
        applyConfiguration();
        
        return true;
    }
    
    void destroy() override {
        if (glContext_) {
            wglDeleteContext(glContext_);
            glContext_ = nullptr;
        }
        
        if (deviceContext_) {
            ReleaseDC(window_, deviceContext_);
            deviceContext_ = nullptr;
        }
        
        if (window_) {
            DestroyWindow(window_);
            window_ = nullptr;
        }
        
        UnregisterClass(windowClassName_, GetModuleHandle(nullptr));
    }
    
    void show() override {
        if (window_) {
            ShowWindow(window_, SW_SHOW);
            UpdateWindow(window_);
            visible_ = true;
        }
    }
    
    void hide() override {
        if (window_) {
            ShowWindow(window_, SW_HIDE);
            visible_ = false;
        }
    }
    
    bool isVisible() const override {
        return visible_;
    }
    
    void focus() override {
        if (window_) {
            SetForegroundWindow(window_);
            SetFocus(window_);
            focused_ = true;
        }
    }
    
    bool isFocused() const override {
        return focused_;
    }
    
    void minimize() override {
        if (window_) {
            ShowWindow(window_, SW_MINIMIZE);
            minimized_ = true;
        }
    }
    
    void restore() override {
        if (window_) {
            ShowWindow(window_, SW_RESTORE);
            minimized_ = false;
            maximized_ = false;
        }
    }
    
    void maximize() override {
        if (window_) {
            ShowWindow(window_, SW_MAXIMIZE);
            maximized_ = true;
        }
    }
    
    bool isMinimized() const override {
        return window_ ? IsIconic(window_) : false;
    }
    
    bool isMaximized() const override {
        if (!window_) return false;
        
        WINDOWPLACEMENT placement;
        if (GetWindowPlacement(window_, &placement)) {
            return placement.showCmd == SW_MAXIMIZE;
        }
        return false;
    }
    
    void setResizable(bool resizable) override {
        resizable_ = resizable;
        if (window_) {
            DWORD style = GetWindowLong(window_, GWL_STYLE);
            if (resizable) {
                style |= WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
            } else {
                style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
            }
            SetWindowLong(window_, GWL_STYLE, style);
            SetWindowPos(window_, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
    
    bool isResizable() const override {
        return resizable_;
    }
    
    void setBorderless(bool borderless) override {
        borderless_ = borderless;
        if (window_) {
            DWORD style = GetWindowLong(window_, GWL_STYLE);
            if (borderless) {
                style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
            } else {
                style |= WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
            }
            SetWindowLong(window_, GWL_STYLE, style);
            SetWindowPos(window_, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
    
    bool isBorderless() const override {
        return borderless_;
    }
    
    void setPosition(int32 x, int32 y) override {
        if (window_) {
            SetWindowPos(window_, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            config_.x = x;
            config_.y = y;
        }
    }
    
    void getPosition(int32& x, int32& y) const override {
        if (window_) {
            RECT rect;
            GetWindowRect(window_, &rect);
            x = rect.left;
            y = rect.top;
        } else {
            x = config_.x;
            y = config_.y;
        }
    }
    
    void setSize(uint32 width, uint32 height) override {
        if (window_) {
            RECT rect;
            GetWindowRect(window_, &rect);
            SetWindowPos(window_, 0, rect.left, rect.top, rect.left + width, rect.top + height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
            config_.width = width;
            config_.height = height;
        }
    }
    
    void getSize(uint32& width, uint32& height) const override {
        if (window_) {
            RECT rect;
            GetWindowRect(window_, &rect);
            width = static_cast<uint32>(rect.right - rect.left);
            height = static_cast<uint32>(rect.bottom - rect.top);
        } else {
            width = config_.width;
            height = config_.height;
        }
    }
    
    void setBounds(int32 x, int32 y, uint32 width, uint32 height) override {
        setPosition(x, y);
        setSize(width, height);
    }
    
    void getBounds(int32& x, int32& y, uint32& width, uint32& height) const override {
        getPosition(x, y);
        getSize(width, height);
    }
    
    void setTitle(const std::string& title) override {
        if (window_) {
            SetWindowTextA(window_, title.c_str());
            config_.title = title;
        }
    }
    
    const std::string& getTitle() const override {
        return config_.title;
    }
    
    void setOpacity(float32 opacity) override {
        if (window_) {
            SetWindowLong(window_, GWL_EXSTYLE, GetWindowLong(window_, GWL_EXSTYLE) | WS_EX_LAYERED);
            SetLayeredWindowAttributes(window_, 0, static_cast<uint8>(opacity * 255.0f), LWA_ALPHA);
        }
    }
    
    float32 getOpacity() const override {
        if (window_) {
            uint8 alpha;
            if (GetLayeredWindowAttributes(window_, nullptr, &alpha, nullptr)) {
                return static_cast<float32>(alpha) / 255.0f;
            }
        }
        return 1.0f;
    }
    
    void setCursorVisible(bool visible) override {
        if (visible != cursorVisible_) {
            ShowCursor(visible);
            cursorVisible_ = visible;
        }
    }
    
    bool isCursorVisible() const override {
        return cursorVisible_;
    }
    
    void setCursorPosition(int32 x, int32 y) override {
        SetCursorPos(x, y);
    }
    
    void getCursorPosition(int32& x, int32& y) const override {
        POINT pos;
        if (GetCursorPos(&pos)) {
            x = pos.x;
            y = pos.y;
        }
    }
    
    void setCursor(CursorType cursorType) override {
        HCURSOR cursor = createCursor(cursorType);
        if (cursor) {
            SetCursor(cursor);
        }
    }
    
    void setIcon(const std::vector<uint8>& iconData, uint32 width, uint32 height) override {
        if (window_ && !iconData.empty()) {
            // Create icon from data
            HICON hIcon = CreateIconFromResource(iconData.data(), iconData.size(), TRUE, 0x00030000);
            if (hIcon) {
                SendMessage(window_, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon));
                SendMessage(window_, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
            }
        }
    }
    
    void setParent(void* parentHandle) override {
        if (window_ && parentHandle) {
            HWND parentWindow = static_cast<HWND>(parentHandle);
            SetParent(window_, parentWindow);
        }
    }
    
    void* getParent() const override {
        return window_ ? GetParent(window_) : nullptr;
    }
    
    void* getNativeHandle() const override {
        return window_;
    }
    
    void* getNativeDisplay() const override {
        return nullptr; // Windows doesn't have a display handle like X11
    }
    
    void* getGLContext() const override {
        return glContext_;
    }
    
    void swapBuffers() override {
        if (glContext_ && deviceContext_) {
            SwapBuffers(deviceContext_);
        }
    }
    
    bool shouldClose() const override {
        return shouldClose_;
    }
    
    void setShouldClose(bool shouldClose) override {
        shouldClose_ = shouldClose;
    }
    
    void pollEvents() override {
        MSG msg;
        while (PeekMessage(&msg, window_, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    void setEventCallback(std::function<void(const WindowEvent&)> callback) override {
        eventCallback_ = callback;
    }
    
    WindowConfig getConfig() const override {
        return config_;
    }
    
    void setConfig(const WindowConfig& config) override {
        config_ = config;
        applyConfiguration();
    }
    
    std::vector<MonitorInfo> getMonitors() const override {
        std::vector<MonitorInfo> monitors;
        
        DISPLAY_DEVICE displayDevice;
        for (DWORD deviceIndex = 0; EnumDisplayDevices(nullptr, deviceIndex, &displayDevice, 0); ++deviceIndex) {
            DEVMODE devMode;
            if (EnumDisplaySettings(displayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &devMode)) {
                MonitorInfo monitor = {};
                monitor.id = deviceIndex;
                monitor.x = devMode.dmPosition.x;
                monitor.y = devMode.dmPosition.y;
                monitor.width = devMode.dmPelsWidth;
                monitor.height = devMode.dmPelsHeight;
                monitor.isPrimary = (displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0;
                monitor.name = displayDevice.DeviceName;
                
                monitors.push_back(monitor);
            }
        }
        
        return monitors;
    }
    
    MonitorInfo getPrimaryMonitor() const override {
        std::vector<MonitorInfo> monitors = getMonitors();
        for (const auto& monitor : monitors) {
            if (monitor.isPrimary) {
                return monitor;
            }
        }
        return monitors.empty() ? MonitorInfo() : monitors[0];
    }
    
    void centerOnMonitor(uint32 monitorId) override {
        std::vector<MonitorInfo> monitors = getMonitors();
        if (monitorId < monitors.size()) {
            const MonitorInfo& monitor = monitors[monitorId];
            int32 x = monitor.x + (monitor.width - static_cast<int32>(config_.width)) / 2;
            int32 y = monitor.y + (monitor.height - static_cast<int32>(config_.height)) / 2;
            setPosition(x, y);
        }
    }
    
    void centerOnPrimaryMonitor() override {
        MonitorInfo primary = getPrimaryMonitor();
        int32 x = primary.x + (primary.width - static_cast<int32>(config_.width)) / 2;
        int32 y = primary.y + (primary.height - static_cast<int32>(config_.height)) / 2;
        setPosition(x, y);
    }
    
private:
    static const char* windowClassName_ = "RedFilesWindow";
    
    WindowConfig config_;
    
    HWND window_;
    HDC deviceContext_;
    HGLRC glContext_;
    
    bool shouldClose_;
    bool focused_;
    bool visible_;
    bool minimized_;
    bool maximized_;
    bool borderless_;
    bool resizable_;
    bool cursorVisible_;
    
    std::function<void(const WindowEvent&)> eventCallback_;
    
    bool registerWindowClass() {
        WNDCLASSEXA wc = {};
        wc.cbSize = sizeof(WNDCLASSEXA);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = windowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = windowClassName_;
        
        return RegisterClassExA(&wc) != 0;
    }
    
    bool createWindow() {
        DWORD style = WS_OVERLAPPEDWINDOW;
        
        if (config_.borderless) {
            style = WS_POPUP | WS_VISIBLE;
        }
        
        window_ = CreateWindowExA(
            0,                              // Extended style
            windowClassName_,                  // Class name
            config_.title.c_str(),             // Window title
            style,                           // Window style
            config_.x,                        // X position
            config_.y,                        // Y position
            config_.width,                    // Width
            config_.height,                   // Height
            nullptr,                           // Parent window
            nullptr,                           // Menu
            GetModuleHandle(nullptr),          // Instance
            nullptr                            // Additional data
        );
        
        return window_ != nullptr;
    }
    
    bool createDeviceContext() {
        deviceContext_ = GetDC(window_);
        return deviceContext_ != nullptr;
    }
    
    bool createGLContext() {
        if (!deviceContext_) {
            return false;
        }
        
        // Load OpenGL library
        HMODULE openglLib = LoadLibraryA("opengl32.dll");
        if (!openglLib) {
            std::cerr << "Failed to load opengl32.dll" << std::endl;
            return false;
        }
        
        // Load WGL functions
        typedef int (WINAPI *ChoosePixelFormat_t)(HDC, const void*);
        typedef BOOL (WINAPI *SetPixelFormat_t)(HDC, int, const void*);
        typedef void* (WINAPI *wglCreateContext_t)(HDC);
        typedef BOOL (WINAPI *wglDeleteContext_t)(void*);
        
        ChoosePixelFormat_t ChoosePixelFormat = reinterpret_cast<ChoosePixelFormat_t>(GetProcAddress(openglLib, "ChoosePixelFormat"));
        SetPixelFormat_t SetPixelFormat = reinterpret_cast<SetPixelFormat_t>(GetProcAddress(openglLib, "SetPixelFormat"));
        wglCreateContext_t wglCreateContext = reinterpret_cast<wglCreateContext_t>(GetProcAddress(openglLib, "wglCreateContext"));
        wglDeleteContext_t wglDeleteContext = reinterpret_cast<wglDeleteContext_t>(GetProcAddress(openglLib, "wglDeleteContext"));
        
        if (!ChoosePixelFormat || !SetPixelFormat || !wglCreateContext || !wglDeleteContext) {
            std::cerr << "Failed to load WGL functions" << std::endl;
            FreeLibrary(openglLib);
            return false;
        }
        
        // Set pixel format
        struct {
            WORD nSize;
            WORD nVersion;
            DWORD dwFlags;
            BYTE iPixelType;
            BYTE cColorBits;
            BYTE cAlphaBits;
            BYTE cDepthBits;
            BYTE cStencilBits;
            BYTE cAuxBuffers;
            BYTE iLayerType;
            BYTE bReserved;
            DWORD dwLayerMask;
            DWORD dwVisibleMask;
            DWORD dwDamageMask;
        } pfd = {};
        
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = 0x00000025; // PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER
        pfd.iPixelType = 0; // PFD_TYPE_RGBA
        pfd.cColorBits = 24;
        pfd.cAlphaBits = 8;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;
        pfd.iLayerType = 0; // PFD_MAIN_PLANE
        
        int pixelFormat = ChoosePixelFormat(deviceContext_, &pfd);
        if (pixelFormat == 0) {
            std::cerr << "Failed to choose pixel format" << std::endl;
            return false;
        }
        
        if (!SetPixelFormat(deviceContext_, pixelFormat, &pfd)) {
            std::cerr << "Failed to set pixel format" << std::endl;
            return false;
        }
        
        // Create OpenGL context
        glContext_ = wglCreateContext(deviceContext_);
        if (!glContext_) {
            std::cerr << "Failed to create OpenGL context" << std::endl;
            return false;
        }
        
        return true;
    }
    
    void applyConfiguration() {
        if (!window_) {
            return;
        }
        
        // Apply window configuration
        setBorderless(config_.borderless);
        setResizable(config_.resizable);
        setCursorVisible(config_.cursorVisible);
        
        if (config_.visible && !visible_) {
            show();
        } else if (!config_.visible && visible_) {
            hide();
        }
        
        if (config_.focused && !focused_) {
            focus();
        }
    }
    
    HCURSOR createCursor(CursorType cursorType) {
        switch (cursorType) {
            case CursorType::Arrow: return LoadCursor(nullptr, IDC_ARROW);
            case CursorType::IBeam: return LoadCursor(nullptr, IDC_IBEAM);
            case CursorType::Crosshair: return LoadCursor(nullptr, IDC_CROSS);
            case CursorType::Hand: return LoadCursor(nullptr, IDC_HAND);
            case CursorType::HorizontalResize: return LoadCursor(nullptr, IDC_SIZEWE);
            case CursorType::VerticalResize: return LoadCursor(nullptr, IDC_SIZENS);
            case CursorType::DiagonalResize: return LoadCursor(nullptr, IDC_SIZENWSE);
            case CursorType::NotAllowed: return LoadCursor(nullptr, IDC_NO);
            case CursorType::Wait: return LoadCursor(nullptr, IDC_WAIT);
            case CursorType::Help: return LoadCursor(nullptr, IDC_HELP);
            default: return LoadCursor(nullptr, IDC_ARROW);
        }
    }
    
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        WindowsWindow* window = nullptr;
        
        if (uMsg == WM_NCCREATE) {
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            window = static_cast<WindowsWindow*>(cs->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        } else {
            window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }
        
        if (window) {
            return window->handleMessage(hwnd, uMsg, wParam, lParam);
        }
        
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    LRESULT handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        WindowEvent windowEvent = {};
        
        switch (uMsg) {
            case WM_CLOSE:
                windowEvent.type = WindowEventType::Close;
                shouldClose_ = true;
                break;
                
            case WM_DESTROY:
                windowEvent.type = WindowEventType::Close;
                shouldClose_ = true;
                break;
                
            case WM_SETFOCUS:
                windowEvent.type = WindowEventType::Focused;
                focused_ = true;
                break;
                
            case WM_KILLFOCUS:
                windowEvent.type = WindowEventType::Unfocused;
                focused_ = false;
                break;
                
            case WM_SIZE:
                windowEvent.type = WindowEventType::Resized;
                windowEvent.width = LOWORD(lParam);
                windowEvent.height = HIWORD(lParam);
                config_.width = windowEvent.width;
                config_.height = windowEvent.height;
                break;
                
            case WM_MOVE:
                windowEvent.type = WindowEventType::Moved;
                windowEvent.x = GET_X_LPARAM(lParam);
                windowEvent.y = GET_Y_LPARAM(lParam);
                config_.x = windowEvent.x;
                config_.y = windowEvent.y;
                break;
                
            case WM_PAINT:
                windowEvent.type = WindowEventType::Paint;
                {
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hwnd, &ps);
                    EndPaint(hwnd, &ps);
                }
                break;
                
            case WM_KEYDOWN:
                windowEvent.type = WindowEventType::KeyPressed;
                windowEvent.keyCode = translateWindowsKey(wParam);
                windowEvent.modifiers = translateWindowsModifiers(GetKeyState(VK_SHIFT) < 0, GetKeyState(VK_CONTROL) < 0, GetKeyState(VK_MENU) < 0, GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0);
                break;
                
            case WM_KEYUP:
                windowEvent.type = WindowEventType::KeyReleased;
                windowEvent.keyCode = translateWindowsKey(wParam);
                windowEvent.modifiers = translateWindowsModifiers(GetKeyState(VK_SHIFT) < 0, GetKeyState(VK_CONTROL) < 0, GetKeyState(VK_MENU) < 0, GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0);
                break;
                
            case WM_LBUTTONDOWN:
                windowEvent.type = WindowEventType::MouseButtonPressed;
                windowEvent.mouseButton = MouseButton::Left;
                windowEvent.x = GET_X_LPARAM(lParam);
                windowEvent.y = GET_Y_LPARAM(lParam);
                windowEvent.modifiers = translateWindowsModifiers(GetKeyState(VK_SHIFT) < 0, GetKeyState(VK_CONTROL) < 0, GetKeyState(VK_MENU) < 0, GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0);
                break;
                
            case WM_LBUTTONUP:
                windowEvent.type = WindowEventType::MouseButtonReleased;
                windowEvent.mouseButton = MouseButton::Left;
                windowEvent.x = GET_X_LPARAM(lParam);
                windowEvent.y = GET_Y_LPARAM(lParam);
                windowEvent.modifiers = translateWindowsModifiers(GetKeyState(VK_SHIFT) < 0, GetKeyState(VK_CONTROL) < 0, GetKeyState(VK_MENU) < 0, GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0);
                break;
                
            case WM_RBUTTONDOWN:
                windowEvent.type = WindowEventType::MouseButtonPressed;
                windowEvent.mouseButton = MouseButton::Right;
                windowEvent.x = GET_X_LPARAM(lParam);
                windowEvent.y = GET_Y_LPARAM(lParam);
                windowEvent.modifiers = translateWindowsModifiers(GetKeyState(VK_SHIFT) < 0, GetKeyState(VK_CONTROL) < 0, GetKeyState(VK_MENU) < 0, GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0);
                break;
                
            case WM_RBUTTONUP:
                windowEvent.type = WindowEventType::MouseButtonReleased;
                windowEvent.mouseButton = MouseButton::Right;
                windowEvent.x = GET_X_LPARAM(lParam);
                windowEvent.y = GET_Y_LPARAM(lParam);
                windowEvent.modifiers = translateWindowsModifiers(GetKeyState(VK_SHIFT) < 0, GetKeyState(VK_CONTROL) < 0, GetKeyState(VK_MENU) < 0, GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0);
                break;
                
            case WM_MBUTTONDOWN:
                windowEvent.type = WindowEventType::MouseButtonPressed;
                windowEvent.mouseButton = MouseButton::Middle;
                windowEvent.x = GET_X_LPARAM(lParam);
                windowEvent.y = GET_Y_LPARAM(lParam);
                windowEvent.modifiers = translateWindowsModifiers(GetKeyState(VK_SHIFT) < 0, GetKeyState(VK_CONTROL) < 0, GetKeyState(VK_MENU) < 0, GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0);
                break;
                
            case WM_MBUTTONUP:
                windowEvent.type = WindowEventType::MouseButtonReleased;
                windowEvent.mouseButton = MouseButton::Middle;
                windowEvent.x = GET_X_LPARAM(lParam);
                windowEvent.y = GET_Y_LPARAM(lParam);
                windowEvent.modifiers = translateWindowsModifiers(GetKeyState(VK_SHIFT) < 0, GetKeyState(VK_CONTROL) < 0, GetKeyState(VK_MENU) < 0, GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0);
                break;
                
            case WM_XBUTTONDOWN:
                windowEvent.type = WindowEventType::MouseButtonPressed;
                windowEvent.mouseButton = (HIWORD(wParam) == XBUTTON1) ? MouseButton::Extra1 : MouseButton::Extra2;
                windowEvent.x = GET_X_LPARAM(lParam);
                windowEvent.y = GET_Y_LPARAM(lParam);
                windowEvent.modifiers = translateWindowsModifiers(GetKeyState(VK_SHIFT) < 0, GetKeyState(VK_CONTROL) < 0, GetKeyState(VK_MENU) < 0, GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0);
                break;
                
            case WM_XBUTTONUP:
                windowEvent.type = WindowEventType::MouseButtonReleased;
                windowEvent.mouseButton = (HIWORD(wParam) == XBUTTON1) ? MouseButton::Extra1 : MouseButton::Extra2;
                windowEvent.x = GET_X_LPARAM(lParam);
                windowEvent.y = GET_Y_LPARAM(lParam);
                windowEvent.modifiers = translateWindowsModifiers(GetKeyState(VK_SHIFT) < 0, GetKeyState(VK_CONTROL) < 0, GetKeyState(VK_MENU) < 0, GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0);
                break;
                
            case WM_MOUSEMOVE:
                windowEvent.type = WindowEventType::MouseMoved;
                windowEvent.x = GET_X_LPARAM(lParam);
                windowEvent.y = GET_Y_LPARAM(lParam);
                windowEvent.modifiers = translateWindowsModifiers(GetKeyState(VK_SHIFT) < 0, GetKeyState(VK_CONTROL) < 0, GetKeyState(VK_MENU) < 0, GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0);
                break;
                
            case WM_MOUSEWHEEL:
                windowEvent.type = WindowEventType::MouseScrolled;
                windowEvent.x = GET_X_LPARAM(lParam);
                windowEvent.y = GET_Y_LPARAM(lParam);
                windowEvent.scrollDelta = static_cast<float32>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
                windowEvent.modifiers = translateWindowsModifiers(GetKeyState(VK_SHIFT) < 0, GetKeyState(VK_CONTROL) < 0, GetKeyState(VK_MENU) < 0, GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0);
                break;
                
            default:
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        
        if (eventCallback_) {
            eventCallback_(windowEvent);
        }
        
        return 0;
    }
    
    KeyCode translateWindowsKey(WPARAM key) const {
        switch (key) {
            case 'A': return KeyCode::A;
            case 'B': return KeyCode::B;
            case 'C': return KeyCode::C;
            case 'D': return KeyCode::D;
            case 'E': return KeyCode::E;
            case 'F': return KeyCode::F;
            case 'G': return KeyCode::G;
            case 'H': return KeyCode::H;
            case 'I': return KeyCode::I;
            case 'J': return KeyCode::J;
            case 'K': return KeyCode::K;
            case 'L': return KeyCode::L;
            case 'M': return KeyCode::M;
            case 'N': return KeyCode::N;
            case 'O': return KeyCode::O;
            case 'P': return KeyCode::P;
            case 'Q': return KeyCode::Q;
            case 'R': return KeyCode::R;
            case 'S': return KeyCode::S;
            case 'T': return KeyCode::T;
            case 'U': return KeyCode::U;
            case 'V': return KeyCode::V;
            case 'W': return KeyCode::W;
            case 'X': return KeyCode::X;
            case 'Y': return KeyCode::Y;
            case 'Z': return KeyCode::Z;
            
            case '0': return KeyCode::Num0;
            case '1': return KeyCode::Num1;
            case '2': return KeyCode::Num2;
            case '3': return KeyCode::Num3;
            case '4': return KeyCode::Num4;
            case '5': return KeyCode::Num5;
            case '6': return KeyCode::Num6;
            case '7': return KeyCode::Num7;
            case '8': return KeyCode::Num8;
            case '9': return KeyCode::Num9;
            
            case VK_RETURN: return KeyCode::Enter;
            case VK_ESCAPE: return KeyCode::Escape;
            case VK_BACK: return KeyCode::Backspace;
            case VK_TAB: return KeyCode::Tab;
            case VK_SPACE: return KeyCode::Space;
            
            case VK_LEFT: return KeyCode::Left;
            case VK_RIGHT: return KeyCode::Right;
            case VK_UP: return KeyCode::Up;
            case VK_DOWN: return KeyCode::Down;
            
            case VK_F1: return KeyCode::F1;
            case VK_F2: return KeyCode::F2;
            case VK_F3: return KeyCode::F3;
            case VK_F4: return KeyCode::F4;
            case VK_F5: return KeyCode::F5;
            case VK_F6: return KeyCode::F6;
            case VK_F7: return KeyCode::F7;
            case VKF8: return KeyCode::F8;
            case VK_F9: return KeyCode::F9;
            case VK_F10: return KeyCode::F10;
            case VK_F11: return KeyCode::F11;
            case VK_F12: return KeyCode::F12;
            
            case VK_LSHIFT: return KeyCode::LeftShift;
            case VK_RSHIFT: return KeyCode::RightShift;
            case VK_LCONTROL: return KeyCode::LeftControl;
            case VK_RCONTROL: return KeyCode::RightControl;
            case VK_LMENU: return KeyCode::LeftAlt;
            case VK_RMENU: return KeyCode::RightAlt;
            
            case VK_LWIN: return KeyCode::LeftSuper;
            case VK_RWIN: return KeyCode::RightSuper;
            
            case VK_NUMPAD0: return KeyCode::Numpad0;
            case VK_NUMPAD1: return KeyCode::Numpad1;
            case VK_NUMPAD2: return KeyCode::Numpad2;
            case VK_NUMPAD3: return KeyCode::Numpad3;
            case VK_NUMPAD4: return KeyCode::Numpad4;
            case VK_NUMPAD5: return KeyCode::Numpad5;
            case VK_NUMPAD6: return KeyCode::Numpad6;
            case VK_NUMPAD7: return KeyCode::Numpad7;
            case VK_NUMPAD8: return KeyCode::Numpad8;
            case VK_NUMPAD9: return KeyCode::Numpad9;
            
            case VK_MULTIPLY: return KeyCode::NumpadMultiply;
            case VK_ADD: return KeyCode::NumpadAdd;
            case VK_SUBTRACT: return KeyCode::NumpadSubtract;
            case VK_DECIMAL: return KeyCode::NumpadDecimal;
            case VK_DIVIDE: return KeyCode::NumpadDivide;
            
            case VK_OEM_1: return KeyCode::Semicolon;
            case VK_OEM_PLUS: return KeyCode::Equals;
            case VK_OEM_COMMA: return KeyCode::Comma;
            case VK_OEM_MINUS: return KeyCode::Minus;
            case VK_OEM_PERIOD: return KeyCode::Period;
            case VK_OEM_2: return KeyCode::Slash;
            case VK_OEM_3: return KeyCode::Grave;
            case VK_OEM_4: return KeyCode::LeftBracket;
            case VK_OEM_5: return KeyCode::Backslash;
            case VK_OEM_6: return KeyCode::RightBracket;
            case VK_OEM_7: return KeyCode::Apostrophe;
            
            case VK_CAPITAL: return KeyCode::CapsLock;
            case VK_SCROLL: return KeyCode::ScrollLock;
            case VK_NUMLOCK: return KeyCode::NumLock;
            case VK_INSERT: return KeyCode::Insert;
            case VK_HOME: return KeyCode::Home;
            case VK_PRIOR: return KeyCode::PageUp;
            case VK_DELETE: return KeyCode::Delete;
            case VK_END: return KeyCode::End;
            case VK_NEXT: return KeyCode::PageDown;
            
            case VK_PAUSE: return KeyCode::Pause;
            case VK_SNAPSHOT: return KeyCode::SysRq;
            case VK_APPS: return KeyCode::Apps;
            
            default: return KeyCode::Unknown;
        }
    }
    
    KeyMod translateWindowsModifiers(bool shift, bool control, bool alt, bool super) const {
        KeyMod modifiers = KeyMod::None;
        
        if (shift) modifiers |= KeyMod::Shift;
        if (control) modifiers |= KeyMod::Control;
        if (alt) modifiers |= KeyMod::Alt;
        if (super) modifiers |= KeyMod::Super;
        
        return modifiers;
    }
};

// Window factory
Window* createWindow() {
    return new WindowsWindow();
}

void destroyWindow(Window* window) {
    delete window;
}

// Windows-specific functions
bool initializeWindowsWindowSystem() {
    // Initialize COM for clipboard operations
    return CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED) == S_OK;
}

void shutdownWindowsWindowSystem() {
    CoUninitialize();
}

HWND getWindowsWindow() {
    // This would typically get the window from the active window
    return nullptr;
}

HDC getWindowsDC() {
    // This would typically get the device context from the active window
    return nullptr;
}

HGLRC getWindowsGLContext() {
    // This would typically get the GL context from the active window
    return nullptr;
}

} // namespace RFWindow