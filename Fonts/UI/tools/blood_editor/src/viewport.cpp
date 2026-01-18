#include "editor.h"
#include "ui_components.h"
#include <iostream>
#include <algorithm>

namespace BloodEditor {

class Viewport {
public:
    Viewport(const glm::vec2& position, const glm::vec2& size);
    ~Viewport() = default;
    
    void update(float deltaTime);
    void render();
    bool handleInput(const glm::vec2& mousePos, bool mouseDown, bool mousePressed);
    
    void setTargetEffect(BloodEffect* effect) { targetEffect_ = effect; }
    
    void setZoom(float zoom) { zoomLevel_ = glm::clamp(zoom, 0.1f, 10.0f); }
    float getZoom() const { return zoomLevel_; }
    
    void setOffset(const glm::vec2& offset) { viewportOffset_ = offset; }
    const glm::vec2& getOffset() const { return viewportOffset_; }
    
    void centerOn(const glm::vec2& worldPos);
    void fitToContent();
    
    void setGridEnabled(bool enabled) { gridEnabled_ = enabled; }
    bool isGridEnabled() const { return gridEnabled_; }
    
    void setGridSize(float size) { gridSize_ = size; }
    float getGridSize() const { return gridSize_; }
    
    glm::vec2 screenToWorld(const glm::vec2& screenPos) const;
    glm::vec2 worldToScreen(const glm::vec2& worldPos) const;
    
private:
    BloodEffect* targetEffect_;
    glm::vec2 position_;
    glm::vec2 size_;
    glm::vec2 viewportOffset_;
    float zoomLevel_;
    
    bool gridEnabled_;
    float gridSize_;
    
    bool panning_;
    glm::vec2 lastMousePos_;
    
    void renderGrid();
    void renderBackground();
    void renderContent();
    void renderOverlay();
    
    bool isMouseOver(const glm::vec2& mousePos) const;
};

Viewport::Viewport(const glm::vec2& position, const glm::vec2& size)
    : targetEffect_(nullptr), position_(position), size_(size),
      viewportOffset_(0.0f), zoomLevel_(1.0f), gridEnabled_(true), 
      gridSize_(20.0f), panning_(false) {
}

void Viewport::update(float deltaTime) {
    // Update viewport if needed
}

void Viewport::render() {
    renderBackground();
    
    if (gridEnabled_) {
        renderGrid();
    }
    
    renderContent();
    renderOverlay();
}

bool Viewport::handleInput(const glm::vec2& mousePos, bool mouseDown, bool mousePressed) {
    if (!isMouseOver(mousePos)) {
        return false;
    }
    
    if (mousePressed) {
        panning_ = true;
        lastMousePos_ = mousePos;
        return true;
    }
    
    if (mouseDown && panning_) {
        glm::vec2 delta = mousePos - lastMousePos_;
        viewportOffset_ += delta;
        lastMousePos_ = mousePos;
        return true;
    }
    
    if (!mouseDown) {
        panning_ = false;
    }
    
    return false;
}

void Viewport::centerOn(const glm::vec2& worldPos) {
    glm::vec2 screenPos = worldToScreen(worldPos);
    glm::vec2 center = position_ + size_ * 0.5f;
    viewportOffset_ += center - screenPos;
}

void Viewport::fitToContent() {
    if (!targetEffect_) return;
    
    // Calculate bounding box of all content
    glm::vec2 minBounds(FLT_MAX, FLT_MAX);
    glm::vec2 maxBounds(-FLT_MAX, -FLT_MAX);
    
    const auto& particles = targetEffect_->getParticles();
    for (const auto& particle : particles) {
        if (!particle.active) continue;
        
        minBounds = glm::min(minBounds, particle.position - glm::vec2(particle.size));
        maxBounds = glm::max(maxBounds, particle.position + glm::vec2(particle.size));
    }
    
    const auto& curves = targetEffect_->getCurves();
    for (const auto& curve : curves) {
        for (const auto& point : curve.points) {
            minBounds = glm::min(minBounds, point);
            maxBounds = glm::max(maxBounds, point);
        }
    }
    
    if (minBounds.x <= maxBounds.x && minBounds.y <= maxBounds.y) {
        glm::vec2 contentSize = maxBounds - minBounds;
        glm::vec2 contentCenter = (minBounds + maxBounds) * 0.5f;
        
        // Calculate zoom to fit content
        float scaleX = size_.x / contentSize.x;
        float scaleY = size_.y / contentSize.y;
        zoomLevel_ = std::min(scaleX, scaleY) * 0.8f; // 80% to leave some margin
        
        centerOn(contentCenter);
    }
}

glm::vec2 Viewport::screenToWorld(const glm::vec2& screenPos) const {
    glm::vec2 localPos = screenPos - position_;
    glm::vec2 centerOffset = localPos - size_ * 0.5f;
    return (centerOffset / zoomLevel_) - viewportOffset_;
}

glm::vec2 Viewport::worldToScreen(const glm::vec2& worldPos) const {
    glm::vec2 offsetPos = (worldPos + viewportOffset_) * zoomLevel_;
    glm::vec2 centerPos = offsetPos + size_ * 0.5f;
    return position_ + centerPos;
}

void Viewport::renderGrid() {
    glm::vec2 worldTopLeft = screenToWorld(position_);
    glm::vec2 worldBottomRight = screenToWorld(position_ + size_);
    
    // Calculate grid start and end positions
    float gridStartX = std::floor(worldTopLeft.x / gridSize_) * gridSize_;
    float gridEndX = std::ceil(worldBottomRight.x / gridSize_) * gridSize_;
    float gridStartY = std::floor(worldTopLeft.y / gridSize_) * gridSize_;
    float gridEndY = std::ceil(worldBottomRight.y / gridSize_) * gridSize_;
    
    glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
    glBegin(GL_LINES);
    
    // Vertical lines
    for (float x = gridStartX; x <= gridEndX; x += gridSize_) {
        glm::vec2 screenPos = worldToScreen(glm::vec2(x, 0));
        glVertex2f(screenPos.x, position_.y);
        glVertex2f(screenPos.x, position_.y + size_.y);
    }
    
    // Horizontal lines
    for (float y = gridStartY; y <= gridEndY; y += gridSize_) {
        glm::vec2 screenPos = worldToScreen(glm::vec2(0, y));
        glVertex2f(position_.x, screenPos.y);
        glVertex2f(position_.x + size_.x, screenPos.y);
    }
    
    glEnd();
    
    // Render origin lines (thicker, different color)
    glColor4f(0.3f, 0.3f, 0.2f, 0.8f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    
    // X-axis
    glm::vec2 xAxisStart = worldToScreen(glm::vec2(worldTopLeft.x, 0));
    glm::vec2 xAxisEnd = worldToScreen(glm::vec2(worldBottomRight.x, 0));
    glVertex2f(xAxisStart.x, xAxisStart.y);
    glVertex2f(xAxisEnd.x, xAxisEnd.y);
    
    // Y-axis
    glm::vec2 yAxisStart = worldToScreen(glm::vec2(0, worldTopLeft.y));
    glm::vec2 yAxisEnd = worldToScreen(glm::vec2(0, worldBottomRight.y));
    glVertex2f(yAxisStart.x, yAxisStart.y);
    glVertex2f(yAxisEnd.x, yAxisEnd.y);
    
    glEnd();
    glLineWidth(1.0f);
}

void Viewport::renderBackground() {
    // Render viewport background
    glColor4f(0.05f, 0.05f, 0.05f, 1.0f);
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
}

void Viewport::renderContent() {
    if (!targetEffect_) return;
    
    glPushMatrix();
    
    // Set up viewport transformation
    glm::vec2 center = position_ + size_ * 0.5f;
    glTranslatef(center.x + viewportOffset_.x, center.y + viewportOffset_.y, 0);
    glScalef(zoomLevel_, zoomLevel_, 1.0f);
    
    // Render the effect
    targetEffect_->render();
    
    glPopMatrix();
}

void Viewport::renderOverlay() {
    // Render zoom level
    glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
    std::cout << "Zoom: " << zoomLevel_ * 100 << "%" << std::endl;
    
    // Render viewport info
    glm::vec2 worldTopLeft = screenToWorld(position_);
    glm::vec2 worldBottomRight = screenToWorld(position_ + size_);
    std::cout << "Viewport: (" << worldTopLeft.x << ", " << worldTopLeft.y 
              << ") to (" << worldBottomRight.x << ", " << worldBottomRight.y << ")" << std::endl;
    
    // Render mouse position in world coordinates
    if (isMouseOver(glm::vec2(0, 0))) { // This would need actual mouse pos
        glm::vec2 worldPos = screenToWorld(glm::vec2(0, 0)); // This would need actual mouse pos
        std::cout << "World Pos: (" << worldPos.x << ", " << worldPos.y << ")" << std::endl;
    }
}

bool Viewport::isMouseOver(const glm::vec2& mousePos) const {
    return mousePos.x >= position_.x && mousePos.x <= position_.x + size_.x &&
           mousePos.y >= position_.y && mousePos.y <= position_.y + size_.y;
}

} // namespace BloodEditor