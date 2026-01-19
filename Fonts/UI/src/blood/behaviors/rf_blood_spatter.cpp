#include "rf_blood_spatter.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

BloodSpatterBehavior::BloodSpatterBehavior()
    : m_spatterForce(10.0f)
    , m_spatterRadius(0.5f)
    , m_spatterCount(0)
    , m_maxSpatters(200)
    , m_isEnabled(true)
    , m_spatterLifetime(2.0f)
    , m_spatterFadeTime(0.5f)
    , m_spatterSpreadAngle(0.0f)
    , m_spatterTurbulence(0.1f)
    , m_spatterColor(0.8f, 0.1f, 0.1f, 1.0f)
    , m_spatterOpacity(0.8f)
    , m_spatterSize(1.0f)
    , m_minSpatterSize(0.1f)
    m_maxSpatterSize(5.0f)
{
    m_spatters.reserve(m_maxSpatters);
    RF_LOG_INFO("BloodSpatterBehavior created");
}

BloodSpatterBehavior::~BloodSpatterBehavior() {
    clear();
    RF_LOG_INFO("BloodSpatterBehavior destroyed");
}

void BloodSpatterBehavior::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("BloodSpatterBehavior initialized");
}

void BloodSpatterBehavior::clear() {
    for (auto& spatter : m_spatters) {
        if (spatter) {
            RF_DELETE(spatter);
        }
    }
    
    m_spatters.clear();
    m_spatterCount = 0;
    RF_LOG_INFO("Cleared all blood splatters");
}

void BloodSpatterBehavior::addSpatter(const Vec3& position, const Vec3& velocity, float size, 
                          const Color& color, float lifetime) {
    if (m_spatterCount >= m_maxSpatters) {
        // Remove oldest spatter
        RF_DELETE(m_spatters.front());
        m_spatters.erase(m_spatters.begin());
        m_spatterCount--;
    }
    
    if (size < m_minSpatterSize) {
        size = m_minSpatterSize;
    }
    
    if (size > m_maxSpatterSize) {
        size = m_maxSpatterSize;
    }
    
    auto spatter = RF_NEW(BloodSpatter);
    spatter->initialize(position, velocity, size, color, lifetime, m_properties);
    
    m_spatters.push_back(spatter);
    m_spatterCount++;
    
    RF_LOG_INFO("Added blood spatter at position (%.2f, %.2f, %.2f)", 
                position.x, position.y, position.z);
}

void BloodSpatterBehavior::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update existing splatters
    for (auto& spatter : m_spatters) {
        if (spatter) {
            spatter->update(deltaTime);
            
            // Remove completed splatters
            if (spatter->isCompleted()) {
                RF_DELETE(spatter);
                m_spatters.erase(std::find(m_spatters.begin(), m_spatters.end(), spatter));
                m_spatterCount--;
            }
        }
    }
    
    // Add new splatters based on blood impacts
    updateSpatterGeneration(deltaTime);
}

void BloodSpatterBehavior::updateSpatterGeneration(float deltaTime) {
    // Generate new splatters based on blood impacts
    // This would be connected to the blood impact system
    // For now, just update existing splatters
    (void)deltaTime;
}

void BloodSpatterBehavior::setSpatterForce(float force) {
    m_spatterForce = force;
    RF_LOG_INFO("Blood spatter force set to %.2f", force);
}

void BloodSpatterBehavior::setSpatterRadius(float radius) {
    m_spatterRadius = radius;
    RF_LOG_INFO("Blood spatter radius set to %.2f", radius);
}

void BloodSpatterBehavior::setSpatterCount(int count) {
    m_spatterCount = count;
    RF_LOG_INFO("Blood spatter count set to %d", count);
}

void BloodSpatterBehavior::setSpatterLifetime(float lifetime) {
    m_spatterLifetime = lifetime;
    RF_LOG_INFO("Blood spatter lifetime set to %.2f", lifetime);
}

void BloodSpatterBehavior::setSpatterFadeTime(float time) {
    m_spatterFadeTime = time;
    RF_LOG_INFO("Blood spatter fade time set to %.2f", time);
}

void BloodSpatterBehavior::setSpatterSpreadAngle(float angle) {
    m_spatterSpreadAngle = angle;
    RF_LOG_INFO("Blood spatter spread angle set to %.2f", angle);
}

void BloodSpatterBehavior::setSpatterTurbulence(float turbulence) {
    m_spatterTurbulence = turbulence;
    RF_LOG_INFO("Blood spatter turbulence set to %.2f", turbulence);
}

void BloodSpatterBehavior::setSpatterColor(const Color& color) {
    m_spatterColor = color;
    RF_LOG_INFO("Blood spatter color set to (%.2f, %.2f, %.2f, %.2f)", 
                color.r, color.g, color.b, color.a);
}

void BloodSpatterBehavior::setSpatterOpacity(float opacity) {
    m_spatterOpacity = opacity;
    RF_LOG_INFO("Blood spatter opacity set to %.2f", opacity);
}

void BloodSpatterBehavior::setSpatterSize(float size) {
    m_spatterSize = size;
    RF_LOG_INFO("Blood spatter size set to %.2f", size);
}

void BloodSpatterBehavior::setMinSpatterSize(float size) {
    m_minSpatterSize = size;
    RF_LOG_INFO("Blood spatter min size set to %.2f", size);
}

void BloodSpatterBehavior::setMaxSpatterSize(float size) {
    m_maxSpatterSize = size;
    RF_LOG_INFO("Blood spatter max size set to %.2f", size);
}

void BloodSpatterBehavior::setMaxSpatters(int maxSpatters) {
    m_maxSpatters = maxSpatters;
    m_spatters.reserve(maxSpatters);
    RF_LOG_INFO("Blood spatter max splatters set to %d", maxSpatters);
}

size_t BloodSpatterBehavior::getSpatterCount() const {
    return m_spatterCount;
}

float BloodSpatterBehavior::getSpatterForce() const {
    return m_spatterForce;
}

float BloodSpatterBehavior::getSpatterRadius() const {
    return m_spatterRadius;
}

float BloodSpatterBehavior::getSpatterLifetime() const {
    return m_spatterLifetime;
}

float BloodSpatterBehavior::getSpatterFadeTime() const {
    return m_spatterFadeTime;
}

float BloodSpatterBehavior::getSpatterSpreadAngle() const {
    return m_spatterSpreadAngle;
}

float BloodSpatterBehavior::getSpatterTurbulence() const {
    return m_spatterTurbulence;
}

Color BloodSpatterBehavior::getSpatterColor() const {
    return m_spatterColor;
}

float BloodSpatterBehavior::getSpatterOpacity() const {
    return m_spatterOpacity;
}

float BloodSpatterBehavior::getSpatterSize() const {
    return m_spatterSize;
}

float BloodSpatterBehavior::getMaxSpatterSize() const {
    return m_maxSpatterSize;
}

bool BloodSpatterBehavior::isEnabled() const {
    return m_isEnabled;
}

const std::vector<BloodSpatter*>& BloodSpatterBehavior::getSpatters() const {
    return m_spatters;
}

void BloodSpatterBehavior::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& BloodSpatterBehavior::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles