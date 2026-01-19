#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace RFBlood {

// Forward declarations
class Vector2;
class Vector3;
class Vector4;
class Color;
class Transform;
class Matrix4;
class Quaternion;

// Blood cell types
enum class BloodCellType {
    RedBloodCell,
    WhiteBloodCell,
    Platelet,
    Plasma,
    Unknown
};

// Blood vessel types
enum class BloodVesselType {
    Artery,
    Vein,
    Capillary,
    Arteriole,
    Venule,
    Unknown
};

// Blood flow states
enum class BloodFlowState {
    Normal,
    Slow,
    Fast,
    Turbulent,
    Stagnant,
    Blocked,
    Unknown
};

// Blood pressure ranges
enum class BloodPressureCategory {
    Low,
    Normal,
    Elevated,
    HighStage1,
    HighStage2,
    HypertensiveCrisis,
    Unknown
};

// Blood oxygen saturation levels
enum class OxygenSaturationLevel {
    Critical,
    Low,
    Normal,
    High,
    Unknown
};

// Blood glucose levels
enum class GlucoseLevel {
    Hypoglycemia,
    Normal,
    Prediabetes,
    Diabetes,
    Unknown
};

// Blood cell structure
struct BloodCell {
    BloodCellType type;
    Vector3 position;
    Vector3 velocity;
    float32 radius;
    float32 mass;
    Color color;
    uint32_t id;
    float32 age;
    float32 health;
    bool isActive;
    
    BloodCell() : type(BloodCellType::Unknown), position(), velocity(), radius(0.0f), mass(0.0f), 
                  color(), id(0), age(0.0f), health(1.0f), isActive(true) {}
};

// Blood vessel structure
struct BloodVessel {
    BloodVesselType type;
    Vector3 start;
    Vector3 end;
    float32 radius;
    float32 length;
    float32 elasticity;
    float32 pressure;
    BloodFlowState flowState;
    std::vector<BloodCell> cells;
    uint32_t id;
    bool isActive;
    
    BloodVessel() : type(BloodVesselType::Unknown), start(), end(), radius(0.0f), length(0.0f),
                    elasticity(1.0f), pressure(0.0f), flowState(BloodFlowState::Normal), 
                    cells(), id(0), isActive(true) {}
};

// Blood system configuration
struct BloodSystemConfig {
    float32 temperature;
    float32 pH;
    float32 viscosity;
    float32 density;
    float32 oxygenSaturation;
    float32 glucoseLevel;
    float32 hemoglobinLevel;
    float32 plateletCount;
    float32 whiteBloodCellCount;
    float32 redBloodCellCount;
    BloodPressureCategory pressureCategory;
    
    BloodSystemConfig() : temperature(37.0f), pH(7.4f), viscosity(0.004f), density(1060.0f),
                          oxygenSaturation(0.98f), glucoseLevel(5.0f), hemoglobinLevel(150.0f),
                          plateletCount(250000.0f), whiteBloodCellCount(7000.0f), 
                          redBloodCellCount(5000000.0f), pressureCategory(BloodPressureCategory::Normal) {}
};

// Blood simulation parameters
struct BloodSimulationParams {
    float32 timeStep;
    float32 gravity;
    float32 friction;
    float32 collisionDamping;
    float32 flowResistance;
    float32 oxygenDiffusionRate;
    float32 glucoseMetabolismRate;
    float32 cellGenerationRate;
    float32 cellDeathRate;
    bool enableCollisions;
    bool enableOxygenTransport;
    bool enableGlucoseMetabolism;
    bool enableCellGeneration;
    bool enableCellDeath;
    
    BloodSimulationParams() : timeStep(0.016f), gravity(9.81f), friction(0.1f), collisionDamping(0.8f),
                              flowResistance(0.05f), oxygenDiffusionRate(0.1f), glucoseMetabolismRate(0.05f),
                              cellGenerationRate(0.001f), cellDeathRate(0.0005f), enableCollisions(true),
                              enableOxygenTransport(true), enableGlucoseMetabolism(true),
                              enableCellGeneration(true), enableCellDeath(true) {}
};

// Blood analysis results
struct BloodAnalysis {
    uint32_t totalCellCount;
    uint32_t redBloodCellCount;
    uint32_t whiteBloodCellCount;
    uint32_t plateletCount;
    float32 averageCellVelocity;
    float32 averageCellRadius;
    float32 averageCellHealth;
    float32 totalVesselLength;
    uint32_t totalVesselCount;
    float32 averageBloodPressure;
    float32 averageFlowRate;
    float32 oxygenSaturation;
    float32 glucoseLevel;
    float32 hemoglobinLevel;
    float32 hematocrit;
    float32 bloodViscosity;
    BloodFlowState dominantFlowState;
    std::vector<BloodCellType> cellTypeDistribution;
    std::vector<BloodVesselType> vesselTypeDistribution;
    
    BloodAnalysis() : totalCellCount(0), redBloodCellCount(0), whiteBloodCellCount(0), plateletCount(0),
                       averageCellVelocity(0.0f), averageCellRadius(0.0f), averageCellHealth(0.0f),
                       totalVesselLength(0.0f), totalVesselCount(0), averageBloodPressure(0.0f),
                       averageFlowRate(0.0f), oxygenSaturation(0.0f), glucoseLevel(0.0f), hemoglobinLevel(0.0f),
                       hematocrit(0.0f), bloodViscosity(0.0f), dominantFlowState(BloodFlowState::Unknown) {}
};

// Blood system class
class BloodSystem {
public:
    // Constructor and destructor
    BloodSystem();
    explicit BloodSystem(const BloodSystemConfig& config);
    ~BloodSystem();
    
    // System management
    bool initialize();
    void shutdown();
    bool isInitialized() const;
    
    // Configuration
    void setConfiguration(const BloodSystemConfig& config);
    const BloodSystemConfig& getConfiguration() const;
    
    void setSimulationParameters(const BloodSimulationParams& params);
    const BloodSimulationParams& getSimulationParameters() const;
    
    // Cell management
    uint32_t addCell(const BloodCell& cell);
    bool removeCell(uint32_t cellId);
    BloodCell* getCell(uint32_t cellId);
    const BloodCell* getCell(uint32_t cellId) const;
    std::vector<BloodCell*> getAllCells();
    std::vector<const BloodCell*> getAllCells() const;
    
    uint32_t getCellCount() const;
    uint32_t getCellCount(BloodCellType type) const;
    void clearAllCells();
    
    // Vessel management
    uint32_t addVessel(const BloodVessel& vessel);
    bool removeVessel(uint32_t vesselId);
    BloodVessel* getVessel(uint32_t vesselId);
    const BloodVessel* getVessel(uint32_t vesselId) const;
    std::vector<BloodVessel*> getAllVessels();
    std::vector<const BloodVessel*> getAllVessels() const;
    
    uint32_t getVesselCount() const;
    uint32_t getVesselCount(BloodVesselType type) const;
    void clearAllVessels();
    
    // Simulation
    void update(float32 deltaTime);
    void step();
    void reset();
    
    // Physics
    void applyForces();
    void updateVelocities(float32 deltaTime);
    void updatePositions(float32 deltaTime);
    void handleCollisions();
    void handleVesselCollisions();
    void handleCellCollisions();
    
    // Biology
    void updateCellHealth(float32 deltaTime);
    void updateOxygenTransport(float32 deltaTime);
    void updateGlucoseMetabolism(float32 deltaTime);
    void generateCells(float32 deltaTime);
    void removeDeadCells(float32 deltaTime);
    
    // Flow dynamics
    void updateBloodFlow();
    void updateBloodPressure();
    void calculateFlowRates();
    void updateFlowStates();
    
    // Analysis
    BloodAnalysis analyze() const;
    BloodAnalysis analyzeRegion(const Vector3& min, const Vector3& max) const;
    BloodAnalysis analyzeVessel(uint32_t vesselId) const;
    
    // Statistics
    float32 getAverageBloodPressure() const;
    float32 getAverageFlowRate() const;
    float32 getAverageCellVelocity() const;
    float32 getAverageCellHealth() const;
    float32 getOxygenSaturation() const;
    float32 getGlucoseLevel() const;
    float32 getHemoglobinLevel() const;
    float32 getHematocrit() const;
    float32 getBloodViscosity() const;
    
    // Diagnostics
    bool isHealthy() const;
    std::vector<std::string> getHealthIssues() const;
    std::vector<std::string> getRecommendations() const;
    
    // Visualization
    void render() const;
    void renderVessels() const;
    void renderCells() const;
    void renderFlow() const;
    void renderAnalysis() const;
    
    // Export/Import
    bool exportToFile(const std::string& filename) const;
    bool importFromFile(const std::string& filename);
    std::string serialize() const;
    bool deserialize(const std::string& data);
    
    // Events
    void setCellAddedCallback(std::function<void(const BloodCell&)> callback);
    void setCellRemovedCallback(std::function<void(const BloodCell&)> callback);
    void setVesselAddedCallback(std::function<void(const BloodVessel&)> callback);
    void setVesselRemovedCallback(std::function<void(const BloodVessel&)> callback);
    void setHealthChangedCallback(std::function<void(const std::string&)> callback);
    
    // Constants
    static constexpr float32 DEFAULT_TEMPERATURE = 37.0f;
    static constexpr float32 DEFAULT_PH = 7.4f;
    static constexpr float32 DEFAULT_VISCOSITY = 0.004f;
    static constexpr float32 DEFAULT_DENSITY = 1060.0f;
    static constexpr float32 DEFAULT_OXYGEN_SATURATION = 0.98f;
    static constexpr float32 DEFAULT_GLUCOSE_LEVEL = 5.0f;
    static constexpr float32 DEFAULT_HEMOGLOBIN_LEVEL = 150.0f;
    static constexpr float32 DEFAULT_PLATELET_COUNT = 250000.0f;
    static constexpr float32 DEFAULT_WHITE_BLOOD_CELL_COUNT = 7000.0f;
    static constexpr float32 DEFAULT_RED_BLOOD_CELL_COUNT = 5000000.0f;
    
private:
    BloodSystemConfig config_;
    BloodSimulationParams params_;
    std::vector<BloodCell> cells_;
    std::vector<BloodVessel> vessels_;
    bool initialized_;
    
    uint32_t nextCellId_;
    uint32_t nextVesselId_;
    
    // Callbacks
    std::function<void(const BloodCell&)> cellAddedCallback_;
    std::function<void(const BloodCell&)> cellRemovedCallback_;
    std::function<void(const BloodVessel&)> vesselAddedCallback_;
    std::function<void(const BloodVessel&)> vesselRemovedCallback_;
    std::function<void(const std::string&)> healthChangedCallback_;
    
    // Internal methods
    void generateDefaultVessels();
    void generateDefaultCells();
    void updateSystemHealth();
    
    // Physics helpers
    Vector3 calculateDragForce(const BloodCell& cell) const;
    Vector3 calculateBuoyancyForce(const BloodCell& cell) const;
    Vector3 calculateVesselForce(const BloodCell& cell, const BloodVessel& vessel) const;
    
    // Biology helpers
    float32 calculateOxygenConsumption(const BloodCell& cell) const;
    float32 calculateGlucoseConsumption(const BloodCell& cell) const;
    float32 calculateCellHealth(const BloodCell& cell) const;
    
    // Analysis helpers
    void calculateCellStatistics(BloodAnalysis& analysis) const;
    void calculateVesselStatistics(BloodAnalysis& analysis) const;
    void calculateFlowStatistics(BloodAnalysis& analysis) const;
    void calculateBiologicalStatistics(BloodAnalysis& analysis) const;
};

// Blood API functions
namespace BloodAPI {
    // System management
    BloodSystem* createBloodSystem();
    BloodSystem* createBloodSystem(const BloodSystemConfig& config);
    void destroyBloodSystem(BloodSystem* system);
    
    // Configuration
    void setDefaultConfiguration(BloodSystem* system);
    void setHealthyConfiguration(BloodSystem* system);
    void setDiabeticConfiguration(BloodSystem* system);
    void setAnemicConfiguration(BloodSystem* system);
    void setHypertensiveConfiguration(BloodSystem* system);
    
    // Cell operations
    uint32_t createRedBloodCell(BloodSystem* system, const Vector3& position);
    uint32_t createWhiteBloodCell(BloodSystem* system, const Vector3& position);
    uint32_t createPlatelet(BloodSystem* system, const Vector3& position);
    uint32_t createPlasma(BloodSystem* system, const Vector3& position);
    
    bool removeCell(BloodSystem* system, uint32_t cellId);
    BloodCell* getCell(BloodSystem* system, uint32_t cellId);
    
    // Vessel operations
    uint32_t createArtery(BloodSystem* system, const Vector3& start, const Vector3& end, float32 radius);
    uint32_t createVein(BloodSystem* system, const Vector3& start, const Vector3& end, float32 radius);
    uint32_t createCapillary(BloodSystem* system, const Vector3& start, const Vector3& end, float32 radius);
    
    bool removeVessel(BloodSystem* system, uint32_t vesselId);
    BloodVessel* getVessel(BloodSystem* system, uint32_t vesselId);
    
    // Simulation
    void update(BloodSystem* system, float32 deltaTime);
    void step(BloodSystem* system);
    void reset(BloodSystem* system);
    
    // Analysis
    BloodAnalysis analyze(BloodSystem* system);
    BloodAnalysis analyzeRegion(BloodSystem* system, const Vector3& min, const Vector3& max);
    BloodAnalysis analyzeVessel(BloodSystem* system, uint32_t vesselId);
    
    // Diagnostics
    bool isHealthy(BloodSystem* system);
    std::vector<std::string> getHealthIssues(BloodSystem* system);
    std::vector<std::string> getRecommendations(BloodSystem* system);
    
    // Visualization
    void render(BloodSystem* system);
    void renderVessels(BloodSystem* system);
    void renderCells(BloodSystem* system);
    void renderFlow(BloodSystem* system);
    void renderAnalysis(BloodSystem* system);
    
    // Export/Import
    bool exportToFile(BloodSystem* system, const std::string& filename);
    bool importFromFile(BloodSystem* system, const std::string& filename);
    std::string serialize(BloodSystem* system);
    bool deserialize(BloodSystem* system, const std::string& data);
    
    // Utilities
    std::string bloodCellTypeToString(BloodCellType type);
    std::string bloodVesselTypeToString(BloodVesselType type);
    std::string bloodFlowStateToString(BloodFlowState state);
    std::string bloodPressureCategoryToString(BloodPressureCategory category);
    std::string oxygenSaturationLevelToString(OxygenSaturationLevel level);
    std::string glucoseLevelToString(GlucoseLevel level);
    
    BloodCellType stringToBloodCellType(const std::string& str);
    BloodVesselType stringToBloodVesselType(const std::string& str);
    BloodFlowState stringToBloodFlowState(const std::string& str);
    BloodPressureCategory stringToBloodPressureCategory(const std::string& str);
    OxygenSaturationLevel stringToOxygenSaturationLevel(const std::string& str);
    GlucoseLevel stringToGlucoseLevel(const std::string& str);
    
    // Validation
    bool isValidBloodCell(const BloodCell& cell);
    bool isValidBloodVessel(const BloodVessel& vessel);
    bool isValidBloodSystemConfig(const BloodSystemConfig& config);
    bool isValidBloodSimulationParams(const BloodSimulationParams& params);
    
    // Constants
    extern const float32 MIN_CELL_RADIUS;
    extern const float32 MAX_CELL_RADIUS;
    extern const float32 MIN_VESSEL_RADIUS;
    extern const float32 MAX_VESSEL_RADIUS;
    extern const float32 MIN_BLOOD_PRESSURE;
    extern const float32 MAX_BLOOD_PRESSURE;
    extern const float32 MIN_FLOW_RATE;
    extern const float32 MAX_FLOW_RATE;
    extern const float32 MIN_OXYGEN_SATURATION;
    extern const float32 MAX_OXYGEN_SATURATION;
    extern const float32 MIN_GLUCOSE_LEVEL;
    extern const float32 MAX_GLUCOSE_LEVEL;
}

} // namespace RFBlood