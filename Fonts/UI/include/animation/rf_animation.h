#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <map>

namespace RFAnimation {

// Forward declarations
class AnimationController;
class AnimationState;
class Keyframe;
class Sequence;
class Timeline;
class Easing;
class Interpolator;
class Tween;

// Animation types
enum class AnimationType {
    POSITION,
    ROTATION,
    SCALE,
    COLOR,
    OPACITY,
    CUSTOM
};

enum class PlaybackState {
    STOPPED,
    PLAYING,
    PAUSED,
    REVERSE
};

enum class WrapMode {
    ONCE,
    LOOP,
    PING_PONG,
    REVERSE
};

// Animation value structure
template<typename T>
struct AnimationValue {
    T value;
    T previousValue;
    T startValue;
    T endValue;
    float currentTime;
    float duration;
    bool isAnimating;
    
    AnimationValue() : value(T()), previousValue(T()), startValue(T()), 
                   endValue(T()), currentTime(0.0f), duration(0.0f), isAnimating(false) {}
    AnimationValue(const T& val) : value(val), previousValue(val), startValue(val), 
                       endValue(val), currentTime(0.0f), duration(0.0f), isAnimating(false) {}
};

// Animation event structure
struct AnimationEvent {
    enum class Type {
        STARTED,
        STOPPED,
        PAUSED,
        RESUMED,
        COMPLETED,
        KEYFRAME_REACHED,
        LOOPED
    };
    
    Type type;
    float time;
    std::string animationName;
    std::map<std::string, std::any> userData;
    
    AnimationEvent(Type t, float tm, const std::string& name = "") 
        : type(t), time(tm), animationName(name) {}
};

// Animation callback type
using AnimationCallback = std::function<void(const AnimationEvent&)>;

// Main animation class
class Animation {
public:
    Animation();
    virtual ~Animation() = default;
    
    // Core animation methods
    virtual void update(float deltaTime) = 0;
    virtual void render() const = 0;
    virtual void reset() = 0;
    
    // Value manipulation
    template<typename T>
    void setValue(const std::string& key, const T& value);
    
    template<typename T>
    T getValue(const std::string& key) const;
    
    template<typename T>
    void animateValue(const std::string& key, const T& fromValue, const T& toValue, 
                   float duration, Easing* easing = nullptr);
    
    template<typename T>
    void animateValueTo(const std::string& key, const T& toValue, 
                   float duration, Easing* easing = nullptr);
    
    // Animation control
    void play();
    void pause();
    void stop();
    void resume();
    void reverse();
    
    // State queries
    bool isPlaying() const;
    bool isPaused() const;
    bool isStopped() const;
    bool isAnimating(const std::string& key) const;
    float getCurrentTime() const;
    float getDuration() const;
    PlaybackState getPlaybackState() const;
    
    // Timing and wrapping
    void setDuration(float duration);
    void setWrapMode(WrapMode mode);
    void setPlaybackSpeed(float speed);
    void setStartTime(float time);
    void setEndTime(float time);
    
    // Event handling
    void addEventListener(const std::string& eventType, AnimationCallback callback);
    void removeEventListener(const std::string& eventType, AnimationCallback callback);
    void clearEventListeners();
    
    // Keyframe support
    template<typename T>
    void addKeyframe(const std::string& key, float time, const T& value);
    
    template<typename T>
    void removeKeyframe(const std::string& key, float time);
    
    template<typename T>
    std::vector<std::pair<float, T>> getKeyframes(const std::string& key) const;
    
    // Sequence support
    void addSequence(const std::string& sequenceName, std::shared_ptr<Sequence> sequence);
    void removeSequence(const std::string& sequenceName);
    std::shared_ptr<Sequence> getSequence(const std::string& sequenceName) const;
    
    // Utility methods
    void cloneFrom(const Animation& other);
    virtual std::unique_ptr<Animation> clone() const = 0;
    
    // Data access
    const std::map<std::string, std::unique_ptr<AnimationValue>>& getValues() const;
    const std::map<std::string, std::vector<std::pair<float, std::any>>>& getKeyframes() const;
    
protected:
    // Protected members for derived classes
    std::map<std::string, std::unique_ptr<AnimationValue>> values_;
    std::map<std::string, std::vector<std::pair<float, std::any>>> keyframes_;
    std::map<std::string, std::shared_ptr<Sequence>> sequences_;
    std::map<std::string, std::vector<AnimationCallback>> eventListeners_;
    
    float currentTime_;
    float duration_;
    float playbackSpeed_;
    float startTime_;
    float endTime_;
    PlaybackState playbackState_;
    WrapMode wrapMode_;
    
    // Protected helper methods
    virtual void updateValue(float deltaTime, const std::string& key);
    virtual void processKeyframes();
    virtual void triggerEvent(const AnimationEvent& event);
    virtual void interpolateBetweenKeyframes(const std::string& key);
    
    template<typename T>
    AnimationValue<T>* getValueInternal(const std::string& key) const;
    
    template<typename T>
    void setValueInternal(const std::string& key, const T& value);
};

// Specialized animation classes
class PositionAnimation : public Animation {
public:
    PositionAnimation();
    void animatePosition(const std::string& key, const std::vector<std::pair<float, std::pair<float, float>>>& path, 
                      float duration, Easing* easing = nullptr);
};

class RotationAnimation : public Animation {
public:
    RotationAnimation();
    void animateRotation(const std::string& key, float fromAngle, float toAngle, 
                      float duration, Easing* easing = nullptr);
};

class ScaleAnimation : public Animation {
public:
    ScaleAnimation();
    void animateScale(const std::string& key, const std::pair<float, float>& fromScale, 
                   const std::pair<float, float>& toScale, float duration, Easing* easing = nullptr);
};

class ColorAnimation : public Animation {
public:
    ColorAnimation();
    void animateColor(const std::string& key, const std::array<float, 4>& fromColor, 
                   const std::array<float, 4>& toColor, float duration, Easing* easing = nullptr);
};

class OpacityAnimation : public Animation {
public:
    OpacityAnimation();
    void animateOpacity(const std::string& key, float fromOpacity, float toOpacity, 
                     float duration, Easing* easing = nullptr);
};

} // namespace RFAnimation