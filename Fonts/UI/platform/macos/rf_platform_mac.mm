#include "../include/rf_platform.h"
#include <Cocoa/Cocoa.h>
#include <CoreFoundation/CoreFoundation.h>
#include <OpenGL/OpenGL.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>

namespace RFPlatform {

using float32 = float;
using uint32 = uint32_t;
using uint8 = uint8_t;

// Custom OpenGL definitions to avoid external library dependency
typedef struct _CGLPixelFormatAttribute {
    int32_t attribute;
} CGLPixelFormatAttribute;

typedef struct _CGLPixelFormatObj* CGLPixelFormatObj;
typedef struct _CGLContextObj* CGLContextObj;

// Custom OpenGL function prototypes
typedef CGLError (APIENTRY *CGLChoosePixelFormat_t)(const CGLPixelFormatAttribute*, CGLPixelFormatObj*, GLint*);
typedef CGLError (APIENTRY *CGLDestroyPixelFormat_t)(CGLPixelFormatObj);
typedef CGLError (APIENTRY *CGLCreateContext_t)(CGLPixelFormatObj, CGLContextObj, CGLContextObj*);
typedef CGLError (APIENTRY *CGLDestroyContext_t)(CGLContextObj);
typedef CGLError (APIENTRY *CGLSetCurrentContext_t)(CGLContextObj);
typedef CGLError (APIENTRY *CGLFlushDrawable_t)(CGLContextObj);
typedef CGLError (APIENTRY *CGLSetParameter_t)(CGLContextObj, CGLContextParameter, const GLint*);

// Custom OpenGL constants
#define kCGLPFAWindow 0x00000067
#define kCGLPFADoubleBuffer 0x00000080
#define kCGLPFARendererID 0x00000304
#define kCGLPFASingleRenderer 0x00000084
#define kCGLPFANoRecovery 0x00000082
#define kCGLPFAAccelerated 0x00000073
#define kCGLPFADepthSize 0x00000012
#define kCGLPFAColorSize 0x0000000B
#define kCGLPFAAlphaSize 0x0000000C
#define kCGLPFAStencilSize 0x0000000D
#define kCGLPFAAccumSize 0x0000000E
#define kCGLPFAAuxBuffers 0x0000000F
#define kCGLPFASampleBuffers 0x00000306
#define kCGLPFASamples 0x00000307
#define kCGLPFAOffScreen 0x00000086
#define kCGLPFAFullScreen 0x00000087
#define kCGLPFAScreenMask 0x00000088
#define kCGLPFAVirtualScreenCount 0x00000089
#define kCGLPFACompliant 0x00000308
#define kCGLPFAPBuffer 0x00000309
#define kCGLPFARemotePBuffer 0x0000030A
#define kCGLPFAWindow 0x00000067
#define kCGLPFAColorFloat 0x0000030B
#define kCGLPFAMultisample 0x0000030C
#define kCGLPFASupersample 0x0000030D
#define kCGLPFASampleAlpha 0x0000030E
#define kCGLPFASampleBuffers 0x00000306
#define kCGLPFASamples 0x00000307
#define kCGLPFARendererID 0x00000304
#define kCGLPFASingleRenderer 0x00000084
#define kCGLPFANoRecovery 0x00000082
#define kCGLPFAAccelerated 0x00000073
#define kCGLPFADepthSize 0x00000012
#define kCGLPFAColorSize 0x0000000B
#define kCGLPFAAlphaSize 0x0000000C
#define kCGLPFAStencilSize 0x0000000D
#define kCGLPFAAccumSize 0x0000000E
#define kCGLPFAAuxBuffers 0x0000000F

#define kCGLContextSwapInterval 0x00000222
#define kCGLContextSwapRectangle 0x00000221
#define kCGLContextDrawableSize 0x00000223
#define kCGLContextCurrentVirtualScreen 0x00000220
#define kCGLContextRetainRenderers 0x00000224
#define kCGLContextSurfaceOpacity 0x00000225
#define kCGLContextSurfaceOrder 0x00000226
#define kCGLContextSurfaceBackFace 0x00000227
#define kCGLContextSurfaceSurfaceID 0x00000228
#define kCGLContextSurfaceSurfaceOrder 0x00000229
#define kCGLContextSurfaceSurfaceFace 0x0000022A
#define kCGLContextSurfaceSurfaceCoverage 0x0000022B
#define kCGLContextSurfaceSurfaceVolatile 0x0000022C
#define kCGLContextSurfaceSurfaceBackingSize 0x0000022D
#define kCGLContextSurfaceSurfaceBackingScaleFactor 0x0000022E
#define kCGLContextSurfaceSurfaceBackingSizeMaximum 0x0000022F
#define kCGLContextSurfaceSurfaceBackingSizeMinimum 0x00000230

// macOS-specific platform implementation with no external dependencies
class MacOSPlatform : public Platform {
public:
    MacOSPlatform() : window_(nullptr), glPixelFormat_(nullptr), glContext_(nullptr),
                      shouldClose_(false), focused_(false), cglFunctions_() {
    }
    
    ~MacOSPlatform() {
        shutdown();
    }
    
    bool initialize(const PlatformConfig& config) override {
        config_ = config;
        
        // Initialize application
        if (!initializeApplication()) {
            std::cerr << "Failed to initialize application" << std::endl;
            return false;
        }
        
        // Create window
        if (!createWindow()) {
            std::cerr << "Failed to create window" << std::endl;
            return false;
        }
        
        // Create OpenGL context
        if (!createGLContext()) {
            std::cerr << "Failed to create OpenGL context" << std::endl;
            return false;
        }
        
        // Show window
        if (config_.windowVisible) {
            [window_ makeKeyAndOrderFront:nil];
        }
        
        return true;
    }
    
    void shutdown() override {
        // Cleanup OpenGL context
        if (glContext_ && cglFunctions_.CGLDestroyContext) {
            cglFunctions_.CGLDestroyContext(glContext_);
            glContext_ = nullptr;
        }
        
        // Cleanup pixel format
        if (glPixelFormat_ && cglFunctions_.CGLDestroyPixelFormat) {
            cglFunctions_.CGLDestroyPixelFormat(glPixelFormat_);
            glPixelFormat_ = nullptr;
        }
        
        // Cleanup window
        if (window_) {
            [window_ close];
            window_ = nullptr;
        }
        
        // Cleanup application
        if (application_) {
            [application_ release];
            application_ = nullptr;
        }
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
    
    void swapBuffers() override {
        if (glContext_ && cglFunctions_.CGLFlushDrawable) {
            cglFunctions_.CGLFlushDrawable(glContext_);
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
        return nullptr; // macOS doesn't have a display handle like X11
    }
    
    void* getGLContext() const override {
        return glContext_;
    }
    
    void setWindowSize(uint32 width, uint32 height) override {
        if (window_) {
            NSRect frame = [window_ frame];
            frame.size.width = width;
            frame.size.height = height;
            [window_ setFrame:frame display:YES];
            config_.windowWidth = width;
            config_.windowHeight = height;
        }
    }
    
    void setWindowPosition(int32 x, int32 y) override {
        if (window_) {
            NSPoint screenPoint = NSMakePoint(x, y);
            [window_ setFrameOrigin:screenPoint];
            config_.windowX = x;
            config_.windowY = y;
        }
    }
    
    void getWindowSize(uint32& width, uint32& height) const override {
        if (window_) {
            NSRect frame = [window_ frame];
            width = static_cast<uint32>(frame.size.width);
            height = static_cast<uint32>(frame.size.height);
        } else {
            width = config_.windowWidth;
            height = config_.windowHeight;
        }
    }
    
    void getWindowPosition(int32& x, int32& y) const override {
        if (window_) {
            NSPoint screenPoint = [window_ frame].origin;
            x = static_cast<int32>(screenPoint.x);
            y = static_cast<int32>(screenPoint.y);
        } else {
            x = config_.windowX;
            y = config_.windowY;
        }
    }
    
    void setWindowTitle(const std::string& title) override {
        if (window_) {
            NSString* nsTitle = [NSString stringWithUTF8String:title.c_str()];
            [window_ setTitle:nsTitle];
            config_.windowTitle = title;
        }
    }
    
    const std::string& getWindowTitle() const override {
        return config_.windowTitle;
    }
    
    void setWindowVisible(bool visible) override {
        if (window_) {
            if (visible) {
                [window_ makeKeyAndOrderFront:nil];
            } else {
                [window_ orderOut:nil];
            }
            config_.windowVisible = visible;
        }
    }
    
    bool isWindowVisible() const override {
        return config_.windowVisible;
    }
    
    void setWindowFocused(bool focused) override {
        if (window_ && focused) {
            [window_ makeKeyAndOrderFront:nil];
            focused_ = true;
        }
    }
    
    bool isWindowFocused() const override {
        return focused_;
    }
    
    void setWindowMinimized(bool minimized) override {
        if (window_) {
            if (minimized) {
                [window_ miniaturize:nil];
            } else {
                [window_ deminiaturize:nil];
            }
        }
    }
    
    bool isWindowMinimized() const override {
        return window_ ? [window_ isMiniaturized] : false;
    }
    
    void setWindowMaximized(bool maximized) override {
        if (window_) {
            if (maximized) {
                [window_ zoom:nil];
            } else {
                [window_ zoom:nil];
            }
        }
    }
    
    bool isWindowMaximized() const override {
        return window_ ? [window_ isZoomed] : false;
    }
    
    void setWindowResizable(bool resizable) override {
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
    
    bool isWindowResizable() const override {
        return window_ ? ([window_ styleMask] & NSWindowStyleMaskResizable) != 0 : true;
    }
    
    void setWindowBorderless(bool borderless) override {
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
    
    bool isWindowBorderless() const override {
        return window_ ? ([window_ styleMask] & NSWindowStyleMaskTitled) == 0 : false;
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
    
    void setCursorVisible(bool visible) override {
        if (visible) {
            [NSCursor unhide];
        } else {
            [NSCursor hide];
        }
    }
    
    bool isCursorVisible() const override {
        return CGCursorIsVisible();
    }
    
    void setClipboardText(const std::string& text) override {
        NSString* nsString = [NSString stringWithUTF8String:text.c_str()];
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
        [pasteboard clearContents];
        [pasteboard writeObjects:@[nsString]];
    }
    
    std::string getClipboardText() override {
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
        NSArray* classes = @[NSString.class];
        NSDictionary* options = nil;
        
        if ([pasteboard canReadObjectForClasses:classes options:options]) {
            NSString* nsString = [pasteboard readObjectForClasses:classes options:options].firstObject;
            return [nsString UTF8String];
        }
        
        return "";
    }
    
    void showMessageBox(const std::string& title, const std::string& message, MessageBoxType type) override {
        NSAlert* alert = [[NSAlert alloc] init];
        
        NSString* nsTitle = [NSString stringWithUTF8String:title.c_str()];
        NSString* nsMessage = [NSString stringWithUTF8String:message.c_str()];
        
        [alert setMessageText:nsTitle];
        [alert setInformativeText:nsMessage];
        
        switch (type) {
            case MessageBoxType::Info:
                [alert setAlertStyle:NSAlertStyleInformational];
                break;
            case MessageBoxType::Warning:
                [alert setAlertStyle:NSAlertStyleWarning];
                break;
            case MessageBoxType::Error:
                [alert setAlertStyle:NSAlertStyleCritical];
                break;
        }
        
        [alert runModal];
        [alert release];
    }
    
    PlatformInfo getPlatformInfo() const override {
        PlatformInfo info = {};
        
        // Get display information
        CGDirectDisplayID display = CGMainDisplayID();
        info.displayWidth = CGDisplayPixelsWide(display);
        info.displayHeight = CGDisplayPixelsHigh(display);
        info.displayDPI = 72; // macOS default DPI
        
        // Get system information
        info.platformName = "macOS";
        info.platformVersion = getMacOSVersion();
        info.cpuCount = getCPUCount();
        info.systemRAM = getSystemRAM();
        
        return info;
    }
    
    uint64 getTime() const override {
        return static_cast<uint64>(mach_absolute_time() * 1000000000ULL / mach_timebase_info().numer / mach_timebase_info().denom);
    }
    
    void sleep(uint32 milliseconds) override {
        usleep(milliseconds * 1000);
    }
    
    void openURL(const std::string& url) override {
        NSString* nsURL = [NSString stringWithUTF8String:url.c_str()];
        NSURL* urlObj = [NSURL URLWithString:nsURL];
        [[NSWorkspace sharedWorkspace] openURL:urlObj];
    }
    
    void openFile(const std::string& filePath) override {
        NSString* nsPath = [NSString stringWithUTF8String:filePath.c_str()];
        NSURL* fileURL = [NSURL fileURLWithPath:nsPath];
        [[NSWorkspace sharedWorkspace] openURL:fileURL];
    }
    
    void openFolder(const std::string& folderPath) override {
        NSString* nsPath = [NSString stringWithUTF8String:folderPath.c_str()];
        NSURL* folderURL = [NSURL fileURLWithPath:nsPath];
        [[NSWorkspace sharedWorkspace] openURL:folderURL];
    }
    
    std::string getExecutablePath() override {
        char path[PATH_MAX];
        uint32 size = PATH_MAX;
        if (_NSGetExecutablePath(path, &size) == 0) {
            return std::string(path);
        }
        return "";
    }
    
    std::string getWorkingDirectory() override {
        char path[PATH_MAX];
        if (getcwd(path, PATH_MAX)) {
            return std::string(path);
        }
        return "";
    }
    
    bool setWorkingDirectory(const std::string& path) override {
        return chdir(path.c_str()) == 0;
    }
    
    std::string getUserDataPath() override {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        NSString* appSupportDir = paths.firstObject;
        NSString* appDir = [appSupportDir stringByAppendingPathComponent:[NSString stringWithUTF8String:config_.applicationName.c_str()]];
        return [appDir UTF8String];
    }
    
    std::string getTempPath() override {
        NSString* tempDir = NSTemporaryDirectory();
        return [tempDir UTF8String];
    }
    
    std::vector<std::string> getCommandLineArguments() override {
        std::vector<std::string> args;
        
        // Get command line arguments from NSProcessInfo
        NSArray* arguments = [[NSProcessInfo processInfo] arguments];
        for (NSString* arg in arguments) {
            args.push_back([arg UTF8String]);
        }
        
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
    struct CGLFunctions {
        CGLChoosePixelFormat_t CGLChoosePixelFormat;
        CGLDestroyPixelFormat_t CGLDestroyPixelFormat;
        CGLCreateContext_t CGLCreateContext;
        CGLDestroyContext_t CGLDestroyContext;
        CGLSetCurrentContext_t CGLSetCurrentContext;
        CGLFlushDrawable_t CGLFlushDrawable;
        CGLSetParameter_t CGLSetParameter;
    };
    
    PlatformConfig config_;
    
    NSApplication* application_;
    NSWindow* window_;
    CGLPixelFormatObj glPixelFormat_;
    CGLContextObj glContext_;
    
    bool shouldClose_;
    bool focused_;
    CGLFunctions cglFunctions_;
    
    bool initializeApplication() {
        // Create application
        application_ = [[NSApplication alloc] init];
        if (!application_) {
            return false;
        }
        
        // Set activation policy
        [application_ setActivationPolicy:NSApplicationActivationPolicyRegular];
        
        // Load OpenGL framework
        if (!loadOpenGLFramework()) {
            return false;
        }
        
        return true;
    }
    
    bool createWindow() {
        // Create window
        NSRect frame = NSMakeRect(config_.windowX, config_.windowY, config_.windowWidth, config_.windowHeight);
        NSUInteger styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
        
        if (config_.windowBorderless) {
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
        NSString* title = [NSString stringWithUTF8String:config_.windowTitle.c_str()];
        [window_ setTitle:title];
        [window_ setReleasedWhenClosed:NO];
        [window_ setAcceptsMouseMovedEvents:YES];
        
        // Create delegate
        WindowDelegate* delegate = [[WindowDelegate alloc] initWithPlatform:this];
        [window_ setDelegate:delegate];
        [delegate release];
        
        return true;
    }
    
    bool createGLContext() {
        if (!loadCGLFunctions()) {
            return false;
        }
        
        // Create pixel format
        CGLPixelFormatAttribute attributes[] = {
            kCGLPFAWindow,
            kCGLPFADoubleBuffer,
            kCGLPFAAccelerated,
            kCGLPFADepthSize, 24,
            kCGLPFAColorSize, 24,
            kCGLPFAAlphaSize, 8,
            kCGLPFAStencilSize, 8,
            0
        };
        
        GLint pixelFormatCount = 0;
        CGLError error = cglFunctions_.CGLChoosePixelFormat(attributes, &glPixelFormat_, &pixelFormatCount);
        if (error != kCGLNoError || !glPixelFormat_) {
            std::cerr << "Failed to choose pixel format: " << error << std::endl;
            return false;
        }
        
        // Create context
        error = cglFunctions_.CGLCreateContext(glPixelFormat_, nullptr, &glContext_);
        if (error != kCGLNoError || !glContext_) {
            std::cerr << "Failed to create OpenGL context: " << error << std::endl;
            return false;
        }
        
        // Set current context
        error = cglFunctions_.CGLSetCurrentContext(glContext_);
        if (error != kCGLNoError) {
            std::cerr << "Failed to set current OpenGL context: " << error << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool loadOpenGLFramework() {
        // Load OpenGL framework
        CFBundleRef bundle = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));
        if (!bundle) {
            std::cerr << "Failed to load OpenGL framework" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool loadCGLFunctions() {
        // Load CGL functions dynamically
        CFBundleRef bundle = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));
        if (!bundle) {
            return false;
        }
        
        cglFunctions_.CGLChoosePixelFormat = reinterpret_cast<CGLChoosePixelFormat_t>(CFBundleGetFunctionPointerForName(bundle, CFSTR("CGLChoosePixelFormat")));
        cglFunctions_.CGLDestroyPixelFormat = reinterpret_cast<CGLDestroyPixelFormat_t>(CFBundleGetFunctionPointerForName(bundle, CFSTR("CGLDestroyPixelFormat")));
        cglFunctions_.CGLCreateContext = reinterpret_cast<CGLCreateContext_t>(CFBundleGetFunctionPointerForName(bundle, CFSTR("CGLCreateContext")));
        cglFunctions_.CGLDestroyContext = reinterpret_cast<CGLDestroyContext_t>(CFBundleGetFunctionPointerForName(bundle, CFSTR("CGLDestroyContext")));
        cglFunctions_.CGLSetCurrentContext = reinterpret_cast<CGLSetCurrentContext_t>(CFBundleGetFunctionPointerForName(bundle, CFSTR("CGLSetCurrentContext")));
        cglFunctions_.CGLFlushDrawable = reinterpret_cast<CGLFlushDrawable_t>(CFBundleGetFunctionPointerForName(bundle, CFSTR("CGLFlushDrawable")));
        cglFunctions_.CGLSetParameter = reinterpret_cast<CGLSetParameter_t>(CFBundleGetFunctionPointerForName(bundle, CFSTR("CGLSetParameter")));
        
        return cglFunctions_.CGLChoosePixelFormat && cglFunctions_.CGLDestroyPixelFormat &&
               cglFunctions_.CGLCreateContext && cglFunctions_.CGLDestroyContext &&
               cglFunctions_.CGLSetCurrentContext && cglFunctions_.CGLFlushDrawable &&
               cglFunctions_.CGLSetParameter;
    }
    
    std::string getMacOSVersion() const {
        // Get macOS version using system calls
        size_t size = 0;
        sysctlbyname("kern.osproductversion", nullptr, &size, nullptr, 0);
        
        if (size > 0) {
            std::vector<char> version(size);
            if (sysctlbyname("kern.osproductversion", version.data(), &size, nullptr, 0) == 0) {
                return std::string(version.data());
            }
        }
        
        return "Unknown";
    }
    
    uint32 getCPUCount() const {
        int count = 0;
        size_t size = sizeof(count);
        if (sysctlbyname("hw.ncpu", &count, &size, nullptr, 0) == 0) {
            return static_cast<uint32>(count);
        }
        return 1;
    }
    
    uint64 getSystemRAM() const {
        uint64 ram = 0;
        size_t size = sizeof(ram);
        if (sysctlbyname("hw.memsize", &ram, &size, nullptr, 0) == 0) {
            return ram;
        }
        return 0;
    }
    
    void handleWindowShouldClose() {
        shouldClose_ = true;
    }
    
    void handleWindowDidBecomeKey() {
        focused_ = true;
    }
    
    void handleWindowDidResignKey() {
        focused_ = false;
    }
    
    void handleWindowDidResize() {
        if (window_) {
            NSRect frame = [window_ frame];
            config_.windowWidth = static_cast<uint32>(frame.size.width);
            config_.windowHeight = static_cast<uint32>(frame.size.height);
        }
    }
    
    void handleWindowDidMove() {
        if (window_) {
            NSPoint origin = [window_ frame].origin;
            config_.windowX = static_cast<int32>(origin.x);
            config_.windowY = static_cast<int32>(origin.y);
        }
    }
};

// Window delegate class
@interface WindowDelegate : NSObject <NSWindowDelegate> {
    MacOSPlatform* platform_;
}

- (id)initWithPlatform:(MacOSPlatform*)platform;

@end

@implementation WindowDelegate

- (id)initWithPlatform:(MacOSPlatform*)platform {
    self = [super init];
    if (self) {
        platform_ = platform;
    }
    return self;
}

- (void)windowShouldClose:(NSNotification*)notification {
    platform_->handleWindowShouldClose();
}

- (void)windowDidBecomeKey:(NSNotification*)notification {
    platform_->handleWindowDidBecomeKey();
}

- (void)windowDidResignKey:(NSNotification*)notification {
    platform_->handleWindowDidResignKey();
}

- (void)windowDidResize:(NSNotification*)notification {
    platform_->handleWindowDidResize();
}

- (void)windowDidMove:(NSNotification*)notification {
    platform_->handleWindowDidMove();
}

@end

// Platform factory
Platform* createPlatform() {
    return new MacOSPlatform();
}

void destroyPlatform(Platform* platform) {
    delete platform;
}

// macOS-specific functions
bool initializeMacOSPlatform() {
    // Initialize Cocoa application
    [NSApplication sharedApplication];
    return true;
}

void shutdownMacOSPlatform() {
    // Cleanup Cocoa application
    [NSApp terminate:nil];
}

NSWindow* getMacOSWindow() {
    // This would typically get the window from the active platform
    return nullptr;
}

CGLContextObj getMacOSGLContext() {
    // This would typically get the GL context from the active platform
    return nullptr;
}

bool checkOpenGLExtension(const char* extension) {
    // Load OpenGL functions dynamically
    CFBundleRef bundle = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));
    if (!bundle) {
        return false;
    }
    
    typedef const GLubyte* (APIENTRY *glGetString_t)(GLenum);
    glGetString_t glGetString = reinterpret_cast<glGetString_t>(CFBundleGetFunctionPointerForName(bundle, CFSTR("glGetString")));
    
    if (!glGetString) {
        return false;
    }
    
    const GLubyte* extensions = glGetString(GL_EXTENSIONS);
    return extensions && strstr(reinterpret_cast<const char*>(extensions), extension) != nullptr;
}

void* getOpenGLProcAddress(const char* procname) {
    // Load OpenGL functions dynamically
    CFBundleRef bundle = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));
    if (!bundle) {
        return nullptr;
    }
    
    return CFBundleGetFunctionPointerForName(bundle, CFSTR(procname));
}

} // namespace RFPlatform