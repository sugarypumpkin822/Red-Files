#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <algorithm>
#include <cmath>

namespace RFUtils {

// Forward declarations
class Vector2;
class Vector3;
class Vector4;

// Color space enumeration
enum class ColorSpace {
    RGB,
    HSV,
    HSL,
    CMYK,
    XYZ,
    LAB,
    LCH,
    YUV,
    YCbCr,
    Grayscale
};

// Color format enumeration
enum class ColorFormat {
    RGB8,      // 8-bit per channel RGB (24-bit)
    RGBA8,     // 8-bit per channel RGBA (32-bit)
    RGB16,     // 16-bit per channel RGB (48-bit)
    RGBA16,    // 16-bit per channel RGBA (64-bit)
    RGB32,     // 32-bit per channel RGB (96-bit)
    RGBA32,    // 32-bit per channel RGBA (128-bit)
    BGR8,      // 8-bit per channel BGR (24-bit)
    BGRA8,     // 8-bit per channel BGRA (32-bit)
    ARGB8,     // 8-bit per channel ARGB (32-bit)
    ABGR8,     // 8-bit per channel ABGR (32-bit)
    RGB565,    // 16-bit RGB (5-6-5)
    RGBA5551,  // 16-bit RGBA (5-5-5-1)
    RGBA4444,  // 16-bit RGBA (4-4-4-4)
    Grayscale8, // 8-bit grayscale
    Grayscale16, // 16-bit grayscale
    HSV32,     // 32-bit HSV (float32 per channel)
    HSL32,     // 32-bit HSL (float32 per channel)
    CMYK32,    // 32-bit CMYK (float32 per channel)
    XYZ32,     // 32-bit XYZ (float32 per channel)
    LAB32,     // 32-bit LAB (float32 per channel)
    LCH32,     // 32-bit LCH (float32 per channel)
    YUV32,     // 32-bit YUV (float32 per channel)
    YCbCr32    // 32-bit YCbCr (float32 per channel)
};

// Color class
class Color {
public:
    float32 r, g, b, a;
    
    // Constructors
    Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
    Color(float32 r, float32 g, float32 b, float32 a = 1.0f) : r(r), g(g), b(b), a(a) {}
    Color(float32 gray, float32 a = 1.0f) : r(gray), g(gray), b(gray), a(a) {}
    Color(uint32_t rgba) { fromRGBA(rgba); }
    Color(const float32* components) : r(components[0]), g(components[1]), b(components[2]), a(components[3]) {}
    Color(const std::array<float32, 4>& components) : r(components[0]), g(components[1]), b(components[2]), a(components[3]) {}
    Color(const Vector3& rgb, float32 a = 1.0f);
    Color(const Vector4& rgba);
    
    // Copy constructor
    Color(const Color& other) : r(other.r), g(other.g), b(other.b), a(other.a) {}
    
    // Assignment operator
    Color& operator=(const Color& other) {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
        return *this;
    }
    
    // Component access
    float32& operator[](size_t index) { return (&r)[index]; }
    const float32& operator[](size_t index) const { return (&r)[index]; }
    
    // Array access
    float32* data() { return &r; }
    const float32* data() const { return &r; }
    
    // Comparison operators
    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
    
    bool operator!=(const Color& other) const {
        return !(*this == other);
    }
    
    // Arithmetic operators
    Color operator+(const Color& other) const {
        return Color(r + other.r, g + other.g, b + other.b, a + other.a);
    }
    
    Color operator-(const Color& other) const {
        return Color(r - other.r, g - other.g, b - other.b, a - other.a);
    }
    
    Color operator*(const Color& other) const {
        return Color(r * other.r, g * other.g, b * other.b, a * other.a);
    }
    
    Color operator/(const Color& other) const {
        return Color(r / other.r, g / other.g, b / other.b, a / other.a);
    }
    
    Color operator*(float32 scalar) const {
        return Color(r * scalar, g * scalar, b * scalar, a * scalar);
    }
    
    Color operator/(float32 scalar) const {
        return Color(r / scalar, g / scalar, b / scalar, a / scalar);
    }
    
    // Assignment arithmetic operators
    Color& operator+=(const Color& other) {
        r += other.r;
        g += other.g;
        b += other.b;
        a += other.a;
        return *this;
    }
    
    Color& operator-=(const Color& other) {
        r -= other.r;
        g -= other.g;
        b -= other.b;
        a -= other.a;
        return *this;
    }
    
    Color& operator*=(const Color& other) {
        r *= other.r;
        g *= other.g;
        b *= other.b;
        a *= other.a;
        return *this;
    }
    
    Color& operator/=(const Color& other) {
        r /= other.r;
        g /= other.g;
        b /= other.b;
        a /= other.a;
        return *this;
    }
    
    Color& operator*=(float32 scalar) {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        a *= scalar;
        return *this;
    }
    
    Color& operator/=(float32 scalar) {
        r /= scalar;
        g /= scalar;
        b /= scalar;
        a /= scalar;
        return *this;
    }
    
    // Unary operators
    Color operator+() const {
        return *this;
    }
    
    Color operator-() const {
        return Color(-r, -g, -b, -a);
    }
    
    // Color operations
    float32 luminance() const;
    float32 brightness() const;
    float32 saturation() const;
    float32 hue() const;
    
    Color grayscale() const;
    Color invert() const;
    Color complement() const;
    Color clamp() const;
    Color saturate(float32 factor) const;
    Color brighten(float32 factor) const;
    Color darken(float32 factor) const;
    Color fade(float32 factor) const;
    
    Color blend(const Color& other, float32 t) const;
    Color multiply(const Color& other) const;
    Color screen(const Color& other) const;
    Color overlay(const Color& other) const;
    Color softLight(const Color& other) const;
    Color hardLight(const Color& other) const;
    Color dodge(const Color& other) const;
    Color burn(const Color& other) const;
    
    // Color space conversions
    void toHSV(float32& h, float32& s, float32& v) const;
    void toHSL(float32& h, float32& s, float32& l) const;
    void toCMYK(float32& c, float32& m, float32& y, float32& k) const;
    void toXYZ(float32& x, float32& y, float32& z) const;
    void toLAB(float32& l, float32& a, float32& b) const;
    void toLCH(float32& l, float32& c, float32& h) const;
    void toYUV(float32& y, float32& u, float32& v) const;
    void toYCbCr(float32& y, float32& cb, float32& cr) const;
    
    static Color fromHSV(float32 h, float32 s, float32 v, float32 a = 1.0f);
    static Color fromHSL(float32 h, float32 s, float32 l, float32 a = 1.0f);
    static Color fromCMYK(float32 c, float32 m, float32 y, float32 k, float32 a = 1.0f);
    static Color fromXYZ(float32 x, float32 y, float32 z, float32 a = 1.0f);
    static Color fromLAB(float32 l, float32 a, float32 b, float32 alpha = 1.0f);
    static Color fromLCH(float32 l, float32 c, float32 h, float32 alpha = 1.0f);
    static Color fromYUV(float32 y, float32 u, float32 v, float32 a = 1.0f);
    static Color fromYCbCr(float32 y, float32 cb, float32 cr, float32 a = 1.0f);
    
    // Format conversions
    uint32_t toRGBA() const;
    uint32_t toARGB() const;
    uint32_t toBGRA() const;
    uint32_t toABGR() const;
    uint16_t toRGB565() const;
    uint16_t toRGBA5551() const;
    uint16_t toRGBA4444() const;
    
    void fromRGBA(uint32_t rgba);
    void fromARGB(uint32_t argb);
    void fromBGRA(uint32_t bgra);
    void fromABGR(uint32_t abgr);
    void fromRGB565(uint16_t rgb565);
    void fromRGBA5551(uint16_t rgba5551);
    void fromRGBA4444(uint16_t rgba4444);
    
    // Utility methods
    bool isZero() const {
        return r == 0.0f && g == 0.0f && b == 0.0f && a == 0.0f;
    }
    
    bool isTransparent() const {
        return a <= 0.0f;
    }
    
    bool isOpaque() const {
        return a >= 1.0f;
    }
    
    bool isValid() const {
        return r >= 0.0f && r <= 1.0f && g >= 0.0f && g <= 1.0f && 
               b >= 0.0f && b <= 1.0f && a >= 0.0f && a <= 1.0f;
    }
    
    Color abs() const {
        return Color(std::abs(r), std::abs(g), std::abs(b), std::abs(a));
    }
    
    Color min(const Color& other) const {
        return Color(std::min(r, other.r), std::min(g, other.g), 
                    std::min(b, other.b), std::min(a, other.a));
    }
    
    Color max(const Color& other) const {
        return Color(std::max(r, other.r), std::max(g, other.g), 
                    std::max(b, other.b), std::max(a, other.a));
    }
    
    Color clamp(const Color& min, const Color& max) const {
        return Color(std::clamp(r, min.r, max.r), std::clamp(g, min.g, max.g), 
                    std::clamp(b, min.b, max.b), std::clamp(a, min.a, max.a));
    }
    
    Color clamp(float32 min, float32 max) const {
        return Color(std::clamp(r, min, max), std::clamp(g, min, max), 
                    std::clamp(b, min, max), std::clamp(a, min, max));
    }
    
    // String conversion
    std::string toString() const;
    std::string toHexString() const;
    
    // Static methods
    static Color zero() { return Color(0.0f, 0.0f, 0.0f, 0.0f); }
    static Color black() { return Color(0.0f, 0.0f, 0.0f, 1.0f); }
    static Color white() { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
    static Color red() { return Color(1.0f, 0.0f, 0.0f, 1.0f); }
    static Color green() { return Color(0.0f, 1.0f, 0.0f, 1.0f); }
    static Color blue() { return Color(0.0f, 0.0f, 1.0f, 1.0f); }
    static Color yellow() { return Color(1.0f, 1.0f, 0.0f, 1.0f); }
    static Color magenta() { return Color(1.0f, 0.0f, 1.0f, 1.0f); }
    static Color cyan() { return Color(0.0f, 1.0f, 1.0f, 1.0f); }
    static Color orange() { return Color(1.0f, 0.5f, 0.0f, 1.0f); }
    static Color purple() { return Color(0.5f, 0.0f, 1.0f, 1.0f); }
    static Color brown() { return Color(0.6f, 0.3f, 0.1f, 1.0f); }
    static Color gray() { return Color(0.5f, 0.5f, 0.5f, 1.0f); }
    static Color pink() { return Color(1.0f, 0.75f, 0.8f, 1.0f); }
    static Color lime() { return Color(0.0f, 1.0f, 0.0f, 1.0f); }
    static Color navy() { return Color(0.0f, 0.0f, 0.5f, 1.0f); }
    static Color teal() { return Color(0.0f, 0.5f, 0.5f, 1.0f); }
    static Color olive() { return Color(0.5f, 0.5f, 0.0f, 1.0f); }
    static Color maroon() { return Color(0.5f, 0.0f, 0.0f, 1.0f); }
    
    static Color fromHex(const std::string& hex);
    static Color fromHex(uint32_t hex);
    static Color fromString(const std::string& str);
    
    static Color lerp(const Color& a, const Color& b, float32 t);
    static Color slerp(const Color& a, const Color& b, float32 t);
    static Color nlerp(const Color& a, const Color& b, float32 t);
    
    static Color min(const Color& a, const Color& b);
    static Color max(const Color& a, const Color& b);
    static Color clamp(const Color& value, const Color& min, const Color& max);
    static Color clamp(const Color& value, float32 min, float32 max);
    
    static Color abs(const Color& color);
    
    // Random color generation
    static Color random();
    static Color random(float32 min, float32 max);
    static Color random(const Color& min, const Color& max);
    static Color randomHSV();
    static Color randomHSL();
    static Color randomRGB();
    static Color randomGrayscale();
    static Color randomPastel();
    static Color randomNeon();
    static Color randomWarm();
    static Color randomCool();
    static Color randomMonochromatic();
    
    // Color temperature
    static Color fromTemperature(float32 kelvin);
    float32 toTemperature() const;
    
    // Color wheel
    static Color fromWheel(float32 angle, float32 saturation = 1.0f, float32 value = 1.0f);
    float32 getWheelAngle() const;
    Color getComplementary() const;
    Color getTriadic(size_t index) const;
    Color getTetradic(size_t index) const;
    Color getAnalogous(size_t index) const;
    Color getSplitComplementary() const;
    
    // Constants
    static constexpr float32 EPSILON = 0.0001f;
};

// Global operators for scalar operations
inline Color operator*(float32 scalar, const Color& color) {
    return color * scalar;
}

inline Color operator/(float32 scalar, const Color& color) {
    return Color(scalar / color.r, scalar / color.g, scalar / color.b, scalar / color.a);
}

// Utility functions
inline Color abs(const Color& color) {
    return color.abs();
}

inline Color min(const Color& a, const Color& b) {
    return a.min(b);
}

inline Color max(const Color& a, const Color& b) {
    return a.max(b);
}

inline Color clamp(const Color& value, const Color& min, const Color& max) {
    return value.clamp(min, max);
}

inline Color clamp(const Color& value, float32 min, float32 max) {
    return value.clamp(min, max);
}

inline Color lerp(const Color& a, const Color& b, float32 t) {
    return a.lerp(b, t);
}

inline Color slerp(const Color& a, const Color& b, float32 t) {
    return a.slerp(b, t);
}

inline Color nlerp(const Color& a, const Color& b, float32 t) {
    return a.nlerp(b, t);
}

// Color conversion utilities
namespace ColorUtils {
    // Color space conversions
    void rgbToHsv(float32 r, float32 g, float32 b, float32& h, float32& s, float32& v);
    void hsvToRgb(float32 h, float32 s, float32 v, float32& r, float32& g, float32& b);
    void rgbToHsl(float32 r, float32 g, float32 b, float32& h, float32& s, float32& l);
    void hslToRgb(float32 h, float32 s, float32 l, float32& r, float32& g, float32& b);
    void rgbToCmyk(float32 r, float32 g, float32 b, float32& c, float32& m, float32& y, float32& k);
    void cmykToRgb(float32 c, float32 m, float32 y, float32 k, float32& r, float32& g, float32& b);
    void rgbToXyz(float32 r, float32 g, float32 b, float32& x, float32& y, float32& z);
    void xyzToRgb(float32 x, float32 y, float32 z, float32& r, float32& g, float32& b);
    void xyzToLab(float32 x, float32 y, float32 z, float32& l, float32& a, float32& b);
    void labToXyz(float32 l, float32 a, float32 b, float32& x, float32& y, float32& z);
    void labToLch(float32 l, float32 a, float32 b, float32& lch_l, float32& c, float32& h);
    void lchToLab(float32 lch_l, float32 c, float32 h, float32& l, float32& a, float32& b);
    void rgbToYuv(float32 r, float32 g, float32 b, float32& y, float32& u, float32& v);
    void yuvToRgb(float32 y, float32 u, float32 v, float32& r, float32& g, float32& b);
    void rgbToYCbCr(float32 r, float32 g, float32 b, float32& y, float32& cb, float32& cr);
    void yCbCrToRgb(float32 y, float32 cb, float32 cr, float32& r, float32& g, float32& b);
    
    // Temperature conversions
    float32 kelvinToRgb(float32 kelvin, float32& r, float32& g, float32& b);
    float32 rgbToKelvin(float32 r, float32 g, float32 b);
    
    // Format conversions
    void rgbaToRgb565(float32 r, float32 g, float32 b, float32 a, uint16_t& rgb565);
    void rgb565ToRgba(uint16_t rgb565, float32& r, float32& g, float32& b, float32& a);
    void rgbaToRgba5551(float32 r, float32 g, float32 b, float32 a, uint16_t& rgba5551);
    void rgba5551ToRgba(uint16_t rgba5551, float32& r, float32& g, float32& b, float32& a);
    void rgbaToRgba4444(float32 r, float32 g, float32 b, float32 a, uint16_t& rgba4444);
    void rgba4444ToRgba(uint16_t rgba4444, float32& r, float32& g, float32& b, float32& a);
    
    // String conversions
    std::string colorToHex(const Color& color);
    Color hexToColor(const std::string& hex);
    std::string colorToString(const Color& color);
    Color stringToColor(const std::string& str);
    
    // Color analysis
    float32 getLuminance(const Color& color);
    float32 getBrightness(const Color& color);
    float32 getSaturation(const Color& color);
    float32 getHue(const Color& color);
    float32 getLightness(const Color& color);
    float32 getChroma(const Color& color);
    
    // Color harmony
    Color getComplementary(const Color& color);
    std::vector<Color> getTriadic(const Color& color);
    std::vector<Color> getTetradic(const Color& color);
    std::vector<Color> getAnalogous(const Color& color);
    std::vector<Color> getSplitComplementary(const Color& color);
    
    // Color palettes
    std::vector<Color> generateMonochromaticPalette(const Color& base, size_t count);
    std::vector<Color> generateAnalogousPalette(const Color& base, size_t count);
    std::vector<Color> generateTriadicPalette(const Color& base, size_t count);
    std::vector<Color> generateTetradicPalette(const Color& base, size_t count);
    std::vector<Color> generateComplementaryPalette(const Color& base, size_t count);
    std::vector<Color> generateWarmPalette(size_t count);
    std::vector<Color> generateCoolPalette(size_t count);
    std::vector<Color> generatePastelPalette(size_t count);
    std::vector<Color> generateNeonPalette(size_t count);
    std::vector<Color> generateGrayscalePalette(size_t count);
    
    // Color correction
    Color adjustBrightness(const Color& color, float32 factor);
    Color adjustContrast(const Color& color, float32 factor);
    Color adjustSaturation(const Color& color, float32 factor);
    Color adjustHue(const Color& color, float32 factor);
    Color adjustGamma(const Color& color, float32 gamma);
    Color adjustExposure(const Color& color, float32 exposure);
    Color adjustColorBalance(const Color& color, float32 r, float32 g, float32 b);
    
    // Color filtering
    Color applySepia(const Color& color);
    Color applyGrayscale(const Color& color);
    Color applyInvert(const Color& color);
    Color applyThreshold(const Color& color, float32 threshold);
    Color applyPosterize(const Color& color, uint32_t levels);
    Color applySolarize(const Color& color, float32 threshold);
    Color applyVignette(const Color& color, float32 strength, float32 x, float32 y, float32 radius);
    
    // Color blending modes
    Color blendNormal(const Color& base, const Color& overlay);
    Color blendMultiply(const Color& base, const Color& overlay);
    Color blendScreen(const Color& base, const Color& overlay);
    Color blendOverlay(const Color& base, const Color& overlay);
    Color blendSoftLight(const Color& base, const Color& overlay);
    Color blendHardLight(const Color& base, const Color& overlay);
    Color blendColorDodge(const Color& base, const Color& overlay);
    Color blendColorBurn(const Color& base, const Color& overlay);
    Color blendDarken(const Color& base, const Color& overlay);
    Color blendLighten(const Color& base, const Color& overlay);
    Color blendDifference(const Color& base, const Color& overlay);
    Color blendExclusion(const Color& base, const Color& overlay);
    
    // Color validation
    bool isValidColor(const Color& color);
    bool isValidRGB(float32 r, float32 g, float32 b);
    bool isValidRGBA(float32 r, float32 g, float32 b, float32 a);
    bool isValidHex(const std::string& hex);
    
    // Color distance
    float32 colorDistance(const Color& color1, const Color& color2);
    float32 colorDistanceRGB(const Color& color1, const Color& color2);
    float32 colorDistanceHSV(const Color& color1, const Color& color2);
    float32 colorDistanceLAB(const Color& color1, const Color& color2);
    
    // Color interpolation
    Color interpolateLinear(const Color& color1, const Color& color2, float32 t);
    Color interpolateCubic(const Color& color1, const Color& color2, const Color& color3, const Color& color4, float32 t);
    Color interpolateBezier(const Color& color1, const Color& color2, const Color& color3, const Color& color4, float32 t);
}

// Hash function for Color
struct ColorHash {
    size_t operator()(const Color& color) const noexcept {
        size_t h1 = std::hash<float32>{}(color.r);
        size_t h2 = std::hash<float32>{}(color.g);
        size_t h3 = std::hash<float32>{}(color.b);
        size_t h4 = std::hash<float32>{}(color.a);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
};

} // namespace RFUtils