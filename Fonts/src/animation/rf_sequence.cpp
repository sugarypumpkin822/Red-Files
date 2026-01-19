#include "rf_sequence.h"
#include "rf_animation.h"
#include "rf_keyframe.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Animation {

Sequence::Sequence()
    : m_duration(0.0f)
    , m_currentTime(0.0f)
    , m_isPlaying(false)
    , m_isLooping(false)
    , m_playbackSpeed(1.0f)
    , m_direction(1)
{
    RF_LOG_INFO("Sequence created");
}

Sequence::~Sequence() {
    clear();
    RF_LOG_INFO("Sequence destroyed");
}

void Sequence::addTrack(KeyframeTrack* track) {
    if (track) {
        m_tracks.push_back(track);
        m_trackMap[track->getName()] = track;
        
        // Update sequence duration
        updateDuration();
        
        RF_LOG_INFO("Added track to sequence: %s", track->getName().c_str());
    }
}

void Sequence::removeTrack(const std::string& name) {
    auto it = m_trackMap.find(name);
    if (it != m_trackMap.end()) {
        auto track = it->second;
        
        // Remove from tracks
        auto trackIt = std::find(m_tracks.begin(), m_tracks.end(), track);
        if (trackIt != m_tracks.end()) {
            m_tracks.erase(trackIt);
        }
        
        // Remove from map
        m_trackMap.erase(it);
        
        // Update duration
        updateDuration();
        
        RF_LOG_INFO("Removed track from sequence: %s", name.c_str());
    }
}

void Sequence::removeTrack(KeyframeTrack* track) {
    if (!track) {
        return;
    }
    
    auto it = m_trackMap.find(track->getName());
    if (it != m_trackMap.end()) {
        m_tracks.erase(std::find(m_tracks.begin(), m_tracks.end(), track));
        m_trackMap.erase(it);
        
        updateDuration();
        
        RF_LOG_INFO("Removed track from sequence: %s", track->getName().c_str());
    }
}

void Sequence::clear() {
    m_tracks.clear();
    m_trackMap.clear();
    m_duration = 0.0f;
    m_currentTime = 0.0f;
    m_isPlaying = false;
    
    RF_LOG_INFO("Cleared all tracks from sequence");
}

KeyframeTrack* Sequence::getTrack(const std::string& name) {
    auto it = m_trackMap.find(name);
    return (it != m_trackMap.end()) ? it->second : nullptr;
}

std::vector<KeyframeTrack*> Sequence::getTracks() const {
    return m_tracks;
}

std::vector<std::string> Sequence::getTrackNames() const {
    std::vector<std::string> names;
    names.reserve(m_trackMap.size());
    
    for (const auto& pair : m_trackMap) {
        names.push_back(pair.first);
    }
    
    return names;
}

void Sequence::play() {
    if (!m_isPlaying) {
        m_isPlaying = true;
        
        if (m_currentTime >= m_duration && m_direction > 0) {
            m_currentTime = 0.0f;
        } else if (m_currentTime <= 0.0f && m_direction < 0) {
            m_currentTime = m_duration;
        }
        
        RF_LOG_INFO("Sequence started playing");
    }
}

void Sequence::pause() {
    if (m_isPlaying && !m_isPaused) {
        m_isPaused = true;
        RF_LOG_INFO("Sequence paused");
    }
}

void Sequence::stop() {
    m_isPlaying = false;
    m_isPaused = false;
    m_currentTime = 0.0f;
    
    RF_LOG_INFO("Sequence stopped");
}

void Sequence::resume() {
    if (m_isPlaying && m_isPaused) {
        m_isPaused = false;
        RF_LOG_INFO("Sequence resumed");
    }
}

void Sequence::reset() {
    m_currentTime = 0.0f;
    m_isPlaying = false;
    m_isPaused = false;
    
    RF_LOG_INFO("Sequence reset");
}

void Sequence::update(float deltaTime) {
    if (!m_isPlaying || m_isPaused || m_duration <= 0.0f) {
        return;
    }
    
    // Update time based on playback speed and direction
    m_currentTime += deltaTime * m_playbackSpeed * m_direction;
    
    // Handle looping
    if (m_isLooping) {
        if (m_currentTime >= m_duration) {
            m_currentTime = m_currentTime - m_duration;
            onSequenceLooped();
        } else if (m_currentTime < 0.0f) {
            m_currentTime = m_duration + m_currentTime;
            onSequenceLooped();
        }
    } else {
        // Handle non-looping boundaries
        if (m_currentTime >= m_duration) {
            m_currentTime = m_duration;
            stop();
            onSequenceCompleted();
        } else if (m_currentTime < 0.0f) {
            m_currentTime = 0.0f;
            stop();
            onSequenceCompleted();
        }
    }
    
    // Update all tracks
    for (auto& track : m_tracks) {
        if (track && track->isEnabledTrack()) {
            track->evaluate(m_currentTime);
        }
    }
}

void Sequence::setTime(float time) {
    m_currentTime = Math::clamp(time, 0.0f, m_duration);
    
    // Update all tracks
    for (auto& track : m_tracks) {
        if (track && track->isEnabledTrack()) {
            track->evaluate(m_currentTime);
        }
    }
}

void Sequence::setPlaybackSpeed(float speed) {
    m_playbackSpeed = speed;
    RF_LOG_INFO("Sequence playback speed set to %.2f", speed);
}

void Sequence::setDirection(int direction) {
    m_direction = Math::clamp(direction, -1, 1);
    RF_LOG_INFO("Sequence direction set to %d", direction);
}

void Sequence::setLooping(bool loop) {
    m_isLooping = loop;
    RF_LOG_INFO("Sequence looping set to %s", loop ? "true" : "false");
}

float Sequence::getDuration() const {
    return m_duration;
}

float Sequence::getCurrentTime() const {
    return m_currentTime;
}

float Sequence::getProgress() const {
    if (m_duration <= 0.0f) {
        return 0.0f;
    }
    return m_currentTime / m_duration;
}

float Sequence::getNormalizedTime() const {
    return getProgress();
}

bool Sequence::isPlaying() const {
    return m_isPlaying && !m_isPaused;
}

bool Sequence::isPaused() const {
    return m_isPaused;
}

bool Sequence::isCompleted() const {
    return !m_isPlaying && !m_isLooping && 
           ((m_direction > 0 && m_currentTime >= m_duration) ||
            (m_direction < 0 && m_currentTime <= 0.0f));
}

size_t Sequence::getTrackCount() const {
    return m_tracks.size();
}

void Sequence::updateDuration() {
    m_duration = 0.0f;
    
    for (const auto& track : m_tracks) {
        if (track) {
            const auto& keyframes = track->getKeyframes();
            if (!keyframes.empty()) {
                float trackDuration = keyframes.back().getTime();
                m_duration = Math::max(m_duration, trackDuration);
            }
        }
    }
}

void Sequence::onSequenceCompleted() {
    RF_LOG_INFO("Sequence completed");
    
    // Notify listeners
    for (auto& listener : m_listeners) {
        if (listener) {
            listener->onSequenceCompleted(this);
        }
    }
}

void Sequence::onSequenceLooped() {
    RF_LOG_INFO("Sequence looped");
    
    // Notify listeners
    for (auto& listener : m_listeners) {
        if (listener) {
            listener->onSequenceLooped(this);
        }
    }
}

void Sequence::addListener(SequenceListener* listener) {
    if (listener) {
        m_listeners.push_back(listener);
        RF_LOG_INFO("Added sequence listener");
    }
}

void Sequence::removeListener(SequenceListener* listener) {
    auto it = std::find(m_listeners.begin(), m_listeners.end(), listener);
    if (it != m_listeners.end()) {
        m_listeners.erase(it);
        RF_LOG_INFO("Removed sequence listener");
    }
}

void Sequence::clearListeners() {
    m_listeners.clear();
    RF_LOG_INFO("Cleared all sequence listeners");
}

// AnimationSequence implementation
AnimationSequence::AnimationSequence()
    : Sequence()
    , m_targetAnimation(nullptr)
    , m_blendWeight(0.0f)
    , m_blendDuration(0.0f)
    , m_currentBlendTime(0.0f)
    , m_isBlending(false)
{
    RF_LOG_INFO("AnimationSequence created");
}

AnimationSequence::~AnimationSequence() {
    RF_LOG_INFO("AnimationSequence destroyed");
}

void AnimationSequence::addAnimation(Animation* animation) {
    if (animation) {
        m_animations.push_back(animation);
        m_animationMap[animation->getName()] = animation;
        
        updateDuration();
        
        RF_LOG_INFO("Added animation to sequence: %s", animation->getName().c_str());
    }
}

void AnimationSequence::removeAnimation(const std::string& name) {
    auto it = m_animationMap.find(name);
    if (it != m_animationMap.end()) {
        auto animation = it->second;
        
        auto animIt = std::find(m_animations.begin(), m_animations.end(), animation);
        if (animIt != m_animations.end()) {
            m_animations.erase(animIt);
        }
        
        m_animationMap.erase(it);
        
        updateDuration();
        
        RF_LOG_INFO("Removed animation from sequence: %s", name.c_str());
    }
}

void AnimationSequence::removeAnimation(Animation* animation) {
    if (!animation) {
        return;
    }
    
    auto it = m_animationMap.find(animation->getName());
    if (it != m_animationMap.end()) {
        m_animations.erase(std::find(m_animations.begin(), m_animations.end(), animation));
        m_animationMap.erase(it);
        
        updateDuration();
        
        RF_LOG_INFO("Removed animation from sequence: %s", animation->getName().c_str());
    }
}

void AnimationSequence::clearAnimations() {
    m_animations.clear();
    m_animationMap.clear();
    m_targetAnimation = nullptr;
    
    updateDuration();
    
    RF_LOG_INFO("Cleared all animations from sequence");
}

Animation* AnimationSequence::getAnimation(const std::string& name) {
    auto it = m_animationMap.find(name);
    return (it != m_animationMap.end()) ? it->second : nullptr;
}

std::vector<Animation*> AnimationSequence::getAnimations() const {
    return m_animations;
}

void AnimationSequence::playAnimation(const std::string& name, float blendDuration) {
    auto animation = getAnimation(name);
    if (animation) {
        playAnimation(animation, blendDuration);
    }
}

void AnimationSequence::playAnimation(Animation* animation, float blendDuration) {
    if (!animation) {
        return;
    }
    
    // Start blending to new animation
    m_targetAnimation = animation;
    m_blendDuration = blendDuration;
    m_currentBlendTime = 0.0f;
    m_isBlending = true;
    
    // Start the target animation
    animation->play();
    
    RF_LOG_INFO("Starting blend to animation: %s", animation->getName().c_str());
}

void AnimationSequence::update(float deltaTime) {
    Sequence::update(deltaTime);
    
    // Update blending
    if (m_isBlending && m_targetAnimation) {
        m_currentBlendTime += deltaTime;
        m_blendWeight = m_currentBlendTime / m_blendDuration;
        
        if (m_blendWeight >= 1.0f) {
            // Blend complete
            m_blendWeight = 1.0f;
            m_isBlending = false;
            m_currentBlendTime = 0.0f;
            
            // Stop previous animation if needed
            for (auto& anim : m_animations) {
                if (anim != m_targetAnimation && anim->isPlaying()) {
                    anim->stop();
                }
            }
        }
    }
}

void AnimationSequence::setBlendWeight(float weight) {
    m_blendWeight = Math::clamp(weight, 0.0f, 1.0f);
}

float AnimationSequence::getBlendWeight() const {
    return m_blendWeight;
}

bool AnimationSequence::isBlending() const {
    return m_isBlending;
}

Animation* AnimationSequence::getCurrentAnimation() const {
    return m_targetAnimation;
}

void AnimationSequence::updateDuration() {
    Sequence::updateDuration();
    
    // Add blend duration if blending
    if (m_isBlending) {
        m_duration += m_blendDuration;
    }
}

} // namespace Animation
} // namespace RedFiles