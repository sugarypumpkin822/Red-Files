#pragma once

#include "rf_animation.h"
#include <vector>
#include <memory>
#include <map>
#include <unordered_set>

namespace RFAnimation {

// Controller types
enum class ControllerType {
    SINGLE,
    PARALLEL,
    SEQUENTIAL,
    BLEND,
    MASK
};

// Blend modes
enum class BlendMode {
    OVER,
    ADD,
    MULTIPLY,
    SCREEN,
    OVERLAY,
    DARKEN,
    LIGHTEN,
    COLOR_DODGE,
    COLOR_BURN,
    HARD_LIGHT,
    SOFT_LIGHT,
    DIFFERENCE,
    EXCLUSION
};

// Controller state
struct ControllerState {
    bool isPlaying;
    bool isPaused;
    bool isLooping;
    float currentTime;
    float duration;
    float playbackSpeed;
    WrapMode wrapMode;
    BlendMode blendMode;
    float weight;
    float fadeInTime;
    float fadeOutTime;
    
    ControllerState() : isPlaying(false), isPaused(false), isLooping(false), 
                   currentTime(0.0f), duration(0.0f), playbackSpeed(1.0f), 
                   wrapMode(WrapMode::LOOP), blendMode(BlendMode::OVER), weight(1.0f),
                   fadeInTime(0.0f), fadeOutTime(0.0f) {}
};

// Animation controller for managing multiple animations
class AnimationController {
public:
    AnimationController();
    ~AnimationController();
    
    // Animation management
    void addAnimation(const std::string& name, std::shared_ptr<Animation> animation);
    void removeAnimation(const std::string& name);
    std::shared_ptr<Animation> getAnimation(const std::string& name) const;
    bool hasAnimation(const std::string& name) const;
    const std::vector<std::string>& getAnimationNames() const;
    
    // Controller management
    void setControllerType(ControllerType type);
    ControllerType getControllerType() const;
    void setBlendMode(BlendMode mode);
    BlendMode getBlendMode() const;
    void setWeight(float weight);
    float getWeight() const;
    
    // Playback control
    void play();
    void pause();
    void stop();
    void resume();
    void reset();
    void restart();
    
    // Timing control
    void setDuration(float duration);
    void setCurrentTime(float time);
    void setPlaybackSpeed(float speed);
    void setWrapMode(WrapMode mode);
    void setFadeInTime(float time);
    void setFadeOutTime(float time);
    
    // State queries
    bool isPlaying() const;
    bool isPaused() const;
    bool isStopped() const;
    float getCurrentTime() const;
    float getDuration() const;
    float getPlaybackSpeed() const;
    WrapMode getWrapMode() const;
    float getWeight() const;
    float getFadeInTime() const;
    float getFadeOutTime() const;
    
    // Update and rendering
    void update(float deltaTime);
    void render() const;
    
    // Animation state
    void setState(const ControllerState& state);
    const ControllerState& getState() const;
    
    // Event handling
    void addEventListener(const std::string& eventType, AnimationCallback callback);
    void removeEventListener(const std::string& eventType, AnimationCallback callback);
    void clearEventListeners();
    
    // Advanced features
    void setAnimationPriority(const std::string& name, int priority);
    void setAnimationLayer(const std::string& name, int layer);
    void setAnimationMask(const std::string& name, uint32_t mask);
    
    // Utility methods
    void cloneFrom(const AnimationController& other);
    std::unique_ptr<AnimationController> clone() const;
    
    // Data access
    const std::map<std::string, std::shared_ptr<Animation>>& getAnimations() const;
    const std::map<std::string, int>& getPriorities() const;
    const std::map<std::string, int>& getLayers() const;
    const std::map<std::string, uint32_t>& getMasks() const;
    
protected:
    // Protected members
    std::map<std::string, std::shared_ptr<Animation>> animations_;
    std::map<std::string, int> priorities_;
    std::map<std::string, int> layers_;
    std::map<std::string, uint32_t> masks_;
    std::map<std::string, std::vector<AnimationCallback>> eventListeners_;
    
    ControllerState state_;
    ControllerType type_;
    
    // Protected helper methods
    virtual void updateController(float deltaTime);
    virtual void renderController() const;
    virtual void processAnimations(float deltaTime);
    virtual void blendAnimations();
    virtual void triggerEvent(const AnimationEvent& event);
    virtual void calculateEffectiveValues();
    
    // Helper methods for different controller types
    void updateSingleController(float deltaTime);
    void updateParallelController(float deltaTime);
    void updateSequentialController(float deltaTime);
    void updateBlendController(float deltaTime);
    void updateMaskController(float deltaTime);
    
    // Blending utilities
    float calculateBlendFactor(const std::string& name) const;
    void applyBlendMode(float& baseValue, float& blendValue, BlendMode mode, float factor);
};

// Specialized controllers
class SingleAnimationController : public AnimationController {
public:
    SingleAnimationController();
    void setAnimation(std::shared_ptr<Animation> animation);
};

class ParallelAnimationController : public AnimationController {
public:
    ParallelAnimationController();
    void addParallelAnimation(const std::string& name, std::shared_ptr<Animation> animation);
};

class SequentialAnimationController : public AnimationController {
public:
    SequentialAnimationController();
    void addSequentialAnimation(const std::string& name, std::shared_ptr<Animation> animation);
};

class BlendAnimationController : public AnimationController {
public:
    BlendAnimationController();
    void setBlendWeights(const std::map<std::string, float>& weights);
};

class MaskAnimationController : public AnimationController {
public:
    MaskAnimationController();
    void setAnimationMasks(const std::map<std::string, uint32_t>& masks);
};

} // namespace RFAnimation