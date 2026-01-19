#include "editor.h"
#include "ui_components.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>
#include <fstream>
#include <sstream>
#include <chrono>

namespace BloodEditor {

// Advanced particle system with multiple emission patterns
class ParticleTuner {
public:
    enum class EmissionPattern {
        POINT,
        LINE,
        CIRCLE,
        SPHERE,
        CONE,
        CUBE,
        CUSTOM
    };
    
    enum class ParticleShape {
        CIRCLE,
        SQUARE,
        TRIANGLE,
        STAR,
        CROSS,
        PLUS,
        DIAMOND,
        HEXAGON
    };
    
    struct ParticleSettings {
        // Emission properties
        EmissionPattern pattern = EmissionPattern::POINT;
        ParticleShape shape = ParticleShape::CIRCLE;
        
        // Basic properties
        float emissionRate = 50.0f;           // Particles per second
        float particleLifetime = 3.0f;        // Seconds
        float particleSize = 2.0f;            // Pixels
        float sizeVariation = 0.5f;           // Size randomization
        
        // Velocity properties
        glm::vec2 baseVelocity = glm::vec2(0.0f, -100.0f);
        glm::vec2 velocityVariation = glm::vec2(50.0f, 20.0f);
        float velocityDamping = 0.98f;        // Air resistance
        
        // Physics properties
        glm::vec2 gravity = glm::vec2(0.0f, -200.0f);
        float bounceFactor = 0.3f;           // Energy loss on collision
        float friction = 0.95f;               // Surface friction
        
        // Color properties
        glm::vec4 baseColor = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
        glm::vec4 colorVariation = glm::vec4(0.2f, 0.1f, 0.1f, 0.0f);
        glm::vec4 endColor = glm::vec4(0.3f, 0.05f, 0.05f, 0.8f);
        bool colorOverLifetime = true;
        
        // Advanced properties
        float rotationSpeed = 0.0f;           // Degrees per second
        float rotationVariation = 180.0f;     // Rotation randomization
        float pulseFrequency = 0.0f;           // Pulsing effect
        float pulseAmplitude = 0.0f;          // Pulse intensity
        
        // Texture properties
        bool useTexture = false;
        std::string texturePath;
        glm::vec2 textureScale = glm::vec2(1.0f, 1.0f);
        float textureRotation = 0.0f;
        
        // Collision properties
        bool enableCollision = true;
        float collisionRadius = 1.0f;
        bool stickToSurfaces = false;
        float stickiness = 0.8f;
        
        // Trail properties
        bool enableTrail = false;
        float trailLength = 10.0f;
        float trailFadeRate = 0.9f;
        
        // Glow properties
        bool enableGlow = false;
        float glowRadius = 5.0f;
        glm::vec4 glowColor = glm::vec4(1.0f, 0.2f, 0.2f, 0.5f);
        
        // Noise properties
        bool enableNoise = false;
        float noiseStrength = 10.0f;
        float noiseFrequency = 0.1f;
        float noiseSpeed = 1.0f;
    };
    
    struct Particle {
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec2 acceleration;
        glm::vec4 color;
        glm::vec4 startColor;
        glm::vec4 targetColor;
        float size;
        float startSize;
        float rotation;
        float rotationSpeed;
        float lifetime;
        float maxLifetime;
        float age;
        bool active;
        bool stuck;
        ParticleShape shape;
        
        // Trail data
        std::vector<glm::vec2> trailPositions;
        std::vector<float> trailAlphas;
        
        // Noise data
        float noiseOffset;
        
        // Pulse data
        float pulsePhase;
    };
    
    ParticleTuner();
    ~ParticleTuner() = default;
    
    void update(float deltaTime);
    void render();
    void renderUI();
    
    void emitParticles(int count = 1);
    void emitFromPattern(const glm::vec2& position, int count);
    
    void clearParticles() { particles_.clear(); }
    size_t getParticleCount() const { return particles_.size(); }
    
    ParticleSettings& getSettings() { return settings_; }
    const ParticleSettings& getSettings() const { return settings_; }
    
    void setSettings(const ParticleSettings& settings) { settings_ = settings; }
    
    const std::vector<Particle>& getParticles() const { return particles_; }
    
    // Preset management
    void loadPreset(const std::string& presetName);
    void savePreset(const std::string& presetName);
    std::vector<std::string> getAvailablePresets() const;
    
    // Performance monitoring
    float getAverageLifetime() const;
    float getEmissionRate() const;
    size_t getActiveParticleCount() const;
    
private:
    ParticleSettings settings_;
    std::vector<Particle> particles_;
    
    // Emission control
    float emissionAccumulator_;
    std::mt19937 randomGenerator_;
    std::uniform_real_distribution<float> uniformDistribution_;
    
    // Performance tracking
    float totalEmissionTime_;
    size_t totalParticlesEmitted_;
    mutable std::vector<float> lifetimeHistory_;
    
    Particle createParticle(const glm::vec2& position);
    void updateParticle(Particle& particle, float deltaTime);
    void updateParticlePhysics(Particle& particle, float deltaTime);
    void updateParticleVisuals(Particle& particle, float deltaTime);
    void updateParticleTrail(Particle& particle, float deltaTime);
    
    void renderParticle(const Particle& particle);
    void renderParticleShape(const Particle& particle);
    void renderParticleGlow(const Particle& particle);
    void renderParticleTrail(const Particle& particle);
    
    // Pattern generation
    glm::vec2 generateEmissionPosition(const glm::vec2& center, EmissionPattern pattern);
    glm::vec2 generateEmissionVelocity(const glm::vec2& position, EmissionPattern pattern);
    
    // Color interpolation
    glm::vec4 interpolateColor(const glm::vec4& start, const glm::vec4& end, float t);
    
    // Noise generation
    float generateNoise(float x, float y, float time);
    
    // Collision detection
    bool checkCollision(const Particle& particle, glm::vec2& collisionNormal);
    void resolveCollision(Particle& particle, const glm::vec2& normal);
    
    // UI helpers
    void renderEmissionPatternUI();
    void renderPhysicsUI();
    void renderColorUI();
    void renderAdvancedUI();
    void renderPresetUI();
    void renderPerformanceUI();
    
    // Preset definitions
    void initializeDefaultPresets();
    std::map<std::string, ParticleSettings> presets_;
};

ParticleTuner::ParticleTuner() 
    : emissionAccumulator_(0.0f), 
      randomGenerator_(std::chrono::steady_clock::now().time_since_epoch().count()),
      uniformDistribution_(0.0f, 1.0f),
      totalEmissionTime_(0.0f),
      totalParticlesEmitted_(0) {
    initializeDefaultPresets();
}

void ParticleTuner::update(float deltaTime) {
    totalEmissionTime_ += deltaTime;
    
    // Update existing particles
    for (auto& particle : particles_) {
        if (particle.active) {
            updateParticle(particle, deltaTime);
        }
    }
    
    // Remove dead particles
    particles_.erase(
        std::remove_if(particles_.begin(), particles_.end(),
            [](const Particle& p) { return !p.active; }),
        particles_.end()
    );
    
    // Emit new particles
    emissionAccumulator_ += settings_.emissionRate * deltaTime;
    int particlesToEmit = static_cast<int>(emissionAccumulator_);
    if (particlesToEmit > 0) {
        emissionAccumulator_ -= particlesToEmit;
        emitParticles(particlesToEmit);
        totalParticlesEmitted_ += particlesToEmit;
    }
}

void ParticleTuner::updateParticle(Particle& particle, float deltaTime) {
    particle.age += deltaTime;
    
    if (particle.age >= particle.maxLifetime) {
        particle.active = false;
        lifetimeHistory_.push_back(particle.age);
        if (lifetimeHistory_.size() > 1000) {
            lifetimeHistory_.erase(lifetimeHistory_.begin());
        }
        return;
    }
    
    if (!particle.stuck) {
        updateParticlePhysics(particle, deltaTime);
    }
    updateParticleVisuals(particle, deltaTime);
    updateParticleTrail(particle, deltaTime);
}

void ParticleTuner::updateParticlePhysics(Particle& particle, float deltaTime) {
    // Apply gravity
    particle.acceleration = settings_.gravity;
    
    // Apply noise if enabled
    if (settings_.enableNoise) {
        float noise = generateNoise(particle.position.x, particle.position.y, particle.age);
        particle.acceleration.x += noise * settings_.noiseStrength;
    }
    
    // Update velocity and position
    particle.velocity += particle.acceleration * deltaTime;
    particle.velocity *= settings_.velocityDamping; // Air resistance
    particle.position += particle.velocity * deltaTime;
    
    // Collision detection
    if (settings_.enableCollision) {
        glm::vec2 collisionNormal;
        if (checkCollision(particle, collisionNormal)) {
            resolveCollision(particle, collisionNormal);
        }
    }
}

void ParticleTuner::updateParticleVisuals(Particle& particle, float deltaTime) {
    float lifeRatio = particle.age / particle.maxLifetime;
    
    // Update color over lifetime
    if (settings_.colorOverLifetime) {
        particle.color = interpolateColor(particle.startColor, particle.targetColor, lifeRatio);
    }
    
    // Update size with pulse effect
    if (settings_.pulseFrequency > 0.0f) {
        particle.pulsePhase += deltaTime * settings_.pulseFrequency * 2.0f * M_PI;
        float pulseFactor = 1.0f + sin(particle.pulsePhase) * settings_.pulseAmplitude;
        particle.size = particle.startSize * pulseFactor;
    }
    
    // Update rotation
    particle.rotation += particle.rotationSpeed * deltaTime;
}

void ParticleTuner::updateParticleTrail(Particle& particle, float deltaTime) {
    if (!settings_.enableTrail) return;
    
    // Add current position to trail
    particle.trailPositions.push_back(particle.position);
    particle.trailAlphas.push_back(1.0f);
    
    // Limit trail length
    size_t maxTrailPoints = static_cast<size_t>(settings_.trailLength);
    while (particle.trailPositions.size() > maxTrailPoints) {
        particle.trailPositions.erase(particle.trailPositions.begin());
        particle.trailAlphas.erase(particle.trailAlphas.begin());
    }
    
    // Fade trail
    for (auto& alpha : particle.trailAlphas) {
        alpha *= settings_.trailFadeRate;
    }
}

void ParticleTuner::render() {
    // Render trails first (behind particles)
    if (settings_.enableTrail) {
        for (const auto& particle : particles_) {
            if (particle.active) {
                renderParticleTrail(particle);
            }
        }
    }
    
    // Render glow effects
    if (settings_.enableGlow) {
        for (const auto& particle : particles_) {
            if (particle.active) {
                renderParticleGlow(particle);
            }
        }
    }
    
    // Render particles
    for (const auto& particle : particles_) {
        if (particle.active) {
            renderParticle(particle);
        }
    }
}

void ParticleTuner::renderParticle(const Particle& particle) {
    glPushMatrix();
    glTranslatef(particle.position.x, particle.position.y, 0);
    glRotatef(particle.rotation, 0, 0, 1);
    
    glColor4f(particle.color.r, particle.color.g, particle.color.b, particle.color.a);
    renderParticleShape(particle);
    
    glPopMatrix();
}

void ParticleTuner::renderParticleShape(const Particle& particle) {
    float size = particle.size;
    
    switch (particle.shape) {
        case ParticleShape::CIRCLE:
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, 0);
            for (int i = 0; i <= 32; ++i) {
                float angle = (i / 32.0f) * 2.0f * M_PI;
                glVertex2f(cos(angle) * size, sin(angle) * size);
            }
            glEnd();
            break;
            
        case ParticleShape::SQUARE:
            glBegin(GL_QUADS);
            glVertex2f(-size, -size);
            glVertex2f(size, -size);
            glVertex2f(size, size);
            glVertex2f(-size, size);
            glEnd();
            break;
            
        case ParticleShape::TRIANGLE:
            glBegin(GL_TRIANGLES);
            glVertex2f(0, size);
            glVertex2f(-size, -size);
            glVertex2f(size, -size);
            glEnd();
            break;
            
        case ParticleShape::STAR:
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, 0);
            for (int i = 0; i <= 10; ++i) {
                float angle = (i / 10.0f) * 2.0f * M_PI;
                float radius = (i % 2 == 0) ? size : size * 0.5f;
                glVertex2f(cos(angle) * radius, sin(angle) * radius);
            }
            glEnd();
            break;
            
        case ParticleShape::CROSS:
            glLineWidth(size * 0.3f);
            glBegin(GL_LINES);
            glVertex2f(-size, 0);
            glVertex2f(size, 0);
            glVertex2f(0, -size);
            glVertex2f(0, size);
            glEnd();
            glLineWidth(1.0f);
            break;
            
        case ParticleShape::PLUS:
            glLineWidth(size * 0.2f);
            glBegin(GL_LINES);
            glVertex2f(-size * 0.7f, 0);
            glVertex2f(size * 0.7f, 0);
            glVertex2f(0, -size * 0.7f);
            glVertex2f(0, size * 0.7f);
            glEnd();
            glLineWidth(1.0f);
            break;
            
        case ParticleShape::DIAMOND:
            glBegin(GL_QUADS);
            glVertex2f(0, size);
            glVertex2f(size, 0);
            glVertex2f(0, -size);
            glVertex2f(-size, 0);
            glEnd();
            break;
            
        case ParticleShape::HEXAGON:
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, 0);
            for (int i = 0; i <= 6; ++i) {
                float angle = (i / 6.0f) * 2.0f * M_PI;
                glVertex2f(cos(angle) * size, sin(angle) * size);
            }
            glEnd();
            break;
    }
}

void ParticleTuner::renderParticleGlow(const Particle& particle) {
    glPushMatrix();
    glTranslatef(particle.position.x, particle.position.y, 0);
    
    // Render glow as a series of expanding circles
    int glowSteps = 5;
    for (int i = glowSteps; i > 0; --i) {
        float glowSize = settings_.glowRadius * (i / float(glowSteps));
        float alpha = settings_.glowColor.a * (1.0f - i / float(glowSteps)) * particle.color.a;
        
        glColor4f(settings_.glowColor.r, settings_.glowColor.g, settings_.glowColor.b, alpha);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 0);
        for (int j = 0; j <= 16; ++j) {
            float angle = (j / 16.0f) * 2.0f * M_PI;
            glVertex2f(cos(angle) * glowSize, sin(angle) * glowSize);
        }
        glEnd();
    }
    
    glPopMatrix();
}

void ParticleTuner::renderParticleTrail(const Particle& particle) {
    if (particle.trailPositions.size() < 2) return;
    
    glLineWidth(particle.size * 0.5f);
    glBegin(GL_LINE_STRIP);
    
    for (size_t i = 0; i < particle.trailPositions.size(); ++i) {
        float alpha = particle.trailAlphas[i] * particle.color.a;
        glColor4f(particle.color.r, particle.color.g, particle.color.b, alpha);
        glVertex2f(particle.trailPositions[i].x, particle.trailPositions[i].y);
    }
    
    glEnd();
    glLineWidth(1.0f);
}

void ParticleTuner::emitParticles(int count) {
    glm::vec2 position(0, 0); // Default position
    emitFromPattern(position, count);
}

void ParticleTuner::emitFromPattern(const glm::vec2& position, int count) {
    for (int i = 0; i < count; ++i) {
        glm::vec2 emitPos = generateEmissionPosition(position, settings_.pattern);
        Particle particle = createParticle(emitPos);
        particles_.push_back(particle);
    }
}

Particle ParticleTuner::createParticle(const glm::vec2& position) {
    Particle particle;
    
    // Position
    particle.position = position;
    
    // Velocity
    particle.velocity = generateEmissionVelocity(position, settings_.pattern);
    particle.acceleration = glm::vec2(0, 0);
    
    // Color
    particle.startColor = settings_.baseColor + glm::vec4(
        (uniformDistribution_(randomGenerator_) - 0.5f) * settings_.colorVariation.r,
        (uniformDistribution_(randomGenerator_) - 0.5f) * settings_.colorVariation.g,
        (uniformDistribution_(randomGenerator_) - 0.5f) * settings_.colorVariation.b,
        (uniformDistribution_(randomGenerator_) - 0.5f) * settings_.colorVariation.a
    );
    particle.startColor = glm::clamp(particle.startColor, 0.0f, 1.0f);
    particle.targetColor = settings_.endColor;
    particle.color = particle.startColor;
    
    // Size
    particle.startSize = settings_.particleSize + (uniformDistribution_(randomGenerator_) - 0.5f) * settings_.sizeVariation;
    particle.size = particle.startSize;
    
    // Rotation
    particle.rotation = 0.0f;
    particle.rotationSpeed = settings_.rotationSpeed + (uniformDistribution_(randomGenerator_) - 0.5f) * settings_.rotationVariation;
    
    // Lifetime
    particle.maxLifetime = settings_.particleLifetime;
    particle.age = 0.0f;
    particle.active = true;
    particle.stuck = false;
    
    // Shape
    particle.shape = settings_.shape;
    
    // Trail
    particle.trailPositions.clear();
    particle.trailAlphas.clear();
    
    // Noise
    particle.noiseOffset = uniformDistribution_(randomGenerator_) * 1000.0f;
    
    // Pulse
    particle.pulsePhase = uniformDistribution_(randomGenerator_) * 2.0f * M_PI;
    
    return particle;
}

glm::vec2 ParticleTuner::generateEmissionPosition(const glm::vec2& center, EmissionPattern pattern) {
    switch (pattern) {
        case EmissionPattern::POINT:
            return center;
            
        case EmissionPattern::LINE: {
            float t = uniformDistribution_(randomGenerator_);
            return center + glm::vec2((t - 0.5f) * 100.0f, 0);
        }
        
        case EmissionPattern::CIRCLE: {
            float angle = uniformDistribution_(randomGenerator_) * 2.0f * M_PI;
            float radius = uniformDistribution_(randomGenerator_) * 50.0f;
            return center + glm::vec2(cos(angle) * radius, sin(angle) * radius);
        }
        
        case EmissionPattern::SPHERE: {
            float theta = uniformDistribution_(randomGenerator_) * 2.0f * M_PI;
            float phi = acos(1.0f - 2.0f * uniformDistribution_(randomGenerator_));
            float radius = uniformDistribution_(randomGenerator_) * 50.0f;
            return center + glm::vec2(
                radius * sin(phi) * cos(theta),
                radius * sin(phi) * sin(theta)
            );
        }
        
        case EmissionPattern::CONE: {
            float angle = uniformDistribution_(randomGenerator_) * 2.0f * M_PI;
            float radius = uniformDistribution_(randomGenerator_) * 30.0f;
            return center + glm::vec2(cos(angle) * radius, -abs(sin(angle) * radius));
        }
        
        case EmissionPattern::CUBE: {
            return center + glm::vec2(
                (uniformDistribution_(randomGenerator_) - 0.5f) * 100.0f,
                (uniformDistribution_(randomGenerator_) - 0.5f) * 100.0f
            );
        }
        
        default:
            return center;
    }
}

glm::vec2 ParticleTuner::generateEmissionVelocity(const glm::vec2& position, EmissionPattern pattern) {
    glm::vec2 velocity = settings_.baseVelocity;
    
    // Add variation
    velocity.x += (uniformDistribution_(randomGenerator_) - 0.5f) * settings_.velocityVariation.x;
    velocity.y += (uniformDistribution_(randomGenerator_) - 0.5f) * settings_.velocityVariation.y;
    
    // Pattern-specific velocity modifications
    switch (pattern) {
        case EmissionPattern::CIRCLE: {
            glm::vec2 direction = glm::normalize(position - glm::vec2(0, 0));
            velocity += direction * 50.0f;
            break;
        }
        
        case EmissionPattern::CONE: {
            glm::vec2 direction = glm::normalize(position - glm::vec2(0, 0));
            velocity += direction * 100.0f;
            break;
        }
        
        default:
            break;
    }
    
    return velocity;
}

glm::vec4 ParticleTuner::interpolateColor(const glm::vec4& start, const glm::vec4& end, float t) {
    return glm::mix(start, end, t);
}

float ParticleTuner::generateNoise(float x, float y, float time) {
    // Simple noise function using sine waves
    float noise = sin(x * settings_.noiseFrequency + time * settings_.noiseSpeed) * 
                  cos(y * settings_.noiseFrequency + time * settings_.noiseSpeed);
    return noise * 0.5f + 0.5f; // Normalize to [0, 1]
}

bool ParticleTuner::checkCollision(const Particle& particle, glm::vec2& collisionNormal) {
    // Simple ground collision
    if (particle.position.y <= -200.0f) {
        collisionNormal = glm::vec2(0, 1);
        return true;
    }
    return false;
}

void ParticleTuner::resolveCollision(Particle& particle, const glm::vec2& normal) {
    // Reflect velocity
    float dotProduct = glm::dot(particle.velocity, normal);
    particle.velocity = particle.velocity - 2.0f * dotProduct * normal;
    particle.velocity *= settings_.bounceFactor;
    
    // Apply friction
    glm::vec2 tangent = glm::vec2(-normal.y, normal.x);
    float tangentVelocity = glm::dot(particle.velocity, tangent);
    particle.velocity -= tangent * tangentVelocity * (1.0f - settings_.friction);
    
    // Check if particle should stick
    if (settings_.stickToSurfaces && glm::length(particle.velocity) < 10.0f) {
        particle.stuck = true;
        particle.velocity = glm::vec2(0, 0);
    }
    
    // Push particle out of collision
    if (particle.position.y <= -200.0f) {
        particle.position.y = -200.0f + settings_.collisionRadius;
    }
}

void ParticleTuner::renderUI() {
    std::cout << "=== Advanced Particle Tuner ===" << std::endl;
    std::cout << "Active Particles: " << getActiveParticleCount() << std::endl;
    std::cout << "Emission Rate: " << settings_.emissionRate << " particles/sec" << std::endl;
    std::cout << "Pattern: " << static_cast<int>(settings_.pattern) << std::endl;
    std::cout << "Shape: " << static_cast<int>(settings_.shape) << std::endl;
    std::cout << "Base Color: " << settings_.baseColor.r << ", " 
              << settings_.baseColor.g << ", " << settings_.baseColor.b << std::endl;
    std::cout << "Particle Size: " << settings_.particleSize << std::endl;
    std::cout << "Lifetime: " << settings_.particleLifetime << "s" << std::endl;
    std::cout << "Gravity: " << settings_.gravity.x << ", " << settings_.gravity.y << std::endl;
    std::cout << "Collision: " << (settings_.enableCollision ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Trail: " << (settings_.enableTrail ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Glow: " << (settings_.enableGlow ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Noise: " << (settings_.enableNoise ? "Enabled" : "Disabled") << std::endl;
    std::cout << "===============================" << std::endl;
}

void ParticleTuner::initializeDefaultPresets() {
    // Blood Splash preset
    ParticleSettings bloodSplash;
    bloodSplash.pattern = EmissionPattern::SPHERE;
    bloodSplash.shape = ParticleShape::CIRCLE;
    bloodSplash.emissionRate = 100.0f;
    bloodSplash.particleLifetime = 2.0f;
    bloodSplash.particleSize = 3.0f;
    bloodSplash.baseColor = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
    bloodSplash.endColor = glm::vec4(0.4f, 0.05f, 0.05f, 0.3f);
    bloodSplash.baseVelocity = glm::vec2(0, 50.0f);
    bloodSplash.velocityVariation = glm::vec2(200.0f, 100.0f);
    bloodSplash.gravity = glm::vec2(0, -300.0f);
    bloodSplash.enableTrail = true;
    bloodSplash.trailLength = 5.0f;
    presets_["Blood Splash"] = bloodSplash;
    
    // Blood Drip preset
    ParticleSettings bloodDrip;
    bloodDrip.pattern = EmissionPattern::POINT;
    bloodDrip.shape = ParticleShape::CIRCLE;
    bloodDrip.emissionRate = 10.0f;
    bloodDrip.particleLifetime = 5.0f;
    bloodDrip.particleSize = 2.0f;
    bloodDrip.baseColor = glm::vec4(0.7f, 0.1f, 0.1f, 0.9f);
    bloodDrip.endColor = glm::vec4(0.3f, 0.05f, 0.05f, 0.1f);
    bloodDrip.baseVelocity = glm::vec2(0, -20.0f);
    bloodDrip.velocityVariation = glm::vec2(5.0f, 5.0f);
    bloodDrip.gravity = glm::vec2(0, -150.0f);
    bloodDrip.stickToSurfaces = true;
    bloodDrip.stickiness = 0.9f;
    presets_["Blood Drip"] = bloodDrip;
    
    // Blood Spray preset
    ParticleSettings bloodSpray;
    bloodSpray.pattern = EmissionPattern::CONE;
    bloodSpray.shape = ParticleShape::SQUARE;
    bloodSpray.emissionRate = 200.0f;
    bloodSpray.particleLifetime = 1.5f;
    bloodSpray.particleSize = 1.5f;
    bloodSpray.baseColor = glm::vec4(0.9f, 0.2f, 0.2f, 0.8f);
    bloodSpray.endColor = glm::vec4(0.5f, 0.1f, 0.1f, 0.2f);
    bloodSpray.baseVelocity = glm::vec2(0, 200.0f);
    bloodSpray.velocityVariation = glm::vec2(100.0f, 50.0f);
    bloodSpray.gravity = glm::vec2(0, -400.0f);
    bloodSpray.rotationSpeed = 360.0f;
    bloodSpray.rotationVariation = 180.0f;
    presets_["Blood Spray"] = bloodSpray;
}

void ParticleTuner::loadPreset(const std::string& presetName) {
    auto it = presets_.find(presetName);
    if (it != presets_.end()) {
        settings_ = it->second;
        std::cout << "Loaded preset: " << presetName << std::endl;
    } else {
        std::cout << "Preset not found: " << presetName << std::endl;
    }
}

void ParticleTuner::savePreset(const std::string& presetName) {
    presets_[presetName] = settings_;
    std::cout << "Saved preset: " << presetName << std::endl;
}

std::vector<std::string> ParticleTuner::getAvailablePresets() const {
    std::vector<std::string> names;
    for (const auto& pair : presets_) {
        names.push_back(pair.first);
    }
    return names;
}

float ParticleTuner::getAverageLifetime() const {
    if (lifetimeHistory_.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (float lifetime : lifetimeHistory_) {
        sum += lifetime;
    }
    return sum / lifetimeHistory_.size();
}

float ParticleTuner::getEmissionRate() const {
    if (totalEmissionTime_ <= 0.0f) return 0.0f;
    return totalParticlesEmitted_ / totalEmissionTime_;
}

size_t ParticleTuner::getActiveParticleCount() const {
    size_t count = 0;
    for (const auto& particle : particles_) {
        if (particle.active) count++;
    }
    return count;
}

} // namespace BloodEditor