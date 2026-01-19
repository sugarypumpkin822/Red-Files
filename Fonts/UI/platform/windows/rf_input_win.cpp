#include "../include/rf_input.h"
#include "../include/rf_platform.h"
#include <windows.h>
#include <windowsx.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iostream>

namespace RFInput {

using float32 = float;
using uint32 = uint32_t;
using uint8 = uint8_t;

// Custom XInput definitions to avoid external library dependency
#define XINPUT_GAMEPAD_DPAD_UP          0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x0008
#define XINPUT_GAMEPAD_START            0x0010
#define XINPUT_GAMEPAD_BACK             0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
#define XINPUT_GAMEPAD_A                0x1000
#define XINPUT_GAMEPAD_B                0x2000
#define XINPUT_GAMEPAD_X                0x4000
#define XINPUT_GAMEPAD_Y                0x8000

typedef struct _XINPUT_GAMEPAD {
    WORD wButtons;
    BYTE bLeftTrigger;
    BYTE bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
} XINPUT_GAMEPAD;

typedef struct _XINPUT_STATE {
    DWORD dwPacketNumber;
    XINPUT_GAMEPAD Gamepad;
} XINPUT_STATE;

typedef struct _XINPUT_VIBRATION {
    WORD wLeftMotorSpeed;
    WORD wRightMotorSpeed;
} XINPUT_VIBRATION;

typedef struct _XINPUT_CAPABILITIES {
    BYTE Type;
    BYTE SubType;
    WORD Flags;
    XINPUT_GAMEPAD Gamepad;
    WORD Vibration;
    WORD LeftTriggerMotor;
    WORD RightTriggerMotor;
} XINPUT_CAPABILITIES;

#define XINPUT_DEVTYPE_GAMEPAD          0x01
#define XINPUT_DEVSUBTYPE_GAMEPAD       0x01

// Custom DirectInput key codes to avoid external library dependency
#define DIK_A          0x1E
#define DIK_B          0x30
#define DIK_C          0x2E
#define DIK_D          0x20
#define DIK_E          0x12
#define DIK_F          0x21
#define DIK_G          0x22
#define DIK_H          0x23
#define DIK_I          0x17
#define DIK_J          0x24
#define DIK_K          0x25
#define DIK_L          0x26
#define DIK_M          0x32
#define DIK_N          0x31
#define DIK_O          0x18
#define DIK_P          0x19
#define DIK_Q          0x10
#define DIK_R          0x13
#define DIK_S          0x1F
#define DIK_T          0x14
#define DIK_U          0x16
#define DIK_V          0x2F
#define DIK_W          0x11
#define DIK_X          0x2D
#define DIK_Y          0x15
#define DIK_Z          0x2C
#define DIK_1          0x02
#define DIK_2          0x03
#define DIK_3          0x04
#define DIK_4          0x05
#define DIK_5          0x06
#define DIK_6          0x07
#define DIK_7          0x08
#define DIK_8          0x09
#define DIK_9          0x0A
#define DIK_0          0x0B
#define DIK_RETURN     0x1C
#define DIK_ESCAPE     0x01
#define DIK_BACK       0x0E
#define DIK_TAB        0x0F
#define DIK_SPACE      0x39
#define DIK_LEFT       0xCB
#define DIK_RIGHT      0xCD
#define DIK_UP         0xC8
#define DIK_DOWN       0xD0
#define DIK_F1         0x3B
#define DIK_F2         0x3C
#define DIK_F3         0x3D
#define DIK_F4         0x3E
#define DIK_F5         0x3F
#define DIK_F6         0x40
#define DIK_F7         0x41
#define DIK_F8         0x42
#define DIK_F9         0x43
#define DIK_F10        0x44
#define DIK_F11        0x57
#define DIK_F12        0x58
#define DIK_LSHIFT     0x2A
#define DIK_RSHIFT     0x36
#define DIK_LCONTROL   0x1D
#define DIK_RCONTROL   0x9D
#define DIK_LALT       0x38
#define DIK_RALT       0xB8
#define DIK_LWIN       0x5B
#define DIK_RWIN       0x5C
#define DIK_NUMPAD0    0x52
#define DIK_NUMPAD1    0x4F
#define DIK_NUMPAD2    0x50
#define DIK_NUMPAD3    0x51
#define DIK_NUMPAD4    0x4B
#define DIK_NUMPAD5    0x4C
#define DIK_NUMPAD6    0x4D
#define DIK_NUMPAD7    0x47
#define DIK_NUMPAD8    0x48
#define DIK_NUMPAD9    0x49
#define DIK_MULTIPLY   0x37
#define DIK_ADD        0x4E
#define DIK_SUBTRACT   0x4A
#define DIK_DECIMAL    0x53
#define DIK_DIVIDE     0xB5
#define DIK_SEMICOLON  0x27
#define DIK_EQUALS     0x0D
#define DIK_COMMA      0x33
#define DIK_MINUS      0x0C
#define DIK_PERIOD     0x34
#define DIK_SLASH      0x35
#define DIK_GRAVE      0x29
#define DIK_LBRACKET   0x1A
#define DIK_BACKSLASH  0x2B
#define DIK_RBRACKET   0x1B
#define DIK_APOSTROPHE 0x28
#define DIK_CAPITAL    0x3A
#define DIK_SCROLL     0x46
#define DIK_NUMLOCK    0x45
#define DIK_INSERT     0xD2
#define DIK_HOME       0xC7
#define DIK_PRIOR      0xC9
#define DIK_DELETE     0xD3
#define DIK_END        0xCF
#define DIK_NEXT       0xD1
#define DIK_PAUSE      0xC1
#define DIK_SYSRQ      0x54
#define DIK_APPS       0xDD

// Custom DirectInput format definitions
typedef struct _DIKEYBOARDSTATE {
    BYTE rgbKeys[256];
} DIKEYBOARDSTATE;

// Custom XInput function prototypes
typedef DWORD (WINAPI *XInputGetState_t)(DWORD dwUserIndex, XINPUT_STATE* pState);
typedef DWORD (WINAPI *XInputSetState_t)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
typedef DWORD (WINAPI *XInputGetCapabilities_t)(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities);
typedef void (WINAPI *XInputEnable_t)(BOOL enable);

#define ERROR_SUCCESS 0
#define ERROR_DEVICE_NOT_CONNECTED 1167
#define ERROR_EMPTY 4306

// Windows-specific input implementation with no external dependencies
class WindowsInputManager : public InputManager {
public:
    WindowsInputManager() : xinputGetState_(nullptr), xinputSetState_(nullptr), 
                           xinputGetCapabilities_(nullptr), xinputEnable_(nullptr),
                           xinputState_(), gamepadConnected_() {
    }
    
    ~WindowsInputManager() {
        shutdown();
    }
    
    bool initialize(void* platformData) override {
        // Initialize keyboard and mouse state
        std::memset(keyboardState_, 0, sizeof(keyboardState_));
        std::memset(prevKeyboardState_, 0, sizeof(prevKeyboardState_));
        std::memset(mouseButtonState_, 0, sizeof(mouseButtonState_));
        std::memset(prevMouseButtonState_, 0, sizeof(prevMouseButtonState_));
        
        mousePosition_ = Vector2(0, 0);
        mouseDelta_ = Vector2(0, 0);
        mouseScrollDelta_ = 0.0f;
        mouseCursorVisible_ = true;
        textInputActive_ = false;
        
        // Initialize XInput dynamically
        if (!initializeXInput()) {
            std::cerr << "Failed to initialize XInput" << std::endl;
            // Continue without XInput - keyboard and mouse will still work
        }
        
        return true;
    }
    
    void shutdown() override {
        // Cleanup XInput
        if (xinputEnable_) {
            xinputEnable_(FALSE);
        }
        
        xinputGetState_ = nullptr;
        xinputSetState_ = nullptr;
        xinputGetCapabilities_ = nullptr;
        xinputEnable_ = nullptr;
    }
    
    void update() override {
        // Store previous states
        std::memcpy(prevKeyboardState_, keyboardState_, sizeof(keyboardState_));
        std::memcpy(prevMouseButtonState_, mouseButtonState_, sizeof(mouseButtonState_));
        
        // Reset mouse delta
        mouseDelta_ = Vector2(0, 0);
        mouseScrollDelta_ = 0.0f;
        
        // Update keyboard and mouse using Windows API
        updateKeyboardMouse();
        
        // Update XInput
        updateXInput();
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
        return gamepadId < MAX_GAMEPADS && gamepadConnected_[gamepadId];
    }
    
    bool isGamepadButtonPressed(uint32 gamepadId, GamepadButton button) const override {
        if (gamepadId >= MAX_GAMEPADS || !gamepadConnected_[gamepadId]) {
            return false;
        }
        
        uint32 buttonIndex = static_cast<uint32>(button);
        return buttonIndex < MAX_GAMEPAD_BUTTONS && 
               (xinputState_[gamepadId].Gamepad.wButtons & (1 << buttonIndex)) != 0;
    }
    
    bool isGamepadButtonJustPressed(uint32 gamepadId, GamepadButton button) const override {
        if (gamepadId >= MAX_GAMEPADS || !gamepadConnected_[gamepadId]) {
            return false;
        }
        
        uint32 buttonIndex = static_cast<uint32>(button);
        bool currentPressed = (xinputState_[gamepadId].Gamepad.wButtons & (1 << buttonIndex)) != 0;
        bool prevPressed = (prevXinputState_[gamepadId].Gamepad.wButtons & (1 << buttonIndex)) != 0;
        return currentPressed && !prevPressed;
    }
    
    bool isGamepadButtonJustReleased(uint32 gamepadId, GamepadButton button) const override {
        if (gamepadId >= MAX_GAMEPADS || !gamepadConnected_[gamepadId]) {
            return false;
        }
        
        uint32 buttonIndex = static_cast<uint32>(button);
        bool currentPressed = (xinputState_[gamepadId].Gamepad.wButtons & (1 << buttonIndex)) != 0;
        bool prevPressed = (prevXinputState_[gamepadId].Gamepad.wButtons & (1 << buttonIndex)) != 0;
        return !currentPressed && prevPressed;
    }
    
    float32 getGamepadAxis(uint32 gamepadId, GamepadAxis axis) const override {
        if (gamepadId >= MAX_GAMEPADS || !gamepadConnected_[gamepadId]) {
            return 0.0f;
        }
        
        const XINPUT_STATE& state = xinputState_[gamepadId];
        
        switch (axis) {
            case GamepadAxis::LeftX:
                return normalizeAxisValue(state.Gamepad.sThumbLX);
            case GamepadAxis::LeftY:
                return normalizeAxisValue(state.Gamepad.sThumbLY);
            case GamepadAxis::RightX:
                return normalizeAxisValue(state.Gamepad.sThumbRX);
            case GamepadAxis::RightY:
                return normalizeAxisValue(state.Gamepad.sThumbRY);
            case GamepadAxis::LeftTrigger:
                return normalizeTriggerValue(state.Gamepad.bLeftTrigger);
            case GamepadAxis::RightTrigger:
                return normalizeTriggerValue(state.Gamepad.bRightTrigger);
            default:
                return 0.0f;
        }
    }
    
    void setMousePosition(const Vector2& position) override {
        mousePosition_ = position;
        
        // Set cursor position using Windows API
        SetCursorPos(static_cast<int32>(position.x), static_cast<int32>(position.y));
    }
    
    void setMouseCursorVisible(bool visible) override {
        if (visible != mouseCursorVisible_) {
            ShowCursor(visible);
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
        for (bool connected : gamepadConnected_) {
            if (connected) {
                count++;
            }
        }
        return count;
    }
    
    GamepadInfo getGamepadInfo(uint32 gamepadId) const override {
        GamepadInfo info = {};
        
        if (gamepadId < MAX_GAMEPADS && gamepadConnected_[gamepadId] && xinputGetCapabilities_) {
            XINPUT_CAPABILITIES caps = {};
            if (xinputGetCapabilities_(gamepadId, 0, &caps) == ERROR_SUCCESS) {
                info.name = "XInput Controller";
                info.vendorId = 0x045E; // Microsoft
                info.productId = 0x028E; // Xbox Controller
                info.buttonCount = 16; // XInput standard
                info.axisCount = 6; // 2 sticks + 2 triggers
                info.connected = true;
            }
        }
        
        return info;
    }
    
    void setGamepadVibration(uint32 gamepadId, float32 lowFrequency, float32 highFrequency) override {
        if (gamepadId >= MAX_GAMEPADS || !gamepadConnected_[gamepadId] || !xinputSetState_) {
            return;
        }
        
        XINPUT_VIBRATION vibration = {};
        vibration.wLeftMotorSpeed = static_cast<uint16>(lowFrequency * 65535.0f);
        vibration.wRightMotorSpeed = static_cast<uint16>(highFrequency * 65535.0f);
        
        xinputSetState_(gamepadId, &vibration);
    }
    
private:
    static const uint32 MAX_GAMEPADS = 4;
    
    XInputGetState_t xinputGetState_;
    XInputSetState_t xinputSetState_;
    XInputGetCapabilities_t xinputGetCapabilities_;
    XInputEnable_t xinputEnable_;
    
    XINPUT_STATE xinputState_[MAX_GAMEPADS];
    XINPUT_STATE prevXinputState_[MAX_GAMEPADS];
    bool gamepadConnected_[MAX_GAMEPADS];
    
    // Input state
    bool keyboardState_[MAX_KEYS];
    bool prevKeyboardState_[MAX_KEYS];
    bool mouseButtonState_[MAX_MOUSE_BUTTONS];
    bool prevMouseButtonState_[MAX_MOUSE_BUTTONS];
    Vector2 mousePosition_;
    Vector2 mouseDelta_;
    float32 mouseScrollDelta_;
    bool mouseCursorVisible_;
    
    // Text input
    bool textInputActive_;
    std::string textInputText_;
    
    bool initializeXInput() {
        // Try to load XInput 1.4 first
        HMODULE xinputLib = LoadLibraryA("xinput1_4.dll");
        if (!xinputLib) {
            // Fall back to XInput 1.3
            xinputLib = LoadLibraryA("xinput1_3.dll");
        }
        if (!xinputLib) {
            // Fall back to XInput 9.1.0
            xinputLib = LoadLibraryA("xinput9_1_0.dll");
        }
        if (!xinputLib) {
            return false;
        }
        
        xinputGetState_ = reinterpret_cast<XInputGetState_t>(GetProcAddress(xinputLib, "XInputGetState"));
        xinputSetState_ = reinterpret_cast<XInputSetState_t>(GetProcAddress(xinputLib, "XInputSetState"));
        xinputGetCapabilities_ = reinterpret_cast<XInputGetCapabilities_t>(GetProcAddress(xinputLib, "XInputGetCapabilities"));
        xinputEnable_ = reinterpret_cast<XInputEnable_t>(GetProcAddress(xinputLib, "XInputEnable"));
        
        if (!xinputGetState_ || !xinputSetState_ || !xinputGetCapabilities_ || !xinputEnable_) {
            FreeLibrary(xinputLib);
            return false;
        }
        
        // Enable XInput
        xinputEnable_(TRUE);
        
        // Initialize gamepad states
        std::memset(xinputState_, 0, sizeof(xinputState_));
        std::memset(prevXinputState_, 0, sizeof(prevXinputState_));
        std::memset(gamepadConnected_, 0, sizeof(gamepadConnected_));
        
        // Check for connected gamepads
        for (uint32 i = 0; i < MAX_GAMEPADS; ++i) {
            if (xinputGetState_(i, &xinputState_[i]) == ERROR_SUCCESS) {
                gamepadConnected_[i] = true;
                prevXinputState_[i] = xinputState_[i];
            }
        }
        
        return true;
    }
    
    void updateKeyboardMouse() {
        // Get keyboard state using Windows API
        BYTE keyboardState[256];
        if (GetKeyboardState(keyboardState)) {
            // Map Windows key state to our key state
            for (uint32 i = 0; i < 256; ++i) {
                KeyCode keyCode = translateWindowsKey(i);
                if (keyCode != KeyCode::Unknown) {
                    uint32 keyIndex = static_cast<uint32>(keyCode);
                    if (keyIndex < MAX_KEYS) {
                        keyboardState_[keyIndex] = (keyboardState[i] & 0x80) != 0;
                    }
                }
            }
        }
        
        // Get mouse position
        POINT cursorPos;
        if (GetCursorPos(&cursorPos)) {
            Vector2 newPosition(static_cast<float32>(cursorPos.x), static_cast<float32>(cursorPos.y));
            mouseDelta_ = newPosition - mousePosition_;
            mousePosition_ = newPosition;
        }
        
        // Get mouse button state
        uint32 mouseButtonState = GetAsyncKeyState(VK_LBUTTON) & 0x8000 ? 1 : 0;
        mouseButtonState |= GetAsyncKeyState(VK_RBUTTON) & 0x8000 ? 2 : 0;
        mouseButtonState |= GetAsyncKeyState(VK_MBUTTON) & 0x8000 ? 4 : 0;
        mouseButtonState |= GetAsyncKeyState(VK_XBUTTON1) & 0x8000 ? 8 : 0;
        mouseButtonState |= GetAsyncKeyState(VK_XBUTTON2) & 0x8000 ? 16 : 0;
        
        // Update mouse button states
        for (uint32 i = 0; i < MAX_MOUSE_BUTTONS; ++i) {
            mouseButtonState_[i] = (mouseButtonState & (1 << i)) != 0;
        }
        
        // Get mouse scroll delta
        mouseScrollDelta_ = 0.0f; // Will be updated in message pump
    }
    
    void updateXInput() {
        if (!xinputGetState_) {
            return;
        }
        
        // Store previous states
        std::memcpy(prevXinputState_, xinputState_, sizeof(xinputState_));
        
        // Update each gamepad
        for (uint32 i = 0; i < MAX_GAMEPADS; ++i) {
            DWORD result = xinputGetState_(i, &xinputState_[i]);
            
            if (result == ERROR_SUCCESS) {
                gamepadConnected_[i] = true;
                
                // Handle scroll wheel (if gamepad has one)
                if (xinputState_[i].Gamepad.wButtons != prevXinputState_[i].Gamepad.wButtons) {
                    // Check if scroll wheel was moved
                    if ((xinputState_[i].Gamepad.wButtons & 0x01) != (prevXinputState_[i].Gamepad.wButtons & 0x01)) {
                        mouseScrollDelta_ += (xinputState_[i].Gamepad.wButtons & 0x01) ? 1.0f : -1.0f;
                    }
                }
            } else if (result == ERROR_DEVICE_NOT_CONNECTED) {
                gamepadConnected_[i] = false;
            }
        }
    }
    
    KeyCode translateWindowsKey(uint8 windowsKey) const {
        // Windows virtual key code to KeyCode mapping
        switch (windowsKey) {
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
            case VK_F8: return KeyCode::F8;
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
    
    float32 normalizeAxisValue(int16 value) const {
        // Normalize from [-32768, 32767] to [-1.0, 1.0]
        if (value == 0) return 0.0f;
        return static_cast<float32>(value) / 32767.0f;
    }
    
    float32 normalizeTriggerValue(uint8 value) const {
        // Normalize from [0, 255] to [0.0, 1.0]
        return static_cast<float32>(value) / 255.0f;
    }
};

// Input manager factory
InputManager* createInputManager() {
    return new WindowsInputManager();
}

void destroyInputManager(InputManager* manager) {
    delete manager;
}

} // namespace RFInput
