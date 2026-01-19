#include "rf_blood_system.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

BloodSystem::BloodSystem()
    : m_systemTime(0.0f)
    , m_timeScale(1.0f)
    , m_isActive(false)
    , m_isPaused(false)
    , m_maxParticles(2000)
    , m_maxEffects(1000)
    , m_maxCollisions(500)
    , m_particleCount(0)
    , m_effectCount(0)
    , m_collisionCount(0)
    , m_dripCount(0)
    , m_poolCount(0)
    , m_spatterCount(0)
    , m_splashCount(0)
    , m_streamCount(0)
    , m_trailCount(0)
    , m_flowCount(0)
    , m_spreadCount(0)
    , m_gravity(9.81f)
    , m_windForce(0.0f, 0.0f, 0.0f)
    , m_temperature(20.0f)
    , m_humidity(0.5f)
    , m_pressure(101325.0f)
{
    m_particles.reserve(m_maxParticles);
    m_effects.reserve(m_maxEffects);
    m_collisions.reserve(m_maxCollisions);
    m_drips.reserve(m_maxDrips);
    m_pools.reserve(m_maxPools);
    m_splatters.reserve(m_maxSplatters);
    m_splashes.reserve(m_maxSplashes);
    m_streams.reserve(m_maxStreams);
    m_trails.reserve(m_maxTrails);
    m_flows.reserve(m_maxFlows);
    m_spreads.reserve(m_maxSpreads);
    
    RF_LOG_INFO("BloodSystem created");
}

BloodSystem::~BloodSystem() {
    clear();
    RF_LOG_INFO("BloodSystem destroyed");
}

void BloodSystem::initialize() {
    RF_LOG_INFO("BloodSystem initialized");
}

void BloodSystem::clear() {
    // Clear all blood system components
    for (auto& particle : m_particles) {
        if (particle) {
            RF_DELETE(particle);
        }
    }
    
    for (auto& effect : m_effects) {
        if (effect) {
            RF_DELETE(effect);
        }
    }
    
    for (auto& collision : m_collisions) {
        if (collision) {
            RF_DELETE(collision);
        }
    }
    
    for (auto& drip : m_drips) {
        if (drip) {
            RF_DELETE(drip);
        }
    }
    
    for (auto& pool : m_pools) {
        if (pool) {
            RF_DELETE(pool);
        }
    }
    
    for (auto& spatter : m_splatters) {
        if (spatter) {
            RF_DELETE(spatter);
        }
    }
    
    for (auto& splash : m_splashes) {
        if (splash) {
            RF_DELETE(splash);
        }
    }
    
    for (auto& stream : m_streams) {
        if (stream) {
            RF_DELETE(stream);
        }
    }
    
    for (auto& trail : m_trails) {
        if (trail) {
            RF_DELETE(trail);
        }
    }
    
    for (auto& flow : m_flows) {
        if (flow) {
            RF_DELETE(flow);
        }
    }
    
    for (auto& spread : m_spreads) {
        if (spread) {
            RF_DELETE(spread);
        }
    }
    
    m_particles.clear();
    m_effects.clear();
    m_collisions.clear();
    m_drips.clear();
    m_pools.clear();
    m_splatters.clear();
    m_splashes.clear();
    m_streams.clear();
    m_trails.clear();
    m_flows.clear();
    m_spreads.clear();
    
    m_particleCount = 0;
    m_effectCount = 0;
    m_collisionCount = 0;
    m_dripCount = 0;
    m_poolCount = 0;
    m_spatterCount = 0;
    m_splashCount = 0;
    m_streamCount = 0;
    m_trailCount = 0;
    m_flowCount = 0;
    m_spreadCount = 0;
    
    RF_LOG_INFO("Cleared all blood system components");
}

void BloodSystem::update(float deltaTime) {
    if (!m_isActive || m_isPaused) {
        return;
    }
    
    // Update system time
    m_systemTime += deltaTime * m_timeScale;
    
    // Update all blood system components
    updateParticles(deltaTime);
    updateEffects(deltaTime);
    updateCollisions(deltaTime);
    updateDrips(deltaTime);
    updatePools(deltaTime);
    updateSplatters(deltaTime);
    updateSplashes(deltaTime);
    updateStreams(deltaTime);
    updateTrails(deltaTime);
    updateFlows(deltaTime);
    updateSpreads(deltaTime);
    
    // Update physics
    updatePhysics(deltaTime);
    
    // Generate new components based on conditions
    updateComponentGeneration(deltaTime);
}

void BloodSystem::updateParticles(float deltaTime) {
    for (auto& particle : m_particles) {
        if (particle) {
            particle->update(deltaTime);
            
            // Apply physics
            applyPhysicsToParticle(particle, deltaTime);
            
            // Remove completed particles
            if (particle->isCompleted()) {
                RF_DELETE(particle);
                m_particles.erase(std::find(m_particles.begin(), m_particles.end(), particle));
                m_particleCount--;
            }
        }
    }
    
    // Generate new particles based on conditions
    updateParticleGeneration(deltaTime);
}

void BloodSystem::updateParticleGeneration(float deltaTime) {
    // Generate new particles based on conditions
    // This would be connected to the particle generation system
    // For now, just update existing particles
    (void)deltaTime;
}

void BloodSystem::applyPhysicsToParticle(BloodParticle* particle, float deltaTime) {
    if (!particle) {
        return;
    }
    
    // Apply gravity
    Vec3 gravityForce = Vec3(0.0f, -m_gravity, 0.0f);
    Vec3 currentVelocity = particle->getVelocity();
    currentVelocity += gravityForce * deltaTime;
    
    // Apply wind
    currentVelocity += m_windForce * deltaTime;
    
    // Apply temperature effects
    float temperatureEffect = (m_temperature - 20.0f) * 0.01f;
    currentVelocity += Vec3(temperatureEffect, 0.0f, 0.0f) * deltaTime;
    
    // Apply humidity effects
    float humidityEffect = (m_humidity - 0.5f) * 0.005f;
    currentVelocity *= (1.0f - humidityEffect * deltaTime);
    
    // Apply pressure effects
    float pressureEffect = (m_pressure - 101325.0f) * 0.00001f;
    currentVelocity *= (1.0f + pressureEffect * deltaTime);
    
    particle->setVelocity(currentVelocity);
    
    // Update position
    Vec3 currentPosition = particle->getPosition();
    currentPosition += currentVelocity * deltaTime;
    particle->setPosition(currentPosition);
}

void BloodSystem::updateEffects(float deltaTime) {
    for (auto& effect : m_effects) {
        if (effect) {
            effect->update(deltaTime);
            
            // Apply physics to effect
            applyPhysicsToEffect(effect, deltaTime);
            
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

void BloodSystem::updateEffectGeneration(float deltaTime) {
    // Generate new effects based on conditions
    // This would be connected to the effect generation system
    // For now, just update existing effects
    (void)deltaTime;
}

void BloodSystem::applyPhysicsToEffect(BloodEffect* effect, float deltaTime) {
    if (!effect) {
        return;
    }
    
    // Apply gravity to effect
    Vec3 gravityForce = Vec3(0.0f, -m_gravity, 0.0f);
    Vec3 currentVelocity = effect->getVelocity();
    currentVelocity += gravityForce * deltaTime;
    
    // Apply wind to effect
    currentVelocity += m_windForce * deltaTime;
    
    // Apply temperature effects
    float temperatureEffect = (m_temperature - 20.0f) * 0.01f;
    currentVelocity += Vec3(temperatureEffect, 0.0f, 0.0f) * deltaTime;
    
    // Apply humidity effects
    float humidityEffect = (m_humidity - 0.5f) * 0.005f;
    currentVelocity *= (1.0f - humidityEffect * deltaTime);
    
    // Apply pressure effects
    float pressureEffect = (m_pressure - 101325.0f) * 0.00001f;
    currentVelocity *= (1.0f + pressureEffect * deltaTime);
    
    effect->setVelocity(currentVelocity);
    
    // Update position
    Vec3 currentPosition = effect->getPosition();
    currentPosition += currentVelocity * deltaTime;
    effect->setPosition(currentPosition);
}

void BloodSystem::updateCollisions(float deltaTime) {
    for (auto& collision : m_collisions) {
        if (collision) {
            collision->update(deltaTime);
            
            // Remove completed collisions
            if (collision->isCompleted()) {
                RF_DELETE(collision);
                m_collisions.erase(std::find(m_collisions.begin(), m_collisions.end(), collision));
                m_collisionCount--;
            }
        }
    }
    
    // Check for new collisions
    updateCollisionDetection(deltaTime);
}

void BloodSystem::updateCollisionDetection(float deltaTime) {
    // Check for collisions between blood particles
    for (size_t i = 0; i < m_collisions.size(); ++i) {
        for (size_t j = i + 1; j < m_collisions.size(); ++j) {
            auto& collision1 = m_collisions[i];
            auto& collision2 = m_collisions[j];
            
            if (collision1 && collision2) {
                Vec3 diff = collision1->getPosition() - collision2->getPosition();
                float distance = Math::length(diff);
                float minDistance = collision1->getRadius() + collision2->getRadius();
                
                if (distance < minDistance) {
                    // Collision detected
                    resolveCollision(collision1, collision2);
                }
            }
        }
    }
    
    // For now, just update existing collisions
    (void)deltaTime;
}

void BloodSystem::resolveCollision(BloodCollision* collision1, BloodCollision* collision2) {
    if (!collision1 || !collision2) {
        return;
    }
    
    // Calculate collision response
    Vec3 normal = Math::normalize(collision2->getPosition() - collision1->getPosition());
    Vec3 relativeVelocity = collision1->getVelocity() - collision2->getVelocity();
    
    // Apply restitution and friction
    float velocityAlongNormal = Math::dot(relativeVelocity, normal);
    Vec3 velocityTangent = relativeVelocity - normal * velocityAlongNormal;
    
    // Update velocities based on collision
    Vec3 newVelocity1 = collision1->getVelocity() - normal * velocityAlongNormal * (1.0f + 0.3f);
    Vec3 newVelocity2 = collision2->getVelocity() + normal * velocityAlongNormal * (1.0f + 0.3f);
    
    // Apply friction
    newVelocity1 -= velocityTangent * 0.5f;
    newVelocity2 -= velocityTangent * 0.5f;
    
    collision1->setVelocity(newVelocity1);
    collision2->setVelocity(newVelocity2);
    
    // Separate colliding objects
    Vec3 separation = normal * (collision1->getRadius() + collision2->getRadius() - Math::length(collision2->getPosition() - collision1->getPosition())) * 0.5f;
    collision1->setPosition(collision1->getPosition() + separation);
    collision2->setPosition(collision2->getPosition() - separation);
    
    RF_LOG_INFO("Resolved collision between blood particles");
}

void BloodSystem::updateDrips(float deltaTime) {
    for (auto& drip : m_drips) {
        if (drip) {
            drip->update(deltaTime);
            
            // Apply physics to drip
            applyPhysicsToDrip(drip, deltaTime);
            
            // Remove completed drips
            if (drip->isCompleted()) {
                RF_DELETE(drip);
                m_drips.erase(std::find(m_drips.begin(), m_drips.end(), drip));
                m_dripCount--;
            }
        }
    }
    
    // Generate new drips based on conditions
    updateDripGeneration(deltaTime);
}

void BloodSystem::updateDripGeneration(float deltaTime) {
    // Generate new drips based on surface conditions
    // This would be connected to the surface system
    // For now, just update existing drips
    (void)deltaTime;
}

void BloodSystem::applyPhysicsToDrip(BloodDrip* drip, float deltaTime) {
    if (!drip) {
        return;
    }
    
    // Apply gravity to drip
    Vec3 gravityForce = Vec3(0.0f, -m_gravity, 0.0f);
    Vec3 currentVelocity = drip->getVelocity();
    currentVelocity += gravityForce * deltaTime;
    
    // Apply wind to drip
    currentVelocity += m_windForce * deltaTime;
    
    // Apply temperature effects
    float temperatureEffect = (m_temperature - 20.0f) * 0.01f;
    currentVelocity += Vec3(temperatureEffect, 0.0f, 0.0f) * deltaTime;
    
    // Apply humidity effects
    float humidityEffect = (m_humidity - 0.5f) * 0.005f;
    currentVelocity *= (1.0f - humidityEffect * deltaTime);
    
    // Apply pressure effects
    float pressureEffect = (m_pressure - 101325.0f) * 0.00001f;
    currentVelocity *= (1.0f + pressureEffect * deltaTime);
    
    drip->setVelocity(currentVelocity);
    
    // Update position
    Vec3 currentPosition = drip->getPosition();
    currentPosition += currentVelocity * deltaTime;
    drip->setPosition(currentPosition);
}

void BloodSystem::updatePools(float deltaTime) {
    for (auto& pool : m_pools) {
        if (pool) {
            pool->update(deltaTime);
            
            // Apply physics to pool
            applyPhysicsToPool(pool, deltaTime);
            
            // Remove completed pools
            if (pool->isCompleted()) {
                RF_DELETE(pool);
                m_pools.erase(std::find(m_pools.begin(), m_pools.end(), pool));
                m_poolCount--;
            }
        }
    }
    
    // Generate new pools based on drips
    updatePoolGeneration(deltaTime);
}

void BloodSystem::updatePoolGeneration(float deltaTime) {
    // Generate new pools based on drips
    // This would be connected to the drip system
    // For now, just update existing pools
    (void)deltaTime;
}

void BloodSystem::applyPhysicsToPool(BloodPool* pool, float deltaTime) {
    if (!pool) {
        return;
    }
    
    // Apply spreading to pool
    float spreadRate = pool->getSpreadRate();
    float currentSize = pool->getSize();
    currentSize += spreadRate * deltaTime;
    pool->setSize(currentSize);
    
    // Apply evaporation to pool
    float evaporationRate = pool->getEvaporationRate();
    currentSize *= (1.0f - evaporationRate * deltaTime);
    pool->setSize(currentSize);
    
    // Apply coagulation to pool
    float coagulationRate = pool->getCoagulationRate();
    float currentViscosity = pool->getViscosity();
    currentViscosity += coagulationRate * deltaTime;
    pool->setViscosity(currentViscosity);
}

void BloodSystem::updateSplatters(float deltaTime) {
    for (auto& spatter : m_splatters) {
        if (spatter) {
            spatter->update(deltaTime);
            
            // Apply physics to spatter
            applyPhysicsToSpatter(spatter, deltaTime);
            
            // Remove completed splatters
            if (spatter->isCompleted()) {
                RF_DELETE(spatter);
                m_splatters.erase(std::find(m_splatters.begin(), m_splatters.end(), spatter));
                m_spatterCount--;
            }
        }
    }
    
    // Generate new splatters based on impacts
    updateSpatterGeneration(deltaTime);
}

void BloodSystem::updateSpatterGeneration(float deltaTime) {
    // Generate new splatters based on impacts
    // This would be connected to the impact system
    // For now, just update existing splatters
    (void)deltaTime;
}

void BloodSystem::applyPhysicsToSpatter(BloodSpatter* spatter, float deltaTime) {
    if (!spatter) {
        return;
    }
    
    // Apply gravity to spatter
    Vec3 gravityForce = Vec3(0.0f, -m_gravity, 0.0f);
    Vec3 currentVelocity = spatter->getVelocity();
    currentVelocity += gravityForce * deltaTime;
    
    // Apply wind to spatter
    currentVelocity += m_windForce * deltaTime;
    
    // Apply temperature effects
    float temperatureEffect = (m_temperature - 20.0f) * 0.01f;
    currentVelocity += Vec3(temperatureEffect, 0.0f, 0.0f) * deltaTime;
    
    // Apply humidity effects
    float humidityEffect = (m_humidity - 0.5f) * 0.005f;
    currentVelocity *= (1.0f - humidityEffect * deltaTime);
    
    // Apply pressure effects
    float pressureEffect = (m_pressure - 101325.0f) * 0.00001f;
    currentVelocity *= (1.0f + pressureEffect * deltaTime);
    
    spatter->setVelocity(currentVelocity);
    
    // Update position
    Vec3 currentPosition = spatter->getPosition();
    currentPosition += currentVelocity * deltaTime;
    spatter->setPosition(currentPosition);
}

void BloodSystem::updateSplashes(float deltaTime) {
    for (auto& splash : m_splashes) {
        if (splash) {
            splash->update(deltaTime);
            
            // Apply physics to splash
            applyPhysicsToSplash(splash, deltaTime);
            
            // Remove completed splashes
            if (splash->isCompleted()) {
                RF_DELETE(splash);
                m_splashes.erase(std::find(m_splashes.begin(), m_splashes.end(), splash));
                m_splashCount--;
            }
        }
    }
    
    // Generate new splashes based on impacts
    updateSplashGeneration(deltaTime);
}

void BloodSystem::updateSplashGeneration(float deltaTime) {
    // Generate new splashes based on impacts
    // This would be connected to the impact system
    // For now, just update existing splashes
    (void)deltaTime;
}

void BloodSystem::applyPhysicsToSplash(BloodSplash* splash, float deltaTime) {
    if (!splash) {
        return;
    }
    
    // Apply gravity to splash
    Vec3 gravityForce = Vec3(0.0f, -m_gravity, 0.0f);
    Vec3 currentVelocity = splash->getVelocity();
    currentVelocity += gravityForce * deltaTime;
    
    // Apply wind to splash
    currentVelocity += m_windForce * deltaTime;
    
    // Apply temperature effects
    float temperatureEffect = (m_temperature - 20.0f) * 0.01f;
    currentVelocity += Vec3(temperatureEffect, 0.0f, 0.0f) * deltaTime;
    
    // Apply humidity effects
    float humidityEffect = (m_humidity - 0.5f) * 0.005f;
    currentVelocity *= (1.0f - humidityEffect * deltaTime);
    
    // Apply pressure effects
    float pressureEffect = (m_pressure - 101325.0f) * 0.00001f;
    currentVelocity *= (1.0f + pressureEffect * deltaTime);
    
    splash->setVelocity(currentVelocity);
    
    // Update position
    Vec3 currentPosition = splash->getPosition();
    currentPosition += currentVelocity * deltaTime;
    splash->setPosition(currentPosition);
}

void BloodSystem::updateStreams(float deltaTime) {
    for (auto& stream : m_streams) {
        if (stream) {
            stream->update(deltaTime);
            
            // Apply physics to stream
            applyPhysicsToStream(stream, deltaTime);
            
            // Remove completed streams
            if (stream->isCompleted()) {
                RF_DELETE(stream);
                m_streams.erase(std::find(m_streams.begin(), m_streams.end(), stream));
                m_streamCount--;
            }
        }
    }
    
    // Generate new streams based on flow conditions
    updateStreamGeneration(deltaTime);
}

void BloodSystem::updateStreamGeneration(float deltaTime) {
    // Generate new streams based on flow conditions
    // This would be connected to the flow system
    // For now, just update existing streams
    (void)deltaTime;
}

void BloodSystem::applyPhysicsToStream(BloodStream* stream, float deltaTime) {
    if (!stream) {
        return;
    }
    
    // Apply gravity to stream
    Vec3 gravityForce = Vec3(0.0f, -m_gravity, 0.0f);
    Vec3 currentVelocity = stream->getVelocity();
    currentVelocity += gravityForce * deltaTime;
    
    // Apply wind to stream
    currentVelocity += m_windForce * deltaTime;
    
    // Apply temperature effects
    float temperatureEffect = (m_temperature - 20.0f) * 0.01f;
    currentVelocity += Vec3(temperatureEffect, 0.0f, 0.0f) * deltaTime;
    
    // Apply humidity effects
    float humidityEffect = (m_humidity - 0.5f) * 0.005f;
    currentVelocity *= (1.0f - humidityEffect * deltaTime);
    
    // Apply pressure effects
    float pressureEffect = (m_pressure - 101325.0f) * 0.00001f;
    currentVelocity *= (1.0f + pressureEffect * deltaTime);
    
    stream->setVelocity(currentVelocity);
    
    // Update position
    Vec3 currentPosition = stream->getPosition();
    currentPosition += currentVelocity * deltaTime;
    stream->setPosition(currentPosition);
}

void BloodSystem::updateTrails(float deltaTime) {
    for (auto& trail : m_trails) {
        if (trail) {
            trail->update(deltaTime);
            
            // Apply physics to trail
            applyPhysicsToTrail(trail, deltaTime);
            
            // Remove completed trails
            if (trail->isCompleted()) {
                RF_DELETE(trail);
                m_trails.erase(std::find(m_trails.begin(), m_trails.end(), trail));
                m_trailCount--;
            }
        }
    }
    
    // Generate new trails based on movement
    updateTrailGeneration(deltaTime);
}

void BloodSystem::updateTrailGeneration(float deltaTime) {
    // Generate new trails based on movement
    // This would be connected to the movement system
    // For now, just update existing trails
    (void)deltaTime;
}

void BloodSystem::applyPhysicsToTrail(BloodTrail* trail, float deltaTime) {
    if (!trail) {
        return;
    }
    
    // Apply gravity to trail
    Vec3 gravityForce = Vec3(0.0f, -m_gravity, 0.0f);
    Vec3 currentVelocity = trail->getVelocity();
    currentVelocity += gravityForce * deltaTime;
    
    // Apply wind to trail
    currentVelocity += m_windForce * deltaTime;
    
    // Apply temperature effects
    float temperatureEffect = (m_temperature - 20.0f) * 0.01f;
    currentVelocity += Vec3(temperatureEffect, 0.0f, 0.0f) * deltaTime;
    
    // Apply humidity effects
    float humidityEffect = (m_humidity - 0.5f) * 0.005f;
    currentVelocity *= (1.0f - humidityEffect * deltaTime);
    
    // Apply pressure effects
    float pressureEffect = (m_pressure - 101325.0f) * 0.00001f;
    currentVelocity *= (1.0f + pressureEffect * deltaTime);
    
    trail->setVelocity(currentVelocity);
    
    // Update position
    Vec3 currentPosition = trail->getPosition();
    currentPosition += currentVelocity * deltaTime;
    trail->setPosition(currentPosition);
}

void BloodSystem::updateFlows(float deltaTime) {
    for (auto& flow : m_flows) {
        if (flow) {
            flow->update(deltaTime);
            
            // Apply physics to flow
            applyPhysicsToFlow(flow, deltaTime);
            
            // Remove completed flows
            if (flow->isCompleted()) {
                RF_DELETE(flow);
                m_flows.erase(std::find(m_flows.begin(), m_flows.end(), flow));
                m_flowCount--;
            }
        }
    }
    
    // Generate new flows based on surface conditions
    updateFlowGeneration(deltaTime);
}

void BloodSystem::updateFlowGeneration(float deltaTime) {
    // Generate new flows based on surface conditions
    // This would be connected to the surface system
    // For now, just update existing flows
    (void)deltaTime;
}

void BloodSystem::applyPhysicsToFlow(BloodFlow* flow, float deltaTime) {
    if (!flow) {
        return;
    }
    
    // Apply gravity to flow
    Vec3 gravityForce = Vec3(0.0f, -m_gravity, 0.0f);
    Vec3 currentVelocity = flow->getVelocity();
    currentVelocity += gravityForce * deltaTime;
    
    // Apply wind to flow
    currentVelocity += m_windForce * deltaTime;
    
    // Apply temperature effects
    float temperatureEffect = (m_temperature - 20.0f) * 0.01f;
    currentVelocity += Vec3(temperatureEffect, 0.0f, 0.0f) * deltaTime;
    
    // Apply humidity effects
    float humidityEffect = (m_humidity - 0.5f) * 0.005f;
    currentVelocity *= (1.0f - humidityEffect * deltaTime);
    
    // Apply pressure effects
    float pressureEffect = (m_pressure - 101325.0f) * 0.00001f;
    currentVelocity *= (1.0f + pressureEffect * deltaTime);
    
    flow->setVelocity(currentVelocity);
    
    // Update position
    Vec3 currentPosition = flow->getPosition();
    currentPosition += currentVelocity * deltaTime;
    flow->setPosition(currentPosition);
}

void BloodSystem::updateSpreads(float deltaTime) {
    for (auto& spread : m_spreads) {
        if (spread) {
            spread->update(deltaTime);
            
            // Apply physics to spread
            applyPhysicsToSpread(spread, deltaTime);
            
            // Remove completed spreads
            if (spread->isCompleted()) {
                RF_DELETE(spread);
                m_spreads.erase(std::find(m_spreads.begin(), m_spreads.end(), spread));
                m_spreadCount--;
            }
        }
    }
    
    // Generate new spreads based on conditions
    updateSpreadGeneration(deltaTime);
}

void BloodSystem::updateSpreadGeneration(float deltaTime) {
    // Generate new spreads based on conditions
    // This would be connected to the surface system
    // For now, just update existing spreads
    (void)deltaTime;
}

void BloodSystem::applyPhysicsToSpread(BloodSpread* spread, float deltaTime) {
    if (!spread) {
        return;
    }
    
    // Apply spreading to spread
    float spreadRate = spread->getSpreadRate();
    float currentSize = spread->getSize();
    currentSize += spreadRate * deltaTime;
    spread->setSize(currentSize);
    
    // Apply evaporation to spread
    float evaporationRate = spread->getEvaporationRate();
    currentSize *= (1.0f - evaporationRate * deltaTime);
    spread->setSize(currentSize);
    
    // Apply coagulation to spread
    float coagulationRate = spread->getCoagulationRate();
    float currentViscosity = spread->getViscosity();
    currentViscosity += coagulationRate * deltaTime;
    spread->setViscosity(currentViscosity);
}

void BloodSystem::updatePhysics(float deltaTime) {
    // Update environmental physics
    updateEnvironmentalPhysics(deltaTime);
    
    // Update global physics parameters
    updateGlobalPhysics(deltaTime);
}

void BloodSystem::updateEnvironmentalPhysics(float deltaTime) {
    // Update environmental conditions
    // This would be connected to the environmental system
    // For now, just use current values
    (void)deltaTime;
}

void BloodSystem::updateGlobalPhysics(float deltaTime) {
    // Update global physics parameters
    // This would be connected to the global physics system
    // For now, just use current values
    (void)deltaTime;
}

void BloodSystem::updateComponentGeneration(float deltaTime) {
    // Generate new components based on conditions
    // This would be connected to the component generation system
    // For now, just update existing components
    (void)deltaTime;
}

void BloodSystem::setTimeScale(float scale) {
    m_timeScale = scale;
    RF_LOG_INFO("Blood system time scale set to %.2f", scale);
}

void BloodSystem::setActive(bool active) {
    m_isActive = active;
    RF_LOG_INFO("Blood system %s", active ? "activated" : "deactivated");
}

void BloodSystem::setPaused(bool paused) {
    m_isPaused = paused;
    RF_LOG_INFO("Blood system %s", paused ? "paused" : "resumed");
}

void BloodSystem::setGravity(float gravity) {
    m_gravity = gravity;
    RF_LOG_INFO("Blood system gravity set to %.2f", gravity);
}

void BloodSystem::setWindForce(const Vec3& windForce) {
    m_windForce = windForce;
    RF_LOG_INFO("Blood system wind force set to (%.2f, %.2f, %.2f)", 
                windForce.x, windForce.y, windForce.z);
}

void BloodSystem::setTemperature(float temperature) {
    m_temperature = temperature;
    RF_LOG_INFO("Blood system temperature set to %.2f", temperature);
}

void BloodSystem::setHumidity(float humidity) {
    m_humidity = humidity;
    RF_LOG_INFO("Blood system humidity set to %.2f", humidity);
}

void BloodSystem::setPressure(float pressure) {
    m_pressure = pressure;
    RF_LOG_INFO("Blood system pressure set to %.2f", pressure);
}

void BloodSystem::setMaxParticles(size_t maxParticles) {
    m_maxParticles = maxParticles;
    m_particles.reserve(maxParticles);
    RF_LOG_INFO("Blood system max particles set to %zu", maxParticles);
}

void BloodSystem::setMaxEffects(size_t maxEffects) {
    m_maxEffects = maxEffects;
    m_effects.reserve(maxEffects);
    RF_LOG_INFO("Blood system max effects set to %zu", maxEffects);
}

void BloodSystem::setMaxCollisions(size_t maxCollisions) {
    m_maxCollisions = maxCollisions;
    m_collisions.reserve(maxCollisions);
    RF_LOG_INFO("Blood system max collisions set to %zu", maxCollisions);
}

float BloodSystem::getSystemTime() const {
    return m_systemTime;
}

float BloodSystem::getTimeScale() const {
    return m_timeScale;
}

bool BloodSystem::isActive() const {
    return m_isActive;
}

bool BloodSystem::isPaused() const {
    return m_isPaused;
}

float BloodSystem::getGravity() const {
    return m_gravity;
}

const Vec3& BloodSystem::getWindForce() const {
    return m_windForce;
}

float BloodSystem::getTemperature() const {
    return m_temperature;
}

float BloodSystem::getHumidity() const {
    return m_humidity;
}

float BloodSystem::getPressure() const {
    return m_pressure;
}

size_t BloodSystem::getParticleCount() const {
    return m_particleCount;
}

size_t BloodSystem::getEffectCount() const {
    return m_effectCount;
}

size_t BloodSystem::getCollisionCount() const {
    return m_collisionCount;
}

size_t BloodSystem::getDripCount() const {
    return m_dripCount;
}

size_t BloodSystem::getPoolCount() const {
    return m_poolCount;
}

size_t BloodSystem::getSpatterCount() const {
    return m_spatterCount;
}

size_t BloodSystem::getSplashCount() const {
    return m_splashCount;
}

size_t BloodSystem::getStreamCount() const {
    return m_streamCount;
}

size_t BloodSystem::getTrailCount() const {
    return m_trailCount;
}

size_t BloodSystem::getFlowCount() const {
    return m_flowCount;
}

size_t BloodSystem::getSpreadCount() const {
    return m_spreadCount;
}

const std::vector<BloodParticle*>& BloodSystem::getParticles() const {
    return m_particles;
}

const std::vector<BloodEffect*>& BloodSystem::getEffects() const {
    return m_effects;
}

const std::vector<BloodCollision*>& BloodSystem::getCollisions() const {
    return m_collisions;
}

const std::vector<BloodDrip*>& BloodSystem::getDrips() const {
    return m_drips;
}

const std::vector<BloodPool*>& BloodSystem::getPools() const {
    return m_pools;
}

const std::vector<BloodSpatter*>& BloodSystem::getSplatters() const {
    return m_splatters;
}

const std::vector<BloodSplash*>& BloodSystem::getSplashes() const {
    return m_splashes;
}

const std::vector<BloodStream*>& BloodSystem::getStreams() const {
    return m_streams;
}

const std::vector<BloodTrail*>& BloodSystem::getTrails() const {
    return m_trails;
}

const std::vector<BloodFlow*>& BloodSystem::getFlows() const {
    return m_flows;
}

const std::vector<BloodSpread*>& BloodSystem::getSpreads() const {
    return m_spreads;
}

} // namespace Blood
} // namespace RedFiles