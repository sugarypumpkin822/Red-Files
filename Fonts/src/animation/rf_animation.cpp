#include "rf_animation.h"
#include "rf_animation_controller.h"
#include "rf_animation_state.h"
#include "rf_easing.h"
#include "rf_interpolator.h"
#include "rf_keyframe.h"
#include "rf_tween.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Animation {

Animation::Animation()
    : m_duration(0.0f)
    , m_currentTime(0.0f)
    , m_isPlaying(false)
    , m_isLooping(false)
    , m_isPaused(false)
    , m_playbackSpeed(1.0f)
    , m_direction(1)
    , m_state(nullptr)
    , m_controller(nullptr)
{
    RF_LOG_INFO("Animation created");
}

Animation::~Animation() {
    RF_LOG_INFO("Animation destroyed");
}

void Animation::initialize(float duration, bool loop) {
    m_duration = duration;
    m_isLooping = loop;
    m_currentTime = 0.0f;
    m_isPlaying = false;
    m_isPaused = false;
    
    RF_LOG_INFO("Animation initialized: duration=%.2f, loop=%s", 
                duration, loop ? "true" : "false");
}

void Animation::play() {
    if (!m_isPlaying) {
        m_isPlaying = true;
        m_isPaused = false;
        
        if (m_currentTime >= m_duration && m_direction > 0) {
            m_currentTime = 0.0f;
        } else if (m_currentTime <= 0.0f && m_direction < 0) {
            m_currentTime = m_duration;
        }
        
        RF_LOG_INFO("Animation started playing");
    }
}

void Animation::pause() {
    if (m_isPlaying && !m_isPaused) {
        m_isPaused = true;
        RF_LOG_INFO("Animation paused");
    }
}

void Animation::stop() {
    m_isPlaying = false;
    m_isPaused = false;
    m_currentTime = 0.0f;
    RF_LOG_INFO("Animation stopped");
}

void Animation::resume() {
    if (m_isPlaying && m_isPaused) {
        m_isPaused = false;
        RF_LOG_INFO("Animation resumed");
    }
}

void Animation::reset() {
    m_currentTime = 0.0f;
    m_isPlaying = false;
    m_isPaused = false;
    RF_LOG_INFO("Animation reset");
}

void Animation::update(float deltaTime) {
    if (!m_isPlaying || m_isPaused || m_duration <= 0.0f) {
        return;
    }
    
    // Update time based on playback speed and direction
    m_currentTime += deltaTime * m_playbackSpeed * m_direction;
    
    // Handle looping
    if (m_isLooping) {
        if (m_currentTime >= m_duration) {
            m_currentTime = m_currentTime - m_duration;
            onAnimationLooped();
        } else if (m_currentTime < 0.0f) {
            m_currentTime = m_duration + m_currentTime;
            onAnimationLooped();
        }
    } else {
        // Handle non-looping boundaries
        if (m_currentTime >= m_duration) {
            m_currentTime = m_duration;
            stop();
            onAnimationCompleted();
        } else if (m_currentTime < 0.0f) {
            m_currentTime = 0.0f;
            stop();
            onAnimationCompleted();
        }
    }
    
    // Update animation state
    if (m_state) {
        m_state->update(m_currentTime, m_duration);
    }
    
    // Notify controller
    if (m_controller) {
        m_controller->onAnimationUpdated(this, m_currentTime);
    }
}

void Animation::setTime(float time) {
    m_currentTime = Math::clamp(time, 0.0f, m_duration);
    
    if (m_state) {
        m_state->update(m_currentTime, m_duration);
    }
    
    if (m_controller) {
        m_controller->onAnimationUpdated(this, m_currentTime);
    }
}

void Animation::setPlaybackSpeed(float speed) {
    m_playbackSpeed = speed;
    RF_LOG_INFO("Animation playback speed set to %.2f", speed);
}

void Animation::setDirection(int direction) {
    m_direction = Math::clamp(direction, -1, 1);
    RF_LOG_INFO("Animation direction set to %d", direction);
}

void Animation::setLooping(bool loop) {
    m_isLooping = loop;
    RF_LOG_INFO("Animation looping set to %s", loop ? "true" : "false");
}

float Animation::getProgress() const {
    if (m_duration <= 0.0f) {
        return 0.0f;
    }
    return m_currentTime / m_duration;
}

float Animation::getNormalizedTime() const {
    return getProgress();
}

bool Animation::isPlaying() const {
    return m_isPlaying && !m_isPaused;
}

bool Animation::isPaused() const {
    return m_isPaused;
}

bool Animation::isCompleted() const {
    return !m_isPlaying && !m_isLooping && 
           ((m_direction > 0 && m_currentTime >= m_duration) ||
            (m_direction < 0 && m_currentTime <= 0.0f));
}

void Animation::addKeyframe(const Keyframe& keyframe) {
    m_keyframes.push_back(keyframe);
    
    // Sort keyframes by time
    std::sort(m_keyframes.begin(), m_keyframes.end(), 
              [](const Keyframe& a, const Keyframe& b) {
                  return a.time < b.time;
              });
    
    RF_LOG_INFO("Added keyframe at time %.2f", keyframe.time);
}

void Animation::removeKeyframe(int index) {
    if (index >= 0 && index < static_cast<int>(m_keyframes.size())) {
        m_keyframes.erase(m_keyframes.begin() + index);
        RF_LOG_INFO("Removed keyframe at index %d", index);
    }
}

void Animation::clearKeyframes() {
    m_keyframes.clear();
    RF_LOG_INFO("Cleared all keyframes");
}

Keyframe* Animation::getKeyframeAtTime(float time) {
    for (auto& keyframe : m_keyframes) {
        if (Math::abs(keyframe.time - time) < 0.001f) {
            return &keyframe;
        }
    }
    return nullptr;
}

std::vector<Keyframe> Animation::getKeyframesInRange(float startTime, float endTime) {
    std::vector<Keyframe> result;
    
    for (const auto& keyframe : m_keyframes) {
        if (keyframe.time >= startTime && keyframe.time <= endTime) {
            result.push_back(keyframe);
        }
    }
    
    return result;
}

void Animation::setState(AnimationState* state) {
    m_state = state;
    if (m_state) {
        m_state->setAnimation(this);
        m_state->enter();
    }
}

void Animation::setController(AnimationController* controller) {
    m_controller = controller;
}

void Animation::onAnimationCompleted() {
    RF_LOG_INFO("Animation completed");
    
    if (m_controller) {
        m_controller->onAnimationCompleted(this);
    }
}

void Animation::onAnimationLooped() {
    RF_LOG_INFO("Animation looped");
    
    if (m_controller) {
        m_controller->onAnimationLooped(this);
    }
}

} // namespace Animation
} // namespace RedFiles