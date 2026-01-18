#include "rf_blood_api.h"
#include "../include/utils/rf_vector3.h"
#include "../include/utils/rf_color.h"
#include "../include/utils/rf_math.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

namespace RFBlood {

// Constants
namespace BloodAPI {
    const float32 MIN_CELL_RADIUS = 2.0f;
    const float32 MAX_CELL_RADIUS = 15.0f;
    const float32 MIN_VESSEL_RADIUS = 5.0f;
    const float32 MAX_VESSEL_RADIUS = 50.0f;
    const float32 MIN_BLOOD_PRESSURE = 60.0f;
    const float32 MAX_BLOOD_PRESSURE = 200.0f;
    const float32 MIN_FLOW_RATE = 0.1f;
    const float32 MAX_FLOW_RATE = 100.0f;
    const float32 MIN_OXYGEN_SATURATION = 0.7f;
    const float32 MAX_OXYGEN_SATURATION = 1.0f;
    const float32 MIN_GLUCOSE_LEVEL = 3.0f;
    const float32 MAX_GLUCOSE_LEVEL = 20.0f;
}

// BloodSystem implementation
BloodSystem::BloodSystem() : initialized_(false), nextCellId_(1), nextVesselId_(1) {
}

BloodSystem::BloodSystem(const BloodSystemConfig& config) : config_(config), initialized_(false), nextCellId_(1), nextVesselId_(1) {
}

BloodSystem::~BloodSystem() {
    shutdown();
}

bool BloodSystem::initialize() {
    if (initialized_) return true;
    generateDefaultVessels();
    generateDefaultCells();
    initialized_ = true;
    return true;
}

void BloodSystem::shutdown() {
    clearAllCells();
    clearAllVessels();
    initialized_ = false;
}

bool BloodSystem::isInitialized() const {
    return initialized_;
}

void BloodSystem::setConfiguration(const BloodSystemConfig& config) {
    config_ = config;
    updateSystemHealth();
}

const BloodSystemConfig& BloodSystem::getConfiguration() const {
    return config_;
}

uint32_t BloodSystem::addCell(const BloodCell& cell) {
    BloodCell newCell = cell;
    newCell.id = nextCellId_++;
    cells_.push_back(newCell);
    if (cellAddedCallback_) cellAddedCallback_(newCell);
    return newCell.id;
}

bool BloodSystem::removeCell(uint32_t cellId) {
    auto it = std::find_if(cells_.begin(), cells_.end(),
        [cellId](const BloodCell& cell) { return cell.id == cellId; });
    if (it != cells_.end()) {
        if (cellRemovedCallback_) cellRemovedCallback_(*it);
        cells_.erase(it);
        return true;
    }
    return false;
}

BloodCell* BloodSystem::getCell(uint32_t cellId) {
    auto it = std::find_if(cells_.begin(), cells_.end(),
        [cellId](const BloodCell& cell) { return cell.id == cellId; });
    return (it != cells_.end()) ? &(*it) : nullptr;
}

const BloodCell* BloodSystem::getCell(uint32_t cellId) const {
    auto it = std::find_if(cells_.begin(), cells_.end(),
        [cellId](const BloodCell& cell) { return cell.id == cellId; });
    return (it != cells_.end()) ? &(*it) : nullptr;
}

uint32_t BloodSystem::getCellCount() const {
    return static_cast<uint32_t>(cells_.size());
}

uint32_t BloodSystem::getCellCount(BloodCellType type) const {
    return static_cast<uint32_t>(std::count_if(cells_.begin(), cells_.end(),
        [type](const BloodCell& cell) { return cell.type == type && cell.isActive; }));
}

void BloodSystem::clearAllCells() {
    cells_.clear();
}

uint32_t BloodSystem::addVessel(const BloodVessel& vessel) {
    BloodVessel newVessel = vessel;
    newVessel.id = nextVesselId_++;
    newVessel.length = (newVessel.end - newVessel.start).length();
    vessels_.push_back(newVessel);
    if (vesselAddedCallback_) vesselAddedCallback_(newVessel);
    return newVessel.id;
}

bool BloodSystem::removeVessel(uint32_t vesselId) {
    auto it = std::find_if(vessels_.begin(), vessels_.end(),
        [vesselId](const BloodVessel& vessel) { return vessel.id == vesselId; });
    if (it != vessels_.end()) {
        if (vesselRemovedCallback_) vesselRemovedCallback_(*it);
        vessels_.erase(it);
        return true;
    }
    return false;
}

BloodVessel* BloodSystem::getVessel(uint32_t vesselId) {
    auto it = std::find_if(vessels_.begin(), vessels_.end(),
        [vesselId](const BloodVessel& vessel) { return vessel.id == vesselId; });
    return (it != vessels_.end()) ? &(*it) : nullptr;
}

const BloodVessel* BloodSystem::getVessel(uint32_t vesselId) const {
    auto it = std::find_if(vessels_.begin(), vessels_.end(),
        [vesselId](const BloodVessel& vessel) { return vessel.id == vesselId; });
    return (it != vessels_.end()) ? &(*it) : nullptr;
}

uint32_t BloodSystem::getVesselCount() const {
    return static_cast<uint32_t>(vessels_.size());
}

void BloodSystem::clearAllVessels() {
    vessels_.clear();
}

void BloodSystem::update(float32 deltaTime) {
    if (!initialized_) return;
    
    applyForces();
    updateVelocities(deltaTime);
    updatePositions(deltaTime);
    
    if (params_.enableCollisions) handleCollisions();
    if (params_.enableOxygenTransport) updateOxygenTransport(deltaTime);
    if (params_.enableGlucoseMetabolism) updateGlucoseMetabolism(deltaTime);
    
    updateCellHealth(deltaTime);
    if (params_.enableCellGeneration) generateCells(deltaTime);
    if (params_.enableCellDeath) removeDeadCells(deltaTime);
    
    updateBloodFlow();
    updateBloodPressure();
    calculateFlowRates();
    updateFlowStates();
}

void BloodSystem::step() {
    update(params_.timeStep);
}

void BloodSystem::reset() {
    clearAllCells();
    clearAllVessels();
    generateDefaultVessels();
    generateDefaultCells();
}

void BloodSystem::applyForces() {
    for (auto& cell : cells_) {
        if (!cell.isActive) continue;
        
        Vector3 totalForce = Vector3::ZERO;
        totalForce.y -= cell.mass * params_.gravity;
        totalForce += calculateDragForce(cell);
        totalForce += calculateBuoyancyForce(cell);
        
        for (const auto& vessel : vessels_) {
            if (vessel.isActive) {
                totalForce += calculateVesselForce(cell, vessel);
            }
        }
        
        Vector3 acceleration = totalForce / cell.mass;
        cell.velocity += acceleration * params_.timeStep;
    }
}

void BloodSystem::updateVelocities(float32 deltaTime) {
    for (auto& cell : cells_) {
        if (!cell.isActive) continue;
        cell.velocity *= (1.0f - params_.friction * deltaTime);
        
        float32 maxVelocity = 100.0f;
        if (cell.velocity.length() > maxVelocity) {
            cell.velocity = cell.velocity.normalized() * maxVelocity;
        }
    }
}

void BloodSystem::updatePositions(float32 deltaTime) {
    for (auto& cell : cells_) {
        if (!cell.isActive) continue;
        cell.position += cell.velocity * deltaTime;
        cell.age += deltaTime;
    }
}

void BloodSystem::handleCollisions() {
    handleCellCollisions();
    handleVesselCollisions();
}

void BloodSystem::handleCellCollisions() {
    for (size_t i = 0; i < cells_.size(); ++i) {
        for (size_t j = i + 1; j < cells_.size(); ++j) {
            BloodCell& cell1 = cells_[i];
            BloodCell& cell2 = cells_[j];
            
            if (!cell1.isActive || !cell2.isActive) continue;
            
            Vector3 diff = cell2.position - cell1.position;
            float32 distance = diff.length();
            float32 minDistance = cell1.radius + cell2.radius;
            
            if (distance < minDistance && distance > 0.0f) {
                Vector3 normal = diff.normalized();
                float32 overlap = minDistance - distance;
                Vector3 separation = normal * (overlap * 0.5f);
                cell1.position -= separation;
                cell2.position += separation;
                
                Vector3 relativeVelocity = cell2.velocity - cell1.velocity;
                float32 velocityAlongNormal = relativeVelocity.dot(normal);
                
                if (velocityAlongNormal > 0) continue;
                
                float32 restitution = params_.collisionDamping;
                float32 impulse = 2 * velocityAlongNormal / (1/cell1.mass + 1/cell2.mass);
                Vector3 impulseVector = impulse * normal * restitution;
                
                cell1.velocity -= impulseVector / cell1.mass;
                cell2.velocity += impulseVector / cell2.mass;
            }
        }
    }
}

void BloodSystem::handleVesselCollisions() {
    for (auto& cell : cells_) {
        if (!cell.isActive) continue;
        
        for (const auto& vessel : vessels_) {
            if (!vessel.isActive) continue;
            
            Vector3 vesselDir = (vessel.end - vessel.start).normalized();
            Vector3 toCell = cell.position - vessel.start;
            float32 projection = toCell.dot(vesselDir);
            
            if (projection < 0 || projection > vessel.length) continue;
            
            Vector3 closestPoint = vessel.start + vesselDir * projection;
            Vector3 toCellFromVessel = cell.position - closestPoint;
            float32 distance = toCellFromVessel.length();
            
            if (distance < vessel.radius && distance > 0.0f) {
                Vector3 normal = toCellFromVessel.normalized();
                float32 overlap = vessel.radius - distance;
                cell.position += normal * overlap;
                
                float32 velocityAlongNormal = cell.velocity.dot(normal);
                if (velocityAlongNormal < 0) {
                    cell.velocity -= normal * velocityAlongNormal * (1.0f + params_.collisionDamping);
                }
            }
        }
    }
}

Vector3 BloodSystem::calculateDragForce(const BloodCell& cell) const {
    return -cell.velocity * params_.flowResistance * cell.radius;
}

Vector3 BloodSystem::calculateBuoyancyForce(const BloodCell& cell) const {
    float32 volume = (4.0f/3.0f) * RFUtils::Math::PI * cell.radius * cell.radius * cell.radius;
    return Vector3(0, config_.density * params_.gravity * volume, 0);
}

Vector3 BloodSystem::calculateVesselForce(const BloodCell& cell, const BloodVessel& vessel) const {
    Vector3 vesselDir = (vessel.end - vessel.start).normalized();
    Vector3 toCell = cell.position - vessel.start;
    float32 projection = toCell.dot(vesselDir);
    
    if (projection < 0 || projection > vessel.length) return Vector3::ZERO;
    
    Vector3 closestPoint = vessel.start + vesselDir * projection;
    Vector3 toCellFromVessel = cell.position - closestPoint;
    float32 distance = toCellFromVessel.length();
    
    if (distance < vessel.radius * 2.0f) {
        float32 flowForce = vessel.pressure / vessel.radius;
        return vesselDir * flowForce * (1.0f - distance / (vessel.radius * 2.0f));
    }
    
    return Vector3::ZERO;
}

void BloodSystem::updateCellHealth(float32 deltaTime) {
    for (auto& cell : cells_) {
        if (!cell.isActive) continue;
        
        float32 healthChange = calculateCellHealth(cell) * deltaTime;
        cell.health = RFUtils::Math::clamp(cell.health + healthChange, 0.0f, 1.0f);
        
        if (cell.health <= 0.0f) {
            cell.isActive = false;
        }
    }
}

void BloodSystem::updateOxygenTransport(float32 deltaTime) {
    for (auto& cell : cells_) {
        if (!cell.isActive || cell.type != BloodCellType::RedBloodCell) continue;
        
        float32 oxygenConsumption = calculateOxygenConsumption(cell);
        config_.oxygenSaturation = RFUtils::Math::clamp(
            config_.oxygenSaturation - oxygenConsumption * deltaTime * params_.oxygenDiffusionRate,
            BloodAPI::MIN_OXYGEN_SATURATION, BloodAPI::MAX_OXYGEN_SATURATION);
    }
}

void BloodSystem::updateGlucoseMetabolism(float32 deltaTime) {
    for (auto& cell : cells_) {
        if (!cell.isActive) continue;
        
        float32 glucoseConsumption = calculateGlucoseConsumption(cell);
        config_.glucoseLevel = RFUtils::Math::clamp(
            config_.glucoseLevel - glucoseConsumption * deltaTime * params_.glucoseMetabolismRate,
            BloodAPI::MIN_GLUCOSE_LEVEL, BloodAPI::MAX_GLUCOSE_LEVEL);
    }
}

void BloodSystem::generateCells(float32 deltaTime) {
    float32 generationChance = params_.cellGenerationRate * deltaTime;
    
    if (RFUtils::Math::random() < generationChance) {
        BloodCell newCell;
        newCell.type = BloodCellType::RedBloodCell;
        newCell.position = Vector3(RFUtils::Math::random() * 800.0f, RFUtils::Math::random() * 600.0f, 0);
        newCell.velocity = Vector3(RFUtils::Math::random(-50.0f, 50.0f), RFUtils::Math::random(-50.0f, 50.0f), 0);
        newCell.radius = RFUtils::Math::random(5.0f, 8.0f);
        newCell.mass = newCell.radius * 0.001f;
        newCell.color = Color(0.8f, 0.2f, 0.2f, 1.0f);
        newCell.health = 1.0f;
        newCell.isActive = true;
        
        addCell(newCell);
    }
}

void BloodSystem::removeDeadCells(float32 deltaTime) {
    cells_.erase(std::remove_if(cells_.begin(), cells_.end(),
        [](const BloodCell& cell) { return !cell.isActive; }), cells_.end());
}

void BloodSystem::updateBloodFlow() {
    for (auto& vessel : vessels_) {
        if (!vessel.isActive) continue;
        
        float32 flowRate = vessel.pressure / (vessel.length * config_.viscosity);
        vessel.flowState = (flowRate > 50.0f) ? BloodFlowState::Fast :
                          (flowRate > 20.0f) ? BloodFlowState::Normal :
                          (flowRate > 5.0f) ? BloodFlowState::Slow :
                          BloodFlowState::Stagnant;
    }
}

void BloodSystem::updateBloodPressure() {
    float32 totalPressure = 0.0f;
    int activeVessels = 0;
    
    for (const auto& vessel : vessels_) {
        if (vessel.isActive) {
            totalPressure += vessel.pressure;
            activeVessels++;
        }
    }
    
    if (activeVessels > 0) {
        float32 avgPressure = totalPressure / activeVessels;
        
        for (auto& vessel : vessels_) {
            if (vessel.isActive) {
                vessel.pressure = RFUtils::Math::lerp(vessel.pressure, avgPressure, 0.1f);
            }
        }
    }
}

void BloodSystem::calculateFlowRates() {
    // Flow rates are calculated in updateBloodFlow()
}

void BloodSystem::updateFlowStates() {
    // Flow states are updated in updateBloodFlow()
}

float32 BloodSystem::calculateOxygenConsumption(const BloodCell& cell) const {
    return cell.health * 0.001f;
}

float32 BloodSystem::calculateGlucoseConsumption(const BloodCell& cell) const {
    return cell.health * 0.0005f;
}

float32 BloodSystem::calculateCellHealth(const BloodCell& cell) const {
    float32 healthFactor = 1.0f;
    
    if (cell.age > 100.0f) {
        healthFactor -= (cell.age - 100.0f) * 0.001f;
    }
    
    if (config_.oxygenSaturation < 0.9f) {
        healthFactor -= (0.9f - config_.oxygenSaturation) * 0.5f;
    }
    
    if (config_.glucoseLevel < 4.0f) {
        healthFactor -= (4.0f - config_.glucoseLevel) * 0.1f;
    }
    
    return RFUtils::Math::clamp(healthFactor - 0.001f, -0.01f, 0.01f);
}

BloodAnalysis BloodSystem::analyze() const {
    BloodAnalysis analysis;
    
    calculateCellStatistics(analysis);
    calculateVesselStatistics(analysis);
    calculateFlowStatistics(analysis);
    calculateBiologicalStatistics(analysis);
    
    return analysis;
}

void BloodSystem::calculateCellStatistics(BloodAnalysis& analysis) const {
    analysis.totalCellCount = static_cast<uint32_t>(cells_.size());
    analysis.redBloodCellCount = getCellCount(BloodCellType::RedBloodCell);
    analysis.whiteBloodCellCount = getCellCount(BloodCellType::WhiteBloodCell);
    analysis.plateletCount = getCellCount(BloodCellType::Platelet);
    
    float32 totalVelocity = 0.0f;
    float32 totalRadius = 0.0f;
    float32 totalHealth = 0.0f;
    int activeCells = 0;
    
    for (const auto& cell : cells_) {
        if (cell.isActive) {
            totalVelocity += cell.velocity.length();
            totalRadius += cell.radius;
            totalHealth += cell.health;
            activeCells++;
        }
    }
    
    if (activeCells > 0) {
        analysis.averageCellVelocity = totalVelocity / activeCells;
        analysis.averageCellRadius = totalRadius / activeCells;
        analysis.averageCellHealth = totalHealth / activeCells;
    }
}

void BloodSystem::calculateVesselStatistics(BloodAnalysis& analysis) const {
    analysis.totalVesselCount = static_cast<uint32_t>(vessels_.size());
    
    float32 totalLength = 0.0f;
    float32 totalPressure = 0.0f;
    int activeVessels = 0;
    
    for (const auto& vessel : vessels_) {
        if (vessel.isActive) {
            totalLength += vessel.length;
            totalPressure += vessel.pressure;
            activeVessels++;
        }
    }
    
    analysis.totalVesselLength = totalLength;
    if (activeVessels > 0) {
        analysis.averageBloodPressure = totalPressure / activeVessels;
    }
}

void BloodSystem::calculateFlowStatistics(BloodAnalysis& analysis) const {
    float32 totalFlowRate = 0.0f;
    std::map<BloodFlowState, int> flowStateCount;
    
    for (const auto& vessel : vessels_) {
        if (vessel.isActive) {
            float32 flowRate = vessel.pressure / (vessel.length * config_.viscosity);
            totalFlowRate += flowRate;
            flowStateCount[vessel.flowState]++;
        }
    }
    
    if (!vessels_.empty()) {
        analysis.averageFlowRate = totalFlowRate / vessels_.size();
    }
    
    int maxCount = 0;
    for (const auto& pair : flowStateCount) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            analysis.dominantFlowState = pair.first;
        }
    }
}

void BloodSystem::calculateBiologicalStatistics(BloodAnalysis& analysis) const {
    analysis.oxygenSaturation = config_.oxygenSaturation;
    analysis.glucoseLevel = config_.glucoseLevel;
    analysis.hemoglobinLevel = config_.hemoglobinLevel;
    analysis.hematocrit = config_.redBloodCellCount / 10000000.0f;
    analysis.bloodViscosity = config_.viscosity;
}

float32 BloodSystem::getAverageBloodPressure() const {
    float32 totalPressure = 0.0f;
    int activeVessels = 0;
    
    for (const auto& vessel : vessels_) {
        if (vessel.isActive) {
            totalPressure += vessel.pressure;
            activeVessels++;
        }
    }
    
    return (activeVessels > 0) ? totalPressure / activeVessels : 0.0f;
}

float32 BloodSystem::getAverageFlowRate() const {
    float32 totalFlowRate = 0.0f;
    int activeVessels = 0;
    
    for (const auto& vessel : vessels_) {
        if (vessel.isActive) {
            totalFlowRate += vessel.pressure / (vessel.length * config_.viscosity);
            activeVessels++;
        }
    }
    
    return (activeVessels > 0) ? totalFlowRate / activeVessels : 0.0f;
}

float32 BloodSystem::getOxygenSaturation() const {
    return config_.oxygenSaturation;
}

float32 BloodSystem::getGlucoseLevel() const {
    return config_.glucoseLevel;
}

bool BloodSystem::isHealthy() const {
    return config_.oxygenSaturation > 0.9f &&
           config_.glucoseLevel > 4.0f && config_.glucoseLevel < 7.0f &&
           getAverageBloodPressure() > 80.0f && getAverageBloodPressure() < 120.0f;
}

std::vector<std::string> BloodSystem::getHealthIssues() const {
    std::vector<std::string> issues;
    
    if (config_.oxygenSaturation < 0.9f) {
        issues.push_back("Low oxygen saturation");
    }
    if (config_.glucoseLevel < 4.0f) {
        issues.push_back("Low glucose level (hypoglycemia)");
    }
    if (config_.glucoseLevel > 7.0f) {
        issues.push_back("High glucose level (hyperglycemia)");
    }
    if (getAverageBloodPressure() < 80.0f) {
        issues.push_back("Low blood pressure (hypotension)");
    }
    if (getAverageBloodPressure() > 120.0f) {
        issues.push_back("High blood pressure (hypertension)");
    }
    
    return issues;
}

void BloodSystem::generateDefaultVessels() {
    BloodVessel vessel;
    
    // Main artery
    vessel.type = BloodVesselType::Artery;
    vessel.start = Vector3(100, 300, 0);
    vessel.end = Vector3(700, 300, 0);
    vessel.radius = 20.0f;
    vessel.elasticity = 0.8f;
    vessel.pressure = 120.0f;
    vessel.flowState = BloodFlowState::Normal;
    vessel.isActive = true;
    addVessel(vessel);
    
    // Main vein
    vessel.type = BloodVesselType::Vein;
    vessel.start = Vector3(100, 400, 0);
    vessel.end = Vector3(700, 400, 0);
    vessel.radius = 18.0f;
    vessel.pressure = 80.0f;
    addVessel(vessel);
    
    // Capillaries
    for (int i = 0; i < 5; ++i) {
        vessel.type = BloodVesselType::Capillary;
        vessel.start = Vector3(200 + i * 100, 300, 0);
        vessel.end = Vector3(200 + i * 100, 400, 0);
        vessel.radius = 3.0f;
        vessel.pressure = 100.0f;
        addVessel(vessel);
    }
}

void BloodSystem::generateDefaultCells() {
    for (int i = 0; i < 50; ++i) {
        BloodCell cell;
        cell.type = BloodCellType::RedBloodCell;
        cell.position = Vector3(RFUtils::Math::random(100.0f, 700.0f), RFUtils::Math::random(250.0f, 450.0f), 0);
        cell.velocity = Vector3(RFUtils::Math::random(-20.0f, 20.0f), RFUtils::Math::random(-10.0f, 10.0f), 0);
        cell.radius = RFUtils::Math::random(5.0f, 8.0f);
        cell.mass = cell.radius * 0.001f;
        cell.color = Color(0.8f, 0.2f, 0.2f, 1.0f);
        cell.health = RFUtils::Math::random(0.7f, 1.0f);
        cell.isActive = true;
        addCell(cell);
    }
    
    for (int i = 0; i < 10; ++i) {
        BloodCell cell;
        cell.type = BloodCellType::WhiteBloodCell;
        cell.position = Vector3(RFUtils::Math::random(100.0f, 700.0f), RFUtils::Math::random(250.0f, 450.0f), 0);
        cell.velocity = Vector3(RFUtils::Math::random(-15.0f, 15.0f), RFUtils::Math::random(-8.0f, 8.0f), 0);
        cell.radius = RFUtils::Math::random(8.0f, 12.0f);
        cell.mass = cell.radius * 0.0015f;
        cell.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        cell.health = RFUtils::Math::random(0.8f, 1.0f);
        cell.isActive = true;
        addCell(cell);
    }
}

void BloodSystem::updateSystemHealth() {
    if (healthChangedCallback_) {
        std::vector<std::string> issues = getHealthIssues();
        if (!issues.empty()) {
            healthChangedCallback_(issues[0]);
        }
    }
}

void BloodSystem::setCellAddedCallback(std::function<void(const BloodCell&)> callback) {
    cellAddedCallback_ = callback;
}

void BloodSystem::setCellRemovedCallback(std::function<void(const BloodCell&)> callback) {
    cellRemovedCallback_ = callback;
}

void BloodSystem::setVesselAddedCallback(std::function<void(const BloodVessel&)> callback) {
    vesselAddedCallback_ = callback;
}

void BloodSystem::setVesselRemovedCallback(std::function<void(const BloodVessel&)> callback) {
    vesselRemovedCallback_ = callback;
}

void BloodSystem::setHealthChangedCallback(std::function<void(const std::string&)> callback) {
    healthChangedCallback_ = callback;
}

// BloodAPI implementation
namespace BloodAPI {

BloodSystem* createBloodSystem() {
    return new BloodSystem();
}

BloodSystem* createBloodSystem(const BloodSystemConfig& config) {
    return new BloodSystem(config);
}

void destroyBloodSystem(BloodSystem* system) {
    delete system;
}

void setDefaultConfiguration(BloodSystem* system) {
    BloodSystemConfig config;
    system->setConfiguration(config);
}

void setHealthyConfiguration(BloodSystem* system) {
    BloodSystemConfig config;
    config.oxygenSaturation = 0.98f;
    config.glucoseLevel = 5.0f;
    system->setConfiguration(config);
}

void setDiabeticConfiguration(BloodSystem* system) {
    BloodSystemConfig config;
    config.glucoseLevel = 12.0f;
    system->setConfiguration(config);
}

void setAnemicConfiguration(BloodSystem* system) {
    BloodSystemConfig config;
    config.oxygenSaturation = 0.85f;
    config.hemoglobinLevel = 100.0f;
    system->setConfiguration(config);
}

void setHypertensiveConfiguration(BloodSystem* system) {
    BloodSystemConfig config;
    config.pressureCategory = BloodPressureCategory::HighStage2;
    system->setConfiguration(config);
}

uint32_t createRedBloodCell(BloodSystem* system, const Vector3& position) {
    BloodCell cell;
    cell.type = BloodCellType::RedBloodCell;
    cell.position = position;
    cell.radius = 6.0f;
    cell.mass = 0.006f;
    cell.color = Color(0.8f, 0.2f, 0.2f, 1.0f);
    cell.health = 1.0f;
    cell.isActive = true;
    return system->addCell(cell);
}

uint32_t createWhiteBloodCell(BloodSystem* system, const Vector3& position) {
    BloodCell cell;
    cell.type = BloodCellType::WhiteBloodCell;
    cell.position = position;
    cell.radius = 10.0f;
    cell.mass = 0.015f;
    cell.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    cell.health = 1.0f;
    cell.isActive = true;
    return system->addCell(cell);
}

uint32_t createPlatelet(BloodSystem* system, const Vector3& position) {
    BloodCell cell;
    cell.type = BloodCellType::Platelet;
    cell.position = position;
    cell.radius = 2.0f;
    cell.mass = 0.002f;
    cell.color = Color(0.8f, 0.6f, 0.4f, 1.0f);
    cell.health = 1.0f;
    cell.isActive = true;
    return system->addCell(cell);
}

uint32_t createPlasma(BloodSystem* system, const Vector3& position) {
    BloodCell cell;
    cell.type = BloodCellType::Plasma;
    cell.position = position;
    cell.radius = 4.0f;
    cell.mass = 0.004f;
    cell.color = Color(0.9f, 0.7f, 0.5f, 0.8f);
    cell.health = 1.0f;
    cell.isActive = true;
    return system->addCell(cell);
}

bool removeCell(BloodSystem* system, uint32_t cellId) {
    return system->removeCell(cellId);
}

BloodCell* getCell(BloodSystem* system, uint32_t cellId) {
    return system->getCell(cellId);
}

uint32_t createArtery(BloodSystem* system, const Vector3& start, const Vector3& end, float32 radius) {
    BloodVessel vessel;
    vessel.type = BloodVesselType::Artery;
    vessel.start = start;
    vessel.end = end;
    vessel.radius = radius;
    vessel.elasticity = 0.8f;
    vessel.pressure = 120.0f;
    vessel.flowState = BloodFlowState::Normal;
    vessel.isActive = true;
    return system->addVessel(vessel);
}

uint32_t createVein(BloodSystem* system, const Vector3& start, const Vector3& end, float32 radius) {
    BloodVessel vessel;
    vessel.type = BloodVesselType::Vein;
    vessel.start = start;
    vessel.end = end;
    vessel.radius = radius;
    vessel.elasticity = 0.6f;
    vessel.pressure = 80.0f;
    vessel.flowState = BloodFlowState::Normal;
    vessel.isActive = true;
    return system->addVessel(vessel);
}

uint32_t createCapillary(BloodSystem* system, const Vector3& start, const Vector3& end, float32 radius) {
    BloodVessel vessel;
    vessel.type = BloodVesselType::Capillary;
    vessel.start = start;
    vessel.end = end;
    vessel.radius = radius;
    vessel.elasticity = 0.3f;
    vessel.pressure = 100.0f;
    vessel.flowState = BloodFlowState::Normal;
    vessel.isActive = true;
    return system->addVessel(vessel);
}

bool removeVessel(BloodSystem* system, uint32_t vesselId) {
    return system->removeVessel(vesselId);
}

BloodVessel* getVessel(BloodSystem* system, uint32_t vesselId) {
    return system->getVessel(vesselId);
}

void update(BloodSystem* system, float32 deltaTime) {
    system->update(deltaTime);
}

void step(BloodSystem* system) {
    system->step();
}

void reset(BloodSystem* system) {
    system->reset();
}

BloodAnalysis analyze(BloodSystem* system) {
    return system->analyze();
}

BloodAnalysis analyzeRegion(BloodSystem* system, const Vector3& min, const Vector3& max) {
    return system->analyzeRegion(min, max);
}

BloodAnalysis analyzeVessel(BloodSystem* system, uint32_t vesselId) {
    return system->analyzeVessel(vesselId);
}

bool isHealthy(BloodSystem* system) {
    return system->isHealthy();
}

std::vector<std::string> getHealthIssues(BloodSystem* system) {
    return system->getHealthIssues();
}

std::vector<std::string> getRecommendations(BloodSystem* system) {
    return system->getRecommendations();
}

std::string bloodCellTypeToString(BloodCellType type) {
    switch (type) {
        case BloodCellType::RedBloodCell: return "RedBloodCell";
        case BloodCellType::WhiteBloodCell: return "WhiteBloodCell";
        case BloodCellType::Platelet: return "Platelet";
        case BloodCellType::Plasma: return "Plasma";
        default: return "Unknown";
    }
}

std::string bloodVesselTypeToString(BloodVesselType type) {
    switch (type) {
        case BloodVesselType::Artery: return "Artery";
        case BloodVesselType::Vein: return "Vein";
        case BloodVesselType::Capillary: return "Capillary";
        case BloodVesselType::Arteriole: return "Arteriole";
        case BloodVesselType::Venule: return "Venule";
        default: return "Unknown";
    }
}

std::string bloodFlowStateToString(BloodFlowState state) {
    switch (state) {
        case BloodFlowState::Normal: return "Normal";
        case BloodFlowState::Slow: return "Slow";
        case BloodFlowState::Fast: return "Fast";
        case BloodFlowState::Turbulent: return "Turbulent";
        case BloodFlowState::Stagnant: return "Stagnant";
        case BloodFlowState::Blocked: return "Blocked";
        default: return "Unknown";
    }
}

bool isValidBloodCell(const BloodCell& cell) {
    return cell.radius > 0 && cell.mass > 0 && cell.health >= 0 && cell.health <= 1;
}

bool isValidBloodVessel(const BloodVessel& vessel) {
    return vessel.radius > 0 && vessel.length > 0 && vessel.elasticity > 0;
}

} // namespace BloodAPI

} // namespace RFBlood
