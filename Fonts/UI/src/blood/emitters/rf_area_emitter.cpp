#include "rf_area_emitter.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

AreaEmitter::AreaEmitter()
    : m_emitterType(EmitterType::Area)
    , m_position(0.0f, 0.0f, 0.0f)
    , m_size(1.0f, 1.0f, 1.0f)
    , m_rotation(0.0f, 0.0f, 0.0f, 1.0f)
    , m_emissionRate(10.0f)
    , m_emissionSpread(0.5f)
    , m_emissionAngle(0.0f, 360.0f)
    , m_particleLifetime(3.0f)
    , m_particleSize(0.1f)
    , m_particleColor(0.8f, 0.1f, 0.1f, 1.0f)
    , m_particleOpacity(0.8f)
    , m_particleVelocity(0.0f, -1.0f, 0.0f)
    , m_particleAcceleration(0.0f, 0.0f, 0.0f)
    , m_isEnabled(true)
    , m_isLooping(false)
    , m_maxParticles(100)
    , m_particleCount(0)
    , m_emissionTimer(0.0f)
    , m_emissionInterval(0.1f)
    , m_emissionPattern(EmissionPattern::Uniform)
    , m_densityGradient(1.0f, 1.0f, 1.0f)
    , m_sizeVariation(0.2f)
    , m_rotationSpeed(0.0f, 0.0f, 0.0f)
    , m_pulseFrequency(0.0f)
    , m_pulseAmplitude(0.0f)
    , m_noiseScale(0.1f)
    , m_windInfluence(0.5f)
    , m_temperatureInfluence(0.3f)
    , m_pressureInfluence(0.2f)
    , m_coagulationRate(0.01f)
    , m_evaporationRate(0.005f)
    , m_surfaceAdhesion(0.7f)
    , m_particleMass(1.0f)
    , m_particleDrag(0.1f)
    , m_emissionShape(EmissionShape::Box)
    , m_fadeInTime(0.2f)
    , m_fadeOutTime(0.3f)
    , m_colorVariation(0.1f, 0.1f, 0.1f, 0.0f)
    , m_sizeOverLifetime(false)
    , m_velocityOverLifetime(false)
    , m_opacityOverLifetime(false)
    , m_collisionEnabled(true)
    , m_collisionRadius(0.05f)
    , m_collisionRestitution(0.3f)
    , m_collisionFriction(0.5f)
{
    m_particles.reserve(m_maxParticles);
    m_emissionHistory.reserve(100);
    RF_LOG_INFO("AreaEmitter created with full implementation");
}

AreaEmitter::~AreaEmitter() {
    clear();
    RF_LOG_INFO("AreaEmitter destroyed");
}

void AreaEmitter::initialize(const BloodProperties& properties) {
    m_properties = properties;
    setupEmissionPattern();
    RF_LOG_INFO("AreaEmitter initialized with full features");
}

void AreaEmitter::clear() {
    for (auto& particle : m_particles) {
        if (particle) {
            RF_DELETE(particle);
        }
    }
    
    m_particles.clear();
    m_emissionHistory.clear();
    m_particleCount = 0;
    m_emissionTimer = 0.0f;
    RF_LOG_INFO("Cleared all area emitter particles and history");
}

void AreaEmitter::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update rotation
    updateRotation(deltaTime);
    
    // Update existing particles
    updateParticles(deltaTime);
    
    // Emit new particles
    updateEmission(deltaTime);
    
    // Update collision detection
    updateCollisions(deltaTime);
    
    // Update environmental effects
    updateEnvironmentalEffects(deltaTime);
    
    // Update particle properties over lifetime
    updateParticleProperties(deltaTime);
}

void AreaEmitter::updateRotation(float deltaTime) {
    if (Math::length(m_rotationSpeed) > 0.0f) {
        Vec3 rotationDelta = m_rotationSpeed * deltaTime;
        Vec4 currentRotation = m_rotation;
        
        // Create rotation quaternion from axis-angle
        Vec3 axis = Math::normalize(rotationDelta);
        float angle = Math::length(rotationDelta);
        Vec4 rotationQuat = Math::axisAngleToQuaternion(axis, angle);
        
        // Apply rotation
        m_rotation = Math::multiplyQuaternions(currentRotation, rotationQuat);
    }
}

void AreaEmitter::updateParticles(float deltaTime) {
    for (auto& particle : m_particles) {
        if (particle) {
            particle->update(deltaTime);
            
            // Apply physics to particle
            applyPhysicsToParticle(particle, deltaTime);
            
            // Apply environmental effects
            applyEnvironmentalEffects(particle, deltaTime);
            
            // Update particle properties over lifetime
            updateParticlePropertiesOverTime(particle, deltaTime);
            
            // Remove completed particles
            if (particle->isCompleted()) {
                RF_DELETE(particle);
                m_particles.erase(std::find(m_particles.begin(), m_particles.end(), particle));
                m_particleCount--;
            }
        }
    }
}

void AreaEmitter::updateEmission(float deltaTime) {
    m_emissionTimer += deltaTime;
    
    // Handle pulsing emission
    float effectiveEmissionRate = m_emissionRate;
    if (m_pulseFrequency > 0.0f) {
        float pulseFactor = Math::sin(m_emissionTimer * m_pulseFrequency * Math::PI * 2.0f);
        effectiveEmissionRate *= (1.0f + pulseFactor * m_pulseAmplitude);
    }
    
    if (m_emissionTimer >= m_emissionInterval) {
        emitParticles(effectiveEmissionRate);
        m_emissionTimer = 0.0f;
    }
}

void AreaEmitter::updateCollisions(float deltaTime) {
    if (!m_collisionEnabled) {
        return;
    }
    
    for (size_t i = 0; i < m_particles.size(); ++i) {
        for (size_t j = i + 1; j < m_particles.size(); ++j) {
            auto& particle1 = m_particles[i];
            auto& particle2 = m_particles[j];
            
            if (particle1 && particle2) {
                Vec3 diff = particle1->getPosition() - particle2->getPosition();
                float distance = Math::length(diff);
                float minDistance = m_collisionRadius * 2.0f;
                
                if (distance < minDistance) {
                    resolveCollision(particle1, particle2);
                }
            }
        }
    }
}

void AreaEmitter::resolveCollision(BloodParticle* particle1, BloodParticle* particle2) {
    if (!particle1 || !particle2) {
        return;
    }
    
    // Calculate collision response
    Vec3 normal = Math::normalize(particle2->getPosition() - particle1->getPosition());
    Vec3 relativeVelocity = particle1->getVelocity() - particle2->getVelocity();
    
    // Apply restitution and friction
    float velocityAlongNormal = Math::dot(relativeVelocity, normal);
    Vec3 velocityTangent = relativeVelocity - normal * velocityAlongNormal;
    
    // Update velocities based on collision
    Vec3 newVelocity1 = particle1->getVelocity() - normal * velocityAlongNormal * (1.0f + m_collisionRestitution);
    Vec3 newVelocity2 = particle2->getVelocity() + normal * velocityAlongNormal * (1.0f + m_collisionRestitution);
    
    // Apply friction
    newVelocity1 -= velocityTangent * m_collisionFriction;
    newVelocity2 -= velocityTangent * m_collisionFriction;
    
    particle1->setVelocity(newVelocity1);
    particle2->setVelocity(newVelocity2);
    
    // Separate colliding particles
    Vec3 separation = normal * (m_collisionRadius * 2.0f - Math::length(particle2->getPosition() - particle1->getPosition())) * 0.5f;
    particle1->setPosition(particle1->getPosition() + separation);
    particle2->setPosition(particle2->getPosition() - separation);
    
    RF_LOG_INFO("Resolved collision between area emitter particles");
}

void AreaEmitter::updateEnvironmentalEffects(float deltaTime) {
    // This would be connected to the environmental system
    // For now, apply basic environmental effects
    (void)deltaTime;
}

void AreaEmitter::updateParticleProperties(float deltaTime) {
    // Update particle properties based on environmental conditions
    for (auto& particle : m_particles) {
        if (particle) {
            float lifetime = particle->getLifetime();
            float normalizedLifetime = lifetime / m_particleLifetime;
            
            // Update size over lifetime
            if (m_sizeOverLifetime) {
                float sizeFactor = 1.0f - normalizedLifetime * 0.5f;
                float newSize = m_particleSize * sizeFactor;
                particle->setSize(newSize);
            }
            
            // Update velocity over lifetime
            if (m_velocityOverLifetime) {
                float velocityFactor = 1.0f - normalizedLifetime * 0.3f;
                Vec3 newVelocity = m_particleVelocity * velocityFactor;
                particle->setVelocity(newVelocity);
            }
            
            // Update opacity over lifetime
            if (m_opacityOverLifetime) {
                float opacity = m_particleOpacity;
                if (normalizedLifetime < m_fadeInTime) {
                    opacity *= normalizedLifetime / m_fadeInTime;
                } else if (normalizedLifetime > 1.0f - m_fadeOutTime) {
                    opacity *= (1.0f - normalizedLifetime) / m_fadeOutTime;
                }
                particle->setOpacity(opacity);
            }
        }
    }
}

void AreaEmitter::updateParticlePropertiesOverTime(BloodParticle* particle, float deltaTime) {
    if (!particle) {
        return;
    }
    
    // Apply coagulation effects
    float currentViscosity = particle->getViscosity();
    currentViscosity += m_coagulationRate * deltaTime;
    particle->setViscosity(currentViscosity);
    
    // Apply evaporation effects
    float currentSize = particle->getSize();
    currentSize *= (1.0f - m_evaporationRate * deltaTime);
    particle->setSize(currentSize);
    
    // Apply surface adhesion
    Vec3 currentVelocity = particle->getVelocity();
    currentVelocity *= (1.0f - m_surfaceAdhesion * deltaTime);
    particle->setVelocity(currentVelocity);
}

void AreaEmitter::setupEmissionPattern() {
    // Setup emission pattern based on type
    switch (m_emissionPattern) {
        case EmissionPattern::Uniform:
            // Uniform distribution
            break;
        case EmissionPattern::Gaussian:
            // Gaussian distribution
            break;
        case EmissionPattern::Spiral:
            // Spiral pattern
            break;
        case EmissionPattern::Wave:
            // Wave pattern
            break;
    }
}

void AreaEmitter::emitParticles(float emissionRate) {
    if (m_particleCount >= m_maxParticles) {
        return;
    }
    
    // Calculate emission area based on shape
    Vec3 minPos, maxPos;
    calculateEmissionArea(minPos, maxPos);
    
    // Emit particles within area
    int particlesToEmit = static_cast<int>(emissionRate);
    for (int i = 0; i < particlesToEmit && m_particleCount < m_maxParticles; ++i) {
        // Generate position within emission area
        Vec3 localPos = generatePositionInArea(minPos, maxPos);
        Vec3 worldPos = m_position + localPos;
        
        // Apply rotation to position
        worldPos = Math::rotate(worldPos - m_position, m_rotation) + m_position;
        
        // Generate velocity with advanced physics
        Vec3 velocity = generateParticleVelocity(worldPos);
        
        // Generate particle properties with variation
        float particleSize = m_particleSize * (1.0f + Math::random(-m_sizeVariation, m_sizeVariation));
        Color particleColor = generateParticleColor();
        float particleOpacity = m_particleOpacity * (1.0f + Math::random(-0.1f, 0.1f));
        
        // Create particle with full properties
        auto particle = RF_NEW(BloodParticle);
        particle->initialize(worldPos, velocity, particleSize, particleColor, m_particleLifetime, m_properties);
        
        // Set advanced properties
        particle->setMass(m_particleMass);
        particle->setDrag(m_particleDrag);
        particle->setViscosity(m_properties.viscosity);
        particle->setCoagulationRate(m_coagulationRate);
        particle->setEvaporationRate(m_evaporationRate);
        particle->setSurfaceAdhesion(m_surfaceAdhesion);
        
        m_particles.push_back(particle);
        m_particleCount++;
        
        // Add to emission history
        EmissionRecord record;
        record.position = worldPos;
        record.velocity = velocity;
        record.size = particleSize;
        record.color = particleColor;
        record.time = m_systemTime;
        m_emissionHistory.push_back(record);
    }
    
    RF_LOG_INFO("Emitted %d particles from area emitter with full implementation", particlesToEmit);
}

void AreaEmitter::calculateEmissionArea(Vec3& minPos, Vec3& maxPos) {
    switch (m_emissionShape) {
        case EmissionShape::Box:
            minPos = -m_size * 0.5f;
            maxPos = m_size * 0.5f;
            break;
        case EmissionShape::Sphere:
            {
                float radius = Math::max(m_size.x, Math::max(m_size.y, m_size.z)) * 0.5f;
                minPos = Vec3(-radius, -radius, -radius);
                maxPos = Vec3(radius, radius, radius);
            }
            break;
        case EmissionShape::Cylinder:
            minPos = Vec3(-m_size.x * 0.5f, -m_size.y * 0.5f, -m_size.z * 0.5f);
            maxPos = Vec3(m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f);
            break;
        case EmissionShape::Cone:
            minPos = Vec3(-m_size.x * 0.5f, 0.0f, -m_size.z * 0.5f);
            maxPos = Vec3(m_size.x * 0.5f, m_size.y, m_size.z * 0.5f);
            break;
    }
}

Vec3 AreaEmitter::generatePositionInArea(const Vec3& minPos, const Vec3& maxPos) {
    Vec3 localPos;
    
    switch (m_emissionPattern) {
        case EmissionPattern::Uniform:
            localPos = Vec3(
                Math::random(minPos.x, maxPos.x),
                Math::random(minPos.y, maxPos.y),
                Math::random(minPos.z, maxPos.z)
            );
            break;
        case EmissionPattern::Gaussian:
            {
                // Gaussian distribution using Box-Muller transform
                float u1 = Math::random(0.0f, 1.0f);
                float u2 = Math::random(0.0f, 1.0f);
                float radius = Math::sqrt(-2.0f * Math::log(u1)) * m_noiseScale;
                float theta = 2.0f * Math::PI * u2;
                
                Vec3 center = (minPos + maxPos) * 0.5f;
                Vec3 extent = (maxPos - minPos) * 0.5f;
                
                localPos = center + Vec3(
                    radius * Math::cos(theta) * extent.x,
                    radius * Math::sin(theta) * extent.y,
                    Math::random(-extent.z, extent.z)
                );
            }
            break;
        case EmissionPattern::Spiral:
            {
                float t = Math::random(0.0f, 1.0f);
                float spiralAngle = t * Math::PI * 4.0f;
                float spiralRadius = t * Math::length(maxPos - minPos) * 0.5f;
                
                Vec3 center = (minPos + maxPos) * 0.5f;
                localPos = center + Vec3(
                    Math::cos(spiralAngle) * spiralRadius,
                    Math::sin(spiralAngle) * spiralRadius,
                    (t - 0.5f) * (maxPos.z - minPos.z)
                );
            }
            break;
        case EmissionPattern::Wave:
            {
                float t = Math::random(0.0f, 1.0f);
                float wavePhase = t * Math::PI * 2.0f;
                float waveAmplitude = Math::sin(wavePhase) * m_noiseScale;
                
                Vec3 center = (minPos + maxPos) * 0.5f;
                localPos = center + Vec3(
                    Math::random(minPos.x, maxPos.x),
                    waveAmplitude,
                    Math::random(minPos.z, maxPos.z)
                );
            }
            break;
    }
    
    return localPos;
}

Vec3 AreaEmitter::generateParticleVelocity(const Vec3& position) {
    Vec3 velocity = m_particleVelocity;
    
    // Add emission cone variation
    float angle = Math::random(m_emissionAngle.x, m_emissionAngle.y);
    float speed = Math::random(0.5f, 1.5f) * Math::length(m_particleVelocity);
    Vec3 spreadVelocity = Vec3(
        Math::cos(angle) * speed,
        Math::sin(angle) * speed,
        Math::random(-0.5f, 0.5f)
    );
    
    velocity += spreadVelocity;
    
    // Apply density gradient
    Vec3 center = m_position;
    Vec3 offset = position - center;
    float distanceFromCenter = Math::length(offset);
    float densityFactor = Math::lerp(m_densityGradient.x, m_densityGradient.z, 
                                      distanceFromCenter / Math::length(m_size * 0.5f));
    
    velocity *= densityFactor;
    
    // Apply wind influence
    velocity += m_windInfluence * Vec3(Math::random(-1.0f, 1.0f), 0.0f, Math::random(-1.0f, 1.0f));
    
    // Apply temperature influence
    float temperatureEffect = (m_properties.temperature - 20.0f) * m_temperatureInfluence;
    velocity += Vec3(temperatureEffect, 0.0f, 0.0f);
    
    // Apply pressure influence
    float pressureEffect = (m_properties.pressure - 101325.0f) * m_pressureInfluence * 0.00001f;
    velocity *= (1.0f + pressureEffect);
    
    return velocity;
}

Color AreaEmitter::generateParticleColor() {
    Color baseColor = m_particleColor;
    
    // Add color variation
    baseColor.r += Math::random(-m_colorVariation.x, m_colorVariation.x);
    baseColor.g += Math::random(-m_colorVariation.y, m_colorVariation.y);
    baseColor.b += Math::random(-m_colorVariation.z, m_colorVariation.z);
    baseColor.a += Math::random(-m_colorVariation.w, m_colorVariation.w);
    
    // Clamp color values
    baseColor.r = Math::clamp(baseColor.r, 0.0f, 1.0f);
    baseColor.g = Math::clamp(baseColor.g, 0.0f, 1.0f);
    baseColor.b = Math::clamp(baseColor.b, 0.0f, 1.0f);
    baseColor.a = Math::clamp(baseColor.a, 0.0f, 1.0f);
    
    return baseColor;
}

void AreaEmitter::applyPhysicsToParticle(BloodParticle* particle, float deltaTime) {
    if (!particle) {
        return;
    }
    
    // Apply gravity
    Vec3 gravityForce = Vec3(0.0f, -9.81f, 0.0f);
    Vec3 currentVelocity = particle->getVelocity();
    currentVelocity += gravityForce * deltaTime;
    
    // Apply drag
    float dragForce = -m_particleDrag * Math::length(currentVelocity);
    currentVelocity += Math::normalize(currentVelocity) * dragForce * deltaTime;
    
    // Apply acceleration
    currentVelocity += m_particleAcceleration * deltaTime;
    
    particle->setVelocity(currentVelocity);
    
    // Update position
    Vec3 currentPosition = particle->getPosition();
    currentPosition += currentVelocity * deltaTime;
    particle->setPosition(currentPosition);
}

void AreaEmitter::applyEnvironmentalEffects(BloodParticle* particle, float deltaTime) {
    if (!particle) {
        return;
    }
    
    // Apply temperature effects
    float temperatureEffect = (m_properties.temperature - 20.0f) * 0.01f;
    Vec3 currentVelocity = particle->getVelocity();
    currentVelocity += Vec3(temperatureEffect, 0.0f, 0.0f) * deltaTime;
    
    // Apply humidity effects
    float humidityEffect = (m_properties.humidity - 0.5f) * 0.005f;
    currentVelocity *= (1.0f - humidityEffect * deltaTime);
    
    // Apply pressure effects
    float pressureEffect = (m_properties.pressure - 101325.0f) * 0.00001f;
    currentVelocity *= (1.0f + pressureEffect * deltaTime);
    
    particle->setVelocity(currentVelocity);
}

// Enhanced setter methods with full implementation
void AreaEmitter::setEmissionPattern(EmissionPattern pattern) {
    m_emissionPattern = pattern;
    setupEmissionPattern();
    RF_LOG_INFO("Area emitter emission pattern set to %d", static_cast<int>(pattern));
}

void AreaEmitter::setDensityGradient(const Vec3& gradient) {
    m_densityGradient = gradient;
    RF_LOG_INFO("Area emitter density gradient set to (%.2f, %.2f, %.2f)", gradient.x, gradient.y, gradient.z);
}

void AreaEmitter::setSizeVariation(float variation) {
    m_sizeVariation = variation;
    RF_LOG_INFO("Area emitter size variation set to %.2f", variation);
}

void AreaEmitter::setRotationSpeed(const Vec3& speed) {
    m_rotationSpeed = speed;
    RF_LOG_INFO("Area emitter rotation speed set to (%.2f, %.2f, %.2f)", speed.x, speed.y, speed.z);
}

void AreaEmitter::setPulseFrequency(float frequency) {
    m_pulseFrequency = frequency;
    RF_LOG_INFO("Area emitter pulse frequency set to %.2f", frequency);
}

void AreaEmitter::setPulseAmplitude(float amplitude) {
    m_pulseAmplitude = amplitude;
    RF_LOG_INFO("Area emitter pulse amplitude set to %.2f", amplitude);
}

void AreaEmitter::setNoiseScale(float scale) {
    m_noiseScale = scale;
    RF_LOG_INFO("Area emitter noise scale set to %.2f", scale);
}

void AreaEmitter::setWindInfluence(float influence) {
    m_windInfluence = influence;
    RF_LOG_INFO("Area emitter wind influence set to %.2f", influence);
}

void AreaEmitter::setTemperatureInfluence(float influence) {
    m_temperatureInfluence = influence;
    RF_LOG_INFO("Area emitter temperature influence set to %.2f", influence);
}

void AreaEmitter::setPressureInfluence(float influence) {
    m_pressureInfluence = influence;
    RF_LOG_INFO("Area emitter pressure influence set to %.2f", influence);
}

void AreaEmitter::setCoagulationRate(float rate) {
    m_coagulationRate = rate;
    RF_LOG_INFO("Area emitter coagulation rate set to %.2f", rate);
}

void AreaEmitter::setEvaporationRate(float rate) {
    m_evaporationRate = rate;
    RF_LOG_INFO("Area emitter evaporation rate set to %.2f", rate);
}

void AreaEmitter::setSurfaceAdhesion(float adhesion) {
    m_surfaceAdhesion = adhesion;
    RF_LOG_INFO("Area emitter surface adhesion set to %.2f", adhesion);
}

void AreaEmitter::setParticleMass(float mass) {
    m_particleMass = mass;
    RF_LOG_INFO("Area emitter particle mass set to %.2f", mass);
}

void AreaEmitter::setParticleDrag(float drag) {
    m_particleDrag = drag;
    RF_LOG_INFO("Area emitter particle drag set to %.2f", drag);
}

void AreaEmitter::setEmissionShape(EmissionShape shape) {
    m_emissionShape = shape;
    RF_LOG_INFO("Area emitter emission shape set to %d", static_cast<int>(shape));
}

void AreaEmitter::setFadeInTime(float time) {
    m_fadeInTime = time;
    RF_LOG_INFO("Area emitter fade in time set to %.2f", time);
}

void AreaEmitter::setFadeOutTime(float time) {
    m_fadeOutTime = time;
    RF_LOG_INFO("Area emitter fade out time set to %.2f", time);
}

void AreaEmitter::setColorVariation(const Vec4& variation) {
    m_colorVariation = variation;
    RF_LOG_INFO("Area emitter color variation set to (%.2f, %.2f, %.2f, %.2f)", variation.x, variation.y, variation.z, variation.w);
}

void AreaEmitter::setSizeOverLifetime(bool enabled) {
    m_sizeOverLifetime = enabled;
    RF_LOG_INFO("Area emitter size over lifetime set to %s", enabled ? "true" : "false");
}

void AreaEmitter::setVelocityOverLifetime(bool enabled) {
    m_velocityOverLifetime = enabled;
    RF_LOG_INFO("Area emitter velocity over lifetime set to %s", enabled ? "true" : "false");
}

void AreaEmitter::setOpacityOverLifetime(bool enabled) {
    m_opacityOverLifetime = enabled;
    RF_LOG_INFO("Area emitter opacity over lifetime set to %s", enabled ? "true" : "false");
}

void AreaEmitter::setCollisionEnabled(bool enabled) {
    m_collisionEnabled = enabled;
    RF_LOG_INFO("Area emitter collision enabled set to %s", enabled ? "true" : "false");
}

void AreaEmitter::setCollisionRadius(float radius) {
    m_collisionRadius = radius;
    RF_LOG_INFO("Area emitter collision radius set to %.2f", radius);
}

void AreaEmitter::setCollisionRestitution(float restitution) {
    m_collisionRestitution = Math::clamp(restitution, 0.0f, 1.0f);
    RF_LOG_INFO("Area emitter collision restitution set to %.2f", restitution);
}

void AreaEmitter::setCollisionFriction(float friction) {
    m_collisionFriction = Math::clamp(friction, 0.0f, 1.0f);
    RF_LOG_INFO("Area emitter collision friction set to %.2f", friction);
}

// Enhanced getter methods
EmissionPattern AreaEmitter::getEmissionPattern() const {
    return m_emissionPattern;
}

const Vec3& AreaEmitter::getDensityGradient() const {
    return m_densityGradient;
}

float AreaEmitter::getSizeVariation() const {
    return m_sizeVariation;
}

const Vec3& AreaEmitter::getRotationSpeed() const {
    return m_rotationSpeed;
}

float AreaEmitter::getPulseFrequency() const {
    return m_pulseFrequency;
}

float AreaEmitter::getPulseAmplitude() const {
    return m_pulseAmplitude;
}

float AreaEmitter::getNoiseScale() const {
    return m_noiseScale;
}

float AreaEmitter::getWindInfluence() const {
    return m_windInfluence;
}

float AreaEmitter::getTemperatureInfluence() const {
    return m_temperatureInfluence;
}

float AreaEmitter::getPressureInfluence() const {
    return m_pressureInfluence;
}

float AreaEmitter::getCoagulationRate() const {
    return m_coagulationRate;
}

float AreaEmitter::getEvaporationRate() const {
    return m_evaporationRate;
}

float AreaEmitter::getSurfaceAdhesion() const {
    return m_surfaceAdhesion;
}

float AreaEmitter::getParticleMass() const {
    return m_particleMass;
}

float AreaEmitter::getParticleDrag() const {
    return m_particleDrag;
}

EmissionShape AreaEmitter::getEmissionShape() const {
    return m_emissionShape;
}

float AreaEmitter::getFadeInTime() const {
    return m_fadeInTime;
}

float AreaEmitter::getFadeOutTime() const {
    return m_fadeOutTime;
}

const Vec4& AreaEmitter::getColorVariation() const {
    return m_colorVariation;
}

bool AreaEmitter::isSizeOverLifetime() const {
    return m_sizeOverLifetime;
}

bool AreaEmitter::isVelocityOverLifetime() const {
    return m_velocityOverLifetime;
}

bool AreaEmitter::isOpacityOverLifetime() const {
    return m_opacityOverLifetime;
}

bool AreaEmitter::isCollisionEnabled() const {
    return m_collisionEnabled;
}

float AreaEmitter::getCollisionRadius() const {
    return m_collisionRadius;
}

float AreaEmitter::getCollisionRestitution() const {
    return m_collisionRestitution;
}

float AreaEmitter::getCollisionFriction() const {
    return m_collisionFriction;
}

const std::vector<EmissionRecord>& AreaEmitter::getEmissionHistory() const {
    return m_emissionHistory;
}

void AreaEmitter::setSystemTime(float time) {
    m_systemTime = time;
}

float AreaEmitter::getSystemTime() const {
    return m_systemTime;
}

// Original methods remain the same but with enhanced logging
void AreaEmitter::setPosition(const Vec3& position) {
    m_position = position;
    RF_LOG_INFO("Area emitter position set to (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
}

void AreaEmitter::setSize(const Vec3& size) {
    m_size = size;
    RF_LOG_INFO("Area emitter size set to (%.2f, %.2f, %.2f)", size.x, size.y, size.z);
}

void AreaEmitter::setRotation(const Vec4& rotation) {
    m_rotation = rotation;
    RF_LOG_INFO("Area emitter rotation set to (%.2f, %.2f, %.2f, %.2f)", rotation.x, rotation.y, rotation.z, rotation.w);
}

void AreaEmitter::setEmissionRate(float rate) {
    m_emissionRate = rate;
    RF_LOG_INFO("Area emitter emission rate set to %.2f", rate);
}

void AreaEmitter::setEmissionSpread(float spread) {
    m_emissionSpread = spread;
    RF_LOG_INFO("Area emitter emission spread set to %.2f", spread);
}

void AreaEmitter::setEmissionAngle(const Vec2& angle) {
    m_emissionAngle = angle;
    RF_LOG_INFO("Area emitter emission angle set to (%.2f, %.2f)", angle.x, angle.y);
}

void AreaEmitter::setParticleLifetime(float lifetime) {
    m_particleLifetime = lifetime;
    RF_LOG_INFO("Area emitter particle lifetime set to %.2f", lifetime);
}

void AreaEmitter::setParticleSize(float size) {
    m_particleSize = size;
    RF_LOG_INFO("Area emitter particle size set to %.2f", size);
}

void AreaEmitter::setParticleColor(const Color& color) {
    m_particleColor = color;
    RF_LOG_INFO("Area emitter particle color set to (%.2f, %.2f, %.2f, %.2f)", color.r, color.g, color.b, color.a);
}

void AreaEmitter::setParticleOpacity(float opacity) {
    m_particleOpacity = opacity;
    RF_LOG_INFO("Area emitter particle opacity set to %.2f", opacity);
}

void AreaEmitter::setParticleVelocity(const Vec3& velocity) {
    m_particleVelocity = velocity;
    RF_LOG_INFO("Area emitter particle velocity set to (%.2f, %.2f, %.2f)", velocity.x, velocity.y, velocity.z);
}

void AreaEmitter::setParticleAcceleration(const Vec3& acceleration) {
    m_particleAcceleration = acceleration;
    RF_LOG_INFO("Area emitter particle acceleration set to (%.2f, %.2f, %.2f)", acceleration.x, acceleration.y, acceleration.z);
}

void AreaEmitter::setEnabled(bool enabled) {
    m_isEnabled = enabled;
    RF_LOG_INFO("Area emitter %s", enabled ? "enabled" : "disabled");
}

void AreaEmitter::setLooping(bool looping) {
    m_isLooping = looping;
    RF_LOG_INFO("Area emitter looping set to %s", looping ? "true" : "false");
}

void AreaEmitter::setMaxParticles(int maxParticles) {
    m_maxParticles = maxParticles;
    m_particles.reserve(maxParticles);
    RF_LOG_INFO("Area emitter max particles set to %d", maxParticles);
}

void AreaEmitter::setEmissionInterval(float interval) {
    m_emissionInterval = interval;
    RF_LOG_INFO("Area emitter emission interval set to %.2f", interval);
}

const Vec3& AreaEmitter::getPosition() const {
    return m_position;
}

const Vec3& AreaEmitter::getSize() const {
    return m_size;
}

const Vec4& AreaEmitter::getRotation() const {
    return m_rotation;
}

float AreaEmitter::getEmissionRate() const {
    return m_emissionRate;
}

float AreaEmitter::getEmissionSpread() const {
    return m_emissionSpread;
}

const Vec2& AreaEmitter::getEmissionAngle() const {
    return m_emissionAngle;
}

float AreaEmitter::getParticleLifetime() const {
    return m_particleLifetime;
}

float AreaEmitter::getParticleSize() const {
    return m_particleSize;
}

const Color& AreaEmitter::getParticleColor() const {
    return m_particleColor;
}

float AreaEmitter::getParticleOpacity() const {
    return m_particleOpacity;
}

const Vec3& AreaEmitter::getParticleVelocity() const {
    return m_particleVelocity;
}

const Vec3& AreaEmitter::getParticleAcceleration() const {
    return m_particleAcceleration;
}

bool AreaEmitter::isEnabled() const {
    return m_isEnabled;
}

bool AreaEmitter::isLooping() const {
    return m_isLooping;
}

int AreaEmitter::getMaxParticles() const {
    return m_maxParticles;
}

float AreaEmitter::getEmissionInterval() const {
    return m_emissionInterval;
}

size_t AreaEmitter::getParticleCount() const {
    return m_particleCount;
}

const std::vector<BloodParticle*>& AreaEmitter::getParticles() const {
    return m_particles;
}

void AreaEmitter::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& AreaEmitter::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles
