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

// 3D Vector class
class Vector3 {
public:
    float32 x, y, z;
    
    // Constructors
    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float32 x, float32 y, float32 z) : x(x), y(y), z(z) {}
    Vector3(float32 value) : x(value), y(value), z(value) {}
    explicit Vector3(const float32* data) : x(data[0]), y(data[1]), z(data[2]) {}
    Vector3(const std::array<float32, 3>& data) : x(data[0]), y(data[1]), z(data[2]) {}
    Vector3(const Vector2& vector, float32 z = 0.0f) : x(vector.x), y(vector.y), z(z) {}
    
    // Copy constructor
    Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}
    
    // Assignment operator
    Vector3& operator=(const Vector3& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }
    
    // Component access
    float32& operator[](size_t index) { return (&x)[index]; }
    const float32& operator[](size_t index) const { return (&x)[index]; }
    
    // Array access
    float32* data() { return &x; }
    const float32* data() const { return &x; }
    
    // Comparison operators
    bool operator==(const Vector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    
    bool operator!=(const Vector3& other) const {
        return !(*this == other);
    }
    
    // Arithmetic operators
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    
    Vector3 operator*(const Vector3& other) const {
        return Vector3(x * other.x, y * other.y, z * other.z);
    }
    
    Vector3 operator/(const Vector3& other) const {
        return Vector3(x / other.x, y / other.y, z / other.z);
    }
    
    Vector3 operator*(float32 scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    
    Vector3 operator/(float32 scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
    
    // Assignment arithmetic operators
    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    
    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    
    Vector3& operator*=(const Vector3& other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }
    
    Vector3& operator/=(const Vector3& other) {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }
    
    Vector3& operator*=(float32 scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    
    Vector3& operator/=(float32 scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    
    // Unary operators
    Vector3 operator+() const {
        return *this;
    }
    
    Vector3 operator-() const {
        return Vector3(-x, -y, -z);
    }
    
    // Vector operations
    float32 dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    Vector3 cross(const Vector3& other) const {
        return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }
    
    float32 length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    float32 lengthSquared() const {
        return x * x + y * y + z * z;
    }
    
    float32 distanceTo(const Vector3& other) const {
        return (*this - other).length();
    }
    
    float32 distanceSquaredTo(const Vector3& other) const {
        return (*this - other).lengthSquared();
    }
    
    float32 angleTo(const Vector3& other) const {
        return std::acos(dot(other) / (length() * other.length()));
    }
    
    Vector3 normalized() const {
        float32 len = length();
        if (len > 0.0f) {
            return *this / len;
        }
        return Vector3();
    }
    
    Vector3& normalize() {
        float32 len = length();
        if (len > 0.0f) {
            *this /= len;
        }
        return *this;
    }
    
    Vector3 reflect(const Vector3& normal) const {
        return *this - normal * (2.0f * dot(normal));
    }
    
    Vector3 project(const Vector3& other) const {
        return other * (dot(other) / other.lengthSquared());
    }
    
    Vector3 projectOnPlane(const Vector3& normal) const {
        return *this - project(normal);
    }
    
    Vector3 lerp(const Vector3& other, float32 t) const {
        return *this + (other - *this) * t;
    }
    
    Vector3 slerp(const Vector3& other, float32 t) const {
        float32 angle = angleTo(other);
        float32 sinAngle = std::sin(angle);
        
        if (sinAngle < 0.001f) {
            return lerp(other, t);
        }
        
        float32 factor1 = std::sin((1.0f - t) * angle) / sinAngle;
        float32 factor2 = std::sin(t * angle) / sinAngle;
        
        return *this * factor1 + other * factor2;
    }
    
    Vector3 nlerp(const Vector3& other, float32 t) const {
        Vector3 result = lerp(other, t);
        return result.normalized();
    }
    
    // Swizzle operations
    Vector2 xx() const { return Vector2(x, x); }
    Vector2 xy() const { return Vector2(x, y); }
    Vector2 xz() const { return Vector2(x, z); }
    Vector2 yx() const { return Vector2(y, x); }
    Vector2 yy() const { return Vector2(y, y); }
    Vector2 yz() const { return Vector2(y, z); }
    Vector2 zx() const { return Vector2(z, x); }
    Vector2 zy() const { return Vector2(z, y); }
    Vector2 zz() const { return Vector2(z, z); }
    
    Vector3 xxx() const { return Vector3(x, x, x); }
    Vector3 xxy() const { return Vector3(x, x, y); }
    Vector3 xxz() const { return Vector3(x, x, z); }
    Vector3 xyx() const { return Vector3(x, y, x); }
    Vector3 xyy() const { return Vector3(x, y, y); }
    Vector3 xyz() const { return Vector3(x, y, z); }
    Vector3 xzx() const { return Vector3(x, z, x); }
    Vector3 xzy() const { return Vector3(x, z, y); }
    Vector3 xzz() const { return Vector3(x, z, z); }
    Vector3 yxx() const { return Vector3(y, x, x); }
    Vector3 yxy() const { return Vector3(y, x, y); }
    Vector3 yxz() const { return Vector3(y, x, z); }
    Vector3 yyx() const { return Vector3(y, y, x); }
    Vector3 yyy() const { return Vector3(y, y, y); }
    Vector3 yyz() const { return Vector3(y, y, z); }
    Vector3 yzx() const { return Vector3(y, z, x); }
    Vector3 yzy() const { return Vector3(y, z, y); }
    Vector3 yzz() const { return Vector3(y, z, z); }
    Vector3 zxx() const { return Vector3(z, x, x); }
    Vector3 zxy() const { return Vector3(z, x, y); }
    Vector3 zxz() const { return Vector3(z, x, z); }
    Vector3 zyx() const { return Vector3(z, y, x); }
    Vector3 zyy() const { return Vector3(z, y, y); }
    Vector3 zyz() const { return Vector3(z, y, z); }
    Vector3 zzx() const { return Vector3(z, z, x); }
    Vector3 zzy() const { return Vector3(z, z, y); }
    Vector3 zzz() const { return Vector3(z, z, z); }
    
    // Conversion operations
    Vector2 toVector2() const { return Vector2(x, y); }
    Vector4 toVector4(float32 w = 0.0f) const;
    
    // Utility methods
    bool isZero() const {
        return x == 0.0f && y == 0.0f && z == 0.0f;
    }
    
    bool isNearZero(float32 tolerance = 0.0001f) const {
        return std::abs(x) < tolerance && std::abs(y) < tolerance && std::abs(z) < tolerance;
    }
    
    bool isNormalized() const {
        return std::abs(length() - 1.0f) < 0.0001f;
    }
    
    bool isFinite() const {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }
    
    Vector3 abs() const {
        return Vector3(std::abs(x), std::abs(y), std::abs(z));
    }
    
    Vector3 floor() const {
        return Vector3(std::floor(x), std::floor(y), std::floor(z));
    }
    
    Vector3 ceil() const {
        return Vector3(std::ceil(x), std::ceil(y), std::ceil(z));
    }
    
    Vector3 round() const {
        return Vector3(std::round(x), std::round(y), std::round(z));
    }
    
    Vector3 min(const Vector3& other) const {
        return Vector3(std::min(x, other.x), std::min(y, other.y), std::min(z, other.z));
    }
    
    Vector3 max(const Vector3& other) const {
        return Vector3(std::max(x, other.x), std::max(y, other.y), std::max(z, other.z));
    }
    
    Vector3 clamp(const Vector3& min, const Vector3& max) const {
        return Vector3(std::clamp(x, min.x, max.x), std::clamp(y, min.y, max.y), std::clamp(z, min.z, max.z));
    }
    
    Vector3 clamp(float32 min, float32 max) const {
        return Vector3(std::clamp(x, min, max), std::clamp(y, min, max), std::clamp(z, min, max));
    }
    
    // String conversion
    std::string toString() const {
        return "Vector3(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }
    
    // Static methods
    static Vector3 zero() { return Vector3(0.0f, 0.0f, 0.0f); }
    static Vector3 one() { return Vector3(1.0f, 1.0f, 1.0f); }
    static Vector3 unitX() { return Vector3(1.0f, 0.0f, 0.0f); }
    static Vector3 unitY() { return Vector3(0.0f, 1.0f, 0.0f); }
    static Vector3 unitZ() { return Vector3(0.0f, 0.0f, 1.0f); }
    static Vector3 up() { return Vector3(0.0f, 1.0f, 0.0f); }
    static Vector3 down() { return Vector3(0.0f, -1.0f, 0.0f); }
    static Vector3 left() { return Vector3(-1.0f, 0.0f, 0.0f); }
    static Vector3 right() { return Vector3(1.0f, 0.0f, 0.0f); }
    static Vector3 forward() { return Vector3(0.0f, 0.0f, -1.0f); }
    static Vector3 backward() { return Vector3(0.0f, 0.0f, 1.0f); }
    
    static Vector3 fromSpherical(float32 radius, float32 theta, float32 phi) {
        float32 sinPhi = std::sin(phi);
        return Vector3(radius * std::cos(theta) * sinPhi, radius * std::sin(theta) * sinPhi, radius * std::cos(phi));
    }
    
    static Vector3 fromCylindrical(float32 radius, float32 angle, float32 height) {
        return Vector3(radius * std::cos(angle), radius * std::sin(angle), height);
    }
    
    static float32 dot(const Vector3& a, const Vector3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    
    static Vector3 cross(const Vector3& a, const Vector3& b) {
        return a.cross(b);
    }
    
    static float32 distance(const Vector3& a, const Vector3& b) {
        return (a - b).length();
    }
    
    static float32 distanceSquared(const Vector3& a, const Vector3& b) {
        return (a - b).lengthSquared();
    }
    
    static float32 angle(const Vector3& a, const Vector3& b) {
        return a.angleTo(b);
    }
    
    static Vector3 lerp(const Vector3& a, const Vector3& b, float32 t) {
        return a + (b - a) * t;
    }
    
    static Vector3 slerp(const Vector3& a, const Vector3& b, float32 t) {
        return a.slerp(b, t);
    }
    
    static Vector3 nlerp(const Vector3& a, const Vector3& b, float32 t) {
        return a.nlerp(b, t);
    }
    
    static Vector3 reflect(const Vector3& vector, const Vector3& normal) {
        return vector.reflect(normal);
    }
    
    static Vector3 project(const Vector3& vector, const Vector3& onto) {
        return vector.project(onto);
    }
    
    static Vector3 projectOnPlane(const Vector3& vector, const Vector3& normal) {
        return vector.projectOnPlane(normal);
    }
    
    static Vector3 min(const Vector3& a, const Vector3& b) {
        return a.min(b);
    }
    
    static Vector3 max(const Vector3& a, const Vector3& b) {
        return a.max(b);
    }
    
    static Vector3 clamp(const Vector3& value, const Vector3& min, const Vector3& max) {
        return value.clamp(min, max);
    }
    
    static Vector3 clamp(const Vector3& value, float32 min, float32 max) {
        return value.clamp(min, max);
    }
    
    static Vector3 abs(const Vector3& vector) {
        return vector.abs();
    }
    
    static Vector3 floor(const Vector3& vector) {
        return vector.floor();
    }
    
    static Vector3 ceil(const Vector3& vector) {
        return vector.ceil();
    }
    
    static Vector3 round(const Vector3& vector) {
        return vector.round();
    }
    
    // Random vector generation
    static Vector3 random();
    static Vector3 random(float32 min, float32 max);
    static Vector3 random(const Vector3& min, const Vector3& max);
    static Vector3 randomNormalized();
    static Vector3 randomOnSphere(float32 radius = 1.0f);
    static Vector3 randomInSphere(float32 radius = 1.0f);
    static Vector3 randomOnCube(float32 size = 1.0f);
    static Vector3 randomInCube(float32 size = 1.0f);
    static Vector3 randomOnCylinder(float32 radius = 1.0f, float32 height = 1.0f);
    static Vector3 randomInCylinder(float32 radius = 1.0f, float32 height = 1.0f);
    
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
inline Vector3 operator*(float32 scalar, const Vector3& vector) {
    return vector * scalar;
}

inline Vector3 operator/(float32 scalar, const Vector3& vector) {
    return Vector3(scalar / vector.x, scalar / vector.y, scalar / vector.z);
}

// Utility functions
inline Vector3 abs(const Vector3& vector) {
    return vector.abs();
}

inline Vector3 floor(const Vector3& vector) {
    return vector.floor();
}

inline Vector3 ceil(const Vector3& vector) {
    return vector.ceil();
}

inline Vector3 round(const Vector3& vector) {
    return vector.round();
}

inline Vector3 min(const Vector3& a, const Vector3& b) {
    return a.min(b);
}

inline Vector3 max(const Vector3& a, const Vector3& b) {
    return a.max(b);
}

inline Vector3 clamp(const Vector3& value, const Vector3& min, const Vector3& max) {
    return value.clamp(min, max);
}

inline Vector3 clamp(const Vector3& value, float32 min, float32 max) {
    return value.clamp(min, max);
}

inline float32 dot(const Vector3& a, const Vector3& b) {
    return a.dot(b);
}

inline Vector3 cross(const Vector3& a, const Vector3& b) {
    return a.cross(b);
}

inline float32 distance(const Vector3& a, const Vector3& b) {
    return a.distanceTo(b);
}

inline float32 distanceSquared(const Vector3& a, const Vector3& b) {
    return a.distanceSquaredTo(b);
}

inline float32 angle(const Vector3& a, const Vector3& b) {
    return a.angleTo(b);
}

inline Vector3 lerp(const Vector3& a, const Vector3& b, float32 t) {
    return a.lerp(b, t);
}

inline Vector3 slerp(const Vector3& a, const Vector3& b, float32 t) {
    return a.slerp(b, t);
}

inline Vector3 nlerp(const Vector3& a, const Vector3& b, float32 t) {
    return a.nlerp(b, t);
}

inline Vector3 reflect(const Vector3& vector, const Vector3& normal) {
    return vector.reflect(normal);
}

inline Vector3 project(const Vector3& vector, const Vector3& onto) {
    return vector.project(onto);
}

inline Vector3 projectOnPlane(const Vector3& vector, const Vector3& normal) {
    return vector.projectOnPlane(normal);
}

// Hash function for Vector3
struct Vector3Hash {
    size_t operator()(const Vector3& vector) const noexcept {
        size_t h1 = std::hash<float32>{}(vector.x);
        size_t h2 = std::hash<float32>{}(vector.y);
        size_t h3 = std::hash<float32>{}(vector.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

} // namespace RFUtils