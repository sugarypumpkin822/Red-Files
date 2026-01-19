#include "rf_point_emitter.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

PointEmitter::PointEmitter()
    : m_emitterType(EmitterType::Point)
    , m_position(0.0f, 0.0f, 0.0f)
    , m_direction(0.0f, -1.0f, 0.0f)
    , m_spreadAngle(90.0f)
    , m_emissionRate(30.0f)
    , m_emissionSpread(0.5f)
    , m_particleLifetime(1.0f)
    , m_particleSize(0.02f)
    , m_particleColor(0.9f, 0.1f, 0.1f, 1.0f)
    , m_particleOpacity(1.0f)
    , m_particleVelocity(5.0f, 0.0f, 0.0f)
    , m_particleAcceleration(0.0f, 0.0f, 0.0f)
    , m_isEnabled(true)
    , m_isLooping(false)
    , m_maxParticles(500)
    , m_particleCount(0)
    , m_emissionTimer(0.0f)
    , m_emissionInterval(0.02f)
    , m_coneAngle(45.0f)
    , m_radialEmission(false)
    , m_sphericalEmission(false)
{
    m_particles.reserve(m_maxParticles);
    RF_LOG_INFO("PointEmitter created");
}

PointEmitter::~PointEmitter() {
    clear();
    RF_LOG_INFO("PointEmitter destroyed");
}

void PointEmitter::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("PointEmitter initialized");
}

void PointEmitter::clear() {
    for (auto& particle : m_particles) {
        if (particle) {
            RF_DELETE(particle);
        }
    }
    
    m_particles.clear();
    m_particleCount = 0;
    RF_LOG_INFO("Cleared all point emitter particles");
}

void PointEmitter::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update existing particles
    updateParticles(deltaTime);
    
    // Emit new particles
    updateEmission(deltaTime);
}

void PointEmitter::updateParticles(float deltaTime) {
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

void PointEmitter::updateEmission(float deltaTime) {
    m_emissionTimer += deltaTime;
    
    if (m_emissionTimer >= m_emissionInterval) {
        emitParticles();
        m_emissionTimer = 0.0f;
    }
}

void PointEmitter::emitParticles() {
    if (m_particleCount >= m_maxParticles) {
        return;
    }
    
    // Emit particles from point
    int particlesToEmit = static_cast<int>(m_emissionRate);
    for (int i = 0; i < particlesToEmit && m_particleCount < m_maxParticles; ++i) {
        Vec3 particlePosition = m_position;
        Vec3 particleVelocity = m_direction;
        
        if (m_sphericalEmission) {
            // Spherical emission
            float theta = Math::random(0.0f, Math::PI * 2.0f);
            float phi = Math::random(0.0f, Math::PI);
            float speed = Math::random(0.5f, 1.5f) * Math::length(m_particleVelocity);
            
            particleVelocity = Vec3(
                Math::sin(phi) * Math::cos(theta) * speed,
                Math::sin(phi) * Math::sin(theta) * speed,
                Math::cos(phi) * speed
            );
        } else if (m_radialEmission) {
            // Radial emission
            float angle = Math::random(0.0f, Math::PI * 2.0f);
            float speed = Math::random(0.5f, 1.5f) * Math::length(m_particleVelocity);
            
            particleVelocity = Vec3(
                Math::cos(angle) * speed,
                Math::sin(angle) * speed,
                m_particleVelocity.z + Math::random(-0.5f, 0.5f)
            );
        } else {
            // Cone emission
            float angle = Math::random(-m_coneAngle * 0.5f, m_coneAngle * 0.5f);
            float speed = Math::random(0.5f, 1.5f) * Math::length(m_particleVelocity);
            
            // Rotate direction by angle around Y axis
            Vec3 rotatedDirection = Vec3(
                m_direction.x * Math::cos(angle) - m_direction.z * Math::sin(angle),
                m_direction.y,
                m_direction.x * Math::sin(angle) + m_direction.z * Math::cos(angle)
            );
            
            particleVelocity = rotatedDirection * speed;
        }
        
        // Add spread to velocity
        float spreadAmount = Math::random(0.0f, m_emissionSpread);
        Vec3 spreadVelocity = Vec3(
            Math::random(-1.0f, 1.0f) * spreadAmount,
            Math::random(-1.0f, 1.0f) * spreadAmount,
            Math::random(-1.0f, 1.0f) * spreadAmount
        );
        
        particleVelocity += spreadVelocity;
        
        // Create particle
        auto particle = RF_NEW(BloodParticle);
        particle->initialize(particlePosition, particleVelocity, m_particleSize, m_particleColor, m_particleLifetime, m_properties);
        
        m_particles.push_back(particle);
        m_particleCount++;
    }
    
    RF_LOG_INFO("Emitted %d point particles", particlesToEmit);
}

void PointEmitter::applyPhysicsToParticle(BloodParticle* particle, float deltaTime) {
    if (!particle) {
        return;
    }
    
    // Apply gravity
    Vec3 gravityForce = Vec3(0.0f, -9.81f, 0.0f);
    Vec3 currentVelocity = particle->getVelocity();
    currentVelocity += gravityForce * deltaTime;
    
    // Apply acceleration
    currentVelocity += m_particleAcceleration * deltaTime;
    
    particle->setVelocity(currentVelocity);
    
    // Update position
    Vec3 currentPosition = particle->getPosition();
    currentPosition += currentVelocity * deltaTime;
    particle->setPosition(currentPosition);
}

void PointEmitter::setPosition(const Vec3& position) {
    m_position = position;
    RF_LOG_INFO("Point emitter position set to (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
}

void PointEmitter::setDirection(const Vec3& direction) {
    m_direction = Math::normalize(direction);
    RF_LOG_INFO("Point emitter direction set to (%.2f, %.2f, %.2f)", direction.x, direction.y, direction.z);
}

void PointEmitter::setSpreadAngle(float angle) {
    m_spreadAngle = angle;
    RF_LOG_INFO("Point emitter spread angle set to %.2f", angle);
}

void PointEmitter::setEmissionRate(float rate) {
    m_emissionRate = rate;
    RF_LOG_INFO("Point emitter emission rate set to %.2f", rate);
}

void PointEmitter::setEmissionSpread(float spread) {
    m_emissionSpread = spread;
    RF_LOG_INFO("Point emitter emission spread set to %.2f", spread);
}

void PointEmitter::setParticleLifetime(float lifetime) {
    m_particleLifetime = lifetime;
    RF_LOG_INFO("Point emitter particle lifetime set to %.2f", lifetime);
}

void PointEmitter::setParticleSize(float size) {
    m_particleSize = size;
    RF_LOG_INFO("Point emitter particle size set to %.2f", size);
}

void PointEmitter::setParticleColor(const Color& color) {
    m_particleColor = color;
    RF_LOG_INFO("Point emitter particle color set to (%.2f, %.2f, %.2f, %.2f)", color.r, color.g, color.b, color.a);
}

void PointEmitter::setParticleOpacity(float opacity) {
    m_particleOpacity = opacity;
    RF_LOG_INFO("Point emitter particle opacity set to %.2f", opacity);
}

void PointEmitter::setParticleVelocity(const Vec3& velocity) {
    m_particleVelocity = velocity;
    RF_LOG_INFO("Point emitter particle velocity set to (%.2f, %.2f, %.2f)", velocity.x, velocity.y, velocity.z);
}

void PointEmitter::setParticleAcceleration(const Vec3& acceleration) {
    m_particleAcceleration = acceleration;
    RF_LOG_INFO("Point emitter particle acceleration set to (%.2f, %.2f, %.2f)", acceleration.x, acceleration.y, acceleration.z);
}

void PointEmitter::setEnabled(bool enabled) {
    m_isEnabled = enabled;
    RF_LOG_INFO("Point emitter %s", enabled ? "enabled" : "disabled");
}

void PointEmitter::setLooping(bool looping) {
    m_isLooping = looping;
    RF_LOG_INFO("Point emitter looping set to %s", looping ? "true" : "false");
}

void PointEmitter::setMaxParticles(int maxParticles) {
    m_maxParticles = maxParticles;
    m_particles.reserve(maxParticles);
    RF_LOG_INFO("Point emitter max particles set to %d", maxParticles);
}

void PointEmitter::setEmissionInterval(float interval) {
    m_emissionInterval = interval;
    RF_LOG_INFO("Point emitter emission interval set to %.2f", interval);
}

void PointEmitter::setConeAngle(float angle) {
    m_coneAngle = angle;
    RF_LOG_INFO("Point emitter cone angle set to %.2f", angle);
}

void PointEmitter::setRadialEmission(bool radial) {
    m_radialEmission = radial;
    RF_LOG_INFO("Point emitter radial emission set to %s", radial ? "true" : "false");
}

void PointEmitter::setSphericalEmission(bool spherical) {
    m_sphericalEmission = spherical;
    RF_LOG_INFO("Point emitter spherical emission set to %s", spherical ? "true" : "false");
}

const Vec3& PointEmitter::getPosition() const {
    return m_position;
}

const Vec3& PointEmitter::getDirection() const {
    return m_direction;
}

float PointEmitter::getSpreadAngle() const {
    return m_spreadAngle;
}

float PointEmitter::getEmissionRate() const {
    return m_emissionRate;
}

float PointEmitter::getEmissionSpread() const {
    return m_emissionSpread;
}

float PointEmitter::getParticleLifetime() const {
    return m_particleLifetime;
}

float PointEmitter::getParticleSize() const {
    return m_particleSize;
}

const Color& PointEmitter::getParticleColor() const {
    return m_particleColor;
}

float PointEmitter::getParticleOpacity() const {
    return m_particleOpacity;
}

const Vec3& PointEmitter::getParticleVelocity() const {
    return m_particleVelocity;
}

const Vec3& PointEmitter::getParticleAcceleration() const {
    return m_particleAcceleration;
}

bool PointEmitter::isEnabled() const {
    return m_isEnabled;
}

bool PointEmitter::isLooping() const {
    return m_isLooping;
}

int PointEmitter::getMaxParticles() const {
    return m_maxParticles;
}

float PointEmitter::getEmissionInterval() const {
    return m_emissionInterval;
}

float PointEmitter::getConeAngle() const {
    return m_coneAngle;
}

bool PointEmitter::isRadialEmission() const {
    return m_radialEmission;
}

bool PointEmitter::isSphericalEmission() const {
    return m_sphericalEmission;
}

size_t PointEmitter::getParticleCount() const {
    return m_particleCount;
}

const std::vector<BloodParticle*>& PointEmitter::getParticles() const {
    return m_particles;
}

void PointEmitter::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& PointEmitter::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles