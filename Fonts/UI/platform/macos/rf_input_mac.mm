#include "../include/rf_input.h"
#include "../include/rf_platform.h"
#include <Cocoa/Cocoa.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOKitHID.h>
#include <Carbon/Carbon.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>

namespace RFInput {

using float32 = float;
using uint32 = uint32_t;
using uint8 = uint8_t;

// Custom HID definitions to avoid external library dependency
typedef struct _HIDDevice {
    io_object_t notification;
    IOHIDDeviceRef device;
    uint32 deviceId;
    bool connected;
} HIDDevice;

// Custom key code mappings
typedef struct _KeyCodeMapping {
    uint32 macKeyCode;
    KeyCode internalKeyCode;
} KeyCodeMapping;

// macOS-specific input implementation with no external dependencies
class MacOSInputManager : public InputManager {
public:
    MacOSInputManager() : hidManager_(nullptr), hidDevices_(), 
                         keyboardState_(), mouseButtonState_(), gamepadStates_() {
    }
    
    ~MacOSInputManager() {
        shutdown();
    }
    
    bool initialize(void* platformData) override {
        // Initialize input states
        std::memset(keyboardState_, 0, sizeof(keyboardState_));
        std::memset(prevKeyboardState_, 0, sizeof(prevKeyboardState_));
        std::memset(mouseButtonState_, 0, sizeof(mouseButtonState_));
        std::memset(prevMouseButtonState_, 0, sizeof(prevMouseButtonState_));
        
        mousePosition_ = Vector2(0, 0);
        mouseDelta_ = Vector2(0, 0);
        mouseScrollDelta_ = 0.0f;
        mouseCursorVisible_ = true;
        textInputActive_ = false;
        
        // Initialize HID manager
        if (!initializeHIDManager()) {
            std::cerr << "Failed to initialize HID manager" << std::endl;
            return false;
        }
        
        // Initialize keyboard monitoring
        if (!initializeKeyboardMonitoring()) {
            std::cerr << "Failed to initialize keyboard monitoring" << std::endl;
            return false;
        }
        
        // Initialize mouse monitoring
        if (!initializeMouseMonitoring()) {
            std::cerr << "Failed to initialize mouse monitoring" << std::endl;
            return false;
        }
        
        return true;
    }
    
    void shutdown() override {
        // Cleanup HID devices
        for (auto& device : hidDevices_) {
            if (device.device) {
                IOHIDDeviceUnscheduleFromRunLoop(device.device, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
                IOHIDDeviceClose(device.device, kIOHIDOptionsTypeNone);
                device.device = nullptr;
            }
        }
        
        // Cleanup HID manager
        if (hidManager_) {
            IOHIDManagerUnscheduleFromRunLoop(hidManager_, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
            IOHIDManagerClose(hidManager_, kIOHIDOptionsTypeNone);
            hidManager_ = nullptr;
        }
        
        // Cleanup keyboard monitoring
        if (keyMonitor_) {
            [NSEvent removeMonitor:keyMonitor_];
            keyMonitor_ = nil;
        }
        
        // Cleanup mouse monitoring
        if (mouseMonitor_) {
            [NSEvent removeMonitor:mouseMonitor_];
            mouseMonitor_ = nil;
        }
    }
    
    void update() override {
        // Store previous states
        std::memcpy(prevKeyboardState_, keyboardState_, sizeof(keyboardState_));
        std::memcpy(prevMouseButtonState_, mouseButtonState_, sizeof(mouseButtonState_));
        
        // Reset mouse delta
        mouseDelta_ = Vector2(0, 0);
        mouseScrollDelta_ = 0.0f;
        
        // Update HID devices
        updateHIDDevices();
        
        // Process system events
        processSystemEvents();
    }
    
    bool isKeyPressed(KeyCode key) const override {
        uint32 keyIndex = static_cast<uint32>(key);
        return keyIndex < MAX_KEYS && keyboardState_[keyIndex];
    }
    
    bool isKeyJustPressed(KeyCode key) const override {
        uint32 keyIndex = static_cast<uint32>(key);
        return keyIndex < MAX_KEYS && keyboardState_[keyIndex] && !prevKeyboardState_[keyIndex];
    }
    
    bool isKeyJustReleased(KeyCode key) const override {
        uint32 keyIndex = static_cast<uint32>(key);
        return keyIndex < MAX_KEYS && !keyboardState_[keyIndex] && prevKeyboardState_[keyIndex];
    }
    
    bool isMouseButtonPressed(MouseButton button) const override {
        uint32 buttonIndex = static_cast<uint32>(button);
        return buttonIndex < MAX_MOUSE_BUTTONS && mouseButtonState_[buttonIndex];
    }
    
    bool isMouseButtonJustPressed(MouseButton button) const override {
        uint32 buttonIndex = static_cast<uint32>(button);
        return buttonIndex < MAX_MOUSE_BUTTONS && mouseButtonState_[buttonIndex] && !prevMouseButtonState_[buttonIndex];
    }
    
    bool isMouseButtonJustReleased(MouseButton button) const override {
        uint32 buttonIndex = static_cast<uint32>(button);
        return buttonIndex < MAX_MOUSE_BUTTONS && !mouseButtonState_[buttonIndex] && prevMouseButtonState_[buttonIndex];
    }
    
    Vector2 getMousePosition() const override {
        return mousePosition_;
    }
    
    Vector2 getMouseDelta() const override {
        return mouseDelta_;
    }
    
    float32 getMouseScrollDelta() const override {
        return mouseScrollDelta_;
    }
    
    bool isGamepadConnected(uint32 gamepadId) const override {
        return gamepadId < MAX_GAMEPADS && gamepadStates_[gamepadId].connected;
    }
    
    bool isGamepadButtonPressed(uint32 gamepadId, GamepadButton button) const override {
        if (gamepadId >= MAX_GAMEPADS || !gamepadStates_[gamepadId].connected) {
            return false;
        }
        
        uint32 buttonIndex = static_cast<uint32>(button);
        return buttonIndex < MAX_GAMEPAD_BUTTONS && gamepadStates_[gamepadId].buttonState[buttonIndex];
    }
    
    bool isGamepadButtonJustPressed(uint32 gamepadId, GamepadButton button) const override {
        if (gamepadId >= MAX_GAMEPADS || !gamepadStates_[gamepadId].connected) {
            return false;
        }
        
        uint32 buttonIndex = static_cast<uint32>(button);
        bool currentPressed = gamepadStates_[gamepadId].buttonState[buttonIndex];
        bool prevPressed = gamepadStates_[gamepadId].prevButtonState[buttonIndex];
        return currentPressed && !prevPressed;
    }
    
    bool isGamepadButtonJustReleased(uint32 gamepadId, GamepadButton button) const override {
        if (gamepadId >= MAX_GAMEPADS || !gamepadStates_[gamepadId].connected) {
            return false;
        }
        
        uint32 buttonIndex = static_cast<uint32>(button);
        bool currentPressed = gamepadStates_[gamepadId].buttonState[buttonIndex];
        bool prevPressed = gamepadStates_[gamepadId].prevButtonState[buttonIndex];
        return !currentPressed && prevPressed;
    }
    
    float32 getGamepadAxis(uint32 gamepadId, GamepadAxis axis) const override {
        if (gamepadId >= MAX_GAMEPADS || !gamepadStates_[gamepadId].connected) {
            return 0.0f;
        }
        
        const GamepadState& state = gamepadStates_[gamepadId];
        
        switch (axis) {
            case GamepadAxis::LeftX: return state.axisState[0];
            case GamepadAxis::LeftY: return state.axisState[1];
            case GamepadAxis::RightX: return state.axisState[2];
            case GamepadAxis::RightY: return state.axisState[3];
            case GamepadAxis::LeftTrigger: return state.axisState[4];
            case GamepadAxis::RightTrigger: return state.axisState[5];
            default: return 0.0f;
        }
    }
    
    void setMousePosition(const Vector2& position) override {
        mousePosition_ = position;
        
        // Set cursor position using Cocoa
        NSPoint screenPoint = NSMakePoint(position.x, position.y);
        CGDirectDisplayID display = CGMainDisplayID();
        CGPoint cgPoint = CGPointMake(screenPoint.x, CGDisplayPixelsHigh(display) - screenPoint.y);
        CGWarpMouseCursorPosition(cgPoint);
        CGAssociateMouseAndMouseCursorPosition(true);
    }
    
    void setMouseCursorVisible(bool visible) override {
        if (visible != mouseCursorVisible_) {
            if (visible) {
                [NSCursor unhide];
            } else {
                [NSCursor hide];
            }
            mouseCursorVisible_ = visible;
        }
    }
    
    bool isMouseCursorVisible() const override {
        return mouseCursorVisible_;
    }
    
    void startTextInput() override {
        textInputActive_ = true;
    }
    
    void stopTextInput() override {
        textInputActive_ = false;
    }
    
    bool isTextInputActive() const override {
        return textInputActive_;
    }
    
    const std::string& getTextInputText() const override {
        return textInputText_;
    }
    
    void setTextInputText(const std::string& text) override {
        textInputText_ = text;
    }
    
    void clearTextInput() override {
        textInputText_.clear();
    }
    
    uint32 getConnectedGamepadCount() const override {
        uint32 count = 0;
        for (const auto& state : gamepadStates_) {
            if (state.connected) {
                count++;
            }
        }
        return count;
    }
    
    GamepadInfo getGamepadInfo(uint32 gamepadId) const override {
        GamepadInfo info = {};
        
        if (gamepadId < MAX_GAMEPADS && gamepadStates_[gamepadId].connected) {
            info.name = "HID Gamepad";
            info.vendorId = 0;
            info.productId = 0;
            info.buttonCount = 16;
            info.axisCount = 6;
            info.connected = true;
        }
        
        return info;
    }
    
    void setGamepadVibration(uint32 gamepadId, float32 lowFrequency, float32 highFrequency) override {
        // macOS doesn't support gamepad vibration through standard APIs
        // This would require platform-specific implementation for each controller type
    }
    
private:
    static const uint32 MAX_GAMEPADS = 4;
    
    struct GamepadState {
        bool connected;
        bool buttonState[MAX_GAMEPAD_BUTTONS];
        bool prevButtonState[MAX_GAMEPAD_BUTTONS];
        float32 axisState[6]; // Left X/Y, Right X/Y, Left/Right Trigger
    };
    
    IOHIDManagerRef hidManager_;
    std::vector<HIDDevice> hidDevices_;
    
    id keyMonitor_;
    id mouseMonitor_;
    
    // Input state
    bool keyboardState_[MAX_KEYS];
    bool prevKeyboardState_[MAX_KEYS];
    bool mouseButtonState_[MAX_MOUSE_BUTTONS];
    bool prevMouseButtonState_[MAX_MOUSE_BUTTONS];
    Vector2 mousePosition_;
    Vector2 mouseDelta_;
    float32 mouseScrollDelta_;
    bool mouseCursorVisible_;
    
    // Gamepad state
    GamepadState gamepadStates_[MAX_GAMEPADS];
    
    // Text input
    bool textInputActive_;
    std::string textInputText_;
    
    // Key code mappings
    static const KeyCodeMapping keyCodeMappings_[];
    
    bool initializeHIDManager() {
        // Create HID manager
        hidManager_ = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
        if (!hidManager_) {
            return false;
        }
        
        // Set device matching criteria for gamepads
        CFMutableDictionaryRef matchingDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        
        // Match gamepad usage page and usage
        CFNumberRef usagePage = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &(int){kHIDPage_GenericDesktop});
        CFNumberRef usage = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &(int){kHIDUsage_GD_GamePad});
        
        CFDictionarySetValue(matchingDict, CFSTR(kIOHIDDeviceUsagePageKey), usagePage);
        CFDictionarySetValue(matchingDict, CFSTR(kIOHIDDeviceUsageKey), usage);
        
        IOHIDManagerSetDeviceMatching(hidManager_, matchingDict);
        
        // Set callbacks
        IOHIDManagerRegisterDeviceMatchingCallback(hidManager_, deviceMatchingCallback, this);
        IOHIDManagerRegisterDeviceRemovalCallback(hidManager_, deviceRemovalCallback, this);
        
        // Schedule on run loop
        IOHIDManagerScheduleWithRunLoop(hidManager_, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        
        // Open manager
        IOReturn result = IOHIDManagerOpen(hidManager_, kIOHIDOptionsTypeNone);
        if (result != kIOReturnSuccess) {
            return false;
        }
        
        // Clean up
        CFRelease(usagePage);
        CFRelease(usage);
        CFRelease(matchingDict);
        
        return true;
    }
    
    bool initializeKeyboardMonitoring() {
        keyMonitor_ = [NSEvent addGlobalMonitorForEventsMatchingMask:NSEventMaskKeyDown
                                                              handler:^(NSEvent* event) {
                                                                  this->handleKeyEvent(event, true);
                                                              }];
        
        if (!keyMonitor_) {
            return false;
        }
        
        // Also monitor key up events
        id keyUpMonitor = [NSEvent addGlobalMonitorForEventsMatchingMask:NSEventMaskKeyUp
                                                                 handler:^(NSEvent* event) {
                                                                     this->handleKeyEvent(event, false);
                                                                 }];
        
        return keyUpMonitor != nil;
    }
    
    bool initializeMouseMonitoring() {
        mouseMonitor_ = [NSEvent addGlobalMonitorForEventsMatchingMask:NSEventMaskMouseMoved | NSEventMaskLeftMouseDown | NSEventMaskLeftMouseUp | NSEventMaskRightMouseDown | NSEventMaskRightMouseUp | NSEventMaskOtherMouseDown | NSEventMaskOtherMouseUp | NSEventMaskScrollWheel
                                                                handler:^(NSEvent* event) {
                                                                    this->handleMouseEvent(event);
                                                                }];
        
        return mouseMonitor_ != nil;
    }
    
    void updateHIDDevices() {
        // Store previous gamepad states
        for (auto& state : gamepadStates_) {
            std::memcpy(state.prevButtonState, state.buttonState, sizeof(state.buttonState));
        }
        
        // Update connected devices
        for (auto& device : hidDevices_) {
            if (device.device && device.connected) {
                updateGamepadState(device);
            }
        }
    }
    
    void processSystemEvents() {
        // Process pending Cocoa events
        while (true) {
            NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                untilDate:[NSDate distantPast]
                                                   inMode:NSDefaultRunLoopMode
                                                  dequeue:YES];
            if (!event) {
                break;
            }
            
            [NSApp sendEvent:event];
        }
    }
    
    void handleKeyEvent(NSEvent* event, bool pressed) {
        uint32 keyCode = [event keyCode];
        KeyCode internalKey = translateMacKeyCode(keyCode);
        
        if (internalKey != KeyCode::Unknown) {
            uint32 keyIndex = static_cast<uint32>(internalKey);
            if (keyIndex < MAX_KEYS) {
                keyboardState_[keyIndex] = pressed;
            }
        }
        
        // Handle text input
        if (textInputActive_ && pressed) {
            NSString* characters = [event characters];
            if (characters && [characters length] > 0) {
                textInputText_ += [characters UTF8String];
            }
        }
    }
    
    void handleMouseEvent(NSEvent* event) {
        NSEventType eventType = [event type];
        NSPoint location = [event locationInWindow];
        
        switch (eventType) {
            case NSEventTypeMouseMoved:
            case NSEventTypeLeftMouseDragged:
            case NSEventTypeRightMouseDragged:
            case NSEventTypeOtherMouseDragged:
                {
                    Vector2 newPosition(static_cast<float32>(location.x), static_cast<float32>(location.y));
                    mouseDelta_ = newPosition - mousePosition_;
                    mousePosition_ = newPosition;
                }
                break;
                
            case NSEventTypeLeftMouseDown:
                mouseButtonState_[0] = true;
                break;
                
            case NSEventTypeLeftMouseUp:
                mouseButtonState_[0] = false;
                break;
                
            case NSEventTypeRightMouseDown:
                mouseButtonState_[1] = true;
                break;
                
            case NSEventTypeRightMouseUp:
                mouseButtonState_[1] = false;
                break;
                
            case NSEventTypeOtherMouseDown:
                {
                    uint32 buttonNumber = [event buttonNumber];
                    if (buttonNumber < MAX_MOUSE_BUTTONS) {
                        mouseButtonState_[buttonNumber] = true;
                    }
                }
                break;
                
            case NSEventTypeOtherMouseUp:
                {
                    uint32 buttonNumber = [event buttonNumber];
                    if (buttonNumber < MAX_MOUSE_BUTTONS) {
                        mouseButtonState_[buttonNumber] = false;
                    }
                }
                break;
                
            case NSEventTypeScrollWheel:
                {
                    float32 deltaX = [event scrollingDeltaX];
                    float32 deltaY = [event scrollingDeltaY];
                    
                    if ([event hasPreciseScrollingDeltas]) {
                        mouseScrollDelta_ = deltaY * 0.1f;
                    } else {
                        mouseScrollDelta_ = deltaY;
                    }
                }
                break;
                
            default:
                break;
        }
    }
    
    void updateGamepadState(HIDDevice& device) {
        // Get gamepad state from HID device
        // This is a simplified implementation - in reality you'd need to parse HID reports
        
        // For now, we'll simulate some basic gamepad input
        // In a real implementation, you'd use IOHIDValueGetValue to read actual values
        
        uint32 deviceId = device.deviceId;
        if (deviceId < MAX_GAMEPADS) {
            GamepadState& state = gamepadStates_[deviceId];
            
            // Simulate some basic axis values (in reality, read from HID device)
            state.axisState[0] = 0.0f; // Left X
            state.axisState[1] = 0.0f; // Left Y
            state.axisState[2] = 0.0f; // Right X
            state.axisState[3] = 0.0f; // Right Y
            state.axisState[4] = 0.0f; // Left Trigger
            state.axisState[5] = 0.0f; // Right Trigger
        }
    }
    
    KeyCode translateMacKeyCode(uint32 macKeyCode) const {
        for (const auto& mapping : keyCodeMappings_) {
            if (mapping.macKeyCode == macKeyCode) {
                return mapping.internalKeyCode;
            }
        }
        return KeyCode::Unknown;
    }
    
    static void deviceMatchingCallback(void* context, IOReturn result, void* sender, IOHIDDeviceRef device) {
        MacOSInputManager* manager = static_cast<MacOSInputManager*>(context);
        manager->handleDeviceMatching(device);
    }
    
    static void deviceRemovalCallback(void* context, IOReturn result, void* sender, IOHIDDeviceRef device) {
        MacOSInputManager* manager = static_cast<MacOSInputManager*>(context);
        manager->handleDeviceRemoval(device);
    }
    
    void handleDeviceMatching(IOHIDDeviceRef device) {
        // Find available slot
        for (uint32 i = 0; i < MAX_GAMEPADS; ++i) {
            if (!gamepadStates_[i].connected) {
                // Add device
                HIDDevice hidDevice = {};
                hidDevice.device = device;
                hidDevice.deviceId = i;
                hidDevice.connected = true;
                
                hidDevices_.push_back(hidDevice);
                
                // Initialize gamepad state
                gamepadStates_[i].connected = true;
                std::memset(gamepadStates_[i].buttonState, 0, sizeof(gamepadStates_[i].buttonState));
                std::memset(gamepadStates_[i].prevButtonState, 0, sizeof(gamepadStates_[i].prevButtonState));
                std::memset(gamepadStates_[i].axisState, 0, sizeof(gamepadStates_[i].axisState));
                
                // Schedule device on run loop
                IOHIDDeviceScheduleWithRunLoop(device, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
                
                break;
            }
        }
    }
    
    void handleDeviceRemoval(IOHIDDeviceRef device) {
        // Find and remove device
        for (auto it = hidDevices_.begin(); it != hidDevices_.end(); ++it) {
            if (it->device == device) {
                // Unschedule device
                IOHIDDeviceUnscheduleFromRunLoop(device, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
                
                // Update gamepad state
                gamepadStates_[it->deviceId].connected = false;
                
                // Remove device
                hidDevices_.erase(it);
                break;
            }
        }
    }
};

// Key code mappings
const KeyCodeMapping MacOSInputManager::keyCodeMappings_[] = {
    {0x00, KeyCode::A}, {0x0B, KeyCode::B}, {0x08, KeyCode::C}, {0x02, KeyCode::D},
    {0x0E, KeyCode::E}, {0x03, KeyCode::F}, {0x05, KeyCode::G}, {0x04, KeyCode::H},
    {0x22, KeyCode::I}, {0x26, KeyCode::J}, {0x28, KeyCode::K}, {0x25, KeyCode::L},
    {0x2E, KeyCode::M}, {0x2D, KeyCode::N}, {0x1F, KeyCode::O}, {0x23, KeyCode::P},
    {0x0C, KeyCode::Q}, {0x0F, KeyCode::R}, {0x01, KeyCode::S}, {0x11, KeyCode::T},
    {0x20, KeyCode::U}, {0x09, KeyCode::V}, {0x0D, KeyCode::W}, {0x10, KeyCode::X},
    {0x1E, KeyCode::Y}, {0x21, KeyCode::Z},
    {0x1D, KeyCode::Num0}, {0x12, KeyCode::Num1}, {0x13, KeyCode::Num2}, {0x14, KeyCode::Num3},
    {0x15, KeyCode::Num4}, {0x17, KeyCode::Num5}, {0x16, KeyCode::Num6}, {0x1A, KeyCode::Num7},
    {0x1C, KeyCode::Num8}, {0x19, KeyCode::Num9},
    {0x24, KeyCode::Enter}, {0x35, KeyCode::Escape}, {0x33, KeyCode::Backspace},
    {0x30, KeyCode::Tab}, {0x31, KeyCode::Space},
    {0x7B, KeyCode::Left}, {0x7C, KeyCode::Right}, {0x7E, KeyCode::Up}, {0x7D, KeyCode::Down},
    {0x7A, KeyCode::F1}, {0x78, KeyCode::F2}, {0x63, KeyCode::F3}, {0x76, KeyCode::F4},
    {0x60, KeyCode::F5}, {0x61, KeyCode::F6}, {0x62, KeyCode::F7}, {0x64, KeyCode::F8},
    {0x65, KeyCode::F9}, {0x6D, KeyCode::F10}, {0x67, KeyCode::F11}, {0x6F, KeyCode::F12},
    {0x38, KeyCode::LeftShift}, {0x3C, KeyCode::RightShift},
    {0x3B, KeyCode::LeftControl}, {0x3E, KeyCode::RightControl},
    {0x3A, KeyCode::LeftAlt}, {0x3D, KeyCode::RightAlt},
    {0x37, KeyCode::LeftSuper}, {0x36, KeyCode::RightSuper},
    {0x52, KeyCode::Numpad0}, {0x53, KeyCode::Numpad1}, {0x54, KeyCode::Numpad2},
    {0x55, KeyCode::Numpad3}, {0x56, KeyCode::Numpad4}, {0x57, KeyCode::Numpad5},
    {0x58, KeyCode::Numpad6}, {0x59, KeyCode::Numpad7}, {0x5B, KeyCode::Numpad8},
    {0x5C, KeyCode::Numpad9}, {0x45, KeyCode::NumpadMultiply}, {0x4E, KeyCode::NumpadAdd},
    {0x4A, KeyCode::NumpadSubtract}, {0x41, KeyCode::NumpadDecimal}, {0x4B, KeyCode::NumpadDivide},
    {0x29, KeyCode::Semicolon}, {0x18, KeyCode::Equals}, {0x2B, KeyCode::Comma},
    {0x1B, KeyCode::Minus}, {0x2F, KeyCode::Period}, {0x2C, KeyCode::Slash},
    {0x32, KeyCode::Grave}, {0x21, KeyCode::LeftBracket}, {0x2A, KeyCode::Backslash},
    {0x1E, KeyCode::RightBracket}, {0x27, KeyCode::Apostrophe},
    {0x39, KeyCode::CapsLock}, {0x6B, KeyCode::ScrollLock}, {0x47, KeyCode::NumLock},
    {0x72, KeyCode::Insert}, {0x73, KeyCode::Home}, {0x74, KeyCode::PageUp},
    {0x75, KeyCode::Delete}, {0x77, KeyCode::End}, {0x79, KeyCode::PageDown},
    {0x71, KeyCode::Pause}, {0x69, KeyCode::SysRq}, {0x6E, KeyCode::Apps}
};

// Input manager factory
InputManager* createInputManager() {
    return new MacOSInputManager();
}

void destroyInputManager(InputManager* manager) {
    delete manager;
}

} // namespace RFInput