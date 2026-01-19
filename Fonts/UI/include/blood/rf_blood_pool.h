#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <cmath>

namespace RFBlood {

// Pool types
enum class PoolType {
    WATER_POOL,
    BLOOD_POOL,
    GORE_POOL,
    SLIME_POOL,
    ACID_POOL,
    CUSTOM
};

// Pool states
enum class PoolState {
    FORMING,
    STABLE,
    RIPPLING,
    EVAPORATING,
    ABSORBING,
    SOLIDIFYING,
    DRYING
};

// Pool properties
struct PoolProperties {
    float initialVolume;
    float minVolume;
    float maxVolume;
    float surfaceTension;
    float viscosity;
    float evaporationRate;
    float absorptionRate;
    float rippleSpeed;
    float rippleDamping;
    float rippleAmplitude;
    float temperature;
    float density;
    bool enableRipples;
    bool enableEvaporation;
    bool enableAbsorption;
    bool enableSolidification;
    bool enableDrying;
    PoolType type;
    std::map<std::string, std::any> customProperties;
    
    PoolProperties() : initialVolume(10.0f), minVolume(0.1f), maxVolume(100.0f), 
                     surfaceTension(0.072f), viscosity(1.0f), evaporationRate(0.01f), 
                     absorptionRate(0.0f), rippleSpeed(1.0f), rippleDamping(0.9f), 
                     rippleAmplitude(0.1f), temperature(20.0f), density(1.0f), 
                     enableRipples(true), enableEvaporation(true), enableAbsorption(false), 
                     enableSolidification(false), enableDrying(false), type(PoolType::BLOOD_POOL) {}
};

// Pool ripple
struct PoolRipple {
    float x, y;
    float radius;
    float amplitude;
    float frequency;
    float phase;
    float lifetime;
    float maxLifetime;
    float damping;
    bool isActive;
    
    PoolRipple() : x(0.0f), y(0.0f), radius(0.0f), amplitude(0.0f), frequency(1.0f), 
               phase(0.0f), lifetime(0.0f), maxLifetime(5.0f), damping(0.9f), isActive(false) {}
    PoolRipple(float x_, float y_, float radius_, float amplitude_, float frequency_, 
              float phase_, float lifetime_, float maxLifetime_, float damping_, bool active)
        : x(x_), y(y_), radius(radius_), amplitude(amplitude_), frequency(frequency_), 
          phase(phase_), lifetime(lifetime_), maxLifetime(maxLifetime_), damping(damping_), 
          isActive(active) {}
};

// Pool structure
struct BloodPool {
    // Position and dimensions
    float x, y, z;
    float width, height, depth;
    float radius;
    
    // Physical properties
    float volume;
    float mass;
    float temperature;
    float surfaceTension;
    float viscosity;
    float evaporationRate;
    float absorptionRate;
    float density;
    
    // Visual properties
    std::vector<float> color;
    float alpha;
    float surfaceHeight;
    float surfaceNormalX, surfaceNormalY, surfaceNormalZ;
    
    // Pool behavior
    float rippleSpeed;
    float rippleDamping;
    float rippleAmplitude;
    bool canRipple;
    bool canEvaporate;
    bool canAbsorb;
    bool canSolidify;
    bool canDry;
    
    // State
    PoolState state;
    float age;
    float lifetime;
    float maxLifetime;
    
    // Ripples
    std::vector<PoolRipple> ripples;
    int maxRipples;
    
    // Identification
    uint32_t id;
    PoolType type;
    std::string name;
    
    // Custom data
    std::map<std::string, std::any> userData;
    std::function<void()> onUpdate;
    std::function<void()> onDeath;
    
    BloodPool() : x(0.0f), y(0.0f), z(0.0f), width(1.0f), height(1.0f), depth(1.0f), 
                radius(1.0f), volume(1.0f), mass(1.0f), temperature(20.0f), 
                surfaceTension(0.072f), viscosity(1.0f), evaporationRate(0.01f), 
                absorptionRate(0.0f), density(1.0f), color{1.0f, 0.0f, 0.0f, 1.0f}, 
                alpha(1.0f), surfaceHeight(0.0f), surfaceNormalX(0.0f), surfaceNormalY(1.0f), 
                surfaceNormalZ(0.0f), rippleSpeed(1.0f), rippleDamping(0.9f), rippleAmplitude(0.1f), 
                canRipple(true), canEvaporate(true), canAbsorb(false), canSolidify(false), 
                canDry(false), state(PoolState::FORMING), age(0.0f), lifetime(0.0f), 
                maxLifetime(100.0f), maxRipples(100), id(0), type(PoolType::BLOOD_POOL), 
                name(""), onUpdate(nullptr), onDeath(nullptr) {}
};

// Pool system
class BloodPool {
public:
    BloodPool();
    virtual ~BloodPool() = default;
    
    // Pool management
    void initialize(const PoolProperties& properties);
    void update(float deltaTime);
    void reset();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // Pool control
    void addVolume(float volume);
    void removeVolume(float volume);
    void setVolume(float volume);
    void setDimensions(float width, float height, float depth);
    void setRadius(float radius);
    void setTemperature(float temperature);
    void setSurfaceTension(float tension);
    void setViscosity(float viscosity);
    void setEvaporationRate(float rate);
    void setAbsorptionRate(float rate);
    
    // Ripple control
    void addRipple(float x, float y, float amplitude, float frequency);
    void addRipple(const PoolRipple& ripple);
    void clearRipples();
    void setMaxRipples(int maxRipples);
    void setRippleProperties(float speed, float damping, float amplitude);
    
    // Pool queries
    PoolType getPoolType() const;
    PoolState getPoolState() const;
    const PoolProperties& getPoolProperties() const;
    float getVolume() const;
    float getMass() const;
    float getTemperature() const;
    float getSurfaceHeight() const;
    size_t getRippleCount() const;
    const std::vector<PoolRipple>& getRipples() const;
    
    // Surface queries
    float getSurfaceHeightAt(float x, float y) const;
    std::array<float, 3> getSurfaceNormalAt(float x, float y) const;
    bool isPointInside(float x, float y) const;
    
    // Event handling
    void addPoolEventListener(const std::string& eventType, std::function<void()> callback);
    void removePoolEventListener(const std::string& eventType, std::function<void()> callback);
    void clearPoolEventListeners();
    
    // Utility methods
    void cloneFrom(const BloodPool& other);
    virtual std::unique_ptr<BloodPool> clone() const = 0;
    
    // Data access
    const std::vector<PoolRipple>& getRipples() const;
    
protected:
    // Protected members
    std::vector<PoolRipple> ripples_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    PoolProperties properties_;
    
    bool isPaused_;
    float accumulatedTime_;
    
    std::mt19937_19937 randomGenerator_;
    
    // Protected helper methods
    virtual void updatePool(float deltaTime);
    virtual void updateRipples(float deltaTime);
    virtual void updateSurface(float deltaTime);
    virtual void updateVolume(float deltaTime);
    virtual void updateTemperature(float deltaTime);
    virtual void updateState(float deltaTime);
    virtual void triggerPoolEvent(const std::string& eventType);
    
    // Ripple helpers
    virtual void updateRipple(PoolRipple& ripple, float deltaTime);
    virtual void removeDeadRipples();
    virtual void limitRippleCount();
    virtual float calculateRippleHeight(const PoolRipple& ripple, float x, float y, float time) const;
    virtual std::array<float, 3> calculateSurfaceNormal(float x, float y) const;
    
    // Surface helpers
    virtual void updateSurfaceHeight();
    virtual void updateSurfaceNormals();
    virtual float calculateSurfaceHeightAt(float x, float y) const;
    virtual std::array<float, 3> calculateSurfaceNormalAt(float x, float y) const;
    
    // Volume helpers
    virtual void updateVolumeFromRipples();
    virtual void updateVolumeFromEvaporation(float deltaTime);
    virtual void updateVolumeFromAbsorption(float deltaTime);
    
    // State helpers
    virtual void transitionToState(PoolState newState);
    virtual void onStateEnter(PoolState state);
    virtual void onStateExit(PoolState state);
    virtual void onStateUpdate(PoolState state, float deltaTime);
    
    // Performance optimization
    void optimizeRippleMemory();
    void removeInactiveRipples();
    void limitActiveRipples(int maxActive);
};

// Specialized pool systems
class WaterPool : public BloodPool {
public:
    WaterPool();
    void updatePool(float deltaTime) override;
    void updateRipples(float deltaTime) override;
    std::unique_ptr<BloodPool> clone() const override;
};

class BloodPool : public BloodPool {
public:
    BloodPool();
    void updatePool(float deltaTime) override;
    void updateRipples(float deltaTime) override;
    void updateVolume(float deltaTime) override;
    std::unique_ptr<BloodPool> clone() const override;
};

class GorePool : public BloodPool {
public:
    GorePool();
    void updatePool(float deltaTime) override;
    void updateRipples(float deltaTime) override;
    void updateVolume(float deltaTime) override;
    std::unique_ptr<BloodPool> clone() const override;
};

class SlimePool : public BloodPool {
public:
    SlimePool();
    void updatePool(float deltaTime) override;
    void updateRipples(float deltaTime) override;
    void updateVolume(float deltaTime) override;
    std::unique_ptr<BloodPool> clone() const override;
};

class AcidPool : public BloodPool {
public:
    AcidPool();
    void updatePool(float deltaTime) override;
    void updateRipples(float deltaTime) override;
    void updateVolume(float deltaTime) override;
    std::unique_ptr<BloodPool> clone() const override;
};

// Pool factory
class BloodPoolFactory {
public:
    static std::unique_ptr<BloodPool> createPool(PoolType type);
    static std::unique_ptr<WaterPool> createWaterPool();
    static std::unique_ptr<BloodPool> createBloodPool();
    static std::unique_ptr<GorePool> createGorePool();
    static std::unique_ptr<SlimePool> createSlimePool();
    static std::unique_ptr<AcidPool> createAcidPool();
    static std::vector<PoolType> getAvailablePoolTypes();
    static PoolProperties createPoolProperties(PoolType type, float initialVolume = 10.0f, 
                                             float minVolume = 0.1f, float maxVolume = 100.0f);
};

} // namespace RFBlood