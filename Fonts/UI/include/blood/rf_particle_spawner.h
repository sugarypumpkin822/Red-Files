#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <cmath>

namespace RFBlood {

// Spawner types
enum class SpawnerType {
    POINT,
    LINE,
    CIRCLE,
    SPHERE,
    CONE,
    CUSTOM
};

// Spawner modes
enum class SpawnerMode {
    INSTANT,
    CONTINUOUS,
    BURST,
    PULSE,
    TRIGGERED
};

// Spawner states
enum class SpawnerState {
    IDLE,
    SPAWNING,
    PAUSED,
    STOPPED,
    COMPLETED
};

// Spawner properties
struct SpawnerProperties {
    float spawnRate;
    float spawnRadius;
    float spawnAngle;
    float spawnSpeed;
    float spawnLifetime;
    float spawnSize;
    float spawnMass;
    float spawnTemperature;
    float spawnViscosity;
    float spawnEvaporationRate;
    float spawnAbsorptionRate;
    bool enablePhysics;
    bool enableCollisions;
    bool enableCoagulation;
    bool enableEvaporation;
    bool enableAbsorption;
    bool enableGravity;
    bool enableWind;
    bool enableRandomRotation;
    bool enableRandomScale;
    SpawnerType type;
    SpawnerMode mode;
    std::map<std::string, std::any> customProperties;
    
    SpawnerProperties() : spawnRate(10.0f), spawnRadius(1.0f), spawnAngle(45.0f), 
                        spawnSpeed(5.0f), spawnLifetime(5.0f), spawnSize(1.0f), 
                        spawnMass(1.0f), spawnTemperature(20.0f), spawnViscosity(1.0f), 
                        spawnEvaporationRate(0.1f), spawnAbsorptionRate(0.0f), 
                        enablePhysics(true), enableCollisions(true), enableCoagulation(true), 
                        enableEvaporation(true), enableAbsorption(false), enableGravity(true), 
                        enableWind(false), enableRandomRotation(false), enableRandomScale(false), 
                        type(SpawnerType::POINT), mode(SpawnerMode::CONTINUOUS) {}
};

// Spawned particle
struct SpawnedParticle {
    float x, y, z;
    float vx, vy, vz;
    float ax, ay, az;
    float radius;
    float mass;
    float age;
    float lifetime;
    float temperature;
    float viscosity;
    float evaporationRate;
    float absorptionRate;
    std::vector<float> color;
    float alpha;
    float rotation;
    float scale;
    bool isActive;
    bool isColliding;
    bool isCoagulating;
    bool isEvaporating;
    bool isAbsorbing;
    uint32_t id;
    
    SpawnedParticle() : x(0.0f), y(0.0f), z(0.0f), vx(0.0f), vy(0.0f), vz(0.0f), 
                     ax(0.0f), ay(0.0f), az(0.0f), radius(1.0f), mass(1.0f), 
                     age(0.0f), lifetime(5.0f), temperature(20.0f), viscosity(1.0f), 
                     evaporationRate(0.1f), absorptionRate(0.0f), color{1.0f, 0.0f, 0.0f, 1.0f}, 
                     alpha(1.0f), rotation(0.0f), scale(1.0f), isActive(false), 
                     isColliding(false), isCoagulating(false), isEvaporating(false), 
                     isAbsorbing(false), id(0) {}
    SpawnedParticle(float x_, float y_, float z_, float vx_, float vy_, float vz_, 
                float ax_, float ay_, float az_, float radius_, float mass_, 
                float age_, float lifetime_, float temp_, float viscosity_, 
                float evapRate_, float absorptionRate_, const std::vector<float>& color_, 
                float alpha_, float rotation_, float scale_, bool active, 
                bool colliding, bool coagulating, bool evaporating, bool absorbing, uint32_t id_)
        : x(x_), y(y_), z(z_), vx(vx_), vy(vy_), vz(vz_), ax(ax_), ay(ay_), az(az_), 
          radius(radius_), mass(mass_), age(age_), lifetime(lifetime_), temperature(temp_), 
          viscosity(viscosity_), evaporationRate(evapRate_), absorptionRate(absorptionRate), 
          color(color_), alpha(alpha_), rotation(rotation_), scale(scale_), isActive(active), 
          isColliding(colliding), isCoagulating(coagulating), isEvaporating(evaporating), 
          isAbsorbing(absorbing), id(id_) {}
};

// Particle spawner
class ParticleSpawner {
public:
    ParticleSpawner();
    virtual ~ParticleSpawner() = default;
    
    // Spawner management
    void initialize(const SpawnerProperties& properties);
    void update(float deltaTime);
    void reset();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // Spawner control
    void startSpawning(float duration);
    void stopSpawning();
    void setSpawnerType(SpawnerType type);
    void setSpawnerMode(SpawnerMode mode);
    void setSpawnRate(float rate);
    void setSpawnRadius(float radius);
    void setSpawnAngle(float angle);
    void setSpawnSpeed(float speed);
    void setSpawnLifetime(float lifetime);
    void setSpawnSize(float size);
    void setSpawnMass(float mass);
    void setSpawnTemperature(float temperature);
    void setSpawnViscosity(float viscosity);
    void setSpawnEvaporationRate(float rate);
    void setSpawnAbsorptionRate(float rate);
    
    // Effect controls
    void setPhysicsEnabled(bool enabled);
    void setCollisionsEnabled(bool enabled);
    void setCoagulationEnabled(bool enabled);
    void setEvaporationEnabled(bool enabled);
    void setAbsorptionEnabled(bool enabled);
    void setGravityEnabled(bool enabled);
    void setWindEnabled(bool enabled);
    void setRandomRotationEnabled(bool enabled);
    void setRandomScaleEnabled(bool enabled);
    
    // Spawner queries
    bool isSpawning() const;
    SpawnerType getSpawnerType() const;
    SpawnerMode getSpawnerMode() const;
    const SpawnerProperties& getSpawnerProperties() const;
    size_t getActiveParticleCount() const;
    const std::vector<SpawnedParticle>& getActiveParticles() const;
    
    // Spawn position generation
    void generatePointSpawn(float x, float y, float z);
    void generateLineSpawn(float startX, float startY, float startZ, float endX, float endY, float endZ);
    void generateCircleSpawn(float centerX, float centerY, float centerZ, float radius);
    void generateSphereSpawn(float centerX, float centerY, float centerZ, float radius);
    void generateConeSpawn(float centerX, float centerY, float centerZ, float angle, float radius);
    void generateCustomSpawn(std::function<std::array<float, 3>()> positionFunction);
    
    // Event handling
    void addSpawnerEventListener(const std::string& eventType, std::function<void()> callback);
    void removeSpawnerEventListener(const std::string& eventType, std::function<void()> callback);
    void clearSpawnerEventListeners();
    
    // Utility methods
    void cloneFrom(const ParticleSpawner& other);
    virtual std::unique_ptr<ParticleSpawner> clone() const = 0;
    
    // Data access
    const std::vector<SpawnedParticle>& getParticles() const;
    const std::vector<SpawnedParticle>& getActiveParticles() const;
    
protected:
    // Protected members
    std::vector<SpawnedParticle> particles_;
    std::vector<SpawnedParticle> activeParticles_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    SpawnerProperties properties_;
    
    bool isPaused_;
    bool isSpawning_;
    float accumulatedTime_;
    float spawnTimer_;
    float spawnAccumulator_;
    
    std::mt19937_19937 randomGenerator_;
    
    // Protected helper methods
    virtual void updateSpawner(float deltaTime);
    virtual void processSpawning(float deltaTime);
    virtual void spawnParticles();
    virtual void updateParticlePhysics(SpawnedParticle& particle, float deltaTime);
    virtual void updateParticleState(SpawnedParticle& particle, float deltaTime);
    virtual void triggerSpawnerEvent(const std::string& eventType, const SpawnedParticle& particle);
    virtual void updateSpawnTimer(float deltaTime);
    
    // Spawn position helpers
    virtual std::array<float, 3> generatePointPosition();
    virtual std::array<float, 3> generateLinePosition();
    virtual std::array<float, 3> generateCirclePosition();
    virtual std::array<float, 3> generateSpherePosition();
    virtual std::array<float, 3> generateConePosition();
    virtual std::array<float, 3> generateCustomPosition();
    
    // Spawn velocity helpers
    virtual std::array<float, 3> generateSpawnVelocity();
    virtual std::array<float, 3> generateSpawnAcceleration();
    
    // Spawn property helpers
    virtual float generateSpawnRadius();
    virtual float generateSpawnLifetime();
    virtual float generateSpawnSize();
    virtual float generateSpawnMass();
    virtual float generateSpawnTemperature();
    virtual float generateSpawnViscosity();
    virtual float generateSpawnEvaporationRate();
    virtual float generateSpawnAbsorptionRate();
    virtual std::array<float, 4> generateSpawnColor();
    virtual float generateSpawnRotation();
    virtual float generateSpawnScale();
    
    // Physics helpers
    virtual void applyGravity(SpawnedParticle& particle, float deltaTime);
    virtual void applyWind(SpawnedParticle& particle, float deltaTime);
    virtual void applyForces(SpawnedParticle& particle, float deltaTime);
    virtual void applyViscosity(SpawnedParticle& particle, float deltaTime);
    virtual void applySurfaceTension(SpawnedParticle& particle, float deltaTime);
    
    // Particle helpers
    virtual SpawnedParticle createParticle(float x, float y, float z, const SpawnerProperties& props);
    virtual void removeDeadParticles();
    virtual void limitActiveParticles(int maxActive);
    
    // Performance optimization
    void optimizeParticleMemory();
    void removeInactiveParticles();
};

// Specialized spawner systems
class PointSpawner : public ParticleSpawner {
public:
    PointSpawner();
    void generatePointSpawn(float x, float y, float z) override;
    std::array<float, 3> generatePointPosition() override;
    std::unique_ptr<ParticleSpawner> clone() const override;
};

class LineSpawner : public ParticleSpawner {
public:
    LineSpawner();
    void generateLineSpawn(float startX, float startY, float startZ, float endX, float endY, float endZ) override;
    std::array<float, 3> generateLinePosition() override;
    std::unique_ptr<ParticleSpawner> clone() const override;
};

class CircleSpawner : public ParticleSpawner {
public:
    CircleSpawner();
    void generateCircleSpawn(float centerX, float centerY, float centerZ, float radius) override;
    std::array<float, 3> generateCirclePosition() override;
    std::unique_ptr<ParticleSpawner> clone() const override;
};

class SphereSpawner : public ParticleSpawner {
public:
    SphereSpawner();
    void generateSphereSpawn(float centerX, float centerY, float centerZ, float radius) override;
    std::array<float, 3> generateSpherePosition() override;
    std::unique_ptr<ParticleSpawner> clone() const override;
};

class ConeSpawner : public ParticleSpawner {
public:
    ConeSpawner();
    void generateConeSpawn(float centerX, float centerY, float centerZ, float angle, float radius) override;
    std::array<float, 3> generateConePosition() override;
    std::unique_ptr<ParticleSpawner> clone() const override;
};

class CustomSpawner : public ParticleSpawner {
public:
    CustomSpawner();
    void setCustomSpawnFunction(std::function<std::array<float, 3>()> positionFunction);
    void generateCustomSpawn(std::function<std::array<float, 3>()> positionFunction) override;
    std::array<float, 3> generateCustomPosition() override;
    std::unique_ptr<ParticleSpawner> clone() const override;
    
protected:
    std::function<std::array<float, 3>()> customSpawnFunction_;
};

// Spawner factory
class ParticleSpawnerFactory {
public:
    static std::unique_ptr<ParticleSpawner> createSpawner(SpawnerType type);
    static std::unique_ptr<PointSpawner> createPointSpawner();
    static std::unique_ptr<LineSpawner> createLineSpawner();
    static std::unique_ptr<CircleSpawner> createCircleSpawner();
    static std::unique_ptr<SphereSpawner> createSphereSpawner();
    static std::unique_ptr<ConeSpawner> createConeSpawner();
    static std::unique_ptr<CustomSpawner> createCustomSpawner();
    static std::vector<SpawnerType> getAvailableSpawnerTypes();
    static std::vector<SpawnerMode> getAvailableSpawnerModes();
    static SpawnerProperties createSpawnerProperties(SpawnerType type, SpawnerMode mode = SpawnerMode::CONTINUOUS, 
                                                   float spawnRate = 10.0f, float spawnRadius = 1.0f, 
                                                   float spawnSpeed = 5.0f);
};

} // namespace RFBlood