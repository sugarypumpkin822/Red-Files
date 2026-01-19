#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <cmath>

namespace RFBlood {

// Spatter types
enum class SpatterType {
    WATER_SPATTER,
    BLOOD_SPATTER,
    GORE_SPATTER,
    SLIME_SPATTER,
    ACID_SPATTER,
    CUSTOM
};

// Spatter patterns
enum class SpatterPattern {
    RANDOM,
    CIRCULAR,
    RADIAL,
    SPIRAL,
    GRID,
    CUSTOM
};

// Spatter states
enum class SpatterState {
    FORMING,
    ACTIVE,
    FADING,
    DRYING,
    SOLIDIFIED,
    COMPLETED
};

// Spatter properties
struct SpatterProperties {
    float initialVolume;
    float minVolume;
    float maxVolume;
    float spreadRadius;
    float spreadSpeed;
    float dropSize;
    float dropSpeed;
    float dropAngle;
    float dropCount;
    float viscosity;
    float evaporationRate;
    float absorptionRate;
    bool enablePhysics;
    bool enableEvaporation;
    bool enableAbsorption;
    bool enableGravity;
    SpatterType type;
    SpatterPattern pattern;
    std::map<std::string, std::any> customProperties;
    
    SpatterProperties() : initialVolume(5.0f), minVolume(0.1f), maxVolume(20.0f), 
                         spreadRadius(5.0f), spreadSpeed(1.0f), dropSize(0.5f), 
                         dropSpeed(9.81f), dropAngle(45.0f), dropCount(20), 
                         viscosity(1.0f), evaporationRate(0.1f), absorptionRate(0.0f), 
                         enablePhysics(true), enableEvaporation(true), enableAbsorption(false), 
                         enableGravity(true), type(SpatterType::BLOOD_SPATTER), 
                         pattern(SpatterPattern::RANDOM) {}
};

// Spatter droplet
struct SpatterDroplet {
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
    uint32_t id;
    
    SpatterDroplet() : x(0.0f), y(0.0f), z(0.0f), vx(0.0f), vy(0.0f), vz(0.0f), 
                    radius(0.5f), mass(0.1f), age(0.0f), lifetime(5.0f), 
                    temperature(20.0f), viscosity(1.0f), evaporationRate(0.1f), 
                    color{1.0f, 0.0f, 0.0f, 1.0f}, alpha(1.0f), isActive(false), id(0) {}
    SpatterDroplet(float x_, float y_, float z_, float vx_, float vy_, float vz_, 
                float radius_, float mass_, float age_, float lifetime_, 
                float temp_, float viscosity_, float evapRate_, 
                const std::vector<float>& color_, float alpha_, bool active, uint32_t id_)
        : x(x_), y(y_), z(z_), vx(vx_), vy(vy_), vz(vz_), radius(radius_), mass(mass_), 
          age(age_), lifetime(lifetime_), temperature(temp_), viscosity(viscosity_), 
          evaporationRate(evapRate_), color(color_), alpha(alpha_), isActive(active), id(id_) {}
};

// Spatter system
class BloodSpatter {
public:
    BloodSpatter();
    virtual ~BloodSpatter() = default;
    
    // Spatter management
    void initialize(const SpatterProperties& properties);
    void update(float deltaTime);
    void reset();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // Spatter control
    void startSpattering(float duration);
    void stopSpattering();
    void setSpatterType(SpatterType type);
    void setSpatterPattern(SpatterPattern pattern);
    void setSpreadRadius(float radius);
    void setSpreadSpeed(float speed);
    void setDropSize(float size);
    void setDropSpeed(float speed);
    void setDropAngle(float angle);
    void setDropCount(int count);
    
    // Spatter properties
    void setInitialVolume(float volume);
    void setVolumeRange(float minVolume, float maxVolume);
    void setViscosity(float viscosity);
    void setEvaporationRate(float rate);
    void setAbsorptionRate(float rate);
    void setGravityEnabled(bool enabled);
    void setPhysicsEnabled(bool enabled);
    
    // Spatter queries
    bool isSpattering() const;
    SpatterType getSpatterType() const;
    SpatterPattern getSpatterPattern() const;
    const SpatterProperties& getSpatterProperties() const;
    size_t getActiveDropletCount() const;
    const std::vector<SpatterDroplet>& getActiveDroplets() const;
    
    // Pattern generation
    void generateRandomPattern(float centerX, float centerY, float centerZ);
    void generateCircularPattern(float centerX, float centerY, float centerZ);
    void generateRadialPattern(float centerX, float centerY, float centerZ);
    void generateSpiralPattern(float centerX, float centerY, float centerZ);
    void generateGridPattern(float centerX, float centerY, float centerZ);
    void generateCustomPattern(float centerX, float centerY, float centerZ, 
                            std::function<std::array<float, 3>(int, int)> patternFunction);
    
    // Event handling
    void addSpatterEventListener(const std::string& eventType, std::function<void()> callback);
    void removeSpatterEventListener(const std::string& eventType, std::function<void()> callback);
    void clearSpatterEventListeners();
    
    // Utility methods
    void cloneFrom(const BloodSpatter& other);
    virtual std::unique_ptr<BloodSpatter> clone() const = 0;
    
    // Data access
    const std::vector<SpatterDroplet>& getDroplets() const;
    const std::vector<SpatterDroplet>& getActiveDroplets() const;
    
protected:
    // Protected members
    std::vector<SpatterDroplet> droplets_;
    std::vector<SpatterDroplet> activeDroplets_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    SpatterProperties properties_;
    
    bool isPaused_;
    bool isSpattering_;
    float accumulatedTime_;
    float spatterTimer_;
    float spatterAccumulator_;
    
    std::mt19937_19937 randomGenerator_;
    
    // Protected helper methods
    virtual void updateSpatter(float deltaTime);
    virtual void processSpattering(float deltaTime);
    virtual void emitDroplets(float centerX, float centerY, float centerZ);
    virtual void updateDropletPhysics(SpatterDroplet& droplet, float deltaTime);
    virtual void updateDropletState(SpatterDroplet& droplet, float deltaTime);
    virtual void triggerSpatterEvent(const std::string& eventType, const SpatterDroplet& droplet);
    virtual void updateSpatterTimer(float deltaTime);
    
    // Pattern generation helpers
    virtual std::array<float, 3> generateRandomPosition(float centerX, float centerY, float centerZ);
    virtual std::array<float, 3> generateCircularPosition(float centerX, float centerY, float centerZ, int index, int total);
    virtual std::array<float, 3> generateRadialPosition(float centerX, float centerY, float centerZ, int index, int total);
    virtual std::array<float, 3> generateSpiralPosition(float centerX, float centerY, float centerZ, int index, int total);
    virtual std::array<float, 3> generateGridPosition(float centerX, float centerY, float centerZ, int index, int total);
    virtual std::array<float, 3> generateCustomPosition(float centerX, float centerY, float centerZ, int index, int total);
    
    // Physics helpers
    virtual void applyGravity(SpatterDroplet& droplet, float deltaTime);
    virtual void applyWind(SpatterDroplet& droplet, float deltaTime);
    virtual void applyForces(SpatterDroplet& droplet, float deltaTime);
    virtual void applyViscosity(SpatterDroplet& droplet, float deltaTime);
    
    // Droplet helpers
    virtual SpatterDroplet createDroplet(float x, float y, float z, const SpatterProperties& props);
    virtual float generateDropletVolume();
    virtual float generateDropletSpeed();
    virtual float generateDropletAngle();
    virtual float generateDropletSize();
    virtual std::array<float, 4> generateDropletColor();
    
    // Performance optimization
    void optimizeDropletMemory();
    void removeDeadDroplets();
    void limitActiveDroplets(int maxActive);
};

// Specialized spatter systems
class WaterSpatter : public BloodSpatter {
public:
    WaterSpatter();
    void updateDropletPhysics(SpatterDroplet& droplet, float deltaTime) override;
    SpatterDroplet createDroplet(float x, float y, float z, const SpatterProperties& props) override;
    std::array<float, 4> generateDropletColor() override;
};

class BloodSpatter : public BloodSpatter {
public:
    BloodSpatter();
    void updateDropletPhysics(SpatterDroplet& droplet, float deltaTime) override;
    SpatterDroplet createDroplet(float x, float y, float z, const SpatterProperties& props) override;
    std::array<float, 4> generateDropletColor() override;
};

class GoreSpatter : public BloodSpatter {
public:
    GoreSpatter();
    void updateDropletPhysics(SpatterDroplet& droplet, float deltaTime) override;
    SpatterDroplet createDroplet(float x, float y, float z, const SpatterProperties& props) override;
    std::array<float, 4> generateDropletColor() override;
};

class SlimeSpatter : public BloodSpatter {
public:
    SlimeSpatter();
    void updateDropletPhysics(SpatterDroplet& droplet, float deltaTime) override;
    SpatterDroplet createDroplet(float x, float y, float z, const SpatterProperties& props) override;
    std::array<float, 4> generateDropletColor() override;
};

class AcidSpatter : public BloodSpatter {
public:
    AcidSpatter();
    void updateDropletPhysics(SpatterDroplet& droplet, float deltaTime) override;
    SpatterDroplet createDroplet(float x, float y, float z, const SpatterProperties& props) override;
    std::array<float, 4> generateDropletColor() override;
};

// Spatter factory
class BloodSpatterFactory {
public:
    static std::unique_ptr<BloodSpatter> createSpatter(SpatterType type);
    static std::unique_ptr<WaterSpatter> createWaterSpatter();
    static std::unique_ptr<BloodSpatter> createBloodSpatter();
    static std::unique_ptr<GoreSpatter> createGoreSpatter();
    static std::unique_ptr<SlimeSpatter> createSlimeSpatter();
    static std::unique_ptr<AcidSpatter> createAcidSpatter();
    static std::vector<SpatterType> getAvailableSpatterTypes();
    static std::vector<SpatterPattern> getAvailableSpatterPatterns();
    static SpatterProperties createSpatterProperties(SpatterType type, SpatterPattern pattern = SpatterPattern::RANDOM, 
                                                  float initialVolume = 5.0f, float spreadRadius = 5.0f);
};

} // namespace RFBlood