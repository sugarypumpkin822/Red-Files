#include "editor.h"
#include "ui_components.h"
#include <iostream>
#include <algorithm>

namespace BloodEditor {
namespace UI {

Window::Window(const std::string& title, const glm::vec2& position, const glm::vec2& size)
    : title_(title), position_(position), size_(size), visible_(true) {
}

void Window::update(float deltaTime) {
    // Base implementation does nothing
}

Button::Button(const std::string& label, const glm::vec2& position, const glm::vec2& size)
    : label_(label), position_(position), size_(size), hovered_(false) {
}

void Button::render() {
    // Simple OpenGL rendering for button
    glColor4f(hovered_ ? 0.3f : 0.2f, hovered_ ? 0.3f : 0.2f, hovered_ ? 0.4f : 0.3f, 0.8f);
    
    glBegin(GL_QUADS);
    glVertex2f(position_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y + size_.y);
    glVertex2f(position_.x, position_.y + size_.y);
    glEnd();
    
    // Render border
    glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(position_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y + size_.y);
    glVertex2f(position_.x, position_.y + size_.y);
    glEnd();
}

bool Button::handleClick(const glm::vec2& mousePos) {
    if (isHovered(mousePos)) {
        if (callback_) {
            callback_();
        }
        return true;
    }
    return false;
}

bool Button::isHovered(const glm::vec2& mousePos) const {
    return mousePos.x >= position_.x && mousePos.x <= position_.x + size_.x &&
           mousePos.y >= position_.y && mousePos.y <= position_.y + size_.y;
}

Slider::Slider(const std::string& label, const glm::vec2& position, const glm::vec2& size,
               float minValue, float maxValue, float defaultValue)
    : label_(label), position_(position), size_(size), 
      minValue_(minValue), maxValue_(maxValue), value_(defaultValue), dragging_(false) {
}

void Slider::render() {
    // Render track
    glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(position_.x, position_.y + size_.y * 0.4f);
    glVertex2f(position_.x + size_.x, position_.y + size_.y * 0.4f);
    glVertex2f(position_.x + size_.x, position_.y + size_.y * 0.6f);
    glVertex2f(position_.x, position_.y + size_.y * 0.6f);
    glEnd();
    
    // Render handle
    float handlePos = position_.x + (value_ - minValue_) / (maxValue_ - minValue_) * size_.x;
    glColor4f(0.4f, 0.4f, 0.5f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(handlePos - 5, position_.y);
    glVertex2f(handlePos + 5, position_.y);
    glVertex2f(handlePos + 5, position_.y + size_.y);
    glVertex2f(handlePos - 5, position_.y + size_.y);
    glEnd();
    
    // Render label (simplified - would use text rendering in real implementation)
    std::cout << label_ << ": " << value_ << std::endl;
}

bool Slider::handleInput(const glm::vec2& mousePos, bool mouseDown) {
    glm::vec2 handlePos = glm::vec2(
        position_.x + (value_ - minValue_) / (maxValue_ - minValue_) * size_.x,
        position_.y + size_.y * 0.5f
    );
    
    bool hoveringHandle = glm::length(mousePos - handlePos) < 10.0f;
    
    if (mouseDown && hoveringHandle) {
        dragging_ = true;
    }
    
    if (dragging_) {
        float normalizedPos = (mousePos.x - position_.x) / size_.x;
        normalizedPos = glm::clamp(normalizedPos, 0.0f, 1.0f);
        value_ = minValue_ + normalizedPos * (maxValue_ - minValue_);
        
        if (callback_) {
            callback_(value_);
        }
    }
    
    if (!mouseDown) {
        dragging_ = false;
    }
    
    return dragging_;
}

ColorPicker::ColorPicker(const std::string& label, const glm::vec2& position, const glm::vec2& size)
    : label_(label), position_(position), size_(size), 
      color_(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)), picking_(false) {
}

void ColorPicker::render() {
    // Render color preview
    glColor4f(color_.r, color_.g, color_.b, color_.a);
    glBegin(GL_QUADS);
    glVertex2f(position_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y + size_.y);
    glVertex2f(position_.x, position_.y + size_.y);
    glEnd();
    
    // Render border
    glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(position_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y + size_.y);
    glVertex2f(position_.x, position_.y + size_.y);
    glEnd();
    
    std::cout << label_ << ": RGB(" << color_.r << ", " << color_.g << ", " << color_.b << ")" << std::endl;
}

bool ColorPicker::handleInput(const glm::vec2& mousePos, bool mouseDown) {
    bool hovering = mousePos.x >= position_.x && mousePos.x <= position_.x + size_.x &&
                   mousePos.y >= position_.y && mousePos.y <= position_.y + size_.y;
    
    if (mouseDown && hovering) {
        picking_ = true;
        // Simple color cycling for demonstration
        static float hue = 0.0f;
        hue += 0.1f;
        if (hue > 1.0f) hue = 0.0f;
        
        // Convert HSV to RGB (simplified)
        color_.r = std::abs(std::sin(hue * 6.28318f));
        color_.g = std::abs(std::sin((hue + 0.333f) * 6.28318f));
        color_.b = std::abs(std::sin((hue + 0.666f) * 6.28318f));
        
        if (callback_) {
            callback_(color_);
        }
    }
    
    if (!mouseDown) {
        picking_ = false;
    }
    
    return picking_;
}

PropertyPanel::PropertyPanel(const glm::vec2& position, const glm::vec2& size)
    : Window("Properties", position, size) {
}

void PropertyPanel::render() {
    if (!visible_) return;
    
    // Render panel background
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
    
    // Render properties (simplified)
    float yOffset = position_.y + 30;
    for (size_t i = 0; i < properties_.size(); ++i) {
        const auto& prop = properties_[i];
        
        // Render property name
        glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
        std::cout << prop.name << std::endl;
        
        yOffset += 25;
        if (yOffset > position_.y + size_.y - 30) break;
    }
}

void PropertyPanel::addFloatProperty(const std::string& name, float value, float minVal, float maxVal) {
    Property prop;
    prop.type = Property::FLOAT;
    prop.name = name;
    prop.floatValue = value;
    prop.minVal = minVal;
    prop.maxVal = maxVal;
    properties_.push_back(prop);
}

void PropertyPanel::addColorProperty(const std::string& name, const glm::vec4& color) {
    Property prop;
    prop.type = Property::COLOR;
    prop.name = name;
    prop.colorValue = color;
    properties_.push_back(prop);
}

void PropertyPanel::addBoolProperty(const std::string& name, bool value) {
    Property prop;
    prop.type = Property::BOOL;
    prop.name = name;
    prop.boolValue = value;
    properties_.push_back(prop);
}

Timeline::Timeline(const glm::vec2& position, const glm::vec2& size)
    : Window("Timeline", position, size), 
      currentTime_(0.0f), duration_(10.0f), playing_(false), scrubbing_(false) {
}

void Timeline::render() {
    if (!visible_) return;
    
    // Render background
    glColor4f(0.1f, 0.1f, 0.1f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(position_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y + size_.y);
    glVertex2f(position_.x, position_.y + size_.y);
    glEnd();
    
    renderTimeline();
    renderKeyframes();
    renderPlayhead();
}

void Timeline::update(float deltaTime) {
    if (playing_) {
        currentTime_ += deltaTime;
        if (currentTime_ >= duration_) {
            currentTime_ = 0.0f;
        }
    }
}

bool Timeline::handleInput(const glm::vec2& mousePos, bool mouseDown) {
    if (mousePos.x >= position_.x && mousePos.x <= position_.x + size_.x &&
        mousePos.y >= position_.y && mousePos.y <= position_.y + size_.y) {
        
        if (mouseDown) {
            // Scrub timeline
            float normalizedPos = (mousePos.x - position_.x) / size_.x;
            currentTime_ = normalizedPos * duration_;
            scrubbing_ = true;
        }
    }
    
    if (!mouseDown) {
        scrubbing_ = false;
    }
    
    return scrubbing_;
}

void Timeline::renderTimeline() {
    // Render timeline track
    glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(position_.x + 10, position_.y + size_.y * 0.4f);
    glVertex2f(position_.x + size_.x - 10, position_.y + size_.y * 0.4f);
    glVertex2f(position_.x + size_.x - 10, position_.y + size_.y * 0.6f);
    glVertex2f(position_.x + 10, position_.y + size_.y * 0.6f);
    glEnd();
}

void Timeline::renderKeyframes() {
    // Render some example keyframes
    glColor4f(0.8f, 0.6f, 0.2f, 1.0f);
    for (int i = 0; i < 5; ++i) {
        float keyframePos = position_.x + 10 + (i / 4.0f) * (size_.x - 20);
        glBegin(GL_TRIANGLES);
        glVertex2f(keyframePos, position_.y + size_.y * 0.3f);
        glVertex2f(keyframePos - 5, position_.y + size_.y * 0.4f);
        glVertex2f(keyframePos + 5, position_.y + size_.y * 0.4f);
        glEnd();
    }
}

void Timeline::renderPlayhead() {
    float playheadPos = position_.x + 10 + (currentTime_ / duration_) * (size_.x - 20);
    
    glColor4f(1.0f, 0.2f, 0.2f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(playheadPos, position_.y + 10);
    glVertex2f(playheadPos, position_.y + size_.y - 10);
    glEnd();
    
    // Render playhead handle
    glBegin(GL_TRIANGLES);
    glVertex2f(playheadPos, position_.y + 5);
    glVertex2f(playheadPos - 8, position_.y + 15);
    glVertex2f(playheadPos + 8, position_.y + 15);
    glEnd();
}

} // namespace UI
} // namespace BloodEditor