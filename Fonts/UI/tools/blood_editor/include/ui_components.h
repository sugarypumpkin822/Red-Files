#pragma once

#include <string>
#include <functional>
#include <vector>
#include <glm/glm.hpp>

namespace BloodEditor {
namespace UI {

class Window {
public:
    Window(const std::string& title, const glm::vec2& position, const glm::vec2& size);
    virtual ~Window() = default;
    
    virtual void render() = 0;
    virtual void update(float deltaTime);
    
    const std::string& getTitle() const { return title_; }
    const glm::vec2& getPosition() const { return position_; }
    const glm::vec2& getSize() const { return size_; }
    
    void setPosition(const glm::vec2& pos) { position_ = pos; }
    void setSize(const glm::vec2& size) { size_ = size; }
    bool isVisible() const { return visible_; }
    void setVisible(bool visible) { visible_ = visible; }
    
protected:
    std::string title_;
    glm::vec2 position_;
    glm::vec2 size_;
    bool visible_;
};

class Button {
public:
    Button(const std::string& label, const glm::vec2& position, const glm::vec2& size);
    
    void render();
    bool handleClick(const glm::vec2& mousePos);
    
    void setLabel(const std::string& label) { label_ = label; }
    void setCallback(std::function<void()> callback) { callback_ = callback; }
    
    bool isHovered(const glm::vec2& mousePos) const;
    
private:
    std::string label_;
    glm::vec2 position_;
    glm::vec2 size_;
    std::function<void()> callback_;
    bool hovered_;
};

class Slider {
public:
    Slider(const std::string& label, const glm::vec2& position, const glm::vec2& size, 
           float minValue, float maxValue, float defaultValue);
    
    void render();
    bool handleInput(const glm::vec2& mousePos, bool mouseDown);
    
    float getValue() const { return value_; }
    void setValue(float value) { value_ = glm::clamp(value, minValue_, maxValue_); }
    
    void setCallback(std::function<void(float)> callback) { callback_ = callback; }
    
private:
    std::string label_;
    glm::vec2 position_;
    glm::vec2 size_;
    float minValue_;
    float maxValue_;
    float value_;
    std::function<void(float)> callback_;
    bool dragging_;
};

class ColorPicker {
public:
    ColorPicker(const std::string& label, const glm::vec2& position, const glm::vec2& size);
    
    void render();
    bool handleInput(const glm::vec2& mousePos, bool mouseDown);
    
    glm::vec4 getColor() const { return color_; }
    void setColor(const glm::vec4& color) { color_ = color; }
    
    void setCallback(std::function<void(const glm::vec4&)> callback) { callback_ = callback; }
    
private:
    std::string label_;
    glm::vec2 position_;
    glm::vec2 size_;
    glm::vec4 color_;
    std::function<void(const glm::vec4&)> callback_;
    bool picking_;
};

class PropertyPanel : public Window {
public:
    PropertyPanel(const glm::vec2& position, const glm::vec2& size);
    
    void render() override;
    
    void addFloatProperty(const std::string& name, float value, float minVal, float maxVal);
    void addColorProperty(const std::string& name, const glm::vec4& color);
    void addBoolProperty(const std::string& name, bool value);
    
private:
    struct Property {
        enum Type { FLOAT, COLOR, BOOL } type;
        std::string name;
        union {
            float floatValue;
            glm::vec4 colorValue;
            bool boolValue;
        };
        float minVal, maxVal;
    };
    
    std::vector<Property> properties_;
};

class Timeline : public Window {
public:
    Timeline(const glm::vec2& position, const glm::vec2& size);
    
    void render() override;
    void update(float deltaTime) override;
    
    bool handleInput(const glm::vec2& mousePos, bool mouseDown);
    
    float getCurrentTime() const { return currentTime_; }
    void setCurrentTime(float time) { currentTime_ = time; }
    
    float getDuration() const { return duration_; }
    void setDuration(float duration) { duration_ = duration; }
    
    bool isPlaying() const { return playing_; }
    void setPlaying(bool playing) { playing_ = playing; }
    
private:
    float currentTime_;
    float duration_;
    bool playing_;
    bool scrubbing_;
    
    void renderTimeline();
    void renderKeyframes();
    void renderPlayhead();
};

} // namespace UI
} // namespace BloodEditor