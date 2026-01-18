#include "editor.h"
#include "ui_components.h"
#include <iostream>
#include <algorithm>

namespace BloodEditor {

class TimelineEditor {
public:
    struct Keyframe {
        float time;
        std::map<std::string, float> properties;
        std::string type; // "particle_spawn", "curve_start", etc.
    };
    
    struct Track {
        std::string name;
        std::vector<Keyframe> keyframes;
        bool enabled;
        glm::vec4 color;
    };
    
    TimelineEditor();
    ~TimelineEditor() = default;
    
    void update(float deltaTime);
    void render();
    bool handleInput(const glm::vec2& mousePos, bool mouseDown, bool mousePressed);
    
    void addTrack(const std::string& name);
    void removeTrack(const std::string& name);
    
    void addKeyframe(const std::string& trackName, float time, const Keyframe& keyframe);
    void removeKeyframe(const std::string& trackName, float time);
    
    void play();
    void pause();
    void stop();
    void setTime(float time);
    
    float getCurrentTime() const { return currentTime_; }
    float getDuration() const { return duration_; }
    void setDuration(float duration) { duration_ = duration; }
    
    bool isPlaying() const { return playing_; }
    
    void setTargetEffect(BloodEffect* effect) { targetEffect_ = effect; }
    
private:
    BloodEffect* targetEffect_;
    std::vector<Track> tracks_;
    float currentTime_;
    float duration_;
    bool playing_;
    bool scrubbing_;
    
    glm::vec2 position_;
    glm::vec2 size_;
    
    int selectedTrack_;
    float selectedKeyframeTime_;
    
    void renderTracks();
    void renderKeyframes();
    void renderPlayhead();
    void renderControls();
    
    int getTrackAtY(float y) const;
    float getTimeAtX(float x) const;
    Track* findTrack(const std::string& name);
    Keyframe* findKeyframe(const std::string& trackName, float time);
    
    void processKeyframes();
    void spawnParticleFromKeyframe(const Keyframe& keyframe);
    void startCurveFromKeyframe(const Keyframe& keyframe);
};

TimelineEditor::TimelineEditor() 
    : targetEffect_(nullptr), currentTime_(0.0f), duration_(10.0f), 
      playing_(false), scrubbing_(false), position_(50, 500), size_(1000, 200),
      selectedTrack_(-1), selectedKeyframeTime_(-1.0f) {
    
    // Create default tracks
    addTrack("Particle Spawn");
    addTrack("Blood Curves");
    addTrack("Effects");
}

void TimelineEditor::update(float deltaTime) {
    if (playing_) {
        currentTime_ += deltaTime;
        if (currentTime_ >= duration_) {
            currentTime_ = 0.0f;
        }
        
        processKeyframes();
    }
}

void TimelineEditor::render() {
    // Render background
    glColor4f(0.1f, 0.1f, 0.1f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(position_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y + size_.y);
    glVertex2f(position_.x, position_.y + size_.y);
    glEnd();
    
    // Render border
    glColor4f(0.3f, 0.3f, 0.3f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(position_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y + size_.y);
    glVertex2f(position_.x, position_.y + size_.y);
    glEnd();
    
    renderTracks();
    renderKeyframes();
    renderPlayhead();
    renderControls();
}

bool TimelineEditor::handleInput(const glm::vec2& mousePos, bool mouseDown, bool mousePressed) {
    // Check if mouse is within timeline area
    if (mousePos.x < position_.x || mousePos.x > position_.x + size_.x ||
        mousePos.y < position_.y || mousePos.y > position_.y + size_.y) {
        return false;
    }
    
    if (mousePressed) {
        // Check for track selection
        selectedTrack_ = getTrackAtY(mousePos.y);
        
        // Check for keyframe selection
        float time = getTimeAtX(mousePos.x);
        if (selectedTrack_ >= 0) {
            Track& track = tracks_[selectedTrack_];
            for (const auto& keyframe : track.keyframes) {
                if (std::abs(keyframe.time - time) < 0.1f) {
                    selectedKeyframeTime_ = keyframe.time;
                    return true;
                }
            }
        }
        
        // Start scrubbing
        scrubbing_ = true;
        currentTime_ = time;
    }
    
    if (mouseDown && scrubbing_) {
        currentTime_ = getTimeAtX(mousePos.x);
        currentTime_ = glm::clamp(currentTime_, 0.0f, duration_);
    }
    
    if (!mouseDown) {
        scrubbing_ = false;
    }
    
    return scrubbing_;
}

void TimelineEditor::addTrack(const std::string& name) {
    Track track;
    track.name = name;
    track.enabled = true;
    track.color = glm::vec4(
        0.2f + (rand() % 100) / 200.0f,
        0.2f + (rand() % 100) / 200.0f,
        0.2f + (rand() % 100) / 200.0f,
        1.0f
    );
    tracks_.push_back(track);
}

void TimelineEditor::removeTrack(const std::string& name) {
    tracks_.erase(
        std::remove_if(tracks_.begin(), tracks_.end(),
            [&name](const Track& t) { return t.name == name; }),
        tracks_.end()
    );
}

void TimelineEditor::addKeyframe(const std::string& trackName, float time, const Keyframe& keyframe) {
    Track* track = findTrack(trackName);
    if (track) {
        track->keyframes.push_back(keyframe);
        std::sort(track->keyframes.begin(), track->keyframes.end(),
            [](const Keyframe& a, const Keyframe& b) { return a.time < b.time; });
    }
}

void TimelineEditor::removeKeyframe(const std::string& trackName, float time) {
    Track* track = findTrack(trackName);
    if (track) {
        track->keyframes.erase(
            std::remove_if(track->keyframes.begin(), track->keyframes.end(),
                [time](const Keyframe& k) { return std::abs(k.time - time) < 0.1f; }),
            track->keyframes.end()
        );
    }
}

void TimelineEditor::play() {
    playing_ = true;
}

void TimelineEditor::pause() {
    playing_ = false;
}

void TimelineEditor::stop() {
    playing_ = false;
    currentTime_ = 0.0f;
}

void TimelineEditor::setTime(float time) {
    currentTime_ = glm::clamp(time, 0.0f, duration_);
}

void TimelineEditor::renderTracks() {
    float trackHeight = 30.0f;
    float yOffset = position_.y + 40;
    
    for (size_t i = 0; i < tracks_.size(); ++i) {
        const Track& track = tracks_[i];
        
        // Render track background
        glColor4f(track.color.r * 0.3f, track.color.g * 0.3f, track.color.b * 0.3f, 0.5f);
        glBegin(GL_QUADS);
        glVertex2f(position_.x + 100, yOffset);
        glVertex2f(position_.x + size_.x - 50, yOffset);
        glVertex2f(position_.x + size_.x - 50, yOffset + trackHeight);
        glVertex2f(position_.x + 100, yOffset + trackHeight);
        glEnd();
        
        // Highlight selected track
        if (i == selectedTrack_) {
            glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(position_.x + 100, yOffset);
            glVertex2f(position_.x + size_.x - 50, yOffset);
            glVertex2f(position_.x + size_.x - 50, yOffset + trackHeight);
            glVertex2f(position_.x + 100, yOffset + trackHeight);
            glEnd();
        }
        
        yOffset += trackHeight + 5;
    }
}

void TimelineEditor::renderKeyframes() {
    float trackHeight = 30.0f;
    float yOffset = position_.y + 40;
    
    for (const auto& track : tracks_) {
        for (const auto& keyframe : track.keyframes) {
            float xPos = position_.x + 100 + (keyframe.time / duration_) * (size_.x - 150);
            
            // Render keyframe
            glColor4f(track.color.r, track.color.g, track.color.b, 1.0f);
            glBegin(GL_TRIANGLES);
            glVertex2f(xPos, yOffset + trackHeight * 0.5f - 5);
            glVertex2f(xPos - 5, yOffset + trackHeight * 0.5f + 5);
            glVertex2f(xPos + 5, yOffset + trackHeight * 0.5f + 5);
            glEnd();
            
            // Highlight selected keyframe
            if (std::abs(keyframe.time - selectedKeyframeTime_) < 0.1f) {
                glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
                glBegin(GL_LINE_LOOP);
                glVertex2f(xPos - 8, yOffset + trackHeight * 0.5f - 8);
                glVertex2f(xPos + 8, yOffset + trackHeight * 0.5f - 8);
                glVertex2f(xPos + 8, yOffset + trackHeight * 0.5f + 8);
                glVertex2f(xPos - 8, yOffset + trackHeight * 0.5f + 8);
                glEnd();
            }
        }
        
        yOffset += trackHeight + 5;
    }
}

void TimelineEditor::renderPlayhead() {
    float xPos = position_.x + 100 + (currentTime_ / duration_) * (size_.x - 150);
    
    glColor4f(1.0f, 0.2f, 0.2f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(xPos, position_.y + 20);
    glVertex2f(xPos, position_.y + size_.y - 20);
    glEnd();
    
    // Render playhead handle
    glBegin(GL_TRIANGLES);
    glVertex2f(xPos, position_.y + 15);
    glVertex2f(xPos - 8, position_.y + 25);
    glVertex2f(xPos + 8, position_.y + 25);
    glEnd();
}

void TimelineEditor::renderControls() {
    // Render play/pause button
    float buttonX = position_.x + 10;
    float buttonY = position_.y + 10;
    float buttonSize = 25;
    
    glColor4f(0.3f, 0.3f, 0.3f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(buttonX, buttonY);
    glVertex2f(buttonX + buttonSize, buttonY);
    glVertex2f(buttonX + buttonSize, buttonY + buttonSize);
    glVertex2f(buttonX, buttonY + buttonSize);
    glEnd();
    
    // Render play/pause icon
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    if (playing_) {
        // Pause icon
        glBegin(GL_QUADS);
        glVertex2f(buttonX + 8, buttonY + 5);
        glVertex2f(buttonX + 12, buttonY + 5);
        glVertex2f(buttonX + 12, buttonY + 20);
        glVertex2f(buttonX + 8, buttonY + 20);
        
        glVertex2f(buttonX + 13, buttonY + 5);
        glVertex2f(buttonX + 17, buttonY + 5);
        glVertex2f(buttonX + 17, buttonY + 20);
        glVertex2f(buttonX + 13, buttonY + 20);
        glEnd();
    } else {
        // Play icon
        glBegin(GL_TRIANGLES);
        glVertex2f(buttonX + 8, buttonY + 5);
        glVertex2f(buttonX + 8, buttonY + 20);
        glVertex2f(buttonX + 18, buttonY + 12.5f);
        glEnd();
    }
    
    // Render time display
    std::cout << "Time: " << currentTime_ << " / " << duration_ << std::endl;
}

int TimelineEditor::getTrackAtY(float y) const {
    float trackHeight = 30.0f;
    float yOffset = position_.y + 40;
    
    for (size_t i = 0; i < tracks_.size(); ++i) {
        if (y >= yOffset && y <= yOffset + trackHeight) {
            return static_cast<int>(i);
        }
        yOffset += trackHeight + 5;
    }
    
    return -1;
}

float TimelineEditor::getTimeAtX(float x) const {
    float normalizedX = (x - position_.x - 100) / (size_.x - 150);
    return normalizedX * duration_;
}

Track* TimelineEditor::findTrack(const std::string& name) {
    auto it = std::find_if(tracks_.begin(), tracks_.end(),
        [&name](const Track& t) { return t.name == name; });
    return (it != tracks_.end()) ? &(*it) : nullptr;
}

Keyframe* TimelineEditor::findKeyframe(const std::string& trackName, float time) {
    Track* track = findTrack(trackName);
    if (!track) return nullptr;
    
    auto it = std::find_if(track->keyframes.begin(), track->keyframes.end(),
        [time](const Keyframe& k) { return std::abs(k.time - time) < 0.1f; });
    return (it != track->keyframes.end()) ? &(*it) : nullptr;
}

void TimelineEditor::processKeyframes() {
    if (!targetEffect_) return;
    
    for (const auto& track : tracks_) {
        if (!track.enabled) continue;
        
        for (const auto& keyframe : track.keyframes) {
            if (std::abs(keyframe.time - currentTime_) < 0.016f) { // Within one frame
                if (keyframe.type == "particle_spawn") {
                    spawnParticleFromKeyframe(keyframe);
                } else if (keyframe.type == "curve_start") {
                    startCurveFromKeyframe(keyframe);
                }
            }
        }
    }
}

void TimelineEditor::spawnParticleFromKeyframe(const Keyframe& keyframe) {
    if (!targetEffect_) return;
    
    BloodParticle particle;
    particle.position = glm::vec2(
        keyframe.properties.count("x") ? keyframe.properties.at("x") : 0.0f,
        keyframe.properties.count("y") ? keyframe.properties.at("y") : 0.0f
    );
    particle.velocity = glm::vec2(
        keyframe.properties.count("vx") ? keyframe.properties.at("vx") : 0.0f,
        keyframe.properties.count("vy") ? keyframe.properties.at("vy") : 0.0f
    );
    particle.color = glm::vec4(
        keyframe.properties.count("r") ? keyframe.properties.at("r") : 0.8f,
        keyframe.properties.count("g") ? keyframe.properties.at("g") : 0.1f,
        keyframe.properties.count("b") ? keyframe.properties.at("b") : 0.1f,
        keyframe.properties.count("a") ? keyframe.properties.at("a") : 1.0f
    );
    particle.size = keyframe.properties.count("size") ? keyframe.properties.at("size") : 1.0f;
    particle.lifetime = keyframe.properties.count("lifetime") ? keyframe.properties.at("lifetime") : 3.0f;
    particle.age = 0.0f;
    particle.active = true;
    
    targetEffect_->addParticle(particle);
}

void TimelineEditor::startCurveFromKeyframe(const Keyframe& keyframe) {
    if (!targetEffect_) return;
    
    BloodCurve curve;
    curve.name = "Timeline Curve";
    curve.thickness = keyframe.properties.count("thickness") ? keyframe.properties.at("thickness") : 2.0f;
    curve.color = glm::vec4(
        keyframe.properties.count("r") ? keyframe.properties.at("r") : 0.8f,
        keyframe.properties.count("g") ? keyframe.properties.at("g") : 0.1f,
        keyframe.properties.count("b") ? keyframe.properties.at("b") : 0.1f,
        keyframe.properties.count("a") ? keyframe.properties.at("a") : 1.0f
    );
    
    // Add some default points for the curve
    for (int i = 0; i < 5; ++i) {
        curve.points.push_back(glm::vec2(
            keyframe.properties.count("x") ? keyframe.properties.at("x") + i * 20.0f : i * 20.0f,
            keyframe.properties.count("y") ? keyframe.properties.at("y") : 0.0f
        ));
    }
    
    targetEffect_->addCurve(curve);
}

} // namespace BloodEditor