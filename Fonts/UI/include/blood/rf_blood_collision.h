#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <unordered_map>

namespace RFBlood {

// Collision types
enum class CollisionType {
    PARTICLE_PARTICLE,
    PARTICLE_SURFACE,
    PARTICLE_MESH,
    PARTICLE_TERRAIN,
    PARTICLE_FLUID,
    PARTICLE_RIGID_BODY
    CUSTOM
};

// Collision response
enum class CollisionResponse {
    NONE,
    BOUNCE,
    STICK,
    SLIDE,
    SINK,
    MERGE,
    DEFORM,
    DESTROY
};

// Collision shape types
enum class CollisionShape {
    SPHERE,
    BOX,
    CAPSULE,
    CONVEX,
    MESH,
    COMPOUND
    CUSTOM
};

// Collision manifold
struct CollisionManifold {
    std::array<float, 3> normal;
    float penetrationDepth;
    std::array<float, 2> contactPoint1;
    std::array<float, 2> contactPoint2;
    float restitution;
    float friction;
    bool isValid;
    
    CollisionManifold() : normal{0.0f, 0.0f, 0.0f}, penetrationDepth(0.0f), 
                    contactPoint1{0.0f, 0.0f}, contactPoint2{0.0f, 0.0f}, 
                    restitution(0.0f), friction(0.0f), isValid(false) {}
};

// Collision pair
struct CollisionPair {
    uint32_t particleId1;
    uint32_t particleId2;
    CollisionType type;
    CollisionResponse response;
    float separationX, separationY, separationZ;
    float impulseX, impulseY, impulseZ;
    float contactTime;
    bool isColliding;
    
    CollisionPair() : particleId1(0), particleId2(0), type(CollisionType::PARTICLE_PARTICLE), 
                   response(CollisionResponse::NONE), separationX(0.0f), separationY(0.0f), separationZ(0.0f), 
                   impulseX(0.0f), impulseY(0.0f), impulseZ(0.0f), contactTime(0.0f), 
                   isColliding(false) {}
    CollisionPair(uint32_t id1, uint32_t id2, CollisionType t, CollisionResponse r, 
               float sx, float sy, float sz, float ix, float iy, float iz, 
               float ct, bool colliding)
        : particleId1(id1), particleId2(id2), type(t), response(r), 
          separationX(sx), separationY(sy), separationZ(sz), impulseX(ix), impulseY(iy), 
          impulseZ(iz), contactTime(ct), isColliding(colliding) {}
};

// Collision system
class BloodCollision {
public:
    BloodCollision();
    ~BloodCollision() = default;
    
    // Collision management
    void initialize(int maxParticles, float worldSize);
    void update(float deltaTime);
    void reset();
    void setPaused(bool paused);
    bool isPaused() const;
    
    // Collision detection
    void addCollisionObject(const std::vector<BloodParticle>& particles);
    void removeCollisionObject(uint32_t objectId);
    void setCollisionType(uint32_t objectId, CollisionType type);
    void setCollisionShape(uint32_t objectId, CollisionShape shape);
    void setCollisionResponse(uint32_t objectId, CollisionResponse response);
    void setRestitution(float restitution);
    void setFriction(float friction);
    
    // Spatial partitioning
    void enableSpatialPartitioning(bool enabled);
    void setGridSize(float gridSize);
    void setMaxDepth(int maxDepth);
    void setMaxObjectsPerCell(int maxObjects);
    
    // Broadphase
    void performBroadphase();
    void narrowPhase();
    
    // Collision queries
    bool isColliding(uint32_t particleId1, uint32_t particleId2) const;
    const std::vector<CollisionPair>& getCollisionPairs() const;
    const std::vector<CollisionPair>& getActiveCollisions() const;
    
    // Event handling
    void addCollisionEventListener(const std::string& eventType, std::function<void()> callback);
    void removeCollisionEventListener(const std::string& eventType, std::function<void()> callback);
    void clearCollisionEventListeners();
    
    // Utility methods
    void cloneFrom(const BloodCollision& other);
    std::unique_ptr<BloodCollision> clone() const;
    
    // Data access
    const std::vector<CollisionPair>& getCollisionPairs() const;
    const std::vector<CollisionPair>& getActiveCollisions() const;
    
protected:
    // Protected members
    std::vector<BloodParticle> collisionObjects_;
    std::vector<CollisionPair> collisionPairs_;
    std::vector<CollisionPair> activeCollisions_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    bool isPaused_;
    float worldSize_;
    float gridSize_;
    int maxDepth_;
    int maxObjectsPerCell_;
    
    bool spatialPartitioningEnabled_;
    
    // Spatial partitioning grid
    std::vector<std::vector<uint32_t>> spatialGrid_;
    std::vector<std::vector<std::vector<uint32_t>>> cellOccupancy_;
    
    // Performance optimization
    bool broadphaseEnabled_;
    int maxBroadphasePairs;
    
    // Protected helper methods
    virtual void updateCollisions(float deltaTime);
    virtual void performBroadphase();
    virtual void narrowPhase();
    virtual void resolveCollision(const CollisionPair& pair);
    virtual void updateCollisionPair(const CollisionPair& pair, float deltaTime);
    virtual void separateCollisionPair(const CollisionPair& pair);
    virtual void removeCollisionPair(const CollisionPair& pair);
    
    // Spatial partitioning helpers
    virtual void updateSpatialGrid();
    virtual void addParticleToGrid(const BloodParticle& particle);
    virtual void removeParticleFromGrid(uint32_t particleId);
    virtual std::vector<uint32_t> getNearbyCells(float x, float y, float z, float radius);
    virtual std::vector<uint32_t> getCellCoords(float x, float y, float z);
    
    // Collision detection helpers
    virtual bool checkSphereCollision(const BloodParticle& a, const BloodParticle& b);
    virtual bool checkBoxCollision(const BloodParticle& a, const BloodParticle& b);
    virtual bool checkCapsuleCollision(const BloodParticle& a, const BloodParticle& b);
    virtual bool checkMeshCollision(const BloodParticle& a, const BloodParticle& b);
    virtual bool checkTerrainCollision(const BloodParticle& a, const BloodParticle& b);
    virtual bool checkFluidCollision(const BloodParticle& a, const BloodParticle& b);
    
    // Collision resolution
    virtual void resolveSphereCollision(const CollisionPair& pair);
    virtual void resolveBoxCollision(const CollisionPair& pair);
    virtual void resolveCapsuleCollision(const CollisionPair& pair);
    virtual void resolveMeshCollision(const CollisionPair& pair);
    virtual void resolveTerrainCollision(const CollisionPair& pair);
    virtual void resolveFluidCollision(const CollisionPair& pair);
    virtual void applyCollisionResponse(const CollisionPair& pair);
    
    // Performance optimization
    void optimizeBroadphasePairs();
    void optimizeNarrowPhasePairs();
    void optimizeSpatialGrid();
    void removeInactivePairs();
    void limitActivePairs(int maxActive);
};

// Specialized collision systems
class SphereCollision : public BloodCollision {
public:
    SphereCollision();
    bool checkSphereCollision(const BloodParticle& a, const BloodParticle& b) override;
    void resolveSphereCollision(const CollisionPair& pair) override;
};

class BoxCollision : public BloodCollision {
public:
    BoxCollision();
    bool checkBoxCollision(const BloodParticle& a, const BloodParticle& b) override;
    void resolveBoxCollision(const CollisionPair& pair) override;
};

class CapsuleCollision : public BloodCollision {
public:
    CapsuleCollision();
    bool checkCapsuleCollision(const BloodParticle& a, const BloodParticle& b) override;
    void resolveCapsuleCollision(const CollisionPair& pair) override;
};

class MeshCollision : public BloodCollision {
public:
    MeshCollision();
    bool checkMeshCollision(const BloodParticle& a, const BloodParticle& b) override;
    void resolveMeshCollision(const CollisionPair& pair) override;
};

class FluidCollision : public BloodCollision {
public:
    FluidCollision();
    bool checkFluidCollision(const BloodParticle& a, const BloodParticle& b) override;
    void resolveFluidCollision(const CollisionPair& pair) override;
};

// Collision factory
class BloodCollisionFactory {
public:
    static std::unique_ptr<BloodCollision> createCollision(CollisionType type);
    static std::unique_ptr<SphereCollision> createSphereCollision();
    static std::unique_ptr<BoxCollision> createBoxCollision();
    static std::unique_ptr<CapsuleCollision> createCapsuleCollision();
    static std::unique_ptr<MeshCollision> createMeshCollision();
    static std::unique_ptr<FluidCollision> createFluidCollision();
    static std::vector<CollisionType> getAvailableCollisionTypes();
};

} // namespace RFBlood