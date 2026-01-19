#include "rf_blood_splash.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

BloodSplashBehavior::BloodSplashBehavior()
    : m_splashForce(20.0f)
    , m_splashRadius(1.0f)
    , m_splashCount(0)
    , m_maxSplashes(100)
    , m_isEnabled(true)
    , m_splashLifetime(1.0f)
    , m_splashFadeTime(0.3f)
    , m_splashSpreadAngle(0.0f)
    , m_splashTurbulence(0.2f)
    , m_splashColor(0.8f, 0.1f, 0.1f, 1.0f)
    , m_splashOpacity(0.9f)
    , m_splashSize(2.0f)
    , m_minSplashSize(0.5f)
    , m_maxSplashSize(8.0f)
    , m_splashHeight(0.5f)
    , m_splashDecay(0.8f)
{
    m_splashes.reserve(m_maxSplashes);
    RF_LOG_INFO("BloodSplashBehavior created");
}

BloodSplashBehavior::~BloodSplashBehavior() {
    clear();
    RF_LOG_INFO("BloodSplashBehavior destroyed");
}

void BloodSplashBehavior::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("BloodSplashBehavior initialized");
}

void BloodSplashBehavior::clear() {
    for (auto& splash : m_splashes) {
        if (splash) {
            RF_DELETE(splash);
        }
    }
    
    m_splashes.clear();
    m_splashCount = 0;
    RF_LOG_INFO("Cleared all blood splashes");
}

void BloodSplashBehavior::addSplash(const Vec3& position, const Vec3& velocity, float size, 
                           const Color& color, float lifetime) {
    if (m_splashCount >= m_maxSplashes) {
        // Remove oldest splash
        RF_DELETE(m_splashes.front());
        m_splashes.erase(m_splashes.begin());
        m_splashCount--;
    }
    
    if (size < m_minSplashSize) {
        size = m_minSplashSize;
    }
    
    if (size > m_maxSplashSize) {
        size = m_maxSplashSize;
    }
    
    auto splash = RF_NEW(BloodSplash);
    splash->initialize(position, velocity, size, color, lifetime, m_properties);
    
    m_splashes.push_back(splash);
    m_splashCount++;
    
    RF_LOG_INFO("Added blood splash at position (%.2f, %.2f, %.2f)", 
                position.x, position.y, position.z);
}

void BloodSplashBehavior::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update existing splashes
    for (auto& splash : m_splashes) {
        if (splash) {
            splash->update(deltaTime);
            
            // Remove completed splashes
            if (splash->isCompleted()) {
                RF_DELETE(splash);
                m_splashes.erase(std::find(m_splashes.begin(), m_splashes.end(), splash));
                m_splashCount--;
            }
        }
    }
    
    // Add new splashes based on blood impacts
    updateSplashGeneration(deltaTime);
}

void BloodSplashBehavior::updateSplashGeneration(float deltaTime) {
    // Generate new splashes based on blood impacts
    // This would be connected to the blood impact system
    // For now, just update existing splashes
    (void)deltaTime;
}

void BloodSplashBehavior::setSplashForce(float force) {
    m_splashForce = force;
    RF_LOG_INFO("Blood splash force set to %.2f", force);
}

void BloodSplashBehavior::setSplashRadius(float radius) {
    m_splashRadius = radius;
    RF_LOG_INFO("Blood splash radius set to %.2f", radius);
}

void BloodSplashBehavior::setSplashCount(int count) {
    m_splashCount = count;
    RF_LOG_INFO("Blood splash count set to %d", count);
}

void BloodSplashBehavior::setSplashLifetime(float lifetime) {
    m_splashLifetime = lifetime;
    RF_LOG_INFO("Blood splash lifetime set to %.2f", lifetime);
}

void BloodSplashBehavior::setSplashFadeTime(float time) {
    m_splashFadeTime = time;
    RF_LOG_INFO("Blood splash fade time set to %.2f", time);
}

void BloodSplashBehavior::setSplashSpreadAngle(float angle) {
    m_splashSpreadAngle = angle;
    RF_LOG_INFO("Blood splash spread angle set to %.2f", angle);
}

void BloodSplashBehavior::setSplashTurbulence(float turbulence) {
    m_splashTurbulence = turbulence;
    RF_LOG_INFO("Blood splash turbulence set to %.2f", turbulence);
}

void BloodSplashBehavior::setSplashColor(const Color& color) {
    m_splashColor = color;
    RF_LOG_INFO("Blood splash color set to (%.2f, %.2f, %.2f, %.2f)", 
                color.r, color.g, color.b, color.a);
}

void BloodSplashBehavior::setSplashOpacity(float opacity) {
    m_splashOpacity = opacity;
    RF_LOG_INFO("Blood splash opacity set to %.2f", opacity);
}

void BloodSplashBehavior::setSplashSize(float size) {
    m_splashSize = size;
    RF_LOG_INFO("Blood splash size set to %.2f", size);
}

void BloodSplashBehavior::setMinSplashSize(float size) {
    m_minSplashSize = size;
    RF_LOG_INFO("Blood splash min size set to %.2f", size);
}

void BloodSplashBehavior::setMaxSplashSize(float size) {
    m_maxSplashSize = size;
    RF_LOG_INFO("Blood splash max size set to %.2f", size);
}

void BloodSplashBehavior::setMaxSplashes(int maxSplashes) {
    m_maxSplashes = maxSplashes;
    m_splashes.reserve(maxSplashes);
    RF_LOG_INFO("Blood splash max splashes set to %d", maxSplashes);
}

size_t BloodSplashBehavior::getSplashCount() const {
    return m_splashCount;
}

float BloodSplashBehavior::getSplashForce() const {
    return m_splashForce;
}

float BloodSplashBehavior::getSplashRadius() const {
    return m_splashRadius;
}

float BloodSplashBehavior::getSplashLifetime() const {
    return m_splashLifetime;
}

float BloodSplashBehavior::getSplashFadeTime() const {
    return m_splashFadeTime;
}

float BloodSplashBehavior::getSplashSpreadAngle() const {
    return m_splashSpreadAngle;
}

float BloodSplashBehavior::getSplashTurbulence() const {
    return m_splashTurbulence;
}

Color BloodSplashBehavior::getSplashColor() const {
    return m_splashColor;
}

float BloodSplashBehavior::getSplashOpacity() const {
    return m_splashOpacity;
}

float BloodSplashBehavior::getSplashSize() const {
    return m_splashSize;
}

float BloodSplashBehavior::getMinSplashSize() const {
    return m_minSplashSize;
}

float BloodSplashBehavior::getMaxSplashSize() const {
    return m_maxSplashSize;
}

bool BloodSplashBehavior::isEnabled() const {
    return m_isEnabled;
}

const std::vector<BloodSplash*>& BloodSplashBehavior::getSplashes() const {
    return m_splashes;
}

void BloodSplashBehavior::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& BloodSplashBehavior::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles