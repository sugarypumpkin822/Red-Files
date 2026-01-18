#include "../include/rf_window.h"
#import <Cocoa/Cocoa.h>
#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <OpenGL/glext.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/glext.h>
#include <vector>
#include <cstring>
#include <algorithm>

namespace RFWindow {

using float32 = float;
using uint32 = uint32_t;
using uint8 = uint8_t;

// Cocoa-specific window implementation
class CocoaWindow : public Window {
public:
    CocoaWindow() : window_(nil), glView_(nil), glContext_(nil), pixelFormat_(nil),
                    shouldClose_(false), focused_(false), visible_(false), 
                    minimized_(false), maximized_(false), borderless_(false),
                    resizable_(true), cursorVisible_(true) {
    }
    
    ~CocoaWindow() {
        destroy();
    }
    
    bool create(const WindowConfig& config) override {
        config_ = config;
        
        // Initialize NSApplication if not already done
        if (![NSApplication sharedApplication]) {
            [NSApplication sharedApplication];
        }
        
        // Create pixel format
        if (!createPixelFormat()) {
            std::cerr << "Failed to create pixel format" << std::endl;
            return false;
        }
        
        // Create window
        if (!createWindow()) {
            std::cerr << "Failed to create Cocoa window" << std::endl;
            return false;
        }
        
        // Create OpenGL view
        if (!createOpenGLView()) {
            std::cerr << "Failed to create OpenGL view" << std::endl;
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
            [glContext_ release];
            glContext_ = nil;
        }
        
        if (glView_) {
            [glView_ release];
            glView_ = nil;
        }
        
        if (pixelFormat_) {
            [pixelFormat_ release];
            pixelFormat_ = nil;
        }
        
        if (window_) {
            [window_ close];
            window_ = nil;
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
            if (minimized_) {
                [window_ deminiaturize:nil];
                minimized_ = false;
            }
            if (maximized_) {
                [window_ zoom:nil];
                maximized_ = false;
            }
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
                styleMask &= ~(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | 
                              NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable);
            } else {
                styleMask |= NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | 
                              NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
            }
            [window_ setStyleMask:styleMask];
        }
    }
    
    bool isBorderless() const override {
        return borderless_;
    }
    
    void setPosition(int32 x, int32 y) override {
        if (window_) {
            NSPoint point = NSMakePoint(x, y);
            [window_ setFrameTopLeftPoint:point];
            config_.x = x;
            config_.y = y;
        }
    }
    
    void getPosition(int32& x, int32& y) const override {
        if (window_) {
            NSPoint point = [window_ frame].origin;
            x = static_cast<int32>(point.x);
            y = static_cast<int32>(point.y);
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
            NSString* titleString = [NSString stringWithUTF8String:title.c_str()];
            [window_ setTitle:titleString];
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
        CGWarpMouseCursorPosition(CGPointMake(x, y));
        CGAssociateMouseAndMouseCursorPosition(true);
    }
    
    void getCursorPosition(int32& x, int32& y) const override {
        NSPoint mouseLocation = [NSEvent mouseLocation];
        x = static_cast<int32>(mouseLocation.x);
        y = static_cast<int32>(mouseLocation.y);
    }
    
    void setCursor(CursorType cursorType) override {
        NSCursor* cursor = createCursor(cursorType);
        if (cursor) {
            [cursor set];
            [cursor release];
        }
    }
    
    void setIcon(const std::vector<uint8>& iconData, uint32 width, uint32 height) override {
        if (window_ && !iconData.empty()) {
            // Create NSImage from icon data
            NSData* data = [NSData dataWithBytes:iconData.data() length:iconData.size()];
            NSImage* image = [[NSImage alloc] initWithData:data];
            
            if (image) {
                [window_ setMiniwindowImage:image];
                [window_ setRepresentedURL:nil]; // Update dock icon
                [image release];
            }
        }
    }
    
    void setParent(void* parentHandle) override {
        if (window_ && parentHandle) {
            NSWindow* parentWindow = static_cast<NSWindow*>(parentHandle);
            [parentWindow addChildWindow:window_ ordered:NSWindowAbove];
        }
    }
    
    void* getParent() const override {
        return window_ ? [[window_ parentWindow] retain] : nullptr;
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
        if (glContext_) {
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
        
        NSArray* screens = [NSScreen screens];
        for (NSUInteger i = 0; i < [screens count]; ++i) {
            NSScreen* screen = [screens objectAtIndex:i];
            NSRect frame = [screen frame];
            
            MonitorInfo monitor = {};
            monitor.id = static_cast<uint32>(i);
            monitor.x = static_cast<int32>(frame.origin.x);
            monitor.y = static_cast<int32>(frame.origin.y);
            monitor.width = static_cast<uint32>(frame.size.width);
            monitor.height = static_cast<uint32>(frame.size.height);
            monitor.isPrimary = (screen == [NSScreen mainScreen]);
            monitor.name = "Monitor " + std::to_string(i);
            
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
    
    NSWindow* window_;
    NSOpenGLView* glView_;
    NSOpenGLContext* glContext_;
    NSOpenGLPixelFormat* pixelFormat_;
    
    bool shouldClose_;
    bool focused_;
    bool visible_;
    bool minimized_;
    bool maximized_;
    bool borderless_;
    bool resizable_;
    bool cursorVisible_;
    
    std::function<void(const WindowEvent&)> eventCallback_;
    
    bool createPixelFormat() {
        NSOpenGLPixelFormatAttribute attributes[] = {
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFAScreenMask,
            CGMainDisplayID(),
            0
        };
        
        pixelFormat_ = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        return pixelFormat_ != nil;
    }
    
    bool createWindow() {
        NSRect frame = NSMakeRect(config_.x, config_.y, config_.width, config_.height);
        
        NSUInteger styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | 
                              NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
        
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
        [window_ setTitle:[NSString stringWithUTF8String:config_.title.c_str()]];
        [window_ setAcceptsMouseMovedEvents:YES];
        [window_ setRestorable:NO];
        
        // Set up window delegate
        [window_ setDelegate:[[CocoaWindowDelegate alloc] initWithWindow:this]];
        
        return true;
    }
    
    bool createOpenGLView() {
        if (!window_ || !pixelFormat_) {
            return false;
        }
        
        NSRect frame = NSMakeRect(0, 0, config_.width, config_.height);
        glView_ = [[NSOpenGLView alloc] initWithFrame:frame pixelFormat:pixelFormat_];
        
        if (!glView_) {
            return false;
        }
        
        [window_ setContentView:glView_];
        return true;
    }
    
    bool createGLContext() {
        if (!glView_ || !pixelFormat_) {
            return false;
        }
        
        glContext_ = [[NSOpenGLContext alloc] initWithFormat:pixelFormat_ shareContext:nil];
        
        if (!glContext_) {
            return false;
        }
        
        [glView_ setOpenGLContext:glContext_];
        [glContext_ makeCurrentContext];
        
        // Set swap interval (vsync)
        GLint swapInterval = 1;
        [glContext_ setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
        
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
            case CursorType::DiagonalResize: return [NSCursor openHandCursor]; // Closest available
            case CursorType::NotAllowed: return [NSCursor operationNotAllowedCursor];
            case CursorType::Wait: return [NSCursor operationNotAllowedCursor]; // Closest available
            case CursorType::Help: return [NSCursor helpCursor];
            default: return [NSCursor arrowCursor];
        }
    }
    
    void processEvent(NSEvent* event) {
        WindowEvent windowEvent = {};
        
        NSEventType eventType = [event type];
        switch (eventType) {
            case NSEventTypeKeyDown:
                windowEvent.type = WindowEventType::KeyPressed;
                windowEvent.keyCode = translateMacKeyCode([event keyCode]);
                windowEvent.modifiers = translateMacModifiers([event modifierFlags]);
                break;
                
            case NSEventTypeKeyUp:
                windowEvent.type = WindowEventType::KeyReleased;
                windowEvent.keyCode = translateMacKeyCode([event keyCode]);
                windowEvent.modifiers = translateMacModifiers([event modifierFlags]);
                break;
                
            case NSEventTypeLeftMouseDown:
                windowEvent.type = WindowEventType::MouseButtonPressed;
                windowEvent.mouseButton = MouseButton::Left;
                windowEvent.x = [event locationInWindow].x;
                windowEvent.y = [event locationInWindow].y;
                windowEvent.modifiers = translateMacModifiers([event modifierFlags]);
                break;
                
            case NSEventTypeLeftMouseUp:
                windowEvent.type = WindowEventType::MouseButtonReleased;
                windowEvent.mouseButton = MouseButton::Left;
                windowEvent.x = [event locationInWindow].x;
                windowEvent.y = [event locationInWindow].y;
                windowEvent.modifiers = translateMacModifiers([event modifierFlags]);
                break;
                
            case NSEventTypeRightMouseDown:
                windowEvent.type = WindowEventType::MouseButtonPressed;
                windowEvent.mouseButton = MouseButton::Right;
                windowEvent.x = [event locationInWindow].x;
                windowEvent.y = [event locationInWindow].y;
                windowEvent.modifiers = translateMacModifiers([event modifierFlags]);
                break;
                
            case NSEventTypeRightMouseUp:
                windowEvent.type = WindowEventType::MouseButtonReleased;
                windowEvent.mouseButton = MouseButton::Right;
                windowEvent.x = [event locationInWindow].x;
                windowEvent.y = [event locationInWindow].y;
                windowEvent.modifiers = translateMacModifiers([event modifierFlags]);
                break;
                
            case NSEventTypeMouseMoved:
                windowEvent.type = WindowEventType::MouseMoved;
                windowEvent.x = [event locationInWindow].x;
                windowEvent.y = [event locationInWindow].y;
                windowEvent.modifiers = translateMacModifiers([event modifierFlags]);
                break;
                
            case NSEventTypeScrollWheel:
                windowEvent.type = WindowEventType::MouseScrolled;
                windowEvent.x = [event locationInWindow].x;
                windowEvent.y = [event locationInWindow].y;
                windowEvent.scrollDelta = [event deltaY];
                windowEvent.modifiers = translateMacModifiers([event modifierFlags]);
                break;
                
            default:
                break;
        }
        
        if (eventCallback_) {
            eventCallback_(windowEvent);
        }
    }
    
    KeyCode translateMacKeyCode(unsigned short keyCode) const {
        switch (keyCode) {
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
            case 0x07: return KeyCode::X;
            case 0x10: return KeyCode::Y;
            case 0x06: return KeyCode::Z;
            
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
            
            default: return KeyCode::Unknown;
        }
    }
    
    KeyMod translateMacModifiers(NSEventModifierFlags flags) const {
        KeyMod modifiers = KeyMod::None;
        
        if (flags & NSEventModifierFlagShift) modifiers |= KeyMod::Shift;
        if (flags & NSEventModifierFlagControl) modifiers |= KeyMod::Control;
        if (flags & NSEventModifierFlagOption) modifiers |= KeyMod::Alt;
        if (flags & NSEventModifierFlagCommand) modifiers |= KeyMod::Super;
        
        return modifiers;
    }
    
    // Window delegate class
    @interface CocoaWindowDelegate : NSObject <NSWindowDelegate> {
        CocoaWindow* window_;
    }
    
    - (id)initWithWindow:(CocoaWindow*)window;
    - (void)windowDidBecomeKey:(NSNotification*)notification;
    - (void)windowDidResignKey:(NSNotification*)notification;
    - (void)windowDidMiniaturize:(NSNotification*)notification;
    - (void)windowDidDeminiaturize:(NSNotification*)notification;
    - (void)windowDidZoom:(NSNotification*)notification;
    - (void)windowWillClose:(NSNotification*)notification;
    - (void)windowDidResize:(NSNotification*)notification;
    - (void)windowDidMove:(NSNotification*)notification;
    @end
    
    @implementation CocoaWindowDelegate
    
    - (id)initWithWindow:(CocoaWindow*)window {
        if (self = [super init]) {
            window_ = window;
        }
        return self;
    }
    
    - (void)windowDidBecomeKey:(NSNotification*)notification {
        window_->focused_ = true;
    }
    
    - (void)windowDidResignKey:(NSNotification*)notification {
        window_->focused_ = false;
    }
    
    - (void)windowDidMiniaturize:(NSNotification*)notification {
        window_->minimized_ = true;
    }
    
    - (void)windowDidDeminiaturize:(NSNotification*)notification {
        window_->minimized_ = false;
    }
    
    - (void)windowDidZoom:(NSNotification*)notification {
        window_->maximized_ = [window_->window_ isZoomed];
    }
    
    - (void)windowWillClose:(NSNotification*)notification {
        window_->shouldClose_ = true;
    }
    
    - (void)windowDidResize:(NSNotification*)notification {
        NSRect frame = [window_->window_ frame];
        window_->config_.width = static_cast<uint32>(frame.size.width);
        window_->config_.height = static_cast<uint32>(frame.size.height);
    }
    
    - (void)windowDidMove:(NSNotification*)notification {
        NSRect frame = [window_->window_ frame];
        window_->config_.x = static_cast<int32>(frame.origin.x);
        window_->config_.y = static_cast<int32>(frame.origin.y);
    }
    
    @end
};

// Window factory
Window* createWindow() {
    return new CocoaWindow();
}

void destroyWindow(Window* window) {
    delete window;
}

// Cocoa-specific functions
bool initializeCocoaWindowSystem() {
    [NSApplication sharedApplication];
    return true;
}

void shutdownCocoaWindowSystem() {
    // Cocoa cleanup is handled by window destruction
}

NSWindow* getCocoaWindow() {
    // This would typically get the window from the active window
    return nullptr;
}

NSOpenGLContext* getCocoaGLContext() {
    // This would typically get the GL context from the active window
    return nullptr;
}

} // namespace RFWindow