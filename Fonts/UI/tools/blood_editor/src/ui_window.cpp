#include "editor.h"
#include "ui_components.h"
#include <iostream>
#include <algorithm>

namespace BloodEditor {
namespace UI {

class UIWindow {
public:
    enum WindowType {
        MAIN_EDITOR,
        PROPERTY_PANEL,
        TIMELINE,
        TOOLBAR,
        PRESET_BROWSER,
        EXPORT_DIALOG
    };
    
    UIWindow(WindowType type, const std::string& title, const glm::vec2& position, const glm::vec2& size);
    virtual ~UIWindow() = default;
    
    virtual void update(float deltaTime);
    virtual void render();
    virtual bool handleInput(const glm::vec2& mousePos, bool mouseDown, bool mousePressed);
    
    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }
    
    void setPosition(const glm::vec2& position) { position_ = position; }
    const glm::vec2& getPosition() const { return position_; }
    
    void setSize(const glm::vec2& size) { size_ = size; }
    const glm::vec2& getSize() const { return size_; }
    
    WindowType getType() const { return type_; }
    const std::string& getTitle() const { return title_; }
    
    void setFocused(bool focused) { focused_ = focused; }
    bool isFocused() const { return focused_; }
    
protected:
    WindowType type_;
    std::string title_;
    glm::vec2 position_;
    glm::vec2 size_;
    bool visible_;
    bool focused_;
    
    virtual void renderContent() = 0;
    virtual void renderBorder();
    virtual void renderTitleBar();
    
    bool isMouseOver(const glm::vec2& mousePos) const;
    bool isMouseOverTitleBar(const glm::vec2& mousePos) const;
    
private:
    bool dragging_;
    glm::vec2 dragOffset_;
};

UIWindow::UIWindow(WindowType type, const std::string& title, const glm::vec2& position, const glm::vec2& size)
    : type_(type), title_(title), position_(position), size_(size), 
      visible_(true), focused_(false), dragging_(false), dragOffset_(0.0f) {
}

void UIWindow::update(float deltaTime) {
    // Base implementation does nothing
}

void UIWindow::render() {
    if (!visible_) return;
    
    renderTitleBar();
    renderContent();
    renderBorder();
}

bool UIWindow::handleInput(const glm::vec2& mousePos, bool mouseDown, bool mousePressed) {
    if (!visible_) return false;
    
    if (mousePressed && isMouseOverTitleBar(mousePos)) {
        dragging_ = true;
        dragOffset_ = mousePos - position_;
        focused_ = true;
        return true;
    }
    
    if (mouseDown && dragging_) {
        position_ = mousePos - dragOffset_;
        return true;
    }
    
    if (!mouseDown) {
        dragging_ = false;
    }
    
    return false;
}

void UIWindow::renderBorder() {
    glColor4f(0.3f, 0.3f, 0.3f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(position_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y + size_.y);
    glVertex2f(position_.x, position_.y + size_.y);
    glEnd();
}

void UIWindow::renderTitleBar() {
    float titleBarHeight = 25.0f;
    
    // Render title bar background
    glColor4f(focused_ ? 0.2f : 0.15f, focused_ ? 0.2f : 0.15f, focused_ ? 0.3f : 0.25f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(position_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y);
    glVertex2f(position_.x + size_.x, position_.y + titleBarHeight);
    glVertex2f(position_.x, position_.y + titleBarHeight);
    glEnd();
    
    // Render title text (simplified - would use proper text rendering)
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    std::cout << title_ << std::endl;
    
    // Render close button
    float closeButtonSize = 15.0f;
    float closeButtonX = position_.x + size_.x - closeButtonSize - 5;
    float closeButtonY = position_.y + 5;
    
    glColor4f(0.8f, 0.2f, 0.2f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(closeButtonX, closeButtonY);
    glVertex2f(closeButtonX + closeButtonSize, closeButtonY);
    glVertex2f(closeButtonX + closeButtonSize, closeButtonY + closeButtonSize);
    glVertex2f(closeButtonX, closeButtonY + closeButtonSize);
    glEnd();
    
    // Render close button X
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(closeButtonX + 3, closeButtonY + 3);
    glVertex2f(closeButtonX + closeButtonSize - 3, closeButtonY + closeButtonSize - 3);
    glVertex2f(closeButtonX + closeButtonSize - 3, closeButtonY + 3);
    glVertex2f(closeButtonX + 3, closeButtonY + closeButtonSize - 3);
    glEnd();
}

bool UIWindow::isMouseOver(const glm::vec2& mousePos) const {
    return mousePos.x >= position_.x && mousePos.x <= position_.x + size_.x &&
           mousePos.y >= position_.y && mousePos.y <= position_.y + size_.y;
}

bool UIWindow::isMouseOverTitleBar(const glm::vec2& mousePos) const {
    float titleBarHeight = 25.0f;
    return mousePos.x >= position_.x && mousePos.x <= position_.x + size_.x &&
           mousePos.y >= position_.y && mousePos.y <= position_.y + titleBarHeight;
}

// Main Editor Window
class MainEditorWindow : public UIWindow {
public:
    MainEditorWindow();
    
    void renderContent() override;
    bool handleInput(const glm::vec2& mousePos, bool mouseDown, bool mousePressed) override;
    
    void setTargetEffect(BloodEffect* effect) { targetEffect_ = effect; }
    
private:
    BloodEffect* targetEffect_;
    glm::vec2 viewportOffset_;
    float zoomLevel_;
    
    bool panning_;
    glm::vec2 lastMousePos_;
};

MainEditorWindow::MainEditorWindow() 
    : UIWindow(MAIN_EDITOR, "Blood Effect Editor", glm::vec2(200, 50), glm::vec2(800, 600)),
      targetEffect_(nullptr), viewportOffset_(0.0f), zoomLevel_(1.0f), panning_(false) {
}

void MainEditorWindow::renderContent() {
    float titleBarHeight = 25.0f;
    
    // Render viewport background
    glColor4f(0.05f, 0.05f, 0.05f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(position_.x, position_.y + titleBarHeight);
    glVertex2f(position_.x + size_.x, position_.y + titleBarHeight);
    glVertex2f(position_.x + size_.x, position_.y + size_.y);
    glVertex2f(position_.x, position_.y + size_.y);
    glEnd();
    
    // Render grid
    glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
    glBegin(GL_LINES);
    for (int x = 0; x < 20; ++x) {
        float xPos = position_.x + x * 40 + viewportOffset_.x;
        glVertex2f(xPos, position_.y + titleBarHeight);
        glVertex2f(xPos, position_.y + size_.y);
    }
    for (int y = 0; y < 15; ++y) {
        float yPos = position_.y + titleBarHeight + y * 40 + viewportOffset_.y;
        glVertex2f(position_.x, yPos);
        glVertex2f(position_.x + size_.x, yPos);
    }
    glEnd();
    
    // Render effect if available
    if (targetEffect_) {
        glPushMatrix();
        glTranslatef(position_.x + size_.x * 0.5f + viewportOffset_.x, 
                   position_.y + titleBarHeight + size_.y * 0.5f + viewportOffset_.y, 0);
        glScalef(zoomLevel_, zoomLevel_, 1.0f);
        targetEffect_->render();
        glPopMatrix();
    }
}

bool MainEditorWindow::handleInput(const glm::vec2& mousePos, bool mouseDown, bool mousePressed) {
    if (UIWindow::handleInput(mousePos, mouseDown, mousePressed)) {
        return true;
    }
    
    float titleBarHeight = 25.0f;
    glm::vec2 localPos = mousePos - glm::vec2(position_.x, position_.y + titleBarHeight);
    
    if (mousePressed && localPos.x >= 0 && localPos.x <= size_.x && 
        localPos.y >= 0 && localPos.y <= size_.y - titleBarHeight) {
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

// Toolbar Window
class ToolbarWindow : public UIWindow {
public:
    ToolbarWindow();
    
    void renderContent() override;
    bool handleInput(const glm::vec2& mousePos, bool mouseDown, bool mousePressed) override;
    
private:
    std::vector<Button> buttons_;
    
    void createButtons();
};

ToolbarWindow::ToolbarWindow() 
    : UIWindow(TOOLBAR, "Toolbar", glm::vec2(10, 50), glm::vec2(180, 400)) {
    createButtons();
}

void ToolbarWindow::renderContent() {
    float titleBarHeight = 25.0f;
    
    // Render toolbar background
    glColor4f(0.1f, 0.1f, 0.1f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(position_.x, position_.y + titleBarHeight);
    glVertex2f(position_.x + size_.x, position_.y + titleBarHeight);
    glVertex2f(position_.x + size_.x, position_.y + size_.y);
    glVertex2f(position_.x, position_.y + size_.y);
    glEnd();
    
    // Render buttons
    for (auto& button : buttons_) {
        button.render();
    }
}

bool ToolbarWindow::handleInput(const glm::vec2& mousePos, bool mouseDown, bool mousePressed) {
    if (UIWindow::handleInput(mousePos, mouseDown, mousePressed)) {
        return true;
    }
    
    for (auto& button : buttons_) {
        if (button.handleClick(mousePos)) {
            return true;
        }
    }
    
    return false;
}

void ToolbarWindow::createButtons() {
    buttons_.clear();
    
    float buttonY = position_.y + 35;
    float buttonSpacing = 35;
    
    buttons_.emplace_back("Play", glm::vec2(position_.x + 10, buttonY), glm::vec2(160, 30));
    buttons_.emplace_back("Pause", glm::vec2(position_.x + 10, buttonY + buttonSpacing), glm::vec2(160, 30));
    buttons_.emplace_back("Stop", glm::vec2(position_.x + 10, buttonY + buttonSpacing * 2), glm::vec2(160, 30));
    buttons_.emplace_back("Clear", glm::vec2(position_.x + 10, buttonY + buttonSpacing * 3), glm::vec2(160, 30));
    buttons_.emplace_back("Export", glm::vec2(position_.x + 10, buttonY + buttonSpacing * 4), glm::vec2(160, 30));
    buttons_.emplace_back("Import", glm::vec2(position_.x + 10, buttonY + buttonSpacing * 5), glm::vec2(160, 30));
}

// Window Manager
class WindowManager {
public:
    WindowManager();
    ~WindowManager() = default;
    
    void update(float deltaTime);
    void render();
    bool handleInput(const glm::vec2& mousePos, bool mouseDown, bool mousePressed);
    
    void addWindow(std::unique_ptr<UIWindow> window);
    void removeWindow(UIWindow::WindowType type);
    
    UIWindow* getWindow(UIWindow::WindowType type);
    void setTargetEffect(BloodEffect* effect);
    
private:
    std::vector<std::unique_ptr<UIWindow>> windows_;
    BloodEffect* targetEffect_;
};

WindowManager::WindowManager() : targetEffect_(nullptr) {
    // Create default windows
    addWindow(std::make_unique<MainEditorWindow>());
    addWindow(std::make_unique<ToolbarWindow>());
}

void WindowManager::update(float deltaTime) {
    for (auto& window : windows_) {
        window->update(deltaTime);
    }
}

void WindowManager::render() {
    for (auto& window : windows_) {
        window->render();
    }
}

bool WindowManager::handleInput(const glm::vec2& mousePos, bool mouseDown, bool mousePressed) {
    for (auto& window : windows_) {
        if (window->handleInput(mousePos, mouseDown, mousePressed)) {
            return true;
        }
    }
    return false;
}

void WindowManager::addWindow(std::unique_ptr<UIWindow> window) {
    windows_.push_back(std::move(window));
}

void WindowManager::removeWindow(UIWindow::WindowType type) {
    windows_.erase(
        std::remove_if(windows_.begin(), windows_.end(),
            [type](const std::unique_ptr<UIWindow>& w) { return w->getType() == type; }),
        windows_.end()
    );
}

UIWindow* WindowManager::getWindow(UIWindow::Type type) {
    auto it = std::find_if(windows_.begin(), windows_.end(),
        [type](const std::unique_ptr<UIWindow>& w) { return w->getType() == type; });
    return (it != windows_.end()) ? it->get() : nullptr;
}

void WindowManager::setTargetEffect(BloodEffect* effect) {
    targetEffect_ = effect;
    
    // Update all windows that need the effect
    for (auto& window : windows_) {
        if (auto mainEditor = dynamic_cast<MainEditorWindow*>(window.get())) {
            mainEditor->setTargetEffect(effect);
        }
    }
}

} // namespace UI
} // namespace BloodEditor