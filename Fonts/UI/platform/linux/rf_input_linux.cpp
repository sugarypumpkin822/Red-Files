#include "../include/rf_input.h"
#include "../include/rf_platform.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <linux/input.h>
#include <linux/joystick.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <fstream>

namespace RFInput {

using float32 = float;
using uint32 = uint32_t;
using uint8 = uint8_t;

// Custom input event structures to avoid external library dependency
typedef struct _InputEvent {
    uint32 type;
    uint32 code;
    int32 value;
    uint64 timestamp;
} InputEvent;

typedef struct _JoystickEvent {
    uint32 time;
    int16 value;
    uint8 type;
    uint8 number;
} JoystickEvent;

// Custom gamepad device structure
typedef struct _GamepadDevice {
    int fd;
    std::string path;
    std::string name;
    bool connected;
    uint32 deviceId;
    uint16 vendorId;
    uint16 productId;
    uint8 buttonCount;
    uint8 axisCount;
} GamepadDevice;

// Linux-specific input implementation with no external dependencies
class LinuxInputManager : public InputManager {
public:
    LinuxInputManager() : display_(nullptr), window_(0), keyboardState_(),
                         mouseButtonState_(), gamepadDevices_(), gamepadStates_() {
    }
    
    ~LinuxInputManager() {
        shutdown();
    }
    
    bool initialize(void* platformData) override {
        // Get X11 display and window from platform data
        if (!platformData) {
            std::cerr << "Platform data is required for Linux input" << std::endl;
            return false;
        }
        
        // Extract display and window from platform data (assuming it's a struct with display and window)
        PlatformData* data = static_cast<PlatformData*>(platformData);
        display_ = static_cast<Display*>(data->display);
        window_ = static_cast<Window>(data->window);
        
        if (!display_ || !window_) {
            std::cerr << "Invalid X11 display or window" << std::endl;
            return false;
        }
        
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
        
        // Initialize gamepad devices
        if (!initializeGamepads()) {
            std::cerr << "Failed to initialize gamepads" << std::endl;
            return false;
        }
        
        return true;
    }
    
    void shutdown() override {
        // Cleanup gamepad devices
        for (auto& device : gamepadDevices_) {
            if (device.fd >= 0) {
                close(device.fd);
                device.fd = -1;
            }
        }
        gamepadDevices_.clear();
        
        // Reset states
        std::memset(keyboardState_, 0, sizeof(keyboardState_));
        std::memset(mouseButtonState_, 0, sizeof(mouseButtonState_));
    }
    
    void update() override {
        // Store previous states
        std::memcpy(prevKeyboardState_, keyboardState_, sizeof(keyboardState_));
        std::memcpy(prevMouseButtonState_, mouseButtonState_, sizeof(mouseButtonState_));
        
        // Reset mouse delta
        mouseDelta_ = Vector2(0, 0);
        mouseScrollDelta_ = 0.0f;
        
        // Update X11 input
        updateX11Input();
        
        // Update gamepads
        updateGamepads();
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
        
        // Set cursor position using X11
        if (display_ && window_) {
            XWarpPointer(display_, None, window_, 0, 0, 0, 0, 
                        static_cast<int>(position.x), static_cast<int>(position.y));
            XFlush(display_);
        }
    }
    
    void setMouseCursorVisible(bool visible) override {
        if (visible != mouseCursorVisible_) {
            mouseCursorVisible_ = visible;
            // Note: X11 cursor visibility would require additional implementation
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
            info.name = "Linux Gamepad";
            info.vendorId = 0;
            info.productId = 0;
            info.buttonCount = 16;
            info.axisCount = 6;
            info.connected = true;
        }
        
        return info;
    }
    
    void setGamepadVibration(uint32 gamepadId, float32 lowFrequency, float32 highFrequency) override {
        // Linux doesn't have a standard vibration API
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
    
    struct PlatformData {
        void* display;
        void* window;
    };
    
    Display* display_;
    Window window_;
    
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
    std::vector<GamepadDevice> gamepadDevices_;
    GamepadState gamepadStates_[MAX_GAMEPADS];
    
    // Text input
    bool textInputActive_;
    std::string textInputText_;
    
    bool initializeGamepads() {
        // Initialize gamepad states
        for (uint32 i = 0; i < MAX_GAMEPADS; ++i) {
            gamepadStates_[i].connected = false;
            std::memset(gamepadStates_[i].buttonState, 0, sizeof(gamepadStates_[i].buttonState));
            std::memset(gamepadStates_[i].prevButtonState, 0, sizeof(gamepadStates_[i].prevButtonState));
            std::memset(gamepadStates_[i].axisState, 0, sizeof(gamepadStates_[i].axisState));
        }
        
        // Scan for joystick devices
        scanJoystickDevices();
        
        return true;
    }
    
    void scanJoystickDevices() {
        // Scan /dev/input/js* for joystick devices
        DIR* dir = opendir("/dev/input");
        if (!dir) {
            return;
        }
        
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (strncmp(entry->d_name, "js", 2) == 0) {
                std::string path = "/dev/input/" + std::string(entry->d_name);
                
                int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
                if (fd >= 0) {
                    // Get joystick info
                    char name[128];
                    if (ioctl(fd, JSIOCGNAME(sizeof(name)), name) >= 0) {
                        GamepadDevice device = {};
                        device.fd = fd;
                        device.path = path;
                        device.name = name;
                        device.connected = true;
                        device.deviceId = gamepadDevices_.size();
                        
                        // Get button and axis count
                        uint8 buttonCount = 0;
                        uint8 axisCount = 0;
                        ioctl(fd, JSIOCGBUTTONS, &buttonCount);
                        ioctl(fd, JSIOCGAXES, &axisCount);
                        device.buttonCount = buttonCount;
                        device.axisCount = axisCount;
                        
                        // Get vendor and product ID
                        if (ioctl(fd, JSIOCGVERSION, &device.vendorId) < 0) {
                            device.vendorId = 0;
                        }
                        if (ioctl(fd, JSIOCGID, &device.productId) < 0) {
                            device.productId = 0;
                        }
                        
                        gamepadDevices_.push_back(device);
                        
                        // Initialize gamepad state
                        if (device.deviceId < MAX_GAMEPADS) {
                            gamepadStates_[device.deviceId].connected = true;
                        }
                    } else {
                        close(fd);
                    }
                }
            }
        }
        
        closedir(dir);
    }
    
    void updateX11Input() {
        if (!display_ || !window_) {
            return;
        }
        
        // Process X11 events
        XEvent event;
        while (XPending(display_)) {
            XNextEvent(display_, &event);
            
            switch (event.type) {
                case KeyPress:
                    handleKeyPress(&event.xkey);
                    break;
                    
                case KeyRelease:
                    handleKeyRelease(&event.xkey);
                    break;
                    
                case ButtonPress:
                    handleButtonPress(&event.xbutton);
                    break;
                    
                case ButtonRelease:
                    handleButtonRelease(&event.xbutton);
                    break;
                    
                case MotionNotify:
                    handleMotionNotify(&event.xmotion);
                    break;
            }
        }
    }
    
    void updateGamepads() {
        // Store previous gamepad states
        for (auto& state : gamepadStates_) {
            std::memcpy(state.prevButtonState, state.buttonState, sizeof(state.buttonState));
        }
        
        // Update each gamepad
        for (auto& device : gamepadDevices_) {
            if (device.fd >= 0 && device.connected) {
                updateGamepadState(device);
            }
        }
        
        // Re-scan for new/disconnected devices periodically
        static uint32 scanCounter = 0;
        scanCounter++;
        if (scanCounter % 1000 == 0) { // Scan every 1000 updates
            scanJoystickDevices();
        }
    }
    
    void updateGamepadState(GamepadDevice& device) {
        if (device.deviceId >= MAX_GAMEPADS) {
            return;
        }
        
        GamepadState& state = gamepadStates_[device.deviceId];
        
        // Read joystick events
        JoystickEvent jsEvent;
        while (read(device.fd, &jsEvent, sizeof(jsEvent)) > 0) {
            if (jsEvent.type & JS_EVENT_BUTTON) {
                uint32 buttonIndex = jsEvent.number;
                if (buttonIndex < MAX_GAMEPAD_BUTTONS) {
                    state.buttonState[buttonIndex] = (jsEvent.value != 0);
                }
            } else if (jsEvent.type & JS_EVENT_AXIS) {
                uint32 axisIndex = jsEvent.number;
                if (axisIndex < 6) { // Limit to 6 axes
                    // Normalize axis value to [-1.0, 1.0]
                    state.axisState[axisIndex] = static_cast<float32>(jsEvent.value) / 32767.0f;
                }
            }
        }
    }
    
    void handleKeyPress(XKeyEvent* event) {
        KeyCode key = translateX11Key(event->keycode);
        if (key != KeyCode::Unknown) {
            uint32 keyIndex = static_cast<uint32>(key);
            if (keyIndex < MAX_KEYS) {
                keyboardState_[keyIndex] = true;
            }
        }
        
        // Handle text input
        if (textInputActive_) {
            char buffer[16];
            int count = XLookupString(event, buffer, sizeof(buffer), nullptr, nullptr);
            if (count > 0) {
                textInputText_.append(buffer, count);
            }
        }
    }
    
    void handleKeyRelease(XKeyEvent* event) {
        KeyCode key = translateX11Key(event->keycode);
        if (key != KeyCode::Unknown) {
            uint32 keyIndex = static_cast<uint32>(key);
            if (keyIndex < MAX_KEYS) {
                keyboardState_[keyIndex] = false;
            }
        }
    }
    
    void handleButtonPress(XButtonEvent* event) {
        uint32 buttonIndex = event->button - 1; // X11 buttons are 1-based
        if (buttonIndex < MAX_MOUSE_BUTTONS) {
            mouseButtonState_[buttonIndex] = true;
        }
        
        // Handle scroll wheel
        if (event->button == 4) { // Scroll up
            mouseScrollDelta_ = 1.0f;
        } else if (event->button == 5) { // Scroll down
            mouseScrollDelta_ = -1.0f;
        }
    }
    
    void handleButtonRelease(XButtonEvent* event) {
        uint32 buttonIndex = event->button - 1; // X11 buttons are 1-based
        if (buttonIndex < MAX_MOUSE_BUTTONS) {
            mouseButtonState_[buttonIndex] = false;
        }
    }
    
    void handleMotionNotify(XMotionEvent* event) {
        Vector2 newPosition(static_cast<float32>(event->x), static_cast<float32>(event->y));
        mouseDelta_ = newPosition - mousePosition_;
        mousePosition_ = newPosition;
    }
    
    KeyCode translateX11Key(uint32 keyCode) const {
        // Translate X11 keycode to internal KeyCode
        KeySym keysym = XKeycodeToKeysym(display_, keyCode, 0);
        
        switch (keysym) {
            case XK_a: case XK_A: return KeyCode::A;
            case XK_b: case XK_B: return KeyCode::B;
            case XK_c: case XK_C: return KeyCode::C;
            case XK_d: case XK_D: return KeyCode::D;
            case XK_e: case XK_E: return KeyCode::E;
            case XK_f: case XK_F: return KeyCode::F;
            case XK_g: case XK_G: return KeyCode::G;
            case XK_h: case XK_H: return KeyCode::H;
            case XK_i: case XK_I: return KeyCode::I;
            case XK_j: case XK_J: return KeyCode::J;
            case XK_k: case XK_K: return KeyCode::K;
            case XK_l: case XK_L: return KeyCode::L;
            case XK_m: case XK_M: return KeyCode::M;
            case XK_n: case XK_N: return KeyCode::N;
            case XK_o: case XK_O: return KeyCode::O;
            case XK_p: case XK_P: return KeyCode::P;
            case XK_q: case XK_Q: return KeyCode::Q;
            case XK_r: case XK_R: return KeyCode::R;
            case XK_s: case XK_S: return KeyCode::S;
            case XK_t: case XK_T: return KeyCode::T;
            case XK_u: case XK_U: return KeyCode::U;
            case XK_v: case XK_V: return KeyCode::V;
            case XK_w: case XK_W: return KeyCode::W;
            case XK_x: case XK_X: return KeyCode::X;
            case XK_y: case XK_Y: return KeyCode::Y;
            case XK_z: case XK_Z: return KeyCode::Z;
            
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
            
            case XK_F1: return KeyCode::F1;
            case XK_F2: return KeyCode::F2;
            case XK_F3: return KeyCode::F3;
            case XK_F4: return KeyCode::F4;
            case XK_F5: return KeyCode::F5;
            case XK_F6: return KeyCode::F6;
            case XK_F7: return KeyCode::F7;
            case XK_F8: return KeyCode::F8;
            case XK_F9: return KeyCode::F9;
            case XK_F10: return KeyCode::F10;
            case XK_F11: return KeyCode::F11;
            case XK_F12: return KeyCode::F12;
            
            case XK_Shift_L: return KeyCode::LeftShift;
            case XK_Shift_R: return KeyCode::RightShift;
            case XK_Control_L: return KeyCode::LeftControl;
            case XK_Control_R: return KeyCode::RightControl;
            case XK_Alt_L: return KeyCode::LeftAlt;
            case XK_Alt_R: return KeyCode::RightAlt;
            case XK_Super_L: return KeyCode::LeftSuper;
            case XK_Super_R: return KeyCode::RightSuper;
            
            case XK_KP_0: return KeyCode::Numpad0;
            case XK_KP_1: return KeyCode::Numpad1;
            case XK_KP_2: return KeyCode::Numpad2;
            case XK_KP_3: return KeyCode::Numpad3;
            case XK_KP_4: return KeyCode::Numpad4;
            case XK_KP_5: return KeyCode::Numpad5;
            case XK_KP_6: return KeyCode::Numpad6;
            case XK_KP_7: return KeyCode::Numpad7;
            case XK_KP_8: return KeyCode::Numpad8;
            case XK_KP_9: return KeyCode::Numpad9;
            
            case XK_KP_Multiply: return KeyCode::NumpadMultiply;
            case XK_KP_Add: return KeyCode::NumpadAdd;
            case XK_KP_Subtract: return KeyCode::NumpadSubtract;
            case XK_KP_Decimal: return KeyCode::NumpadDecimal;
            case XK_KP_Divide: return KeyCode::NumpadDivide;
            
            case XK_semicolon: return KeyCode::Semicolon;
            case XK_equal: return KeyCode::Equals;
            case XK_comma: return KeyCode::Comma;
            case XK_minus: return KeyCode::Minus;
            case XK_period: return KeyCode::Period;
            case XK_slash: return KeyCode::Slash;
            case XK_grave: return KeyCode::Grave;
            case XK_bracketleft: return KeyCode::LeftBracket;
            case XK_backslash: return KeyCode::Backslash;
            case XK_bracketright: return KeyCode::RightBracket;
            case XK_apostrophe: return KeyCode::Apostrophe;
            
            case XK_Caps_Lock: return KeyCode::CapsLock;
            case XK_Scroll_Lock: return KeyCode::ScrollLock;
            case XK_Num_Lock: return KeyCode::NumLock;
            case XK_Insert: return KeyCode::Insert;
            case XK_Home: return KeyCode::Home;
            case XK_Page_Up: return KeyCode::PageUp;
            case XK_Delete: return KeyCode::Delete;
            case XK_End: return KeyCode::End;
            case XK_Page_Down: return KeyCode::PageDown;
            
            case XK_Pause: return KeyCode::Pause;
            case XK_Sys_Req: return KeyCode::SysRq;
            case XK_Menu: return KeyCode::Apps;
            
            default: return KeyCode::Unknown;
        }
    }
};

// Input manager factory
InputManager* createInputManager() {
    return new LinuxInputManager();
}

void destroyInputManager(InputManager* manager) {
    delete manager;
}

} // namespace RFInput