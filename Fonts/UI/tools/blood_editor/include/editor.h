#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace BloodEditor {

struct BloodParticle {
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec4 color;
    float size;
    float lifetime;
    float age;
    bool active;
};

struct BloodCurve {
    std::vector<glm::vec2> points;
    float thickness;
    glm::vec4 color;
    std::string name;
};

class BloodEffect {
public:
    BloodEffect();
    ~BloodEffect();
    
    void update(float deltaTime);
    void render();
    void addParticle(const BloodParticle& particle);
    void addCurve(const BloodCurve& curve);
    void clear();
    
    std::vector<BloodParticle>& getParticles() { return particles_; }
    std::vector<BloodCurve>& getCurves() { return curves_; }
    
private:
    std::vector<BloodParticle> particles_;
    std::vector<BloodCurve> curves_;
    float time_;
};

class Editor {
public:
    Editor();
    ~Editor();
    
    bool initialize();
    void run();
    void shutdown();
    
    BloodEffect* getCurrentEffect() { return currentEffect_.get(); }
    
private:
    void update(float deltaTime);
    void render();
    void handleInput();
    
    std::unique_ptr<BloodEffect> currentEffect_;
    bool running_;
    float deltaTime_;
};

} // namespace BloodEditor