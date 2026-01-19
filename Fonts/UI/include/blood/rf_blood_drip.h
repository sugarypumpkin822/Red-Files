#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <cmath>

namespace RFBlood {

// Drip types
enum class DripType {
    WATER_DROP,
    BLOOD_DROP,
    GORE_DROP,
    SLIME_DROP,
    ACID_DROP,
    CUSTOM
};

// Drip states
enum class DripState {
    FORMING,
    FALLING,
    SPLASHING,
    SPREADING,
    ABSORBED,
    EVAPORATED
};

// Drip properties
struct DripProperties {
    float initialVolume;
    float minVolume;
    float maxVolume;
    float surfaceTension;
    float viscosity;
    float evaporationRate;
    float absorptionRate;
    float dripRate;
    float dripSpeed;
    float dripAngle;
    float spreadRadius;
    bool enablePhysics;
    bool enableEvaporation;
    bool enableAbsorption;
    bool enableSplashing;
    DripType type;
    std::map<std::string, std::any> customProperties;
    
    DripProperties() : initialVolume(1.0f), minVolume(0.1f), maxVolume(5.0f), 
                     surfaceTension(0.072f), viscosity(1.0f), evaporationRate(0.1f), 
                     absorptionRate(0.0f), dripRate(1.0f), dripSpeed(9.81f), 
                     dripAngle(45.0f), spreadRadius(1.0f), enablePhysics(true), 
                     enableEvaporation(true), enableAbsorption(false), 
                     enableSplashing(true), type(DripType::BLOOD_DROP) {}
};

// Drip structure
struct BloodDrip {
    // Position and motion
    float x, y, z;
    float vx, vy, vz;
    float ax, ay, az;
    
    // Physical properties
    float volume;
    float mass;
    float temperature;
    float surfaceTension;
    float viscosity;
    float evaporationRate;
    float absorptionRate;
    
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
    DripState state;
    
    // Drip behavior
    float dripRate;
    float dripSpeed;
    float dripAngle;
    float spreadRadius;
    bool canSplash;
    bool canSpread;
    bool canEvaporate;
    bool canAbsorb;
    
    // Identification
    uint32_t id;
    DripType type;
    std::string name;
    
    // Custom data
    std::map<std::string, std::any> userData;
    std::function<void()> onUpdate;
    std::function<void()> onDeath;
    
    BloodDrip() : x(0.0f), y(0.0f), z(0.0f), vx(0.0f), vy(0.0f), vz(0.0f), 
                   ax(0.0f), ay(0.0f), az(0.0f), volume(1.0f), mass(1.0f), 
                   temperature(20.0f), surfaceTension(0.072f), viscosity(1.0f), 
                   evaporationRate(0.1f), absorptionRate(0.0f), dripRate(1.0f), 
                   dripSpeed(9.81f), dripAngle(45.0f), spreadRadius(1.0f), 
                   canSplash(true), canSpread(true), canEvaporate(true), canAbsorb(false), 
                   id(0), type(DripType::BLOOD_DROP), name(""), onUpdate(nullptr), onDeath(nullptr) {}
    BloodDrip(float x_, float y_, float z_, float vx_, float vy_, float vz_, 
               float ax_, float ay_, float az_, float volume_, float mass_, 
               float temp_, float surfaceTension_, float viscosity_, 
               float evapRate_, float absorptionRate_, float dripRate_, 
               float dripSpeed_, float dripAngle_, float spreadRadius_, 
               bool canSplash_, bool canSpread_, bool canEvaporate_, bool canAbsorb_)
        : x(x_), y(y_), z(z_), vx(vx_), vy(vy_), vz(vz_), ax(ax_), ay(ay_), az(az_), 
          volume(volume_), mass(mass_), temperature(temp_), surfaceTension(surfaceTension_), 
          viscosity(viscosity_), evaporationRate(evapRate_), absorptionRate(absorptionRate), 
          dripRate(dripRate_), dripSpeed(dripSpeed_), dripAngle(dripAngle_), 
          spreadRadius(spreadRadius_), canSplash(canSplash_), canSpread(canSpread_), 
          canEvaporate(canEvaporate_), canAbsorb(canAbsorb_), 
          id(0), type(DripType::BLOOD_DROP), name(""), onUpdate(nullptr), onDeath(nullptr) {}
};

// Drip emitter
class BloodDrip {
public:
    BloodDrip();
    virtual ~BloodDrip() = default;
    
    // Drip management
    void initialize(const DripProperties& properties);
    void update(float deltaTime);
    void reset();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // Drip control
    void startDripping(float duration);
    void stopDripping();
    void setDripRate(float rate);
    void setDripSpeed(float speed);
    void setDripAngle(float angle);
    void setSpreadRadius(float radius);
    
    // Drip properties
    void setDripType(DripType type);
    void setInitialVolume(float volume);
    void setVolumeRange(float minVolume, float maxVolume);
    void setSurfaceTension(float tension);
    void setViscosity(float viscosity);
    void setEvaporationRate(float rate);
    void setAbsorptionRate(float rate);
    
    // Drip queries
    bool isDripping() const;
    DripType getDripType() const;
    const DripProperties& getDripProperties() const;
    size_t getActiveDripCount() const;
    const std::vector<BloodDrip>& getActiveDrips() const;
    
    // Event handling
    void addDripEventListener(const std::string& eventType, std::function<void()> callback);
    void removeDripEventListener(const std::string& eventType, std::function<void()> callback);
    void clearDripEventListeners();
    
    // Utility methods
    void cloneFrom(const BloodDrip& other);
    virtual std::unique_ptr<BloodDrip> clone() const = 0;
    
    // Data access
    const std::vector<BloodDrip>& getDrips() const;
    const std::vector<BloodDrip>& getActiveDrips() const;
    
protected:
    // Protected members
    std::vector<BloodDrip> drips_;
    std::vector<BloodDrip> activeDrips_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    DripProperties properties_;
    
    bool isPaused_;
    float accumulatedTime_;
    float dripTimer_;
    float dripAccumulator_;
    
    std::mt19937_19937 randomGenerator_;
    
    // Protected helper methods
    virtual void updateDrips(float deltaTime);
    virtual void processDripping(float deltaTime);
    virtual void emitDrip();
    virtual void updateDripPhysics(BloodDrip& drip, float deltaTime);
    virtual void updateDripState(BloodDrip& drip, float deltaTime);
    virtual void triggerDripEvent(const std::string& eventType, const BloodDrip& drip);
    virtual void updateDripTimer(float deltaTime);
    
    // Physics helpers
    virtual void applyGravity(BloodDrip& drip, float deltaTime);
    virtual void applyWind(BloodDrip& drip, float deltaTime);
    virtual void applyForces(BloodDrip& drip, float deltaTime);
    virtual void applySurfaceTension(BloodDrip& drip, float deltaTime);
    virtual void applyViscosity(BloodDrip& drip, float deltaTime);
    
    // Drip helpers
    virtual BloodDrip createDrip(float x, float y, float z, const DripProperties& props);
    virtual float generateDripVolume();
    float generateDripSpeed();
    float generateDripAngle();
    float generateSpreadRadius();
    std::array<float, 4> generateDripColor();
    
    // Performance optimization
    void optimizeDripMemory();
    void removeDeadDrips();
    void limitActiveDrips(int maxActive);
};

// Specialized drip emitters
class WaterDrip : public BloodDrip {
public:
    WaterDrip();
    void updateDripPhysics(BloodDrip& drip, float deltaTime) override;
    BloodDrip createDrip(float x, float y, float z, const DripProperties& props) override;
    std::array<float, 4> generateDripColor() override;
};

class BloodDrip : public BloodDrip {
public:
    BloodDrip();
    void updateDripPhysics(BloodDrip& drip, float deltaTime) override;
    BloodDrip createDrip(float x, float y, float z, const DripProperties& props) override;
    std::array<float, 4> generateDripColor() override;
};

class GoreDrip : public BloodDrip {
public:
    GoreDrip();
    void updateDripPhysics(BloodDrip& drip, float deltaTime) override;
    BloodDrip createDrip(float x, float y, float z, const DripProperties& props) override;
    std::array<float, 4> generateDripColor() override;
};

class SlimeDrip : public BloodDrip {
public:
    SlimeDrip();
    void updateDripPhysics(BloodDrip& drip, float deltaTime) override;
    BloodDrip createDrip(float x, float y, float z, const DripProperties& props) override;
    std::array<float, 4> generateDripColor() override;
};

class AcidDrip : public BloodDrip {
public:
    AcidDrip();
    void updateDripPhysics(BloodDrip& drip, float deltaTime) override;
    BloodDrip createDrip(float x, float y, float z, const DripProperties& props) override;
    std::array<float, 4> generateDripColor() override;
};

// Drip factory
class BloodDripFactory {
public:
    static std::unique_ptr<BloodDrip> createDrip(DripType type);
    static std::unique_ptr<WaterDrip> createWaterDrip();
    static std::unique_ptr<BloodDrip> createBloodDrip();
    static std::unique_ptr<GoreDrip> createGoreDrip();
    static std::unique_ptr<SlimeDrip> createSlimeDrip();
    static std::unique_ptr<AcidDrip> createAcidDrip();
    static std::vector<DripType> getAvailableDripTypes();
    static DripProperties createDripProperties(DripType type, float initialVolume = 1.0f, 
                                                 float minVolume = 0.1f, float maxVolume = 5.0f);
};

} // namespace RFBlood