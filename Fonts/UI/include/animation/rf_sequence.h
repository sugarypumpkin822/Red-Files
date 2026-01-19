#pragma once

#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace RFAnimation {

// Sequence types
enum class SequenceType {
    LINEAR,
    BEZIER,
    CATMULL_ROM,
    B_SPLINE,
    HERMITE,
    STEP,
    CONSTANT
};

// Playback modes
enum class PlaybackMode {
    FORWARD,
    REVERSE,
    PING_PONG,
    RANDOM
};

// Loop modes
enum class LoopMode {
    NONE,
    REPEAT,
    BOUNCE,
    MIRROR
};

// Sequence element
struct SequenceElement {
    float time;
    float duration;
    std::map<std::string, std::any> values;
    std::map<std::string, std::any> velocities;
    std::map<std::string, std::any> accelerations;
    std::function<void()> onEnter;
    std::function<void()> onExit;
    std::function<void()> onUpdate;
    
    SequenceElement() : time(0.0f), duration(0.0f), onEnter(nullptr), onExit(nullptr), onUpdate(nullptr) {}
    SequenceElement(float tm, float dur, const std::map<std::string, std::any>& vals)
        : time(tm), duration(dur), values(vals), onEnter(nullptr), onExit(nullptr), onUpdate(nullptr) {}
};

// Animation sequence class
class Sequence {
public:
    Sequence(const std::string& name);
    virtual ~Sequence() = default;
    
    // Sequence management
    void addElement(const SequenceElement& element);
    void removeElement(float time);
    void clearElements();
    const std::vector<SequenceElement>& getElements() const;
    
    // Sequence control
    void play();
    void pause();
    void stop();
    void resume();
    void reset();
    void restart();
    
    // Playback control
    void setPlaybackMode(PlaybackMode mode);
    void setLoopMode(LoopMode mode);
    void setPlaybackSpeed(float speed);
    void setCurrentTime(float time);
    void setDuration(float duration);
    
    // State queries
    bool isPlaying() const;
    bool isPaused() const;
    bool isStopped() const;
    bool isCompleted() const;
    float getCurrentTime() const;
    float getDuration() const;
    float getPlaybackSpeed() const;
    PlaybackMode getPlaybackMode() const;
    LoopMode getLoopMode() const;
    
    // Value access
    std::map<std::string, std::any> getCurrentValues() const;
    std::map<std::string, std::any> getCurrentVelocities() const;
    std::map<std::string, std::any> getCurrentAccelerations() const;
    template<typename T>
    T getCurrentValue(const std::string& key) const;
    template<typename T>
    T getCurrentVelocity(const std::string& key) const;
    template<typename T>
    T getCurrentAcceleration(const std::string& key) const;
    
    // Event handling
    void addEventListener(const std::string& eventType, std::function<void()> callback);
    void removeEventListener(const std::string& eventType, std::function<void()> callback);
    void clearEventListeners();
    
    // Utility methods
    void cloneFrom(const Sequence& other);
    virtual std::unique_ptr<Sequence> clone() const = 0;
    
    // Data access
    const std::string& getName() const;
    const std::vector<SequenceElement>& getElements() const;
    
protected:
    // Protected members
    std::string name_;
    std::vector<SequenceElement> elements_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    float currentTime_;
    float duration_;
    float playbackSpeed_;
    PlaybackMode playbackMode_;
    LoopMode loopMode_;
    
    bool isPlaying_;
    bool isPaused_;
    bool isStopped_;
    bool isCompleted_;
    
    // Protected helper methods
    virtual void updateSequence(float deltaTime);
    virtual void processElements(float deltaTime);
    virtual void triggerEvent(const std::string& eventType);
    virtual void interpolateBetweenElements();
    virtual void updateCurrentValues();
    
    // Helper methods
    SequenceElement* findElementAt(float time) const;
    SequenceElement* findNextElement(float time) const;
    SequenceElement* findPreviousElement(float time) const;
    float calculateInterpolationFactor(float currentTime, float elementTime, float elementDuration) const;
    void updatePlaybackDirection();
};

// Specialized sequence classes
class LinearSequence : public Sequence {
public:
    LinearSequence(const std::string& name);
    void addLinearElement(float time, float duration, const std::map<std::string, std::any>& values);
    std::unique_ptr<Sequence> clone() const override;
};

class BezierSequence : public Sequence {
public:
    BezierSequence(const std::string& name);
    void addBezierElement(float time, float duration, const std::map<std::string, std::any>& values, 
                      const std::map<std::string, std::any>& controlPoints);
    std::unique_ptr<Sequence> clone() const override;
};

class CatmullRomSequence : public Sequence {
public:
    CatmullRomSequence(const std::string& name);
    void addCatmullRomElement(float time, float duration, const std::map<std::string, std::any>& values);
    std::unique_ptr<Sequence> clone() const override;
};

class BSplineSequence : public Sequence {
public:
    BSplineSequence(const std::string& name);
    void addBSplineElement(float time, float duration, const std::map<std::string, std::any>& values);
    std::unique_ptr<Sequence> clone() const override;
};

class HermiteSequence : public Sequence {
public:
    HermiteSequence(const std::string& name);
    void addHermiteElement(float time, float duration, const std::map<std::string, std::any>& values, 
                         const std::map<std::string, std::any>& tangents);
    std::unique_ptr<Sequence> clone() const override;
};

// Sequence manager
class SequenceManager {
public:
    SequenceManager();
    ~SequenceManager();
    
    // Sequence management
    void addSequence(const std::string& name, std::shared_ptr<Sequence> sequence);
    void removeSequence(const std::string& name);
    std::shared_ptr<Sequence> getSequence(const std::string& name) const;
    bool hasSequence(const std::string& name) const;
    const std::vector<std::string>& getSequenceNames() const;
    
    // Sequence control
    void playSequence(const std::string& name);
    void pauseSequence(const std::string& name);
    void stopSequence(const std::string& name);
    void resumeSequence(const std::string& name);
    void resetSequence(const std::string& name);
    
    // Global control
    void playAllSequences();
    void pauseAllSequences();
    void stopAllSequences();
    void resumeAllSequences();
    void resetAllSequences();
    
    // State queries
    bool isPlaying(const std::string& name) const;
    bool isPaused(const std::string& name) const;
    bool isStopped(const std::string& name) const;
    
    // Event handling
    void addSequenceEventListener(const std::string& sequenceName, const std::string& eventType, 
                               std::function<void()> callback);
    void removeSequenceEventListener(const std::string& sequenceName, const std::string& eventType, 
                                    std::function<void()> callback);
    void clearSequenceEventListeners();
    void clearSequenceEventListeners();
    
    // Utility methods
    void cloneFrom(const SequenceManager& other);
    std::unique_ptr<SequenceManager> clone() const;
    
    // Data access
    const std::map<std::string, std::shared_ptr<Sequence>>& getSequences() const;
    
protected:
    // Protected members
    std::map<std::string, std::shared_ptr<Sequence>> sequences_;
    std::map<std::string, std::vector<std::function<void()>>> sequenceEventListeners_;
    
    // Protected helper methods
    virtual void updateSequences(float deltaTime);
    virtual void processSequenceEvents();
    virtual void triggerSequenceEvent(const std::string& sequenceName, 
                                 const std::string& eventType);
    virtual void updateSequence(const std::shared_ptr<Sequence>& sequence, float deltaTime);
};

// Sequence factory
class SequenceFactory {
public:
    static std::unique_ptr<Sequence> createSequence(const std::string& name, SequenceType type);
    static std::unique_ptr<Sequence> createLinearSequence(const std::string& name);
    static std::unique_ptr<Sequence> createBezierSequence(const std::string& name);
    static std::unique_ptr<Sequence> createCatmullRomSequence(const std::string& name);
    static std::unique_ptr<Sequence> createBSplineSequence(const std::string& name);
    static std::unique_ptr<Sequence> createHermiteSequence(const std::string& name);
    static std::vector<SequenceType> getAvailableTypes();
};

} // namespace RFAnimation