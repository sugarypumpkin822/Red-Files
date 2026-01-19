#include "rf_interpolator.h"
#include "rf_easing.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Animation {

Interpolator::Interpolator()
    : m_easingType(EasingType::Linear)
    , m_interpolationType(InterpolationType::Linear)
{
    RF_LOG_INFO("Interpolator created");
}

Interpolator::~Interpolator() {
    RF_LOG_INFO("Interpolator destroyed");
}

void Interpolator::setEasingType(EasingType type) {
    m_easingType = type;
    RF_LOG_INFO("Easing type set to %d", static_cast<int>(type));
}

void Interpolator::setInterpolationType(InterpolationType type) {
    m_interpolationType = type;
    RF_LOG_INFO("Interpolation type set to %d", static_cast<int>(type));
}

float Interpolator::interpolate(float t, float start, float end) const {
    // Apply easing first
    float easedT = applyEasing(m_easingType, Math::clamp(t, 0.0f, 1.0f));
    
    // Then apply interpolation
    return interpolateValue(easedT, start, end);
}

Vec2 Interpolator::interpolate(float t, const Vec2& start, const Vec2& end) const {
    float easedT = applyEasing(m_easingType, Math::clamp(t, 0.0f, 1.0f));
    return interpolateVector(easedT, start, end);
}

Vec3 Interpolator::interpolate(float t, const Vec3& start, const Vec3& end) const {
    float easedT = applyEasing(m_easingType, Math::clamp(t, 0.0f, 1.0f));
    return interpolateVector(easedT, start, end);
}

Vec4 Interpolator::interpolate(float t, const Vec4& start, const Vec4& end) const {
    float easedT = applyEasing(m_easingType, Math::clamp(t, 0.0f, 1.0f));
    return interpolateVector(easedT, start, end);
}

Color Interpolator::interpolate(float t, const Color& start, const Color& end) const {
    float easedT = applyEasing(m_easingType, Math::clamp(t, 0.0f, 1.0f));
    return interpolateColor(easedT, start, end);
}

float Interpolator::interpolateValue(float t, float start, float end) const {
    switch (m_interpolationType) {
        case InterpolationType::Linear:
            return Math::lerp(start, end, t);
            
        case InterpolationType::Step:
            return (t < 0.5f) ? start : end;
            
        case InterpolationType::CatmullRom:
            return interpolateCatmullRom(t, start, end);
            
        case InterpolationType::Bezier:
            return interpolateBezier(t, start, end);
            
        default:
            return Math::lerp(start, end, t);
    }
}

Vec2 Interpolator::interpolateVector(float t, const Vec2& start, const Vec2& end) const {
    switch (m_interpolationType) {
        case InterpolationType::Linear:
            return Math::lerp(start, end, t);
            
        case InterpolationType::Step:
            return (t < 0.5f) ? start : end;
            
        case InterpolationType::CatmullRom:
            return interpolateCatmullRom(t, start, end);
            
        case InterpolationType::Bezier:
            return interpolateBezier(t, start, end);
            
        default:
            return Math::lerp(start, end, t);
    }
}

Vec3 Interpolator::interpolateVector(float t, const Vec3& start, const Vec3& end) const {
    switch (m_interpolationType) {
        case InterpolationType::Linear:
            return Math::lerp(start, end, t);
            
        case InterpolationType::Step:
            return (t < 0.5f) ? start : end;
            
        case InterpolationType::CatmullRom:
            return interpolateCatmullRom(t, start, end);
            
        case InterpolationType::Bezier:
            return interpolateBezier(t, start, end);
            
        default:
            return Math::lerp(start, end, t);
    }
}

Vec4 Interpolator::interpolateVector(float t, const Vec4& start, const Vec4& end) const {
    switch (m_interpolationType) {
        case InterpolationType::Linear:
            return Math::lerp(start, end, t);
            
        case InterpolationType::Step:
            return (t < 0.5f) ? start : end;
            
        case InterpolationType::CatmullRom:
            return interpolateCatmullRom(t, start, end);
            
        case InterpolationType::Bezier:
            return interpolateBezier(t, start, end);
            
        default:
            return Math::lerp(start, end, t);
    }
}

Color Interpolator::interpolateColor(float t, const Color& start, const Color& end) const {
    switch (m_interpolationType) {
        case InterpolationType::Linear:
            return Color::lerp(start, end, t);
            
        case InterpolationType::Step:
            return (t < 0.5f) ? start : end;
            
        case InterpolationType::CatmullRom:
            return interpolateCatmullRom(t, start, end);
            
        case InterpolationType::Bezier:
            return interpolateBezier(t, start, end);
            
        default:
            return Color::lerp(start, end, t);
    }
}

float Interpolator::interpolateCatmullRom(float t, float start, float end) const {
    // For single values, Catmull-Rom reduces to linear interpolation
    // unless we have more control points
    return Math::lerp(start, end, t);
}

Vec2 Interpolator::interpolateCatmullRom(float t, const Vec2& start, const Vec2& end) const {
    // For 2D vectors, Catmull-Rom reduces to linear interpolation
    // unless we have more control points
    return Math::lerp(start, end, t);
}

Vec3 Interpolator::interpolateCatmullRom(float t, const Vec3& start, const Vec3& end) const {
    // For 3D vectors, Catmull-Rom reduces to linear interpolation
    // unless we have more control points
    return Math::lerp(start, end, t);
}

Vec4 Interpolator::interpolateCatmullRom(float t, const Vec4& start, const Vec4& end) const {
    // For 4D vectors, Catmull-Rom reduces to linear interpolation
    // unless we have more control points
    return Math::lerp(start, end, t);
}

Color Interpolator::interpolateCatmullRom(float t, const Color& start, const Color& end) const {
    // For colors, Catmull-Rom reduces to linear interpolation
    // unless we have more control points
    return Color::lerp(start, end, t);
}

float Interpolator::interpolateBezier(float t, float start, float end) const {
    // Simple quadratic Bezier with control point at midpoint
    float controlPoint = (start + end) * 0.5f;
    
    float oneMinusT = 1.0f - t;
    return oneMinusT * oneMinusT * start + 
           2.0f * oneMinusT * t * controlPoint + 
           t * t * end;
}

Vec2 Interpolator::interpolateBezier(float t, const Vec2& start, const Vec2& end) const {
    // Simple quadratic Bezier with control point at midpoint
    Vec2 controlPoint = (start + end) * 0.5f;
    
    float oneMinusT = 1.0f - t;
    return start * (oneMinusT * oneMinusT) + 
           controlPoint * (2.0f * oneMinusT * t) + 
           end * (t * t);
}

Vec3 Interpolator::interpolateBezier(float t, const Vec3& start, const Vec3& end) const {
    // Simple quadratic Bezier with control point at midpoint
    Vec3 controlPoint = (start + end) * 0.5f;
    
    float oneMinusT = 1.0f - t;
    return start * (oneMinusT * oneMinusT) + 
           controlPoint * (2.0f * oneMinusT * t) + 
           end * (t * t);
}

Vec4 Interpolator::interpolateBezier(float t, const Vec4& start, const Vec4& end) const {
    // Simple quadratic Bezier with control point at midpoint
    Vec4 controlPoint = (start + end) * 0.5f;
    
    float oneMinusT = 1.0f - t;
    return start * (oneMinusT * oneMinusT) + 
           controlPoint * (2.0f * oneMinusT * t) + 
           end * (t * t);
}

Color Interpolator::interpolateBezier(float t, const Color& start, const Color& end) const {
    // Simple quadratic Bezier with control point at midpoint
    Color controlPoint = Color::lerp(start, end, 0.5f);
    
    float oneMinusT = 1.0f - t;
    return start * (oneMinusT * oneMinusT) + 
           controlPoint * (2.0f * oneMinusT * t) + 
           end * (t * t);
}

// Advanced interpolation with multiple control points
float Interpolator::interpolateAdvanced(float t, const std::vector<float>& points) const {
    if (points.size() < 2) {
        return 0.0f;
    }
    
    if (points.size() == 2) {
        return interpolateValue(t, points[0], points[1]);
    }
    
    // Find the segment containing t
    float segmentCount = static_cast<float>(points.size() - 1);
    float segmentT = t * segmentCount;
    int segmentIndex = static_cast<int>(segmentT);
    float localT = segmentT - segmentIndex;
    
    // Clamp to valid range
    segmentIndex = Math::clamp(segmentIndex, 0, static_cast<int>(points.size() - 2));
    localT = Math::clamp(localT, 0.0f, 1.0f);
    
    return interpolateValue(localT, points[segmentIndex], points[segmentIndex + 1]);
}

Vec2 Interpolator::interpolateAdvanced(float t, const std::vector<Vec2>& points) const {
    if (points.size() < 2) {
        return Vec2(0.0f, 0.0f);
    }
    
    if (points.size() == 2) {
        return interpolateVector(t, points[0], points[1]);
    }
    
    // Find the segment containing t
    float segmentCount = static_cast<float>(points.size() - 1);
    float segmentT = t * segmentCount;
    int segmentIndex = static_cast<int>(segmentT);
    float localT = segmentT - segmentIndex;
    
    // Clamp to valid range
    segmentIndex = Math::clamp(segmentIndex, 0, static_cast<int>(points.size() - 2));
    localT = Math::clamp(localT, 0.0f, 1.0f);
    
    return interpolateVector(localT, points[segmentIndex], points[segmentIndex + 1]);
}

Vec3 Interpolator::interpolateAdvanced(float t, const std::vector<Vec3>& points) const {
    if (points.size() < 2) {
        return Vec3(0.0f, 0.0f, 0.0f);
    }
    
    if (points.size() == 2) {
        return interpolateVector(t, points[0], points[1]);
    }
    
    // Find the segment containing t
    float segmentCount = static_cast<float>(points.size() - 1);
    float segmentT = t * segmentCount;
    int segmentIndex = static_cast<int>(segmentT);
    float localT = segmentT - segmentIndex;
    
    // Clamp to valid range
    segmentIndex = Math::clamp(segmentIndex, 0, static_cast<int>(points.size() - 2));
    localT = Math::clamp(localT, 0.0f, 1.0f);
    
    return interpolateVector(localT, points[segmentIndex], points[segmentIndex + 1]);
}

Vec4 Interpolator::interpolateAdvanced(float t, const std::vector<Vec4>& points) const {
    if (points.size() < 2) {
        return Vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    
    if (points.size() == 2) {
        return interpolateVector(t, points[0], points[1]);
    }
    
    // Find the segment containing t
    float segmentCount = static_cast<float>(points.size() - 1);
    float segmentT = t * segmentCount;
    int segmentIndex = static_cast<int>(segmentT);
    float localT = segmentT - segmentIndex;
    
    // Clamp to valid range
    segmentIndex = Math::clamp(segmentIndex, 0, static_cast<int>(points.size() - 2));
    localT = Math::clamp(localT, 0.0f, 1.0f);
    
    return interpolateVector(localT, points[segmentIndex], points[segmentIndex + 1]);
}

Color Interpolator::interpolateAdvanced(float t, const std::vector<Color>& points) const {
    if (points.size() < 2) {
        return Color(0.0f, 0.0f, 0.0f, 1.0f);
    }
    
    if (points.size() == 2) {
        return interpolateColor(t, points[0], points[1]);
    }
    
    // Find the segment containing t
    float segmentCount = static_cast<float>(points.size() - 1);
    float segmentT = t * segmentCount;
    int segmentIndex = static_cast<int>(segmentT);
    float localT = segmentT - segmentIndex;
    
    // Clamp to valid range
    segmentIndex = Math::clamp(segmentIndex, 0, static_cast<int>(points.size() - 2));
    localT = Math::clamp(localT, 0.0f, 1.0f);
    
    return interpolateColor(localT, points[segmentIndex], points[segmentIndex + 1]);
}

EasingType Interpolator::getEasingType() const {
    return m_easingType;
}

InterpolationType Interpolator::getInterpolationType() const {
    return m_interpolationType;
}

} // namespace Animation
} // namespace RedFiles