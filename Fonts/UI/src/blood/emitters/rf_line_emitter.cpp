#include "rf_line_emitter.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

LineEmitter::LineEmitter()
    : m_emitterType(EmitterType::Line)
    , m_startPosition(0.0f, 0.0f, 0.0f)
    , m_endPosition(0.0f, 0.0f, 0.0f)
    , m_emissionRate(25.0f)
    , m_emissionSpread(0.4f)
    , m_particleLifetime(1.5f)
    , m_particleSize(0.03f)
    , m_particleColor(0.8f, 0.1f, 0.1f, 1.0f)
    , m_particleOpacity(0.8f)
    , m_particleVelocity(0.0f, 0.0f, 0.0f)
    , m_particleAcceleration(0.0f, 0.0f, 0.0f)
    , m_isEnabled(true)
    , m_isLooping(false)
    , m_maxParticles(300)
    , m_particleCount(0)
    , m_emissionTimer(0.0f)
    , m_emissionInterval(0.04f)
    , m_lineLength(2.0f)
    , m_lineWidth(0.1f)
    , m_lineVariation(0.2f)
    , m_lineTurbulence(0.1f)
{
    m_particles.reserve(m_maxParticles);
    RF_LOG_INFO("LineEmitter created");
}

LineEmitter::~LineEmitter() {
    clear();
    RF_LOG_INFO("LineEmitter destroyed");
}

void LineEmitter::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("LineEmitter initialized");
}

void LineEmitter::clear() {
    for (auto& particle : m_particles) {
        if (particle) {
            RF_DELETE(particle);
        }
    }
    
    m_particles.clear();
    m_particleCount = 0;
    RF_LOG_INFO("Cleared all line emitter particles");
}

void LineEmitter::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update existing particles
    updateParticles(deltaTime);
    
    // Emit new particles
    updateEmission(deltaTime);
}

void LineEmitter::updateParticles(float deltaTime) {
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

void LineEmitter::updateEmission(float deltaTime) {
    m_emissionTimer += deltaTime;
    
    if (m_emissionTimer >= m_emissionInterval) {
        emitParticles();
        m_emissionTimer = 0.0f;
    }
}

void LineEmitter::emitParticles() {
    if (m_particleCount >= m_maxParticles) {
        return;
    }
    
    // Emit particles along the line
    int particlesToEmit = static_cast<int>(m_emissionRate);
    for (int i = 0; i < particlesToEmit && m_particleCount < m_maxParticles; ++i) {
        // Generate position along line
        float t = static_cast<float>(i) / static_cast<float>(particlesToEmit - 1);
        Vec3 linePosition = Math::lerp(m_startPosition, m_endPosition, t);
        
        // Add variation to position
        Vec3 variation = Vec3(
            Math::random(-m_lineVariation, m_lineVariation),
            Math::random(-m_lineVariation, m_lineVariation),
            Math::random(-m_lineVariation, m_lineVariation)
        );
        
        Vec3 particlePosition = linePosition + variation;
        
        // Add width variation to position
        Vec3 widthVariation = Vec3(
            Math::random(-m_lineWidth * 0.5f, m_lineWidth * 0.5f),
            Math::random(-m_lineWidth * 0.5f, m_lineWidth * 0.5f),
            Math::random(-m_lineWidth * 0.5f, m_lineWidth * 0.5f)
        );
        
        particlePosition += widthVariation;
        
        // Generate velocity along line
        Vec3 lineDirection = Math::normalize(m_endPosition - m_startPosition);
        Vec3 baseVelocity = m_particleVelocity;
        Vec3 velocity = baseVelocity;
        
        // Add line direction to velocity
        velocity += lineDirection * Math::length(m_particleVelocity);
        
        // Add spread to velocity
        float spreadAngle = Math::random(0.0f, Math::PI * 2.0f);
        float spreadAmount = Math::random(0.0f, m_emissionSpread);
        
        Vec3 spreadVelocity = Vec3(
            Math::sin(spreadAngle) * spreadAmount,
            Math::cos(spreadAngle) * spreadAmount,
            Math::random(-0.1f, 0.1f)
        );
        
        velocity += spreadVelocity;
        
        // Add turbulence to velocity
        float turbulenceAngle = Math::random(0.0f, Math::PI * 2.0f);
        float turbulenceAmount = Math::random(0.0f, m_lineTurbulence);
        
        Vec3 turbulenceVelocity = Vec3(
            Math::sin(turbulenceAngle) * turbulenceAmount,
            Math::cos(turbulenceAngle) * turbulenceAmount,
            Math::random(-0.1f, 0.1f)
        );
        
        velocity += turbulenceVelocity;
        
        // Add line variation to velocity
        velocity += variation * m_lineVariation * 0.5f;
        
        // Create particle
        auto particle = RF_NEW(BloodParticle);
        particle->initialize(particlePosition, velocity, m_particleSize, m_particleColor, m_particleLifetime, m_properties);
        
        m_particles.push_back(particle);
        m_particleCount++;
    }
    
    RF_LOG_INFO("Emitted %d line particles", particlesToEmit);
}

void LineEmitter::applyPhysicsToParticle(BloodParticle* particle, float deltaTime) {
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

void LineEmitter::setStartPosition(const Vec3& position) {
    m_startPosition = position;
    RF_LOG_INFO("Line emitter start position set to (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
}

void LineEmitter::setEndPosition(const Vec3& position) {
    m_endPosition = position;
    RF_LOG_INFO("Line emitter end position set to (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
}

void LineEmitter::setEmissionRate(float rate) {
    m_emissionRate = rate;
    RF_LOG_INFO("Line emitter emission rate set to %.2f", rate);
}

void LineEmitter::setEmissionSpread(float spread) {
    m_emissionSpread = spread;
    RF_LOG_INFO("Line emitter emission spread set to %.2f", spread);
}

void LineEmitter::setParticleLifetime(float lifetime) {
    m_particleLifetime = lifetime;
    RF_LOG_INFO("Line emitter particle lifetime set to %.2f", lifetime);
}

void LineEmitter::setParticleSize(float size) {
    m_particleSize = size;
    RF_LOG_INFO("Line emitter particle size set to %.2f", size);
}

void LineEmitter::setParticleColor(const Color& color) {
    m_particleColor = color;
    RF_LOG_INFO("Line emitter particle color set to (%.2f, %.2f, %.2f, %.2f)", color.r, color.g, color.b, color.a);
}

void LineEmitter::setParticleOpacity(float opacity) {
    m_particleOpacity = opacity;
    RF_LOG_INFO("Line emitter particle opacity set to %.2f", opacity);
}

void LineEmitter::setParticleVelocity(const Vec3& velocity) {
    m_particleVelocity = velocity;
    RF_LOG_INFO("Line emitter particle velocity set to (%.2f, %.2f, %.2f)", velocity.x, velocity.y, velocity.z);
}

void LineEmitter::setParticleAcceleration(const Vec3& acceleration) {
    m_particleAcceleration = acceleration;
    RF_LOG_INFO("Line emitter particle acceleration set to (%.2f, %.2f, %.2f)", acceleration.x, acceleration.y, acceleration.z);
}

void LineEmitter::setEnabled(bool enabled) {
    m_isEnabled = enabled;
    RF_LOG_INFO("Line emitter %s", enabled ? "enabled" : "disabled");
}

void LineEmitter::setLooping(bool looping) {
    m_isLooping = looping;
    RF_LOG_INFO("Line emitter looping set to %s", looping ? "true" : "false");
}

void LineEmitter::setMaxParticles(int maxParticles) {
    m_maxParticles = maxParticles;
    m_particles.reserve(maxParticles);
    RF_LOG_INFO("Line emitter max particles set to %d", maxParticles);
}

void LineEmitter::setEmissionInterval(float interval) {
    m_emissionInterval = interval;
    RF_LOG_INFO("Line emitter emission interval set to %.2f", interval);
}

void LineEmitter::setLineLength(float length) {
    m_lineLength = length;
    RF_LOG_INFO("Line emitter line length set to %.2f", length);
}

void LineEmitter::setLineWidth(float width) {
    m_lineWidth = width;
    RF_LOG_INFO("Line emitter line width set to %.2f", width);
}

void LineEmitter::setLineVariation(float variation) {
    m_lineVariation = variation;
    RF_LOG_INFO("Line emitter line variation set to %.2f", variation);
}

void LineEmitter::setLineTurbulence(float turbulence) {
    m_lineTurbulence = turbulence;
    RF_LOG_INFO("Line emitter line turbulence set to %.2f", turbulence);
}

const Vec3& LineEmitter::getStartPosition() const {
    return m_startPosition;
}

const Vec3& LineEmitter::getEndPosition() const {
    return m_endPosition;
}

float LineEmitter::getEmissionRate() const {
    return m_emissionRate;
}

float LineEmitter::getEmissionSpread() const {
    return m_emissionSpread;
}

float LineEmitter::getParticleLifetime() const {
    return m_particleLifetime;
}

float LineEmitter::getParticleSize() const {
    return m_particleSize;
}

const Color& LineEmitter::getParticleColor() const {
    return m_particleColor;
}

float LineEmitter::getParticleOpacity() const {
    return m_particleOpacity;
}

const Vec3& LineEmitter::getParticleVelocity() const {
    return m_particleVelocity;
}

const Vec3& LineEmitter::getParticleAcceleration() const {
    return m_particleAcceleration;
}

bool LineEmitter::isEnabled() const {
    return m_isEnabled;
}

bool LineEmitter::isLooping() const {
    return m_isLooping;
}

int LineEmitter::getMaxParticles() const {
    return m_maxParticles;
}

float LineEmitter::getEmissionInterval() const {
    return m_emissionInterval;
}

float LineEmitter::getLineLength() const {
    return m_lineLength;
}

float LineEmitter::getLineWidth() const {
    return m_lineWidth;
}

float LineEmitter::getLineVariation() const {
    return m_lineVariation;
}

float LineEmitter::getLineTurbulence() const {
    return m_lineTurbulence;
}

size_t LineEmitter::getParticleCount() const {
    return m_particleCount;
}

const std::vector<BloodParticle*>& LineEmitter::getParticles() const {
    return m_particles;
}

void LineEmitter::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& LineEmitter::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles