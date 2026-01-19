#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <cmath>

namespace RFBlood {

// Updater types
enum class UpdaterType {
    PHYSICS,
    VISUAL,
    LIFETIME,
    COLLISION,
    COAGULATION,
    EVAPORATION,
    ABSORPTION,
    CUSTOM
};

// Updater modes
enum class UpdaterMode {
    CONTINUOUS,
    PERIODIC,
    TRIGGERED,
    CONDITIONAL
};

// Updater states
enum class UpdaterState {
    IDLE,
    UPDATING,
    PAUSED,
    STOPPED,
    COMPLETED
};

// Updater properties
struct UpdaterProperties {
    float updateRate;
    float updateInterval;
    float updateDelay;
    float updateDuration;
    bool enablePhysics;
    bool enableVisual;
    bool enableLifetime;
    bool enableCollisions;
    bool enableCoagulation;
    bool enableEvaporation;
    bool enableAbsorption;
    bool enableRandomness;
    bool enableOptimization;
    UpdaterType type;
    UpdaterMode mode;
    std::map<std::string, std::any> customProperties;
    
    UpdaterProperties() : updateRate(60.0f), updateInterval(1.0f), updateDelay(0.0f), 
                        updateDuration(1.0f), enablePhysics(true), enableVisual(true), 
                        enableLifetime(true), enableCollisions(true), enableCoagulation(true), 
                        enableEvaporation(true), enableAbsorption(false), enableRandomness(false), 
                        enableOptimization(true), type(UpdaterType::PHYSICS), mode(UpdaterMode::CONTINUOUS) {}
};

// Update result
struct UpdateResult {
    bool success;
    float updateTime;
    int updatedParticles;
    int removedParticles;
    int addedParticles;
    std::vector<std::string> errors;
    std::map<std::string, std::any> userData;
    
    UpdateResult() : success(false), updateTime(0.0f), updatedParticles(0), removedParticles(0), addedParticles(0) {}
    UpdateResult(bool succ, float time, int updated, int removed, int added, 
                const std::vector<std::string>& errs, const std::map<std::string, std::any>& data)
        : success(succ), updateTime(time), updatedParticles(updated), removedParticles(removed), 
          addedParticles(added), errors(errs), userData(data) {}
};

// Particle updater
class ParticleUpdater {
public:
    ParticleUpdater();
    virtual ~ParticleUpdater() = default;
    
    // Updater management
    void initialize(const UpdaterProperties& properties);
    void update(float deltaTime);
    void reset();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // Updater control
    void startUpdating(float duration);
    void stopUpdating();
    void setUpdaterType(UpdaterType type);
    void setUpdaterMode(UpdaterMode mode);
    void setUpdateRate(float rate);
    void setUpdateInterval(float interval);
    void setUpdateDelay(float delay);
    void setUpdateDuration(float duration);
    
    // Effect controls
    void setPhysicsEnabled(bool enabled);
    void setVisualEnabled(bool enabled);
    void setLifetimeEnabled(bool enabled);
    void setCollisionsEnabled(bool enabled);
    void setCoagulationEnabled(bool enabled);
    void setEvaporationEnabled(bool enabled);
    void setAbsorptionEnabled(bool enabled);
    void setRandomnessEnabled(bool enabled);
    void setOptimizationEnabled(bool enabled);
    
    // Updater queries
    bool isUpdating() const;
    UpdaterType getUpdaterType() const;
    UpdaterMode getUpdaterMode() const;
    const UpdaterProperties& getUpdaterProperties() const;
    float getUpdateRate() const;
    float getUpdateInterval() const;
    const std::vector<UpdateResult>& getUpdateResults() const;
    const UpdateResult& getLatestResult() const;
    
    // Particle management
    void addParticle(const SpawnedParticle& particle);
    void removeParticle(uint32_t particleId);
    void clearParticles();
    const std::vector<SpawnedParticle>& getParticles() const;
    const std::vector<SpawnedParticle>& getActiveParticles() const;
    
    // Event handling
    void addUpdaterEventListener(const std::string& eventType, std::function<void()> callback);
    void removeUpdaterEventListener(const std::string& eventType, std::function<void()> callback);
    void clearUpdaterEventListeners();
    
    // Utility methods
    void cloneFrom(const ParticleUpdater& other);
    virtual std::unique_ptr<ParticleUpdater> clone() const = 0;
    
    // Data access
    const std::vector<SpawnedParticle>& getParticles() const;
    const std::vector<SpawnedParticle>& getActiveParticles() const;
    const std::vector<UpdateResult>& getUpdateResults() const;
    
protected:
    // Protected members
    std::vector<SpawnedParticle> particles_;
    std::vector<SpawnedParticle> activeParticles_;
    std::vector<UpdateResult> updateResults_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    UpdaterProperties properties_;
    
    bool isPaused_;
    bool isUpdating_;
    float accumulatedTime_;
    float updateTimer_;
    float updateAccumulator_;
    
    std::mt19937_19937 randomGenerator_;
    
    // Protected helper methods
    virtual void updateUpdater(float deltaTime);
    virtual void processUpdating(float deltaTime);
    virtual void updateParticles(float deltaTime);
    virtual void updateParticlePhysics(SpawnedParticle& particle, float deltaTime);
    virtual void updateParticleVisual(SpawnedParticle& particle, float deltaTime);
    virtual void updateParticleLifetime(SpawnedParticle& particle, float deltaTime);
    virtual void updateParticleCollisions(SpawnedParticle& particle, float deltaTime);
    virtual void updateParticleCoagulation(SpawnedParticle& particle, float deltaTime);
    virtual void updateParticleEvaporation(SpawnedParticle& particle, float deltaTime);
    virtual void updateParticleAbsorption(SpawnedParticle& particle, float deltaTime);
    virtual void triggerUpdaterEvent(const std::string& eventType);
    virtual void updateTimer(float deltaTime);
    
    // Update helpers
    virtual UpdateResult createUpdateResult();
    virtual void addUpdateResult(const UpdateResult& result);
    virtual void removeOldResults();
    
    // Physics helpers
    virtual void applyGravity(SpawnedParticle& particle, float deltaTime);
    virtual void applyWind(SpawnedParticle& particle, float deltaTime);
    virtual void applyForces(SpawnedParticle& particle, float deltaTime);
    virtual void applyViscosity(SpawnedParticle& particle, float deltaTime);
    virtual void applySurfaceTension(SpawnedParticle& particle, float deltaTime);
    
    // Visual helpers
    virtual void updateParticleColor(SpawnedParticle& particle, float deltaTime);
    virtual void updateParticleAlpha(SpawnedParticle& particle, float deltaTime);
    virtual void updateParticleSize(SpawnedParticle& particle, float deltaTime);
    virtual void updateParticleRotation(SpawnedParticle& particle, float deltaTime);
    virtual void updateParticleScale(SpawnedParticle& particle, float deltaTime);
    
    // Lifetime helpers
    virtual void updateParticleAge(SpawnedParticle& particle, float deltaTime);
    virtual void checkParticleLifetime(SpawnedParticle& particle);
    virtual void removeDeadParticles();
    
    // Collision helpers
    virtual void checkParticleCollisions(SpawnedParticle& particle);
    virtual void resolveParticleCollision(SpawnedParticle& particle1, SpawnedParticle& particle2);
    
    // Coagulation helpers
    virtual void checkParticleCoagulation(SpawnedParticle& particle);
    virtual void coagulateParticles(SpawnedParticle& particle1, SpawnedParticle& particle2);
    
    // Evaporation helpers
    virtual void updateParticleEvaporation(SpawnedParticle& particle, float deltaTime);
    virtual void checkParticleEvaporation(SpawnedParticle& particle);
    
    // Absorption helpers
    virtual void updateParticleAbsorption(SpawnedParticle& particle, float deltaTime);
    virtual void checkParticleAbsorption(SpawnedParticle& particle);
    
    // Performance optimization
    void optimizeParticleMemory();
    void removeInactiveParticles();
    void limitActiveParticles(int maxActive);
};

// Specialized updater systems
class PhysicsUpdater : public ParticleUpdater {
public:
    PhysicsUpdater();
    void updateParticlePhysics(SpawnedParticle& particle, float deltaTime) override;
    std::unique_ptr<ParticleUpdater> clone() const override;
};

class VisualUpdater : public ParticleUpdater {
public:
    VisualUpdater();
    void updateParticleVisual(SpawnedParticle& particle, float deltaTime) override;
    std::unique_ptr<ParticleUpdater> clone() const override;
};

class LifetimeUpdater : public ParticleUpdater {
public:
    LifetimeUpdater();
    void updateParticleLifetime(SpawnedParticle& particle, float deltaTime) override;
    std::unique_ptr<ParticleUpdater> clone() const override;
};

class CollisionUpdater : public ParticleUpdater {
public:
    CollisionUpdater();
    void updateParticleCollisions(SpawnedParticle& particle, float deltaTime) override;
    std::unique_ptr<ParticleUpdater> clone() const override;
};

class CoagulationUpdater : public ParticleUpdater {
public:
    CoagulationUpdater();
    void updateParticleCoagulation(SpawnedParticle& particle, float deltaTime) override;
    std::unique_ptr<ParticleUpdater> clone() const override;
};

class EvaporationUpdater : public ParticleUpdater {
public:
    EvaporationUpdater();
    void updateParticleEvaporation(SpawnedParticle& particle, float deltaTime) override;
    std::unique_ptr<ParticleUpdater> clone() const override;
};

class AbsorptionUpdater : public ParticleUpdater {
public:
    AbsorptionUpdater();
    void updateParticleAbsorption(SpawnedParticle& particle, float deltaTime) override;
    std::unique_ptr<ParticleUpdater> clone() const override;
};

class CustomUpdater : public ParticleUpdater {
public:
    CustomUpdater();
    void setCustomUpdateFunction(std::function<void(SpawnedParticle&, float)> function);
    void updateParticles(float deltaTime) override;
    std::unique_ptr<ParticleUpdater> clone() const override;
    
protected:
    std::function<void(SpawnedParticle&, float)> customUpdateFunction_;
};

// Updater factory
class ParticleUpdaterFactory {
public:
    static std::unique_ptr<ParticleUpdater> createUpdater(UpdaterType type);
    static std::unique_ptr<PhysicsUpdater> createPhysicsUpdater();
    static std::unique_ptr<VisualUpdater> createVisualUpdater();
    static std::unique_ptr<LifetimeUpdater> createLifetimeUpdater();
    static std::unique_ptr<CollisionUpdater> createCollisionUpdater();
    static std::unique_ptr<CoagulationUpdater> createCoagulationUpdater();
    static std::unique_ptr<EvaporationUpdater> createEvaporationUpdater();
    static std::unique_ptr<AbsorptionUpdater> createAbsorptionUpdater();
    static std::unique_ptr<CustomUpdater> createCustomUpdater();
    static std::vector<UpdaterType> getAvailableUpdaterTypes();
    static std::vector<UpdaterMode> getAvailableUpdaterModes();
    static UpdaterProperties createUpdaterProperties(UpdaterType type, UpdaterMode mode = UpdaterMode::CONTINUOUS, 
                                                   float updateRate = 60.0f, float updateInterval = 1.0f);
};

} // namespace RFBlood