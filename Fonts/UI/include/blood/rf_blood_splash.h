#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <cmath>

namespace RFBlood {

// Splash types
enum class SplashType {
    WATER_SPLASH,
    BLOOD_SPLASH,
    GORE_SPLASH,
    SLIME_SPLASH,
    ACID_SPLASH,
    CUSTOM
};

// Splash patterns
enum class SplashPattern {
    CIRCULAR,
    RADIAL,
    SPIRAL,
    EXPLOSIVE,
    FOUNTAIN,
    CUSTOM
};

// Splash states
enum class SplashState {
    FORMING,
    ACTIVE,
    PEAKING,
    FALLING,
    RIPPLE,
    COMPLETED
};

// Splash properties
struct SplashProperties {
    float initialVolume;
    float minVolume;
    float maxVolume;
    float splashHeight;
    float splashRadius;
    float splashSpeed;
    float splashAngle;
    float dropCount;
    float dropSize;
    float dropSpeed;
    float dropSpread;
    float viscosity;
    float evaporationRate;
    float absorptionRate;
    bool enablePhysics;
    bool enableEvaporation;
    bool enableAbsorption;
    bool enableGravity;
    bool enableRipples;
    SplashType type;
    SplashPattern pattern;
    std::map<std::string, std::any> customProperties;
    
    SplashProperties() : initialVolume(10.0f), minVolume(0.1f), maxVolume(50.0f), 
                         splashHeight(5.0f), splashRadius(10.0f), splashSpeed(15.0f), 
                         splashAngle(45.0f), dropCount(50), dropSize(0.5f), dropSpeed(9.81f), 
                         dropSpread(1.0f), viscosity(1.0f), evaporationRate(0.1f), 
                         absorptionRate(0.0f), enablePhysics(true), enableEvaporation(true), 
                         enableAbsorption(false), enableGravity(true), enableRipples(true), 
                         type(SplashType::BLOOD_SPLASH), pattern(SplashPattern::CIRCULAR) {}
};

// Splash droplet
struct SplashDroplet {
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
    bool isRippling;
    uint32_t id;
    
    SplashDroplet() : x(0.0f), y(0.0f), z(0.0f), vx(0.0f), vy(0.0f), vz(0.0f), 
                    radius(0.5f), mass(0.1f), age(0.0f), lifetime(5.0f), 
                    temperature(20.0f), viscosity(1.0f), evaporationRate(0.1f), 
                    color{1.0f, 0.0f, 0.0f, 1.0f}, alpha(1.0f), isActive(false), 
                    isRippling(false), id(0) {}
    SplashDroplet(float x_, float y_, float z_, float vx_, float vy_, float vz_, 
                float radius_, float mass_, float age_, float lifetime_, 
                float temp_, float viscosity_, float evapRate_, 
                const std::vector<float>& color_, float alpha_, bool active, 
                bool rippling, uint32_t id_)
        : x(x_), y(y_), z(z_), vx(vx_), vy(vy_), vz(vz_), radius(radius_), mass(mass_), 
          age(age_), lifetime(lifetime_), temperature(temp_), viscosity(viscosity_), 
          evaporationRate(evapRate_), color(color_), alpha(alpha_), isActive(active), 
          isRippling(rippling), id(id_) {}
};

// Splash ripple
struct SplashRipple {
    float x, y, z;
    float radius;
    float amplitude;
    float frequency;
    float phase;
    float lifetime;
    float maxLifetime;
    float damping;
    bool isActive;
    
    SplashRipple() : x(0.0f), y(0.0f), z(0.0f), radius(0.0f), amplitude(0.0f), frequency(1.0f), 
                  phase(0.0f), lifetime(0.0f), maxLifetime(5.0f), damping(0.9f), isActive(false) {}
    SplashRipple(float x_, float y_, float z_, float radius_, float amplitude_, float frequency_, 
                float phase_, float lifetime_, float maxLifetime_, float damping_, bool active)
        : x(x_), y(y_), z(z_), radius(radius_), amplitude(amplitude_), frequency(frequency_), 
          phase(phase_), lifetime(lifetime_), maxLifetime(maxLifetime_), damping(damping_), 
          isActive(active) {}
};

// Splash system
class BloodSplash {
public:
    BloodSplash();
    virtual ~BloodSplash() = default;
    
    // Splash management
    void initialize(const SplashProperties& properties);
    void update(float deltaTime);
    void reset();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // Splash control
    void startSplash(float duration);
    void stopSplash();
    void setSplashType(SplashType type);
    void setSplashPattern(SplashPattern pattern);
    void setSplashHeight(float height);
    void setSplashRadius(float radius);
    void setSplashSpeed(float speed);
    void setSplashAngle(float angle);
    void setDropCount(int count);
    void setDropSize(float size);
    void setDropSpeed(float speed);
    void setDropSpread(float spread);
    
    // Splash properties
    void setInitialVolume(float volume);
    void setVolumeRange(float minVolume, float maxVolume);
    void setViscosity(float viscosity);
    void setEvaporationRate(float rate);
    void setAbsorptionRate(float rate);
    void setGravityEnabled(bool enabled);
    void setPhysicsEnabled(bool enabled);
    void setRipplesEnabled(bool enabled);
    
    // Splash queries
    bool isSplashing() const;
    SplashType getSplashType() const;
    SplashPattern getSplashPattern() const;
    const SplashProperties& getSplashProperties() const;
    size_t getActiveDropletCount() const;
    size_t getActiveRippleCount() const;
    const std::vector<SplashDroplet>& getActiveDroplets() const;
    const std::vector<SplashRipple>& getActiveRipples() const;
    
    // Pattern generation
    void generateCircularPattern(float centerX, float centerY, float centerZ);
    void generateRadialPattern(float centerX, float centerY, float centerZ);
    void generateSpiralPattern(float centerX, float centerY, float centerZ);
    void generateExplosivePattern(float centerX, float centerY, float centerZ);
    void generateFountainPattern(float centerX, float centerY, float centerZ);
    void generateCustomPattern(float centerX, float centerY, float centerZ, 
                             std::function<std::array<float, 3>(int, int)> patternFunction);
    
    // Ripple control
    void addRipple(float x, float y, float z, float amplitude, float frequency);
    void addRipple(const SplashRipple& ripple);
    void clearRipples();
    void setMaxRipples(int maxRipples);
    void setRippleProperties(float damping, float amplitude);
    
    // Event handling
    void addSplashEventListener(const std::string& eventType, std::function<void()> callback);
    void removeSplashEventListener(const std::string& eventType, std::function<void()> callback);
    void clearSplashEventListeners();
    
    // Utility methods
    void cloneFrom(const BloodSplash& other);
    virtual std::unique_ptr<BloodSplash> clone() const = 0;
    
    // Data access
    const std::vector<SplashDroplet>& getDroplets() const;
    const std::vector<SplashDroplet>& getActiveDroplets() const;
    const std::vector<SplashRipple>& getRipples() const;
    const std::vector<SplashRipple>& getActiveRipples() const;
    
protected:
    // Protected members
    std::vector<SplashDroplet> droplets_;
    std::vector<SplashDroplet> activeDroplets_;
    std::vector<SplashRipple> ripples_;
    std::vector<SplashRipple> activeRipples_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    SplashProperties properties_;
    
    bool isPaused_;
    bool isSplashing_;
    float accumulatedTime_;
    float splashTimer_;
    float splashAccumulator_;
    
    std::mt19937_19937 randomGenerator_;
    
    // Protected helper methods
    virtual void updateSplash(float deltaTime);
    virtual void processSplashing(float deltaTime);
    virtual void emitDroplets(float centerX, float centerY, float centerZ);
    virtual void updateDropletPhysics(SplashDroplet& droplet, float deltaTime);
    virtual void updateDropletState(SplashDroplet& droplet, float deltaTime);
    virtual void updateRipples(float deltaTime);
    virtual void triggerSplashEvent(const std::string& eventType, const SplashDroplet& droplet);
    virtual void updateSplashTimer(float deltaTime);
    
    // Pattern generation helpers
    virtual std::array<float, 3> generateCircularPosition(float centerX, float centerY, float centerZ, int index, int total);
    virtual std::array<float, 3> generateRadialPosition(float centerX, float centerY, float centerZ, int index, int total);
    virtual std::array<float, 3> generateSpiralPosition(float centerX, float centerY, float centerZ, int index, int total);
    virtual std::array<float, 3> generateExplosivePosition(float centerX, float centerY, float centerZ, int index, int total);
    virtual std::array<float, 3> generateFountainPosition(float centerX, float centerY, float centerZ, int index, int total);
    virtual std::array<float, 3> generateCustomPosition(float centerX, float centerY, float centerZ, int index, int total);
    
    // Physics helpers
    virtual void applyGravity(SplashDroplet& droplet, float deltaTime);
    virtual void applyWind(SplashDroplet& droplet, float deltaTime);
    virtual void applyForces(SplashDroplet& droplet, float deltaTime);
    virtual void applyViscosity(SplashDroplet& droplet, float deltaTime);
    virtual void applySurfaceTension(SplashDroplet& droplet, float deltaTime);
    
    // Droplet helpers
    virtual SplashDroplet createDroplet(float x, float y, float z, const SplashProperties& props);
    virtual float generateDropletVolume();
    virtual float generateDropletSpeed();
    virtual float generateDropletAngle();
    virtual float generateDropletSize();
    virtual std::array<float, 4> generateDropletColor();
    
    // Ripple helpers
    virtual void updateRipple(SplashRipple& ripple, float deltaTime);
    virtual void removeDeadRipples();
    virtual void limitRippleCount();
    virtual float calculateRippleHeight(const SplashRipple& ripple, float x, float y, float time) const;
    
    // Performance optimization
    void optimizeDropletMemory();
    void optimizeRippleMemory();
    void removeDeadDroplets();
    void removeDeadRipples();
    void limitActiveDroplets(int maxActive);
    void limitActiveRipples(int maxActive);
};

// Specialized splash systems
class WaterSplash : public BloodSplash {
public:
    WaterSplash();
    void updateDropletPhysics(SplashDroplet& droplet, float deltaTime) override;
    SplashDroplet createDroplet(float x, float y, float z, const SplashProperties& props) override;
    std::array<float, 4> generateDropletColor() override;
};

class BloodSplash : public BloodSplash {
public:
    BloodSplash();
    void updateDropletPhysics(SplashDroplet& droplet, float deltaTime) override;
    SplashDroplet createDroplet(float x, float y, float z, const SplashProperties& props) override;
    std::array<float, 4> generateDropletColor() override;
};

class GoreSplash : public BloodSplash {
public:
    GoreSplash();
    void updateDropletPhysics(SplashDroplet& droplet, float deltaTime) override;
    SplashDroplet createDroplet(float x, float y, float z, const SplashProperties& props) override;
    std::array<float, 4> generateDropletColor() override;
};

class SlimeSplash : public BloodSplash {
public:
    SlimeSplash();
    void updateDropletPhysics(SplashDroplet& droplet, float deltaTime) override;
    SplashDroplet createDroplet(float x, float y, float z, const SplashProperties& props) override;
    std::array<float, 4> generateDropletColor() override;
};

class AcidSplash : public BloodSplash {
public:
    AcidSplash();
    void updateDropletPhysics(SplashDroplet& droplet, float deltaTime) override;
    SplashDroplet createDroplet(float x, float y, float z, const SplashProperties& props) override;
    std::array<float, 4> generateDropletColor() override;
};

// Splash factory
class BloodSplashFactory {
public:
    static std::unique_ptr<BloodSplash> createSplash(SplashType type);
    static std::unique_ptr<WaterSplash> createWaterSplash();
    static std::unique_ptr<BloodSplash> createBloodSplash();
    static std::unique_ptr<GoreSplash> createGoreSplash();
    static std::unique_ptr<SlimeSplash> createSlimeSplash();
    static std::unique_ptr<AcidSplash> createAcidSplash();
    static std::vector<SplashType> getAvailableSplashTypes();
    static std::vector<SplashPattern> getAvailableSplashPatterns();
    static SplashProperties createSplashProperties(SplashType type, SplashPattern pattern = SplashPattern::CIRCULAR, 
                                                  float initialVolume = 10.0f, float splashHeight = 5.0f, 
                                                  float splashRadius = 10.0f);
};

} // namespace RFBlood