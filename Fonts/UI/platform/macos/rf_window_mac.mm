#include "../include/rf_window.h"
#include <Cocoa/Cocoa.h>
#include <CoreFoundation/CoreFoundation.h>
#include <OpenGL/OpenGL.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <CoreGraphics/CoreGraphics.h>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>

namespace RFWindow {

using float32 = float;
using uint32 = uint32_t;
using uint8 = uint8_t;

// macOS-specific window implementation with no external dependencies
class MacOSWindow : public Window {
public:
    MacOSWindow() : window_(nullptr), view_(nullptr), glContext_(nullptr),
                    shouldClose_(false), focused_(false), visible_(false),
                    minimized_(false), maximized_(false), borderless_(false),
                    resizable_(true), cursorVisible_(true) {
    }
    
    ~MacOSWindow() {
        destroy();
    }
    
    bool create(const WindowConfig& config) override {
        config_ = config;
        
        // Create application if needed
        if (!initializeApplication()) {
            std::cerr << "Failed to initialize application" << std::endl;
            return false;
        }
        
        // Create window
        if (!createWindow()) {
            std::cerr << "Failed to create window" << std::endl;
            return false;
        }
        
        // Create view
        if (!createView()) {
            std::cerr << "Failed to create view" << std::endl;
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
        // Cleanup OpenGL context
        if (glContext_) {
            [glContext_ release];
            glContext_ = nullptr;
        }
        
        // Cleanup view
        if (view_) {
            [view_ release];
            view_ = nullptr;
        }
        
        // Cleanup window
        if (window_) {
            [window_ close];
            window_ = nullptr;
        }
    }
    
    void show() override {
        if (window_) {
            [window_ makeKeyAndOrderFront:nil];
            visible_ = true;
        }
    }
    
    void hide() override {
        if (window_) {
            [window_ orderOut:nil];
            visible_ = false;
        }
    }
    
    bool isVisible() const override {
        return visible_;
    }
    
    void focus() override {
        if (window_) {
            [window_ makeKeyAndOrderFront:nil];
            focused_ = true;
        }
    }
    
    bool isFocused() const override {
        return focused_;
    }
    
    void minimize() override {
        if (window_) {
            [window_ miniaturize:nil];
            minimized_ = true;
        }
    }
    
    void restore() override {
        if (window_) {
            [window_ deminiaturize:nil];
            minimized_ = false;
            maximized_ = false;
        }
    }
    
    void maximize() override {
        if (window_) {
            [window_ zoom:nil];
            maximized_ = true;
        }
    }
    
    bool isMinimized() const override {
        return window_ ? [window_ isMiniaturized] : false;
    }
    
    bool isMaximized() const override {
        return window_ ? [window_ isZoomed] : false;
    }
    
    void setResizable(bool resizable) override {
        resizable_ = resizable;
        if (window_) {
            NSUInteger styleMask = [window_ styleMask];
            if (resizable) {
                styleMask |= NSWindowStyleMaskResizable;
            } else {
                styleMask &= ~NSWindowStyleMaskResizable;
            }
            [window_ setStyleMask:styleMask];
        }
    }
    
    bool isResizable() const override {
        return resizable_;
    }
    
    void setBorderless(bool borderless) override {
        borderless_ = borderless;
        if (window_) {
            NSUInteger styleMask = [window_ styleMask];
            if (borderless) {
                styleMask &= ~(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable);
            } else {
                styleMask |= NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
            }
            [window_ setStyleMask:styleMask];
        }
    }
    
    bool isBorderless() const override {
        return borderless_;
    }
    
    void setPosition(int32 x, int32 y) override {
        if (window_) {
            NSPoint screenPoint = NSMakePoint(x, y);
            [window_ setFrameOrigin:screenPoint];
            config_.x = x;
            config_.y = y;
        }
    }
    
    void getPosition(int32& x, int32& y) const override {
        if (window_) {
            NSPoint screenPoint = [window_ frame].origin;
            x = static_cast<int32>(screenPoint.x);
            y = static_cast<int32>(screenPoint.y);
        } else {
            x = config_.x;
            y = config_.y;
        }
    }
    
    void setSize(uint32 width, uint32 height) override {
        if (window_) {
            NSRect frame = [window_ frame];
            frame.size.width = width;
            frame.size.height = height;
            [window_ setFrame:frame display:YES];
            config_.width = width;
            config_.height = height;
        }
    }
    
    void getSize(uint32& width, uint32& height) const override {
        if (window_) {
            NSRect frame = [window_ frame];
            width = static_cast<uint32>(frame.size.width);
            height = static_cast<uint32>(frame.size.height);
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
            NSString* nsTitle = [NSString stringWithUTF8String:title.c_str()];
            [window_ setTitle:nsTitle];
            config_.title = title;
        }
    }
    
    const std::string& getTitle() const override {
        return config_.title;
    }
    
    void setOpacity(float32 opacity) override {
        if (window_) {
            [window_ setAlphaValue:opacity];
        }
    }
    
    float32 getOpacity() const override {
        return window_ ? [window_ alphaValue] : 1.0f;
    }
    
    void setCursorVisible(bool visible) override {
        if (visible != cursorVisible_) {
            if (visible) {
                [NSCursor unhide];
            } else {
                [NSCursor hide];
            }
            cursorVisible_ = visible;
        }
    }
    
    bool isCursorVisible() const override {
        return cursorVisible_;
    }
    
    void setCursorPosition(int32 x, int32 y) override {
        CGDirectDisplayID display = CGMainDisplayID();
        CGPoint cgPoint = CGPointMake(x, CGDisplayPixelsHigh(display) - y);
        CGWarpMouseCursorPosition(cgPoint);
        CGAssociateMouseAndMouseCursorPosition(true);
    }
    
    void getCursorPosition(int32& x, int32& y) const override {
        CGEventRef event = CGEventCreate(nil);
        CGPoint cursorLocation = CGEventGetLocation(event);
        CFRelease(event);
        
        CGDirectDisplayID display = CGMainDisplayID();
        x = static_cast<int32>(cursorLocation.x);
        y = static_cast<int32>(CGDisplayPixelsHigh(display) - cursorLocation.y);
    }
    
    void setCursor(CursorType cursorType) override {
        NSCursor* cursor = createCursor(cursorType);
        if (cursor) {
            [cursor set];
        }
    }
    
    void setIcon(const std::vector<uint8>& iconData, uint32 width, uint32 height) override {
        if (window_ && !iconData.empty()) {
            // Create NSImage from data
            NSData* imageData = [NSData dataWithBytes:iconData.data() length:iconData.size()];
            NSImage* image = [[NSImage alloc] initWithData:imageData];
            if (image) {
                [window_ setRepresentedURL:nil];
                [[window_ standardWindowButton:NSWindowDocumentIconButton] setImage:image];
                [image release];
            }
        }
    }
    
    void setParent(void* parentHandle) override {
        if (window_ && parentHandle) {
            NSWindow* parentWindow = static_cast<NSWindow*>(parentHandle);
            [window_ setParentWindow:parentWindow];
        }
    }
    
    void* getParent() const override {
        return window_ ? [window_ parentWindow] : nullptr;
    }
    
    void* getNativeHandle() const override {
        return window_;
    }
    
    void* getNativeDisplay() const override {
        return nullptr; // macOS doesn't have a display handle like X11
    }
    
    void* getGLContext() const override {
        return glContext_;
    }
    
    void swapBuffers() override {
        if (glContext_ && view_) {
            [glContext_ flushBuffer];
        }
    }
    
    bool shouldClose() const override {
        return shouldClose_;
    }
    
    void setShouldClose(bool shouldClose) override {
        shouldClose_ = shouldClose;
    }
    
    void pollEvents() override {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
        
        NSEvent* event = nil;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                           untilDate:[NSDate distantPast]
                                              inMode:NSDefaultRunLoopMode
                                             dequeue:YES])) {
            [NSApp sendEvent:event];
        }
        
        [pool release];
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
        
        // Get all display IDs
        CGDirectDisplayID displays[32];
        uint32 displayCount = 0;
        
        CGError error = CGGetActiveDisplayList(32, displays, &displayCount);
        if (error == kCGErrorSuccess) {
            for (uint32 i = 0; i < displayCount; ++i) {
                CGDirectDisplayID display = displays[i];
                
                MonitorInfo monitor = {};
                monitor.id = i;
                monitor.x = static_cast<int32>(CGDisplayBounds(display).origin.x);
                monitor.y = static_cast<int32>(CGDisplayBounds(display).origin.y);
                monitor.width = static_cast<uint32>(CGDisplayBounds(display).size.width);
                monitor.height = static_cast<uint32>(CGDisplayBounds(display).size.height);
                monitor.isPrimary = (display == CGMainDisplayID());
                
                // Get display name
                NSString* displayName = [[NSString alloc] initWithUTF8String:CGDisplayIOServicePort(display)];
                monitor.name = [displayName UTF8String];
                [displayName release];
                
                monitors.push_back(monitor);
            }
        }
        
        return monitors;
    }
    
    MonitorInfo getPrimaryMonitor() const override {
        CGDirectDisplayID display = CGMainDisplayID();
        
        MonitorInfo monitor = {};
        monitor.id = 0;
        monitor.x = static_cast<int32>(CGDisplayBounds(display).origin.x);
        monitor.y = static_cast<int32>(CGDisplayBounds(display).origin.y);
        monitor.width = static_cast<uint32>(CGDisplayBounds(display).size.width);
        monitor.height = static_cast<uint32>(CGDisplayBounds(display).size.height);
        monitor.isPrimary = true;
        
        // Get display name
        NSString* displayName = [[NSString alloc] initWithUTF8String:CGDisplayIOServicePort(display)];
        monitor.name = [displayName UTF8String];
        [displayName release];
        
        return monitor;
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
    
    NSWindow* window_;
    NSView* view_;
    NSOpenGLContext* glContext_;
    
    bool shouldClose_;
    bool focused_;
    bool visible_;
    bool minimized_;
    bool maximized_;
    bool borderless_;
    bool resizable_;
    bool cursorVisible_;
    
    std::function<void(const WindowEvent&)> eventCallback_;
    
    bool initializeApplication() {
        // Initialize NSApplication if not already done
        if (![NSApplication sharedApplication]) {
            return false;
        }
        
        return true;
    }
    
    bool createWindow() {
        // Create window
        NSRect frame = NSMakeRect(config_.x, config_.y, config_.width, config_.height);
        NSUInteger styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
        
        if (config_.borderless) {
            styleMask = NSWindowStyleMaskBorderless;
        }
        
        window_ = [[NSWindow alloc] initWithContentRect:frame
                                             styleMask:styleMask
                                               backing:NSBackingStoreBuffered
                                                 defer:NO];
        
        if (!window_) {
            return false;
        }
        
        // Set window properties
        NSString* title = [NSString stringWithUTF8String:config_.title.c_str()];
        [window_ setTitle:title];
        [window_ setReleasedWhenClosed:NO];
        [window_ setAcceptsMouseMovedEvents:YES];
        
        // Create delegate
        WindowDelegate* delegate = [[WindowDelegate alloc] initWithWindow:this];
        [window_ setDelegate:delegate];
        [delegate release];
        
        return true;
    }
    
    bool createView() {
        // Create view
        NSRect frame = NSMakeRect(0, 0, config_.width, config_.height);
        view_ = [[NSView alloc] initWithFrame:frame];
        
        if (!view_) {
            return false;
        }
        
        // Set view properties
        [view_ setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        [window_ setContentView:view_];
        
        return true;
    }
    
    bool createGLContext() {
        // Create OpenGL pixel format
        NSOpenGLPixelFormatAttribute attributes[] = {
            NSOpenGLPFAWindow,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFAStencilSize, 8,
            0
        };
        
        NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        if (!pixelFormat) {
            return false;
        }
        
        // Create OpenGL context
        glContext_ = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
        [pixelFormat release];
        
        if (!glContext_) {
            return false;
        }
        
        // Set context properties
        [glContext_ setView:view_];
        [glContext_ makeCurrentContext];
        
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
    
    NSCursor* createCursor(CursorType cursorType) {
        switch (cursorType) {
            case CursorType::Arrow: return [NSCursor arrowCursor];
            case CursorType::IBeam: return [NSCursor IBeamCursor];
            case CursorType::Crosshair: return [NSCursor crosshairCursor];
            case CursorType::Hand: return [NSCursor pointingHandCursor];
            case CursorType::HorizontalResize: return [NSCursor resizeLeftRightCursor];
            case CursorType::VerticalResize: return [NSCursor resizeUpDownCursor];
            case CursorType::DiagonalResize: return [NSCursor resizeLeftRightCursor]; // macOS doesn't have diagonal cursors
            case CursorType::NotAllowed: return [NSCursor operationNotAllowedCursor];
            case CursorType::Wait: return [NSCursor arrowCursor]; // macOS doesn't have wait cursor
            case CursorType::Help: return [NSCursor arrowCursor]; // macOS doesn't have help cursor
            default: return [NSCursor arrowCursor];
        }
    }
    
    void handleWindowShouldClose() {
        shouldClose_ = true;
        
        if (eventCallback_) {
            WindowEvent event = {};
            event.type = WindowEventType::Close;
            eventCallback_(event);
        }
    }
    
    void handleWindowDidBecomeKey() {
        focused_ = true;
        
        if (eventCallback_) {
            WindowEvent event = {};
            event.type = WindowEventType::Focused;
            eventCallback_(event);
        }
    }
    
    void handleWindowDidResignKey() {
        focused_ = false;
        
        if (eventCallback_) {
            WindowEvent event = {};
            event.type = WindowEventType::Unfocused;
            eventCallback_(event);
        }
    }
    
    void handleWindowDidResize() {
        if (window_) {
            NSRect frame = [window_ frame];
            config_.width = static_cast<uint32>(frame.size.width);
            config_.height = static_cast<uint32>(frame.size.height);
            
            if (eventCallback_) {
                WindowEvent event = {};
                event.type = WindowEventType::Resized;
                event.width = config_.width;
                event.height = config_.height;
                eventCallback_(event);
            }
        }
    }
    
    void handleWindowDidMove() {
        if (window_) {
            NSPoint origin = [window_ frame].origin;
            config_.x = static_cast<int32>(origin.x);
            config_.y = static_cast<int32>(origin.y);
            
            if (eventCallback_) {
                WindowEvent event = {};
                event.type = WindowEventType::Moved;
                event.x = config_.x;
                event.y = config_.y;
                eventCallback_(event);
            }
        }
    }
    
    void handleKeyDown(NSEvent* event) {
        if (eventCallback_) {
            WindowEvent windowEvent = {};
            windowEvent.type = WindowEventType::KeyPressed;
            windowEvent.keyCode = translateMacKeyCode([event keyCode]);
            windowEvent.modifiers = translateMacModifiers([event modifierFlags]);
            eventCallback_(windowEvent);
        }
    }
    
    void handleKeyUp(NSEvent* event) {
        if (eventCallback_) {
            WindowEvent windowEvent = {};
            windowEvent.type = WindowEventType::KeyReleased;
            windowEvent.keyCode = translateMacKeyCode([event keyCode]);
            windowEvent.modifiers = translateMacModifiers([event modifierFlags]);
            eventCallback_(windowEvent);
        }
    }
    
    void handleMouseDown(NSEvent* event) {
        if (eventCallback_) {
            WindowEvent windowEvent = {};
            windowEvent.type = WindowEventType::MouseButtonPressed;
            windowEvent.mouseButton = translateMacMouseButton([event buttonNumber]);
            windowEvent.x = static_cast<int32>([event locationInWindow].x);
            windowEvent.y = static_cast<int32>([event locationInWindow].y);
            windowEvent.modifiers = translateMacModifiers([event modifierFlags]);
            eventCallback_(windowEvent);
        }
    }
    
    void handleMouseUp(NSEvent* event) {
        if (eventCallback_) {
            WindowEvent windowEvent = {};
            windowEvent.type = WindowEventType::MouseButtonReleased;
            windowEvent.mouseButton = translateMacMouseButton([event buttonNumber]);
            windowEvent.x = static_cast<int32>([event locationInWindow].x);
            windowEvent.y = static_cast<int32>([event locationInWindow].y);
            windowEvent.modifiers = translateMacModifiers([event modifierFlags]);
            eventCallback_(windowEvent);
        }
    }
    
    void handleMouseMoved(NSEvent* event) {
        if (eventCallback_) {
            WindowEvent windowEvent = {};
            windowEvent.type = WindowEventType::MouseMoved;
            windowEvent.x = static_cast<int32>([event locationInWindow].x);
            windowEvent.y = static_cast<int32>([event locationInWindow].y);
            windowEvent.modifiers = translateMacModifiers([event modifierFlags]);
            eventCallback_(windowEvent);
        }
    }
    
    void handleMouseScroll(NSEvent* event) {
        if (eventCallback_) {
            WindowEvent windowEvent = {};
            windowEvent.type = WindowEventType::MouseScrolled;
            windowEvent.x = static_cast<int32>([event locationInWindow].x);
            windowEvent.y = static_cast<int32>([event locationInWindow].y);
            windowEvent.scrollDelta = static_cast<float32>([event scrollingDeltaY]);
            windowEvent.modifiers = translateMacModifiers([event modifierFlags]);
            eventCallback_(windowEvent);
        }
    }
    
    KeyCode translateMacKeyCode(uint32 macKeyCode) const {
        switch (macKeyCode) {
            case 0x00: return KeyCode::A;
            case 0x0B: return KeyCode::B;
            case 0x08: return KeyCode::C;
            case 0x02: return KeyCode::D;
            case 0x0E: return KeyCode::E;
            case 0x03: return KeyCode::F;
            case 0x05: return KeyCode::G;
            case 0x04: return KeyCode::H;
            case 0x22: return KeyCode::I;
            case 0x26: return KeyCode::J;
            case 0x28: return KeyCode::K;
            case 0x25: return KeyCode::L;
            case 0x2E: return KeyCode::M;
            case 0x2D: return KeyCode::N;
            case 0x1F: return KeyCode::O;
            case 0x23: return KeyCode::P;
            case 0x0C: return KeyCode::Q;
            case 0x0F: return KeyCode::R;
            case 0x01: return KeyCode::S;
            case 0x11: return KeyCode::T;
            case 0x20: return KeyCode::U;
            case 0x09: return KeyCode::V;
            case 0x0D: return KeyCode::W;
            case 0x10: return KeyCode::X;
            case 0x1E: return KeyCode::Y;
            case 0x21: return KeyCode::Z;
            
            case 0x1D: return KeyCode::Num0;
            case 0x12: return KeyCode::Num1;
            case 0x13: return KeyCode::Num2;
            case 0x14: return KeyCode::Num3;
            case 0x15: return KeyCode::Num4;
            case 0x17: return KeyCode::Num5;
            case 0x16: return KeyCode::Num6;
            case 0x1A: return KeyCode::Num7;
            case 0x1C: return KeyCode::Num8;
            case 0x19: return KeyCode::Num9;
            
            case 0x24: return KeyCode::Enter;
            case 0x35: return KeyCode::Escape;
            case 0x33: return KeyCode::Backspace;
            case 0x30: return KeyCode::Tab;
            case 0x31: return KeyCode::Space;
            
            case 0x7B: return KeyCode::Left;
            case 0x7C: return KeyCode::Right;
            case 0x7E: return KeyCode::Up;
            case 0x7D: return KeyCode::Down;
            
            case 0x7A: return KeyCode::F1;
            case 0x78: return KeyCode::F2;
            case 0x63: return KeyCode::F3;
            case 0x76: return KeyCode::F4;
            case 0x60: return KeyCode::F5;
            case 0x61: return KeyCode::F6;
            case 0x62: return KeyCode::F7;
            case 0x64: return KeyCode::F8;
            case 0x65: return KeyCode::F9;
            case 0x6D: return KeyCode::F10;
            case 0x67: return KeyCode::F11;
            case 0x6F: return KeyCode::F12;
            
            case 0x38: return KeyCode::LeftShift;
            case 0x3C: return KeyCode::RightShift;
            case 0x3B: return KeyCode::LeftControl;
            case 0x3E: return KeyCode::RightControl;
            case 0x3A: return KeyCode::LeftAlt;
            case 0x3D: return KeyCode::RightAlt;
            case 0x37: return KeyCode::LeftSuper;
            case 0x36: return KeyCode::RightSuper;
            
            case 0x52: return KeyCode::Numpad0;
            case 0x53: return KeyCode::Numpad1;
            case 0x54: return KeyCode::Numpad2;
            case 0x55: return KeyCode::Numpad3;
            case 0x56: return KeyCode::Numpad4;
            case 0x57: return KeyCode::Numpad5;
            case 0x58: return KeyCode::Numpad6;
            case 0x59: return KeyCode::Numpad7;
            case 0x5B: return KeyCode::Numpad8;
            case 0x5C: return KeyCode::Numpad9;
            
            case 0x45: return KeyCode::NumpadMultiply;
            case 0x4E: return KeyCode::NumpadAdd;
            case 0x4A: return KeyCode::NumpadSubtract;
            case 0x41: return KeyCode::NumpadDecimal;
            case 0x4B: return KeyCode::NumpadDivide;
            
            case 0x29: return KeyCode::Semicolon;
            case 0x18: return KeyCode::Equals;
            case 0x2B: return KeyCode::Comma;
            case 0x1B: return KeyCode::Minus;
            case 0x2F: return KeyCode::Period;
            case 0x2C: return KeyCode::Slash;
            case 0x32: return KeyCode::Grave;
            case 0x21: return KeyCode::LeftBracket;
            case 0x2A: return KeyCode::Backslash;
            case 0x1E: return KeyCode::RightBracket;
            case 0x27: return KeyCode::Apostrophe;
            
            case 0x39: return KeyCode::CapsLock;
            case 0x6B: return KeyCode::ScrollLock;
            case 0x47: return KeyCode::NumLock;
            case 0x72: return KeyCode::Insert;
            case 0x73: return KeyCode::Home;
            case 0x74: return KeyCode::PageUp;
            case 0x75: return KeyCode::Delete;
            case 0x77: return KeyCode::End;
            case 0x79: return KeyCode::PageDown;
            
            case 0x71: return KeyCode::Pause;
            case 0x69: return KeyCode::SysRq;
            case 0x6E: return KeyCode::Apps;
            
            default: return KeyCode::Unknown;
        }
    }
    
    MouseButton translateMacMouseButton(uint32 macButton) const {
        switch (macButton) {
            case 0: return MouseButton::Left;
            case 1: return MouseButton::Right;
            case 2: return MouseButton::Middle;
            case 3: return MouseButton::Extra1;
            case 4: return MouseButton::Extra2;
            default: return MouseButton::Left;
        }
    }
    
    KeyMod translateMacModifiers(uint32 macModifiers) const {
        KeyMod modifiers = KeyMod::None;
        
        if (macModifiers & NSEventModifierFlagShift) modifiers |= KeyMod::Shift;
        if (macModifiers & NSEventModifierFlagControl) modifiers |= KeyMod::Control;
        if (macModifiers & NSEventModifierFlagOption) modifiers |= KeyMod::Alt;
        if (macModifiers & NSEventModifierFlagCommand) modifiers |= KeyMod::Super;
        
        return modifiers;
    }
};

// Window delegate class
@interface WindowDelegate : NSObject <NSWindowDelegate> {
    MacOSWindow* window_;
}

- (id)initWithWindow:(MacOSWindow*)window;

@end

@implementation WindowDelegate

- (id)initWithWindow:(MacOSWindow*)window {
    self = [super init];
    if (self) {
        window_ = window;
    }
    return self;
}

- (void)windowShouldClose:(NSNotification*)notification {
    window_->handleWindowShouldClose();
}

- (void)windowDidBecomeKey:(NSNotification*)notification {
    window_->handleWindowDidBecomeKey();
}

- (void)windowDidResignKey:(NSNotification*)notification {
    window_->handleWindowDidResignKey();
}

- (void)windowDidResize:(NSNotification*)notification {
    window_->handleWindowDidResize();
}

- (void)windowDidMove:(NSNotification*)notification {
    window_->handleWindowDidMove();
}

- (void)keyDown:(NSEvent*)event {
    window_->handleKeyDown(event);
}

- (void)keyUp:(NSEvent*)event {
    window_->handleKeyUp(event);
}

- (void)mouseDown:(NSEvent*)event {
    window_->handleMouseDown(event);
}

- (void)mouseUp:(NSEvent*)event {
    window_->handleMouseUp(event);
}

- (void)mouseMoved:(NSEvent*)event {
    window_->handleMouseMoved(event);
}

- (void)mouseDragged:(NSEvent*)event {
    window_->handleMouseMoved(event);
}

- (void)scrollWheel:(NSEvent*)event {
    window_->handleMouseScroll(event);
}

@end

// Window factory
Window* createWindow() {
    return new MacOSWindow();
}

void destroyWindow(Window* window) {
    delete window;
}

// macOS-specific functions
bool initializeMacOSWindowSystem() {
    // Initialize NSApplication
    [NSApplication sharedApplication];
    return true;
}

void shutdownMacOSWindowSystem() {
    // Cleanup NSApplication
    [NSApp terminate:nil];
}

NSWindow* getMacOSWindow() {
    // This would typically get the window from the active window
    return nullptr;
}

NSOpenGLContext* getMacOSGLContext() {
    // This would typically get the GL context from the active window
    return nullptr;
}

} // namespace RFWindow