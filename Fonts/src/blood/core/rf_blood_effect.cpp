#include "rf_blood_effect.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

BloodEffect::BloodEffect()
    : m_effectType(EffectType::Splash)
    , m_effectForce(10.0f)
    , m_effectRadius(2.0f)
    , m_effectIntensity(1.0f)
    , m_effectLifetime(2.0f)
    , m_effectFadeTime(0.5f)
    , m_effectColor(0.8f, 0.1f, 0.1f, 1.0f)
    , m_effectOpacity(0.9f)
    , m_effectTurbulence(0.1f)
    , m_effectGravity(9.81f)
    , m_effectViscosity(0.8f)
    , m_effectCoagulation(0.05f)
    , m_effectEvaporation(0.02f)
    , m_isEnabled(true)
    , m_effectCount(0)
    , m_maxEffects(50)
{
    m_effects.reserve(m_maxEffects);
    RF_LOG_INFO("BloodEffect created");
}

BloodEffect::~BloodEffect() {
    clear();
    RF_LOG_INFO("BloodEffect destroyed");
}

void BloodEffect::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("BloodEffect initialized");
}

void BloodEffect::clear() {
    for (auto& effect : m_effects) {
        if (effect) {
            RF_DELETE(effect);
        }
    }
    
    m_effects.clear();
    m_effectCount = 0;
    RF_LOG_INFO("Cleared all blood effects");
}

void BloodEffect::addEffect(const Vec3& position, const Vec3& velocity, float size, 
                        const Color& color, float lifetime) {
    if (m_effectCount >= m_maxEffects) {
        // Remove oldest effect
        RF_DELETE(m_effects.front());
        m_effects.erase(m_effects.begin());
        m_effectCount--;
    }
    
    auto effect = RF_NEW(BloodEffect);
    effect->initialize(position, velocity, size, color, lifetime, m_properties);
    
    m_effects.push_back(effect);
    m_effectCount++;
    
    RF_LOG_INFO("Added blood effect at position (%.2f, %.2f, %.2f)", 
                position.x, position.y, position.z);
}

void BloodEffect::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update existing effects
    for (auto& effect : m_effects) {
        if (effect) {
            effect->update(deltaTime);
            
            // Remove completed effects
            if (effect->isCompleted()) {
                RF_DELETE(effect);
                m_effects.erase(std::find(m_effects.begin(), m_effects.end(), effect));
                m_effectCount--;
            }
        }
    }
    
    // Generate new effects based on conditions
    updateEffectGeneration(deltaTime);
}

void BloodEffect::updateEffectGeneration(float deltaTime) {
    // Generate new effects based on conditions
    // This would be connected to the effect trigger system
    // For now, just update existing effects
    (void)deltaTime;
}

void BloodEffect::setEffectType(EffectType type) {
    m_effectType = type;
    RF_LOG_INFO("Blood effect type set to %d", static_cast<int>(type));
}

void BloodEffect::setEffectForce(float force) {
    m_effectForce = force;
    RF_LOG_INFO("Blood effect force set to %.2f", force);
}

void BloodEffect::setEffectRadius(float radius) {
    m_effectRadius = radius;
    RF_LOG_INFO("Blood effect radius set to %.2f", radius);
}

void BloodEffect::setEffectIntensity(float intensity) {
    m_effectIntensity = intensity;
    RF_LOG_INFO("Blood effect intensity set to %.2f", intensity);
}

void BloodEffect::setEffectLifetime(float lifetime) {
    m_effectLifetime = lifetime;
    RF_LOG_INFO("Blood effect lifetime set to %.2f", lifetime);
}

void BloodEffect::setEffectFadeTime(float time) {
    m_effectFadeTime = time;
    RF_LOG_INFO("Blood effect fade time set to %.2f", time);
}

void BloodEffect::setEffectColor(const Color& color) {
    m_effectColor = color;
    RF_LOG_INFO("Blood effect color set to (%.2f, %.2f, %.2f, %.2f)", 
                color.r, color.g, color.b, color.a);
}

void BloodEffect::setEffectOpacity(float opacity) {
    m_effectOpacity = opacity;
    RF_LOG_INFO("Blood effect opacity set to %.2f", opacity);
}

void BloodEffect::setEffectTurbulence(float turbulence) {
    m_effectTurbulence = turbulence;
    RF_LOG_INFO("Blood effect turbulence set to %.2f", turbulence);
}

void BloodEffect::setEffectGravity(float gravity) {
    m_effectGravity = gravity;
    RF_LOG_INFO("Blood effect gravity set to %.2f", gravity);
}

void BloodEffect::setEffectViscosity(float viscosity) {
    m_effectViscosity = viscosity;
    RF_LOG_INFO("Blood effect viscosity set to %.2f", viscosity);
}

void BloodEffect::setEffectCoagulation(float coagulation) {
    m_effectCoagulation = coagulation;
    RF_LOG_INFO("Blood effect coagulation set to %.2f", coagulation);
}

void BloodEffect::setEffectEvaporation(float evaporation) {
    m_effectEvaporation = evaporation;
    RF_LOG_INFO("Blood effect evaporation set to %.2f", evaporation);
}

void BloodEffect::setMaxEffects(int maxEffects) {
    m_maxEffects = maxEffects;
    m_effects.reserve(maxEffects);
    RF_LOG_INFO("Blood effect max effects set to %d", maxEffects);
}

size_t BloodEffect::getEffectCount() const {
    return m_effectCount;
}

EffectType BloodEffect::getEffectType() const {
    return m_effectType;
}

float BloodEffect::getEffectForce() const {
    return m_effectForce;
}

float BloodEffect::getEffectRadius() const {
    return m_effectRadius;
}

float BloodEffect::getEffectIntensity() const {
    return m_effectIntensity;
}

float BloodEffect::getEffectLifetime() const {
    return m_effectLifetime;
}

float BloodEffect::getEffectFadeTime() const {
    return m_effectFadeTime;
}

Color BloodEffect::getEffectColor() const {
    return m_effectColor;
}

float BloodEffect::getEffectOpacity() const {
    return m_effectOpacity;
}

float BloodEffect::getEffectTurbulence() const {
    return m_effectTurbulence;
}

float BloodEffect::getEffectGravity() const {
    return m_effectGravity;
}

float BloodEffect::getEffectViscosity() const {
    return m_effectViscosity;
}

float BloodEffect::getEffectCoagulation() const {
    return m_effectCoagulation;
}

float BloodEffect::getEffectEvaporation() const {
    return m_effectEvaporation;
}

bool BloodEffect::isEnabled() const {
    return m_isEnabled;
}

const std::vector<BloodEffect*>& BloodEffect::getEffects() const {
    return m_effects;
}

void BloodEffect::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& BloodEffect::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles