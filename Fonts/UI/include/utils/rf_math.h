#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>
#include <limits>
#include <type_traits>

namespace RFUtils {

// Mathematical constants
namespace MathConstants {
    constexpr float32 PI = 3.14159265358979323846f;
    constexpr float32 TWO_PI = 2.0f * PI;
    constexpr float32 HALF_PI = PI / 2.0f;
    constexpr float32 QUARTER_PI = PI / 4.0f;
    constexpr float32 INV_PI = 1.0f / PI;
    constexpr float32 INV_TWO_PI = 1.0f / TWO_PI;
    constexpr float32 DEG_TO_RAD = PI / 180.0f;
    constexpr float32 RAD_TO_DEG = 180.0f / PI;
    constexpr float32 E = 2.71828182845904523536f;
    constexpr float32 LOG2_E = 1.44269504088896340736f;
    constexpr float32 LOG10_E = 0.43429448190325182765f;
    constexpr float32 LN_2 = 0.69314718055994530942f;
    constexpr float32 LN_10 = 2.30258509299404568402f;
    constexpr float32 SQRT_2 = 1.41421356237309504880f;
    constexpr float32 SQRT_3 = 1.73205080756887729353f;
    constexpr float32 GOLDEN_RATIO = 1.61803398874989484820f;
    constexpr float32 EPSILON = std::numeric_limits<float32>::epsilon();
    constexpr float32 INFINITY = std::numeric_limits<float32>::infinity();
    constexpr float32 NAN = std::numeric_limits<float32>::quiet_NaN();
}

// Basic mathematical functions
namespace Math {
    // Absolute value
    template<typename T>
    constexpr T abs(T value) {
        return value < 0 ? -value : value;
    }
    
    // Minimum and maximum
    template<typename T>
    constexpr T min(T a, T b) {
        return a < b ? a : b;
    }
    
    template<typename T>
    constexpr T max(T a, T b) {
        return a > b ? a : b;
    }
    
    template<typename T>
    constexpr T clamp(T value, T min, T max) {
        return value < min ? min : (value > max ? max : value);
    }
    
    // Linear interpolation
    template<typename T>
    constexpr T lerp(T a, T b, T t) {
        return a + (b - a) * t;
    }
    
    // Inverse linear interpolation
    template<typename T>
    constexpr T invLerp(T a, T b, T value) {
        return (value - a) / (b - a);
    }
    
    // Smooth step
    template<typename T>
    constexpr T smoothStep(T edge0, T edge1, T x) {
        T t = clamp((x - edge0) / (edge1 - edge0), T(0), T(1));
        return t * t * (T(3) - T(2) * t);
    }
    
    // Step function
    template<typename T>
    constexpr T step(T edge, T x) {
        return x < edge ? T(0) : T(1);
    }
    
    // Sign function
    template<typename T>
    constexpr T sign(T value) {
        return value > 0 ? T(1) : (value < 0 ? T(-1) : T(0));
    }
    
    // Power functions
    template<typename T>
    constexpr T pow2(T value) {
        return value * value;
    }
    
    template<typename T>
    constexpr T pow3(T value) {
        return value * value * value;
    }
    
    template<typename T>
    constexpr T pow4(T value) {
        return value * value * value * value;
    }
    
    // Square root (approximation for integers)
    template<typename T>
    T sqrt(T value) {
        return std::sqrt(static_cast<double>(value));
    }
    
    // Fast inverse square root (Quake algorithm)
    float32 fastInverseSqrt(float32 value);
    
    // Trigonometric functions
    float32 sin(float32 radians);
    float32 cos(float32 radians);
    float32 tan(float32 radians);
    float32 asin(float32 value);
    float32 acos(float32 value);
    float32 atan(float32 value);
    float32 atan2(float32 y, float32 x);
    
    // Hyperbolic functions
    float32 sinh(float32 value);
    float32 cosh(float32 value);
    float32 tanh(float32 value);
    float32 asinh(float32 value);
    float32 acosh(float32 value);
    float32 atanh(float32 value);
    
    // Exponential and logarithmic functions
    float32 exp(float32 value);
    float32 exp2(float32 value);
    float32 log(float32 value);
    float32 log2(float32 value);
    float32 log10(float32 value);
    
    // Rounding functions
    float32 floor(float32 value);
    float32 ceil(float32 value);
    float32 round(float32 value);
    float32 trunc(float32 value);
    
    // Fractional part
    float32 frac(float32 value);
    
    // Modulo functions
    float32 mod(float32 x, float32 y);
    float32 fmod(float32 x, float32 y);
    
    // Distance functions
    float32 distance(float32 a, float32 b);
    float32 distanceSquared(float32 a, float32 b);
    
    // Comparison functions
    bool isEqual(float32 a, float32 b, float32 epsilon = MathConstants::EPSILON);
    bool isEqualRelative(float32 a, float32 b, float32 maxRelativeError = MathConstants::EPSILON);
    bool isZero(float32 value, float32 epsilon = MathConstants::EPSILON);
    bool isNearZero(float32 value, float32 epsilon = MathConstants::EPSILON);
    bool isFinite(float32 value);
    bool isInfinite(float32 value);
    bool isNaN(float32 value);
    
    // Angle conversion
    float32 degToRad(float32 degrees);
    float32 radToDeg(float32 radians);
    
    // Interpolation functions
    float32 smoothStep(float32 edge0, float32 edge1, float32 x);
    float32 smootherStep(float32 edge0, float32 edge1, float32 x);
    float32 cubicBezier(float32 t, float32 p0, float32 p1, float32 p2, float32 p3);
    
    // Noise functions
    float32 noise1D(float32 x);
    float32 noise2D(float32 x, float32 y);
    float32 noise3D(float32 x, float32 y, float32 z);
    float32 fbm1D(float32 x, int32_t octaves = 4, float32 persistence = 0.5f);
    float32 fbm2D(float32 x, float32 y, int32_t octaves = 4, float32 persistence = 0.5f);
    float32 fbm3D(float32 x, float32 y, float32 z, int32_t octaves = 4, float32 persistence = 0.5f);
    
    // Random functions
    float32 random();
    float32 random(float32 min, float32 max);
    int32_t random(int32_t min, int32_t max);
    float32 randomGaussian(float32 mean = 0.0f, float32 stdDev = 1.0f);
    
    // Hash functions
    uint32_t hash(uint32_t value);
    uint32_t hash(float32 value);
    uint32_t hash(int32_t value);
    uint32_t hashCombine(uint32_t seed, uint32_t hash);
    
    // Bit manipulation
    uint32_t reverseBits(uint32_t value);
    uint32_t countLeadingZeros(uint32_t value);
    uint32_t countTrailingZeros(uint32_t value);
    uint32_t countSetBits(uint32_t value);
    bool isPowerOfTwo(uint32_t value);
    uint32_t nextPowerOfTwo(uint32_t value);
    uint32_t prevPowerOfTwo(uint32_t value);
    
    // Color conversion utilities
    void rgbToHsv(float32 r, float32 g, float32 b, float32& h, float32& s, float32& v);
    void hsvToRgb(float32 h, float32 s, float32 v, float32& r, float32& g, float32& b);
    void rgbToHsl(float32 r, float32 g, float32 b, float32& h, float32& s, float32& l);
    void hslToRgb(float32 h, float32 s, float32 l, float32& r, float32& g, float32& b);
    
    // Matrix operations (forward declarations)
    class Matrix2;
    class Matrix3;
    class Matrix4;
    
    // Vector operations (forward declarations)
    class Vector2;
    class Vector3;
    class Vector4;
    
    // Quaternion operations (forward declaration)
    class Quaternion;
}

// Integer math utilities
namespace IntMath {
    // Integer division with proper rounding
    int32_t divRound(int32_t a, int32_t b);
    int32_t divFloor(int32_t a, int32_t b);
    int32_t divCeil(int32_t a, int32_t b);
    
    // Integer power functions
    int32_t pow(int32_t base, uint32_t exp);
    int32_t pow2(int32_t value);
    int32_t pow3(int32_t value);
    
    // Integer square root
    int32_t sqrt(int32_t value);
    int32_t isqrt(int32_t value); // Integer square root
    
    // Greatest common divisor and least common multiple
    int32_t gcd(int32_t a, int32_t b);
    int32_t lcm(int32_t a, int32_t b);
    
    // Prime number functions
    bool isPrime(int32_t value);
    int32_t nextPrime(int32_t value);
    int32_t prevPrime(int32_t value);
    
    // Factorial functions
    uint64_t factorial(int32_t n);
    uint64_t factorialApprox(int32_t n);
    
    // Binomial coefficients
    uint64_t binomial(int32_t n, int32_t k);
    
    // Fibonacci sequence
    uint64_t fibonacci(int32_t n);
    uint64_t fibonacciIterative(int32_t n);
    
    // Modular arithmetic
    int32_t mod(int32_t a, int32_t b);
    int32_t modInverse(int32_t a, int32_t b);
    int32_t modPow(int32_t base, int32_t exp, int32_t mod);
    
    // Bit manipulation
    bool getBit(uint32_t value, uint32_t index);
    void setBit(uint32_t& value, uint32_t index);
    void clearBit(uint32_t& value, uint32_t index);
    void toggleBit(uint32_t& value, uint32_t index);
    uint32_t extractBits(uint32_t value, uint32_t start, uint32_t count);
    void insertBits(uint32_t& value, uint32_t bits, uint32_t start, uint32_t count);
    
    // Byte manipulation
    uint8_t getByte(uint32_t value, uint32_t index);
    void setByte(uint32_t& value, uint8_t byte, uint32_t index);
    uint32_t swapEndian(uint32_t value);
    uint16_t swapEndian(uint16_t value);
    
    // Interpolation
    int32_t lerp(int32_t a, int32_t b, float32 t);
    int32_t smoothStep(int32_t edge0, int32_t edge1, int32_t x);
}

// Floating-point math utilities
namespace FloatMath {
    // Comparison with epsilon
    bool isEqual(float32 a, float32 b, float32 epsilon = MathConstants::EPSILON);
    bool isEqualRelative(float32 a, float32 b, float32 maxRelativeError = MathConstants::EPSILON);
    bool isZero(float32 value, float32 epsilon = MathConstants::EPSILON);
    bool isNearZero(float32 value, float32 epsilon = MathConstants::EPSILON);
    bool isFinite(float32 value);
    bool isInfinite(float32 value);
    bool isNaN(float32 value);
    
    // Fast approximations
    float32 fastSin(float32 x);
    float32 fastCos(float32 x);
    float32 fastTan(float32 x);
    float32 fastAsin(float32 x);
    float32 fastAcos(float32 x);
    float32 fastAtan(float32 x);
    float32 fastAtan2(float32 y, float32 x);
    float32 fastExp(float32 x);
    float32 fastLog(float32 x);
    float32 fastLog2(float32 x);
    float32 fastPow(float32 base, float32 exp);
    float32 fastSqrt(float32 x);
    
    // Polynomial approximations
    float32 polySin(float32 x, int32_t degree = 5);
    float32 polyCos(float32 x, int32_t degree = 5);
    float32 polyExp(float32 x, int32_t degree = 6);
    float32 polyLog(float32 x, int32_t degree = 6);
    
    // Special functions
    float32 gamma(float32 x);
    float32 beta(float32 a, float32 b);
    float32 erf(float32 x);
    float32 erfc(float32 x);
    
    // Bessel functions
    float32 besselJ0(float32 x);
    float32 besselJ1(float32 x);
    float32 besselY0(float32 x);
    float32 besselY1(float32 x);
    
    // Error functions
    float32 errorFunction(float32 x);
    float32 complementaryErrorFunction(float32 x);
    
    // Interpolation
    float32 cubicHermite(float32 p0, float32 p1, float32 m0, float32 m1, float32 t);
    float32 catmullRom(float32 p0, float32 p1, float32 p2, float32 p3, float32 t);
    float32 kochanekBartels(float32 p0, float32 p1, float32 p2, float32 p3, 
                           float32 tension, float32 bias, float32 continuity, float32 t);
}

// Geometry utilities
namespace Geometry {
    // Point in polygon test
    bool pointInPolygon(const float32* points, int32_t numPoints, float32 x, float32 y);
    bool pointInConvexPolygon(const float32* points, int32_t numPoints, float32 x, float32 y);
    bool pointInTriangle(float32 x, float32 y, float32 x1, float32 y1, float32 x2, float32 y2, float32 x3, float32 y3);
    
    // Distance functions
    float32 pointToLineDistance(float32 px, float32 py, float32 x1, float32 y1, float32 x2, float32 y2);
    float32 pointToSegmentDistance(float32 px, float32 py, float32 x1, float32 y1, float32 x2, float32 y2);
    float32 pointToPlaneDistance(const float32* point, const float32* planeNormal, float32 planeD);
    
    // Intersection tests
    bool lineIntersection(float32 x1, float32 y1, float32 x2, float32 y2, 
                         float32 x3, float32 y3, float32 x4, float32 y4, 
                         float32& ix, float32& iy);
    bool segmentIntersection(float32 x1, float32 y1, float32 x2, float32 y2, 
                           float32 x3, float32 y3, float32 x4, float32 y4, 
                           float32& ix, float32& iy);
    bool rayLineIntersection(float32 rx, float32 ry, float32 rdx, float32 rdy,
                            float32 x1, float32 y1, float32 x2, float32 y2,
                            float32& t);
    
    // Bounding box operations
    void boundingBoxFromPoints(const float32* points, int32_t numPoints, 
                              float32& minX, float32& minY, float32& maxX, float32& maxY);
    bool boundingBoxIntersection(float32 minX1, float32 minY1, float32 maxX1, float32 maxY1,
                                float32 minX2, float32 minY2, float32 maxX2, float32 maxY2);
    
    // Circle operations
    bool circleIntersection(float32 x1, float32 y1, float32 r1, 
                           float32 x2, float32 y2, float32 r2,
                           float32& ix1, float32& iy1, float32& ix2, float32& iy2);
    bool pointInCircle(float32 px, float32 py, float32 cx, float32 cy, float32 radius);
    float32 circleArea(float32 radius);
    float32 circleCircumference(float32 radius);
    
    // Triangle operations
    float32 triangleArea(float32 x1, float32 y1, float32 x2, float32 y2, float32 x3, float32 y3);
    float32 trianglePerimeter(float32 x1, float32 y1, float32 x2, float32 y2, float32 x3, float32 y3);
    bool triangleContainsPoint(float32 px, float32 py, float32 x1, float32 y1, float32 x2, float32 y2, float32 x3, float32 y3);
    void triangleCentroid(float32 x1, float32 y1, float32 x2, float32 y2, float32 x3, float32 y3, float32& cx, float32& cy);
    void triangleIncenter(float32 x1, float32 y1, float32 x2, float32 y2, float32 x3, float32 y3, float32& cx, float32& cy);
    void triangleCircumcenter(float32 x1, float32 y1, float32 x2, float32 y2, float32 x3, float32 y3, float32& cx, float32& cy);
    
    // Polygon operations
    float32 polygonArea(const float32* points, int32_t numPoints);
    float32 polygonPerimeter(const float32* points, int32_t numPoints);
    bool polygonIsConvex(const float32* points, int32_t numPoints);
    void polygonCentroid(const float32* points, int32_t numPoints, float32& cx, float32& cy);
    
    // 3D geometry
    bool pointInSphere(const float32* point, const float32* center, float32 radius);
    bool pointInBox(const float32* point, const float32* min, const float32* max);
    float32 pointToPlaneDistance(const float32* point, const float32* normal, float32 d);
    bool rayPlaneIntersection(const float32* rayOrigin, const float32* rayDir,
                             const float32* planeNormal, float32 planeD,
                             float32& t);
    bool raySphereIntersection(const float32* rayOrigin, const float32* rayDir,
                              const float32* sphereCenter, float32 sphereRadius,
                              float32& t1, float32& t2);
    bool rayBoxIntersection(const float32* rayOrigin, const float32* rayDir,
                           const float32* boxMin, const float32* boxMax,
                           float32& t1, float32& t2);
}

// Statistics utilities
namespace Statistics {
    // Basic statistics
    float32 mean(const float32* data, size_t count);
    float32 median(const float32* data, size_t count);
    float32 mode(const float32* data, size_t count);
    float32 variance(const float32* data, size_t count);
    float32 standardDeviation(const float32* data, size_t count);
    float32 skewness(const float32* data, size_t count);
    float32 kurtosis(const float32* data, size_t count);
    
    // Range and quartiles
    float32 range(const float32* data, size_t count);
    float32 min(const float32* data, size_t count);
    float32 max(const float32* data, size_t count);
    float32 quartile(const float32* data, size_t count, float32 q);
    float32 percentile(const float32* data, size_t count, float32 p);
    
    // Correlation and regression
    float32 correlation(const float32* x, const float32* y, size_t count);
    float32 covariance(const float32* x, const float32* y, size_t count);
    void linearRegression(const float32* x, const float32* y, size_t count, 
                         float32& slope, float32& intercept, float32& r2);
    
    // Probability distributions
    float32 normalPDF(float32 x, float32 mean, float32 stdDev);
    float32 normalCDF(float32 x, float32 mean, float32 stdDev);
    float32 exponentialPDF(float32 x, float32 lambda);
    float32 exponentialCDF(float32 x, float32 lambda);
    float32 uniformPDF(float32 x, float32 a, float32 b);
    float32 uniformCDF(float32 x, float32 a, float32 b);
    
    // Random sampling
    float32 sampleNormal(float32 mean = 0.0f, float32 stdDev = 1.0f);
    float32 sampleExponential(float32 lambda = 1.0f);
    float32 sampleUniform(float32 a = 0.0f, float32 b = 1.0f);
    void sampleNormal(float32* samples, size_t count, float32 mean = 0.0f, float32 stdDev = 1.0f);
    void sampleExponential(float32* samples, size_t count, float32 lambda = 1.0f);
    void sampleUniform(float32* samples, size_t count, float32 a = 0.0f, float32 b = 1.0f);
}

// Utility functions
namespace Utils {
    // Type traits
    template<typename T>
    struct is_numeric : std::integral_constant<bool, std::is_arithmetic<T>::value> {};
    
    template<typename T>
    struct is_integer : std::integral_constant<bool, std::is_integral<T>::value> {};
    
    template<typename T>
    struct is_floating_point : std::integral_constant<bool, std::is_floating_point<T>::value> {};
    
    // Compile-time checks
    template<typename T>
    constexpr bool isNumeric() { return is_numeric<T>::value; }
    
    template<typename T>
    constexpr bool isInteger() { return is_integer<T>::value; }
    
    template<typename T>
    constexpr bool isFloatingPoint() { return is_floating_point<T>::value; }
    
    // Safe arithmetic operations
    template<typename T>
    bool safeAdd(T a, T b, T& result);
    
    template<typename T>
    bool safeSub(T a, T b, T& result);
    
    template<typename T>
    bool safeMul(T a, T b, T& result);
    
    template<typename T>
    bool safeDiv(T a, T b, T& result);
    
    // Memory utilities
    void* alignedMalloc(size_t size, size_t alignment);
    void alignedFree(void* ptr);
    void* reallocAligned(void* ptr, size_t newSize, size_t alignment);
    
    // Hash utilities
    uint32_t hashBytes(const void* data, size_t length);
    uint64_t hashBytes64(const void* data, size_t length);
    uint32_t hashString(const char* str);
    uint64_t hashString64(const char* str);
    
    // Time utilities
    uint64_t getCurrentTimeMicroseconds();
    uint64_t getCurrentTimeNanoseconds();
    void sleep(uint32_t milliseconds);
    void sleepMicroseconds(uint32_t microseconds);
}

// Mathematical constants and functions
using namespace MathConstants;
using namespace Math;
using namespace IntMath;
using namespace FloatMath;
using namespace Geometry;
using namespace Statistics;
using namespace Utils;

} // namespace RFUtils