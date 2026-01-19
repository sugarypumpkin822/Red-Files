#pragma once

#include "rf_allocator.h"
#include <vector>
#include <queue>
#include <stack>
#include <unordered_set>
#include <functional>
#include <type_traits>
#include <mutex>
#include <atomic>

namespace RFMemory {

// Object pool configuration
struct ObjectPoolConfig {
    size_t objectSize;
    size_t initialCapacity;
    size_t maxCapacity;
    size_t growthIncrement;
    bool allowGrowth;
    bool constructOnAllocate;
    bool destructOnDeallocate;
    bool trackObjects;
    bool validateObjects;
    size_t alignment;
    uint32_t flags;
    
    ObjectPoolConfig() : objectSize(0), initialCapacity(0), maxCapacity(SIZE_MAX), 
                        growthIncrement(0), allowGrowth(false), constructOnAllocate(true), 
                        destructOnDeallocate(true), trackObjects(false), validateObjects(false), 
                        alignment(Alignment::DEFAULT_ALIGNMENT), flags(0) {}
};

// Object pool statistics
struct ObjectPoolStats {
    size_t totalObjects;
    size_t usedObjects;
    size_t freeObjects;
    size_t peakObjects;
    size_t totalMemory;
    size_t usedMemory;
    size_t freeMemory;
    size_t peakMemory;
    size_t allocationOverhead;
    uint64_t allocationCount;
    uint64_t deallocationCount;
    uint64_t constructionCount;
    uint64_t destructionCount;
    uint64_t growthCount;
    uint64_t shrinkCount;
    uint64_t allocationTime;
    uint64_t deallocationTime;
    uint64_t constructionTime;
    uint64_t destructionTime;
    double averageAllocationTime;
    double averageDeallocationTime;
    double averageConstructionTime;
    double averageDestructionTime;
    uint32_t allocationFailures;
    uint32_t deallocationFailures;
    uint32_t constructionFailures;
    uint32_t destructionFailures;
    double utilizationRatio;
    
    ObjectPoolStats() : totalObjects(0), usedObjects(0), freeObjects(0), peakObjects(0), 
                       totalMemory(0), usedMemory(0), freeMemory(0), peakMemory(0), 
                       allocationOverhead(0), allocationCount(0), deallocationCount(0), 
                       constructionCount(0), destructionCount(0), growthCount(0), shrinkCount(0), 
                       allocationTime(0), deallocationTime(0), constructionTime(0), destructionTime(0), 
                       averageAllocationTime(0.0), averageDeallocationTime(0.0), 
                       averageConstructionTime(0.0), averageDestructionTime(0.0), 
                       allocationFailures(0), deallocationFailures(0), constructionFailures(0), 
                       destructionFailures(0), utilizationRatio(0.0) {}
};

// Object pool entry
struct ObjectPoolEntry {
    void* object;
    bool isInUse;
    uint64_t objectId;
    uint64_t allocationId;
    uint64_t timestamp;
    const char* file;
    int line;
    const char* function;
    
    ObjectPoolEntry() : object(nullptr), isInUse(false), objectId(0), allocationId(0), 
                       timestamp(0), file(nullptr), line(0), function(nullptr) {}
    
    ObjectPoolEntry(void* obj, uint64_t id, uint64_t allocId, uint64_t ts, 
                   const char* f = nullptr, int l = 0, const char* fn = nullptr)
        : object(obj), isInUse(true), objectId(id), allocationId(allocId), 
          timestamp(ts), file(f), line(l), function(fn) {}
};

// Base object pool interface
class ObjectPoolBase {
public:
    ObjectPoolBase() = default;
    virtual ~ObjectPoolBase() = default;
    
    // Core operations
    virtual void* allocate() = 0;
    virtual void deallocate(void* object) = 0;
    virtual bool contains(const void* object) const = 0;
    virtual bool isValid(const void* object) const = 0;
    
    // Pool management
    virtual void reset() = 0;
    virtual void purge() = 0;
    virtual void trim() = 0;
    virtual void resize(size_t newCapacity) = 0;
    
    // Information
    virtual size_t getCapacity() const = 0;
    virtual size_t getUsedCount() const = 0;
    virtual size_t getFreeCount() const = 0;
    virtual size_t getObjectSize() const = 0;
    virtual size_t getTotalMemory() const = 0;
    virtual size_t getUsedMemory() const = 0;
    virtual size_t getFreeMemory() const = 0;
    
    // Configuration
    virtual void setMaxCapacity(size_t maxCapacity) = 0;
    virtual size_t getMaxCapacity() const = 0;
    virtual void setGrowthEnabled(bool enabled) = 0;
    virtual bool isGrowthEnabled() const = 0;
    virtual void setGrowthIncrement(size_t increment) = 0;
    virtual size_t getGrowthIncrement() const = 0;
    
    // Statistics
    virtual ObjectPoolStats getStatistics() const = 0;
    
    // Validation
    virtual bool validate() const = 0;
    virtual void dumpStatistics() const = 0;
    virtual std::string dumpStatisticsToString() const = 0;
    
protected:
    static std::atomic<uint64_t> nextObjectId_;
    static std::atomic<uint64_t> nextAllocationId_;
};

// Template object pool
template<typename T>
class ObjectPool : public ObjectPoolBase {
public:
    using ConstructorFunction = std::function<T*()>;
    using DestructorFunction = std::function<void(T*)>;
    using ValidatorFunction = std::function<bool(const T*)>;
    
    // Constructors
    explicit ObjectPool(size_t initialCapacity = 0);
    ObjectPool(const ObjectPoolConfig& config);
    ObjectPool(size_t initialCapacity, ConstructorFunction constructor, DestructorFunction destructor = nullptr);
    
    // Destructor
    virtual ~ObjectPool();
    
    // Core allocation interface
    T* allocate();
    void deallocate(T* object);
    void* allocate() override;
    void deallocate(void* object) override;
    
    // Advanced allocation methods
    template<typename... Args>
    T* allocateWithArgs(Args&&... args);
    T* allocateAndConstruct();
    template<typename... Args>
    T* allocateAndConstructWithArgs(Args&&... args);
    
    // Pool management
    void reset() override;
    void purge() override;
    void trim() override;
    void resize(size_t newCapacity) override;
    void grow(size_t additionalObjects = 0);
    void shrink(size_t objectsToRemove = 0);
    
    // Object management
    bool contains(const void* object) const override;
    bool contains(const T* object) const;
    bool isValid(const void* object) const override;
    bool isValid(const T* object) const;
    size_t getObjectIndex(const T* object) const;
    T* getObjectByIndex(size_t index);
    const T* getObjectByIndex(size_t index) const;
    
    // Information
    size_t getCapacity() const override;
    size_t getUsedCount() const override;
    size_t getFreeCount() const override;
    size_t getObjectSize() const override;
    size_t getTotalMemory() const override;
    size_t getUsedMemory() const override;
    size_t getFreeMemory() const override;
    double getUtilizationRatio() const;
    
    // Configuration
    void setMaxCapacity(size_t maxCapacity) override;
    size_t getMaxCapacity() const override;
    void setGrowthEnabled(bool enabled) override;
    bool isGrowthEnabled() const override;
    void setGrowthIncrement(size_t increment) override;
    size_t getGrowthIncrement() const override;
    void setConstructOnAllocate(bool enabled);
    bool isConstructOnAllocate() const;
    void setDestructOnDeallocate(bool enabled);
    bool isDestructOnDeallocate() const;
    void setTrackObjects(bool enabled);
    bool isTrackObjectsEnabled() const;
    void setValidateObjects(bool enabled);
    bool isValidateObjectsEnabled() const;
    
    // Custom functions
    void setConstructor(ConstructorFunction constructor);
    void setDestructor(DestructorFunction destructor);
    void setValidator(ValidatorFunction validator);
    ConstructorFunction getConstructor() const;
    DestructorFunction getDestructor() const;
    ValidatorFunction getValidator() const;
    
    // Statistics
    ObjectPoolStats getStatistics() const override;
    std::vector<ObjectPoolEntry> getUsedObjects() const;
    std::vector<ObjectPoolEntry> getFreeObjects() const;
    std::vector<ObjectPoolEntry> getAllObjects() const;
    
    // Validation and debugging
    bool validate() const override;
    bool validateObjects() const;
    bool validateObject(const T* object) const;
    void dumpStatistics() const override;
    std::string dumpStatisticsToString() const override;
    void dumpObjects() const;
    std::string dumpObjectsToString() const;
    void dumpUsedObjects() const;
    std::string dumpUsedObjectsToString() const;
    void dumpFreeObjects() const;
    std::string dumpFreeObjectsToString() const;
    
    // Event handling
    using AllocationCallback = std::function<void(T* object, uint64_t objectId)>;
    using DeallocationCallback = std::function<void(T* object, uint64_t objectId)>;
    using ConstructionCallback = std::function<void(T* object, uint64_t objectId)>;
    using DestructionCallback = std::function<void(T* object, uint64_t objectId)>;
    using GrowthCallback = std::function<void(size_t oldCapacity, size_t newCapacity)>;
    using ShrinkCallback = std::function<void(size_t oldCapacity, size_t newCapacity)>;
    
    void setAllocationCallback(AllocationCallback callback);
    void setDeallocationCallback(DeallocationCallback callback);
    void setConstructionCallback(ConstructionCallback callback);
    void setDestructionCallback(DestructionCallback callback);
    void setGrowthCallback(GrowthCallback callback);
    void setShrinkCallback(ShrinkCallback callback);
    void clearCallbacks();
    
protected:
    // Internal helpers
    void* allocateInternal();
    void deallocateInternal(void* object);
    T* constructObject(void* memory);
    void destructObject(T* object);
    bool validateObjectInternal(const T* object) const;
    
    // Pool management
    void initializePool();
    void cleanupPool();
    void growPool(size_t additionalObjects);
    void shrinkPool(size_t objectsToRemove);
    void addObjectsToPool(size_t count);
    void removeObjectsFromPool(size_t count);
    
    // Object tracking
    void addObjectToTracking(T* object, uint64_t objectId, uint64_t allocationId, 
                           const char* file = nullptr, int line = 0, const char* function = nullptr);
    void removeObjectFromTracking(T* object);
    void updateObjectTracking(T* object, bool inUse);
    
    // Statistics
    void updateAllocationStatistics(uint64_t allocationTime);
    void updateDeallocationStatistics(uint64_t deallocationTime);
    void updateConstructionStatistics(uint64_t constructionTime);
    void updateDestructionStatistics(uint64_t destructionTime);
    void updateGrowthStatistics(size_t oldCapacity, size_t newCapacity);
    void updateShrinkStatistics(size_t oldCapacity, size_t newCapacity);
    void calculateUtilizationRatio();
    
    // Validation
    bool validatePoolIntegrity() const;
    bool validateObjectStorage() const;
    bool validateFreeList() const;
    
    // Debug helpers
    void logAllocation(T* object, uint64_t objectId);
    void logDeallocation(T* object, uint64_t objectId);
    void logConstruction(T* object, uint64_t objectId);
    void logDestruction(T* object, uint64_t objectId);
    void logGrowth(size_t oldCapacity, size_t newCapacity);
    void logShrink(size_t oldCapacity, size_t newCapacity);
    
    // Thread safety
    mutable std::mutex mutex_;
    
private:
    // Pool configuration
    size_t objectSize_;
    size_t capacity_;
    size_t maxCapacity_;
    size_t growthIncrement_;
    bool allowGrowth_;
    bool constructOnAllocate_;
    bool destructOnDeallocate_;
    bool trackObjects_;
    bool validateObjects_;
    size_t alignment_;
    
    // Object storage
    std::vector<uint8_t*> objectMemory_;
    std::queue<T*> freeObjects_;
    std::vector<ObjectPoolEntry> objectEntries_;
    std::unordered_set<T*> usedObjects_;
    
    // Statistics
    mutable ObjectPoolStats stats_;
    
    // Custom functions
    ConstructorFunction constructor_;
    DestructorFunction destructor_;
    ValidatorFunction validator_;
    
    // Callbacks
    AllocationCallback allocationCallback_;
    DeallocationCallback deallocationCallback_;
    ConstructionCallback constructionCallback_;
    DestructionCallback destructionCallback_;
    GrowthCallback growthCallback_;
    ShrinkCallback shrinkCallback_;
    
    // Identification
    uint64_t poolId_;
    
    // Disabled operations
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
};

// Template method implementations
template<typename T>
ObjectPool<T>::ObjectPool(size_t initialCapacity)
    : objectSize_(sizeof(T)), capacity_(initialCapacity), maxCapacity_(SIZE_MAX), 
      growthIncrement_(initialCapacity), allowGrowth_(true), constructOnAllocate_(true), 
      destructOnDeallocate_(true), trackObjects_(false), validateObjects_(false), 
      alignment_(alignof(T)), poolId_(nextObjectId_.fetch_add(1)) {
    initializePool();
}

template<typename T>
ObjectPool<T>::ObjectPool(const ObjectPoolConfig& config)
    : objectSize_(config.objectSize), capacity_(config.initialCapacity), 
      maxCapacity_(config.maxCapacity), growthIncrement_(config.growthIncrement), 
      allowGrowth_(config.allowGrowth), constructOnAllocate_(config.constructOnAllocate), 
      destructOnDeallocate_(config.destructOnAllocate), trackObjects_(config.trackObjects), 
      validateObjects_(config.validateObjects), alignment_(config.alignment), 
      poolId_(nextObjectId_.fetch_add(1)) {
    if (objectSize_ == 0) {
        objectSize_ = sizeof(T);
    }
    if (alignment_ == 0) {
        alignment_ = alignof(T);
    }
    initializePool();
}

template<typename T>
ObjectPool<T>::ObjectPool(size_t initialCapacity, ConstructorFunction constructor, DestructorFunction destructor)
    : objectSize_(sizeof(T)), capacity_(initialCapacity), maxCapacity_(SIZE_MAX), 
      growthIncrement_(initialCapacity), allowGrowth_(true), constructOnAllocate_(false), 
      destructOnDeallocate_(false), trackObjects_(false), validateObjects_(false), 
      alignment_(alignof(T)), constructor_(constructor), destructor_(destructor), 
      poolId_(nextObjectId_.fetch_add(1)) {
    initializePool();
}

template<typename T>
ObjectPool<T>::~ObjectPool() {
    cleanupPool();
}

template<typename T>
T* ObjectPool<T>::allocate() {
    std::lock_guard<std::mutex> lock(mutex_);
    return static_cast<T*>(allocateInternal());
}

template<typename T>
void ObjectPool<T>::deallocate(T* object) {
    std::lock_guard<std::mutex> lock(mutex_);
    deallocateInternal(object);
}

template<typename T>
void* ObjectPool<T>::allocate() {
    return allocate();
}

template<typename T>
void ObjectPool<T>::deallocate(void* object) {
    deallocate(static_cast<T*>(object));
}

template<typename T>
template<typename... Args>
T* ObjectPool<T>::allocateWithArgs(Args&&... args) {
    std::lock_guard<std::mutex> lock(mutex_);
    void* memory = allocateInternal();
    if (memory) {
        T* object = new(memory) T(std::forward<Args>(args)...);
        if (constructionCallback_) {
            constructionCallback_(object, nextAllocationId_.fetch_add(1));
        }
        return object;
    }
    return nullptr;
}

template<typename T>
T* ObjectPool<T>::allocateAndConstruct() {
    std::lock_guard<std::mutex> lock(mutex_);
    void* memory = allocateInternal();
    if (memory) {
        T* object = constructObject(memory);
        return object;
    }
    return nullptr;
}

template<typename T>
template<typename... Args>
T* ObjectPool<T>::allocateAndConstructWithArgs(Args&&... args) {
    return allocateWithArgs(std::forward<Args>(args)...);
}

template<typename T>
void ObjectPool<T>::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Destroy all used objects if destructOnDeallocate is enabled
    if (destructOnDeallocate_) {
        for (auto& entry : objectEntries_) {
            if (entry.isInUse && entry.object) {
                destructObject(static_cast<T*>(entry.object));
            }
        }
    }
    
    // Clear all tracking
    usedObjects_.clear();
    while (!freeObjects_.empty()) {
        freeObjects_.pop();
    }
    
    // Reset all entries
    for (auto& entry : objectEntries_) {
        entry.isInUse = false;
        entry.object = nullptr;
    }
    
    // Add all objects to free list
    for (size_t i = 0; i < objectMemory_.size(); ++i) {
        freeObjects_.push(static_cast<T*>(objectMemory_[i]));
        objectEntries_[i].object = objectMemory_[i];
        objectEntries_[i].isInUse = false;
    }
    
    // Reset statistics
    stats_.usedObjects = 0;
    stats_.freeObjects = capacity_;
}

template<typename T>
void ObjectPool<T>::purge() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Destroy all objects
    for (auto& entry : objectEntries_) {
        if (entry.isInUse && entry.object && destructOnDeallocate_) {
            destructObject(static_cast<T*>(entry.object));
        }
    }
    
    // Clear all memory
    for (auto* memory : objectMemory_) {
        if (memory) {
            ::operator delete(memory);
        }
    }
    objectMemory_.clear();
    
    // Clear all tracking
    usedObjects_.clear();
    while (!freeObjects_.empty()) {
        freeObjects_.pop();
    }
    objectEntries_.clear();
    
    // Reset capacity and statistics
    capacity_ = 0;
    stats_.totalObjects = 0;
    stats_.usedObjects = 0;
    stats_.freeObjects = 0;
}

template<typename T>
void ObjectPool<T>::trim() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Calculate how many objects can be removed
    size_t removableObjects = freeObjects_.size();
    if (removableObjects == 0) {
        return;
    }
    
    // Don't remove more than growth increment allows
    if (growthIncrement_ > 0 && removableObjects > growthIncrement_) {
        removableObjects = growthIncrement_;
    }
    
    shrinkPool(removableObjects);
}

template<typename T>
void ObjectPool<T>::resize(size_t newCapacity) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (newCapacity == capacity_) {
        return;
    }
    
    if (newCapacity < capacity_) {
        // Shrink if possible
        size_t removableObjects = capacity_ - newCapacity;
        size_t freeCount = freeObjects_.size();
        if (removableObjects <= freeCount) {
            shrinkPool(removableObjects);
        }
    } else if (newCapacity > capacity_) {
        // Grow
        growPool(newCapacity - capacity_);
    }
}

template<typename T>
bool ObjectPool<T>::contains(const void* object) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return usedObjects_.find(const_cast<T*>(static_cast<const T*>(object))) != usedObjects_.end();
}

template<typename T>
bool ObjectPool<T>::contains(const T* object) const {
    return contains(static_cast<const void*>(object));
}

template<typename T>
bool ObjectPool<T>::isValid(const void* object) const {
    return contains(object) && validateObjectInternal(static_cast<const T*>(object));
}

template<typename T>
bool ObjectPool<T>::isValid(const T* object) const {
    return isValid(static_cast<const void*>(object));
}

template<typename T>
size_t ObjectPool<T>::getCapacity() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return capacity_;
}

template<typename T>
size_t ObjectPool<T>::getUsedCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return usedObjects_.size();
}

template<typename T>
size_t ObjectPool<T>::getFreeCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return freeObjects_.size();
}

template<typename T>
size_t ObjectPool<T>::getObjectSize() const {
    return objectSize_;
}

template<typename T>
size_t ObjectPool<T>::getTotalMemory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return capacity_ * (objectSize_ + alignment_ - 1);
}

template<typename T>
size_t ObjectPool<T>::getUsedMemory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return usedObjects_.size() * (objectSize_ + alignment_ - 1);
}

template<typename T>
size_t ObjectPool<T>::getFreeMemory() const {
    return getTotalMemory() - getUsedMemory();
}

template<typename T>
double ObjectPool<T>::getUtilizationRatio() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (capacity_ == 0) return 0.0;
    return static_cast<double>(usedObjects_.size()) / capacity_;
}

// Object pool factory
class ObjectPoolFactory {
public:
    // Create object pools
    template<typename T>
    static std::unique_ptr<ObjectPool<T>> create(size_t initialCapacity = 0);
    
    template<typename T>
    static std::unique_ptr<ObjectPool<T>> create(const ObjectPoolConfig& config);
    
    template<typename T>
    static std::unique_ptr<ObjectPool<T>> create(size_t initialCapacity, 
                                                typename ObjectPool<T>::ConstructorFunction constructor,
                                                typename ObjectPool<T>::DestructorFunction destructor = nullptr);
    
    // Create with predefined configurations
    template<typename T>
    static std::unique_ptr<ObjectPool<T>> createSmall(size_t baseCapacity = 16);
    
    template<typename T>
    static std::unique_ptr<ObjectPool<T>> createMedium(size_t baseCapacity = 64);
    
    template<typename T>
    static std::unique_ptr<ObjectPool<T>> createLarge(size_t baseCapacity = 256);
    
    template<typename T>
    static std::unique_ptr<ObjectPool<T>> createHuge(size_t baseCapacity = 1024);
    
    // Create with growth enabled
    template<typename T>
    static std::unique_ptr<ObjectPool<T>> createWithGrowth(size_t initialCapacity, size_t growthIncrement = 0);
    
    template<typename T>
    static std::unique_ptr<ObjectPool<T>> createWithAutoGrowth(size_t initialCapacity);
    
    // Create with debugging
    template<typename T>
    static std::unique_ptr<ObjectPool<T>> createWithDebugging(size_t initialCapacity);
    
    template<typename T>
    static std::unique_ptr<ObjectPool<T>> createWithTracking(size_t initialCapacity);
    
    // Utility methods
    template<typename T>
    static size_t calculateOptimalCapacity(size_t estimatedUsage);
    
    template<typename T>
    static size_t calculateGrowthIncrement(size_t baseCapacity);
    
    static ObjectPoolConfig createDefaultConfig(size_t objectSize, size_t initialCapacity);
    static ObjectPoolConfig createGrowthConfig(size_t objectSize, size_t initialCapacity, size_t growthIncrement);
    static ObjectPoolConfig createDebugConfig(size_t objectSize, size_t initialCapacity);
    static ObjectPoolConfig createTrackingConfig(size_t objectSize, size_t initialCapacity);
};

// RAII object pool allocation
template<typename T>
class PoolObject {
public:
    explicit PoolObject(ObjectPool<T>* pool)
        : pool_(pool), object_(nullptr) {
        if (pool_) {
            object_ = pool_->allocate();
        }
    }
    
    ~PoolObject() {
        if (pool_ && object_) {
            pool_->deallocate(object_);
        }
    }
    
    T* get() const { return object_; }
    T& operator*() const { return *object_; }
    T* operator->() const { return object_; }
    bool isValid() const { return object_ != nullptr; }
    
    // Reset to get a new object
    void reset() {
        if (pool_ && object_) {
            pool_->deallocate(object_);
            object_ = pool_->allocate();
        }
    }
    
    // Release ownership
    T* release() {
        T* obj = object_;
        object_ = nullptr;
        return obj;
    }
    
    // Move constructor and assignment
    PoolObject(PoolObject&& other) noexcept
        : pool_(other.pool_), object_(other.object_) {
        other.pool_ = nullptr;
        other.object_ = nullptr;
    }
    
    PoolObject& operator=(PoolObject&& other) noexcept {
        if (this != &other) {
            if (pool_ && object_) {
                pool_->deallocate(object_);
            }
            pool_ = other.pool_;
            object_ = other.object_;
            other.pool_ = nullptr;
            other.object_ = nullptr;
        }
        return *this;
    }
    
private:
    ObjectPool<T>* pool_;
    T* object_;
    
    // Disabled copy operations
    PoolObject(const PoolObject&) = delete;
    PoolObject& operator=(const PoolObject&) = delete;
};

// Object pool utilities
namespace ObjectPoolUtils {
    // Pool analysis
    template<typename T>
    struct PoolAnalysis {
        size_t totalObjects;
        size_t usedObjects;
        size_t freeObjects;
        size_t totalMemory;
        size_t usedMemory;
        size_t freeMemory;
        size_t overheadMemory;
        double utilizationRatio;
        std::vector<uint64_t> objectIds;
        std::vector<uint64_t> allocationIds;
    };
    
    template<typename T>
    PoolAnalysis<T> analyzePool(const ObjectPool<T>& pool);
    
    template<typename T>
    void dumpPoolAnalysis(const ObjectPool<T>& pool);
    
    template<typename T>
    std::string dumpPoolAnalysisToString(const ObjectPool<T>& pool);
    
    // Performance analysis
    template<typename T>
    struct PoolPerformanceMetrics {
        double allocationSpeed; // allocations per second
        double deallocationSpeed; // deallocations per second
        double averageAllocationTime; // microseconds
        double averageDeallocationTime; // microseconds
        double memoryUtilization; // percentage
        uint32_t allocationFailures;
        uint32_t deallocationFailures;
        uint32_t growthOperations;
        uint32_t shrinkOperations;
    };
    
    template<typename T>
    PoolPerformanceMetrics<T> analyzePerformance(const ObjectPool<T>& pool);
    
    template<typename T>
    void dumpPerformanceMetrics(const ObjectPool<T>& pool);
    
    template<typename T>
    std::string dumpPerformanceMetricsToString(const ObjectPool<T>& pool);
    
    // Memory optimization
    template<typename T>
    bool optimizePoolLayout(ObjectPool<T>& pool);
    
    template<typename T>
    bool compactPoolMemory(ObjectPool<T>& pool);
    
    template<typename T>
    size_t estimateOptimalCapacity(const ObjectPool<T>& pool);
    
    template<typename T>
    size_t estimateOptimalGrowthIncrement(const ObjectPool<T>& pool);
    
    // Debugging utilities
    template<typename T>
    void validateObjectPool(const ObjectPool<T>& pool);
    
    template<typename T>
    void dumpObjectPoolState(const ObjectPool<T>& pool);
    
    template<typename T>
    std::string dumpObjectPoolStateToString(const ObjectPool<T>& pool);
    
    template<typename T>
    bool detectObjectPoolCorruption(const ObjectPool<T>& pool);
    
    template<typename T>
    void repairObjectPoolCorruption(ObjectPool<T>& pool);
}

// Object pool macros for convenience
#define RF_OBJECT_POOL_ALLOCATE(pool, type) \
    PoolObject<type> _pool_obj(pool)

#define RF_OBJECT_POOL_ALLOCATE_NAMED(pool, type, name) \
    PoolObject<type> name(pool)

} // namespace RFMemory

// Include template implementations
#include "rf_object_pool_impl.h"