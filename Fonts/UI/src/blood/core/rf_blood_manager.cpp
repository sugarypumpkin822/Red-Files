#include "rf_blood_manager.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

BloodManager::BloodManager()
    : m_globalTime(0.0f)
    , m_timeScale(1.0f)
    , m_isPaused(false)
    , m_maxParticles(1000)
    , m_maxEffects(500)
    , m_isEnabled(true)
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
    
    RF_LOG_INFO("BloodManager created");
}

BloodManager::~BloodManager() {
    clear();
    RF_LOG_INFO("BloodManager destroyed");
}

void BloodManager::initialize() {
    RF_LOG_INFO("BloodManager initialized");
}

void BloodManager::clear() {
    // Clear all blood systems
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
    
    RF_LOG_INFO("Cleared all blood systems");
}

void BloodManager::update(float deltaTime) {
    if (!m_isEnabled || m_isPaused) {
        return;
    }
    
    // Update global time
    m_globalTime += deltaTime * m_timeScale;
    
    // Update all blood systems
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
}

void BloodManager::updateParticles(float deltaTime) {
    for (auto& particle : m_particles) {
        if (particle) {
            particle->update(deltaTime);
            
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

void BloodManager::updateParticleGeneration(float deltaTime) {
    // Generate new particles based on conditions
    // This would be connected to the particle generation system
    // For now, just update existing particles
    (void)deltaTime;
}

void BloodManager::updateEffects(float deltaTime) {
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

void BloodManager::updateEffectGeneration(float deltaTime) {
    // Generate new effects based on conditions
    // This would be connected to the effect generation system
    // For now, just update existing effects
    (void)deltaTime;
}

void BloodManager::updateCollisions(float deltaTime) {
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

void BloodManager::updateCollisionDetection(float deltaTime) {
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

void BloodManager::resolveCollision(BloodCollision* collision1, BloodCollision* collision2) {
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

void BloodManager::updateDrips(float deltaTime) {
    for (auto& drip : m_drips) {
        if (drip) {
            drip->update(deltaTime);
            
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

void BloodManager::updateDripGeneration(float deltaTime) {
    // Generate new drips based on surface conditions
    // This would be connected to the surface system
    // For now, just update existing drips
    (void)deltaTime;
}

void BloodManager::updatePools(float deltaTime) {
    for (auto& pool : m_pools) {
        if (pool) {
            pool->update(deltaTime);
            
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

void BloodManager::updatePoolGeneration(float deltaTime) {
    // Generate new pools based on drips
    // This would be connected to the drip system
    // For now, just update existing pools
    (void)deltaTime;
}

void BloodManager::updateSplatters(float deltaTime) {
    for (auto& spatter : m_splatters) {
        if (spatter) {
            spatter->update(deltaTime);
            
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

void BloodManager::updateSpatterGeneration(float deltaTime) {
    // Generate new splatters based on impacts
    // This would be connected to the impact system
    // For now, just update existing splatters
    (void)deltaTime;
}

void BloodManager::updateSplashes(float deltaTime) {
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
    
    // Generate new splashes based on impacts
    updateSplashGeneration(deltaTime);
}

void BloodManager::updateSplashGeneration(float deltaTime) {
    // Generate new splashes based on impacts
    // This would be connected to the impact system
    // For now, just update existing splashes
    (void)deltaTime;
}

void BloodManager::updateStreams(float deltaTime) {
    for (auto& stream : m_streams) {
        if (stream) {
            stream->update(deltaTime);
            
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

void BloodManager::updateStreamGeneration(float deltaTime) {
    // Generate new streams based on flow conditions
    // This would be connected to the flow system
    // For now, just update existing streams
    (void)deltaTime;
}

void BloodManager::updateTrails(float deltaTime) {
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
    
    // Generate new trails based on movement
    updateTrailGeneration(deltaTime);
}

void BloodManager::updateTrailGeneration(float deltaTime) {
    // Generate new trails based on movement
    // This would be connected to the movement system
    // For now, just update existing trails
    (void)deltaTime;
}

void BloodManager::updateFlows(float deltaTime) {
    for (auto& flow : m_flows) {
        if (flow) {
            flow->update(deltaTime);
            
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

void BloodManager::updateFlowGeneration(float deltaTime) {
    // Generate new flows based on surface conditions
    // This would be connected to the surface system
    // For now, just update existing flows
    (void)deltaTime;
}

void BloodManager::updateSpreads(float deltaTime) {
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
    
    // Generate new spreads based on conditions
    updateSpreadGeneration(deltaTime);
}

void BloodManager::updateSpreadGeneration(float deltaTime) {
    // Generate new spreads based on conditions
    // This would be connected to the surface system
    // For now, just update existing spreads
    (void)deltaTime;
}

void BloodManager::setTimeScale(float scale) {
    m_timeScale = scale;
    RF_LOG_INFO("Blood manager time scale set to %.2f", scale);
}

void BloodManager::setPaused(bool paused) {
    m_isPaused = paused;
    RF_LOG_INFO("Blood manager %s", paused ? "paused" : "resumed");
}

void BloodManager::setEnabled(bool enabled) {
    m_isEnabled = enabled;
    RF_LOG_INFO("Blood manager %s", enabled ? "enabled" : "disabled");
}

void BloodManager::setMaxParticles(size_t maxParticles) {
    m_maxParticles = maxParticles;
    m_particles.reserve(maxParticles);
    RF_LOG_INFO("Blood manager max particles set to %zu", maxParticles);
}

void BloodManager::setMaxEffects(size_t maxEffects) {
    m_maxEffects = maxEffects;
    m_effects.reserve(maxEffects);
    RF_LOG_INFO("Blood manager max effects set to %zu", maxEffects);
}

void BloodManager::setMaxCollisions(size_t maxCollisions) {
    m_maxCollisions = maxCollisions;
    m_collisions.reserve(maxCollisions);
    RF_LOG_INFO("Blood manager max collisions set to %zu", maxCollisions);
}

void BloodManager::setMaxDrips(size_t maxDrips) {
    m_maxDrips = maxDrips;
    m_drips.reserve(maxDrips);
    RF_LOG_INFO("Blood manager max drips set to %zu", maxDrips);
}

void BloodManager::setMaxPools(size_t maxPools) {
    m_maxPools = maxPools;
    m_pools.reserve(maxPools);
    RF_LOG_INFO("Blood manager max pools set to %zu", maxPools);
}

void BloodManager::setMaxSplatters(size_t maxSplatters) {
    m_maxSplatters = maxSplatters;
    m_splatters.reserve(maxSplatters);
    RF_LOG_INFO("Blood manager max splatters set to %zu", maxSplatters);
}

void BloodManager::setMaxSplashes(size_t maxSplashes) {
    m_maxSplashes = maxSplashes;
    m_splashes.reserve(maxSplashes);
    RF_LOG_INFO("Blood manager max splashes set to %zu", maxSplashes);
}

void BloodManager::setMaxStreams(size_t maxStreams) {
    m_maxStreams = maxStreams;
    m_streams.reserve(maxStreams);
    RF_LOG_INFO("Blood manager max streams set to %zu", maxStreams);
}

void BloodManager::setMaxTrails(size_t maxTrails) {
    m_maxTrails = maxTrails;
    m_trails.reserve(maxTrails);
    RF_LOG_INFO("Blood manager max trails set to %zu", maxTrails);
}

void BloodManager::setMaxFlows(size_t maxFlows) {
    m_maxFlows = maxFlows;
    m_flows.reserve(maxFlows);
    RF_LOG_INFO("Blood manager max flows set to %zu", maxFlows);
}

void BloodManager::setMaxSpreads(size_t maxSpreads) {
    m_maxSpreads = maxSpreads;
    m_spreads.reserve(maxSpreads);
    RF_LOG_INFO("Blood manager max spreads set to %zu", maxSpreads);
}

float BloodManager::getGlobalTime() const {
    return m_globalTime;
}

float BloodManager::getTimeScale() const {
    return m_timeScale;
}

bool BloodManager::isPaused() const {
    return m_isPaused;
}

bool BloodManager::isEnabled() const {
    return m_isEnabled;
}

size_t BloodManager::getParticleCount() const {
    return m_particleCount;
}

size_t BloodManager::getEffectCount() const {
    return m_effectCount;
}

size_t BloodManager::getCollisionCount() const {
    return m_collisionCount;
}

size_t BloodManager::getDripCount() const {
    return m_dripCount;
}

size_t BloodManager::getPoolCount() const {
    return m_poolCount;
}

size_t BloodManager::getSpatterCount() const {
    return m_spatterCount;
}

size_t BloodManager::getSplashCount() const {
    return m_splashCount;
}

size_t BloodManager::getStreamCount() const {
    return m_streamCount;
}

size_t BloodManager::getTrailCount() const {
    return m_trailCount;
}

size_t BloodManager::getFlowCount() const {
    return m_flowCount;
}

size_t BloodManager::getSpreadCount() const {
    return m_spreadCount;
}

const std::vector<BloodParticle*>& BloodManager::getParticles() const {
    return m_particles;
}

const std::vector<BloodEffect*>& BloodManager::getEffects() const {
    return m_effects;
}

const std::vector<BloodCollision*>& BloodManager::getCollisions() const {
    return m_collisions;
}

const std::vector<BloodDrip*>& BloodManager::getDrips() const {
    return m_drips;
}

const std::vector<BloodPool*>& BloodManager::getPools() const {
    return m_pools;
}

const std::vector<BloodSpatter*>& BloodManager::getSplatters() const {
    return m_splatters;
}

const std::vector<BloodSplash*>& BloodManager::getSplashes() const {
    return m_splashes;
}

const std::vector<BloodStream*>& BloodManager::getStreams() const {
    return m_streams;
}

const std::vector<BloodTrail*>& BloodManager::getTrails() const {
    return m_trails;
}

const std::vector<BloodFlow*>& BloodManager::getFlows() const {
    return m_flows;
}

const std::vector<BloodSpread*>& BloodManager::getSpreads() const {
    return m_spreads;
}

} // namespace Blood
} // namespace RedFiles