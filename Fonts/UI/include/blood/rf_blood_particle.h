#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <random>

namespace RFBlood {

// Particle states
enum class ParticleState {
    ACTIVE,
    INACTIVE,
    DYING,
    DEAD,
    COAGULATING
    EVAPORATING
    ABSORBED
};

// Particle types
enum class ParticleType {
    BLOOD,
    WATER,
    GORE,
    BONE,
    TISSUE,
    CUSTOM
};

// Particle render modes
enum class ParticleRenderMode {
    POINT,
    SPHERE,
    BILLBOARD,
    RIBBON,
    TRAIL,
    MESH
};

// Particle structure
struct BloodParticle {
    // Position and motion
    float x, y, z;
    float vx, vy, vz;
    float ax, ay, az;
    
    // Physical properties
    float radius;
    float mass;
    float density;
    float temperature;
    float viscosity;
    float surfaceTension;
    
    // Visual properties
    std::vector<float> color;
    float alpha;
    float size;
    float rotation;
    float scale;
    
    // Lifetime
    float age;
    float lifetime;
    float maxLifetime;
    ParticleState state;
    
    // Interaction properties
    bool canCollide;
    bool canCoagulate;
    bool canEvaporate;
    bool canAbsorb;
    float coagulationStrength;
    float evaporationRate;
    float absorptionRate;
    
    // Rendering
    ParticleRenderMode renderMode;
    uint32_t textureID;
    std::vector<float> uvCoords;
    
    // Identification
    uint32_t id;
    ParticleType type;
    std::string name;
    
    // Custom data
    std::map<std::string, std::any> userData;
    std::function<void()> onUpdate;
    std::function<void()> onDeath;
    
    BloodParticle() : x(0.0f), y(0.0f), z(0.0f), vx(0.0f), vy(0.0f), vz(0.0f), 
                   ax(0.0f), ay(0.0f), az(0.0f), radius(1.0f), mass(1.0f), 
                   density(1.0f), temperature(37.0f), viscosity(1.0f), 
                   surfaceTension(0.072f), color{1.0f, 0.0f, 0.0f, 1.0f}, 
                   alpha(1.0f), size(1.0f), rotation(0.0f), scale(1.0f), 
                   age(0.0f), lifetime(5.0f), maxLifetime(5.0f), 
                   state(ParticleState::ACTIVE), canCollide(true), canCoagulate(true), 
                   canEvaporate(true), canAbsorb(false), coagulationStrength(1.0f), 
                   evaporationRate(0.1f), absorptionRate(0.0f), renderMode(ParticleRenderMode::POINT), 
                   textureID(0), uvCoords{0.0f, 0.0f}, id(0), type(ParticleType::BLOOD), 
                   name(""), onUpdate(nullptr), onDeath(nullptr) {}
};

// Particle emitter
class BloodParticle {
public:
    BloodParticle();
    virtual ~BloodParticle() = default;
    
    // Emission
    virtual void emit(const std::vector<BloodParticle>& particles) = 0;
    virtual void emitBurst(const std::vector<BloodParticle>& particles, float spread = 1.0f) = 0;
    virtual void emitFountain(const std::vector<BloodParticle>& particles, float rate, float duration) = 0;
    virtual void emitContinuous(const std::vector<BloodParticle>& particles, float rate) = 0;
    
    // Particle management
    virtual void updateParticles(float deltaTime) = 0;
    virtual void renderParticles() const = 0;
    virtual void cleanupParticles() = 0;
    
    // Configuration
    virtual void setMaxParticles(int maxParticles) = 0;
    virtual void setEmissionRate(float rate) = 0;
    virtual void setEmissionShape(const std::string& shape) = 0;
    virtual void setParticleProperties(const BloodParticle& templateParticle) = 0;
    
    // State queries
    virtual size_t getParticleCount() const = 0;
    virtual size_t getActiveParticleCount() const = 0;
    virtual const std::vector<BloodParticle>& getParticles() const = 0;
    virtual const std::vector<BloodParticle>& getActiveParticles() const = 0;
    
    // Event handling
    virtual void addParticleEventListener(const std::string& eventType, 
                                      std::function<void()> callback) = 0;
    virtual void removeParticleEventListener(const std::string& eventType, 
                                         std::function<void()> callback) = 0;
    virtual void clearParticleEventListeners() = 0;
    
    // Utility methods
    virtual void cloneFrom(const BloodParticle& other) = 0;
    virtual std::unique_ptr<BloodParticle> clone() const = 0;
    
protected:
    // Protected members
    std::vector<BloodParticle> particles_;
    std::vector<BloodParticle> activeParticles_;
    std::vector<BloodParticle> inactiveParticles_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    int maxParticles_;
    float emissionRate_;
    std::string emissionShape_;
    BloodParticle templateParticle_;
    
    // Protected helper methods
    virtual void updateParticlePhysics(BloodParticle& particle, float deltaTime);
    virtual void updateParticleLifetime(BloodParticle& particle, float deltaTime);
    virtual void updateParticleState(BloodParticle& particle);
    virtual void triggerParticleEvent(const std::string& eventType, 
                                       const BloodParticle& particle);
    virtual void optimizeParticleMemory();
    
    // Physics helpers
    virtual void applyGravity(BloodParticle& particle, float deltaTime);
    virtual void applyWind(BloodParticle& particle, float deltaTime);
    virtual void applyForces(BloodParticle& particle, float deltaTime);
    virtual bool checkParticleCollision(const BloodParticle& a, const BloodParticle& b);
    virtual void resolveCollision(BloodParticle& a, BloodParticle& b);
};

// Specialized particle emitters
class BloodDropletEmitter : public BloodParticle {
public:
    BloodDropletEmitter();
    void emit(const std::vector<BloodParticle>& particles) override;
    void emitBurst(const std::vector<BloodParticle>& particles, float spread) override;
};

class BloodSprayEmitter : public BloodParticle {
public:
    BloodSprayEmitter();
    void emit(const std::vector<BloodParticle>& particles) override;
    void emitBurst(const std::vector<BloodParticle>& particles, float spread) override;
};

class BloodGushEmitter : public BloodParticle {
public:
    BloodGushEmitter();
    void emit(const std::vector<BloodParticle>& particles) override;
    void emitFountain(const std::vector<BloodParticle>& particles, float rate, float duration) override;
};

class BloodStreamEmitter : public BloodParticle {
public:
    BloodStreamEmitter();
    void emitContinuous(const std::vector<BloodParticle>& particles, float rate) override;
};

class BloodExplosionEmitter : public BloodParticle {
public:
    BloodExplosionEmitter();
    void emitBurst(const std::vector<BloodParticle>& particles, float spread) override;
};

// Particle factory
class BloodParticleFactory {
public:
    static std::unique_ptr<BloodParticle> createEmitter(ParticleType type);
    static std::unique_ptr<BloodDropletEmitter> createDropletEmitter();
    static std::unique_ptr<BloodSprayEmitter> createSprayEmitter();
    static std::unique_ptr<BloodGushEmitter> createGushEmitter();
    static std::unique_ptr<BloodStreamEmitter> createStreamEmitter();
    static std::unique_ptr<BloodExplosionEmitter> createExplosionEmitter();
    static std::vector<ParticleType> getAvailableParticleTypes();
    static BloodParticle createParticleTemplate(ParticleType type);
};

} // namespace RFBlood