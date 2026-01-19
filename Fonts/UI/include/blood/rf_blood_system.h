#pragma once

#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <random>

namespace RFBlood {

// Blood system types
enum class BloodType {
    WATER,
    BLOOD,
    PLASMA,
    GORE,
    SLIME,
    ACID,
    CUSTOM
};

// Blood properties
struct BloodProperties {
    float viscosity;
    float density;
    float surfaceTension;
    float cohesion;
    float evaporationRate;
    float absorptionRate;
    BloodType type;
    std::map<std::string, std::any> customProperties;
    
    BloodProperties() : viscosity(1.0f), density(1.0f), surfaceTension(0.072f), 
                     cohesion(1.0f), evaporationRate(0.0f), absorptionRate(0.0f), 
                     type(BloodType::BLOOD) {}
};

// Blood particle structure
struct BloodParticle {
    float x, y, z;
    float vx, vy, vz;
    float radius;
    float mass;
    float age;
    float lifetime;
    float temperature;
    BloodProperties properties;
    std::vector<float> color;
    bool isActive;
    uint32_t id;
    
    BloodParticle() : x(0.0f), y(0.0f), z(0.0f), vx(0.0f), vy(0.0f), vz(0.0f), 
                   radius(1.0f), mass(1.0f), age(0.0f), lifetime(5.0f), 
                   temperature(37.0f), isActive(false), id(0) {}
    BloodParticle(float x_, float y_, float z_, float vx_, float vy_, float vz_, 
               float radius_, float mass_, const BloodProperties& props)
        : x(x_), y(y_), z(z_), vx(vx_), vy(vy_), vz(vz_), radius(radius_), mass(mass_), 
          age(0.0f), lifetime(5.0f), temperature(37.0f), properties(props), 
          isActive(true), id(0) {
        color = {1.0f, 0.0f, 0.0f, 1.0f}; // Default blood color
    }
};

// Blood system configuration
struct BloodSystemConfig {
    int maxParticles;
    float gravity;
    float airResistance;
    float windX, windY, windZ;
    float temperature;
    bool enableCollisions;
    bool enableCoagulation;
    bool enableEvaporation;
    bool enableAbsorption;
    float coagulationDistance;
    float evaporationRate;
    float absorptionRate;
    
    BloodSystemConfig() : maxParticles(10000), gravity(9.81f), airResistance(0.1f), 
                         windX(0.0f), windY(0.0f), windZ(0.0f), temperature(20.0f), 
                         enableCollisions(true), enableCoagulation(true), enableEvaporation(true), 
                         enableAbsorption(false), coagulationDistance(2.0f), evaporationRate(0.1f), 
                         absorptionRate(0.0f) {}
};

// Blood system class
class BloodSystem {
public:
    BloodSystem();
    ~BloodSystem();
    
    // System management
    void initialize(const BloodSystemConfig& config);
    void update(float deltaTime);
    void reset();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // Particle management
    void emitParticle(const BloodParticle& particle);
    void emitParticles(const std::vector<BloodParticle>& particles);
    void emitBurst(const std::vector<BloodParticle>& particles, float spread = 1.0f);
    void emitFountain(const std::vector<BloodParticle>& particles, float rate, float duration);
    
    // Blood properties
    void setBloodProperties(const BloodProperties& properties);
    const BloodProperties& getBloodProperties() const;
    void setBloodType(BloodType type);
    BloodType getBloodType() const;
    
    // Environment
    void setGravity(float gravity);
    void setWind(float x, float y, float z);
    void setTemperature(float temperature);
    void setAirResistance(float resistance);
    
    // Particle queries
    size_t getParticleCount() const;
    size_t getActiveParticleCount() const;
    const std::vector<BloodParticle>& getParticles() const;
    const std::vector<BloodParticle>& getActiveParticles() const;
    
    // Performance
    void setMaxParticles(int maxParticles);
    int getMaxParticles() const;
    float getPerformanceMetrics() const;
    void enablePerformanceMonitoring(bool enabled);
    
    // Event handling
    void addSystemEventListener(const std::string& eventType, std::function<void()> callback);
    void removeSystemEventListener(const std::string& eventType, std::function<void()> callback);
    void clearSystemEventListeners();
    
    // Utility methods
    void cloneFrom(const BloodSystem& other);
    std::unique_ptr<BloodSystem> clone() const;
    
    // Data access
    const BloodSystemConfig& getConfig() const;
    const std::vector<BloodParticle>& getParticles() const;
    
protected:
    // Protected members
    std::vector<BloodParticle> particles_;
    std::vector<BloodParticle> activeParticles_;
    std::vector<BloodParticle> inactiveParticles_;
    BloodSystemConfig config_;
    BloodProperties bloodProperties_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    bool isPaused_;
    float accumulatedTime_;
    
    // Performance monitoring
    bool performanceMonitoringEnabled_;
    float updateTime_;
    float particleUpdateTime_;
    float collisionUpdateTime_;
    float renderTime_;
    
    // Protected helper methods
    virtual void updateParticles(float deltaTime);
    virtual void updateParticlePhysics(BloodParticle& particle, float deltaTime);
    virtual void handleCollisions();
    virtual void handleCoagulation();
    virtual void handleEvaporation();
    virtual void handleAbsorption();
    virtual void updateParticleLifetime(BloodParticle& particle, float deltaTime);
    
    // Physics helpers
    void applyGravity(BloodParticle& particle, float deltaTime);
    void applyWind(BloodParticle& particle, float deltaTime);
    void applyAirResistance(BloodParticle& particle, float deltaTime);
    bool checkParticleCollision(const BloodParticle& a, const BloodParticle& b);
    void resolveCollision(BloodParticle& a, BloodParticle& b);
    
    // Coagulation helpers
    bool shouldCoagulate(const BloodParticle& a, const BloodParticle& b);
    void coagulateParticles(BloodParticle& a, BloodParticle& b);
    BloodParticle createCoagulatedParticle(const BloodParticle& a, const BloodParticle& b);
    
    // Performance helpers
    void startPerformanceTimer(const std::string& operation);
    void endPerformanceTimer(const std::string& operation);
    void updatePerformanceMetrics();
    
    // Memory management
    void optimizeParticleMemory();
    void compactParticleArrays();
};

// Blood system factory
class BloodSystemFactory {
public:
    static std::unique_ptr<BloodSystem> createBloodSystem();
    static std::unique_ptr<BloodSystem> createBloodSystem(const BloodSystemConfig& config);
    static BloodProperties createBloodProperties(BloodType type);
    static BloodProperties createCustomBloodProperties(float viscosity, float density, 
                                                     float surfaceTension, float cohesion);
    static std::vector<BloodType> getAvailableBloodTypes();
};

} // namespace RFBlood