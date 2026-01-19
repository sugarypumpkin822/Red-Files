#pragma once

#include "rf_blood_particle.h"
#include <vector>
#include <memory>
#include <functional>
#include <random>

namespace RFBlood {

// Emitter types
enum class EmitterType {
    POINT,
    LINE,
    CIRCLE,
    SPHERE,
    CONE,
    CUSTOM
};

// Emission shapes
enum class EmissionShape {
    POINT,
    LINE,
    CIRCLE,
    SPHERE,
    RING,
    DISC,
    SQUARE,
    TRIANGLE,
    CUSTOM
};

// Emission patterns
enum class EmissionPattern {
    CONSTANT,
    BURST,
    FOUNTAIN,
    STREAM,
    WAVE,
    VORTEX,
    CUSTOM
};

// Emitter configuration
struct EmitterConfig {
    EmitterType type;
    EmissionShape shape;
    EmissionPattern pattern;
    float emissionRate;
    float burstCount;
    float burstSpread;
    float coneAngle;
    float coneRadius;
    bool randomDirection;
    bool randomRotation;
    bool randomSpeed;
    float minSpeed;
    float maxSpeed;
    float minLifetime;
    float maxLifetime;
    float minSize;
    float maxSize;
    float minScale;
    float maxScale;
    
    EmitterConfig() : type(EmitterType::POINT), shape(EmissionShape::POINT), 
                   pattern(EmissionPattern::CONSTANT), emissionRate(10.0f), burstCount(10), 
                   burstSpread(1.0f), coneAngle(45.0f), coneRadius(1.0f), 
                   randomDirection(false), randomRotation(false), randomSpeed(false), 
                   minSpeed(1.0f), maxSpeed(10.0f), minLifetime(1.0f), 
                   maxLifetime(10.0f), minSize(1.0f), maxSize(1.0f), 
                   minScale(1.0f), maxScale(1.0f) {}
};

// Emitter class
class BloodEmitter {
public:
    BloodEmitter();
    virtual ~BloodEmitter() = default;
    
    // Emitter management
    void initialize(const EmitterConfig& config);
    void update(float deltaTime);
    void reset();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // Emission control
    void emit();
    void emitBurst(int count = 1);
    void startFountain(float duration);
    void startStream();
    void stopFountain();
    void stopStream();
    
    // Configuration
    void setEmissionRate(float rate);
    void setEmissionShape(EmissionShape shape);
    void setEmissionPattern(EmissionPattern pattern);
    void setDirection(const std::array<float, 3>& direction);
    void setCone(float angle, float radius);
    void setRandomDirection(bool enabled);
    void setRandomRotation(bool enabled);
    void setSpeedRange(float minSpeed, float maxSpeed);
    void setLifetimeRange(float minLifetime, float maxLifetime);
    void setSizeRange(float minSize, float maxSize);
    void setScaleRange(float minScale, float maxScale);
    
    // Particle template
    void setParticleTemplate(const BloodParticle& particle);
    const BloodParticle& getParticleTemplate() const;
    
    // State queries
    bool isEmitting() const;
    bool isFountainActive() const;
    bool isStreamActive() const;
    float getEmissionTime() const;
    float getEmissionRate() const;
    size_t getEmittedCount() const;
    
    // Event handling
    void addEmitterEventListener(const std::string& eventType, std::function<void()> callback);
    void removeEmitterEventListener(const std::string& eventType, std::function<void()> callback);
    void clearEmitterEventListeners();
    
    // Utility methods
    void cloneFrom(const BloodEmitter& other);
    virtual std::unique_ptr<BloodEmitter> clone() const = 0;
    
    // Data access
    const EmitterConfig& getConfig() const;
    const std::vector<BloodParticle>& getEmittedParticles() const;
    
protected:
    // Protected members
    EmitterConfig config_;
    std::vector<BloodParticle> emittedParticles_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    bool isPaused_;
    float emissionTime_;
    float accumulatedTime_;
    float burstTimer_;
    float fountainTimer_;
    float streamTimer_;
    
    bool isEmitting_;
    bool isFountainActive_;
    bool isStreamActive_;
    size_t totalEmittedCount_;
    
    BloodParticle particleTemplate_;
    std::array<float, 3> emissionDirection_;
    std::mt19937_19937 randomGenerator_;
    
    // Protected helper methods
    virtual void updateEmitter(float deltaTime);
    virtual void processEmission(float deltaTime);
    virtual void emitParticle();
    virtual void emitBurstParticles(int count);
    virtual void emitFountainParticles(float deltaTime);
    virtual void emitStreamParticles(float deltaTime);
    virtual void triggerEmitterEvent(const std::string& eventType);
    virtual void updateEmissionTime(float deltaTime);
    virtual void updateBurstTimer(float deltaTime);
    virtual void updateFountainTimer(float deltaTime);
    virtual void updateStreamTimer(float deltaTime);
    
    // Emission helpers
    virtual std::array<float, 3> generateEmissionDirection();
    virtual float generateEmissionSpeed();
    virtual float generateEmissionLifetime();
    virtual float generateEmissionSize();
    virtual float generateEmissionScale();
    virtual std::array<float, 3> generateEmissionRotation();
    virtual BloodParticle createEmittedParticle();
};

// Specialized emitters
class PointEmitter : public BloodEmitter {
public:
    PointEmitter();
    void emitParticle() override;
};

class LineEmitter : public BloodEmitter {
public:
    LineEmitter();
    void emitParticle() override;
};

class CircleEmitter : public BloodEmitter {
public:
    CircleEmitter();
    void emitParticle() override;
};

class SphereEmitter : public BloodEmitter {
public:
    SphereEmitter();
    void emitParticle() override;
};

class ConeEmitter : public BloodEmitter {
public:
    ConeEmitter();
    void emitParticle() override;
    void setCone(float angle, float radius);
};

class CustomEmitter : public BloodEmitter {
public:
    CustomEmitter();
    void setCustomEmissionFunction(std::function<BloodParticle()> function);
    void emitParticle() override;
};

// Emitter factory
class BloodEmitterFactory {
public:
    static std::unique_ptr<BloodEmitter> createEmitter(EmitterType type);
    static std::unique_ptr<PointEmitter> createPointEmitter();
    static std::unique_ptr<LineEmitter> createLineEmitter();
    static std::unique_ptr<CircleEmitter> createCircleEmitter();
    static std::unique_ptr<SphereEmitter> createSphereEmitter();
    static std::unique_ptr<ConeEmitter> createConeEmitter();
    static std::unique_ptr<CustomEmitter> createCustomEmitter();
    static std::vector<EmitterType> getAvailableEmitterTypes();
    static std::vector<EmissionShape> getAvailableEmissionShapes();
    static std::vector<EmissionPattern> getAvailableEmissionPatterns();
};

} // namespace RFBlood