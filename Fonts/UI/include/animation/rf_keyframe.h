#pragma once

#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace RFAnimation {

// Keyframe types
enum class KeyframeType {
    POSITION,
    ROTATION,
    SCALE,
    COLOR,
    OPACITY,
    CUSTOM
};

// Keyframe structure
struct Keyframe {
    float time;
    std::map<std::string, std::any> values;
    KeyframeType type;
    std::string name;
    bool isTriggered;
    std::function<void()> onTrigger;
    
    Keyframe() : time(0.0f), type(KeyframeType::POSITION), name(""), isTriggered(false), onTrigger(nullptr) {}
    Keyframe(float time_, KeyframeType type_, const std::string& name_, 
              std::function<void()> onTrigger_ = nullptr)
        : time(time_), type(type_), name(name_), isTriggered(false), onTrigger(onTrigger_) {}
};

// Keyframe track
class KeyframeTrack {
public:
    KeyframeTrack(const std::string& name, KeyframeType type);
    ~KeyframeTrack() = default;
    
    // Keyframe management
    void addKeyframe(const Keyframe& keyframe);
    void removeKeyframe(float time);
    void removeKeyframe(float time);
    void clearKeyframes();
    
    // Keyframe queries
    const std::vector<Keyframe>& getKeyframes() const;
    Keyframe* getKeyframeAt(float time) const;
    Keyframe* getNextKeyframe(float time) const;
    Keyframe* getPreviousKeyframe(float time) const;
    std::vector<Keyframe*> getKeyframesInRange(float startTime, float endTime) const;
    
    // Keyframe evaluation
    std::map<std::string, std::any> evaluateAt(float time) const;
    std::map<std::string, std::any> evaluateBetween(float startTime, float endTime) const;
    
    // Utility methods
    void cloneFrom(const KeyframeTrack& other);
    std::unique_ptr<KeyframeTrack> clone() const;
    
    // Data access
    const std::string& getName() const;
    KeyframeType getType() const;
    const std::vector<Keyframe>& getKeyframes() const;
    
protected:
    std::string name_;
    KeyframeType type_;
    std::vector<Keyframe> keyframes_;
    
    // Protected helper methods
    virtual void sortKeyframes();
    virtual void interpolateKeyframes();
    virtual void evaluateTrack(float currentTime);
    virtual void triggerKeyframeEvents(float currentTime);
    
    // Helper methods
    virtual std::any interpolateValue(const std::string& key, float time);
    virtual std::any interpolateValueBetween(const std::string& key, float t1, float t2);
};

// Animation sequence class
class Sequence {
public:
    Sequence(const std::string& name);
    ~Sequence() = default;
    
    // Sequence management
    void addKeyframeTrack(const std::string& trackName, KeyframeType type);
    void removeKeyframeTrack(const std::string& trackName);
    void clearKeyframeTrack(const std::string& trackName);
    std::shared_ptr<KeyframeTrack> getKeyframeTrack(const std::string& trackName) const;
    const std::vector<std::string>& getKeyframeTrackNames() const;
    
    // Sequence control
    void play();
    void pause();
    void stop();
    void reset();
    void setLooping(bool enabled);
    void setPlaybackSpeed(float speed);
    
    // State queries
    bool isPlaying() const;
    bool isPaused() const;
    bool isLooping() const;
    float getCurrentTime() const;
    float getDuration() const;
    float getPlaybackSpeed() const;
    
    // Data access
    const std::string& getName() const;
    const std::map<std::string, std::shared_ptr<KeyframeTrack>>& getKeyframeTracks() const;
    
protected:
    std::map<std::string, std::shared_ptr<KeyframeTrack>> keyframeTracks_;
    bool isPlaying_;
    bool isPaused_;
    bool isLooping_;
    float currentTime_;
    float duration_;
    float playbackSpeed_;
    
    // Protected helper methods
    virtual void updateSequence(float deltaTime);
    virtual void processKeyframeTracks(float deltaTime);
    virtual void triggerSequenceEvents();
    virtual void updateKeyframeTrack(const std::string& trackName, float deltaTime);
    virtual void triggerKeyframeEvent(const std::string& trackName, const Keyframe& keyframe);
};

// Animation timeline class
class Timeline {
public:
    Timeline();
    ~Timeline() = default;
    
    // Timeline management
    void addSequence(const std::shared_ptr<Sequence> sequence);
    void removeSequence(const std::string& sequenceName);
    void clearSequences();
    
    // Timeline control
    void play();
    void pause();
    void stop();
    void reset();
    void setLooping(bool enabled);
    void setPlaybackSpeed(float speed);
    
    // Timeline state
    bool isPlaying() const;
    bool isPaused() const;
    bool isLooping() const;
    float getCurrentTime() const;
    float getDuration() const;
    float getPlaybackSpeed() const;
    
    // Data access
    const std::vector<std::shared_ptr<Sequence>>& getSequences() const;
    
    // Event handling
    void addTimelineEventListener(const std::string& eventType, std::function<void()> callback);
    void removeTimelineEventListener(const std::string& eventType, std::function<void()> callback);
    void clearTimelineEventListeners();
    
    // Utility methods
    void cloneFrom(const Timeline& other);
    std::unique_ptr<Timeline> clone() const;
    
protected:
    std::vector<std::shared_ptr<Sequence>> sequences_;
    bool isPlaying_;
    bool isPaused_;
    bool isLooping_;
    float currentTime_;
    float duration_;
    float playbackSpeed_;
    
    // Protected helper methods
    virtual void updateTimeline(float deltaTime);
    virtual void processSequences(float deltaTime);
    virtual void triggerTimelineEvents();
    virtual void updateSequence(const std::shared_ptr<Sequence>& sequence, float deltaTime);
    virtual void triggerSequenceEvent(const std::shared_ptr<Sequence>& sequence);
    
    // Helper methods
    virtual void sortSequences();
    virtual void calculateTimelineDuration();
    virtual void triggerTimelineEvent(const std::string& eventType, 
                             const std::shared_ptr<Sequence>& sequence);
};

// Animation manager class
class AnimationManager {
public:
    AnimationManager();
    ~AnimationManager();
    
    // Animation management
    void addAnimation(const std::string& name, std::shared_ptr<Animation> animation);
    void removeAnimation(const std::string& name);
    std::shared_ptr<Animation> getAnimation(const std::string& name) const;
    bool hasAnimation(const std::string& name) const;
    const std::vector<std::string>& getAnimationNames() const;
    
    // Animation control
    void playAnimation(const std::string& name);
    void pauseAnimation(const std::string& name);
    void stopAnimation(const std::string& name);
    void resumeAnimation(const std::string& name);
    void resetAnimation(const std::string& name);
    
    // Global control
    void playAllAnimations();
    void pauseAllAnimations();
    void stopAllAnimations();
    void resetAllAnimations();
    
    // Animation state queries
    bool isPlaying(const std::string& name) const;
    bool isPaused(const std::string& name) const;
    bool isStopped(const std::string& name) const;
    
    // Event handling
    void addAnimationEventListener(const std::string& animationName, const std::string& eventType, 
                              std::function<void(const AnimationEvent&)> callback);
    void removeAnimationEventListener(const std::string& animationName, const std::string& eventType, 
                                 std::function<void(const AnimationEvent&)> callback);
    void clearAnimationEventListeners();
    void clearAnimationEventListeners();
    
    // Utility methods
    void cloneFrom(const AnimationManager& other);
    std::unique_ptr<AnimationManager> clone() const;
    
    // Data access
    const std::map<std::string, std::shared_ptr<Animation>>& getAnimations() const;
    
protected:
    std::map<std::string, std::shared_ptr<Animation>> animations_;
    std::map<std::string, std::vector<std::function<void()>> animationEventListeners_;
    
    // Protected helper methods
    virtual void updateAnimations(float deltaTime);
    virtual void processAnimationEvents();
    virtual void triggerAnimationEvent(const std::string& animationName, 
                             const AnimationEvent& event);
    virtual void updateAnimation(const std::shared_ptr<Animation>& animation, float deltaTime);
};

} // namespace RFAnimation