#pragma once

#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace RFAnimation {

// Timeline types
enum class TimelineType {
    LINEAR,
    BEZIER,
    STEP,
    CONSTANT
};

// Timeline modes
enum class TimelineMode {
    EDIT,
    PLAYBACK,
    PREVIEW
};

// Marker types
enum class MarkerType {
    KEYFRAME,
    EVENT,
    BOOKMARK,
    NOTE
};

// Timeline marker
struct TimelineMarker {
    float time;
    std::string name;
    MarkerType type;
    std::map<std::string, std::any> userData;
    std::function<void()> onTrigger;
    
    TimelineMarker() : time(0.0f), type(MarkerType::KEYFRAME), onTrigger(nullptr) {}
    TimelineMarker(float tm, const std::string& nm, MarkerType tp, 
                 std::function<void()> onTrig = nullptr)
        : time(tm), name(nm), type(tp), onTrigger(onTrig) {}
};

// Timeline track
class TimelineTrack {
public:
    TimelineTrack(const std::string& name, TimelineType type);
    virtual ~TimelineTrack() = default;
    
    // Track management
    void addMarker(const TimelineMarker& marker);
    void removeMarker(float time);
    void clearMarkers();
    const std::vector<TimelineMarker>& getMarkers() const;
    
    // Track control
    void setTrackType(TimelineType type);
    void setEnabled(bool enabled);
    void setVisible(bool visible);
    void setHeight(float height);
    void setColor(const std::array<float, 4>& color);
    
    // Track queries
    const std::string& getName() const;
    TimelineType getTrackType() const;
    bool isEnabled() const;
    bool isVisible() const;
    float getHeight() const;
    const std::array<float, 4>& getColor() const;
    
    // Utility methods
    void cloneFrom(const TimelineTrack& other);
    virtual std::unique_ptr<TimelineTrack> clone() const = 0;
    
    // Data access
    const std::vector<TimelineMarker>& getMarkers() const;
    
protected:
    // Protected members
    std::string name_;
    TimelineType type_;
    std::vector<TimelineMarker> markers_;
    
    bool isEnabled_;
    bool isVisible_;
    float height_;
    std::array<float, 4> color_;
    
    // Protected helper methods
    virtual void updateTrack(float deltaTime);
    virtual void processMarkers(float currentTime);
    virtual void triggerMarkerEvents(float currentTime);
    virtual void sortMarkers();
};

// Timeline class
class Timeline {
public:
    Timeline();
    ~Timeline() = default;
    
    // Timeline management
    void addTrack(const std::string& name, TimelineType type);
    void removeTrack(const std::string& name);
    void clearTracks();
    std::shared_ptr<TimelineTrack> getTrack(const std::string& name) const;
    const std::vector<std::string>& getTrackNames() const;
    const std::map<std::string, std::shared_ptr<TimelineTrack>>& getTracks() const;
    
    // Timeline control
    void play();
    void pause();
    void stop();
    void resume();
    void reset();
    void setMode(TimelineMode mode);
    void setCurrentTime(float time);
    void setDuration(float duration);
    void setPlaybackSpeed(float speed);
    void setLooping(bool enabled);
    
    // Timeline state
    bool isPlaying() const;
    bool isPaused() const;
    bool isStopped() const;
    bool isLooping() const;
    float getCurrentTime() const;
    float getDuration() const;
    float getPlaybackSpeed() const;
    TimelineMode getMode() const;
    
    // Zoom and pan
    void setZoom(float zoom);
    void setPan(float x, float y);
    void setViewport(float x, float y, float width, float height);
    float getZoom() const;
    std::pair<float, float> getPan() const;
    std::array<float, 4> getViewport() const;
    
    // Selection
    void setSelection(float startTime, float endTime);
    void clearSelection();
    std::pair<float, float> getSelection() const;
    bool hasSelection() const;
    
    // Snapping
    void setSnapEnabled(bool enabled);
    void setSnapInterval(float interval);
    void setSnapToMarkers(bool enabled);
    bool isSnapEnabled() const;
    float getSnapInterval() const;
    bool isSnapToMarkers() const;
    
    // Event handling
    void addTimelineEventListener(const std::string& eventType, std::function<void()> callback);
    void removeTimelineEventListener(const std::string& eventType, std::function<void()> callback);
    void clearTimelineEventListeners();
    
    // Utility methods
    void cloneFrom(const Timeline& other);
    std::unique_ptr<Timeline> clone() const;
    
    // Data access
    const std::map<std::string, std::shared_ptr<TimelineTrack>>& getTracks() const;
    
protected:
    // Protected members
    std::map<std::string, std::shared_ptr<TimelineTrack>> tracks_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    float currentTime_;
    float duration_;
    float playbackSpeed_;
    TimelineMode mode_;
    
    bool isPlaying_;
    bool isPaused_;
    bool isStopped_;
    bool isLooping_;
    
    // Zoom and pan
    float zoom_;
    float panX_, panY_;
    std::array<float, 4> viewport_;
    
    // Selection
    std::pair<float, float> selection_;
    bool hasSelection_;
    
    // Snapping
    bool snapEnabled_;
    float snapInterval_;
    bool snapToMarkers_;
    
    // Protected helper methods
    virtual void updateTimeline(float deltaTime);
    virtual void processTracks(float deltaTime);
    virtual void triggerTimelineEvents();
    virtual void updateSelection();
    virtual float snapToInterval(float value) const;
    virtual float snapToMarker(float time) const;
    virtual void updateViewport();
    
    // Helper methods
    TimelineTrack* findTrackAt(float x, float y) const;
    TimelineMarker* findMarkerAt(float time, const std::string& trackName) const;
    void updateTrackHeights();
    void updateTrackColors();
};

// Specialized timeline tracks
class KeyframeTrack : public TimelineTrack {
public:
    KeyframeTrack(const std::string& name);
    void addKeyframe(float time, const std::map<std::string, std::any>& values);
    void removeKeyframe(float time);
    std::unique_ptr<TimelineTrack> clone() const override;
};

class EventTrack : public TimelineTrack {
public:
    EventTrack(const std::string& name);
    void addEvent(float time, const std::string& eventName, const std::map<std::string, std::any>& userData);
    void removeEvent(float time);
    std::unique_ptr<TimelineTrack> clone() const override;
};

class BookmarkTrack : public TimelineTrack {
public:
    BookmarkTrack(const std::string& name);
    void addBookmark(float time, const std::string& description);
    void removeBookmark(float time);
    std::unique_ptr<TimelineTrack> clone() const override;
};

class NoteTrack : public TimelineTrack {
public:
    NoteTrack(const std::string& name);
    void addNote(float time, const std::string& note, const std::map<std::string, std::any>& userData);
    void removeNote(float time);
    std::unique_ptr<TimelineTrack> clone() const override;
};

// Timeline editor
class TimelineEditor {
public:
    TimelineEditor();
    ~TimelineEditor() = default;
    
    // Editor management
    void setTimeline(std::shared_ptr<Timeline> timeline);
    std::shared_ptr<Timeline> getTimeline() const;
    
    // Editing operations
    void addTrack(const std::string& name, TimelineType type);
    void removeTrack(const std::string& name);
    void moveTrack(const std::string& name, int newIndex);
    void duplicateTrack(const std::string& name);
    
    // Marker operations
    void addMarker(const std::string& trackName, const TimelineMarker& marker);
    void removeMarker(const std::string& trackName, float time);
    void moveMarker(const std::string& trackName, float oldTime, float newTime);
    
    // Selection operations
    void selectMarker(const std::string& trackName, float time);
    void selectRange(float startTime, float endTime);
    void clearSelection();
    
    // Editing tools
    void setTool(const std::string& tool);
    void setEditMode(const std::string& mode);
    const std::string& getCurrentTool() const;
    const std::string& getCurrentEditMode() const;
    
    // Event handling
    void addEditorEventListener(const std::string& eventType, std::function<void()> callback);
    void removeEditorEventListener(const std::string& eventType, std::function<void()> callback);
    void clearEditorEventListeners();
    
    // Utility methods
    void cloneFrom(const TimelineEditor& other);
    std::unique_ptr<TimelineEditor> clone() const;
    
    // Data access
    const std::vector<std::string>& getSelectedMarkers() const;
    const std::pair<float, float>& getSelectedRange() const;
    
protected:
    // Protected members
    std::shared_ptr<Timeline> timeline_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    std::vector<std::string> selectedMarkers_;
    std::pair<float, float> selectedRange_;
    std::string currentTool_;
    std::string currentEditMode_;
    
    // Protected helper methods
    virtual void updateEditor(float deltaTime);
    virtual void processEditorEvents();
    virtual void triggerEditorEvent(const std::string& eventType);
    virtual void updateSelection();
};

// Timeline factory
class TimelineFactory {
public:
    static std::unique_ptr<Timeline> createTimeline();
    static std::unique_ptr<TimelineTrack> createTrack(const std::string& name, TimelineType type);
    static std::unique_ptr<TimelineEditor> createEditor();
    static std::vector<TimelineType> getAvailableTrackTypes();
};

} // namespace RFAnimation