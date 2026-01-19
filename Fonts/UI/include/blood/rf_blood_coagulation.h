#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <random>

namespace RFBlood {

// Coagulation types
enum class CoagulationType {
    SPHERE_MERGE,
    SURFACE_TENSION,
    VOLUME_MERGE,
    STRING_MERGE,
    CUSTOM
};

// Coagulation states
enum class CoagulationState {
    SEPARATE,
    APPROACHING,
    MERGED,
    SOLIDIFIED,
    FAILED
};

// Coagulation properties
struct CoagulationProperties {
    float mergeDistance;
    float mergeSpeed;
    float mergeThreshold;
    float surfaceTensionFactor;
    float viscosityFactor;
    float temperatureFactor;
    float minParticleCount;
    float maxParticleCount;
    bool enableVolumeConservation;
    bool enableSurfaceTension;
    bool enableViscosity;
    bool enableTemperature;
    CoagulationType type;
    
    CoagulationProperties() : mergeDistance(2.0f), mergeSpeed(1.0f), mergeThreshold(0.5f), 
                           surfaceTensionFactor(1.0f), viscosityFactor(1.0f), 
                           temperatureFactor(1.0f), minParticleCount(2), maxParticleCount(50), 
                           enableVolumeConservation(true), enableSurfaceTension(true), 
                           enableViscosity(true), enableTemperature(false), 
                           type(CoagulationType::SPHERE_MERGE) {}
};

// Coagulation result
struct CoagulationResult {
    bool success;
    std::vector<uint32_t> mergedParticleIds;
    std::vector<uint32_t> removedParticleIds;
    float finalVolume;
    float finalSurfaceArea;
    float finalMass;
    std::array<float, 4> finalColor;
    CoagulationState state;
    float mergeTime;
    
    CoagulationResult() : success(false), finalVolume(0.0f), finalSurfaceArea(0.0f), 
                      finalMass(0.0f), finalColor{0.0f, 0.0f, 0.0f, 1.0f}, 
                      state(CoagulationState::SEPARATE), mergeTime(0.0f) {}
};

// Coagulation pair
struct CoagulationPair {
    uint32_t particleId1;
    uint32_t particleId2;
    float distance;
    float mergeProbability;
    float mergeTime;
    bool isMerging;
    bool isMerged;
    
    CoagulationPair() : particleId1(0), particleId2(0), distance(0.0f), mergeProbability(0.0f), 
                   mergeTime(0.0f), isMerging(false), isMerged(false) {}
    CoagulationPair(uint32_t id1, uint32_t id2, float dist, float prob, float time, 
                bool merging, bool merged)
        : particleId1(id1), particleId2(id2), distance(dist), mergeProbability(prob), 
          mergeTime(time), isMerging(merging), isMerged(merged) {}
};

// Coagulation system
class BloodCoagulation {
public:
    BloodCoagulation();
    ~BloodCoagulation() = default;
    
    // Coagulation management
    void initialize(const CoagulationProperties& properties);
    void update(float deltaTime);
    void reset();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // Coagulation control
    void setCoagulationType(CoagulationType type);
    void setMergeDistance(float distance);
    void setMergeSpeed(float speed);
    void setMergeThreshold(float threshold);
    void setSurfaceTensionFactor(float factor);
    void setViscosityFactor(float factor);
    void setTemperatureFactor(float factor);
    void setVolumeConservation(bool enabled);
    void setMinParticleCount(int count);
    void setMaxParticleCount(int count);
    
    // Coagulation queries
    CoagulationType getCoagulationType() const;
    const CoagulationProperties& getCoagulationProperties() const;
    size_t getActiveCoagulationCount() const;
    size_t getCoagulationPairCount() const;
    const std::vector<CoagulationPair>& getCoagulationPairs() const;
    
    // Manual coagulation
    void forceCoagulation(uint32_t particleId1, uint32_t particleId2);
    void cancelCoagulation(uint32_t particleId1, uint32_t particleId2);
    
    // Event handling
    void addCoagulationEventListener(const std::string& eventType, std::function<void()> callback);
    void removeCoagulationEventListener(const std::string& eventType, std::function<void()> callback);
    void clearCoagulationEventListeners();
    
    // Utility methods
    void cloneFrom(const BloodCoagulation& other);
    std::unique_ptr<BloodCoagulation> clone() const;
    
    // Data access
    const std::vector<CoagulationPair>& getCoagulationPairs() const;
    const std::vector<CoagulationResult>& getCoagulationResults() const;
    
protected:
    // Protected members
    std::vector<CoagulationPair> coagulationPairs_;
    std::vector<CoagulationResult> coagulationResults_;
    CoagulationProperties properties_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    bool isPaused_;
    float accumulatedTime_;
    
    std::mt19937_19937 randomGenerator_;
    
    // Protected helper methods
    virtual void updateCoagulation(float deltaTime);
    virtual void processCoagulationPairs(float deltaTime);
    virtual void updateCoagulationPair(CoagulationPair& pair, float deltaTime);
    virtual void completeCoagulation(CoagulationPair& pair);
    virtual void failCoagulation(CoagulationPair& pair);
    
    // Coagulation algorithms
    virtual bool sphereMergeTest(uint32_t id1, uint32_t id2, float maxDistance);
    virtual bool surfaceTensionMergeTest(uint32_t id1, uint32_t id2, float maxDistance);
    virtual bool volumeMergeTest(uint32_t id1, uint32 id2, float maxDistance);
    virtual bool stringMergeTest(uint32_t id1, uint32 id2, float maxDistance);
    
    // Helper methods
    virtual float calculateMergeProbability(uint32_t id1, uint32_t id2, float distance, 
                                       const BloodParticle& p1, const BloodParticle& p2);
    virtual float calculateMergeSpeed(const BloodParticle& p1, const BloodParticle& p2);
    virtual float calculateMergeTime(const BloodParticle& p1, const BloodParticle& p2);
    virtual CoagulationResult createMergedParticle(const BloodParticle& p1, const BloodParticle& p2);
    virtual void removeCoagulationPair(uint32_t id1, uint32 id2);
    virtual void addCoagulationPair(uint32_t id1, uint32 id2, float distance, float probability, 
                                   float time, bool isMerging);
    virtual void updateCoagulationPairState(uint32_t id1, uint32 id2, CoagulationState state);
    
    // Performance optimization
    void optimizeCoagulationPairs();
    void removeCompletedPairs();
    void limitActivePairs(int maxActive);
};

// Specialized coagulation systems
class SphereCoagulation : public BloodCoagulation {
public:
    SphereCoagulation();
    void updateCoagulationPairs(float deltaTime) override;
    bool sphereMergeTest(uint32_t id1, uint32_t id2, float maxDistance) override;
    CoagulationResult createMergedParticle(const BloodParticle& p1, const BloodParticle& p2) override;
};

class SurfaceTensionCoagulation : public BloodCoagulation {
public:
    SurfaceTensionCoagulation();
    void updateCoagulationPairs(float deltaTime) override;
    bool surfaceTensionMergeTest(uint32_t id1, uint32 id2, float maxDistance) override;
    CoagulationResult createMergedParticle(const BloodParticle& p1, const BloodParticle& p2) override;
};

class VolumeCoagulation : public BloodCoagulation {
public:
    VolumeCoagulation();
    void updateCoagulationPairs(float deltaTime) override;
    bool volumeMergeTest(uint32_t id1, uint32_t id2, float maxDistance) override;
    CoagulationResult createMergedParticle(const BloodParticle& p1, const BloodParticle& p2) override;
};

class StringCoagulation : public BloodCoagulation {
public:
    StringCoagulation();
    void updateCoagulationPairs(float deltaTime) override;
    bool stringMergeTest(uint32_t id1, uint32 id2, float maxDistance) override;
    CoagulationResult createMergedParticle(const BloodParticle& p1, const BloodParticle& p2) override;
};

// Coagulation factory
class BloodCoagulationFactory {
public:
    static std::unique_ptr<BloodCoagulation> createCoagulation(CoagulationType type);
    static std::unique_ptr<SphereCoagulation> createSphereCoagulation();
    static std::unique_ptr<SurfaceTensionCoagulation> createSurfaceTensionCoagulation();
    static std::unique_ptr<VolumeCoagulation> createVolumeCoagulation();
    static std::unique_ptr<StringCoagulation> createStringCoagulation();
    static std::vector<CoagulationType> getAvailableCoagulationTypes();
    static CoagulationProperties createCoagulationProperties(CoagulationType type, float mergeDistance = 2.0f, 
                                                         float mergeSpeed = 1.0f, float mergeThreshold = 0.5f);
};

} // namespace RFBlood