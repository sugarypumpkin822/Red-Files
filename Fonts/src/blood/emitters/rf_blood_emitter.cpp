#include "rf_blood_emitter.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

BloodEmitter::BloodEmitter()
    : m_emitterType(EmitterType::Blood)
    , m_position(0.0f, 0.0f, 0.0f)
    , m_direction(0.0f, -1.0f, 0.0f)
    , m_spreadAngle(45.0f)
    , m_emissionRate(20.0f)
    , m_emissionSpread(0.3f)
    , m_particleLifetime(2.0f)
    , m_particleSize(0.05f)
    , m_particleColor(0.8f, 0.1f, 0.1f, 1.0f)
    , m_particleOpacity(0.9f)
    , m_particleVelocity(2.0f, 0.0f, 0.0f)
    , m_particleAcceleration(0.0f, 0.0f, 0.0f)
    , m_isEnabled(true)
    , m_isLooping(false)
    , m_maxParticles(200)
    , m_particleCount(0)
    , m_emissionTimer(0.0f)
    , m_emissionInterval(0.05f)
    , m_bloodVolume(1.0f)
    , m_bloodPressure(1.0f)
    , m_bloodTemperature(37.0f)
    , m_bloodViscosity(0.8f)
{
    m_particles.reserve(m_maxParticles);
    RF_LOG_INFO("BloodEmitter created");
}

BloodEmitter::~BloodEmitter() {
    clear();
    RF_LOG_INFO("BloodEmitter destroyed");
}

void BloodEmitter::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("BloodEmitter initialized");
}

void BloodEmitter::clear() {
    for (auto& particle : m_particles) {
        if (particle) {
            RF_DELETE(particle);
        }
    }
    
    m_particles.clear();
    m_particleCount = 0;
    RF_LOG_INFO("Cleared all blood emitter particles");
}

void BloodEmitter::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update existing particles
    updateParticles(deltaTime);
    
    // Emit new particles
    updateEmission(deltaTime);
}

void BloodEmitter::updateParticles(float deltaTime) {
    for (auto& particle : m_particles) {
        if (particle) {
            particle->update(deltaTime);
            
            // Apply physics to particle
            applyPhysicsToParticle(particle, deltaTime);
            
            // Remove completed particles
            if (particle->isCompleted()) {
                RF_DELETE(particle);
                m_particles.erase(std::find(m_particles.begin(), m_particles.end(), particle));
                m_particleCount--;
            }
        }
    }
}

void BloodEmitter::updateEmission(float deltaTime) {
    m_emissionTimer += deltaTime;
    
    if (m_emissionTimer >= m_emissionInterval) {
        emitParticles();
        m_emissionTimer = 0.0f;
    }
}

void BloodEmitter::emitParticles() {
    if (m_particleCount >= m_maxParticles) {
        return;
    }
    
    // Emit particles based on blood properties
    int particlesToEmit = static_cast<int>(m_emissionRate);
    for (int i = 0; i < particlesToEmit && m_particleCount < m_maxParticles; ++i) {
        // Generate particle with blood-specific properties
        Vec3 particlePosition = m_position;
        Vec3 particleVelocity = m_direction;
        
        // Add spread to velocity
        float spreadAngle = Math::random(-m_spreadAngle, m_spreadAngle);
        float spreadAmount = Math::random(0.0f, m_emissionSpread);
        
        Vec3 spreadVelocity = Vec3(
            Math::sin(spreadAngle) * spreadAmount,
            Math::cos(spreadAngle) * spreadAmount,
            Math::random(-0.1f, 0.1f)
        );
        
        particleVelocity += spreadVelocity;
        
        // Apply temperature effects to velocity
        float temperatureEffect = (m_bloodTemperature - 20.0f) * 0.01f;
        particleVelocity.y += temperatureEffect;
        
        // Apply viscosity effects
        particleVelocity *= (1.0f - m_bloodViscosity * 0.1f);
        
        // Create particle
        auto particle = RF_NEW(BloodParticle);
        particle->initialize(particlePosition, particleVelocity, m_particleSize, m_particleColor, m_particleLifetime, m_properties);
        
        // Set blood-specific properties
        particle->setBloodVolume(m_bloodVolume);
        particle->setBloodPressure(m_bloodPressure);
        particle->setBloodTemperature(m_bloodTemperature);
        particle->setBloodViscosity(m_bloodViscosity);
        
        m_particles.push_back(particle);
        m_particleCount++;
    }
    
    RF_LOG_INFO("Emitted %d blood particles", particlesToEmit);
}

void BloodEmitter::applyPhysicsToParticle(BloodParticle* particle, float deltaTime) {
    if (!particle) {
        return;
    }
    
    // Apply gravity
    Vec3 gravityForce = Vec3(0.0f, -9.81f, 0.0f);
    Vec3 currentVelocity = particle->getVelocity();
    currentVelocity += gravityForce * deltaTime;
    
    // Apply blood-specific physics
    float bloodVolume = particle->getBloodVolume();
    float bloodPressure = particle->getBloodPressure();
    float bloodTemperature = particle->getBloodTemperature();
    float bloodViscosity = particle->getBloodViscosity();
    
    // Apply volume effects
    currentVelocity *= (1.0f + bloodVolume * 0.01f);
    
    // Apply pressure effects
    currentVelocity *= (1.0f + bloodPressure * 0.001f);
    
    // Apply temperature effects
    float temperatureEffect = (bloodTemperature - 20.0f) * 0.02f;
    currentVelocity += Vec3(temperatureEffect, 0.0f, 0.0f) * deltaTime;
    
    // Apply viscosity effects
    currentVelocity *= (1.0f - bloodViscosity * 0.05f);
    
    // Apply acceleration
    currentVelocity += m_particleAcceleration * deltaTime;
    
    particle->setVelocity(currentVelocity);
    
    // Update position
    Vec3 currentPosition = particle->getPosition();
    currentPosition += currentVelocity * deltaTime;
    particle->setPosition(currentPosition);
}

void BloodEmitter::setPosition(const Vec3& position) {
    m_position = position;
    RF_LOG_INFO("Blood emitter position set to (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
}

void BloodEmitter::setDirection(const Vec3& direction) {
    m_direction = Math::normalize(direction);
    RF_LOG_INFO("Blood emitter direction set to (%.2f, %.2f, %.2f)", direction.x, direction.y, direction.z);
}

void BloodEmitter::setSpreadAngle(float angle) {
    m_spreadAngle = angle;
    RF_LOG_INFO("Blood emitter spread angle set to %.2f", angle);
}

void BloodEmitter::setEmissionRate(float rate) {
    m_emissionRate = rate;
    RF_LOG_INFO("Blood emitter emission rate set to %.2f", rate);
}

void BloodEmitter::setEmissionSpread(float spread) {
    m_emissionSpread = spread;
    RF_LOG_INFO("Blood emitter emission spread set to %.2f", spread);
}

void BloodEmitter::setParticleLifetime(float lifetime) {
    m_particleLifetime = lifetime;
    RF_LOG_INFO("Blood emitter particle lifetime set to %.2f", lifetime);
}

void BloodEmitter::setParticleSize(float size) {
    m_particleSize = size;
    RF_LOG_INFO("Blood emitter particle size set to %.2f", size);
}

void BloodEmitter::setParticleColor(const Color& color) {
    m_particleColor = color;
    RF_LOG_INFO("Blood emitter particle color set to (%.2f, %.2f, %.2f, %.2f)", color.r, color.g, color.b, color.a);
}

void BloodEmitter::setParticleOpacity(float opacity) {
    m_particleOpacity = opacity;
    RF_LOG_INFO("Blood emitter particle opacity set to %.2f", opacity);
}

void BloodEmitter::setParticleVelocity(const Vec3& velocity) {
    m_particleVelocity = velocity;
    RF_LOG_INFO("Blood emitter particle velocity set to (%.2f, %.2f, %.2f)", velocity.x, velocity.y, velocity.z);
}

void BloodEmitter::setParticleAcceleration(const Vec3& acceleration) {
    m_particleAcceleration = acceleration;
    RF_LOG_INFO("Blood emitter particle acceleration set to (%.2f, %.2f, %.2f)", acceleration.x, acceleration.y, acceleration.z);
}

void BloodEmitter::setEnabled(bool enabled) {
    m_isEnabled = enabled;
    RF_LOG_INFO("Blood emitter %s", enabled ? "enabled" : "disabled");
}

void BloodEmitter::setLooping(bool looping) {
    m_isLooping = looping;
    RF_LOG_INFO("Blood emitter looping set to %s", looping ? "true" : "false");
}

void BloodEmitter::setMaxParticles(int maxParticles) {
    m_maxParticles = maxParticles;
    m_particles.reserve(maxParticles);
    RF_LOG_INFO("Blood emitter max particles set to %d", maxParticles);
}

void BloodEmitter::setEmissionInterval(float interval) {
    m_emissionInterval = interval;
    RF_LOG_INFO("Blood emitter emission interval set to %.2f", interval);
}

void BloodEmitter::setBloodVolume(float volume) {
    m_bloodVolume = volume;
    RF_LOG_INFO("Blood emitter blood volume set to %.2f", volume);
}

void BloodEmitter::setBloodPressure(float pressure) {
    m_bloodPressure = pressure;
    RF_LOG_INFO("Blood emitter blood pressure set to %.2f", pressure);
}

void BloodEmitter::setBloodTemperature(float temperature) {
    m_bloodTemperature = temperature;
    RF_LOG_INFO("Blood emitter blood temperature set to %.2f", temperature);
}

void BloodEmitter::setBloodViscosity(float viscosity) {
    m_bloodViscosity = viscosity;
    RF_LOG_INFO("Blood emitter blood viscosity set to %.2f", viscosity);
}

const Vec3& BloodEmitter::getPosition() const {
    return m_position;
}

const Vec3& BloodEmitter::getDirection() const {
    return m_direction;
}

float BloodEmitter::getSpreadAngle() const {
    return m_spreadAngle;
}

float BloodEmitter::getEmissionRate() const {
    return m_emissionRate;
}

float BloodEmitter::getEmissionSpread() const {
    return m_emissionSpread;
}

float BloodEmitter::getParticleLifetime() const {
    return m_particleLifetime;
}

float BloodEmitter::getParticleSize() const {
    return m_particleSize;
}

const Color& BloodEmitter::getParticleColor() const {
    return m_particleColor;
}

float BloodEmitter::getParticleOpacity() const {
    return m_particleOpacity;
}

const Vec3& BloodEmitter::getParticleVelocity() const {
    return m_particleVelocity;
}

const Vec3& BloodEmitter::getParticleAcceleration() const {
    return m_particleAcceleration;
}

bool BloodEmitter::isEnabled() const {
    return m_isEnabled;
}

bool BloodEmitter::isLooping() const {
    return m_isLooping;
}

int BloodEmitter::getMaxParticles() const {
    return m_maxParticles;
}

float BloodEmitter::getEmissionInterval() const {
    return m_emissionInterval;
}

float BloodEmitter::getBloodVolume() const {
    return m_bloodVolume;
}

float BloodEmitter::getBloodPressure() const {
    return m_bloodPressure;
}

float BloodEmitter::getBloodTemperature() const {
    return m_bloodTemperature;
}

float BloodEmitter::getBloodViscosity() const {
    return m_bloodViscosity;
}

size_t BloodEmitter::getParticleCount() const {
    return m_particleCount;
}

const std::vector<BloodParticle*>& BloodEmitter::getParticles() const {
    return m_particles;
}

void BloodEmitter::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& BloodEmitter::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles