#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <random>

namespace RFBlood {

// Effect types
enum class EffectType {
    SPLASH,
    SPATTER,
    DRIP,
    STREAM,
    POOL,
    TRAIL,
    GUSH,
    EXPLOSION,
    CUSTOM
};

// Effect states
enum class EffectState {
    INACTIVE,
    STARTING,
    ACTIVE,
    FADING,
    ENDING,
    COMPLETED
};

// Effect properties
struct EffectProperties {
    float intensity;
    float duration;
    float fadeTime;
    float fadeInTime;
    float fadeOutTime;
    float scale;
    float rotation;
    float alpha;
    bool enablePhysics;
    bool enableCollisions;
    bool enableCoagulation;
    bool enableEvaporation;
    bool enableAbsorption;
    EffectType type;
    std::map<std::string, std::any> customProperties;
    
    EffectProperties() : intensity(1.0f), duration(5.0f), fadeTime(1.0f), 
                        fadeInTime(0.5f), fadeOutTime(0.5f), scale(1.0f), 
                        rotation(0.0f), alpha(1.0f), enablePhysics(true), 
                        enableCollisions(true), enableCoagulation(true), 
                        enableEvaporation(true), enableAbsorption(false), 
                        type(EffectType::SPLASH) {}
};

// Effect event
struct EffectEvent {
    enum class Type {
        STARTED,
        STOPPED,
        PAUSED,
        RESUMED,
        COMPLETED,
        FADED_IN,
        FADED_OUT,
        COLLISION,
        COAGULATION,
        EVAPORATION,
        ABSORPTION
    };
    
    Type type;
    float time;
    std::string effectName;
    std::map<std::string, std::any> userData;
    
    EffectEvent(Type t, float tm, const std::string& name = "") 
        : type(t), time(tm), effectName(name) {}
};

// Effect callback type
using EffectCallback = std::function<void(const EffectEvent&)>;

// Blood effect class
class BloodEffect {
public:
    BloodEffect();
    virtual ~BloodEffect() = default;
    
    // Effect management
    void initialize(const EffectProperties& properties);
    void update(float deltaTime);
    void reset();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // Effect control
    void start();
    void stop();
    void pause();
    void resume();
    void restart();
    void setIntensity(float intensity);
    void setDuration(float duration);
    void setFadeTime(float fadeTime);
    void setScale(float scale);
    void setRotation(float rotation);
    void setAlpha(float alpha);
    
    // State queries
    bool isActive() const;
    bool isCompleted() const;
    EffectState getState() const;
    float getCurrentTime() const;
    float getDuration() const;
    float getIntensity() const;
    float getScale() const;
    float getRotation() const;
    float getAlpha() const;
    
    // Event handling
    void addEventListener(const std::string& eventType, EffectCallback callback);
    void removeEventListener(const std::string& eventType, EffectCallback callback);
    void clearEventListeners();
    
    // Utility methods
    void cloneFrom(const BloodEffect& other);
    virtual std::unique_ptr<BloodEffect> clone() const = 0;
    
    // Data access
    const EffectProperties& getProperties() const;
    const std::vector<EffectEvent>& getEvents() const;
    
protected:
    // Protected members
    EffectProperties properties_;
    std::vector<EffectEvent> events_;
    std::map<std::string, std::vector<EffectCallback>> eventListeners_;
    
    float currentTime_;
    float accumulatedTime_;
    EffectState state_;
    bool isPaused_;
    
    std::mt19937_19937 randomGenerator_;
    
    // Protected helper methods
    virtual void updateEffect(float deltaTime);
    virtual void processEffect(float deltaTime);
    virtual void triggerEvent(const EffectEvent& event);
    virtual void updateState(float deltaTime);
    virtual void updateIntensity(float deltaTime);
    virtual void updateScale(float deltaTime);
    virtual void updateRotation(float deltaTime);
    virtual void updateAlpha(float deltaTime);
    
    // State transitions
    virtual void transitionToState(EffectState newState);
    virtual void onStateEnter(EffectState state);
    virtual void onStateExit(EffectState state);
    virtual void onStateUpdate(EffectState state, float deltaTime);
    
    // Effect helpers
    virtual void startEffect();
    virtual void stopEffect();
    virtual void pauseEffect();
    virtual void resumeEffect();
    virtual void restartEffect();
    
    // Performance optimization
    void optimizeEffectMemory();
    void removeCompletedEffects();
    void limitActiveEffects(int maxActive);
};

// Specialized effect classes
class SplashEffect : public BloodEffect {
public:
    SplashEffect();
    void initialize(const EffectProperties& properties) override;
    void updateEffect(float deltaTime) override;
    std::unique_ptr<BloodEffect> clone() const override;
    
    // Splash-specific methods
    void setSplashRadius(float radius);
    void setSplashHeight(float height);
    void setSplashSpread(float spread);
    void setSplashParticles(int count);
    
protected:
    float splashRadius_;
    float splashHeight_;
    float splashSpread_;
    int splashParticles_;
    
    void createSplashParticles();
    void updateSplashParticles(float deltaTime);
};

class SpatterEffect : public BloodEffect {
public:
    SpatterEffect();
    void initialize(const EffectProperties& properties) override;
    void updateEffect(float deltaTime) override;
    std::unique_ptr<BloodEffect> clone() const override;
    
    // Spatter-specific methods
    void setSpatterCount(int count);
    void setSpatterRadius(float radius);
    void setSpatterSpread(float spread);
    void setSpatterPattern(const std::string& pattern);
    
protected:
    int spatterCount_;
    float spatterRadius_;
    float spatterSpread_;
    std::string spatterPattern_;
    
    void createSpatterParticles();
    void updateSpatterParticles(float deltaTime);
};

class DripEffect : public BloodEffect {
public:
    DripEffect();
    void initialize(const EffectProperties& properties) override;
    void updateEffect(float deltaTime) override;
    std::unique_ptr<BloodEffect> clone() const override;
    
    // Drip-specific methods
    void setDripRate(float rate);
    void setDripSpeed(float speed);
    void setDripAngle(float angle);
    void setDripVolume(float volume);
    
protected:
    float dripRate_;
    float dripSpeed_;
    float dripAngle_;
    float dripVolume_;
    
    void createDripParticles();
    void updateDripParticles(float deltaTime);
};

class StreamEffect : public BloodEffect {
public:
    StreamEffect();
    void initialize(const EffectProperties& properties) override;
    void updateEffect(float deltaTime) override;
    std::unique_ptr<BloodEffect> clone() const override;
    
    // Stream-specific methods
    void setStreamWidth(float width);
    void setStreamSpeed(float speed);
    void setStreamLength(float length);
    void setStreamDirection(const std::array<float, 3>& direction);
    
protected:
    float streamWidth_;
    float streamSpeed_;
    float streamLength_;
    std::array<float, 3> streamDirection_;
    
    void createStreamParticles();
    void updateStreamParticles(float deltaTime);
};

class PoolEffect : public BloodEffect {
public:
    PoolEffect();
    void initialize(const EffectProperties& properties) override;
    void updateEffect(float deltaTime) override;
    std::unique_ptr<BloodEffect> clone() const override;
    
    // Pool-specific methods
    void setPoolRadius(float radius);
    void setPoolDepth(float depth);
    void setPoolVolume(float volume);
    void setPoolViscosity(float viscosity);
    
protected:
    float poolRadius_;
    float poolDepth_;
    float poolVolume_;
    float poolViscosity_;
    
    void createPoolParticles();
    void updatePoolParticles(float deltaTime);
};

class TrailEffect : public BloodEffect {
public:
    TrailEffect();
    void initialize(const EffectProperties& properties) override;
    void updateEffect(float deltaTime) override;
    std::unique_ptr<BloodEffect> clone() const override;
    
    // Trail-specific methods
    void setTrailLength(float length);
    void setTrailWidth(float width);
    void setTrailSpeed(float speed);
    void setTrailDirection(const std::array<float, 3>& direction);
    
protected:
    float trailLength_;
    float trailWidth_;
    float trailSpeed_;
    std::array<float, 3> trailDirection_;
    
    void createTrailParticles();
    void updateTrailParticles(float deltaTime);
};

class GushEffect : public BloodEffect {
public:
    GushEffect();
    void initialize(const EffectProperties& properties) override;
    void updateEffect(float deltaTime) override;
    std::unique_ptr<BloodEffect> clone() const override;
    
    // Gush-specific methods
    void setGushVolume(float volume);
    void setGushSpeed(float speed);
    void setGushAngle(float angle);
    void setGushDuration(float duration);
    
protected:
    float gushVolume_;
    float gushSpeed_;
    float gushAngle_;
    float gushDuration_;
    
    void createGushParticles();
    void updateGushParticles(float deltaTime);
};

class ExplosionEffect : public BloodEffect {
public:
    ExplosionEffect();
    void initialize(const EffectProperties& properties) override;
    void updateEffect(float deltaTime) override;
    std::unique_ptr<BloodEffect> clone() const override;
    
    // Explosion-specific methods
    void setExplosionRadius(float radius);
    void setExplosionForce(float force);
    void setExplosionParticles(int count);
    void setExplosionDuration(float duration);
    
protected:
    float explosionRadius_;
    float explosionForce_;
    int explosionParticles_;
    float explosionDuration_;
    
    void createExplosionParticles();
    void updateExplosionParticles(float deltaTime);
};

// Effect factory
class BloodEffectFactory {
public:
    static std::unique_ptr<BloodEffect> createEffect(EffectType type);
    static std::unique_ptr<SplashEffect> createSplashEffect();
    static std::unique_ptr<SpatterEffect> createSpatterEffect();
    static std::unique_ptr<DripEffect> createDripEffect();
    static std::unique_ptr<StreamEffect> createStreamEffect();
    static std::unique_ptr<PoolEffect> createPoolEffect();
    static std::unique_ptr<TrailEffect> createTrailEffect();
    static std::unique_ptr<GushEffect> createGushEffect();
    static std::unique_ptr<ExplosionEffect> createExplosionEffect();
    static std::vector<EffectType> getAvailableEffectTypes();
    static EffectProperties createEffectProperties(EffectType type, float intensity = 1.0f, 
                                                  float duration = 5.0f);
};

} // namespace RFBlood