#include "rf_drip_behavior.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

DripBehavior::DripBehavior()
    : m_dripRate(0.5f)
    , m_dripForce(1.0f)
    , m_dripSize(0.2f)
    , m_dripLifetime(3.0f)
    , m_dripFadeTime(0.5f)
    , m_dripColor(0.8f, 0.1f, 0.1f, 1.0f)
    , m_dripOpacity(0.8f)
    , m_dripTurbulence(0.1f)
    , m_dripGravity(9.81f)
    , m_dripViscosity(0.5f)
    , m_dripCoagulation(0.05f)
    , m_isEnabled(true)
    , m_dripCount(0)
    , m_maxDrips(50)
{
    m_drips.reserve(m_maxDrips);
    RF_LOG_INFO("DripBehavior created");
}

DripBehavior::~DripBehavior() {
    clear();
    RF_LOG_INFO("DripBehavior destroyed");
}

void DripBehavior::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("DripBehavior initialized");
}

void DripBehavior::clear() {
    for (auto& drip : m_drips) {
        if (drip) {
            RF_DELETE(drip);
        }
    }
    
    m_drips.clear();
    m_dripCount = 0;
    RF_LOG_INFO("Cleared all drips");
}

void DripBehavior::addDrip(const Vec3& position, const Vec3& velocity, float size, 
                       const Color& color, float lifetime) {
    if (m_dripCount >= m_maxDrips) {
        // Remove oldest drip
        RF_DELETE(m_drips.front());
        m_drips.erase(m_drips.begin());
        m_dripCount--;
    }
    
    auto drip = RF_NEW(Drip);
    drip->initialize(position, velocity, size, color, lifetime, m_properties);
    
    m_drips.push_back(drip);
    m_dripCount++;
    
    RF_LOG_INFO("Added drip at position (%.2f, %.2f, %.2f)", 
                position.x, position.y, position.z);
}

void DripBehavior::update(float deltaTime) {
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
    
    // Generate new drips based on surface conditions
    updateDripGeneration(deltaTime);
}

void DripBehavior::updateDripGeneration(float deltaTime) {
    // Generate new drips based on surface conditions
    // This would be connected to the surface system
    // For now, just update existing drips
    (void)deltaTime;
}

void DripBehavior::setDripRate(float rate) {
    m_dripRate = rate;
    RF_LOG_INFO("Drip rate set to %.2f", rate);
}

void DripBehavior::setDripForce(float force) {
    m_dripForce = force;
    RF_LOG_INFO("Drip force set to %.2f", force);
}

void DripBehavior::setDripSize(float size) {
    m_dripSize = size;
    RF_LOG_INFO("Drip size set to %.2f", size);
}

void DripBehavior::setDripLifetime(float lifetime) {
    m_dripLifetime = lifetime;
    RF_LOG_INFO("Drip lifetime set to %.2f", lifetime);
}

void DripBehavior::setDripFadeTime(float time) {
    m_dripFadeTime = time;
    RF_LOG_INFO("Drip fade time set to %.2f", time);
}

void DripBehavior::setDripColor(const Color& color) {
    m_dripColor = color;
    RF_LOG_INFO("Drip color set to (%.2f, %.2f, %.2f, %.2f)", 
                color.r, color.g, color.b, color.a);
}

void DripBehavior::setDripOpacity(float opacity) {
    m_dripOpacity = opacity;
    RF_LOG_INFO("Drip opacity set to %.2f", opacity);
}

void DripBehavior::setDripTurbulence(float turbulence) {
    m_dripTurbulence = turbulence;
    RF_LOG_INFO("Drip turbulence set to %.2f", turbulence);
}

void DripBehavior::setDripGravity(float gravity) {
    m_dripGravity = gravity;
    RF_LOG_INFO("Drip gravity set to %.2f", gravity);
}

void DripBehavior::setDripViscosity(float viscosity) {
    m_dripViscosity = viscosity;
    RF_LOG_INFO("Drip viscosity set to %.2f", viscosity);
}

void DripBehavior::setDripCoagulation(float coagulation) {
    m_dripCoagulation = coagulation;
    RF_LOG_INFO("Drip coagulation set to %.2f", coagulation);
}

void DripBehavior::setMaxDrips(int maxDrips) {
    m_maxDrips = maxDrips;
    m_drips.reserve(maxDrips);
    RF_LOG_INFO("Drip max drips set to %d", maxDrips);
}

size_t DripBehavior::getDripCount() const {
    return m_dripCount;
}

float DripBehavior::getDripRate() const {
    return m_dripRate;
}

float DripBehavior::getDripForce() const {
    return m_dripForce;
}

float DripBehavior::getDripSize() const {
    return m_dripSize;
}

float DripBehavior::getDripLifetime() const {
    return m_dripLifetime;
}

float DripBehavior::getDripFadeTime() const {
    return m_dripFadeTime;
}

Color DripBehavior::getDripColor() const {
    return m_dripColor;
}

float DripBehavior::getDripOpacity() const {
    return m_dripOpacity;
}

float DripBehavior::getDripTurbulence() const {
    return m_dripTurbulence;
}

float DripBehavior::getDripGravity() const {
    return m_dripGravity;
}

float DripBehavior::getDripViscosity() const {
    return m_dripViscosity;
}

float DripBehavior::getDripCoagulation() const {
    return m_dripCoagulation;
}

bool DripBehavior::isEnabled() const {
    return m_isEnabled;
}

const std::vector<Drip*>& DripBehavior::getDrips() const {
    return m_drips;
}

void DripBehavior::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& DripBehavior::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles