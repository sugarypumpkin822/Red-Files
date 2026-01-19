#pragma once

#include <cstdint>
#include <string>
#include <array>

namespace RFConstants {

// Version information
constexpr uint32_t VERSION_MAJOR = 1;
constexpr uint32_t VERSION_MINOR = 0;
constexpr uint32_t VERSION_PATCH = 0;
constexpr uint32_t VERSION_BUILD = 0;
constexpr const char* VERSION_STRING = "1.0.0.0";

// Mathematical constants
constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 6.28318530717958647692f;
constexpr float HALF_PI = 1.57079632679489661923f;
constexpr float QUARTER_PI = 0.78539816339744830962f;
constexpr float INV_PI = 0.31830988618379067154f;
constexpr float INV_TWO_PI = 0.15915494309189533577f;
constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / PI;
constexpr float EULER = 2.71828182845904523536f;
constexpr float SQRT_2 = 1.41421356237309504880f;
constexpr float SQRT_3 = 1.73205080756887729353f;
constexpr float GOLDEN_RATIO = 1.61803398874989484820f;

// Physical constants
constexpr float GRAVITY = 9.80665f;
constexpr float WATER_DENSITY = 1000.0f;
constexpr float AIR_DENSITY = 1.225f;
constexpr float BLOOD_DENSITY = 1060.0f;
constexpr float SPEED_OF_LIGHT = 299792458.0f;
constexpr float SPEED_OF_SOUND = 343.0f;
constexpr float BOLTZMANN_CONSTANT = 1.380649e-23f;
constexpr float AVOGADRO_NUMBER = 6.02214076e23f;
constexpr float PLANCK_CONSTANT = 6.62607015e-34f;

// Time constants
constexpr float MILLISECONDS_PER_SECOND = 1000.0f;
constexpr float MICROSECONDS_PER_SECOND = 1000000.0f;
constexpr float NANOSECONDS_PER_SECOND = 1000000000.0f;
constexpr float SECONDS_PER_MINUTE = 60.0f;
constexpr float MINUTES_PER_HOUR = 60.0f;
constexpr float HOURS_PER_DAY = 24.0f;
constexpr float DAYS_PER_WEEK = 7.0f;
constexpr float DAYS_PER_MONTH = 30.44f;
constexpr float DAYS_PER_YEAR = 365.25f;

// Memory constants
constexpr uint32_t KILOBYTE = 1024;
constexpr uint32_t MEGABYTE = 1048576;
constexpr uint32_t GIGABYTE = 1073741824;
constexpr uint32_t TERABYTE = 1099511627776;
constexpr uint32_t PETABYTE = 1125899906842624;
constexpr uint32_t MAX_BUFFER_SIZE = 65536;
constexpr uint32_t MAX_STRING_LENGTH = 4096;
constexpr uint32_t MAX_PATH_LENGTH = 260;

// Graphics constants
constexpr uint32_t MAX_TEXTURE_SIZE = 4096;
constexpr uint32_t MAX_RENDER_TARGETS = 8;
constexpr uint32_t MAX_VERTEX_BUFFERS = 16;
constexpr uint32_t MAX_INDEX_BUFFERS = 16;
constexpr uint32_t MAX_CONSTANT_BUFFERS = 16;
constexpr uint32_t MAX_SHADER_STAGES = 6;
constexpr uint32_t MAX_TEXTURE_UNITS = 32;
constexpr uint32_t MAX_SAMPLERS = 16;
constexpr uint32_t MAX_VIEWPORTS = 4;
constexpr uint32_t MAX_RENDER_TARGET_LAYERS = 8;

// Audio constants
constexpr uint32_t AUDIO_SAMPLE_RATE_8K = 8000;
constexpr uint32_t AUDIO_SAMPLE_RATE_11K = 11025;
constexpr uint32_t AUDIO_SAMPLE_RATE_16K = 16000;
constexpr uint32_t AUDIO_SAMPLE_RATE_22K = 22050;
constexpr uint32_t AUDIO_SAMPLE_RATE_44K = 44100;
constexpr uint32_t AUDIO_SAMPLE_RATE_48K = 48000;
constexpr uint32_t AUDIO_SAMPLE_RATE_96K = 96000;
constexpr uint32_t AUDIO_SAMPLE_RATE_192K = 192000;
constexpr uint32_t AUDIO_CHANNELS_MONO = 1;
constexpr uint32_t AUDIO_CHANNELS_STEREO = 2;
constexpr uint32_t AUDIO_CHANNELS_5_1 = 6;
constexpr uint32_t AUDIO_CHANNELS_7_1 = 8;
constexpr uint32_t AUDIO_BITS_8 = 8;
constexpr uint32_t AUDIO_BITS_16 = 16;
constexpr uint32_t AUDIO_BITS_24 = 24;
constexpr uint32_t AUDIO_BITS_32 = 32;

// Input constants
constexpr uint32_t MAX_MOUSE_BUTTONS = 8;
constexpr uint32_t MAX_KEYBOARD_KEYS = 256;
constexpr uint32_t MAX_GAMEPAD_BUTTONS = 16;
constexpr uint32_t MAX_GAMEPAD_AXES = 8;
constexpr uint32_t MAX_TOUCH_POINTS = 10;
constexpr uint32_t MAX_GESTURES = 32;
constexpr float MOUSE_SENSITIVITY_DEFAULT = 1.0f;
constexpr float GAMEPAD_DEADZONE_DEFAULT = 0.1f;
constexpr float GAMEPAD_SENSITIVITY_DEFAULT = 1.0f;

// Network constants
constexpr uint32_t MAX_PACKET_SIZE = 1500;
constexpr uint32_t MAX_CONNECTIONS = 64;
constexpr uint32_t MAX_CHANNELS = 8;
constexpr uint32_t MAX_RELIABLE_COMMANDS = 64;
constexpr uint32_t MAX_UNRELIABLE_COMMANDS = 128;
constexpr uint32_t NETWORK_TIMEOUT_MS = 5000;
constexpr uint32_t NETWORK_HEARTBEAT_INTERVAL_MS = 1000;
constexpr uint32_t NETWORK_RESEND_INTERVAL_MS = 100;
constexpr uint32_t NETWORK_MAX_RETRIES = 5;

// Performance constants
constexpr uint32_t MAX_FRAMES_PER_SECOND = 240;
constexpr uint32_t MIN_FRAMES_PER_SECOND = 15;
constexpr uint32_t TARGET_FRAMES_PER_SECOND = 60;
constexpr float TARGET_FRAME_TIME = 1.0f / TARGET_FRAMES_PER_SECOND;
constexpr uint32_t MAX_UPDATE_TIME_MS = 16;
constexpr uint32_t MAX_RENDER_TIME_MS = 16;
constexpr uint32_t MAX_AUDIO_TIME_MS = 1;
constexpr uint32_t MAX_INPUT_TIME_MS = 1;
constexpr uint32_t MAX_NETWORK_TIME_MS = 2;

// Debug constants
constexpr uint32_t MAX_LOG_ENTRIES = 10000;
constexpr uint32_t MAX_DEBUG_MESSAGES = 1000;
constexpr uint32_t MAX_BREAKPOINTS = 64;
constexpr uint32_t MAX_WATCH_VARIABLES = 128;
constexpr uint32_t MAX_PROFILER_SAMPLES = 1000;
constexpr uint32_t DEBUG_DRAW_MAX_VERTICES = 10000;
constexpr uint32_t DEBUG_DRAW_MAX_INDICES = 20000;

// File system constants
constexpr uint32_t MAX_OPEN_FILES = 256;
constexpr uint32_t MAX_FILE_SIZE = 1073741824; // 1GB
constexpr uint32_t FILE_BUFFER_SIZE = 4096;
constexpr uint32_t MAX_DIRECTORY_ENTRIES = 1024;
constexpr uint32_t MAX_FILE_PATH_LENGTH = 260;
constexpr uint32_t MAX_FILE_NAME_LENGTH = 255;

// String constants
constexpr const char* EMPTY_STRING = "";
constexpr const char* DEFAULT_STRING = "default";
constexpr const char* TRUE_STRING = "true";
constexpr const char* FALSE_STRING = "false";
constexpr const char* YES_STRING = "yes";
constexpr const char* NO_STRING = "no";
constexpr const char* ON_STRING = "on";
constexpr const char* OFF_STRING = "off";
constexpr const char* ENABLED_STRING = "enabled";
constexpr const char* DISABLED_STRING = "disabled";

// Color constants
constexpr std::array<float, 4> COLOR_WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
constexpr std::array<float, 4> COLOR_BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
constexpr std::array<float, 4> COLOR_RED = {1.0f, 0.0f, 0.0f, 1.0f};
constexpr std::array<float, 4> COLOR_GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
constexpr std::array<float, 4> COLOR_BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
constexpr std::array<float, 4> COLOR_YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
constexpr std::array<float, 4> COLOR_MAGENTA = {1.0f, 0.0f, 1.0f, 1.0f};
constexpr std::array<float, 4> COLOR_CYAN = {0.0f, 1.0f, 1.0f, 1.0f};
constexpr std::array<float, 4> COLOR_GRAY = {0.5f, 0.5f, 0.5f, 1.0f};
constexpr std::array<float, 4> COLOR_LIGHT_GRAY = {0.75f, 0.75f, 0.75f, 1.0f};
constexpr std::array<float, 4> COLOR_DARK_GRAY = {0.25f, 0.25f, 0.25f, 1.0f};
constexpr std::array<float, 4> COLOR_TRANSPARENT = {0.0f, 0.0f, 0.0f, 0.0f};

// Blood system constants
constexpr float BLOOD_VISCOSITY_DEFAULT = 1.0f;
constexpr float BLOOD_DENSITY_DEFAULT = 1060.0f;
constexpr float BLOOD_SURFACE_TENSION = 0.058f;
constexpr float BLOOD_COHESION = 0.1f;
constexpr float BLOOD_ADHESION = 0.05f;
constexpr float BLOOD_EVAPORATION_RATE = 0.001f;
constexpr float BLOOD_COAGULATION_TIME = 5.0f;
constexpr float BLOOD_COAGULATION_DISTANCE = 2.0f;
constexpr float BLOOD_PARTICLE_SIZE_DEFAULT = 1.0f;
constexpr float BLOOD_PARTICLE_LIFETIME_DEFAULT = 10.0f;
constexpr uint32_t BLOOD_MAX_PARTICLES = 10000;
constexpr uint32_t BLOOD_MAX_EFFECTS = 1000;
constexpr uint32_t BLOOD_MAX_POOLS = 100;
constexpr uint32_t BLOOD_MAX_STREAMS = 50;
constexpr uint32_t BLOOD_MAX_TRAILS = 200;

// Animation constants
constexpr float ANIMATION_DEFAULT_FPS = 30.0f;
constexpr float ANIMATION_DEFAULT_DURATION = 1.0f;
constexpr float ANIMATION_BLEND_TIME = 0.25f;
constexpr uint32_t ANIMATION_MAX_KEYFRAMES = 1000;
constexpr uint32_t ANIMATION_MAX_SEQUENCES = 100;
constexpr uint32_t ANIMATION_MAX_TIMELINES = 50;
constexpr uint32_t ANIMATION_MAX_TWEENS = 500;

// Error codes
enum class ErrorCode {
    SUCCESS = 0,
    INVALID_PARAMETER = 1,
    OUT_OF_MEMORY = 2,
    FILE_NOT_FOUND = 3,
    PERMISSION_DENIED = 4,
    INVALID_OPERATION = 5,
    TIMEOUT = 6,
    NETWORK_ERROR = 7,
    DEVICE_ERROR = 8,
    FORMAT_ERROR = 9,
    PARSE_ERROR = 10,
    VALIDATION_ERROR = 11,
    INITIALIZATION_ERROR = 12,
    RESOURCE_ERROR = 13,
    STATE_ERROR = 14,
    UNKNOWN_ERROR = 999
};

// Log levels
enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARNING = 3,
    ERROR = 4,
    FATAL = 5
};

// Priority levels
enum class Priority {
    LOW = 0,
    NORMAL = 1,
    HIGH = 2,
    URGENT = 3,
    CRITICAL = 4
};

// State values
enum class State {
    IDLE = 0,
    INITIALIZING = 1,
    RUNNING = 2,
    PAUSED = 3,
    STOPPING = 4,
    STOPPED = 5,
    ERROR = 6
};

// Result values
enum class Result {
    SUCCESS = 0,
    FAILURE = 1,
    PENDING = 2,
    CANCELLED = 3,
    TIMEOUT = 4
};

// Utility functions
constexpr bool isPowerOfTwo(uint32_t value) {
    return value != 0 && (value & (value - 1)) == 0;
}

constexpr uint32_t nextPowerOfTwo(uint32_t value) {
    if (value == 0) return 1;
    value--;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value++;
    return value;
}

constexpr float clamp(float value, float min, float max) {
    return (value < min) ? min : (value > max) ? max : value;
}

constexpr int clamp(int value, int min, int max) {
    return (value < min) ? min : (value > max) ? max : value;
}

constexpr float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

constexpr float smoothstep(float edge0, float edge1, float x) {
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

constexpr float degreesToRadians(float degrees) {
    return degrees * DEG_TO_RAD;
}

constexpr float radiansToDegrees(float radians) {
    return radians * RAD_TO_DEG;
}

} // namespace RFConstants