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

// 4D Vector class
class Vector4 {
public:
    float32 x, y, z, w;
    
    // Constructors
    Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vector4(float32 x, float32 y, float32 z, float32 w) : x(x), y(y), z(z), w(w) {}
    Vector4(float32 value) : x(value), y(value), z(value), w(value) {}
    explicit Vector4(const float32* data) : x(data[0]), y(data[1]), z(data[2]), w(data[3]) {}
    Vector4(const std::array<float32, 4>& data) : x(data[0]), y(data[1]), z(data[2]), w(data[3]) {}
    Vector4(const Vector2& vector, float32 z = 0.0f, float32 w = 0.0f) : x(vector.x), y(vector.y), z(z), w(w) {}
    Vector4(const Vector3& vector, float32 w = 0.0f) : x(vector.x), y(vector.y), z(vector.z), w(w) {}
    
    // Copy constructor
    Vector4(const Vector4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
    
    // Assignment operator
    Vector4& operator=(const Vector4& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }
    
    // Component access
    float32& operator[](size_t index) { return (&x)[index]; }
    const float32& operator[](size_t index) const { return (&x)[index]; }
    
    // Array access
    float32* data() { return &x; }
    const float32* data() const { return &x; }
    
    // Comparison operators
    bool operator==(const Vector4& other) const {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }
    
    bool operator!=(const Vector4& other) const {
        return !(*this == other);
    }
    
    // Arithmetic operators
    Vector4 operator+(const Vector4& other) const {
        return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
    }
    
    Vector4 operator-(const Vector4& other) const {
        return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
    }
    
    Vector4 operator*(const Vector4& other) const {
        return Vector4(x * other.x, y * other.y, z * other.z, w * other.w);
    }
    
    Vector4 operator/(const Vector4& other) const {
        return Vector4(x / other.x, y / other.y, z / other.z, w / other.w);
    }
    
    Vector4 operator*(float32 scalar) const {
        return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
    }
    
    Vector4 operator/(float32 scalar) const {
        return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
    }
    
    // Assignment arithmetic operators
    Vector4& operator+=(const Vector4& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }
    
    Vector4& operator-=(const Vector4& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }
    
    Vector4& operator*=(const Vector4& other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        return *this;
    }
    
    Vector4& operator/=(const Vector4& other) {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        w /= other.w;
        return *this;
    }
    
    Vector4& operator*=(float32 scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }
    
    Vector4& operator/=(float32 scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }
    
    // Unary operators
    Vector4 operator+() const {
        return *this;
    }
    
    Vector4 operator-() const {
        return Vector4(-x, -y, -z, -w);
    }
    
    // Vector operations
    float32 dot(const Vector4& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }
    
    float32 length() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }
    
    float32 lengthSquared() const {
        return x * x + y * y + z * z + w * w;
    }
    
    float32 distanceTo(const Vector4& other) const {
        return (*this - other).length();
    }
    
    float32 distanceSquaredTo(const Vector4& const other) const {
        return (*this - other).lengthSquared();
    }
    
    float32 angleTo(const Vector4& other) const {
        return std::acos(dot(other) / (length() * other.length()));
    }
    
    Vector4 normalized() const {
        float32 len = length();
        if (len > 0.0f) {
            return *this / len;
        }
        return Vector4();
    }
    
    Vector4& normalize() {
        float32 len = length();
        if (len > 0.0f) {
            *this /= len;
        }
        return *this;
    }
    
    Vector4 reflect(const Vector4& normal) const {
        return *this - normal * (2.0f * dot(normal));
    }
    
    Vector4 project(const Vector4& other) const {
        return other * (dot(other) / other.lengthSquared());
    }
    
    Vector4 projectOnHyperplane(const Vector4& normal) const {
        return *this - project(normal);
    }
    
    Vector4 lerp(const Vector4& other, float32 t) const {
        return *this + (other - *this) * t;
    }
    
    Vector4 slerp(const Vector4& other, float32 t) const {
        float32 angle = angleTo(other);
        float32 sinAngle = std::sin(angle);
        
        if (sinAngle < 0.001f) {
            return lerp(other, t);
        }
        
        float32 factor1 = std::sin((1.0f - t) * angle) / sinAngle;
        float32 factor2 = std::sin(t * angle) / sinAngle;
        
        return *this * factor1 + other * factor2;
    }
    
    Vector4 nlerp(const Vector4& other, float32 t) const {
        Vector4 result = lerp(other, t);
        return result.normalized();
    }
    
    // Swizzle operations
    Vector2 xx() const { return Vector2(x, x); }
    Vector2 xy() const { return Vector2(x, y); }
    Vector2 xz() const { return Vector2(x, z); }
    Vector2 xw() const { return Vector2(x, w); }
    Vector2 yx() const { return Vector2(y, x); }
    Vector2 yy() const { return Vector2(y, y); }
    Vector2 yz() const { return Vector2(y, z); }
    Vector2 yw() const { return Vector2(y, w); }
    Vector2 zx() const { return Vector2(z, x); }
    Vector2 zy() const { return Vector2(z, y); }
    Vector2 zz() const { return Vector2(z, z); }
    Vector2 zw() const { return Vector2(z, w); }
    Vector2 wx() const { return Vector2(w, x); }
    Vector2 wy() const { return Vector2(w, y); }
    Vector2 wz() const { return Vector2(w, z); }
    Vector2 ww() const { return Vector2(w, w); }
    
    Vector3 xxx() const { return Vector3(x, x, x); }
    Vector3 xxy() const { return Vector3(x, x, y); }
    Vector3 xxz() const { return Vector3(x, x, z); }
    Vector3 xxw() const { return Vector3(x, x, w); }
    Vector3 xyx() const { return Vector3(x, y, x); }
    Vector3 xyy() const { return Vector3(x, y, y); }
    Vector3 yxz() const { return Vector3(x, y, z); }
    Vector3 xyw() const { return Vector3(x, y, w); }
    Vector3 xzx() const { return Vector3(x, z, x); }
    Vector3 zyx() const { return Vector3(z, y, x); }
    Vector3 zzy() const { return Vector3(z, z, y); }
    Vector3 zzw() const { return Vector3(z, z, w); }
    Vector3 xwx() const { return Vector3(x, w, x); }
    Vector3 ywx() const { return Vector3(y, w, x); }
    Vector3 wwx() const { return Vector3(w, w, x); }
    Vector3 wwy() const { return Vector3(w, w, y); }
    Vector3 wwz() const { return Vector3(w, w, z); }
    Vector3 www() const { return Vector3(w, w, w); }
    
    Vector4 xxxx() const { return Vector4(x, x, x, x); }
    Vector4 xxxy() const { return Vector4(x, x, x, y); }
    Vector4 xxxz() const { return Vector4(x, x, x, z); }
    Vector4 xxxw() const { return Vector4(x, x, x, w); }
    Vector4 xxyx() const { return Vector4(x, x, y, x); }
    Vector4 xxyy() const { return Vector4(x, x, y, y); }
    Vector4 xxyz() const { return Vector4(x, x, y, z); }
    Vector4 xxyw() const { return Vector4(x, x, y, w); }
    Vector4 xxzx() const { return Vector4(x, x, z, x); }
    Vector4 xxzy() const { return Vector4(x, x, z, y); }
    Vector4 xxzz() const { return Vector4(x, x, z, z); }
    Vector4 xxzw() const { return Vector4(x, x, z, w); }
    Vector4 xyxx() const { return Vector4(x, y, x, x); }
    Vector4 xyxy() const { return Vector4(x, y, x, y); }
    Vector4 xyzx() const { return Vector4(x, y, z, x); }
    Vector4 xyzy() const { return Vector4(x, y, z, y); }
    Vector4 xyzz() const { return Vector4(x, y, z, z); }
    Vector4 xyw() const { return Vector4(x, y, z, w); }
    Vector4 xzxx() const { return Vector4(x, z, x, x); }
    Vector4 xzxy() const { return Vector4(x, z, x, y); }
    Vector4 xzzx() const { return Vector4(x, z, z, x); }
    Vector4 xzzy() const { return Vector4(x, z, z, y); }
    Vector4 xzzz() const { return Vector4(x, z, z, z); }
    Vector4 xzw() const { return Vector4(x, z, z, w); }
    Vector4 xwx() const { return Vector4(x, w, x, x); }
    Vector4 xwy() const { return Vector4(x, w, x, y); }
    Vector4 xwz() const { return Vector4(x, w, z, x); }
    Vector4 xwz() const { return Vector4(x, w, z, z); }
    Vector4 xww() const { return Vector4(x, w, w, w); }
    Vector4 yxxx() const { return Vector4(y, x, x, x); }
    Vector4 yxxy() const { return Vector4(y, x, x, y); }
    Vector4 yxxz() const { return Vector4(y, x, x, z); }
    Vector4 yxxw() const { return Vector4(y, x, x, w); }
    Vector4 yxyx() const { return Vector4(y, x, y, x); }
    Vector4 yxyy() const { return Vector4(y, x, y, y); }
    Vector4 yxyz() const { return Vector4(y, x, y, z); }
    Vector4 yxyw() const { return Vector4(y, x, y, w); }
    Vector4 yzxx() const { return Vector4(y, z, x, x); }
    Vector4 yzxy() const { return Vector4(y, z, x, y); }
    Vector4 yzzx() const { return Vector4(y, z, z, x); }
    Vector4 yzzy() const { return Vector4(y, z, z, y); }
    Vector4 yzzz() const { return Vector4(y, z, z, z); }
    Vector4 yzw() const { return Vector4(y, z, z, w); }
    Vector4 ywxx() const { return Vector4(y, w, x, x); }
    Vector4 ywy() const { return Vector4(y, w, x, y); }
    Vector4 ywz() const { return Vector4(y, w, z, x); }
    Vector4 ywz() const { return Vector4(y, w, z, z); }
    Vector4 yww() const { return Vector4(y, w, w, w); }
    Vector4 zxxx() const { return Vector4(z, x, x, x); }
    Vector4 zxxy() const { return Vector4(z, x, x, y); }
    Vector4 zxxz() const { return Vector4(z, x, x, z); }
    Vector4 zxxw() const { return Vector4(z, x, x, w); }
    Vector4 zyxx() const { return Vector4(z, y, x, x); }
    Vector4 zyxy() const { return Vector4(z, y, x, y); }
    Vector4 zyyx() const { return Vector4(z, y, y, x); }
    Vector4 zyyy() const { return Vector4(z, y, y, y); }
    Vector4 zyzx() const { return Vector4(z, y, z, x); }
    Vector4 zyzy() const { return Vector4(z, y, z, y); }
    Vector4 zzzz() const { return Vector4(z, y, z, z); }
    Vector4 zzw() const { return Vector4(z, y, z, w); }
    Vector4 zwxx() const { return Vector4(z, w, x, x); }
    Vector4 zwy() const { return Vector4(z, w, x, y); }
    Vector4 zwz() const { return Vector4(z, w, z, x); }
    Vector4 zww() const { return Vector4(z, w, z, z); }
    Vector4 wxxx() const { return Vector4(w, x, x, x); }
    Vector4 wxxy() const { return Vector4(w, x, x, y); }
    Vector4 wxxz() const { return Vector4(w, x, x, z); }
    Vector4 wxxw() const { return Vector4(w, x, x, w); }
    Vector4 wyxx() const { return Vector4(w, y, x, x); }
    Vector4 wyxy() const { return Vector4(w, y, x, y); }
    Vector4 wyyx() const { return Vector4(w, y, y, x); }
    Vector4 wyyy() const { return Vector4(w, y, y, y); }
    Vector4 wyzx() const { return Vector4(w, y, z, x); }
    Vector4 wyzy() const { return Vector4(w, y, z, y); }
    Vector4 wyzz() const { return Vector4(w, y, z, z); }
    Vector4 wzw() const { return Vector4(w, y, z, w); }
    Vector4 wxxx() const { return Vector4(w, w, x, x); }
    Vector4 wxy() const { return Vector4(w, w, x, y); }
    Vector4 wxz() const { return Vector4(w, w, z, x); }
    Vector4 wzz() const { return Vector4(w, w, z, z); }
    Vector4 www() const { return Vector4(w, w, w, w); }
    
    // Conversion operations
    Vector2 toVector2() const { return Vector2(x, y); }
    Vector3 toVector3() const { return Vector3(x, y, z); }
    
    // Utility methods
    bool isZero() const {
        return x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f;
    }
    
    bool isNearZero(float32 tolerance = 0.0001f) const {
        return std::abs(x) < tolerance && std::abs(y) < tolerance && 
               std::abs(z) < tolerance && std::abs(w) < tolerance;
    }
    
    bool isNormalized() const {
        return std::abs(length() - 1.0f) < 0.0001f;
    }
    
    bool isFinite() const {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(w);
    }
    
    Vector4 abs() const {
        return Vector4(std::abs(x), std::abs(y), std::abs(z), std::abs(w));
    }
    
    Vector4 floor() const {
        return Vector4(std::floor(x), std::floor(y), std::floor(z), std::floor(w));
    }
    
    Vector4 ceil() const {
        return Vector4(std::ceil(x), std::ceil(y), std::ceil(z), std::ceil(w));
    }
    
    Vector4 round() const {
        return Vector4(std::round(x), std::round(y), std::round(z), std::round(w));
    }
    
    Vector4 min(const Vector4& other) const {
        return Vector4(std::min(x, other.x), std::min(y, other.y), std::min(z, other.z), std::min(w, other.w));
    }
    
    Vector4 max(const Vector4& other) const {
        return Vector4(std::max(x, other.x), std::max(y, other.y), std::max(z, other.z), std::max(w, other.w));
    }
    
    Vector4 clamp(const Vector4& min, const Vector4& max) const {
        return Vector4(std::clamp(x, min.x, max.x), std::clamp(y, min.y, max.y), 
                        std::clamp(z, min.z, max.z), std::clamp(w, min.w, max.w));
    }
    
    Vector4 clamp(float32 min, float32 max) const {
        return Vector4(std::clamp(x, min, max), std::clamp(y, min, max), 
                        std::clamp(z, min, max), std::clamp(w, min, max));
    }
    
    // String conversion
    std::string toString() const {
        return "Vector4(" + std::to_string(x) + ", " + std::to_string(y) + ", " + 
               std::to_string(z) + ", " + std::to_string(w) + ")";
    }
    
    // Static methods
    static Vector4 zero() { return Vector4(0.0f, 0.0f, 0.0f, 0.0f); }
    static Vector4 one() { return Vector4(1.0f, 1.0f, 1.0f, 1.0f); }
    static Vector4 unitX() { return Vector4(1.0f, 0.0f, 0.0f, 0.0f); }
    static Vector4 unitY() { return Vector4(0.0f, 1.0f, 0.0f, 0.0f); }
    static Vector4 unitZ() { return Vector4(0.0f, 0.0f, 1.0f, 0.0f); }
    static Vector4 unitW() { return Vector4(0.0f, 0.0f, 0.0f, 1.0f); }
    
    static Vector4 fromSpherical(float32 radius, float32 theta, float32 phi, float32 psi) {
        float32 sinPhi = std::sin(phi);
        float32 cosPhi = std::cos(phi);
        float32 sinPsi = std::sin(psi);
        float32 cosPsi = std::cos(psi);
        return Vector4(radius * std::cos(theta) * sinPhi * cosPsi, 
                        radius * std::sin(theta) * sinPhi * cosPsi, 
                        radius * cosPhi, radius * sinPhi);
    }
    
    static Vector4 fromCylindrical(float32 radius, float32 angle, float32 height, float32 w) {
        return Vector4(radius * std::cos(angle), radius * std::sin(angle), height, w);
    }
    
    static float32 dot(const Vector4& a, const Vector4& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }
    
    static float32 distance(const Vector4& a, const Vector4& b) {
        return (a - b).length();
    }
    
    static float32 distanceSquared(const Vector4& a, const Vector4& b) {
        return (a - b).lengthSquared();
    }
    
    static float32 angle(const Vector4& a, const Vector4& b) {
        return a.angleTo(b);
    }
    
    static Vector4 lerp(const Vector4& a, const Vector4& b, float32 t) {
        return a + (b - a) * t;
    }
    
    static Vector4 slerp(const Vector4& a, const Vector4& b, float32 t) {
        return a.slerp(b, t);
    }
    
    static Vector4 nlerp(const Vector4& a, const Vector4& b, float32 t) {
        return a.nlerp(b, t);
    }
    
    static Vector4 reflect(const Vector4& vector, const Vector4& normal) {
        return vector.reflect(normal);
    }
    
    static Vector4 project(const Vector4& vector, const Vector4& onto) {
        return vector.project(onto);
    }
    
    static Vector4 projectOnHyperplane(const Vector4& vector, const Vector4& normal) {
        return vector.projectOnHyperplane(normal);
    }
    
    static Vector4 min(const Vector4& a, const Vector4& b) {
        return a.min(b);
    }
    
    static Vector4 max(const Vector4& a, const Vector4& b) {
        return a.max(b);
    }
    
    static Vector4 clamp(const Vector4& value, const Vector4& min, const Vector4& max) {
        return value.clamp(min, max);
    }
    
    static Vector4 clamp(const Vector4& value, float32 min, float32 max) {
        return value.clamp(min, max);
    }
    
    static Vector4 abs(const Vector4& vector) {
        return vector.abs();
    }
    
    static Vector4 floor(const Vector4& vector) {
        return vector.floor();
    }
    
    static Vector4 ceil(const Vector4& vector) {
        return vector.ceil();
    }
    
    static Vector4 round(const Vector4& vector) {
        return vector.round();
    }
    
    // Random vector generation
    static Vector4 random();
    static Vector4 random(float32 min, float32 max);
    static Vector4 random(const Vector4& min, const Vector4& max);
    static Vector4 randomNormalized();
    static Vector4 randomOnSphere(float32 radius = 1.0f);
    static Vector4 randomInSphere(float32 radius = 1.0f);
    static Vector4 randomOnHyperSphere(float32 radius = 1.0f);
    static Vector4 randomInHyperSphere(float32 radius = 1.0f);
    static Vector4 randomOnCube(float32 size = 1.0f);
    static Vector4 randomInCube(float32 size = 1.0f);
    static Vector4 randomOnCylinder(float32 radius = 1.0f, float32 height = 1.0f, float32 w = 0.0f);
    static Vector4 randomInCylinder(float32 radius = 1.0f, float32 height = 1.0f, float32 w = 0.0f);
    
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
inline Vector4 operator*(float32 scalar, const Vector4& vector) {
    return vector * scalar;
}

inline Vector4 operator/(float32 scalar, const Vector4& vector) {
    return Vector4(scalar / vector.x, scalar / vector.y, scalar / vector.z, scalar / vector.w);
}

// Utility functions
inline Vector4 abs(const Vector4& vector) {
    return vector.abs();
}

inline Vector4 floor(const Vector4& vector) {
    return vector.floor();
}

inline Vector4 ceil(const Vector4& vector) {
    return vector.ceil();
}

inline Vector4 round(const Vector4& vector) {
    return vector.round();
}

inline Vector4 min(const Vector4& a, const Vector4& b) {
    return a.min(b);
}

inline Vector4 max(const Vector4& a, const Vector4& b) {
    return a.max(b);
}

inline Vector4 clamp(const Vector4& value, const Vector4& min, const Vector4& max) {
    return value.clamp(min, max);
}

inline Vector4 clamp(const Vector4& value, float32 min, float32 max) {
    return value.clamp(min, max);
}

inline float32 dot(const Vector4& a, const Vector4& b) {
    return a.dot(b);
}

inline float32 distance(const Vector4& a, const Vector4& b) {
    return a.distanceTo(b);
}

inline float32 distanceSquared(const Vector4& a, const Vector4& b) {
    return a.distanceSquaredTo(b);
}

inline float32 angle(const Vector4& a, const Vector4& b) {
    return a.angleTo(b);
}

inline Vector4 lerp(const Vector4& a, const Vector4& b, float32 t) {
    return a.lerp(b, t);
}

inline Vector4 slerp(const Vector4& a, const Vector4& b, float32 t) {
    return a.slerp(b, t);
}

inline Vector4 nlerp(const Vector4& a, const Vector4& b, float32 t) {
    return a.nlerp(b, t);
}

inline Vector4 reflect(const Vector4& vector, const Vector4& normal) {
    return vector.reflect(normal);
}

inline Vector4 project(const Vector4& vector, const Vector4& onto) {
    return vector.project(onto);
}

inline Vector4 projectOnHyperplane(const Vector4& vector, const Vector4& normal) {
    return vector.projectOnHyperplane(normal);
}

// Hash function for Vector4
struct Vector4Hash {
    size_t operator()(const Vector4& vector) const noexcept {
        size_t h1 = std::hash<float32>{}(vector.x);
        size_t h2 = std::hash<float32>{}(vector.y);
        size_t h3 = std::hash<float32>{}(vector.z);
        size_t h4 = std::hash<float32>{}(vector.w);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h3 << 3);
    }
};

} // namespace RFUtils