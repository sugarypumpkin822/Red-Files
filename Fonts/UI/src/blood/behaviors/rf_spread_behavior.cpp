#include "rf_spread_behavior.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

SpreadBehavior::SpreadBehavior()
    : m_spreadRate(0.3f)
    , m_spreadForce(1.0f)
    , m_spreadRadius(1.0f)
    , m_spreadThickness(0.05f)
    , m_spreadLifetime(4.0f)
    , m_spreadFadeTime(1.0f)
    , m_spreadColor(0.8f, 0.1f, 0.1f, 1.0f)
    , m_spreadOpacity(0.6f)
    , m_spreadTurbulence(0.02f)
    , m_spreadGravity(9.81f)
    , m_spreadViscosity(0.7f)
    , m_spreadCoagulation(0.08f)
    , m_spreadEvaporation(0.01f)
    , m_isEnabled(true)
    , m_spreadCount(0)
    , m_maxSpreads(20)
{
    m_spreads.reserve(m_maxSpreads);
    RF_LOG_INFO("SpreadBehavior created");
}

SpreadBehavior::~SpreadBehavior() {
    clear();
    RF_LOG_INFO("SpreadBehavior destroyed");
}

void SpreadBehavior::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("SpreadBehavior initialized");
}

void SpreadBehavior::clear() {
    for (auto& spread : m_spreads) {
        if (spread) {
            RF_DELETE(spread);
        }
    }
    
    m_spreads.clear();
    m_spreadCount = 0;
    RF_LOG_INFO("Cleared all spreads");
}

void SpreadBehavior::addSpread(const Vec3& position, const Vec3& velocity, float size, 
                         const Color& color, float lifetime) {
    if (m_spreadCount >= m_maxSpreads) {
        // Remove oldest spread
        RF_DELETE(m_spreads.front());
        m_spreads.erase(m_spreads.begin());
        m_spreadCount--;
    }
    
    auto spread = RF_NEW(Spread);
    spread->initialize(position, velocity, size, color, lifetime, m_properties);
    
    m_spreads.push_back(spread);
    m_spreadCount++;
    
    RF_LOG_INFO("Added spread at position (%.2f, %.2f, %.2f)", 
                position.x, position.y, position.z);
}

void SpreadBehavior::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update existing spreads
    for (auto& spread : m_spreads) {
        if (spread) {
            spread->update(deltaTime);
            
            // Remove completed spreads
            if (spread->isCompleted()) {
                RF_DELETE(spread);
                m_spreads.erase(std::find(m_spreads.begin(), m_spreads.end(), spread));
                m_spreadCount--;
            }
        }
    }
    
    // Generate new spreads based on surface conditions
    updateSpreadGeneration(deltaTime);
}

void SpreadBehavior::updateSpreadGeneration(float deltaTime) {
    // Generate new spreads based on surface conditions
    // This would be connected to the surface system
    // For now, just update existing spreads
    (void)deltaTime;
}

void SpreadBehavior::setSpreadRate(float rate) {
    m_spreadRate = rate;
    RF_LOG_INFO("Spread rate set to %.2f", rate);
}

void SpreadBehavior::setSpreadForce(float force) {
    m_spreadForce = force;
    RF_LOG_INFO("Spread force set to %.2f", force);
}

void SpreadBehavior::setSpreadRadius(float radius) {
    m_spreadRadius = radius;
    RF_LOG_INFO("Spread radius set to %.2f", radius);
}

void SpreadBehavior::setSpreadThickness(float thickness) {
    m_spreadThickness = thickness;
    RF_LOG_INFO("Spread thickness set to %.2f", thickness);
}

void SpreadBehavior::setSpreadLifetime(float lifetime) {
    m_spreadLifetime = lifetime;
    RF_LOG_INFO("Spread lifetime set to %.2f", lifetime);
}

void SpreadBehavior::setSpreadFadeTime(float time) {
    m_spreadFadeTime = time;
    RF_LOG_INFO("Spread fade time set to %.2f", time);
}

void SpreadBehavior::setSpreadColor(const Color& color) {
    m_spreadColor = color;
    RF_LOG_INFO("Spread color set to (%.2f, %.2f, %.2f, %.2f)", 
                color.r, color.g, color.b, color.a);
}

void SpreadBehavior::setSpreadOpacity(float opacity) {
    m_spreadOpacity = opacity;
    RF_LOG_INFO("Spread opacity set to %.2f", opacity);
}

void SpreadBehavior::setSpreadTurbulence(float turbulence) {
    m_spreadTurbulence = turbulence;
    RF_LOG_INFO("Spread turbulence set to %.2f", turbulence);
}

void SpreadBehavior::setSpreadGravity(float gravity) {
    m_spreadGravity = gravity;
    RF_LOG_INFO("Spread gravity set to %.2f", gravity);
}

void SpreadBehavior::setSpreadViscosity(float viscosity) {
    m_spreadViscosity = viscosity;
    RF_LOG_INFO("Spread viscosity set to %.2f", viscosity);
}

void SpreadBehavior::setSpreadCoagulation(float coagulation) {
    m_spreadCoagulation = coagulation;
    RF_LOG_INFO("Spread coagulation set to %.2f", coagulation);
}

void SpreadBehavior::setSpreadEvaporation(float evaporation) {
    m_spreadEvaporation = evaporation;
    RF_LOG_INFO("Spread evaporation set to %.2f", evaporation);
}

void SpreadBehavior::setMaxSpreads(int maxSpreads) {
    m_maxSpreads = maxSpreads;
    m_spreads.reserve(maxSpreads);
    RF_LOG_INFO("Spread max spreads set to %d", maxSpreads);
}

size_t SpreadBehavior::getSpreadCount() const {
    return m_spreadCount;
}

float SpreadBehavior::getSpreadRate() const {
    return m_spreadRate;
}

float SpreadBehavior::getSpreadForce() const {
    return m_spreadForce;
}

float SpreadBehavior::getSpreadRadius() const {
    return m_spreadRadius;
}

float SpreadBehavior::getSpreadThickness() const {
    return m_spreadThickness;
}

float SpreadBehavior::getSpreadLifetime() const {
    return m_spreadLifetime;
}

float SpreadBehavior::getSpreadFadeTime() const {
    return m_spreadFadeTime;
}

Color SpreadBehavior::getSpreadColor() const {
    return m_spreadColor;
}

float SpreadBehavior::getSpreadOpacity() const {
    return m_spreadOpacity;
}

float SpreadBehavior::getSpreadTurbulence() const {
    return m_spreadTurbulence;
}

float SpreadBehavior::getSpreadGravity() const {
    return m_spreadGravity;
}

float SpreadBehavior::getSpreadViscosity() const {
    return m_spreadViscosity;
}

float SpreadBehavior::getSpreadCoagulation() const {
    return m_spreadCoagulation;
}

float SpreadBehavior::getSpreadEvaporation() const {
    return m_spreadEvaporation;
}

bool SpreadBehavior::isEnabled() const {
    return m_isEnabled;
}

const std::vector<Spread*>& SpreadBehavior::getSpreads() const {
    return m_spreads;
}

void SpreadBehavior::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& SpreadBehavior::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles