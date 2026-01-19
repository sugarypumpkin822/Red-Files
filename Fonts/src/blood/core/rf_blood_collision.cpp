#include "rf_blood_collision.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

BloodCollision::BloodCollision()
    : m_collisionType(CollisionType::Sphere)
    , m_restitution(0.3f)
    , m_friction(0.5f)
    , m_maxCollisions(100)
    , m_collisionCount(0)
    , m_isEnabled(true)
{
    m_collisions.reserve(m_maxCollisions);
    RF_LOG_INFO("BloodCollision created");
}

BloodCollision::~BloodCollision() {
    clear();
    RF_LOG_INFO("BloodCollision destroyed");
}

void BloodCollision::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("BloodCollision initialized");
}

void BloodCollision::clear() {
    for (auto& collision : m_collisions) {
        if (collision) {
            RF_DELETE(collision);
        }
    }
    
    m_collisions.clear();
    m_collisionCount = 0;
    RF_LOG_INFO("Cleared all blood collisions");
}

void BloodCollision::addCollision(const Vec3& position, const Vec3& velocity, float radius, 
                             const Color& color, float lifetime) {
    if (m_collisionCount >= m_maxCollisions) {
        // Remove oldest collision
        RF_DELETE(m_collisions.front());
        m_collisions.erase(m_collisions.begin());
        m_collisionCount--;
    }
    
    auto collision = RF_NEW(BloodCollision);
    collision->initialize(position, velocity, radius, color, lifetime, m_properties);
    
    m_collisions.push_back(collision);
    m_collisionCount++;
    
    RF_LOG_INFO("Added blood collision at position (%.2f, %.2f, %.2f)", 
                position.x, position.y, position.z);
}

void BloodCollision::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update existing collisions
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

void BloodCollision::updateCollisionDetection(float deltaTime) {
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

void BloodCollision::resolveCollision(BloodCollision* collision1, BloodCollision* collision2) {
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
    Vec3 newVelocity1 = collision1->getVelocity() - normal * velocityAlongNormal * (1.0f + m_restitution);
    Vec3 newVelocity2 = collision2->getVelocity() + normal * velocityAlongNormal * (1.0f + m_restitution);
    
    // Apply friction
    newVelocity1 -= velocityTangent * m_friction;
    newVelocity2 -= velocityTangent * m_friction;
    
    collision1->setVelocity(newVelocity1);
    collision2->setVelocity(newVelocity2);
    
    // Separate colliding objects
    Vec3 separation = normal * (collision1->getRadius() + collision2->getRadius() - Math::length(collision2->getPosition() - collision1->getPosition())) * 0.5f;
    collision1->setPosition(collision1->getPosition() + separation);
    collision2->setPosition(collision2->getPosition() - separation);
    
    RF_LOG_INFO("Resolved collision between blood particles");
}

void BloodCollision::setCollisionType(CollisionType type) {
    m_collisionType = type;
    RF_LOG_INFO("Blood collision type set to %d", static_cast<int>(type));
}

void BloodCollision::setRestitution(float restitution) {
    m_restitution = Math::clamp(restitution, 0.0f, 1.0f);
    RF_LOG_INFO("Blood collision restitution set to %.2f", restitution);
}

void BloodCollision::setFriction(float friction) {
    m_friction = Math::clamp(friction, 0.0f, 1.0f);
    RF_LOG_INFO("Blood collision friction set to %.2f", friction);
}

void BloodCollision::setMaxCollisions(int maxCollisions) {
    m_maxCollisions = maxCollisions;
    m_collisions.reserve(maxCollisions);
    RF_LOG_INFO("Blood collision max collisions set to %d", maxCollisions);
}

void BloodCollision::setEnabled(bool enabled) {
    m_isEnabled = enabled;
    RF_LOG_INFO("Blood collision %s", enabled ? "enabled" : "disabled");
}

size_t BloodCollision::getCollisionCount() const {
    return m_collisionCount;
}

CollisionType BloodCollision::getCollisionType() const {
    return m_collisionType;
}

float BloodCollision::getRestitution() const {
    return m_restitution;
}

float BloodCollision::getFriction() const {
    return m_friction;
}

bool BloodCollision::isEnabled() const {
    return m_isEnabled;
}

const std::vector<BloodCollision*>& BloodCollision::getCollisions() const {
    return m_collisions;
}

void BloodCollision::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& BloodCollision::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles