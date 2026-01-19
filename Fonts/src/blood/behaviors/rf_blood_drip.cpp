#include "rf_blood_drip.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

BloodDripBehavior::BloodDripBehavior()
    : m_dripRate(0.0f)
    , m_dripSpeed(1.0f)
    , m_gravity(9.81f)
    , m_surfaceTension(0.0728f)
    , m_viscosity(0.5f)
    , m_coagulationRate(0.1f)
    , m_evaporationRate(0.05f)
    , m_minDripSize(0.1f)
    , m_maxDripSize(2.0f)
    , m_dripLifetime(5.0f)
    , m_spreadRadius(0.5f)
    , m_dripColor(0.8f, 0.1f, 0.1f, 1.0f)
    , m_dripOpacity(0.8f)
    , m_isEnabled(true)
    , m_dripCount(0)
    , m_maxDrips(100)
{
    m_drips.reserve(m_maxDrips);
    RF_LOG_INFO("BloodDripBehavior created");
}

BloodDripBehavior::~BloodDripBehavior() {
    clear();
    RF_LOG_INFO("BloodDripBehavior destroyed");
}

void BloodDripBehavior::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("BloodDripBehavior initialized");
}

void BloodDripBehavior::clear() {
    for (auto& drip : m_drips) {
        if (drip) {
            RF_DELETE(drip);
        }
    }
    
    m_drips.clear();
    m_dripCount = 0;
    RF_LOG_INFO("Cleared all blood drips");
}

void BloodDripBehavior::addDrip(const Vec3& position, const Vec3& velocity, float size, 
                   const Color& color, float lifetime) {
    if (m_dripCount >= m_maxDrips) {
        // Remove oldest drip
        RF_DELETE(m_drips.front());
        m_drips.erase(m_drips.begin());
        m_dripCount--;
    }
    
    if (size < m_minDripSize) {
        size = m_minDripSize;
    }
    
    if (size > m_maxDripSize) {
        size = m_maxDripSize;
    }
    
    auto drip = RF_NEW(BloodDrip);
    drip->initialize(position, velocity, size, color, lifetime, m_properties);
    
    m_drips.push_back(drip);
    m_dripCount++;
    
    RF_LOG_INFO("Added blood drip at position (%.2f, %.2f, %.2f)", 
                position.x, position.y, position.z);
}

void BloodDripBehavior::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update existing drips
    for (auto& drip : m_drips) {
        if (drip) {
            drip->update(deltaTime);
            
            // Remove completed drips
            if (drip->isCompleted()) {
                RF_DELETE(drip);
                m_drips.erase(std::find(m_drips.begin(), m_drips.end(), drip));
                m_dripCount--;
            }
        }
    }
    
    // Add new drips based on surface
    updateDripGeneration(deltaTime);
}

void BloodDripBehavior::updateDripGeneration(float deltaTime) {
    // Generate new drips based on blood surface
    // This would be connected to the blood surface system
    
    // For now, just update existing drips
    (void)deltaTime;
}

void BloodDripBehavior::setDripRate(float rate) {
    m_dripRate = rate;
    RF_LOG_INFO("Blood drip rate set to %.2f", rate);
}

void BloodDripBehavior::setDripSpeed(float speed) {
    m_dripSpeed = speed;
    RF_LOG_INFO("Blood drip speed set to %.2f", speed);
}

void BloodDripBehavior::setGravity(float gravity) {
    m_gravity = gravity;
    RF_LOG_INFO("Blood drip gravity set to %.2f", gravity);
}

void BloodDripBehavior::setSurfaceTension(float tension) {
    m_surfaceTension = tension;
    RF_LOG_INFO("Blood surface tension set to %.2f", tension);
}

void BloodDripBehavior::setViscosity(float viscosity) {
    m_viscosity = viscosity;
    RF_LOG_INFO("Blood viscosity set to %.2f", viscosity);
}

void BloodDripBehavior::setCoagulationRate(float rate) {
    m_coagulationRate = rate;
    RF_LOG_INFO("Blood coagulation rate set to %.2f", rate);
}

void BloodDripBehavior::setEvaporationRate(float rate) {
    m_evaporationRate = rate;
    RF_LOG_INFO("Blood evaporation rate set to %.2f", rate);
}

void BloodDripBehavior::setMinDripSize(float size) {
    m_minDripSize = size;
    RF_LOG_INFO("Blood min drip size set to %.2f", size);
}

void BloodDripBehavior::setMaxDripSize(float size) {
    m_maxDripSize = size;
    RF_LOG_INFO("Blood max drip size set to %.2f", size);
}

void BloodDripBehavior::setDripLifetime(float lifetime) {
    m_dripLifetime = lifetime;
    RF_LOG_INFO("Blood drip lifetime set to %.2f", lifetime);
}

void BloodDripBehavior::setSpreadRadius(float radius) {
    m_spreadRadius = radius;
    RF_LOG_INFO("Blood drip spread radius set to %.2f", radius);
}

void BloodDripBehavior::setDripColor(const Color& color) {
    m_dripColor = color;
    RF_LOG_INFO("Blood drip color set to (%.2f, %.2f, %.2f, %.2f)", 
                color.r, color.g, color.b, color.a);
}

void BloodDripBehavior::setDripOpacity(float opacity) {
    m_dripOpacity = opacity;
    RF_LOG_INFO("Blood drip opacity set to %.2f", opacity);
}

void BloodDripBehavior::setEnabled(bool enabled) {
    m_isEnabled = enabled;
    RF_LOG_INFO("Blood drip behavior %s", enabled ? "enabled" : "disabled");
}

void BloodDripBehavior::setMaxDrips(int maxDrips) {
    m_maxDrips = maxDrips;
    m_drips.reserve(maxDrips);
    RF_LOG_INFO("Blood max drips set to %d", maxDrips);
}

size_t BloodDripBehavior::getDripCount() const {
    return m_dripCount;
}

float BloodDripBehavior::getDripRate() const {
    return m_dripRate;
}

float BloodDavior::getDripSpeed() const {
    return m_dripSpeed;
}

float BloodDripBehavior::getGravity() const {
    return m_gravity;
}

float BloodDripBehavior::getSurfaceTension() const {
    return m_surfaceTension;
}

float BloodDripBehavior::getViscosity() const {
    return m_viscosity;
}

float BloodDripBehavior::getCoagulationRate() const {
    return m_coagulationRate;
}

float BloodDripBehavior::getEvaporationRate() const {
    return m_evaporationRate;
}

float BloodDripBehavior::getMinDripSize() const {
    return m_minDripSize;
}

float BloodDripBehavior::getMaxDripSize() const {
    return m_maxDripSize;
}

float BloodDripBehavior::getDripLifetime() const {
    return m_dripLifetime;
}

float BloodDripBehavior::getSpreadRadius() const {
    return m_spreadRadius;
}

Color BloodDripBehavior::getDripColor() const {
    return m_dripColor;
}

float BloodDripBehavior::getDripOpacity() const {
    return m_dripOpacity;
}

bool BloodDripBehavior::isEnabled() const {
    return m_isEnabled;
}

const std::vector<BloodDrip*>& BloodDripBehavior::getDrips() const {
    return m_drips;
}

void BloodDripBehavior::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& BloodDripBehavior::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles