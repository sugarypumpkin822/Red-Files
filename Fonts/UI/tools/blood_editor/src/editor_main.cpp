#include "editor.h"
#include "ui_components.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <chrono>

namespace BloodEditor {

BloodEffect::BloodEffect() : time_(0.0f) {
    particles_.reserve(1000);
    curves_.reserve(100);
}

BloodEffect::~BloodEffect() = default;

void BloodEffect::update(float deltaTime) {
    time_ += deltaTime;
    
    // Update particles
    for (auto& particle : particles_) {
        if (!particle.active) continue;
        
        particle.age += deltaTime;
        if (particle.age >= particle.lifetime) {
            particle.active = false;
            continue;
        }
        
        // Update position with velocity
        particle.position += particle.velocity * deltaTime;
        
        // Apply gravity
        particle.velocity.y += 98.0f * deltaTime; // 9.8 m/s^2 scaled
        
        // Apply drag
        particle.velocity *= 0.99f;
        
        // Fade out over lifetime
        float lifeRatio = particle.age / particle.lifetime;
        particle.color.a = 1.0f - lifeRatio;
    }
    
    // Remove inactive particles
    particles_.erase(
        std::remove_if(particles_.begin(), particles_.end(),
            [](const BloodParticle& p) { return !p.active; }),
        particles_.end()
    );
}

void BloodEffect::render() {
    // Render curves
    for (const auto& curve : curves_) {
        if (curve.points.size() < 2) continue;
        
        // Render curve as connected line segments
        glBegin(GL_LINE_STRIP);
        glColor4f(curve.color.r, curve.color.g, curve.color.b, curve.color.a);
        
        for (const auto& point : curve.points) {
            glVertex2f(point.x, point.y);
        }
        
        glEnd();
    }
    
    // Render particles
    glBegin(GL_QUADS);
    for (const auto& particle : particles_) {
        if (!particle.active) continue;
        
        glColor4f(particle.color.r, particle.color.g, particle.color.b, particle.color.a);
        
        float halfSize = particle.size * 0.5f;
        glVertex2f(particle.position.x - halfSize, particle.position.y - halfSize);
        glVertex2f(particle.position.x + halfSize, particle.position.y - halfSize);
        glVertex2f(particle.position.x + halfSize, particle.position.y + halfSize);
        glVertex2f(particle.position.x - halfSize, particle.position.y + halfSize);
    }
    glEnd();
}

void BloodEffect::addParticle(const BloodParticle& particle) {
    particles_.push_back(particle);
}

void BloodEffect::addCurve(const BloodCurve& curve) {
    curves_.push_back(curve);
}

void BloodEffect::clear() {
    particles_.clear();
    curves_.clear();
    time_ = 0.0f;
}

Editor::Editor() : running_(false), deltaTime_(0.0f) {
    currentEffect_ = std::make_unique<BloodEffect>();
}

Editor::~Editor() {
    shutdown();
}

bool Editor::initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Blood Effect Editor", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    
    // Set up OpenGL
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    running_ = true;
    return true;
}

void Editor::run() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (running_) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        deltaTime_ = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        update(deltaTime_);
        render();
        handleInput();
        
        glfwSwapBuffers(glfwGetCurrentContext());
        glfwPollEvents();
        
        // Check for window close
        if (glfwWindowShouldClose(glfwGetCurrentContext())) {
            running_ = false;
        }
    }
}

void Editor::shutdown() {
    if (running_) {
        running_ = false;
        glfwTerminate();
    }
}

void Editor::update(float deltaTime) {
    if (currentEffect_) {
        currentEffect_->update(deltaTime);
    }
}

void Editor::render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    if (currentEffect_) {
        currentEffect_->render();
    }
}

void Editor::handleInput() {
    GLFWwindow* window = glfwGetCurrentContext();
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        running_ = false;
    }
    
    // Add blood particles on mouse click
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        BloodParticle particle;
        particle.position = glm::vec2(xpos, 720 - ypos); // Flip Y coordinate
        particle.velocity = glm::vec2(
            (rand() % 200 - 100) / 100.0f,  // Random X velocity
            (rand() % 200 - 100) / 100.0f   // Random Y velocity
        );
        particle.color = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f); // Blood red
        particle.size = (rand() % 10 + 5) / 10.0f; // Random size
        particle.lifetime = 3.0f + (rand() % 200) / 100.0f; // 3-5 seconds
        particle.age = 0.0f;
        particle.active = true;
        
        currentEffect_->addParticle(particle);
    }
}

} // namespace BloodEditor

int main() {
    BloodEditor::Editor editor;
    
    if (!editor.initialize()) {
        std::cerr << "Failed to initialize editor" << std::endl;
        return -1;
    }
    
    editor.run();
    
    return 0;
}