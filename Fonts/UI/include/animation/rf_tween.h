#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <map>

namespace RFAnimation {

// Tween types
enum class TweenType {
    POSITION,
    ROTATION,
    SCALE,
    COLOR,
    OPACITY,
    CUSTOM
};

// Tween states
enum class TweenState {
    IDLE,
    PLAYING,
    PAUSED,
    STOPPED,
    COMPLETED
};

// Tween properties
struct TweenProperties {
    float duration;
    float delay;
    float timeScale;
    bool useTimeScale;
    bool ignoreTimeScale;
    int repeatCount;
    bool yoyo;
    bool autoReverse;
    float startTime;
    float endTime;
    
    TweenProperties() : duration(1.0f), delay(0.0f), timeScale(1.0f), 
                     useTimeScale(true), ignoreTimeScale(false), repeatCount(1), 
                     yoyo(false), autoReverse(false), startTime(0.0f), endTime(0.0f) {}
};

// Tween value structure
template<typename T>
struct TweenValue {
    T fromValue;
    T toValue;
    T currentValue;
    T previousValue;
    T startValue;
    float progress;
    float elapsed;
    bool isAnimating;
    
    TweenValue() : fromValue(T()), toValue(T()), currentValue(T()), previousValue(T()), 
                   startValue(T()), progress(0.0f), elapsed(0.0f), isAnimating(false) {}
    TweenValue(const T& from, const T& to) : fromValue(from), toValue(to), currentValue(from), 
                   previousValue(from), startValue(from), progress(0.0f), elapsed(0.0f), 
                   isAnimating(false) {}
};

// Tween class
class Tween {
public:
    Tween();
    virtual ~Tween() = default;
    
    // Tween management
    template<typename T>
    void addTween(const std::string& property, const T& fromValue, const T& toValue, 
                float duration, EasingFunction easing = nullptr);
    
    template<typename T>
    void addTweenTo(const std::string& property, const T& toValue, 
                   float duration, EasingFunction easing = nullptr);
    
    template<typename T>
    void addTweenFrom(const std::string& property, const T& fromValue, 
                     float duration, EasingFunction easing = nullptr);
    
    void removeTween(const std::string& property);
    void clearTweens();
    
    // Tween control
    void play();
    void pause();
    void stop();
    void resume();
    void reset();
    void restart();
    
    // Tween properties
    void setDuration(float duration);
    void setDelay(float delay);
    void setTimeScale(float timeScale);
    void setRepeatCount(int count);
    void setYoyo(bool enabled);
    void setAutoReverse(bool enabled);
    void setStartTime(float time);
    void setEndTime(float time);
    
    // State queries
    bool isPlaying() const;
    bool isPaused() const;
    bool isStopped() const;
    bool isCompleted() const;
    bool isAnimating(const std::string& property) const;
    float getCurrentTime() const;
    float getDuration() const;
    float getProgress() const;
    TweenState getState() const;
    
    // Value access
    template<typename T>
    T getCurrentValue(const std::string& property) const;
    template<typename T>
    T getFromValue(const std::string& property) const;
    template<typename T>
    T getToValue(const std::string& property) const;
    template<typename T>
    T getProgress(const std::string& property) const;
    
    // Event handling
    void addEventListener(const std::string& eventType, std::function<void()> callback);
    void removeEventListener(const std::string& eventType, std::function<void()> callback);
    void clearEventListeners();
    
    // Utility methods
    void cloneFrom(const Tween& other);
    virtual std::unique_ptr<Tween> clone() const = 0;
    
    // Data access
    const std::map<std::string, std::unique_ptr<TweenValue>>& getTweens() const;
    const TweenProperties& getProperties() const;
    
protected:
    // Protected members
    std::map<std::string, std::unique_ptr<TweenValue>> tweens_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    TweenProperties properties_;
    
    float currentTime_;
    float globalTimeScale_;
    TweenState state_;
    
    // Protected helper methods
    virtual void updateTween(float deltaTime);
    virtual void processTweens(float deltaTime);
    virtual void triggerEvent(const std::string& eventType);
    virtual void updateTweenValue(const std::string& property, float deltaTime);
    
    // Helper methods
    virtual void interpolateValue(const std::string& property, float t);
    virtual void updateTweenProgress(const std::string& property, float deltaTime);
    virtual void checkTweenCompletion(const std::string& property);
    
    template<typename T>
    TweenValue* getTweenValueInternal(const std::string& property) const;
    
    template<typename T>
    void setTweenValueInternal(const std::string& property, const T& value);
};

// Specialized tween classes
class PositionTween : public Tween {
public:
    PositionTween();
    void addPositionTween(const std::string& property, const std::array<float, 2>& from, 
                      const std::array<float, 2>& to, float duration, 
                      EasingFunction easing = nullptr);
};

class RotationTween : public Tween {
public:
    RotationTween();
    void addRotationTween(const std::string& property, float fromAngle, float toAngle, 
                        float duration, EasingFunction easing = nullptr);
};

class ScaleTween : public Tween {
public:
    ScaleTween();
    void addScaleTween(const std::string& property, const std::array<float, 2>& from, 
                    const std::array<float, 2>& to, float duration, 
                    EasingFunction easing = nullptr);
};

class ColorTween : public Tween {
public:
    ColorTween();
    void addColorTween(const std::string& property, const std::array<float, 4>& from, 
                   const std::array<float, 4>& to, float duration, 
                   EasingFunction easing = nullptr);
};

class OpacityTween : public Tween {
public:
    OpacityTween();
    void addOpacityTween(const std::string& property, float fromOpacity, float toOpacity, 
                      float duration, EasingFunction easing = nullptr);
};

// Tween manager
class TweenManager {
public:
    TweenManager();
    ~TweenManager();
    
    // Tween management
    void addTween(const std::string& name, std::shared_ptr<Tween> tween);
    void removeTween(const std::string& name);
    std::shared_ptr<Tween> getTween(const std::string& name) const;
    bool hasTween(const std::string& name) const;
    const std::vector<std::string>& getTweenNames() const;
    
    // Global control
    void playAllTweens();
    void pauseAllTweens();
    void stopAllTweens();
    void resumeAllTweens();
    void resetAllTweens();
    
    // Time management
    void setGlobalTimeScale(float timeScale);
    float getGlobalTimeScale() const;
    void setCurrentTime(float time);
    float getCurrentTime() const;
    
    // State queries
    bool isPlaying(const std::string& name) const;
    bool isPaused(const std::string& name) const;
    bool isStopped(const std::string& name) const;
    bool isCompleted(const std::string& name) const;
    
    // Event handling
    void addTweenEventListener(const std::string& tweenName, const std::string& eventType, 
                           std::function<void()> callback);
    void removeTweenEventListener(const std::string& tweenName, const std::string& eventType, 
                                std::function<void()> callback);
    void clearTweenEventListeners();
    void clearTweenEventListeners();
    
    // Utility methods
    void cloneFrom(const TweenManager& other);
    std::unique_ptr<TweenManager> clone() const;
    
    // Data access
    const std::map<std::string, std::shared_ptr<Tween>>& getTweens() const;
    
protected:
    // Protected members
    std::map<std::string, std::shared_ptr<Tween>> tweens_;
    std::map<std::string, std::vector<std::function<void()>>> tweenEventListeners_;
    
    float globalTimeScale_;
    float currentTime_;
    
    // Protected helper methods
    virtual void updateTweens(float deltaTime);
    virtual void processTweenEvents();
    virtual void triggerTweenEvent(const std::string& tweenName, 
                             const std::string& eventType);
    virtual void updateTween(const std::shared_ptr<Tween>& tween, float deltaTime);
};

// Tween factory
class TweenFactory {
public:
    static std::unique_ptr<Tween> createTween(TweenType type);
    static std::unique_ptr<PositionTween> createPositionTween();
    static std::unique_ptr<RotationTween> createRotationTween();
    static std::unique_ptr<ScaleTween> createScaleTween();
    static std::unique_ptr<ColorTween> createColorTween();
    static std::unique_ptr<OpacityTween> createOpacityTween();
    static std::vector<TweenType> getAvailableTypes();
};

} // namespace RFAnimation