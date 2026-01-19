#include "rf_edge_emitter.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

EdgeEmitter::EdgeEmitter()
    : m_emitterType(EmitterType::Edge)
    , m_startPosition(0.0f, 0.0f, 0.0f)
    , m_endPosition(1.0f, 0.0f, 0.0f)
    , m_emissionRate(15.0f)
    , m_emissionSpread(0.2f)
    , m_particleLifetime(2.5f)
    , m_particleSize(0.08f)
    , m_particleColor(0.7f, 0.1f, 0.1f, 1.0f)
    , m_particleOpacity(0.7f)
    , m_particleVelocity(3.0f, 0.0f, 0.0f)
    , m_particleAcceleration(0.0f, 0.0f, 0.0f)
    , m_isEnabled(true)
    , m_isLooping(false)
    , m_maxParticles(150)
    , m_particleCount(0)
    , m_emissionTimer(0.0f)
    , m_emissionInterval(0.1f)
    , m_edgeLength(1.0f)
    , m_edgeNormal(0.0f, 1.0f, 0.0f)
    , m_edgeVariation(0.3f)
{
    m_particles.reserve(m_maxParticles);
    RF_LOG_INFO("EdgeEmitter created");
}

EdgeEmitter::~EdgeEmitter() {
    clear();
    RF_LOG_INFO("EdgeEmitter destroyed");
}

void EdgeEmitter::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("EdgeEmitter initialized");
}

void EdgeEmitter::clear() {
    for (auto& particle : m_particles) {
        if (particle) {
            RF_DELETE(particle);
        }
    }
    
    m_particles.clear();
    m_particleCount = 0;
    RF_LOG_INFO("Cleared all edge emitter particles");
}

void EdgeEmitter::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update existing particles
    updateParticles(deltaTime);
    
    // Emit new particles
    updateEmission(deltaTime);
}

void EdgeEmitter::updateParticles(float deltaTime) {
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

void EdgeEmitter::updateEmission(float deltaTime) {
    m_emissionTimer += deltaTime;
    
    if (m_emissionTimer >= m_emissionInterval) {
        emitParticles();
        m_emissionTimer = 0.0f;
    }
}

void EdgeEmitter::emitParticles() {
    if (m_particleCount >= m_maxParticles) {
        return;
    }
    
    // Emit particles along the edge
    int particlesToEmit = static_cast<int>(m_emissionRate);
    for (int i = 0; i < particlesToEmit && m_particleCount < m_maxParticles; ++i) {
        // Generate position along edge
        float t = static_cast<float>(i) / static_cast<float>(particlesToEmit - 1);
        Vec3 edgePosition = Math::lerp(m_startPosition, m_endPosition, t);
        
        // Add variation to position
        Vec3 variation = Vec3(
            Math::random(-m_edgeVariation, m_edgeVariation),
            Math::random(-m_edgeVariation, m_edgeVariation),
            Math::random(-m_edgeVariation, m_edgeVariation)
        );
        
        Vec3 particlePosition = edgePosition + variation;
        
        // Generate velocity along edge with normal component
        Vec3 edgeDirection = Math::normalize(m_endPosition - m_startPosition);
        float normalComponent = Math::dot(m_edgeNormal, edgeDirection);
        
        Vec3 baseVelocity = m_particleVelocity;
        Vec3 velocity = baseVelocity;
        
        // Add normal component to velocity
        velocity += m_edgeNormal * normalComponent * 2.0f;
        
        // Add spread to velocity
        float spreadAngle = Math::random(0.0f, Math::PI * 2.0f);
        float spreadAmount = Math::random(0.0f, m_emissionSpread);
        
        Vec3 spreadVelocity = Vec3(
            Math::sin(spreadAngle) * spreadAmount,
            Math::cos(spreadAngle) * spreadAmount,
            Math::random(-0.1f, 0.1f)
        );
        
        velocity += spreadVelocity;
        
        // Add edge variation to velocity
        velocity += variation * m_edgeVariation;
        
        // Create particle
        auto particle = RF_NEW(BloodParticle);
        particle->initialize(particlePosition, velocity, m_particleSize, m_particleColor, m_particleLifetime, m_properties);
        
        m_particles.push_back(particle);
        m_particleCount++;
    }
    
    RF_LOG_INFO("Emitted %d edge particles", particlesToEmit);
}

void EdgeEmitter::applyPhysicsToParticle(BloodParticle* particle, float deltaTime) {
    if (!particle) {
        return;
    }
    
    // Apply gravity
    Vec3 gravityForce = Vec3(0.0f, -9.81f, 0.0f);
    Vec3 currentVelocity = particle->getVelocity();
    currentVelocity += gravityForce * deltaTime;
    
    // Apply edge-specific physics
    // Particles should stick to the edge surface
    Vec3 edgeDirection = Math::normalize(m_endPosition - m_startPosition);
    Vec3 edgeNormal = m_edgeNormal;
    
    // Calculate distance to edge
    Vec3 particlePos = particle->getPosition();
    Vec3 closestPointOnEdge = m_startPosition + edgeDirection * Math::clamp(Math::dot(particlePos - m_startPosition, edgeDirection), 0.0f, Math::length(m_endPosition - m_startPosition));
    
    // Apply edge attraction if close enough
    float distanceToEdge = Math::length(particlePos - closestPointOnEdge);
    if (distanceToEdge < 0.5f) {
        Vec3 attractionForce = edgeDirection * 5.0f;
        currentVelocity += attractionForce * deltaTime;
    }
    
    // Apply acceleration
    currentVelocity += m_particleAcceleration * deltaTime;
    
    particle->setVelocity(currentVelocity);
    
    // Update position
    Vec3 currentPosition = particle->getPosition();
    currentPosition += currentVelocity * deltaTime;
    particle->setPosition(currentPosition);
}

void EdgeEmitter::setStartPosition(const Vec3& position) {
    m_startPosition = position;
    RF_LOG_INFO("Edge emitter start position set to (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
}

void EdgeEmitter::setEndPosition(const Vec3& position) {
    m_endPosition = position;
    RF_LOG_INFO("Edge emitter end position set to (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
}

void EdgeEmitter::setEmissionRate(float rate) {
    m_emissionRate = rate;
    RF_LOG_INFO("Edge emitter emission rate set to %.2f", rate);
}

void EdgeEmitter::setEmissionSpread(float spread) {
    m_emissionSpread = spread;
    RF_LOG_INFO("Edge emitter emission spread set to %.2f", spread);
}

void EdgeEmitter::setParticleLifetime(float lifetime) {
    m_particleLifetime = lifetime;
    RF_LOG_INFO("Edge emitter particle lifetime set to %.2f", lifetime);
}

void EdgeEmitter::setParticleSize(float size) {
    m_particleSize = size;
    RF_LOG_INFO("Edge emitter particle size set to %.2f", size);
}

void EdgeEmitter::setParticleColor(const Color& color) {
    m_particleColor = color;
    RF_LOG_INFO("Edge emitter particle color set to (%.2f, %.2f, %.2f, %.2f)", color.r, color.g, color.b, color.a);
}

void EdgeEmitter::setParticleOpacity(float opacity) {
    m_particleOpacity = opacity;
    RF_LOG_INFO("Edge emitter particle opacity set to %.2f", opacity);
}

void EdgeEmitter::setParticleVelocity(const Vec3& velocity) {
    m_particleVelocity = velocity;
    RF_LOG_INFO("Edge emitter particle velocity set to (%.2f, %.2f, %.2f)", velocity.x, velocity.y, velocity.z);
}

void EdgeEmitter::setParticleAcceleration(const Vec3& acceleration) {
    m_particleAcceleration = acceleration;
    RF_LOG_INFO("Edge emitter particle acceleration set to (%.2f, %.2f, %.2f)", acceleration.x, acceleration.y, acceleration.z);
}

void EdgeEmitter::setEnabled(bool enabled) {
    m_isEnabled = enabled;
    RF_LOG_INFO("Edge emitter %s", enabled ? "enabled" : "disabled");
}

void EdgeEmitter::setLooping(bool looping) {
    m_isLooping = looping;
    RF_LOG_INFO("Edge emitter looping set to %s", looping ? "true" : "false");
}

void EdgeEmitter::setMaxParticles(int maxParticles) {
    m_maxParticles = maxParticles;
    m_particles.reserve(maxParticles);
    RF_LOG_INFO("Edge emitter max particles set to %d", maxParticles);
}

void EdgeEmitter::setEmissionInterval(float interval) {
    m_emissionInterval = interval;
    RF_LOG_INFO("Edge emitter emission interval set to %.2f", interval);
}

void EdgeEmitter::setEdgeLength(float length) {
    m_edgeLength = length;
    RF_LOG_INFO("Edge emitter edge length set to %.2f", length);
}

void EdgeEmitter::setEdgeNormal(const Vec3& normal) {
    m_edgeNormal = Math::normalize(normal);
    RF_LOG_INFO("Edge emitter edge normal set to (%.2f, %.2f, %.2f)", normal.x, normal.y, normal.z);
}

void EdgeEmitter::setEdgeVariation(float variation) {
    m_edgeVariation = variation;
    RF_LOG_INFO("Edge emitter edge variation set to %.2f", variation);
}

const Vec3& EdgeEmitter::getStartPosition() const {
    return m_startPosition;
}

const Vec3& EdgeEmitter::getEndPosition() const {
    return m_endPosition;
}

float EdgeEmitter::getEmissionRate() const {
    return m_emissionRate;
}

float EdgeEmitter::getEmissionSpread() const {
    return m_emissionSpread;
}

float EdgeEmitter::getParticleLifetime() const {
    return m_particleLifetime;
}

float EdgeEmitter::getParticleSize() const {
    return m_particleSize;
}

const Color& EdgeEmitter::getParticleColor() const {
    return m_particleColor;
}

float EdgeEmitter::getParticleOpacity() const {
    return m_particleOpacity;
}

const Vec3& EdgeEmitter::getParticleVelocity() const {
    return m_particleVelocity;
}

const Vec3& EdgeEmitter::getParticleAcceleration() const {
    return m_particleAcceleration;
}

bool EdgeEmitter::isEnabled() const {
    return m_isEnabled;
}

bool EdgeEmitter::isLooping() const {
    return m_isLooping;
}

int EdgeEmitter::getMaxParticles() const {
    return m_maxParticles;
}

float EdgeEmitter::getEmissionInterval() const {
    return m_emissionInterval;
}

float EdgeEmitter::getEdgeLength() const {
    return m_edgeLength;
}

const Vec3& EdgeEmitter::getEdgeNormal() const {
    return m_edgeNormal;
}

float EdgeEmitter::getEdgeVariation() const {
    return m_edgeVariation;
}

size_t EdgeEmitter::getParticleCount() const {
    return m_particleCount;
}

const std::vector<BloodParticle*>& EdgeEmitter::getParticles() const {
    return m_particles;
}

void EdgeEmitter::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& EdgeEmitter::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles