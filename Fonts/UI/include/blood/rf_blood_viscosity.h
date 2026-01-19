#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <cmath>

namespace RFBlood {

// Viscosity types
enum class ViscosityType {
    NEWTONIAN,
    NON_NEWTONIAN,
    SHEAR_THINNING,
    SHEAR_THICKENING,
    BINGHAM,
    HERSCHEL_BULKLEY,
    POWER_LAW,
    CUSTOM
};

// Viscosity states
enum class ViscosityState {
    STABLE,
    CHANGING,
    THINNING,
    THICKENING,
    SOLIDIFIED,
    LIQUIDIFIED
};

// Viscosity properties
struct ViscosityProperties {
    float baseViscosity;
    float minViscosity;
    float maxViscosity;
    float shearRate;
    float shearStress;
    float yieldStress;
    float consistencyIndex;
    float flowBehaviorIndex;
    float temperature;
    float pressure;
    float concentration;
    bool enableTemperatureEffect;
    bool enablePressureEffect;
    bool enableConcentrationEffect;
    bool enableShearThinning;
    bool enableShearThickening;
    ViscosityType type;
    std::map<std::string, std::any> customProperties;
    
    ViscosityProperties() : baseViscosity(1.0f), minViscosity(0.1f), maxViscosity(10.0f), 
                         shearRate(1.0f), shearStress(1.0f), yieldStress(0.0f), 
                         consistencyIndex(1.0f), flowBehaviorIndex(1.0f), temperature(20.0f), 
                         pressure(1.0f), concentration(1.0f), enableTemperatureEffect(true), 
                         enablePressureEffect(false), enableConcentrationEffect(false), 
                         enableShearThinning(false), enableShearThickening(false), 
                         type(ViscosityType::NEWTONIAN) {}
};

// Viscosity measurement
struct ViscosityMeasurement {
    float time;
    float viscosity;
    float shearRate;
    float shearStress;
    float temperature;
    float pressure;
    float concentration;
    ViscosityState state;
    
    ViscosityMeasurement() : time(0.0f), viscosity(1.0f), shearRate(1.0f), shearStress(1.0f), 
                           temperature(20.0f), pressure(1.0f), concentration(1.0f), 
                           state(ViscosityState::STABLE) {}
    ViscosityMeasurement(float tm, float visc, float shearR, float shearS, 
                       float temp, float press, float conc, ViscosityState st)
        : time(tm), viscosity(visc), shearRate(shearR), shearStress(shearS), 
          temperature(temp), pressure(press), concentration(conc), state(st) {}
};

// Viscosity system
class BloodViscosity {
public:
    BloodViscosity();
    virtual ~BloodViscosity() = default;
    
    // Viscosity management
    void initialize(const ViscosityProperties& properties);
    void update(float deltaTime);
    void reset();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // Viscosity control
    void setBaseViscosity(float viscosity);
    void setViscosityRange(float minViscosity, float maxViscosity);
    void setShearRate(float rate);
    void setShearStress(float stress);
    void setYieldStress(float stress);
    void setConsistencyIndex(float index);
    void setFlowBehaviorIndex(float index);
    void setTemperature(float temperature);
    void setPressure(float pressure);
    void setConcentration(float concentration);
    
    // Effect controls
    void setTemperatureEffectEnabled(bool enabled);
    void setPressureEffectEnabled(bool enabled);
    void setConcentrationEffectEnabled(bool enabled);
    void setShearThinningEnabled(bool enabled);
    void setShearThickeningEnabled(bool enabled);
    
    // Viscosity queries
    float getCurrentViscosity() const;
    float getBaseViscosity() const;
    float getShearRate() const;
    float getShearStress() const;
    float getYieldStress() const;
    float getConsistencyIndex() const;
    float getFlowBehaviorIndex() const;
    float getTemperature() const;
    float getPressure() const;
    float getConcentration() const;
    ViscosityState getViscosityState() const;
    ViscosityType getViscosityType() const;
    const ViscosityProperties& getViscosityProperties() const;
    
    // Measurement
    void takeMeasurement();
    const std::vector<ViscosityMeasurement>& getMeasurements() const;
    const ViscosityMeasurement& getLatestMeasurement() const;
    void clearMeasurements();
    void setMaxMeasurements(int maxMeasurements);
    
    // Event handling
    void addViscosityEventListener(const std::string& eventType, std::function<void()> callback);
    void removeViscosityEventListener(const std::string& eventType, std::function<void()> callback);
    void clearViscosityEventListeners();
    
    // Utility methods
    void cloneFrom(const BloodViscosity& other);
    virtual std::unique_ptr<BloodViscosity> clone() const = 0;
    
    // Data access
    const std::vector<ViscosityMeasurement>& getMeasurements() const;
    
protected:
    // Protected members
    std::vector<ViscosityMeasurement> measurements_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    ViscosityProperties properties_;
    
    bool isPaused_;
    float accumulatedTime_;
    float measurementTimer_;
    int maxMeasurements_;
    
    std::mt19937_19937 randomGenerator_;
    
    // Protected helper methods
    virtual void updateViscosity(float deltaTime);
    virtual void calculateViscosity();
    virtual void updateViscosityState();
    virtual void triggerViscosityEvent(const std::string& eventType);
    virtual void updateMeasurementTimer(float deltaTime);
    
    // Viscosity calculation helpers
    virtual float calculateNewtonianViscosity();
    virtual float calculateNonNewtonianViscosity();
    virtual float calculateShearThinningViscosity();
    virtual float calculateShearThickeningViscosity();
    virtual float calculateBinghamViscosity();
    virtual float calculateHerschelBulkleyViscosity();
    virtual float calculatePowerLawViscosity();
    virtual float calculateCustomViscosity();
    
    // Effect helpers
    virtual float applyTemperatureEffect(float baseViscosity);
    virtual float applyPressureEffect(float baseViscosity);
    virtual float applyConcentrationEffect(float baseViscosity);
    virtual float applyShearThinningEffect(float baseViscosity);
    virtual float applyShearThickeningEffect(float baseViscosity);
    
    // Measurement helpers
    virtual ViscosityMeasurement createMeasurement();
    virtual void addMeasurement(const ViscosityMeasurement& measurement);
    virtual void removeOldMeasurements();
    
    // State helpers
    virtual void transitionToState(ViscosityState newState);
    virtual void onStateEnter(ViscosityState state);
    virtual void onStateExit(ViscosityState state);
    virtual void onStateUpdate(ViscosityState state, float deltaTime);
    
    // Performance optimization
    void optimizeMeasurementMemory();
    void limitMeasurementCount();
};

// Specialized viscosity systems
class NewtonianViscosity : public BloodViscosity {
public:
    NewtonianViscosity();
    void calculateViscosity() override;
    std::unique_ptr<BloodViscosity> clone() const override;
};

class NonNewtonianViscosity : public BloodViscosity {
public:
    NonNewtonianViscosity();
    void calculateViscosity() override;
    std::unique_ptr<BloodViscosity> clone() const override;
};

class ShearThinningViscosity : public BloodViscosity {
public:
    ShearThinningViscosity();
    void calculateViscosity() override;
    std::unique_ptr<BloodViscosity> clone() const override;
};

class ShearThickeningViscosity : public BloodViscosity {
public:
    ShearThickeningViscosity();
    void calculateViscosity() override;
    std::unique_ptr<BloodViscosity> clone() const override;
};

class BinghamViscosity : public BloodViscosity {
public:
    BinghamViscosity();
    void calculateViscosity() override;
    std::unique_ptr<BloodViscosity> clone() const override;
};

class HerschelBulkleyViscosity : public BloodViscosity {
public:
    HerschelBulkleyViscosity();
    void calculateViscosity() override;
    std::unique_ptr<BloodViscosity> clone() const override;
};

class PowerLawViscosity : public BloodViscosity {
public:
    PowerLawViscosity();
    void calculateViscosity() override;
    std::unique_ptr<BloodViscosity> clone() const override;
};

class CustomViscosity : public BloodViscosity {
public:
    CustomViscosity();
    void setCustomViscosityFunction(std::function<float(const ViscosityProperties&)> function);
    void calculateViscosity() override;
    std::unique_ptr<BloodViscosity> clone() const override;
    
protected:
    std::function<float(const ViscosityProperties&)> customViscosityFunction_;
};

// Viscosity factory
class BloodViscosityFactory {
public:
    static std::unique_ptr<BloodViscosity> createViscosity(ViscosityType type);
    static std::unique_ptr<NewtonianViscosity> createNewtonianViscosity();
    static std::unique_ptr<NonNewtonianViscosity> createNonNewtonianViscosity();
    static std::unique_ptr<ShearThinningViscosity> createShearThinningViscosity();
    static std::unique_ptr<ShearThickeningViscosity> createShearThickeningViscosity();
    static std::unique_ptr<BinghamViscosity> createBinghamViscosity();
    static std::unique_ptr<HerschelBulkleyViscosity> createHerschelBulkleyViscosity();
    static std::unique_ptr<PowerLawViscosity> createPowerLawViscosity();
    static std::unique_ptr<CustomViscosity> createCustomViscosity();
    static std::vector<ViscosityType> getAvailableViscosityTypes();
    static ViscosityProperties createViscosityProperties(ViscosityType type, float baseViscosity = 1.0f, 
                                                        float minViscosity = 0.1f, float maxViscosity = 10.0f);
};

} // namespace RFBlood