#include "rf_blood_pool.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

BloodPoolBehavior::BloodPoolBehavior()
    : m_poolSize(0.0f)
    , m_spreadRate(0.5f)
    , m_coagulationRate(0.1f)
    , m_evaporationRate(0.02f)
    , m_minPoolSize(0.5f)
    , m_maxPoolSize(10.0f)
    , m_isEnabled(true)
    , m_poolCount(0)
    , m_maxPools(50)
{
    m_pools.reserve(m_maxPools);
    RF_LOG_INFO("BloodPoolBehavior created");
}

BloodPoolBehavior::~BloodPoolBehavior() {
    clear();
    RF_LOG_INFO("BloodPoolBehavior destroyed");
}

void BloodPoolBehavior::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("BloodPoolBehavior initialized");
}

void BloodPoolBehavior::clear() {
    for (auto& pool : m_pools) {
        if (pool) {
            RF_DELETE(pool);
        }
    }
    
    m_pools.clear();
    m_poolCount = 0;
    RF_LOG_INFO("Cleared all blood pools");
}

void BloodPoolBehavior::addPool(const Vec3& position, float size, const Color& color) {
    if (m_poolCount >= m_maxPools) {
        // Remove oldest pool
        RF_DELETE(m_pools.front());
        m_pools.erase(m_pools.begin());
        m_poolCount--;
    }
    
    if (size < m_minPoolSize) {
        size = m_minPoolSize;
    }
    
    if (size > m_maxPoolSize) {
        size = m_maxPoolSize;
    }
    
    auto pool = RF_NEW(BloodPool);
    pool->initialize(position, size, color, m_properties);
    
    m_pools.push_back(pool);
    m_poolCount++;
    
    RF_LOG_INFO("Added blood pool at position (%.2f, %.2f, %.2f)", 
                position.x, position.y, position.z);
}

void BloodPoolBehavior::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update existing pools
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
    
    // Add new pools based on existing drips
    updatePoolGeneration(deltaTime);
}

void BloodPoolBehavior::updatePoolGeneration(float deltaTime) {
    // Generate new pools based on blood drips
    // This would be connected to the blood drip system
    // For now, just update existing pools
    (void)deltaTime;
}

void BloodPoolBehavior::setPoolSize(float size) {
    m_poolSize = size;
    RF_LOG_INFO("Blood pool size set to %.2f", size);
}

void BloodPoolBehavior::setSpreadRate(float rate) {
    m_spreadRate = rate;
    RF_LOG_INFO("Blood pool spread rate set to %.2f", rate);
}

void BloodPoolBehavior::setCoagulationRate(float rate) {
    m_coagulationRate = rate;
    RF_LOG_INFO("Blood pool coagulation rate set to %.2f", rate);
}

void BloodPoolBehavior::setEvaporationRate(float rate) {
    m_evaporationRate = rate;
    RF_LOG_INFO("Blood pool evaporation rate set to %.2f", rate);
}

void BloodPoolBehavior::setMinPoolSize(float size) {
    m_minPoolSize = size;
    RF_LOG_INFO("Blood min pool size set to %.2f", size);
}

void BloodPoolBehavior::setMaxPoolSize(float size) {
    m_maxPoolSize = size;
    RF_LOG_INFO("Blood max pool size set to %.2f", size);
}

void BloodPoolBehavior::setMaxPools(int maxPools) {
    m_maxPools = maxPools;
    m_pools.reserve(maxPools);
    RF_LOG_INFO("Blood max pools set to %d", maxPools);
}

size_t BloodPoolBehavior::getPoolCount() const {
    return m_poolCount;
}

float BloodPoolBehavior::getPoolSize() const {
    return m_poolSize;
}

float BloodPoolBehavior::getSpreadRate() const {
    return m_spreadRate;
}

float BloodPoolBehavior::getCoagulationRate() const {
    return m_coagulationRate;
}

float BloodPoolBehavior::getEvaporationRate() const {
    return m_evaporationRate;
}

float BloodPoolBehavior::getMinPoolSize() const {
    return m_minPoolSize;
}

float BloodPoolBehavior::getMaxPoolSize() const {
    return m_maxPoolSize;
}

bool BloodPoolBehavior::isEnabled() const {
    return m_isEnabled;
}

const std::vector<BloodPool*>& BloodPoolBehavior::getPools() const {
    return m_pools;
}

void BloodPoolBehavior::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& BloodPoolBehavior::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles