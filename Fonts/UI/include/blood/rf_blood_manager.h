#pragma once

#include "rf_blood_system.h"
#include "rf_blood_particle.h"
#include "rf_blood_emitter.h"
#include "rf_blood_coagulation.h"
#include "rf_blood_collision.h"
#include "rf_blood_drip.h"
#include "rf_blood_effect.h"
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace RFBlood {

// Manager types
enum class ManagerType {
    SYSTEM,
    PARTICLE,
    EMITTER,
    COAGULATION,
    COLLISION,
    DRIP,
    EFFECT,
    COMPOSITE
};

// Manager states
enum class ManagerState {
    IDLE,
    INITIALIZING,
    RUNNING,
    PAUSED,
    STOPPING,
    STOPPED,
    ERROR
};

// Manager configuration
struct ManagerConfig {
    int maxParticles;
    float maxMemoryUsage;
    float maxUpdateTime;
    bool enablePerformanceMonitoring;
    bool enableMemoryMonitoring;
    bool enableEventLogging;
    bool enableDebugMode;
    std::map<std::string, std::any> customSettings;
    
    ManagerConfig() : maxParticles(10000), maxMemoryUsage(100.0f), maxUpdateTime(16.67f), 
                     enablePerformanceMonitoring(true), enableMemoryMonitoring(true), 
                     enableEventLogging(false), enableDebugMode(false) {}
};

// Manager event
struct ManagerEvent {
    enum class Type {
        INITIALIZED,
        STARTED,
        STOPPED,
        PAUSED,
        RESUMED,
        ERROR,
        PERFORMANCE_WARNING,
        MEMORY_WARNING,
        PARTICLE_LIMIT_REACHED,
        UPDATE_TIME_EXCEEDED
    };
    
    Type type;
    float time;
    std::string managerName;
    std::map<std::string, std::any> userData;
    
    ManagerEvent(Type t, float tm, const std::string& name = "") 
        : type(t), time(tm), managerName(name) {}
};

// Manager callback type
using ManagerCallback = std::function<void(const ManagerEvent&)>;

// Blood manager class
class BloodManager {
public:
    BloodManager();
    ~BloodManager();
    
    // Manager management
    void initialize(const ManagerConfig& config);
    void update(float deltaTime);
    void reset();
    void shutdown();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // System management
    void addSystem(std::shared_ptr<BloodSystem> system);
    void removeSystem(const std::string& name);
    std::shared_ptr<BloodSystem> getSystem(const std::string& name) const;
    bool hasSystem(const std::string& name) const;
    const std::vector<std::string>& getSystemNames() const;
    
    // Particle management
    void addParticleEmitter(std::shared_ptr<BloodParticle> emitter);
    void removeParticleEmitter(const std::string& name);
    std::shared_ptr<BloodParticle> getParticleEmitter(const std::string& name) const;
    bool hasParticleEmitter(const std::string& name) const;
    const std::vector<std::string>& getParticleEmitterNames() const;
    
    // Emitter management
    void addBloodEmitter(std::shared_ptr<BloodEmitter> emitter);
    void removeBloodEmitter(const std::string& name);
    std::shared_ptr<BloodEmitter> getBloodEmitter(const std::string& name) const;
    bool hasBloodEmitter(const std::string& name) const;
    const std::vector<std::string>& getBloodEmitterNames() const;
    
    // Coagulation management
    void addCoagulation(std::shared_ptr<BloodCoagulation> coagulation);
    void removeCoagulation(const std::string& name);
    std::shared_ptr<BloodCoagulation> getCoagulation(const std::string& name) const;
    bool hasCoagulation(const std::string& name) const;
    const std::vector<std::string>& getCoagulationNames() const;
    
    // Collision management
    void addCollision(std::shared_ptr<BloodCollision> collision);
    void removeCollision(const std::string& name);
    std::shared_ptr<BloodCollision> getCollision(const std::string& name) const;
    bool hasCollision(const std::string& name) const;
    const std::vector<std::string>& getCollisionNames() const;
    
    // Drip management
    void addDrip(std::shared_ptr<BloodDrip> drip);
    void removeDrip(const std::string& name);
    std::shared_ptr<BloodDrip> getDrip(const std::string& name) const;
    bool hasDrip(const std::string& name) const;
    const std::vector<std::string>& getDripNames() const;
    
    // Effect management
    void addEffect(std::shared_ptr<BloodEffect> effect);
    void removeEffect(const std::string& name);
    std::shared_ptr<BloodEffect> getEffect(const std::string& name) const;
    bool hasEffect(const std::string& name) const;
    const std::vector<std::string>& getEffectNames() const;
    
    // Global control
    void startAll();
    void stopAll();
    void pauseAll();
    void resumeAll();
    void resetAll();
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enabled);
    void enableMemoryMonitoring(bool enabled);
    void enableEventLogging(bool enabled);
    void enableDebugMode(bool enabled);
    
    // Performance queries
    float getUpdateTime() const;
    float getMemoryUsage() const;
    size_t getTotalParticleCount() const;
    size_t getActiveParticleCount() const;
    size_t getTotalSystemCount() const;
    size_t getActiveSystemCount() const;
    
    // Configuration
    void setMaxParticles(int maxParticles);
    void setMaxMemoryUsage(float maxMemoryUsage);
    void setMaxUpdateTime(float maxUpdateTime);
    void setGlobalTimeScale(float timeScale);
    
    // Event handling
    void addManagerEventListener(const std::string& eventType, ManagerCallback callback);
    void removeManagerEventListener(const std::string& eventType, ManagerCallback callback);
    void clearManagerEventListeners();
    
    // Utility methods
    void cloneFrom(const BloodManager& other);
    std::unique_ptr<BloodManager> clone() const;
    
    // Data access
    const ManagerConfig& getConfig() const;
    const std::map<std::string, std::shared_ptr<BloodSystem>>& getSystems() const;
    const std::map<std::string, std::shared_ptr<BloodParticle>>& getParticleEmitters() const;
    const std::map<std::string, std::shared_ptr<BloodEmitter>>& getBloodEmitters() const;
    const std::map<std::string, std::shared_ptr<BloodCoagulation>>& getCoagulations() const;
    const std::map<std::string, std::shared_ptr<BloodCollision>>& getCollisions() const;
    const std::map<std::string, std::shared_ptr<BloodDrip>>& getDrips() const;
    const std::map<std::string, std::shared_ptr<BloodEffect>>& getEffects() const;
    
protected:
    // Protected members
    ManagerConfig config_;
    std::map<std::string, std::vector<ManagerCallback>> eventListeners_;
    
    std::map<std::string, std::shared_ptr<BloodSystem>> systems_;
    std::map<std::string, std::shared_ptr<BloodParticle>> particleEmitters_;
    std::map<std::string, std::shared_ptr<BloodEmitter>> bloodEmitters_;
    std::map<std::string, std::shared_ptr<BloodCoagulation>> coagulations_;
    std::map<std::string, std::shared_ptr<BloodCollision>> collisions_;
    std::map<std::string, std::shared_ptr<BloodDrip>> drips_;
    std::map<std::string, std::shared_ptr<BloodEffect>> effects_;
    
    ManagerState state_;
    bool isPaused_;
    float globalTimeScale_;
    
    // Performance monitoring
    bool performanceMonitoringEnabled_;
    bool memoryMonitoringEnabled_;
    bool eventLoggingEnabled_;
    bool debugModeEnabled_;
    
    float updateTime_;
    float memoryUsage_;
    size_t totalParticleCount_;
    size_t activeParticleCount_;
    size_t totalSystemCount_;
    size_t activeSystemCount_;
    
    // Protected helper methods
    virtual void updateManager(float deltaTime);
    virtual void updateSystems(float deltaTime);
    virtual void updateParticleEmitters(float deltaTime);
    virtual void updateBloodEmitters(float deltaTime);
    virtual void updateCoagulations(float deltaTime);
    virtual void updateCollisions(float deltaTime);
    virtual void updateDrips(float deltaTime);
    virtual void updateEffects(float deltaTime);
    
    virtual void triggerManagerEvent(const ManagerEvent& event);
    virtual void updatePerformanceMetrics(float deltaTime);
    virtual void updateMemoryMetrics();
    virtual void updateParticleCount();
    virtual void updateSystemCount();
    
    // Performance optimization
    virtual void optimizePerformance();
    virtual void limitParticleCount();
    virtual void limitMemoryUsage();
    virtual void limitUpdateTime();
    
    // Helper methods
    virtual void initializeSystems();
    virtual void shutdownSystems();
    virtual void validateConfiguration();
    virtual void logEvent(const ManagerEvent& event);
    virtual void logDebugInfo(const std::string& message);
    
    // State management
    virtual void transitionToState(ManagerState newState);
    virtual void onStateEnter(ManagerState state);
    virtual void onStateExit(ManagerState state);
    virtual void onStateUpdate(ManagerState state, float deltaTime);
};

// Specialized managers
class SystemManager : public BloodManager {
public:
    SystemManager();
    void addSystem(std::shared_ptr<BloodSystem> system);
    void removeSystem(const std::string& name);
    std::unique_ptr<BloodManager> clone() const override;
};

class ParticleManager : public BloodManager {
public:
    ParticleManager();
    void addParticleEmitter(std::shared_ptr<BloodParticle> emitter);
    void removeParticleEmitter(const std::string& name);
    std::unique_ptr<BloodManager> clone() const override;
};

class EffectManager : public BloodManager {
public:
    EffectManager();
    void addEffect(std::shared_ptr<BloodEffect> effect);
    void removeEffect(const std::string& name);
    std::unique_ptr<BloodManager> clone() const override;
};

class CompositeManager : public BloodManager {
public:
    CompositeManager();
    void addSubManager(const std::string& name, std::shared_ptr<BloodManager> manager);
    void removeSubManager(const std::string& name);
    std::shared_ptr<BloodManager> getSubManager(const std::string& name) const;
    std::unique_ptr<BloodManager> clone() const override;
    
protected:
    std::map<std::string, std::shared_ptr<BloodManager>> subManagers_;
    void updateManager(float deltaTime) override;
};

// Manager factory
class BloodManagerFactory {
public:
    static std::unique_ptr<BloodManager> createManager(ManagerType type);
    static std::unique_ptr<SystemManager> createSystemManager();
    static std::unique_ptr<ParticleManager> createParticleManager();
    static std::unique_ptr<EffectManager> createEffectManager();
    static std::unique_ptr<CompositeManager> createCompositeManager();
    static std::vector<ManagerType> getAvailableManagerTypes();
    static ManagerConfig createManagerConfig(ManagerType type, int maxParticles = 10000, 
                                           float maxMemoryUsage = 100.0f);
};

} // namespace RFBlood