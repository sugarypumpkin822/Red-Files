#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <algorithm>

namespace RFUtils {

// Forward declarations
class Vector2;
class Vector3;
class Vector4;
class Matrix3;
class Matrix4;

// 2D Vector class
class Vector2 {
public:
    float32 x, y;
    
    // Constructors
    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float32 x, float32 y) : x(x), y(y) {}
    Vector2(float32 value) : x(value), y(value) {}
    explicit Vector2(const float32* data) : x(data[0]), y(data[1]) {}
    Vector2(const std::array<float32, 2>& data) : x(data[0]), y(data[1]) {}
    
    // Copy constructor
    Vector2(const Vector2& other) : x(other.x), y(other.y) {}
    
    // Assignment operator
    Vector2& operator=(const Vector2& other) {
        x = other.x;
        y = other.y;
        return *this;
    }
    
    // Component access
    float32& operator[](size_t index) { return (&x)[index]; }
    const float32& operator[](size_t index) const { return (&x)[index]; }
    
    // Array access
    float32* data() { return &x; }
    const float32* data() const { return &x; }
    
    // Comparison operators
    bool operator==(const Vector2& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }
    
    // Arithmetic operators
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }
    
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }
    
    Vector2 operator*(const Vector2& other) const {
        return Vector2(x * other.x, y * other.y);
    }
    
    Vector2 operator/(const Vector2& other) const {
        return Vector2(x / other.x, y / other.y);
    }
    
    Vector2 operator*(float32 scalar) const {
        return Vector2(x * scalar, y * scalar);
    }
    
    Vector2 operator/(float32 scalar) const {
        return Vector2(x / scalar, y / scalar);
    }
    
    // Assignment arithmetic operators
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    
    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    
    Vector2& operator*=(const Vector2& other) {
        x *= other.x;
        y *= other.y;
        return *this;
    }
    
    Vector2& operator/=(const Vector2& other) {
        x /= other.x;
        y /= other.y;
        return *this;
    }
    
    Vector2& operator*=(float32 scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    
    Vector2& operator/=(float32 scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }
    
    // Unary operators
    Vector2 operator+() const {
        return *this;
    }
    
    Vector2 operator-() const {
        return Vector2(-x, -y);
    }
    
    // Vector operations
    float32 dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }
    
    float32 cross(const Vector2& other) const {
        return x * other.y - y * other.x;
    }
    
    float32 length() const {
        return std::sqrt(x * x + y * y);
    }
    
    float32 lengthSquared() const {
        return x * x + y * y;
    }
    
    float32 distanceTo(const Vector2& other) const {
        return (*this - other).length();
    }
    
    float32 distanceSquaredTo(const Vector2& other) const {
        return (*this - other).lengthSquared();
    }
    
    float32 angle() const {
        return std::atan2(y, x);
    }
    
    float32 angleTo(const Vector2& other) const {
        return std::atan2(cross(other), dot(other));
    }
    
    Vector2 normalized() const {
        float32 len = length();
        if (len > 0.0f) {
            return *this / len;
        }
        return Vector2();
    }
    
    Vector2& normalize() {
        float32 len = length();
        if (len > 0.0f) {
            *this /= len;
        }
        return *this;
    }
    
    Vector2 perpendicular() const {
        return Vector2(-y, x);
    }
    
    Vector2 reflect(const Vector2& normal) const {
        return *this - normal * (2.0f * dot(normal));
    }
    
    Vector2 project(const Vector2& other) const {
        return other * (dot(other) / other.lengthSquared());
    }
    
    Vector2 lerp(const Vector2& other, float32 t) const {
        return *this + (other - *this) * t;
    }
    
    Vector2 slerp(const Vector2& other, float32 t) const {
        float32 angle = angleTo(other);
        float32 sinAngle = std::sin(angle);
        
        if (sinAngle < 0.001f) {
            return lerp(other, t);
        }
        
        float32 factor1 = std::sin((1.0f - t) * angle) / sinAngle;
        float32 factor2 = std::sin(t * angle) / sinAngle;
        
        return *this * factor1 + other * factor2;
    }
    
    Vector2 nlerp(const Vector2& other, float32 t) const {
        Vector2 result = lerp(other, t);
        return result.normalized();
    }
    
    // Rotation operations
    Vector2 rotate(float32 angle) const {
        float32 cosAngle = std::cos(angle);
        float32 sinAngle = std::sin(angle);
        return Vector2(x * cosAngle - y * sinAngle, x * sinAngle + y * cosAngle);
    }
    
    Vector2& rotateInPlace(float32 angle) {
        float32 cosAngle = std::cos(angle);
        float32 sinAngle = std::sin(angle);
        float32 newX = x * cosAngle - y * sinAngle;
        float32 newY = x * sinAngle + y * cosAngle;
        x = newX;
        y = newY;
        return *this;
    }
    
    // Swizzle operations
    Vector2 xx() const { return Vector2(x, x); }
    Vector2 xy() const { return Vector2(x, y); }
    Vector2 yx() const { return Vector2(y, x); }
    Vector2 yy() const { return Vector2(y, y); }
    
    // Conversion operations
    Vector3 toVector3(float32 z = 0.0f) const;
    Vector4 toVector4(float32 z = 0.0f, float32 w = 0.0f) const;
    
    // Utility methods
    bool isZero() const {
        return x == 0.0f && y == 0.0f;
    }
    
    bool isNearZero(float32 tolerance = 0.0001f) const {
        return std::abs(x) < tolerance && std::abs(y) < tolerance;
    }
    
    bool isNormalized() const {
        return std::abs(length() - 1.0f) < 0.0001f;
    }
    
    bool isFinite() const {
        return std::isfinite(x) && std::isfinite(y);
    }
    
    Vector2 abs() const {
        return Vector2(std::abs(x), std::abs(y));
    }
    
    Vector2 floor() const {
        return Vector2(std::floor(x), std::floor(y));
    }
    
    Vector2 ceil() const {
        return Vector2(std::ceil(x), std::ceil(y));
    }
    
    Vector2 round() const {
        return Vector2(std::round(x), std::round(y));
    }
    
    Vector2 min(const Vector2& other) const {
        return Vector2(std::min(x, other.x), std::min(y, other.y));
    }
    
    Vector2 max(const Vector2& other) const {
        return Vector2(std::max(x, other.x), std::max(y, other.y));
    }
    
    Vector2 clamp(const Vector2& min, const Vector2& max) const {
        return Vector2(std::clamp(x, min.x, max.x), std::clamp(y, min.y, max.y));
    }
    
    Vector2 clamp(float32 min, float32 max) const {
        return Vector2(std::clamp(x, min, max), std::clamp(y, min, max));
    }
    
    // String conversion
    std::string toString() const {
        return "Vector2(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
    
    // Static methods
    static Vector2 zero() { return Vector2(0.0f, 0.0f); }
    static Vector2 one() { return Vector2(1.0f, 1.0f); }
    static Vector2 unitX() { return Vector2(1.0f, 0.0f); }
    static Vector2 unitY() { return Vector2(0.0f, 1.0f); }
    static Vector2 up() { return Vector2(0.0f, 1.0f); }
    static Vector2 down() { return Vector2(0.0f, -1.0f); }
    static Vector2 left() { return Vector2(-1.0f, 0.0f); }
    static Vector2 right() { return Vector2(1.0f, 0.0f); }
    
    static Vector2 fromAngle(float32 angle) {
        return Vector2(std::cos(angle), std::sin(angle));
    }
    
    static float32 dot(const Vector2& a, const Vector2& b) {
        return a.x * b.x + a.y * b.y;
    }
    
    static float32 cross(const Vector2& a, const Vector2& b) {
        return a.x * b.y - a.y * b.x;
    }
    
    static float32 distance(const Vector2& a, const Vector2& b) {
        return (a - b).length();
    }
    
    static float32 distanceSquared(const Vector2& a, const Vector2& b) {
        return (a - b).lengthSquared();
    }
    
    static float32 angle(const Vector2& a, const Vector2& b) {
        return std::atan2(a.cross(b), a.dot(b));
    }
    
    static Vector2 lerp(const Vector2& a, const Vector2& b, float32 t) {
        return a + (b - a) * t;
    }
    
    static Vector2 slerp(const Vector2& a, const Vector2& b, float32 t) {
        return a.slerp(b, t);
    }
    
    static Vector2 nlerp(const Vector2& a, const Vector2& b, float32 t) {
        return a.nlerp(b, t);
    }
    
    static Vector2 reflect(const Vector2& vector, const Vector2& normal) {
        return vector.reflect(normal);
    }
    
    static Vector2 project(const Vector2& vector, const Vector2& onto) {
        return vector.project(onto);
    }
    
    static Vector2 min(const Vector2& a, const Vector2& b) {
        return a.min(b);
    }
    
    static Vector2 max(const Vector2& a, const Vector2& b) {
        return a.max(b);
    }
    
    static Vector2 clamp(const Vector2& value, const Vector2& min, const Vector2& max) {
        return value.clamp(min, max);
    }
    
    static Vector2 clamp(const Vector2& value, float32 min, float32 max) {
        return value.clamp(min, max);
    }
    
    static Vector2 abs(const Vector2& vector) {
        return vector.abs();
    }
    
    static Vector2 floor(const Vector2& vector) {
        return vector.floor();
    }
    
    static Vector2 ceil(const Vector2& vector) {
        return vector.ceil();
    }
    
    static Vector2 round(const Vector2& vector) {
        return vector.round();
    }
    
    static Vector2 perpendicular(const Vector2& vector) {
        return vector.perpendicular();
    }
    
    static Vector2 rotate(const Vector2& vector, float32 angle) {
        return vector.rotate(angle);
    }
    
    // Random vector generation
    static Vector2 random();
    static Vector2 random(float32 min, float32 max);
    static Vector2 random(const Vector2& min, const Vector2& max);
    static Vector2 randomNormalized();
    static Vector2 randomOnCircle(float32 radius = 1.0f);
    static Vector2 randomInCircle(float32 radius = 1.0f);
    static Vector2 randomOnRectangle(float32 width, float32 height);
    static Vector2 randomInRectangle(float32 width, float32 height);
    
    // Constants
    static constexpr float32 EPSILON = 0.0001f;
    static constexpr float32 PI = 3.14159265358979323846f;
    static constexpr float32 TWO_PI = 2.0f * PI;
    static constexpr float32 HALF_PI = PI / 2.0f;
    static constexpr float32 QUARTER_PI = PI / 4.0f;
    static constexpr float32 DEG_TO_RAD = PI / 180.0f;
    static constexpr float32 RAD_TO_DEG = 180.0f / PI;
};

// Global operators for scalar operations
inline Vector2 operator*(float32 scalar, const Vector2& vector) {
    return vector * scalar;
}

inline Vector2 operator/(float32 scalar, const Vector2& vector) {
    return Vector2(scalar / vector.x, scalar / vector.y);
}

// Utility functions
inline Vector2 abs(const Vector2& vector) {
    return vector.abs();
}

inline Vector2 floor(const Vector2& vector) {
    return vector.floor();
}

inline Vector2 ceil(const Vector2& vector) {
    return vector.ceil();
}

inline Vector2 round(const Vector2& vector) {
    return vector.round();
}

inline Vector2 min(const Vector2& a, const Vector2& b) {
    return a.min(b);
}

inline Vector2 max(const Vector2& a, const Vector2& b) {
    return a.max(b);
}

inline Vector2 clamp(const Vector2& value, const Vector2& min, const Vector2& max) {
    return value.clamp(min, max);
}

inline Vector2 clamp(const Vector2& value, float32 min, float32 max) {
    return value.clamp(min, max);
}

inline float32 dot(const Vector2& a, const Vector2& b) {
    return a.dot(b);
}

inline float32 cross(const Vector2& a, const Vector2& b) {
    return a.cross(b);
}

inline float32 distance(const Vector2& a, const Vector2& b) {
    return a.distanceTo(b);
}

inline float32 distanceSquared(const Vector2& a, const Vector2& b) {
    return a.distanceSquaredTo(b);
}

inline float32 angle(const Vector2& a, const Vector2& b) {
    return a.angleTo(b);
}

inline Vector2 lerp(const Vector2& a, const Vector2& b, float32 t) {
    return a.lerp(b, t);
}

inline Vector2 slerp(const Vector2& a, const Vector2& b, float32 t) {
    return a.slerp(b, t);
}

inline Vector2 nlerp(const Vector2& a, const Vector2& b, float32 t) {
    return a.nlerp(b, t);
}

inline Vector2 reflect(const Vector2& vector, const Vector2& normal) {
    return vector.reflect(normal);
}

inline Vector2 project(const Vector2& vector, const Vector2& onto) {
    return vector.project(onto);
}

inline Vector2 perpendicular(const Vector2& vector) {
    return vector.perpendicular();
}

inline Vector2 rotate(const Vector2& vector, float32 angle) {
    return vector.rotate(angle);
}

// Hash function for Vector2
struct Vector2Hash {
    size_t operator()(const Vector2& vector) const noexcept {
        size_t h1 = std::hash<float32>{}(vector.x);
        size_t h2 = std::hash<float32>{}(vector.y);
        return h1 ^ (h2 << 1);
    }
};

} // namespace RFUtils