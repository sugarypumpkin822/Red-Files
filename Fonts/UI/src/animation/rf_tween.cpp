#include "rf_tween.h"
#include "rf_easing.h"
#include "rf_interpolator.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Animation {

Tween::Tween()
    : m_fromValue(0.0f)
    , m_toValue(0.0f)
    , m_currentValue(0.0f)
    , m_duration(1.0f)
    , m_currentTime(0.0f)
    , m_isPlaying(false)
    , m_isPaused(false)
    , m_isCompleted(false)
    , m_isLooping(false)
    , m_easingType(EasingType::Linear)
    , m_interpolationType(InterpolationType::Linear)
    , m_playbackSpeed(1.0f)
    , m_direction(1)
    , m_delay(0.0f)
    , m_delayTime(0.0f)
    , m_repeatCount(1)
    , m_currentRepeat(0)
    , m_yoyo(false)
    , m_pingPong(false)
    , m_reverseOnComplete(false)
{
    RF_LOG_INFO("Tween created");
}

Tween::~Tween() {
    RF_LOG_INFO("Tween destroyed");
}

void Tween::initialize(float from, float to, float duration, EasingType easingType, 
                   InterpolationType interpolationType) {
    m_fromValue = from;
    m_toValue = to;
    m_currentValue = from;
    m_duration = duration;
    m_currentTime = 0.0f;
    m_isPlaying = false;
    m_isCompleted = false;
    m_easingType = easingType;
    m_interpolationType = interpolationType;
    
    RF_LOG_INFO("Tween initialized: from=%.2f, to=%.2f, duration=%.2f", from, to, duration);
}

void Tween::play() {
    if (!m_isPlaying && !m_isCompleted) {
        m_isPlaying = true;
        m_isPaused = false;
        
        if (m_delay > 0.0f) {
            m_delayTime = 0.0f;
        }
        
        RF_LOG_INFO("Tween started playing");
    }
}

void Tween::pause() {
    if (m_isPlaying && !m_isPaused) {
        m_isPaused = true;
        RF_LOG_INFO("Tween paused");
    }
}

void Tween::stop() {
    m_isPlaying = false;
    m_isPaused = false;
    m_currentTime = 0.0f;
    m_currentValue = m_fromValue;
    m_isCompleted = false;
    
    RF_LOG_INFO("Tween stopped");
}

void Tween::resume() {
    if (m_isPlaying && m_isPaused) {
        m_isPaused = false;
        RF_LOG_INFO("Tween resumed");
    }
}

void Tween::reset() {
    m_currentTime = 0.0f;
    m_currentValue = m_fromValue;
    m_isPlaying = false;
    m_isPaused = false;
    m_isCompleted = false;
    
    RF_LOG_INFO("Tween reset");
}

void Tween::update(float deltaTime) {
    if (!m_isPlaying || m_isPaused || m_isCompleted) {
        return;
    }
    
    // Handle delay
    if (m_delay > 0.0f) {
        m_delayTime += deltaTime;
        if (m_delayTime < m_delay) {
            return;
        }
    }
    
    // Update time
    m_currentTime += deltaTime * m_playbackSpeed * m_direction;
    
    // Check if tween is complete
    bool timeComplete = false;
    if (m_direction > 0) {
        timeComplete = m_currentTime >= m_duration;
    } else {
        timeComplete = m_currentTime <= 0.0f;
    }
    
    if (timeComplete) {
        handleCompletion();
        return;
    }
    
    // Calculate progress
    float progress = m_currentTime / m_duration;
    progress = Math::clamp(progress, 0.0f, 1.0f);
    
    // Apply easing
    float easedProgress = applyEasing(m_easingType, progress);
    
    // Interpolate value
    m_currentValue = Math::lerp(m_fromValue, m_toValue, easedProgress);
    
    // Notify update
    onUpdate(m_currentValue, progress);
}

void Tween::setFrom(float from) {
    m_fromValue = from;
    if (!m_isPlaying) {
        m_currentValue = from;
    }
}

void Tween::setTo(float to) {
    m_toValue = to;
}

void Tween::setDuration(float duration) {
    m_duration = duration;
}

void Tween::setEasingType(EasingType type) {
    m_easingType = type;
}

void Tween::setInterpolationType(InterpolationType type) {
    m_interpolationType = type;
}

void Tween::setPlaybackSpeed(float speed) {
    m_playbackSpeed = speed;
}

void Tween::setDirection(int direction) {
    m_direction = Math::clamp(direction, -1, 1);
}

void Tween::setDelay(float delay) {
    m_delay = delay;
}

void Tween::setLooping(bool loop) {
    m_isLooping = loop;
}

void Tween::setRepeatCount(int count) {
    m_repeatCount = count;
}

void Tween::setYoyo(bool yoYo) {
    m_yoyo = yoYo;
}

void Tween::setPingPong(bool pingPong) {
    m_pingPong = pingPong;
}

void Tween::setReverseOnComplete(bool reverse) {
    m_reverseOnComplete = reverse;
}

float Tween::getFrom() const {
    return m_fromValue;
}

float Tween::getTo() const {
    return m_toValue;
}

float Tween::getCurrentValue() const {
    return m_currentValue;
}

float Tween::getDuration() const {
    return m_duration;
}

float Tween::getCurrentTime() const {
    return m_currentTime;
}

float Tween::getProgress() const {
    if (m_duration <= 0.0f) {
        return 0.0f;
    }
    return Math::clamp(m_currentTime / m_duration, 0.0f, 1.0f);
}

float Tween::getNormalizedProgress() const {
    return getProgress();
}

bool Tween::isPlaying() const {
    return m_isPlaying && !m_isPaused;
}

bool Tween::isPaused() const {
    return m_isPaused;
}

bool Tween::isCompleted() const {
    return m_isCompleted;
}

bool Tween::isLooping() const {
    return m_isLooping;
}

EasingType Tween::getEasingType() const {
    return m_easingType;
}

InterpolationType Tween::getInterpolationType() const {
    return m_interpolationType;
}

float Tween::getPlaybackSpeed() const {
    return m_playbackSpeed;
}

int Tween::getDirection() const {
    return m_direction;
}

float Tween::getDelay() const {
    return m_delay;
}

int Tween::getRepeatCount() const {
    return m_repeatCount;
}

int Tween::getCurrentRepeat() const {
    return m_currentRepeat;
}

bool Tween::isYoyo() const {
    return m_yoyo;
}

bool Tween::isPingPong() const {
    return m_pingPong;
}

bool Tween::shouldReverseOnComplete() const {
    return m_reverseOnComplete;
}

void Tween::handleCompletion() {
    m_currentRepeat++;
    
    bool shouldRepeat = (m_currentRepeat < m_repeatCount);
    bool shouldReverse = (m_yoyo && (m_currentRepeat % 2 == 1));
    bool shouldPingPong = (m_pingPong && (m_currentRepeat % 2 == 1));
    
    if (shouldRepeat) {
        if (shouldReverse || shouldPingPong) {
            // Reverse the tween
            std::swap(m_fromValue, m_toValue);
            m_direction *= -1;
        }
        
        // Reset time and continue
        m_currentTime = 0.0f;
        m_isCompleted = false;
        
        if (!m_isLooping) {
            m_isPlaying = false;
        }
    } else {
        m_isCompleted = true;
        m_isPlaying = false;
    }
    
    onCompleted(m_currentValue);
}

void Tween::onUpdate(float value, float progress) {
    // Override in derived classes
    (void)value;
    (void)progress;
}

void Tween::onCompleted(float value) {
    // Override in derived classes
    (void)value;
}

// TweenManager implementation
TweenManager::TweenManager()
    : m_maxTweens(1000)
    , m_activeTweens(0)
{
    m_tweens.reserve(m_maxTweens);
    RF_LOG_INFO("TweenManager created");
}

TweenManager::~TweenManager() {
    clear();
    RF_LOG_INFO("TweenManager destroyed");
}

Tween* TweenManager::createTween(const std::string& name) {
    if (m_activeTweens >= m_maxTweens) {
        RF_LOG_WARNING("Maximum number of tweens reached");
        return nullptr;
    }
    
    auto tween = RF_NEW(Tween);
    tween->setName(name);
    
    m_tweens.push_back(tween);
    m_tweenMap[name] = tween;
    m_activeTweens++;
    
    RF_LOG_INFO("Created tween: %s", name.c_str());
    return tween;
}

void TweenManager::destroyTween(Tween* tween) {
    if (!tween) {
        return;
    }
    
    auto it = std::find(m_tweens.begin(), m_tweens.end(), tween);
    if (it != m_tweens.end()) {
        m_tweens.erase(it);
    }
    
    auto mapIt = m_tweenMap.find(tween->getName());
    if (mapIt != m_tweenMap.end()) {
        m_tweenMap.erase(mapIt);
    }
    
    m_activeTweens--;
    
    RF_DELETE(tween);
    RF_LOG_INFO("Destroyed tween: %s", tween->getName().c_str());
}

void TweenManager::destroyTween(const std::string& name) {
    auto it = m_tweenMap.find(name);
    if (it != m_tweenMap.end()) {
        destroyTween(it->second);
    }
}

Tween* TweenManager::getTween(const std::string& name) {
    auto it = m_tweenMap.find(name);
    return (it != m_tweenMap.end()) ? it->second : nullptr;
}

void TweenManager::update(float deltaTime) {
    for (auto& tween : m_tweens) {
        if (tween) {
            tween->update(deltaTime);
        }
    }
}

void TweenManager::playTween(const std::string& name) {
    auto tween = getTween(name);
    if (tween) {
        tween->play();
    }
}

void TweenManager::pauseTween(const std::string& name) {
    auto tween = getTween(name);
    if (tween) {
        tween->pause();
    }
}

void TweenManager::stopTween(const std::string& name) {
    auto tween = getTween(name);
    if (tween) {
        tween->stop();
    }
}

void TweenManager::resumeTween(const std::string& name) {
    auto tween = getTween(name);
    if (tween) {
        tween->resume();
    }
}

void TweenManager::clear() {
    for (auto& tween : m_tweens) {
        if (tween) {
            RF_DELETE(tween);
        }
    }
    
    m_tweens.clear();
    m_tweenMap.clear();
    m_activeTweens = 0;
    
    RF_LOG_INFO("Cleared all tweens");
}

size_t TweenManager::getActiveTweenCount() const {
    return m_activeTweens;
}

size_t TweenManager::getTweenCount() const {
    return m_tweens.size();
}

std::vector<Tween*> TweenManager::getTweens() const {
    return m_tweens;
}

std::vector<std::string> TweenManager::getTweenNames() const {
    std::vector<std::string> names;
    names.reserve(m_tweenMap.size());
    
    for (const auto& pair : m_tweenMap) {
        names.push_back(pair.first);
    }
    
    return names;
}

void TweenManager::setMaxTweens(size_t maxTweens) {
    m_maxTweens = maxTweens;
    m_tweens.reserve(maxTweens);
    RF_LOG_INFO("Max tweens set to %zu", maxTweens);
}

} // namespace Animation
} // namespace RedFiles