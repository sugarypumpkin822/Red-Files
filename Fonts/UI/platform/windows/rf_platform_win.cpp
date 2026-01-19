#include "../include/rf_platform.h"
#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>

namespace RFPlatform {

using float32 = float;
using uint32 = uint32_t;
using uint8 = uint8_t;

// Custom OpenGL definitions to avoid external library dependency
#define PFD_TYPE_RGBA             0
#define PFD_TYPE_COLORINDEX       1
#define PFD_DOUBLEBUFFER          0x00000001
#define PFD_STEREO               0x00000002
#define PFD_DRAW_TO_WINDOW        0x00000004
#define PFD_DRAW_TO_BITMAP        0x00000008
#define PFD_SUPPORT_GDI           0x00000010
#define PFD_SUPPORT_OPENGL        0x00000020
#define PFD_GENERIC_ACCELERATED   0x00000100
#define PFD_SWAP_LAYER_BUFFERS    0x00000200
#define PFD_SWAP_EXCHANGE         0x00000400
#define PFD_DEPTH_DONTCARE        0x20000000
#define PFD_DOUBLEBUFFER_DONTCARE 0x80000000

typedef struct tagPIXELFORMATDESCRIPTOR {
    WORD  nSize;
    WORD  nVersion;
    DWORD dwFlags;
    BYTE  iPixelType;
    BYTE  cColorBits;
    BYTE  cRedBits;
    BYTE  cRedShift;
    BYTE  cGreenBits;
    BYTE  cGreenShift;
    BYTE  cBlueBits;
    BYTE  cBlueShift;
    BYTE  cAlphaBits;
    BYTE  cAlphaShift;
    BYTE  cAccumBits;
    BYTE  cAccumRedBits;
    BYTE  cAccumGreenBits;
    BYTE  cAccumBlueBits;
    BYTE  cAccumAlphaBits;
    BYTE  cDepthBits;
    BYTE  cStencilBits;
    BYTE  cAuxBuffers;
    BYTE  iLayerType;
    BYTE  bReserved;
    DWORD dwLayerMask;
    DWORD dwVisibleMask;
    DWORD dwDamageMask;
} PIXELFORMATDESCRIPTOR, *PPIXELFORMATDESCRIPTOR, FAR *LPPIXELFORMATDESCRIPTOR;

// Custom WGL definitions
typedef void* HGLRC;
typedef HDC HDC;
typedef HWND HWND;

// Custom WGL function prototypes
typedef int (WINAPI *ChoosePixelFormat_t)(HDC, const PIXELFORMATDESCRIPTOR*);
typedef BOOL (WINAPI *SetPixelFormat_t)(HDC, int, const PIXELFORMATDESCRIPTOR*);
typedef HGLRC (WINAPI *wglCreateContext_t)(HDC);
typedef BOOL (WINAPI *wglDeleteContext_t)(HGLRC);
typedef BOOL (WINAPI *wglMakeCurrent_t)(HDC, HGLRC);
typedef BOOL (WINAPI *wglSwapBuffers_t)(HDC);
typedef PROC (WINAPI *wglGetProcAddress_t)(LPCSTR);

// Custom OpenGL function prototypes
typedef void (APIENTRY *glClear_t)(GLbitfield mask);
typedef void (APIENTRY *glClearColor_t)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
typedef void (APIENTRY *glViewport_t)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef const GLubyte* (APIENTRY *glGetString_t)(GLenum name);
typedef GLenum (APIENTRY *glGetError_t)(void);

// Custom OpenGL constants
#define GL_COLOR_BUFFER_BIT              0x00004000
#define GL_DEPTH_BUFFER_BIT              0x00000100
#define GL_STENCIL_BUFFER_BIT            0x00000400
#define GL_VENDOR                        0x1F00
#define GL_RENDERER                      0x1F01
#define GL_VERSION                       0x1F02
#define GL_EXTENSIONS                    0x1F03

// Windows-specific platform implementation with no external dependencies
class WindowsPlatform : public Platform {
public:
    WindowsPlatform() : window_(nullptr), deviceContext_(nullptr), glContext_(nullptr),
                       shouldClose_(false), focused_(false), wglFunctions_() {
    }
    
    ~WindowsPlatform() {
        shutdown();
    }
    
    bool initialize(const PlatformConfig& config) override {
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
        
        // Show window
        if (config_.windowVisible) {
            ShowWindow(window_, SW_SHOW);
            UpdateWindow(window_);
        }
        
        return true;
    }
    
    void shutdown() override {
        if (glContext_ && wglFunctions_.wglDeleteContext) {
            wglFunctions_.wglDeleteContext(glContext_);
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
    
    void pollEvents() override {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    void swapBuffers() override {
        if (glContext_ && deviceContext_ && wglFunctions_.wglSwapBuffers) {
            wglFunctions_.wglSwapBuffers(deviceContext_);
        }
    }
    
    bool shouldClose() const override {
        return shouldClose_;
    }
    
    void setShouldClose(bool shouldClose) override {
        shouldClose_ = shouldClose;
    }
    
    void* getWindowHandle() const override {
        return window_;
    }
    
    void* getDisplayHandle() const override {
        return nullptr; // Windows doesn't have a display handle like X11
    }
    
    void* getGLContext() const override {
        return glContext_;
    }
    
    void setWindowSize(uint32 width, uint32 height) override {
        if (window_) {
            RECT rect;
            GetWindowRect(window_, &rect);
            SetWindowPos(window_, rect.left, rect.top, rect.left + width, rect.top + height, SWP_NOMOVE | SWP_NOZORDER);
            config_.windowWidth = width;
            config_.windowHeight = height;
        }
    }
    
    void setWindowPosition(int32 x, int32 y) override {
        if (window_) {
            SetWindowPos(window_, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            config_.windowX = x;
            config_.windowY = y;
        }
    }
    
    void getWindowSize(uint32& width, uint32& height) const override {
        if (window_) {
            RECT rect;
            GetWindowRect(window_, &rect);
            width = static_cast<uint32>(rect.right - rect.left);
            height = static_cast<uint32>(rect.bottom - rect.top);
        } else {
            width = config_.windowWidth;
            height = config_.windowHeight;
        }
    }
    
    void getWindowPosition(int32& x, int32& y) const override {
        if (window_) {
            RECT rect;
            GetWindowRect(window_, &rect);
            x = rect.left;
            y = rect.top;
        } else {
            x = config_.windowX;
            y = config_.windowY;
        }
    }
    
    void setWindowTitle(const std::string& title) override {
        if (window_) {
            SetWindowTextA(window_, title.c_str());
            config_.windowTitle = title;
        }
    }
    
    const std::string& getWindowTitle() const override {
        return config_.windowTitle;
    }
    
    void setWindowVisible(bool visible) override {
        if (window_) {
            ShowWindow(window_, visible ? SW_SHOW : SW_HIDE);
            UpdateWindow(window_);
            config_.windowVisible = visible;
        }
    }
    
    bool isWindowVisible() const override {
        return config_.windowVisible;
    }
    
    void setWindowFocused(bool focused) override {
        if (window_ && focused) {
            SetForegroundWindow(window_);
            focused_ = true;
        }
    }
    
    bool isWindowFocused() const override {
        return focused_;
    }
    
    void setWindowMinimized(bool minimized) override {
        if (window_) {
            ShowWindow(window_, minimized ? SW_MINIMIZE : SW_RESTORE);
            UpdateWindow(window_);
        }
    }
    
    bool isWindowMinimized() const override {
        return window_ ? IsIconic(window_) : false;
    }
    
    void setWindowMaximized(bool maximized) override {
        if (window_) {
            ShowWindow(window_, maximized ? SW_MAXIMIZE : SW_RESTORE);
            UpdateWindow(window_);
        }
    }
    
    bool isWindowMaximized() const override {
        if (!window_) return false;
        
        WINDOWPLACEMENT placement;
        if (GetWindowPlacement(window_, &placement)) {
            return placement.showCmd == SW_MAXIMIZE;
        }
        return false;
    }
    
    void setWindowResizable(bool resizable) override {
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
    
    bool isWindowResizable() const override {
        if (!window_) return true;
        
        DWORD style = GetWindowLong(window_, GWL_STYLE);
        return (style & WS_THICKFRAME) != 0;
    }
    
    void setWindowBorderless(bool borderless) override {
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
    
    bool isWindowBorderless() const override {
        if (!window_) return false;
        
        DWORD style = GetWindowLong(window_, GWL_STYLE);
        return (style & WS_CAPTION) == 0;
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
    
    void setCursorVisible(bool visible) override {
        ShowCursor(visible);
    }
    
    bool isCursorVisible() const override {
        CURSORINFO cursorInfo;
        return GetCursorInfo(&cursorInfo) && cursorInfo.flags == CURSOR_SHOWING;
    }
    
    void setClipboardText(const std::string& text) override {
        if (OpenClipboard(nullptr)) {
            EmptyClipboard();
            
            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
            if (hMem) {
                char* pMem = static_cast<char*>(GlobalLock(hMem));
                if (pMem) {
                    std::memcpy(pMem, text.c_str(), text.size() + 1);
                    GlobalUnlock(hMem);
                    SetClipboardData(CF_TEXT, hMem);
                }
                GlobalFree(hMem);
            }
            
            CloseClipboard();
        }
    }
    
    std::string getClipboardText() override {
        std::string text;
        
        if (OpenClipboard(nullptr)) {
            HANDLE hData = GetClipboardData(CF_TEXT);
            if (hData) {
                char* pData = static_cast<char*>(GlobalLock(hData));
                if (pData) {
                    text = pData;
                    GlobalUnlock(hData);
                }
            }
            CloseClipboard();
        }
        
        return text;
    }
    
    void showMessageBox(const std::string& title, const std::string& message, MessageBoxType type) override {
        UINT uType = MB_OK;
        switch (type) {
            case MessageBoxType::Info: uType = MB_ICONINFORMATION; break;
            case MessageBoxType::Warning: uType = MB_ICONWARNING; break;
            case MessageBoxType::Error: uType = MB_ICONERROR; break;
        }
        
        MessageBoxA(window_, message.c_str(), title.c_str(), uType);
    }
    
    PlatformInfo getPlatformInfo() const override {
        PlatformInfo info = {};
        
        // Get display information
        info.displayWidth = GetSystemMetrics(SM_CXSCREEN);
        info.displayHeight = GetSystemMetrics(SM_CYSCREEN);
        info.displayDPI = GetDeviceCaps(GetDC(nullptr), LOGPIXELSX);
        
        // Get system information
        info.platformName = "Windows";
        info.platformVersion = getWindowsVersion();
        info.cpuCount = getCPUCount();
        info.systemRAM = getSystemRAM();
        
        return info;
    }
    
    uint64 getTime() const override {
        LARGE_INTEGER frequency;
        LARGE_INTEGER counter;
        
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&counter);
        
        return static_cast<uint64>(counter.QuadPart * 1000000000ULL / frequency.QuadPart);
    }
    
    void sleep(uint32 milliseconds) override {
        Sleep(milliseconds);
    }
    
    void openURL(const std::string& url) override {
        ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
    
    void openFile(const std::string& filePath) override {
        ShellExecuteA(nullptr, "open", filePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
    
    void openFolder(const std::string& folderPath) override {
        ShellExecuteA(nullptr, "explore", folderPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
    
    std::string getExecutablePath() override {
        char path[MAX_PATH];
        DWORD result = GetModuleFileNameA(nullptr, path, MAX_PATH);
        if (result > 0) {
            return std::string(path);
        }
        return "";
    }
    
    std::string getWorkingDirectory() override {
        char path[MAX_PATH];
        DWORD result = GetCurrentDirectoryA(path, MAX_PATH);
        if (result > 0) {
            return std::string(path);
        }
        return "";
    }
    
    bool setWorkingDirectory(const std::string& path) override {
        return SetCurrentDirectoryA(path.c_str()) != FALSE;
    }
    
    std::string getUserDataPath() override {
        char path[MAX_PATH];
        if (SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, path, MAX_PATH) != S_OK) {
            return "";
        }
        return std::string(path) + "\\" + config_.applicationName;
    }
    
    std::string getTempPath() override {
        char path[MAX_PATH];
        DWORD result = GetTempPathA(MAX_PATH, path);
        if (result > 0) {
            return std::string(path);
        }
        return "";
    }
    
    std::vector<std::string> getCommandLineArguments() override {
        std::vector<std::string> args;
        
        int argc;
        LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        
        for (int i = 0; i < argc; ++i) {
            int length = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, nullptr, 0, nullptr);
            if (length > 0) {
                std::string arg(length, '\0');
                WideCharToMultiByte(CP_UTF8, 0, argv[i], length, &arg[0], length, nullptr);
                args.push_back(arg);
            }
        }
        
        LocalFree(argv);
        return args;
    }
    
    void setConfiguration(const PlatformConfig& config) override {
        config_ = config;
        
        // Apply configuration changes
        if (window_) {
            setWindowSize(config.windowWidth, config.windowHeight);
            setWindowPosition(config.windowX, config.windowY);
            setWindowTitle(config.windowTitle);
            setWindowVisible(config.windowVisible);
        }
    }
    
    const PlatformConfig& getConfiguration() const override {
        return config_;
    }
    
private:
    static const char* windowClassName_ = "RedFilesWindow";
    
    struct WGLFunctions {
        ChoosePixelFormat_t ChoosePixelFormat;
        SetPixelFormat_t SetPixelFormat;
        wglCreateContext_t wglCreateContext;
        wglDeleteContext_t wglDeleteContext;
        wglMakeCurrent_t wglMakeCurrent;
        wglSwapBuffers_t wglSwapBuffers;
        wglGetProcAddress_t wglGetProcAddress;
    };
    
    PlatformConfig config_;
    
    HWND window_;
    HDC deviceContext_;
    HGLRC glContext_;
    
    bool shouldClose_;
    bool focused_;
    WGLFunctions wglFunctions_;
    
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
        
        if (config_.windowBorderless) {
            style = WS_POPUP | WS_VISIBLE;
        }
        
        window_ = CreateWindowExA(
            0,                              // Extended style
            windowClassName_,                  // Class name
            config_.windowTitle.c_str(),          // Window title
            style,                           // Window style
            config_.windowX,                        // X position
            config_.windowY,                        // Y position
            config_.windowWidth,                    // Width
            config_.windowHeight,                   // Height
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
        wglFunctions_.ChoosePixelFormat = reinterpret_cast<ChoosePixelFormat_t>(GetProcAddress(openglLib, "ChoosePixelFormat"));
        wglFunctions_.SetPixelFormat = reinterpret_cast<SetPixelFormat_t>(GetProcAddress(openglLib, "SetPixelFormat"));
        wglFunctions_.wglCreateContext = reinterpret_cast<wglCreateContext_t>(GetProcAddress(openglLib, "wglCreateContext"));
        wglFunctions_.wglDeleteContext = reinterpret_cast<wglDeleteContext_t>(GetProcAddress(openglLib, "wglDeleteContext"));
        wglFunctions_.wglMakeCurrent = reinterpret_cast<wglMakeCurrent_t>(GetProcAddress(openglLib, "wglMakeCurrent"));
        wglFunctions_.wglSwapBuffers = reinterpret_cast<wglSwapBuffers_t>(GetProcAddress(openglLib, "wglSwapBuffers"));
        wglFunctions_.wglGetProcAddress = reinterpret_cast<wglGetProcAddress_t>(GetProcAddress(openglLib, "wglGetProcAddress"));
        
        if (!wglFunctions_.ChoosePixelFormat || !wglFunctions_.SetPixelFormat || 
            !wglFunctions_.wglCreateContext || !wglFunctions_.wglDeleteContext ||
            !wglFunctions_.wglMakeCurrent || !wglFunctions_.wglSwapBuffers) {
            std::cerr << "Failed to load WGL functions" << std::endl;
            FreeLibrary(openglLib);
            return false;
        }
        
        // Set pixel format
        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 24;
        pfd.cAlphaBits = 8;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;
        pfd.cRedBits = 8;
        pfd.cGreenBits = 8;
        pfd.cBlueBits = 8;
        pfd.cAlphaBits = 8;
        pfd.cAccumRedBits = 8;
        pfd.cAccumGreenBits = 8;
        pfd.cAccumBlueBits = 8;
        pfd.cAccumAlphaBits = 8;
        pfd.cAccumStencilBits = 8;
        
        int pixelFormat = wglFunctions_.ChoosePixelFormat(deviceContext_, &pfd);
        if (pixelFormat == 0) {
            std::cerr << "Failed to choose pixel format" << std::endl;
            return false;
        }
        
        if (!wglFunctions_.SetPixelFormat(deviceContext_, pixelFormat, &pfd)) {
            std::cerr << "Failed to set pixel format" << std::endl;
            return false;
        }
        
        // Create OpenGL context
        glContext_ = wglFunctions_.wglCreateContext(deviceContext_);
        if (!glContext_) {
            std::cerr << "Failed to create OpenGL context" << std::endl;
            return false;
        }
        
        // Make context current
        if (!wglFunctions_.wglMakeCurrent(deviceContext_, glContext_)) {
            std::cerr << "Failed to make OpenGL context current" << std::endl;
            return false;
        }
        
        return true;
    }
    
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        WindowsPlatform* platform = nullptr;
        
        if (uMsg == WM_NCCREATE) {
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            platform = static_cast<WindowsPlatform*>(cs->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(platform));
        } else {
            platform = reinterpret_cast<WindowsPlatform*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }
        
        if (platform) {
            return platform->handleMessage(hwnd, uMsg, wParam, lParam);
        }
        
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    LRESULT handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_CLOSE:
                shouldClose_ = true;
                return 0;
                
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
                
            case WM_SETFOCUS:
                focused_ = true;
                return 0;
                
            case WM_KILLFOCUS:
                focused_ = false;
                return 0;
                
            case WM_SIZE:
                if (wParam == SIZE_MINIMIZED) {
                    // Window was minimized
                } else if (wParam == SIZE_MAXIMIZED) {
                    // Window was maximized
                } else if (wParam == SIZE_RESTORED) {
                    // Window was restored
                }
                return 0;
                
            case WM_MOVE:
                // Window was moved
                return 0;
                
            case WM_PAINT:
                {
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hwnd, &ps);
                    EndPaint(hwnd, &ps);
                    return 0;
                }
                
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
                // Handle keyboard events
                return 0;
                
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP:
            case WM_MOUSEMOVE:
            case WM_MOUSEWHEEL:
                // Handle mouse events
                return 0;
                
            default:
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    
    std::string getWindowsVersion() const {
        OSVERSIONINFOEX osvi;
        ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        
        if (GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&osvi), 0)) {
            return std::to_string(osvi.dwMajorVersion) + "." + 
                   std::to_string(osvi.dwMinorVersion) + "." + 
                   std::to_string(osvi.dwBuildNumber);
        }
        
        return "Unknown";
    }
    
    uint32 getCPUCount() const {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        return si.dwNumberOfProcessors;
    }
    
    uint64 getSystemRAM() const {
        MEMORYSTATUSEX ms;
        if (GlobalMemoryStatusEx(&ms)) {
            return ms.ullTotalPhys;
        }
        return 0;
    }
};

// Platform factory
Platform* createPlatform() {
    return new WindowsPlatform();
}

void destroyPlatform(Platform* platform) {
    delete platform;
}

// Windows-specific functions
bool initializeWindowsPlatform() {
    // Initialize COM for clipboard operations
    return CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED) == S_OK;
}

void shutdownWindowsPlatform() {
    CoUninitialize();
}

HWND getWindowsWindow() {
    // This would typically get the window from the active platform
    return nullptr;
}

HDC getWindowsDC() {
    // This would typically get the device context from the active platform
    return nullptr;
}

HGLRC getWindowsGLContext() {
    // This would typically get the GL context from the active platform
    return nullptr;
}

bool checkOpenGLExtension(const char* extension) {
    // Load OpenGL functions dynamically
    HMODULE openglLib = LoadLibraryA("opengl32.dll");
    if (!openglLib) {
        return false;
    }
    
    glGetString_t glGetString = reinterpret_cast<glGetString_t>(GetProcAddress(openglLib, "glGetString"));
    if (!glGetString) {
        FreeLibrary(openglLib);
        return false;
    }
    
    const char* extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    FreeLibrary(openglLib);
    
    return extensions && strstr(extensions, extension) != nullptr;
}

void* getOpenGLProcAddress(const char* procname) {
    HMODULE openglLib = LoadLibraryA("opengl32.dll");
    if (!openglLib) {
        return nullptr;
    }
    
    wglGetProcAddress_t wglGetProcAddress = reinterpret_cast<wglGetProcAddress_t>(GetProcAddress(openglLib, "wglGetProcAddress"));
    if (!wglGetProcAddress) {
        FreeLibrary(openglLib);
        return nullptr;
    }
    
    void* proc = wglGetProcAddress(procname);
    FreeLibrary(openglLib);
    return proc;
}

} // namespace RFPlatform
