#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <cmath>

namespace RFBlood {

// Stream types
enum class StreamType {
    WATER_STREAM,
    BLOOD_STREAM,
    GORE_STREAM,
    SLIME_STREAM,
    ACID_STREAM,
    CUSTOM
};

// Stream states
enum class StreamState {
    FORMING,
    FLOWING,
    DRIPPING,
    SPLITTING,
    EVAPORATING,
    DRYING,
    COMPLETED
};

// Stream patterns
enum class StreamPattern {
    STRAIGHT,
    CURVED,
    WAVY,
    SPIRAL,
    CUSTOM
};

// Stream properties
struct StreamProperties {
    float initialVolume;
    float minVolume;
    float maxVolume;
    float flowRate;
    float flowSpeed;
    float flowAngle;
    float streamWidth;
    float streamLength;
    float viscosity;
    float evaporationRate;
    float absorptionRate;
    float dropFrequency;
    float dropSize;
    float dropSpeed;
    bool enablePhysics;
    bool enableEvaporation;
    bool enableAbsorption;
    bool enableGravity;
    bool enableDripping;
    bool enableSplitting;
    StreamType type;
    StreamPattern pattern;
    std::map<std::string, std::any> customProperties;
    
    StreamProperties() : initialVolume(20.0f), minVolume(0.1f), maxVolume(100.0f), 
                        flowRate(10.0f), flowSpeed(5.0f), flowAngle(45.0f), streamWidth(1.0f), 
                        streamLength(10.0f), viscosity(1.0f), evaporationRate(0.01f), 
                        absorptionRate(0.0f), dropFrequency(1.0f), dropSize(0.5f), 
                        dropSpeed(9.81f), enablePhysics(true), enableEvaporation(true), 
                        enableAbsorption(false), enableGravity(true), enableDripping(true), 
                        enableSplitting(false), type(StreamType::BLOOD_STREAM), 
                        pattern(StreamPattern::STRAIGHT) {}
};

// Stream particle
struct StreamParticle {
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
    bool isSplitting;
    uint32_t id;
    
    StreamParticle() : x(0.0f), y(0.0f), z(0.0f), vx(0.0f), vy(0.0f), vz(0.0f), 
                    radius(0.5f), mass(0.1f), age(0.0f), lifetime(10.0f), 
                    temperature(20.0f), viscosity(1.0f), evaporationRate(0.01f), 
                    color{1.0f, 0.0f, 0.0f, 1.0f}, alpha(1.0f), isActive(false), 
                    isDripping(false), isSplitting(false), id(0) {}
    StreamParticle(float x_, float y_, float z_, float vx_, float vy_, float vz_, 
                float radius_, float mass_, float age_, float lifetime_, 
                float temp_, float viscosity_, float evapRate_, 
                const std::vector<float>& color_, float alpha_, bool active, 
                bool dripping, bool splitting, uint32_t id_)
        : x(x_), y(y_), z(z_), vx(vx_), vy(vy_), vz(vz_), radius(radius_), mass(mass_), 
          age(age_), lifetime(lifetime_), temperature(temp_), viscosity(viscosity_), 
          evaporationRate(evapRate_), color(color_), alpha(alpha_), isActive(active), 
          isDripping(dripping), isSplitting(splitting), id(id_) {}
};

// Stream droplet
struct StreamDroplet {
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
    
    StreamDroplet() : x(0.0f), y(0.0f), z(0.0f), vx(0.0f), vy(0.0f), vz(0.0f), 
                    radius(0.3f), mass(0.05f), age(0.0f), lifetime(5.0f), 
                    temperature(20.0f), evaporationRate(0.1f), 
                    color{1.0f, 0.0f, 0.0f, 1.0f}, alpha(1.0f), isActive(false), id(0) {}
    StreamDroplet(float x_, float y_, float z_, float vx_, float vy_, float vz_, 
                float radius_, float mass_, float age_, float lifetime_, 
                float temp_, float evapRate_, 
                const std::vector<float>& color_, float alpha_, bool active, uint32_t id_)
        : x(x_), y(y_), z(z_), vx(vx_), vy(vy_), vz(vz_), radius(radius_), mass(mass_), 
          age(age_), lifetime(lifetime_), temperature(temp_), evaporationRate(evapRate_), 
          color(color_), alpha(alpha_), isActive(active), id(id_) {}
};

// Stream system
class BloodStream {
public:
    BloodStream();
    virtual ~BloodStream() = default;
    
    // Stream management
    void initialize(const StreamProperties& properties);
    void update(float deltaTime);
    void reset();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // Stream control
    void startStreaming(float duration);
    void stopStreaming();
    void setStreamType(StreamType type);
    void setStreamPattern(StreamPattern pattern);
    void setFlowRate(float rate);
    void setFlowSpeed(float speed);
    void setFlowAngle(float angle);
    void setStreamWidth(float width);
    void setStreamLength(float length);
    
    // Stream properties
    void setInitialVolume(float volume);
    void setVolumeRange(float minVolume, float maxVolume);
    void setViscosity(float viscosity);
    void setEvaporationRate(float rate);
    void setAbsorptionRate(float rate);
    void setDropFrequency(float frequency);
    void setDropSize(float size);
    void setDropSpeed(float speed);
    void setGravityEnabled(bool enabled);
    void setPhysicsEnabled(bool enabled);
    void setDrippingEnabled(bool enabled);
    void setSplittingEnabled(bool enabled);
    
    // Stream queries
    bool isStreaming() const;
    StreamType getStreamType() const;
    StreamPattern getStreamPattern() const;
    const StreamProperties& getStreamProperties() const;
    size_t getActiveParticleCount() const;
    size_t getActiveDropletCount() const;
    const std::vector<StreamParticle>& getActiveParticles() const;
    const std::vector<StreamDroplet>& getActiveDroplets() const;
    
    // Pattern generation
    void generateStraightPattern(float startX, float startY, float startZ, float endX, float endY, float endZ);
    void generateCurvedPattern(float startX, float startY, float startZ, float endX, float endY, float endZ, float curvature);
    void generateWavyPattern(float startX, float startY, float startZ, float endX, float endY, float endZ, float amplitude, float frequency);
    void generateSpiralPattern(float centerX, float centerY, float centerZ, float radius, float height, float turns);
    void generateCustomPattern(float startX, float startY, float startZ, float endX, float endY, float endZ, 
                             std::function<std::array<float, 3>(float)> pathFunction);
    
    // Event handling
    void addStreamEventListener(const std::string& eventType, std::function<void()> callback);
    void removeStreamEventListener(const std::string& eventType, std::function<void()> callback);
    void clearStreamEventListeners();
    
    // Utility methods
    void cloneFrom(const BloodStream& other);
    virtual std::unique_ptr<BloodStream> clone() const = 0;
    
    // Data access
    const std::vector<StreamParticle>& getParticles() const;
    const std::vector<StreamParticle>& getActiveParticles() const;
    const std::vector<StreamDroplet>& getDroplets() const;
    const std::vector<StreamDroplet>& getActiveDroplets() const;
    
protected:
    // Protected members
    std::vector<StreamParticle> particles_;
    std::vector<StreamParticle> activeParticles_;
    std::vector<StreamDroplet> droplets_;
    std::vector<StreamDroplet> activeDroplets_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    StreamProperties properties_;
    
    bool isPaused_;
    bool isStreaming_;
    float accumulatedTime_;
    float streamTimer_;
    float streamAccumulator_;
    float dropletAccumulator_;
    
    std::mt19937_19937 randomGenerator_;
    
    // Protected helper methods
    virtual void updateStream(float deltaTime);
    virtual void processStreaming(float deltaTime);
    virtual void emitParticles(float startX, float startY, float startZ, float endX, float endY, float endZ);
    virtual void emitDroplets();
    virtual void updateParticlePhysics(StreamParticle& particle, float deltaTime);
    virtual void updateDropletPhysics(StreamDroplet& droplet, float deltaTime);
    virtual void updateParticleState(StreamParticle& particle, float deltaTime);
    virtual void updateDropletState(StreamDroplet& droplet, float deltaTime);
    virtual void triggerStreamEvent(const std::string& eventType, const StreamParticle& particle);
    virtual void updateStreamTimer(float deltaTime);
    
    // Pattern generation helpers
    virtual std::array<float, 3> generateStraightPosition(float startX, float startY, float startZ, float endX, float endY, float endZ, float t);
    virtual std::array<float, 3> generateCurvedPosition(float startX, float startY, float startZ, float endX, float endY, float endZ, float t, float curvature);
    virtual std::array<float, 3> generateWavyPosition(float startX, float startY, float startZ, float endX, float endY, float endZ, float t, float amplitude, float frequency);
    virtual std::array<float, 3> generateSpiralPosition(float centerX, float centerY, float centerZ, float radius, float height, float t, float turns);
    virtual std::array<float, 3> generateCustomPosition(float startX, float startY, float startZ, float endX, float endY, float endZ, float t);
    
    // Physics helpers
    virtual void applyGravity(StreamParticle& particle, float deltaTime);
    virtual void applyWind(StreamParticle& particle, float deltaTime);
    virtual void applyForces(StreamParticle& particle, float deltaTime);
    virtual void applyViscosity(StreamParticle& particle, float deltaTime);
    virtual void applySurfaceTension(StreamParticle& particle, float deltaTime);
    
    // Particle helpers
    virtual StreamParticle createParticle(float x, float y, float z, const StreamProperties& props);
    virtual StreamDroplet createDroplet(float x, float y, float z, const StreamProperties& props);
    virtual float generateParticleVolume();
    virtual float generateParticleSpeed();
    virtual float generateParticleAngle();
    virtual float generateDropletVolume();
    virtual float generateDropletSpeed();
    virtual std::array<float, 4> generateParticleColor();
    virtual std::array<float, 4> generateDropletColor();
    
    // Stream helpers
    virtual void updateStreamVolume(float deltaTime);
    virtual void updateStreamLength(float deltaTime);
    virtual void updateStreamWidth(float deltaTime);
    virtual void updateStreamFlow(float deltaTime);
    virtual void calculateStreamProperties();
    
    // Performance optimization
    void optimizeParticleMemory();
    void optimizeDropletMemory();
    void removeDeadParticles();
    void removeDeadDroplets();
    void limitActiveParticles(int maxActive);
    void limitActiveDroplets(int maxActive);
};

// Specialized stream systems
class WaterStream : public BloodStream {
public:
    WaterStream();
    void updateParticlePhysics(StreamParticle& particle, float deltaTime) override;
    StreamParticle createParticle(float x, float y, float z, const StreamProperties& props) override;
    std::array<float, 4> generateParticleColor() override;
    std::array<float, 4> generateDropletColor() override;
};

class BloodStream : public BloodStream {
public:
    BloodStream();
    void updateParticlePhysics(StreamParticle& particle, float deltaTime) override;
    StreamParticle createParticle(float x, float y, float z, const StreamProperties& props) override;
    std::array<float, 4> generateParticleColor() override;
    std::array<float, 4> generateDropletColor() override;
};

class GoreStream : public BloodStream {
public:
    GoreStream();
    void updateParticlePhysics(StreamParticle& particle, float deltaTime) override;
    StreamParticle createParticle(float x, float y, float z, const StreamProperties& props) override;
    std::array<float, 4> generateParticleColor() override;
    std::array<float, 4> generateDropletColor() override;
};

class SlimeStream : public BloodStream {
public:
    SlimeStream();
    void updateParticlePhysics(StreamParticle& particle, float deltaTime) override;
    StreamParticle createParticle(float x, float y, float z, const StreamProperties& props) override;
    std::array<float, 4> generateParticleColor() override;
    std::array<float, 4> generateDropletColor() override;
};

class AcidStream : public BloodStream {
public:
    AcidStream();
    void updateParticlePhysics(StreamParticle& particle, float deltaTime) override;
    StreamParticle createParticle(float x, float y, float z, const StreamProperties& props) override;
    std::array<float, 4> generateParticleColor() override;
    std::array<float, 4> generateDropletColor() override;
};

// Stream factory
class BloodStreamFactory {
public:
    static std::unique_ptr<BloodStream> createStream(StreamType type);
    static std::unique_ptr<WaterStream> createWaterStream();
    static std::unique_ptr<BloodStream> createBloodStream();
    static std::unique_ptr<GoreStream> createGoreStream();
    static std::unique_ptr<SlimeStream> createSlimeStream();
    static std::unique_ptr<AcidStream> createAcidStream();
    static std::vector<StreamType> getAvailableStreamTypes();
    static std::vector<StreamPattern> getAvailableStreamPatterns();
    static StreamProperties createStreamProperties(StreamType type, StreamPattern pattern = StreamPattern::STRAIGHT, 
                                                  float initialVolume = 20.0f, float flowRate = 10.0f, 
                                                  float flowSpeed = 5.0f, float streamLength = 10.0f);
};

} // namespace RFBlood