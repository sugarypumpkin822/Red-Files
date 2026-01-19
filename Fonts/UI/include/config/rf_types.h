#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>
#include <stack>
#include <list>
#include <deque>
#include <forward_list>
#include <tuple>
#include <pair>
#include <optional>
#include <variant>
#include <any>
#include <functional>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <future>
#include <shared_mutex>
#include <scoped_lock>

namespace RFTypes {

// Basic type aliases
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using float32 = float;
using float64 = double;

// Size and index types
using Size = size_t;
using Index = size_t;
using Count = size_t;
using Offset = ptrdiff_t;
using Difference = ptrdiff_t;

// Handle types
using Handle = uint32;
using ResourceHandle = uint64;
using EntityHandle = uint32;
using ComponentHandle = uint32;
using SystemHandle = uint32;

// ID types
using ID = uint32;
using UniqueID = uint64;
using GUID = uint64;
using UUID = uint64;

// Time types
using Time = float64;
using Duration = float64;
using Timestamp = uint64;
using Milliseconds = std::chrono::milliseconds;
using Microseconds = std::chrono::microseconds;
using Nanoseconds = std::chrono::nanoseconds;
using Seconds = std::chrono::seconds;
using Minutes = std::chrono::minutes;
using Hours = std::chrono::hours;

// Memory types
using Address = void*;
using ConstAddress = const void*;
using Byte = uint8;
using ConstByte = const uint8;

// String types
using String = std::string;
using StringView = std::string_view;
using WideString = std::wstring;
using WideStringView = std::wstring_view;

// Container types
template<typename T>
using Vector = std::vector<T>;

template<typename T>
using List = std::list<T>;

template<typename T>
using ForwardList = std::forward_list<T>;

template<typename T>
using Deque = std::deque<T>;

template<typename T>
using Queue = std::queue<T>;

template<typename T>
using PriorityQueue = std::priority_queue<T>;

template<typename T>
using Stack = std::stack<T>;

template<typename T, Size N>
using Array = std::array<T, N>;

template<typename T>
using Set = std::set<T>;

template<typename T>
using UnorderedSet = std::unordered_set<T>;

template<typename T>
using MultiSet = std::multiset<T>;

template<typename T>
using UnorderedMultiSet = std::unordered_multiset<T>;

template<typename K, typename V>
using Map = std::map<K, V>;

template<typename K, typename V>
using UnorderedMap = std::unordered_map<K, V>;

template<typename K, typename V>
using MultiMap = std::multimap<K, V>;

template<typename K, typename V>
using UnorderedMultiMap = std::unordered_multimap<K, V>;

// Smart pointer types
template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

template<typename T>
using EnableSharedFromThis = std::enable_shared_from_this<T>;

// Optional and variant types
template<typename T>
using Optional = std::optional<T>;

template<typename... Types>
using Variant = std::variant<Types...>;

using Any = std::any;

// Function types
template<typename ReturnType, typename... Args>
using Function = std::function<ReturnType(Args...)>;

template<typename ReturnType, typename... Args>
using Callback = std::function<ReturnType(Args...)>;

template<typename ReturnType, typename... Args>
using Delegate = std::function<ReturnType(Args...)>;

// Thread types
using Thread = std::thread;
using Mutex = std::mutex;
using RecursiveMutex = std::recursive_mutex;
using SharedMutex = std::shared_mutex;
using ReadWriteLock = std::shared_mutex;
using ConditionVariable = std::condition_variable;
using ConditionVariableAny = std::condition_variable_any;
using ScopedLock = std::scoped_lock;
using UniqueLock = std::unique_lock;
using SharedLock = std::shared_lock;
using LockGuard = std::lock_guard;

// Atomic types
template<typename T>
using Atomic = std::atomic<T>;

using AtomicBool = std::atomic<bool>;
using AtomicInteger8 = std::atomic<int8>;
using AtomicInteger16 = std::atomic<int16>;
using AtomicInteger32 = std::atomic<int32>;
using AtomicInteger64 = std::atomic<int64>;
using AtomicUInteger8 = std::atomic<uint8>;
using AtomicUInteger16 = std::atomic<uint16>;
using AtomicUInteger32 = std::atomic<uint32>;
using AtomicUInteger64 = std::atomic<uint64>;
using AtomicFloat = std::atomic<float>;
using AtomicDouble = std::atomic<double>;
using AtomicPointer = std::atomic<void*>;

// Future types
template<typename T>
using Future = std::future<T>;

template<typename T>
using SharedFuture = std::shared_future<T>;

template<typename T>
using Promise = std::promise<T>;

template<typename T>
using PackagedTask = std::packaged_task<T>;

// Tuple types
template<typename... Types>
using Tuple = std::tuple<Types...>;

template<typename T, typename U>
using Pair = std::pair<T, U>;

// Iterator types
template<typename Container>
using Iterator = typename Container::iterator;

template<typename Container>
using ConstIterator = typename Container::const_iterator;

template<typename Container>
using ReverseIterator = typename Container::reverse_iterator;

template<typename Container>
using ConstReverseIterator = typename Container::const_reverse_iterator;

// Range types
template<typename T>
using Range = std::pair<T, T>;

template<typename T>
using InclusiveRange = std::pair<T, T>;

template<typename T>
using ExclusiveRange = std::pair<T, T>;

// Color types
struct ColorRGB {
    float r, g, b;
    ColorRGB() : r(0.0f), g(0.0f), b(0.0f) {}
    ColorRGB(float r_, float g_, float b_) : r(r_), g(g_), b(b_) {}
};

struct ColorRGBA {
    float r, g, b, a;
    ColorRGBA() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
    ColorRGBA(float r_, float g_, float b_, float a_) : r(r_), g(g_), b(b_), a(a_) {}
};

struct ColorHSV {
    float h, s, v;
    ColorHSV() : h(0.0f), s(0.0f), v(0.0f) {}
    ColorHSV(float h_, float s_, float v_) : h(h_), s(s_), v(v_) {}
};

struct ColorHSVA {
    float h, s, v, a;
    ColorHSVA() : h(0.0f), s(0.0f), v(0.0f), a(1.0f) {}
    ColorHSVA(float h_, float s_, float v_, float a_) : h(h_), s(s_), v(v_), a(a_) {}
};

// Vector types
struct Vector2 {
    float x, y;
    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x_, float y_) : x(x_), y(y_) {}
};

struct Vector3 {
    float x, y, z;
    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

struct Vector4 {
    float x, y, z, w;
    Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vector4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
};

// Matrix types
struct Matrix2x2 {
    float m[2][2];
    Matrix2x2() { m[0][0] = 1.0f; m[0][1] = 0.0f; m[1][0] = 0.0f; m[1][1] = 1.0f; }
};

struct Matrix3x3 {
    float m[3][3];
    Matrix3x3() { 
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f;
    }
};

struct Matrix4x4 {
    float m[4][4];
    Matrix4x4() { 
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    }
};

// Quaternion type
struct Quaternion {
    float x, y, z, w;
    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Quaternion(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
};

// Rectangle types
struct Rectangle {
    float x, y, width, height;
    Rectangle() : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {}
    Rectangle(float x_, float y_, float w_, float h_) : x(x_), y(y_), width(w_), height(h_) {}
};

struct RectangleInt {
    int32 x, y, width, height;
    RectangleInt() : x(0), y(0), width(0), height(0) {}
    RectangleInt(int32 x_, int32 y_, int32 w_, int32 h_) : x(x_), y(y_), width(w_), height(h_) {}
};

// Circle type
struct Circle {
    float x, y, radius;
    Circle() : x(0.0f), y(0.0f), radius(0.0f) {}
    Circle(float x_, float y_, float r_) : x(x_), y(y_), radius(r_) {}
};

// Sphere type
struct Sphere {
    float x, y, z, radius;
    Sphere() : x(0.0f), y(0.0f), z(0.0f), radius(0.0f) {}
    Sphere(float x_, float y_, float z_, float r_) : x(x_), y(y_), z(z_), radius(r_) {}
};

// AABB (Axis-Aligned Bounding Box) type
struct AABB {
    Vector3 min;
    Vector3 max;
    AABB() : min(0.0f, 0.0f, 0.0f), max(0.0f, 0.0f, 0.0f) {}
    AABB(const Vector3& min_, const Vector3& max_) : min(min_), max(max_) {}
};

// Transform type
struct Transform {
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;
    Transform() : position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f, 1.0f), scale(1.0f, 1.0f, 1.0f) {}
    Transform(const Vector3& pos, const Quaternion& rot, const Vector3& scl) 
        : position(pos), rotation(rot), scale(scl) {}
};

// Ray type
struct Ray {
    Vector3 origin;
    Vector3 direction;
    Ray() : origin(0.0f, 0.0f, 0.0f), direction(0.0f, 0.0f, 1.0f) {}
    Ray(const Vector3& orig, const Vector3& dir) : origin(orig), direction(dir) {}
};

// Plane type
struct Plane {
    Vector3 normal;
    float distance;
    Plane() : normal(0.0f, 1.0f, 0.0f), distance(0.0f) {}
    Plane(const Vector3& norm, float dist) : normal(norm), distance(dist) {}
};

// Triangle type
struct Triangle {
    Vector3 v0, v1, v2;
    Triangle() : v0(0.0f, 0.0f, 0.0f), v1(0.0f, 0.0f, 0.0f), v2(0.0f, 0.0f, 0.0f) {}
    Triangle(const Vector3& a, const Vector3& b, const Vector3& c) : v0(a), v1(b), v2(c) {}
};

// Line type
struct Line {
    Vector3 start;
    Vector3 end;
    Line() : start(0.0f, 0.0f, 0.0f), end(0.0f, 0.0f, 0.0f) {}
    Line(const Vector3& s, const Vector3& e) : start(s), end(e) {}
};

// Segment type
struct Segment {
    Vector3 start;
    Vector3 end;
    Segment() : start(0.0f, 0.0f, 0.0f), end(0.0f, 0.0f, 0.0f) {}
    Segment(const Vector3& s, const Vector3& e) : start(s), end(e) {}
};

// Polygon type
using Polygon = Vector<Vector2>;

// Polyline type
using Polyline = Vector<Vector2>;

// Path type
using Path = Vector<Vector2>;

// Bezier curve type
struct BezierCurve {
    Vector2 p0, p1, p2, p3;
    BezierCurve() : p0(0.0f, 0.0f), p1(0.0f, 0.0f), p2(0.0f, 0.0f), p3(0.0f, 0.0f) {}
    BezierCurve(const Vector2& a, const Vector2& b, const Vector2& c, const Vector2& d) 
        : p0(a), p1(b), p2(c), p3(d) {}
};

// Spline type
using Spline = Vector<Vector3>;

// Mesh type
struct Mesh {
    Vector<Vector3> vertices;
    Vector<Vector3> normals;
    Vector<Vector2> texCoords;
    Vector<uint32> indices;
    Mesh() {}
};

// Texture type
struct Texture {
    uint32 width;
    uint32 height;
    uint32 channels;
    Vector<uint8> data;
    Texture() : width(0), height(0), channels(0) {}
    Texture(uint32 w, uint32 h, uint32 c) : width(w), height(h), channels(c) {}
};

// Font type
struct Font {
    String name;
    float32 size;
    bool bold;
    bool italic;
    Font() : name(""), size(12.0f), bold(false), italic(false) {}
    Font(const String& n, float32 s, bool b = false, bool i = false) 
        : name(n), size(s), bold(b), italic(i) {}
};

// Audio buffer type
struct AudioBuffer {
    uint32 sampleRate;
    uint16 channels;
    uint16 bitsPerSample;
    Vector<float32> samples;
    AudioBuffer() : sampleRate(44100), channels(2), bitsPerSample(16) {}
    AudioBuffer(uint32 rate, uint16 ch, uint16 bits) : sampleRate(rate), channels(ch), bitsPerSample(bits) {}
};

// Video frame type
struct VideoFrame {
    uint32 width;
    uint32 height;
    uint32 format;
    Vector<uint8> data;
    Timestamp timestamp;
    VideoFrame() : width(0), height(0), format(0), timestamp(0) {}
    VideoFrame(uint32 w, uint32 h, uint32 fmt, Timestamp ts) 
        : width(w), height(h), format(fmt), timestamp(ts) {}
};

// Event type
struct Event {
    String type;
    Any data;
    Timestamp timestamp;
    Event() : type(""), timestamp(0) {}
    Event(const String& t, const Any& d, Timestamp ts) : type(t), data(d), timestamp(ts) {}
};

// Message type
struct Message {
    String sender;
    String receiver;
    String subject;
    Any payload;
    Timestamp timestamp;
    Message() : sender(""), receiver(""), subject(""), timestamp(0) {}
    Message(const String& s, const String& r, const String& sub, const Any& p, Timestamp ts) 
        : sender(s), receiver(r), subject(sub), payload(p), timestamp(ts) {}
};

// Command type
struct Command {
    String name;
    Vector<String> arguments;
    Any context;
    Timestamp timestamp;
    Command() : name(""), timestamp(0) {}
    Command(const String& n, const Vector<String>& args, const Any& ctx, Timestamp ts) 
        : name(n), arguments(args), context(ctx), timestamp(ts) {}
};

// Result type
template<typename T>
struct Result {
    bool success;
    T value;
    String error;
    Result() : success(false), value(T{}), error("") {}
    Result(bool s, const T& val, const String& err) : success(s), value(val), error(err) {}
    static Result<T> success(const T& val) { return Result<T>(true, val, ""); }
    static Result<T> failure(const String& err) { return Result<T>(false, T{}, err); }
};

// Error type
struct Error {
    String code;
    String message;
    String details;
    Timestamp timestamp;
    Error() : code(""), message(""), details(""), timestamp(0) {}
    Error(const String& c, const String& msg, const String& det, Timestamp ts) 
        : code(c), message(msg), details(det), timestamp(ts) {}
};

// Log entry type
struct LogEntry {
    String level;
    String message;
    String category;
    Timestamp timestamp;
    LogEntry() : level(""), message(""), category(""), timestamp(0) {}
    LogEntry(const String& lvl, const String& msg, const String& cat, Timestamp ts) 
        : level(lvl), message(msg), category(cat), timestamp(ts) {}
};

// Performance metrics type
struct PerformanceMetrics {
    float32 frameTime;
    float32 cpuUsage;
    float32 memoryUsage;
    float32 gpuUsage;
    uint32 frameCount;
    Timestamp timestamp;
    PerformanceMetrics() : frameTime(0.0f), cpuUsage(0.0f), memoryUsage(0.0f), 
                         gpuUsage(0.0f), frameCount(0), timestamp(0) {}
    PerformanceMetrics(float32 ft, float32 cpu, float32 mem, float32 gpu, uint32 fc, Timestamp ts) 
        : frameTime(ft), cpuUsage(cpu), memoryUsage(mem), gpuUsage(gpu), frameCount(fc), timestamp(ts) {}
};

// Type traits
template<typename T>
struct IsPointer {
    static constexpr bool value = std::is_pointer_v<T>;
};

template<typename T>
struct IsReference {
    static constexpr bool value = std::is_reference_v<T>;
};

template<typename T>
struct IsConst {
    static constexpr bool value = std::is_const_v<T>;
};

template<typename T>
struct IsVolatile {
    static constexpr bool value = std::is_volatile_v<T>;
};

template<typename T>
struct IsIntegral {
    static constexpr bool value = std::is_integral_v<T>;
};

template<typename T>
struct IsFloatingPoint {
    static constexpr bool value = std::is_floating_point_v<T>;
};

template<typename T>
struct IsArithmetic {
    static constexpr bool value = std::is_arithmetic_v<T>;
};

template<typename T>
struct IsEnum {
    static constexpr bool value = std::is_enum_v<T>;
};

template<typename T>
struct IsClass {
    static constexpr bool value = std::is_class_v<T>;
};

template<typename T>
struct IsUnion {
    static constexpr bool value = std::is_union_v<T>;
};

template<typename T>
struct IsFunction {
    static constexpr bool value = std::is_function_v<T>;
};

template<typename T>
struct IsArray {
    static constexpr bool value = std::is_array_v<T>;
};

template<typename T>
struct IsPointerOrReference {
    static constexpr bool value = IsPointer<T>::value || IsReference<T>::value;
};

template<typename T>
struct IsConstOrVolatile {
    static constexpr bool value = IsConst<T>::value || IsVolatile<T>::value;
};

template<typename T>
struct IsFundamental {
    static constexpr bool value = IsArithmetic<T>::value || IsEnum<T>::value;
};

template<typename T>
struct IsCompound {
    static constexpr bool value = !IsFundamental<T>::value;
};

// Type utilities
template<typename T>
using RemoveConst = typename std::remove_const<T>::type;

template<typename T>
using RemoveVolatile = typename std::remove_volatile<T>::type;

template<typename T>
using RemoveCV = typename std::remove_cv<T>::type;

template<typename T>
using RemoveReference = typename std::remove_reference<T>::type;

template<typename T>
using RemovePointer = typename std::remove_pointer<T>::type;

template<typename T>
using RemoveExtent = typename std::remove_extent<T>::type;

template<typename T>
using AddConst = typename std::add_const<T>::type;

template<typename T>
using AddVolatile = typename std::add_volatile<T>::type;

template<typename T>
using AddCV = typename std::add_cv<T>::type;

template<typename T>
using AddLValueReference = typename std::add_lvalue_reference<T>::type;

template<typename T>
using AddRValueReference = typename std::add_rvalue_reference<T>::type;

template<typename T>
using AddPointer = typename std::add_pointer<T>::type;

template<typename T>
using MakeSigned = typename std::make_signed<T>::type;

template<typename T>
using MakeUnsigned = typename std::make_unsigned<T>::type;

template<typename T>
using Decay = typename std::decay<T>::type;

} // namespace RFTypes