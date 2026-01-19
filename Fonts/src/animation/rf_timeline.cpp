#include "rf_timeline.h"
#include "rf_animation.h"
#include "rf_sequence.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Animation {

Timeline::Timeline()
    : m_duration(0.0f)
    , m_currentTime(0.0f)
    , m_isPlaying(false)
    , m_isLooping(false)
    , m_playbackSpeed(1.0f)
    , m_timeScale(1.0f)
    , m_snapToFrame(true)
    , m_frameRate(60.0f)
    , m_zoomLevel(1.0f)
    , m_viewOffset(0.0f, 0.0f)
    , m_viewRange(0.0f, 10.0f)
{
    RF_LOG_INFO("Timeline created");
}

Timeline::~Timeline() {
    clear();
    RF_LOG_INFO("Timeline destroyed");
}

void Timeline::addSequence(Sequence* sequence) {
    if (sequence) {
        m_sequences.push_back(sequence);
        m_sequenceMap[sequence->getName()] = sequence;
        
        updateDuration();
        
        RF_LOG_INFO("Added sequence to timeline: %s", sequence->getName().c_str());
    }
}

void Timeline::removeSequence(const std::string& name) {
    auto it = m_sequenceMap.find(name);
    if (it != m_sequenceMap.end()) {
        auto sequence = it->second;
        
        auto seqIt = std::find(m_sequences.begin(), m_sequences.end(), sequence);
        if (seqIt != m_sequences.end()) {
            m_sequences.erase(seqIt);
        }
        
        m_sequenceMap.erase(it);
        
        updateDuration();
        
        RF_LOG_INFO("Removed sequence from timeline: %s", name.c_str());
    }
}

void Timeline::removeSequence(Sequence* sequence) {
    if (!sequence) {
        return;
    }
    
    auto it = m_sequenceMap.find(sequence->getName());
    if (it != m_sequenceMap.end()) {
        m_sequences.erase(std::find(m_sequences.begin(), m_sequences.end(), sequence));
        m_sequenceMap.erase(it);
        
        updateDuration();
        
        RF_LOG_INFO("Removed sequence from timeline: %s", sequence->getName().c_str());
    }
}

void Timeline::clear() {
    m_sequences.clear();
    m_sequenceMap.clear();
    m_duration = 0.0f;
    m_currentTime = 0.0f;
    m_isPlaying = false;
    
    RF_LOG_INFO("Cleared all sequences from timeline");
}

Sequence* Timeline::getSequence(const std::string& name) {
    auto it = m_sequenceMap.find(name);
    return (it != m_sequenceMap.end()) ? it->second : nullptr;
}

std::vector<Sequence*> Timeline::getSequences() const {
    return m_sequences;
}

std::vector<std::string> Timeline::getSequenceNames() const {
    std::vector<std::string> names;
    names.reserve(m_sequenceMap.size());
    
    for (const auto& pair : m_sequenceMap) {
        names.push_back(pair.first);
    }
    
    return names;
}

void Timeline::play() {
    if (!m_isPlaying) {
        m_isPlaying = true;
        
        if (m_currentTime >= m_duration) {
            m_currentTime = 0.0f;
        }
        
        // Play all sequences
        for (auto& sequence : m_sequences) {
            if (sequence) {
                sequence->play();
            }
        }
        
        RF_LOG_INFO("Timeline started playing");
    }
}

void Timeline::pause() {
    if (m_isPlaying && !m_isPaused) {
        m_isPaused = true;
        
        // Pause all sequences
        for (auto& sequence : m_sequences) {
            if (sequence) {
                sequence->pause();
            }
        }
        
        RF_LOG_INFO("Timeline paused");
    }
}

void Timeline::stop() {
    m_isPlaying = false;
    m_isPaused = false;
    m_currentTime = 0.0f;
    
    // Stop all sequences
    for (auto& sequence : m_sequences) {
        if (sequence) {
            sequence->stop();
        }
    }
    
    RF_LOG_INFO("Timeline stopped");
}

void Timeline::resume() {
    if (m_isPlaying && m_isPaused) {
        m_isPaused = false;
        
        // Resume all sequences
        for (auto& sequence : m_sequences) {
            if (sequence) {
                sequence->resume();
            }
        }
        
        RF_LOG_INFO("Timeline resumed");
    }
}

void Timeline::reset() {
    m_currentTime = 0.0f;
    m_isPlaying = false;
    m_isPaused = false;
    
    // Reset all sequences
    for (auto& sequence : m_sequences) {
        if (sequence) {
            sequence->reset();
        }
    }
    
    RF_LOG_INFO("Timeline reset");
}

void Timeline::update(float deltaTime) {
    if (!m_isPlaying || m_isPaused) {
        return;
    }
    
    // Update timeline time
    m_currentTime += deltaTime * m_playbackSpeed * m_timeScale;
    
    // Handle looping
    if (m_isLooping) {
        if (m_currentTime >= m_duration) {
            m_currentTime = m_currentTime - m_duration;
            onTimelineLooped();
        } else if (m_currentTime < 0.0f) {
            m_currentTime = m_duration + m_currentTime;
            onTimelineLooped();
        }
    } else {
        // Handle non-looping boundaries
        if (m_currentTime >= m_duration) {
            m_currentTime = m_duration;
            stop();
            onTimelineCompleted();
        } else if (m_currentTime < 0.0f) {
            m_currentTime = 0.0f;
            stop();
            onTimelineCompleted();
        }
    }
    
    // Update all sequences
    for (auto& sequence : m_sequences) {
        if (sequence) {
            sequence->update(deltaTime * m_timeScale);
        }
    }
    
    // Notify listeners
    for (auto& listener : m_listeners) {
        if (listener) {
            listener->onTimelineUpdated(this, m_currentTime);
        }
    }
}

void Timeline::setTime(float time) {
    m_currentTime = Math::clamp(time, 0.0f, m_duration);
    
    // Update all sequences
    for (auto& sequence : m_sequences) {
        if (sequence) {
            sequence->setTime(m_currentTime);
        }
    }
    
    // Notify listeners
    for (auto& listener : m_listeners) {
        if (listener) {
            listener->onTimelineUpdated(this, m_currentTime);
        }
    }
}

void Timeline::setPlaybackSpeed(float speed) {
    m_playbackSpeed = speed;
    RF_LOG_INFO("Timeline playback speed set to %.2f", speed);
}

void Timeline::setTimeScale(float scale) {
    m_timeScale = scale;
    RF_LOG_INFO("Timeline time scale set to %.2f", scale);
}

void Timeline::setLooping(bool loop) {
    m_isLooping = loop;
    RF_LOG_INFO("Timeline looping set to %s", loop ? "true" : "false");
}

void Timeline::setSnapToFrame(bool snap) {
    m_snapToFrame = snap;
    RF_LOG_INFO("Timeline snap to frame set to %s", snap ? "true" : "false");
}

void Timeline::setFrameRate(float frameRate) {
    m_frameRate = frameRate;
    RF_LOG_INFO("Timeline frame rate set to %.2f", frameRate);
}

void Timeline::setZoomLevel(float zoom) {
    m_zoomLevel = Math::clamp(zoom, 0.1f, 10.0f);
    RF_LOG_INFO("Timeline zoom level set to %.2f", m_zoomLevel);
}

void Timeline::setViewOffset(const Vec2& offset) {
    m_viewOffset = offset;
}

void Timeline::setViewRange(float start, float end) {
    m_viewRange.x = start;
    m_viewRange.y = end;
}

float Timeline::getDuration() const {
    return m_duration;
}

float Timeline::getCurrentTime() const {
    return m_currentTime;
}

float Timeline::getProgress() const {
    if (m_duration <= 0.0f) {
        return 0.0f;
    }
    return m_currentTime / m_duration;
}

float Timeline::getNormalizedTime() const {
    return getProgress();
}

bool Timeline::isPlaying() const {
    return m_isPlaying && !m_isPaused;
}

bool Timeline::isPaused() const {
    return m_isPaused;
}

bool Timeline::isCompleted() const {
    return !m_isPlaying && !m_isLooping && 
           m_currentTime >= m_duration;
}

float Timeline::getPlaybackSpeed() const {
    return m_playbackSpeed;
}

float Timeline::getTimeScale() const {
    return m_timeScale;
}

bool Timeline::isSnappingToFrame() const {
    return m_snapToFrame;
}

float Timeline::getFrameRate() const {
    return m_frameRate;
}

float Timeline::getZoomLevel() const {
    return m_zoomLevel;
}

Vec2 Timeline::getViewOffset() const {
    return m_viewOffset;
}

Vec2 Timeline::getViewRange() const {
    return m_viewRange;
}

float Timeline::timeToFrame(float time) const {
    if (m_frameRate <= 0.0f) {
        return 0.0f;
    }
    return time * m_frameRate;
}

float Timeline::frameToTime(float frame) const {
    if (m_frameRate <= 0.0f) {
        return 0.0f;
    }
    return frame / m_frameRate;
}

int Timeline::getCurrentFrame() const {
    return static_cast<int>(timeToFrame(m_currentTime));
}

void Timeline::setCurrentFrame(int frame) {
    setTime(frameToTime(static_cast<float>(frame)));
}

void Timeline::addMarker(const std::string& name, float time) {
    m_markers[name] = time;
    RF_LOG_INFO("Added marker '%s' at time %.2f", name.c_str(), time);
}

void Timeline::removeMarker(const std::string& name) {
    auto it = m_markers.find(name);
    if (it != m_markers.end()) {
        m_markers.erase(it);
        RF_LOG_INFO("Removed marker '%s'", name.c_str());
    }
}

void Timeline::clearMarkers() {
    m_markers.clear();
    RF_LOG_INFO("Cleared all timeline markers");
}

float Timeline::getMarkerTime(const std::string& name) const {
    auto it = m_markers.find(name);
    return (it != m_markers.end()) ? it->second : 0.0f;
}

std::vector<std::string> Timeline::getMarkerNames() const {
    std::vector<std::string> names;
    names.reserve(m_markers.size());
    
    for (const auto& pair : m_markers) {
        names.push_back(pair.first);
    }
    
    return names;
}

size_t Timeline::getSequenceCount() const {
    return m_sequences.size();
}

void Timeline::updateDuration() {
    m_duration = 0.0f;
    
    for (const auto& sequence : m_sequences) {
        if (sequence) {
            float seqDuration = sequence->getDuration();
            m_duration = Math::max(m_duration, seqDuration);
        }
    }
}

void Timeline::onTimelineCompleted() {
    RF_LOG_INFO("Timeline completed");
    
    // Notify listeners
    for (auto& listener : m_listeners) {
        if (listener) {
            listener->onTimelineCompleted(this);
        }
    }
}

void Timeline::onTimelineLooped() {
    RF_LOG_INFO("Timeline looped");
    
    // Notify listeners
    for (auto& listener : m_listeners) {
        if (listener) {
            listener->onTimelineLooped(this);
        }
    }
}

void Timeline::addListener(TimelineListener* listener) {
    if (listener) {
        m_listeners.push_back(listener);
        RF_LOG_INFO("Added timeline listener");
    }
}

void Timeline::removeListener(TimelineListener* listener) {
    auto it = std::find(m_listeners.begin(), m_listeners.end(), listener);
    if (it != m_listeners.end()) {
        m_listeners.erase(it);
        RF_LOG_INFO("Removed timeline listener");
    }
}

void Timeline::clearListeners() {
    m_listeners.clear();
    RF_LOG_INFO("Cleared all timeline listeners");
}

} // namespace Animation
} // namespace RedFiles