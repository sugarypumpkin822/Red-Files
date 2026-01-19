#include "../include/rf_window.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <dlfcn.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <iostream>

namespace RFWindow {

using float32 = float;
using uint32 = uint32_t;
using uint8 = uint8_t;

// Custom OpenGL/GLX definitions to avoid external library dependency
typedef struct _GLXContext* GLXContext;
typedef struct _XVisualInfo* XVisualInfo;

// Custom GLX function prototypes
typedef XVisualInfo* (*glXChooseVisual_t)(Display*, int, int*);
typedef GLXContext (*glXCreateContext_t)(Display*, XVisualInfo*, GLXContext, Bool);
typedef void (*glXDestroyContext_t)(Display*, GLXContext);
typedef Bool (*glXMakeCurrent_t)(Display*, GLXDrawable, GLXContext);
typedef void (*glXSwapBuffers_t)(Display*, GLXDrawable);

// Custom GLX constants
#define GLX_USE_GL 0x0001
#define GLX_BUFFER_SIZE 0x0002
#define GLX_LEVEL 0x0003
#define GLX_RGBA 0x0004
#define GLX_DOUBLEBUFFER 0x0005
#define GLX_STEREO 0x0006
#define GLX_AUX_BUFFERS 0x0007
#define GLX_RED_SIZE 0x0008
#define GLX_GREEN_SIZE 0x0009
#define GLX_BLUE_SIZE 0x000a
#define GLX_ALPHA_SIZE 0x000b
#define GLX_DEPTH_SIZE 0x000c
#define GLX_STENCIL_SIZE 0x000d
#define GLX_ACCUM_RED_SIZE 0x000e
#define GLX_ACCUM_GREEN_SIZE 0x000f
#define GLX_ACCUM_BLUE_SIZE 0x0010
#define GLX_ACCUM_ALPHA_SIZE 0x0011

// Custom XRandR definitions to avoid external library dependency
typedef struct _XRRScreenResources* XRRScreenResources;
typedef struct _XRRCrtcInfo* XRRCrtcInfo;

// Custom XRandR function prototypes
typedef XRRScreenResources* (*XRRGetScreenResources_t)(Display*, Window);
typedef void (*XRRFreeScreenResources_t)(XRRScreenResources*);
typedef XRRCrtcInfo* (*XRRGetCrtcInfo_t)(Display*, XRRScreenResources*, RRCrtc);
typedef void (*XRRFreeCrtcInfo_t)(XRRCrtcInfo*);
typedef Bool (*XRRQueryExtension_t)(Display*, int*, int*);

// X11-specific window implementation
class X11Window : public Window {
public:
    X11Window() : display_(nullptr), window_(0), glContext_(nullptr), visual_(nullptr),
                  colormap_(0), parentWindow_(0), shouldClose_(false), focused_(false),
                  minimized_(false), maximized_(false), visible_(false), borderless_(false),
                  resizable_(true), cursorVisible_(true) {
        // Load GLX functions dynamically
        void* libGL = dlopen("libGL.so", RTLD_LAZY);
        if (libGL) {
            glxFunctions_.glXChooseVisual = (glXChooseVisual_t)dlsym(libGL, "glXChooseVisual");
            glxFunctions_.glXCreateContext = (glXCreateContext_t)dlsym(libGL, "glXCreateContext");
            glxFunctions_.glXDestroyContext = (glXDestroyContext_t)dlsym(libGL, "glXDestroyContext");
            glxFunctions_.glXMakeCurrent = (glXMakeCurrent_t)dlsym(libGL, "glXMakeCurrent");
            glxFunctions_.glXSwapBuffers = (glXSwapBuffers_t)dlsym(libGL, "glXSwapBuffers");
        }

        // Load XRandR functions dynamically
        void* libXrandr = dlopen("libXrandr.so", RTLD_LAZY);
        if (libXrandr) {
            xrandrFunctions_.XRRGetScreenResources = (XRRGetScreenResources_t)dlsym(libXrandr, "XRRGetScreenResources");
            xrandrFunctions_.XRRFreeScreenResources = (XRRFreeScreenResources_t)dlsym(libXrandr, "XRRFreeScreenResources");
            xrandrFunctions_.XRRGetCrtcInfo = (XRRGetCrtcInfo_t)dlsym(libXrandr, "XRRGetCrtcInfo");
            xrandrFunctions_.XRRFreeCrtcInfo = (XRRFreeCrtcInfo_t)dlsym(libXrandr, "XRRFreeCrtcInfo");
            xrandrFunctions_.XRRQueryExtension = (XRRQueryExtension_t)dlsym(libXrandr, "XRRQueryExtension");
        }
    }

    ~X11Window() {
        destroy();
    }

    bool create(const WindowConfig& config) override {
        config_ = config;

        // Open X11 display
        display_ = XOpenDisplay(nullptr);
        if (!display_) {
            std::cerr << "Failed to open X11 display" << std::endl;
            return false;
        }

        int screen = DefaultScreen(display_);

        // Choose visual
        if (!chooseVisual()) {
            std::cerr << "Failed to choose visual" << std::endl;
            return false;
        }

        // Create colormap
        colormap_ = XCreateColormap(display_, RootWindow(display_, screen), visual_, AllocNone);

        // Set window attributes
        XSetWindowAttributes swa;
        swa.colormap = colormap_;
        swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | 
                         ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                         StructureNotifyMask | FocusChangeMask | EnterWindowMask | LeaveWindowMask |
                         PropertyChangeMask;

        // Determine parent window
        Window parent = config_.parentWindowHandle ? 
                       reinterpret_cast<Window>(config_.parentWindowHandle) : 
                       RootWindow(display_, screen);

        // Create window
        window_ = XCreateWindow(display_, parent,
                               config_.x, config_.y,
                               config_.width, config_.height,
                               0, visualInfo_.depth, InputOutput, visual_,
                               CWColormap | CWEventMask, &swa);

        if (!window_) {
            std::cerr << "Failed to create X11 window" << std::endl;
            return false;
        }

        // Create OpenGL context
        if (!createGLContext()) {
            std::cerr << "Failed to create OpenGL context" << std::endl;
            return false;
        }

        // Make context current
        if (!glxFunctions_.glXMakeCurrent || !glxFunctions_.glXMakeCurrent(display_, window_, glContext_)) {
            std::cerr << "Failed to make OpenGL context current" << std::endl;
            return false;
        }

        // Set window properties
        setupWindowProperties();

        // Set window size and position
        if (config_.width > 0 && config_.height > 0) {
            XResizeWindow(display_, window_, config_.width, config_.height);
        }

        if (config_.x >= 0 && config_.y >= 0) {
            XMoveWindow(display_, window_, config_.x, config_.y);
        }

        // Set window title
        if (!config_.title.empty()) {
            XStoreName(display_, window_, config_.title.c_str());
        }

        // Apply initial state
        if (config_.visible) {
            show();
        }

        if (config_.focused) {
            focus();
        }

        if (config_.borderless) {
            setBorderless(true);
        }

        return true;
    }

    void destroy() override {
        if (glContext_ && glxFunctions_.glXDestroyContext) {
            glxFunctions_.glXDestroyContext(display_, glContext_);
            glContext_ = nullptr;
        }

        if (window_) {
            XDestroyWindow(display_, window_);
            window_ = 0;
        }

        if (colormap_) {
            XFreeColormap(display_, colormap_);
            colormap_ = 0;
        }

        if (display_) {
            XCloseDisplay(display_);
            display_ = nullptr;
        }
    }

    void show() override {
        if (display_ && window_) {
            XMapWindow(display_, window_);
            XFlush(display_);
            visible_ = true;
        }
    }

    void hide() override {
        if (display_ && window_) {
            XUnmapWindow(display_, window_);
            XFlush(display_);
            visible_ = false;
        }
    }

    bool isVisible() const override {
        return visible_;
    }

    void focus() override {
        if (display_ && window_) {
            XSetInputFocus(display_, window_, RevertToParent, CurrentTime);
            XFlush(display_);
            focused_ = true;
        }
    }

    bool isFocused() const override {
        return focused_;
    }

    void minimize() override {
        if (display_ && window_) {
            XIconifyWindow(display_, window_, DefaultScreen(display_));
            XFlush(display_);
            minimized_ = true;
        }
    }

    void restore() override {
        if (display_ && window_) {
            if (minimized_) {
                XMapWindow(display_, window_);
                minimized_ = false;
            }
            if (maximized_) {
                // Restore from maximized state
                sendClientMessage("_NET_WM_STATE", 0, 
                                   XInternAtom(display_, "_NET_WM_STATE_MAXIMIZED_HORZ", False),
                                   XInternAtom(display_, "_NET_WM_STATE_MAXIMIZED_VERT", False));
                maximized_ = false;
            }
            XFlush(display_);
        }
    }

    void maximize() override {
        if (display_ && window_) {
            sendClientMessage("_NET_WM_STATE", 1,
                               XInternAtom(display_, "_NET_WM_STATE_MAXIMIZED_HORZ", False),
                               XInternAtom(display_, "_NET_WM_STATE_MAXIMIZED_VERT", False));
            XFlush(display_);
            maximized_ = true;
        }
    }

    bool isMinimized() const override {
        return minimized_;
    }

    bool isMaximized() const override {
        return maximized_;
    }

    void setResizable(bool resizable) override {
        resizable_ = resizable;
        if (display_ && window_) {
            XSizeHints sizeHints;
            sizeHints.flags = PMinSize | PMaxSize;

            if (resizable) {
                sizeHints.min_width = 100;
                sizeHints.min_height = 100;
                sizeHints.max_width = 32767;
                sizeHints.max_height = 32767;
            } else {
                sizeHints.min_width = config_.width;
                sizeHints.min_height = config_.height;
                sizeHints.max_width = config_.width;
                sizeHints.max_height = config_.height;
            }

            XSetWMNormalHints(display_, window_, &sizeHints);
            XFlush(display_);
        }
    }

    bool isResizable() const override {
        return resizable_;
    }

    void setBorderless(bool borderless) override {
        borderless_ = borderless;
        if (display_ && window_) {
            if (borderless) {
                // Remove window decorations
                Atom atomBorderless = XInternAtom(display_, "_MOTIF_WM_HINTS", False);
                struct {
                    unsigned long flags;
                    unsigned long functions;
                    unsigned long decorations;
                    long input_mode;
                    unsigned long status;
                } hints = {2, 0, 0, 0, 0};

                XChangeProperty(display_, window_, atomBorderless, atomBorderless,
                               32, PropModeReplace, (unsigned char*)&hints, 5);
            } else {
                // Restore window decorations
                Atom atomBorderless = XInternAtom(display_, "_MOTIF_WM_HINTS", False);
                XDeleteProperty(display_, window_, atomBorderless);
            }
            XFlush(display_);
        }
    }

    bool isBorderless() const override {
        return borderless_;
    }

    void setPosition(int32 x, int32 y) override {
        if (display_ && window_) {
            XMoveWindow(display_, window_, x, y);
            XFlush(display_);
            config_.x = x;
            config_.y = y;
        }
    }

    void getPosition(int32& x, int32& y) const override {
        x = config_.x;
        y = config_.y;

        if (display_ && window_) {
            XWindowAttributes attribs;
            if (XGetWindowAttributes(display_, window_, &attribs)) {
                x = attribs.x;
                y = attribs.y;
            }
        }
    }

    void setSize(uint32 width, uint32 height) override {
        if (display_ && window_) {
            XResizeWindow(display_, window_, width, height);
            XFlush(display_);
            config_.width = width;
            config_.height = height;
        }
    }

    void getSize(uint32& width, uint32& height) const override {
        width = config_.width;
        height = config_.height;

        if (display_ && window_) {
            XWindowAttributes attribs;
            if (XGetWindowAttributes(display_, window_, &attribs)) {
                width = attribs.width;
                height = attribs.height;
            }
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
        if (display_ && window_) {
            XStoreName(display_, window_, title.c_str());
            XFlush(display_);
            config_.title = title;
        }
    }

    const std::string& getTitle() const override {
        return config_.title;
    }

    void setOpacity(float32 opacity) override {
        if (display_ && window_) {
            // Set window opacity using _NET_WM_WINDOW_OPACITY
            Atom atomOpacity = XInternAtom(display_, "_NET_WM_WINDOW_OPACITY", False);
            uint32 opacityValue = static_cast<uint32>(opacity * 0xFFFFFFFF);
            XChangeProperty(display_, window_, atomOpacity, XA_CARDINAL, 32,
                           PropModeReplace, (unsigned char*)&opacityValue, 1);
            XFlush(display_);
        }
    }

    float32 getOpacity() const override {
        // Implementation would query _NET_WM_WINDOW_OPACITY
        return 1.0f;
    }

    void setCursorVisible(bool visible) override {
        cursorVisible_ = visible;
        if (display_ && window_) {
            if (visible) {
                XUndefineCursor(display_, window_);
            } else {
                // Create empty cursor
                Pixmap bitmap;
                XColor black;
                static char empty_data[1] = {0};

                black.red = black.green = black.blue = 0;

                bitmap = XCreateBitmapFromData(display_, window_, empty_data, 1, 1);
                Cursor cursor = XCreatePixmapCursor(display_, bitmap, bitmap,
                                                   &black, &black, 0, 0);
                XDefineCursor(display_, window_, cursor);
                XFreeCursor(display_, cursor);
                XFreePixmap(display_, bitmap);
            }
            XFlush(display_);
        }
    }

    bool isCursorVisible() const override {
        return cursorVisible_;
    }

    void setCursorPosition(int32 x, int32 y) override {
        if (display_ && window_) {
            XWarpPointer(display_, None, window_, 0, 0, 0, 0, x, y);
            XFlush(display_);
        }
    }

    void getCursorPosition(int32& x, int32& y) const override {
        if (display_ && window_) {
            Window root, child;
            int rootX, rootY, winX, winY;
            unsigned int mask;

            if (XQueryPointer(display_, window_, &root, &child, &rootX, &rootY, &winX, &winY, &mask)) {
                x = winX;
                y = winY;
            }
        }
    }

    void setCursor(CursorType cursorType) override {
        if (display_ && window_) {
            Cursor cursor = createCursor(cursorType);
            if (cursor) {
                XDefineCursor(display_, window_, cursor);
                XFlush(display_);
            }
        }
    }

    void setIcon(const std::vector<uint8>& iconData, uint32 width, uint32 height) override {
        if (display_ && window_ && !iconData.empty()) {
            // Create icon pixmap
            Visual* visual = visual_;
            int depth = visualInfo_.depth;

            Pixmap iconPixmap = XCreatePixmap(display_, window_, width, height, depth);
            Pixmap maskPixmap = XCreatePixmap(display_, window_, width, height, 1);

            // Set up icon properties
            XWMHints wmHints;
            wmHints.flags = IconPixmapHint | IconMaskHint;
            wmHints.icon_pixmap = iconPixmap;
            wmHints.icon_mask = maskPixmap;

            XSetWMHints(display_, window_, &wmHints);
            XFlush(display_);
        }
    }

    void setParent(void* parentHandle) override {
        if (display_ && window_) {
            Window parent = parentHandle ? 
                           reinterpret_cast<Window>(parentHandle) : 
                           RootWindow(display_, DefaultScreen(display_));

            XReparentWindow(display_, window_, parent, config_.x, config_.y);
            XFlush(display_);
            parentWindow_ = parent;
        }
    }

    void* getParent() const override {
        return reinterpret_cast<void*>(parentWindow_);
    }

    void* getNativeHandle() const override {
        return reinterpret_cast<void*>(window_);
    }

    void* getNativeDisplay() const override {
        return reinterpret_cast<void*>(display_);
    }

    void* getGLContext() const override {
        return glContext_;
    }

    void swapBuffers() override {
        if (display_ && window_ && glxFunctions_.glXSwapBuffers) {
            glxFunctions_.glXSwapBuffers(display_, window_);
        }
    }

    bool shouldClose() const override {
        return shouldClose_;
    }

    void setShouldClose(bool shouldClose) override {
        shouldClose_ = shouldClose;
    }

    void pollEvents() override {
        if (!display_) {
            return;
        }

        XEvent event;
        while (XPending(display_) > 0) {
            XNextEvent(display_, &event);
            processEvent(event);
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

        // Apply configuration changes
        if (!config_.title.empty()) {
            setTitle(config_.title);
        }

        if (config_.width > 0 && config_.height > 0) {
            setSize(config_.width, config_.height);
        }

        if (config_.x >= 0 && config_.y >= 0) {
            setPosition(config_.x, config_.y);
        }

        setBorderless(config_.borderless);
        setResizable(config_.resizable);
        setCursorVisible(config_.cursorVisible);

        if (config_.visible && !visible_) {
            show();
        } else if (!config_.visible && visible_) {
            hide();
        }
    }

    std::vector<MonitorInfo> getMonitors() const override {
        std::vector<MonitorInfo> monitors;

        if (display_) {
            // Use XRandR to get monitor information
            int event_base, error_base;
            if (xrandrFunctions_.XRRQueryExtension && xrandrFunctions_.XRRQueryExtension(display_, &event_base, &error_base)) {
                int screen = DefaultScreen(display_);
                if (xrandrFunctions_.XRRGetScreenResources) {
                    XRRScreenResources* resources = xrandrFunctions_.XRRGetScreenResources(display_, RootWindow(display_, screen));

                    if (resources) {
                        for (int i = 0; i < resources->ncrtc; ++i) {
                            if (xrandrFunctions_.XRRGetCrtcInfo) {
                                XRRCrtcInfo* crtcInfo = xrandrFunctions_.XRRGetCrtcInfo(display_, resources, resources->crtcs[i]);
                                if (crtcInfo && crtcInfo->mode != None) {
                                    MonitorInfo monitor = {};
                                    monitor.id = i;
                                    monitor.x = crtcInfo->x;
                                    monitor.y = crtcInfo->y;
                                    monitor.width = crtcInfo->width;
                                    monitor.height = crtcInfo->height;
                                    monitor.isPrimary = (i == 0); // Simplified primary detection
                                    monitor.name = "Monitor " + std::to_string(i);

                                    monitors.push_back(monitor);
                                }
                                if (xrandrFunctions_.XRRFreeCrtcInfo) {
                                    xrandrFunctions_.XRRFreeCrtcInfo(crtcInfo);
                                }
                            }
                        }
                        if (xrandrFunctions_.XRRFreeScreenResources) {
                            xrandrFunctions_.XRRFreeScreenResources(resources);
                        }
                    }
                }
            }
        }

        // Fallback: return primary monitor info
        if (monitors.empty()) {
            MonitorInfo monitor = {};
            monitor.id = 0;
            monitor.x = 0;
            monitor.y = 0;
            monitor.width = DisplayWidth(display_, DefaultScreen(display_));
            monitor.height = DisplayHeight(display_, DefaultScreen(display_));
            monitor.isPrimary = true;
            monitor.name = "Primary Monitor";
            monitors.push_back(monitor);
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
    WindowConfig config_;

    Display* display_;
    Window window_;
    GLXContext glContext_;
    XVisualInfo visualInfo_;
    Visual* visual_;
    Colormap colormap_;
    Window parentWindow_;

    bool shouldClose_;
    bool focused_;
    bool minimized_;
    bool maximized_;
    bool visible_;
    bool borderless_;
    bool resizable_;
    bool cursorVisible_;

    std::function<void(const WindowEvent&)> eventCallback_;

    // Dynamic function pointers
    struct GLXFunctions {
        glXChooseVisual_t glXChooseVisual;
        glXCreateContext_t glXCreateContext;
        glXDestroyContext_t glXDestroyContext;
        glXMakeCurrent_t glXMakeCurrent;
        glXSwapBuffers_t glXSwapBuffers;
    } glxFunctions_;

    struct XRandRFunctions {
        XRRGetScreenResources_t XRRGetScreenResources;
        XRRFreeScreenResources_t XRRFreeScreenResources;
        XRRGetCrtcInfo_t XRRGetCrtcInfo;
        XRRFreeCrtcInfo_t XRRFreeCrtcInfo;
        XRRQueryExtension_t XRRQueryExtension;
    } xrandrFunctions_;

    bool chooseVisual() {
        int visualAttribs[] = {
            GLX_RGBA,
            GLX_DEPTH_SIZE, 24,
            GLX_DOUBLEBUFFER,
            GLX_RED_SIZE, 8,
            GLX_GREEN_SIZE, 8,
            GLX_BLUE_SIZE, 8,
            GLX_ALPHA_SIZE, 8,
            None
        };

        if (!glxFunctions_.glXChooseVisual) {
            return false;
        }

        visual_ = glxFunctions_.glXChooseVisual(display_, DefaultScreen(display_), visualAttribs);
        if (!visual_) {
            return false;
        }

        visualInfo_ = *visual_;
        visual_ = visualInfo_.visual;

        return true;
    }

    bool createGLContext() {
        if (!glxFunctions_.glXCreateContext) {
            return false;
        }

        glContext_ = glxFunctions_.glXCreateContext(display_, &visualInfo_, nullptr, GL_TRUE);
        if (!glContext_) {
            return false;
        }

        return true;
    }

    void setupWindowProperties() {
        if (!display_ || !window_) {
            return;
        }
        
        // Set up window manager protocols
        Atom atomDeleteWindow = XInternAtom(display_, "WM_DELETE_WINDOW", False);
        Atom atomWMProtocols = XInternAtom(display_, "WM_PROTOCOLS", False);
        XSetWMProtocols(display_, window_, &atomDeleteWindow, 1);
        
        // Set window class hints
        XClassHint classHint;
        classHint.res_name = const_cast<char*>(config_.title.c_str());
        classHint.res_class = const_cast<char*>("RedFiles");
        XSetClassHint(display_, window_, &classHint);
        
        // Set size hints
        XSizeHints sizeHints;
        sizeHints.flags = PSize | PMinSize;
        sizeHints.width = config_.width;
        sizeHints.height = config_.height;
        sizeHints.min_width = 100;
        sizeHints.min_height = 100;
        XSetWMNormalHints(display_, window_, &sizeHints);
    }
    
    void sendClientMessage(const std::string& message, long data0, long data1, long data2) {
        if (!display_ || !window_) {
            return;
        }
        
        Atom atomMessage = XInternAtom(display_, message.c_str(), False);
        XEvent event;
        event.type = ClientMessage;
        event.xclient.window = window_;
        event.xclient.message_type = atomMessage;
        event.xclient.format = 32;
        event.xclient.data.l[0] = data0;
        event.xclient.data.l[1] = data1;
        event.xclient.data.l[2] = data2;
        
        XSendEvent(display_, RootWindow(display_, DefaultScreen(display_)), False, 
                   SubstructureNotifyMask | SubstructureRedirectMask, &event);
    }
    
    Cursor createCursor(CursorType cursorType) {
        if (!display_) {
            return None;
        }
        
        // Map cursor types to X11 cursors
        unsigned int cursorShape;
        switch (cursorType) {
            case CursorType::Arrow: cursorShape = XC_arrow; break;
            case CursorType::IBeam: cursorShape = XC_xterm; break;
            case CursorType::Crosshair: cursorShape = XC_crosshair; break;
            case CursorType::Hand: cursorShape = XC_hand2; break;
            case CursorType::HorizontalResize: cursorShape = XC_sb_h_double_arrow; break;
            case CursorType::VerticalResize: cursorShape = XC_sb_v_double_arrow; break;
            case CursorType::DiagonalResize: cursorShape = XC_fleur; break;
            case CursorType::NotAllowed: cursorShape = XC_X_cursor; break;
            case CursorType::Wait: cursorShape = XC_watch; break;
            case CursorType::Help: cursorShape = XC_question_arrow; break;
            default: cursorShape = XC_arrow; break;
        }
        
        return XCreateFontCursor(display_, cursorShape);
    }
    
    void processEvent(const XEvent& event) {
        WindowEvent windowEvent = {};
        
        switch (event.type) {
            case ClientMessage:
                if (event.xclient.data.l[0] == static_cast<long>(XInternAtom(display_, "WM_DELETE_WINDOW", False))) {
                    windowEvent.type = WindowEventType::Close;
                    shouldClose_ = true;
                }
                break;
                
            case DestroyNotify:
                windowEvent.type = WindowEventType::Close;
                shouldClose_ = true;
                break;
                
            case FocusIn:
                windowEvent.type = WindowEventType::Focused;
                focused_ = true;
                break;
                
            case FocusOut:
                windowEvent.type = WindowEventType::Unfocused;
                focused_ = false;
                break;
                
            case ConfigureNotify:
                windowEvent.type = WindowEventType::Resized;
                windowEvent.width = event.xconfigure.width;
                windowEvent.height = event.xconfigure.height;
                windowEvent.x = event.xconfigure.x;
                windowEvent.y = event.xconfigure.y;
                config_.width = event.xconfigure.width;
                config_.height = event.xconfigure.height;
                config_.x = event.xconfigure.x;
                config_.y = event.xconfigure.y;
                break;
                
            case Expose:
                windowEvent.type = WindowEventType::Paint;
                break;
                
            case MapNotify:
                windowEvent.type = WindowEventType::Shown;
                visible_ = true;
                break;
                
            case UnmapNotify:
                windowEvent.type = WindowEventType::Hidden;
                visible_ = false;
                break;
                
            case KeyPress:
                windowEvent.type = WindowEventType::KeyPressed;
                windowEvent.keyCode = translateX11Key(event.xkey.keycode);
                windowEvent.modifiers = translateX11Modifiers(event.xkey.state);
                break;
                
            case KeyRelease:
                windowEvent.type = WindowEventType::KeyReleased;
                windowEvent.keyCode = translateX11Key(event.xkey.keycode);
                windowEvent.modifiers = translateX11Modifiers(event.xkey.state);
                break;
                
            case ButtonPress:
                windowEvent.type = WindowEventType::MouseButtonPressed;
                windowEvent.mouseButton = translateX11MouseButton(event.xbutton.button);
                windowEvent.x = event.xbutton.x;
                windowEvent.y = event.xbutton.y;
                windowEvent.modifiers = translateX11Modifiers(event.xbutton.state);
                break;
                
            case ButtonRelease:
                windowEvent.type = WindowEventType::MouseButtonReleased;
                windowEvent.mouseButton = translateX11MouseButton(event.xbutton.button);
                windowEvent.x = event.xbutton.x;
                windowEvent.y = event.xbutton.y;
                windowEvent.modifiers = translateX11Modifiers(event.xbutton.state);
                break;
                
            case MotionNotify:
                windowEvent.type = WindowEventType::MouseMoved;
                windowEvent.x = event.xmotion.x;
                windowEvent.y = event.xmotion.y;
                windowEvent.modifiers = translateX11Modifiers(event.xmotion.state);
                break;
                
            case EnterNotify:
                windowEvent.type = WindowEventType::MouseEntered;
                break;
                
            case LeaveNotify:
                windowEvent.type = WindowEventType::MouseExited;
                break;
        }
        
        if (eventCallback_) {
            eventCallback_(windowEvent);
        }
    }
    
    KeyCode translateX11Key(unsigned int keycode) {
        KeySym keysym = XKeycodeToKeysym(display_, keycode, 0);
        
        switch (keysym) {
            case XK_a: return KeyCode::A;
            case XK_b: return KeyCode::B;
            case XK_c: return KeyCode::C;
            case XK_d: return KeyCode::D;
            case XK_e: return KeyCode::E;
            case XK_f: return KeyCode::F;
            case XK_g: return KeyCode::G;
            case XK_h: return KeyCode::H;
            case XK_i: return KeyCode::I;
            case XK_j: return KeyCode::J;
            case XK_k: return KeyCode::K;
            case XK_l: return KeyCode::L;
            case XK_m: return KeyCode::M;
            case XK_n: return KeyCode::N;
            case XK_o: return KeyCode::O;
            case XK_p: return KeyCode::P;
            case XK_q: return KeyCode::Q;
            case XK_r: return KeyCode::R;
            case XK_s: return KeyCode::S;
            case XK_t: return KeyCode::T;
            case XK_u: return KeyCode::U;
            case XK_v: return KeyCode::V;
            case XK_w: return KeyCode::W;
            case XK_x: return KeyCode::X;
            case XK_y: return KeyCode::Y;
            case XK_z: return KeyCode::Z;
            
            case XK_0: return KeyCode::Num0;
            case XK_1: return KeyCode::Num1;
            case XK_2: return KeyCode::Num2;
            case XK_3: return KeyCode::Num3;
            case XK_4: return KeyCode::Num4;
            case XK_5: return KeyCode::Num5;
            case XK_6: return KeyCode::Num6;
            case XK_7: return KeyCode::Num7;
            case XK_8: return KeyCode::Num8;
            case XK_9: return KeyCode::Num9;
            
            case XK_Return: return KeyCode::Enter;
            case XK_Escape: return KeyCode::Escape;
            case XK_BackSpace: return KeyCode::Backspace;
            case XK_Tab: return KeyCode::Tab;
            case XK_space: return KeyCode::Space;
            
            case XK_Left: return KeyCode::Left;
            case XK_Right: return KeyCode::Right;
            case XK_Up: return KeyCode::Up;
            case XK_Down: return KeyCode::Down;
            
            default: return KeyCode::Unknown;
        }
    }
    
    KeyMod translateX11Modifiers(unsigned int state) {
        KeyMod modifiers = KeyMod::None;
        
        if (state & ShiftMask) modifiers |= KeyMod::Shift;
        if (state & ControlMask) modifiers |= KeyMod::Control;
        if (state & Mod1Mask) modifiers |= KeyMod::Alt;
        if (state & Mod4Mask) modifiers |= KeyMod::Super;
        
        return modifiers;
    }
    
    MouseButton translateX11MouseButton(unsigned int button) {
        switch (button) {
            case Button1: return MouseButton::Left;
            case Button2: return MouseButton::Middle;
            case Button3: return MouseButton::Right;
            case Button4: return MouseButton::Extra1;
            case Button5: return MouseButton::Extra2;
            default: return MouseButton::Unknown;
        }
    }
};

// Window factory
Window* createWindow() {
    return new X11Window();
}

void destroyWindow(Window* window) {
    delete window;
}

// X11-specific functions
bool initializeX11WindowSystem() {
    return XInitThreads() != 0;
}

void shutdownX11WindowSystem() {
    // X11 cleanup is handled by window destruction
}

Display* getX11Display() {
    // This would typically get the display from the active window
    return nullptr;
}

Window getX11Window() {
    // This would typically get the window from the active window
    return 0;
}

GLXContext getX11GLContext() {
    // This would typically get the GL context from the active window
    return nullptr;
}

} // namespace RFWindow