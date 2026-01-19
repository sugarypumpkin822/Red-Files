#include "rf_animation_controller.h"
#include "rf_animation.h"
#include "rf_animation_state.h"
#include "../core/rf_memory.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Animation {

AnimationController::AnimationController()
    : m_currentAnimation(nullptr)
    , m_globalTime(0.0f)
    , m_timeScale(1.0f)
    , m_isPaused(false)
    , m_maxAnimations(100)
{
    m_animations.reserve(m_maxAnimations);
    RF_LOG_INFO("AnimationController created");
}

AnimationController::~AnimationController() {
    clear();
    RF_LOG_INFO("AnimationController destroyed");
}

void AnimationController::update(float deltaTime) {
    if (m_isPaused) {
        return;
    }
    
    // Update global time
    m_globalTime += deltaTime * m_timeScale;
    
    // Update all active animations
    for (auto& animation : m_animations) {
        if (animation && animation->isPlaying()) {
            animation->update(deltaTime * m_timeScale);
        }
    }
    
    // Update current animation
    if (m_currentAnimation && m_currentAnimation->isPlaying()) {
        m_currentAnimation->update(deltaTime * m_timeScale);
    }
}

Animation* AnimationController::createAnimation(const std::string& name) {
    if (m_animations.size() >= m_maxAnimations) {
        RF_LOG_WARNING("Maximum number of animations reached");
        return nullptr;
    }
    
    auto animation = RF_NEW(Animation);
    animation->setName(name);
    animation->setController(this);
    
    m_animations.push_back(animation);
    m_animationMap[name] = animation;
    
    RF_LOG_INFO("Created animation: %s", name.c_str());
    return animation;
}

void AnimationController::destroyAnimation(Animation* animation) {
    if (!animation) {
        return;
    }
    
    // Remove from active animations
    auto it = std::find(m_animations.begin(), m_animations.end(), animation);
    if (it != m_animations.end()) {
        m_animations.erase(it);
    }
    
    // Remove from map
    auto mapIt = m_animationMap.find(animation->getName());
    if (mapIt != m_animationMap.end()) {
        m_animationMap.erase(mapIt);
    }
    
    // Clear current animation if needed
    if (m_currentAnimation == animation) {
        m_currentAnimation = nullptr;
    }
    
    RF_DELETE(animation);
    RF_LOG_INFO("Destroyed animation: %s", animation->getName().c_str());
}

void AnimationController::destroyAnimation(const std::string& name) {
    auto it = m_animationMap.find(name);
    if (it != m_animationMap.end()) {
        destroyAnimation(it->second);
    }
}

Animation* AnimationController::getAnimation(const std::string& name) {
    auto it = m_animationMap.find(name);
    return (it != m_animationMap.end()) ? it->second : nullptr;
}

Animation* AnimationController::getCurrentAnimation() const {
    return m_currentAnimation;
}

void AnimationController::playAnimation(const std::string& name) {
    auto animation = getAnimation(name);
    if (animation) {
        playAnimation(animation);
    }
}

void AnimationController::playAnimation(Animation* animation) {
    if (!animation) {
        return;
    }
    
    // Stop current animation
    if (m_currentAnimation && m_currentAnimation != animation) {
        m_currentAnimation->stop();
    }
    
    // Play new animation
    m_currentAnimation = animation;
    animation->play();
    
    RF_LOG_INFO("Playing animation: %s", animation->getName().c_str());
}

void AnimationController::stopAnimation(const std::string& name) {
    auto animation = getAnimation(name);
    if (animation) {
        animation->stop();
        
        if (m_currentAnimation == animation) {
            m_currentAnimation = nullptr;
        }
    }
}

void AnimationController::stopAllAnimations() {
    for (auto& animation : m_animations) {
        if (animation) {
            animation->stop();
        }
    }
    
    m_currentAnimation = nullptr;
    RF_LOG_INFO("Stopped all animations");
}

void AnimationController::pauseAnimation(const std::string& name) {
    auto animation = getAnimation(name);
    if (animation) {
        animation->pause();
    }
}

void AnimationController::pauseAllAnimations() {
    for (auto& animation : m_animations) {
        if (animation) {
            animation->pause();
        }
    }
    
    RF_LOG_INFO("Paused all animations");
}

void AnimationController::resumeAnimation(const std::string& name) {
    auto animation = getAnimation(name);
    if (animation) {
        animation->resume();
    }
}

void AnimationController::resumeAllAnimations() {
    for (auto& animation : m_animations) {
        if (animation) {
            animation->resume();
        }
    }
    
    RF_LOG_INFO("Resumed all animations");
}

void AnimationController::clear() {
    for (auto& animation : m_animations) {
        if (animation) {
            RF_DELETE(animation);
        }
    }
    
    m_animations.clear();
    m_animationMap.clear();
    m_currentAnimation = nullptr;
    
    RF_LOG_INFO("Cleared all animations");
}

void AnimationController::setTimeScale(float timeScale) {
    m_timeScale = timeScale;
    RF_LOG_INFO("Time scale set to %.2f", timeScale);
}

void AnimationController::pause() {
    m_isPaused = true;
    RF_LOG_INFO("Animation controller paused");
}

void AnimationController::resume() {
    m_isPaused = false;
    RF_LOG_INFO("Animation controller resumed");
}

bool AnimationController::isPaused() const {
    return m_isPaused;
}

float AnimationController::getGlobalTime() const {
    return m_globalTime;
}

void AnimationController::setGlobalTime(float time) {
    m_globalTime = time;
    RF_LOG_INFO("Global time set to %.2f", time);
}

size_t AnimationController::getAnimationCount() const {
    return m_animations.size();
}

std::vector<std::string> AnimationController::getAnimationNames() const {
    std::vector<std::string> names;
    names.reserve(m_animationMap.size());
    
    for (const auto& pair : m_animationMap) {
        names.push_back(pair.first);
    }
    
    return names;
}

void AnimationController::setMaxAnimations(size_t maxAnimations) {
    m_maxAnimations = maxAnimations;
    m_animations.reserve(maxAnimations);
    RF_LOG_INFO("Max animations set to %zu", maxAnimations);
}

void AnimationController::onAnimationCompleted(Animation* animation) {
    RF_LOG_INFO("Animation completed: %s", animation->getName().c_str());
    
    // Notify listeners
    for (auto& listener : m_listeners) {
        if (listener) {
            listener->onAnimationCompleted(animation);
        }
    }
}

void AnimationController::onAnimationLooped(Animation* animation) {
    RF_LOG_INFO("Animation looped: %s", animation->getName().c_str());
    
    // Notify listeners
    for (auto& listener : m_listeners) {
        if (listener) {
            listener->onAnimationLooped(animation);
        }
    }
}

void AnimationController::onAnimationUpdated(Animation* animation, float time) {
    // Notify listeners
    for (auto& listener : m_listeners) {
        if (listener) {
            listener->onAnimationUpdated(animation, time);
        }
    }
}

void AnimationController::addListener(AnimationListener* listener) {
    if (listener) {
        m_listeners.push_back(listener);
        RF_LOG_INFO("Added animation listener");
    }
}

void AnimationController::removeListener(AnimationListener* listener) {
    auto it = std::find(m_listeners.begin(), m_listeners.end(), listener);
    if (it != m_listeners.end()) {
        m_listeners.erase(it);
        RF_LOG_INFO("Removed animation listener");
    }
}

void AnimationController::clearListeners() {
    m_listeners.clear();
    RF_LOG_INFO("Cleared all animation listeners");
}

} // namespace Animation
} // namespace RedFiles