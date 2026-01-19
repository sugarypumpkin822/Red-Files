#include "rf_blood_trail.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

BloodTrailBehavior::BloodTrailBehavior()
    : m_trailForce(2.0f)
    , m_trailRadius(0.2f)
    , m_trailLength(5.0f)
    , m_trailCount(0)
    , m_maxTrails(100)
    , m_isEnabled(true)
    , m_trailLifetime(2.0f)
    , m_trailFadeTime(0.3f)
    , m_trailWidth(0.1f)
    , m_trailColor(0.8f, 0.1f, 0.1f, 1.0f)
    , m_trailOpacity(0.7f)
    , m_trailSpeed(1.0f)
    , m_trailTurbulence(0.02f)
    , m_trailGravity(9.81f)
    , m_trailViscosity(0.6f)
    , m_trailCoagulation(0.05f)
    , m_trailSegmentCount(10)
    , m_trailSegmentSpacing(0.1f)
{
    m_trails.reserve(m_maxTrails);
    RF_LOG_INFO("BloodTrailBehavior created");
}

BloodTrailBehavior::~BloodTrailBehavior() {
    clear();
    RF_LOG_INFO("BloodTrailBehavior destroyed");
}

void BloodTrailBehavior::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("BloodTrailBehavior initialized");
}

void BloodTrailBehavior::clear() {
    for (auto& trail : m_trails) {
        if (trail) {
            RF_DELETE(trail);
        }
    }
    
    m_trails.clear();
    m_trailCount = 0;
    RF_LOG_INFO("Cleared all blood trails");
}

void BloodTrailBehavior::addTrail(const Vec3& position, const Vec3& velocity, float size, 
                           const Color& color, float lifetime) {
    if (m_trailCount >= m_maxTrails) {
        // Remove oldest trail
        RF_DELETE(m_trails.front());
        m_trails.erase(m_trails.begin());
        m_trailCount--;
    }
    
    auto trail = RF_NEW(BloodTrail);
    trail->initialize(position, velocity, size, color, lifetime, m_properties);
    
    m_trails.push_back(trail);
    m_trailCount++;
    
    RF_LOG_INFO("Added blood trail at position (%.2f, %.2f, %.2f)", 
                position.x, position.y, position.z);
}

void BloodTrailBehavior::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update existing trails
    for (auto& trail : m_trails) {
        if (trail) {
            trail->update(deltaTime);
            
            // Remove completed trails
            if (trail->isCompleted()) {
                RF_DELETE(trail);
                m_trails.erase(std::find(m_trails.begin(), m_trails.end(), trail));
                m_trailCount--;
            }
        }
    }
    
    // Add new trails based on blood movement
    updateTrailGeneration(deltaTime);
}

void BloodTrailBehavior::updateTrailGeneration(float deltaTime) {
    // Generate new trails based on blood movement
    // This would be connected to the blood movement system
    // For now, just update existing trails
    (void)deltaTime;
}

void BloodTrailBehavior::setTrailForce(float force) {
    m_trailForce = force;
    RF_LOG_INFO("Blood trail force set to %.2f", force);
}

void BloodTrailBehavior::setTrailRadius(float radius) {
    m_trailRadius = radius;
    RF_LOG_INFO("Blood trail radius set to %.2f", radius);
}

void BloodTrailBehavior::setTrailLength(float length) {
    m_trailLength = length;
    RF_LOG_INFO("Blood trail length set to %.2f", length);
}

void BloodTrailBehavior::setTrailCount(int count) {
    m_trailCount = count;
    RF_LOG_INFO("Blood trail count set to %d", count);
}

void BloodTrailBehavior::setTrailLifetime(float lifetime) {
    m_trailLifetime = lifetime;
    RF_LOG_INFO("Blood trail lifetime set to %.2f", lifetime);
}

void BloodTrailBehavior::setTrailFadeTime(float time) {
    m_trailFadeTime = time;
    RF_LOG_INFO("Blood trail fade time set to %.2f", time);
}

void BloodTrailBehavior::setTrailWidth(float width) {
    m_trailWidth = width;
    RF_LOG_INFO("Blood trail width set to %.2f", width);
}

void BloodTrailBehavior::setTrailColor(const Color& color) {
    m_trailColor = color;
    RF_LOG_INFO("Blood trail color set to (%.2f, %.2f, %.2f, %.2f)", 
                color.r, color.g, color.b, color.a);
}

void BloodTrailBehavior::setTrailOpacity(float opacity) {
    m_trailOpacity = opacity;
    RF_LOG_INFO("Blood trail opacity set to %.2f", opacity);
}

void BloodTrailBehavior::setTrailSpeed(float speed) {
    m_trailSpeed = speed;
    RF_LOG_INFO("Blood trail speed set to %.2f", speed);
}

void BloodTrailBehavior::setTrailTurbulence(float turbulence) {
    m_trailTurbulence = turbulence;
    RF_LOG_INFO("Blood trail turbulence set to %.2f", turbulence);
}

void BloodTrailBehavior::setTrailGravity(float gravity) {
    m_trailGravity = gravity;
    RF_LOG_INFO("Blood trail gravity set to %.2f", gravity);
}

void BloodTrailBehavior::setTrailViscosity(float viscosity) {
    m_trailViscosity = viscosity;
    RF_LOG_INFO("Blood trail viscosity set to %.2f", viscosity);
}

void BloodTrailBehavior::setTrailCoagulation(float coagulation) {
    m_trailCoagulation = coagulation;
    RF_LOG_INFO("Blood trail coagulation set to %.2f", coagulation);
}

void BloodTrailBehavior::setTrailSegmentCount(int count) {
    m_trailSegmentCount = count;
    RF_LOG_INFO("Blood trail segment count set to %d", count);
}

void BloodTrailBehavior::setTrailSegmentSpacing(float spacing) {
    m_trailSegmentSpacing = spacing;
    RF_LOG_INFO("Blood trail segment spacing set to %.2f", spacing);
}

void BloodTrailBehavior::setMaxTrails(int maxTrails) {
    m_maxTrails = maxTrails;
    m_trails.reserve(maxTrails);
    RF_LOG_INFO("Blood trail max trails set to %d", maxTrails);
}

size_t BloodTrailBehavior::getTrailCount() const {
    return m_trailCount;
}

float BloodTrailBehavior::getTrailForce() const {
    return m_trailForce;
}

float BloodTrailBehavior::getTrailRadius() const {
    return m_trailRadius;
}

float BloodTrailBehavior::getTrailLength() const {
    return m_trailLength;
}

float BloodTrailBehavior::getTrailLifetime() const {
    return m_trailLifetime;
}

float BloodTrailBehavior::getTrailFadeTime() const {
    return m_trailFadeTime;
}

float BloodTrailBehavior::getTrailWidth() const {
    return m_trailWidth;
}

Color BloodTrailBehavior::getTrailColor() const {
    return m_trailColor;
}

float BloodTrailBehavior::getTrailOpacity() const {
    return m_trailOpacity;
}

float BloodTrailBehavior::getTrailSpeed() const {
    return m_trailSpeed;
}

float BloodTrailBehavior::getTrailTurbulence() const {
    return m_trailTurbulence;
}

float BloodTrailBehavior::getTrailGravity() const {
    return m_trailGravity;
}

float BloodTrailBehavior::getTrailViscosity() const {
    return m_trailViscosity;
}

float BloodTrailBehavior::getTrailCoagulation() const {
    return m_trailCoagulation;
}

int BloodTrailBehavior::getTrailSegmentCount() const {
    return m_trailSegmentCount;
}

float BloodTrailBehavior::getTrailSegmentSpacing() const {
    return m_trailSegmentSpacing;
}

bool BloodTrailBehavior::isEnabled() const {
    return m_isEnabled;
}

const std::vector<BloodTrail*>& BloodTrailBehavior::getTrails() const {
    return m_trails;
}

void BloodTrailBehavior::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& BloodTrailBehavior::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles