#include "rf_keyframe.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Animation {

Keyframe::Keyframe()
    : time(0.0f)
    , value(0.0f)
    , inTangent(0.0f, 0.0f)
    , outTangent(0.0f, 0.0f)
    , interpolationType(InterpolationType::Linear)
    , easingType(EasingType::Linear)
{
    RF_LOG_INFO("Keyframe created");
}

Keyframe::Keyframe(float time_, float value_)
    : time(time_)
    , value(value_)
    , inTangent(0.0f, 0.0f)
    , outTangent(0.0f, 0.0f)
    , interpolationType(InterpolationType::Linear)
    , easingType(EasingType::Linear)
{
    RF_LOG_INFO("Keyframe created with time=%.2f, value=%.2f", time_, value_);
}

Keyframe::Keyframe(float time_, float value_, const Vec2& inTangent_, const Vec2& outTangent_)
    : time(time_)
    , value(value_)
    , inTangent(inTangent_)
    , outTangent(outTangent_)
    , interpolationType(InterpolationType::Linear)
    , easingType(EasingType::Linear)
{
    RF_LOG_INFO("Keyframe created with time=%.2f, value=%.2f", time_, value_);
}

Keyframe::Keyframe(float time_, float value_, InterpolationType interpType, EasingType easeType)
    : time(time_)
    , value(value_)
    , inTangent(0.0f, 0.0f)
    , outTangent(0.0f, 0.0f)
    , interpolationType(interpType)
    , easingType(easeType)
{
    RF_LOG_INFO("Keyframe created with time=%.2f, value=%.2f", time_, value_);
}

Keyframe::Keyframe(float time_, float value_, const Vec2& inTangent_, const Vec2& outTangent_, 
                 InterpolationType interpType, EasingType easeType)
    : time(time_)
    , value(value_)
    , inTangent(inTangent_)
    , outTangent(outTangent_)
    , interpolationType(interpType)
    , easingType(easeType)
{
    RF_LOG_INFO("Keyframe created with time=%.2f, value=%.2f", time_, value_);
}

Keyframe::Keyframe(float time_, const Vec2& value_)
    : time(time_)
    , value2D(value_)
    , inTangent(0.0f, 0.0f)
    , outTangent(0.0f, 0.0f)
    , interpolationType(InterpolationType::Linear)
    , easingType(EasingType::Linear)
    , is2D(true)
{
    RF_LOG_INFO("2D Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Vec2& value_, const Vec2& inTangent_, const Vec2& outTangent_)
    : time(time_)
    , value2D(value_)
    , inTangent(inTangent_)
    , outTangent(outTangent_)
    , interpolationType(InterpolationType::Linear)
    , easingType(EasingType::Linear)
    , is2D(true)
{
    RF_LOG_INFO("2D Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Vec2& value_, InterpolationType interpType, EasingType easeType)
    : time(time_)
    , value2D(value_)
    , inTangent(0.0f, 0.0f)
    , outTangent(0.0f, 0.0f)
    , interpolationType(interpType)
    , easingType(easeType)
    , is2D(true)
{
    RF_LOG_INFO("2D Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Vec2& value_, const Vec2& inTangent_, const Vec2& outTangent_, 
                 InterpolationType interpType, EasingType easeType)
    : time(time_)
    , value2D(value_)
    , inTangent(inTangent_)
    , outTangent(outTangent_)
    , interpolationType(interpType)
    , easingType(easeType)
    , is2D(true)
{
    RF_LOG_INFO("2D Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Vec3& value_)
    : time(time_)
    , value3D(value_)
    , inTangent(0.0f, 0.0f, 0.0f)
    , outTangent(0.0f, 0.0f, 0.0f)
    , interpolationType(InterpolationType::Linear)
    , easingType(EasingType::Linear)
    , is3D(true)
{
    RF_LOG_INFO("3D Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Vec3& value_, const Vec3& inTangent_, const Vec3& outTangent_)
    : time(time_)
    , value3D(value_)
    , inTangent(inTangent_)
    , outTangent(outTangent_)
    , interpolationType(InterpolationType::Linear)
    , easingType(EasingType::Linear)
    , is3D(true)
{
    RF_LOG_INFO("3D Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Vec3& value_, InterpolationType interpType, EasingType easeType)
    : time(time_)
    , value3D(value_)
    , inTangent(0.0f, 0.0f, 0.0f)
    , outTangent(0.0f, 0.0f, 0.0f)
    , interpolationType(interpType)
    , easingType(easeType)
    , is3D(true)
{
    RF_LOG_INFO("3D Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Vec3& value_, const Vec3& inTangent_, const Vec3& outTangent_, 
                 InterpolationType interpType, EasingType easeType)
    : time(time_)
    , value3D(value_)
    , inTangent(inTangent_)
    , outTangent(outTangent_)
    , interpolationType(interpType)
    , easingType(easeType)
    , is3D(true)
{
    RF_LOG_INFO("3D Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Vec4& value_)
    : time(time_)
    , value4D(value_)
    , inTangent(0.0f, 0.0f, 0.0f, 0.0f)
    , outTangent(0.0f, 0.0f, 0.0f, 0.0f)
    , interpolationType(InterpolationType::Linear)
    , easingType(EasingType::Linear)
    , is4D(true)
{
    RF_LOG_INFO("4D Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Vec4& value_, const Vec4& inTangent_, const Vec4& outTangent_)
    : time(time_)
    , value4D(value_)
    , inTangent(inTangent_)
    , outTangent(outTangent_)
    , interpolationType(InterpolationType::Linear)
    , easingType(EasingType::Linear)
    , is4D(true)
{
    RF_LOG_INFO("4D Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Vec4& value_, InterpolationType interpType, EasingType easeType)
    : time(time_)
    , value4D(value_)
    , inTangent(0.0f, 0.0f, 0.0f, 0.0f)
    , outTangent(0.0f, 0.0f, 0.0f, 0.0f)
    , interpolationType(interpType)
    , easingType(easeType)
    , is4D(true)
{
    RF_LOG_INFO("4D Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Vec4& value_, const Vec4& inTangent_, const Vec4& outTangent_, 
                 InterpolationType interpType, EasingType easeType)
    : time(time_)
    , value4D(value_)
    , inTangent(inTangent_)
    , outTangent(outTangent_)
    , interpolationType(interpType)
    , easingType(easeType)
    , is4D(true)
{
    RF_LOG_INFO("4D Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Color& value_)
    : time(time_)
    , valueColor(value_)
    , inTangent(0.0f, 0.0f)
    , outTangent(0.0f, 0.0f)
    , interpolationType(InterpolationType::Linear)
    , easingType(EasingType::Linear)
    , isColor(true)
{
    RF_LOG_INFO("Color Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Color& value_, const Vec2& inTangent_, const Vec2& outTangent_)
    : time(time_)
    , valueColor(value_)
    , inTangent(inTangent_)
    , outTangent(outTangent_)
    , interpolationType(InterpolationType::Linear)
    , easingType(EasingType::Linear)
    , isColor(true)
{
    RF_LOG_INFO("Color Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Color& value_, InterpolationType interpType, EasingType easeType)
    : time(time_)
    , valueColor(value_)
    , inTangent(0.0f, 0.0f)
    , outTangent(0.0f, 0.0f)
    , interpolationType(interpType)
    , easingType(easeType)
    , isColor(true)
{
    RF_LOG_INFO("Color Keyframe created with time=%.2f", time_);
}

Keyframe::Keyframe(float time_, const Color& value_, const Vec2& inTangent_, const Vec2& outTangent_, 
                 InterpolationType interpType, EasingType easeType)
    : time(time_)
    , valueColor(value_)
    , inTangent(inTangent_)
    , outTangent(outTangent_)
    , interpolationType(interpType)
    , easingType(easeType)
    , isColor(true)
{
    RF_LOG_INFO("Color Keyframe created with time=%.2f", time_);
}

Keyframe::~Keyframe() {
    RF_LOG_INFO("Keyframe destroyed");
}

void Keyframe::setTime(float time_) {
    time = time_;
}

void Keyframe::setValue(float value_) {
    value = value_;
    is2D = false;
    is3D = false;
    is4D = false;
    isColor = false;
}

void Keyframe::setValue(const Vec2& value_) {
    value2D = value_;
    is2D = true;
    is3D = false;
    is4D = false;
    isColor = false;
}

void Keyframe::setValue(const Vec3& value_) {
    value3D = value_;
    is2D = false;
    is3D = true;
    is4D = false;
    isColor = false;
}

void Keyframe::setValue(const Vec4& value_) {
    value4D = value_;
    is2D = false;
    is3D = false;
    is4D = true;
    isColor = false;
}

void Keyframe::setValue(const Color& value_) {
    valueColor = value_;
    is2D = false;
    is3D = false;
    is4D = false;
    isColor = true;
}

void Keyframe::setInTangent(const Vec2& tangent) {
    inTangent = tangent;
}

void Keyframe::setInTangent(const Vec3& tangent) {
    inTangent3D = tangent;
}

void Keyframe::setInTangent(const Vec4& tangent) {
    inTangent4D = tangent;
}

void Keyframe::setOutTangent(const Vec2& tangent) {
    outTangent = tangent;
}

void Keyframe::setOutTangent(const Vec3& tangent) {
    outTangent3D = tangent;
}

void Keyframe::setOutTangent(const Vec4& tangent) {
    outTangent4D = tangent;
}

void Keyframe::setInterpolationType(InterpolationType type) {
    interpolationType = type;
}

void Keyframe::setEasingType(EasingType type) {
    easingType = type;
}

float Keyframe::getTime() const {
    return time;
}

float Keyframe::getFloatValue() const {
    return is2D ? value2D.x : (is3D ? value3D.x : (is4D ? value4D.x : value));
}

Vec2 Keyframe::getVec2Value() const {
    return is2D ? value2D : (is3D ? Vec2(value3D.x, value3D.y) : (is4D ? Vec2(value4D.x, value4D.y) : Vec2(value, value)));
}

Vec3 Keyframe::getVec3Value() const {
    return is3D ? value3D : (is4D ? Vec3(value4D.x, value4D.y, value4D.z) : (is2D ? Vec3(value2D.x, value2D.y, 0.0f) : Vec3(value, value, value)));
}

Vec4 Keyframe::getVec4Value() const {
    return is4D ? value4D : (is3D ? Vec4(value3D.x, value3D.y, value3D.z, 1.0f) : (is2D ? Vec4(value2D.x, value2D.y, 0.0f, 1.0f) : Vec4(value, value, value, 1.0f)));
}

Color Keyframe::getColorValue() const {
    return isColor ? valueColor : (is4D ? Color(value4D.x, value4D.y, value4D.z, value4D.w) : (is3D ? Color(value3D.x, value3D.y, value3D.z, 1.0f) : (is2D ? Color(value2D.x, value2D.y, 0.0f, 1.0f) : Color(value, value, value, 1.0f))));
}

Vec2 Keyframe::getInTangent2D() const {
    return inTangent;
}

Vec3 Keyframe::getInTangent3D() const {
    return inTangent3D;
}

Vec4 Keyframe::getInTangent4D() const {
    return inTangent4D;
}

Vec2 Keyframe::getOutTangent2D() const {
    return outTangent;
}

Vec3 Keyframe::getOutTangent3D() const {
    return outTangent3D;
}

Vec4 Keyframe::getOutTangent4D() const {
    return outTangent4D;
}

InterpolationType Keyframe::getInterpolationType() const {
    return interpolationType;
}

EasingType Keyframe::getEasingType() const {
    return easingType;
}

bool Keyframe::is2DKeyframe() const {
    return is2D;
}

bool Keyframe::is3DKeyframe() const {
    return is3D;
}

bool Keyframe::is4DKeyframe() const {
    return is4D;
}

bool Keyframe::isColorKeyframe() const {
    return isColor;
}

bool Keyframe::operator<(const Keyframe& other) const {
    return time < other.time;
}

bool Keyframe::operator==(const Keyframe& other) const {
    return Math::abs(time - other.time) < 0.001f;
}

bool Keyframe::operator!=(const Keyframe& other) const {
    return !(*this == other);
}

// KeyframeTrack implementation
KeyframeTrack::KeyframeTrack()
    : name("")
    , trackType(TrackType::Float)
    , isLooping(false)
    , isEnabled(true)
{
    RF_LOG_INFO("KeyframeTrack created");
}

KeyframeTrack::KeyframeTrack(const std::string& name_, TrackType type)
    : name(name_)
    , trackType(type)
    , isLooping(false)
    , isEnabled(true)
{
    RF_LOG_INFO("KeyframeTrack created: %s", name_.c_str());
}

KeyframeTrack::~KeyframeTrack() {
    clear();
    RF_LOG_INFO("KeyframeTrack destroyed");
}

void KeyframeTrack::addKeyframe(const Keyframe& keyframe) {
    keyframes.push_back(keyframe);
    
    // Sort keyframes by time
    std::sort(keyframes.begin(), keyframes.end());
    
    RF_LOG_INFO("Added keyframe to track %s at time %.2f", name.c_str(), keyframe.getTime());
}

void KeyframeTrack::removeKeyframe(int index) {
    if (index >= 0 && index < static_cast<int>(keyframes.size())) {
        keyframes.erase(keyframes.begin() + index);
        RF_LOG_INFO("Removed keyframe from track %s at index %d", name.c_str(), index);
    }
}

void KeyframeTrack::removeKeyframe(float time) {
    for (auto it = keyframes.begin(); it != keyframes.end(); ++it) {
        if (Math::abs(it->getTime() - time) < 0.001f) {
            keyframes.erase(it);
            RF_LOG_INFO("Removed keyframe from track %s at time %.2f", name.c_str(), time);
            break;
        }
    }
}

void KeyframeTrack::clear() {
    keyframes.clear();
    RF_LOG_INFO("Cleared all keyframes from track %s", name.c_str());
}

const Keyframe* KeyframeTrack::getKeyframeAtTime(float time) const {
    if (keyframes.empty()) {
        return nullptr;
    }
    
    // Find the keyframe at or before the specified time
    const Keyframe* result = nullptr;
    for (const auto& keyframe : keyframes) {
        if (keyframe.getTime() <= time) {
            result = &keyframe;
        } else {
            break;
        }
    }
    
    return result;
}

const Keyframe* KeyframeTrack::getNextKeyframe(float time) const {
    if (keyframes.empty()) {
        return nullptr;
    }
    
    // Find the first keyframe after the specified time
    for (const auto& keyframe : keyframes) {
        if (keyframe.getTime() > time) {
            return &keyframe;
        }
    }
    
    return nullptr;
}

std::pair<const Keyframe*, const Keyframe*> KeyframeTrack::getSurroundingKeyframes(float time) const {
    const Keyframe* prev = nullptr;
    const Keyframe* next = nullptr;
    
    for (const auto& keyframe : keyframes) {
        if (keyframe.getTime() <= time) {
            prev = &keyframe;
        } else {
            next = &keyframe;
            break;
        }
    }
    
    return std::make_pair(prev, next);
}

float KeyframeTrack::evaluate(float time) const {
    if (keyframes.empty()) {
        return 0.0f;
    }
    
    if (keyframes.size() == 1) {
        return keyframes[0].getFloatValue();
    }
    
    auto surrounding = getSurroundingKeyframes(time);
    const Keyframe* prev = surrounding.first;
    const Keyframe* next = surrounding.second;
    
    if (!prev && !next) {
        return 0.0f;
    }
    
    if (!prev) {
        return next->getFloatValue();
    }
    
    if (!next) {
        if (isLooping) {
            return evaluateBetween(keyframes.back(), keyframes.front(), time);
        }
        return prev->getFloatValue();
    }
    
    return evaluateBetween(*prev, *next, time);
}

float KeyframeTrack::evaluateBetween(const Keyframe& prev, const Keyframe& next, float time) const {
    float prevTime = prev.getTime();
    float nextTime = next.getTime();
    
    if (Math::abs(nextTime - prevTime) < 0.001f) {
        return prev.getFloatValue();
    }
    
    float t = (time - prevTime) / (nextTime - prevTime);
    t = Math::clamp(t, 0.0f, 1.0f);
    
    // Apply easing
    float easedT = applyEasing(prev.getEasingType(), t);
    
    // Interpolate between values
    return Math::lerp(prev.getFloatValue(), next.getFloatValue(), easedT);
}

void KeyframeTrack::setName(const std::string& name_) {
    name = name_;
}

void KeyframeTrack::setTrackType(TrackType type) {
    trackType = type;
}

void KeyframeTrack::setLooping(bool loop) {
    isLooping = loop;
}

void KeyframeTrack::setEnabled(bool enabled) {
    isEnabled = enabled;
}

const std::string& KeyframeTrack::getName() const {
    return name;
}

TrackType KeyframeTrack::getTrackType() const {
    return trackType;
}

bool KeyframeTrack::isLoopingTrack() const {
    return isLooping;
}

bool KeyframeTrack::isEnabledTrack() const {
    return isEnabled;
}

size_t KeyframeTrack::getKeyframeCount() const {
    return keyframes.size();
}

const std::vector<Keyframe>& KeyframeTrack::getKeyframes() const {
    return keyframes;
}

} // namespace Animation
} // namespace RedFiles