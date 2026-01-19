#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <cmath>

namespace RFBlood {

// Trail types
enum class TrailType {
    WATER_TRAIL,
    BLOOD_TRAIL,
    GORE_TRAIL,
    SLIME_TRAIL,
    ACID_TRAIL,
    CUSTOM
};

// Trail states
enum class TrailState {
    FORMING,
    ACTIVE,
    FADING,
    DRYING,
    SOLIDIFYING,
    COMPLETED
};

// Trail patterns
enum class TrailPattern {
    STRAIGHT,
    CURVED,
    WAVY,
    SPIRAL,
    CUSTOM
};

// Trail properties
struct TrailProperties {
    float initialLength;
    float minLength;
    float maxLength;
    float trailWidth;
    float trailSpeed;
    float trailAngle;
    float dropFrequency;
    float dropSize;
    float dropSpeed;
    float viscosity;
    float evaporationRate;
    float absorptionRate;
    bool enablePhysics;
    bool enableEvaporation;
    bool enableAbsorption;
    bool enableGravity;
    bool enableDripping;
    TrailType type;
    TrailPattern pattern;
    std::map<std::string, std::any> customProperties;
    
    TrailProperties() : initialLength(10.0f), minLength(0.5f), maxLength(50.0f), 
                     trailWidth(1.0f), trailSpeed(5.0f), trailAngle(45.0f), dropFrequency(0.5f), 
                     dropSize(0.3f), dropSpeed(9.81f), viscosity(1.0f), 
                     evaporationRate(0.01f), absorptionRate(0.0f), enablePhysics(true), 
                     enableEvaporation(true), enableAbsorption(false), enableGravity(true), 
                     enableDripping(true), type(TrailType::BLOOD_TRAIL), 
                     pattern(TrailPattern::STRAIGHT) {}
};

// Trail particle
struct TrailParticle {
    float x, y, z;
    float vx, vy, vz;
    float radius;
    float mass;
    float age;
    float lifetime;
    float temperature;
    float viscosity;
    float evaporationRate;
    std::vector<float> color;
    float alpha;
    bool isActive;
    bool isDripping;
    uint32_t id;
    
    TrailParticle() : x(0.0f), y(0.0f), z(0.0f), vx(0.0f), vy(0.0f), vz(0.0f), 
                   radius(0.5f), mass(0.1f), age(0.0f), lifetime(5.0f), 
                   temperature(20.0f), viscosity(1.0f), evaporationRate(0.01f), 
                   color{1.0f, 0.0f, 0.0f, 1.0f}, alpha(1.0f), isActive(false), 
                   isDripping(false), id(0) {}
    TrailParticle(float x_, float y_, float z_, float vx_, float vy_, float vz_, 
                float radius_, float mass_, float age_, float lifetime_, 
                float temp_, float viscosity_, float evapRate_, 
                const std::vector<float>& color_, float alpha_, bool active, 
                bool dripping, uint32_t id_)
        : x(x_), y(y_), z(z_), vx(vx_), vy(vy_), vz(vz_), radius(radius_), mass(mass_), 
          age(age_), lifetime(lifetime_), temperature(temp_), viscosity(viscosity_), 
          evaporationRate(evapRate_), color(color_), alpha(alpha_), isActive(active), 
          isDripping(dripping), id(id_) {}
};

// Trail droplet
struct TrailDroplet {
    float x, y, z;
    float vx, vy, vz;
    float radius;
    float mass;
    float age;
    float lifetime;
    float temperature;
    float evaporationRate;
    std::vector<float> color;
    float alpha;
    bool isActive;
    uint32_t id;
    
    TrailDroplet() : x(0.0f), y(0.0f), z(0.0f), vx(0.0f), vy(0.0f), vz(0.0f), 
                    radius(0.3f), mass(0.05f), age(0.0f), lifetime(5.0f), 
                    temperature(20.0f), evaporationRate(0.1f), 
                    color{1.0f, 0.0f, 0.0f, 1.0f}, alpha(1.0f), isActive(false), id(0) {}
    TrailDroplet(float x_, float y_, float z_, float vx_, float vy_, float vz_, 
                float radius_, float mass_, float age_, float lifetime_, 
                float temp_, float evapRate_, 
                const std::vector<float>& color_, float alpha_, bool active, uint32_t id_)
        : x(x_), y(y_), z(z_), vx(vx_), vy(vy_), vz(vz_), radius(radius_), mass(mass_), 
          age(age_), lifetime(lifetime_), temperature(temp_), evaporationRate(evapRate_), 
          color(color_), alpha(alpha_), isActive(active), id(id_) {}
};

// Trail system
class BloodTrail {
public:
    BloodTrail();
    virtual ~BloodTrail() = default;
    
    // Trail management
    void initialize(const TrailProperties& properties);
    void update(float deltaTime);
    void reset();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // Trail control
    void startTrail(float duration);
    void stopTrail();
    void setTrailType(TrailType type);
    void setTrailPattern(TrailPattern pattern);
    void setTrailLength(float length);
    void setTrailWidth(float width);
    void setTrailSpeed(float speed);
    void setTrailAngle(float angle);
    void setDropFrequency(float frequency);
    void setDropSize(float size);
    void setDropSpeed(float speed);
    
    // Trail properties
    void setInitialLength(float length);
    void setLengthRange(float minLength, float maxLength);
    void setViscosity(float viscosity);
    void setEvaporationRate(float rate);
    void setAbsorptionRate(float rate);
    void setGravityEnabled(bool enabled);
    void setPhysicsEnabled(bool enabled);
    void setDrippingEnabled(bool enabled);
    
    // Trail queries
    bool isTrailing() const;
    TrailType getTrailType() const;
    TrailPattern getTrailPattern() const;
    const TrailProperties& getTrailProperties() const;
    size_t getActiveParticleCount() const;
    size_t getActiveDropletCount() const;
    const std::vector<TrailParticle>& getActiveParticles() const;
    const std::vector<TrailDroplet>& getActiveDroplets() const;
    
    // Pattern generation
    void generateStraightPattern(float startX, float startY, float startZ, float endX, float endY, float endZ);
    void generateCurvedPattern(float startX, float startY, float startZ, float endX, float endY, float endZ, float curvature);
    void generateWavyPattern(float startX, float startY, float startZ, float endX, float endY, float endZ, float amplitude, float frequency);
    void generateSpiralPattern(float centerX, float centerY, float centerZ, float radius, float height, float turns);
    void generateCustomPattern(float startX, float startY, float startZ, float endX, float endY, float endZ, 
                              std::function<std::array<float, 3>(float)> pathFunction);
    
    // Event handling
    void addTrailEventListener(const std::string& eventType, std::function<void()> callback);
    void removeTrailEventListener(const std::string& eventType, std::function<void()> callback);
    void clearTrailEventListeners();
    
    // Utility methods
    void cloneFrom(const BloodTrail& other);
    virtual std::unique_ptr<BloodTrail> clone() const = 0;
    
    // Data access
    const std::vector<TrailParticle>& getParticles() const;
    const std::vector<TrailParticle>& getActiveParticles() const;
    const std::vector<TrailDroplet>& getDroplets() const;
    const std::vector<TrailDroplet>& getActiveDroplets() const;
    
protected:
    // Protected members
    std::vector<TrailParticle> particles_;
    std::vector<TrailParticle> activeParticles_;
    std::vector<TrailDroplet> droplets_;
    std::vector<TrailDroplet> activeDroplets_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    TrailProperties properties_;
    
    bool isPaused_;
    bool isTrailing_;
    float accumulatedTime_;
    float trailTimer_;
    float trailAccumulator_;
    float dropletAccumulator_;
    
    std::mt19937_19937 randomGenerator_;
    
    // Protected helper methods
    virtual void updateTrail(float deltaTime);
    virtual void processTrailing(float deltaTime);
    virtual void emitParticles(float startX, float startY, float startZ, float endX, float endY, float endZ);
    virtual void emitDroplets();
    virtual void updateParticlePhysics(TrailParticle& particle, float deltaTime);
    virtual void updateDropletPhysics(TrailDroplet& droplet, float deltaTime);
    virtual void updateParticleState(TrailParticle& particle, float deltaTime);
    virtual void updateDropletState(TrailDroplet& droplet, float deltaTime);
    virtual void triggerTrailEvent(const std::string& eventType, const TrailParticle& particle);
    virtual void updateTrailTimer(float deltaTime);
    
    // Pattern generation helpers
    virtual std::array<float, 3> generateStraightPosition(float startX, float startY, float startZ, float endX, float endY, float endZ, float t);
    virtual std::array<float, 3> generateCurvedPosition(float startX, float startY, float startZ, float endX, float endY, float endZ, float t, float curvature);
    virtual std::array<float, 3> generateWavyPosition(float startX, float startY, float startZ, float endX, float endY, float endZ, float t, float amplitude, float frequency);
    virtual std::array<float, 3> generateSpiralPosition(float centerX, float centerY, float centerZ, float radius, float height, float t, float turns);
    virtual std::array<float, 3> generateCustomPosition(float startX, float startY, float startZ, float endX, float endY, float endZ, float t);
    
    // Physics helpers
    virtual void applyGravity(TrailParticle& particle, float deltaTime);
    virtual void applyWind(TrailParticle& particle, float deltaTime);
    virtual void applyForces(TrailParticle& particle, float deltaTime);
    virtual void applyViscosity(TrailParticle& particle, float deltaTime);
    virtual void applySurfaceTension(TrailParticle& particle, float deltaTime);
    
    // Particle helpers
    virtual TrailParticle createParticle(float x, float y, float z, const TrailProperties& props);
    virtual TrailDroplet createDroplet(float x, float y, float z, const TrailProperties& props);
    virtual float generateParticleVolume();
    virtual float generateParticleSpeed();
    virtual float generateParticleAngle();
    virtual float generateDropletVolume();
    virtual float generateDropletSpeed();
    virtual std::array<float, 4> generateParticleColor();
    virtual std::array<float, 4> generateDropletColor();
    
    // Trail helpers
    virtual void updateTrailLength(float deltaTime);
    virtual void updateTrailWidth(float deltaTime);
    virtual void updateTrailFlow(float deltaTime);
    virtual void calculateTrailProperties();
    
    // Performance optimization
    void optimizeParticleMemory();
    void optimizeDropletMemory();
    void removeDeadParticles();
    void removeDeadDroplets();
    void limitActiveParticles(int maxActive);
    void limitActiveDroplets(int maxActive);
};

// Specialized trail systems
class WaterTrail : public BloodTrail {
public:
    WaterTrail();
    void updateParticlePhysics(TrailParticle& particle, float deltaTime) override;
    TrailParticle createParticle(float x, float y, float z, const TrailProperties& props) override;
    TrailDroplet createDroplet(float x, float y, float z, const TrailProperties& props) override;
    std::array<float, 4> generateParticleColor() override;
    std::array<float, 4> generateDropletColor() override;
};

class BloodTrail : public BloodTrail {
public:
    BloodTrail();
    void updateParticlePhysics(TrailParticle& particle, float deltaTime) override;
    TrailParticle createParticle(float x, float y, float z, const TrailProperties& props) override;
    TrailDroplet createDroplet(float x, float y, float z, const TrailProperties& props) override;
    std::array<float, 4> generateParticleColor() override;
    std::array<float, 4> generateDropletColor() override;
};

class GoreTrail : public BloodTrail {
public:
    GoreTrail();
    void updateParticlePhysics(TrailParticle& particle, float deltaTime) override;
    TrailParticle createParticle(float x, float y, float z, const TrailProperties& props) override;
    TrailDroplet createDroplet(float x, float y, float z, const TrailProperties& props) override;
    std::array<float, 4> generateParticleColor() override;
    std::array<float, 4> generateDropletColor() override;
};

class SlimeTrail : public BloodTrail {
public:
    SlimeTrail();
    void updateParticlePhysics(TrailParticle& particle, float deltaTime) override;
    TrailParticle createParticle(float x, float y, float z, const TrailProperties& props) override;
    TrailDroplet createDroplet(float x, float y, float z, const TrailProperties& props) override;
    std::array<float, 4> generateParticleColor() override;
    std::array<float, 4> generateDropletColor() override;
};

class AcidTrail : public BloodTrail {
public:
    AcidTrail();
    void updateParticlePhysics(TrailParticle& particle, float deltaTime) override;
    TrailParticle createParticle(float x, float y, float z, const TrailProperties& props) override;
    TrailDroplet createDroplet(float x, float y, float z, const TrailProperties& props) override;
    std::array<float, 4> generateParticleColor() override;
    std::array<float, 4> generateDropletColor() override;
};

// Trail factory
class BloodTrailFactory {
public:
    static std::unique_ptr<BloodTrail> createTrail(TrailType type);
    static std::unique_ptr<WaterTrail> createWaterTrail();
    static std::unique_ptr<BloodTrail> createBloodTrail();
    static std::unique_ptr<GoreTrail> createGoreTrail();
    static std::unique_ptr<SlimeTrail> createSlimeTrail();
    static std::unique_ptr<AcidTrail> createAcidTrail();
    static std::vector<TrailType> getAvailableTrailTypes();
    static std::vector<TrailPattern> getAvailableTrailPatterns();
    static TrailProperties createTrailProperties(TrailType type, TrailPattern pattern = TrailPattern::STRAIGHT, 
                                                float initialLength = 10.0f, float minLength = 0.5f, 
                                                float maxLength = 50.0f);
};

} // namespace RFBlood