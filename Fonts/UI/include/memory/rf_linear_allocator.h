#pragma once

#include "rf_allocator.h"
#include <vector>
#include <unordered_map>
#include <mutex>

namespace RFMemory {

// Linear allocator configuration
struct LinearAllocatorConfig {
    size_t capacity;
    bool ownsMemory;
    bool allowGrowth;
    size_t growthIncrement;
    bool useGuardPages;
    bool useCanaryBytes;
    bool zeroOnReset;
    size_t maxAllocations;
    uint32_t defaultAlignment;
    
    LinearAllocatorConfig() : capacity(0), ownsMemory(true), allowGrowth(false), 
                             growthIncrement(0), useGuardPages(false), useCanaryBytes(false), 
                             zeroOnReset(false), maxAllocations(SIZE_MAX), 
                             defaultAlignment(Alignment::DEFAULT_ALIGNMENT) {}
};

// Linear allocation header
struct LinearAllocationHeader {
    size_t size;
    size_t alignment;
    uint32_t flags;
    uint64_t allocationId;
    void* userPointer;
    uint8_t canaryStart[4];
    uint8_t canaryEnd[4];
    
    static constexpr uint32_t CANARY_VALUE = 0xDEADBEEF;
    
    LinearAllocationHeader() : size(0), alignment(Alignment::DEFAULT_ALIGNMENT), 
                              flags(0), allocationId(0), userPointer(nullptr) {
        setCanary();
    }
    
    LinearAllocationHeader(size_t sz, size_t align, uint32_t flg, uint64_t id, void* userPtr = nullptr)
        : size(sz), alignment(align), flags(flg), allocationId(id), userPointer(userPtr) {
        setCanary();
    }
    
    void setCanary() {
        uint32_t canary = CANARY_VALUE;
        std::memcpy(canaryStart, &canary, sizeof(canary));
        std::memcpy(canaryEnd, &canary, sizeof(canary));
    }
    
    bool validateCanary() const {
        uint32_t expectedCanary = CANARY_VALUE;
        return std::memcmp(canaryStart, &expectedCanary, sizeof(expectedCanary)) == 0 &&
               std::memcmp(canaryEnd, &expectedCanary, sizeof(expectedCanary)) == 0;
    }
};

// Linear allocator statistics
struct LinearAllocatorStats {
    size_t totalAllocations;
    size_t currentAllocations;
    size_t peakAllocations;
    size_t totalBytesAllocated;
    size_t currentBytesAllocated;
    size_t peakBytesAllocated;
    size_t allocationOverhead;
    size_t fragmentationLoss;
    size_t totalCapacity;
    size_t usedCapacity;
    size_t freeCapacity;
    size_t growthCount;
    size_t resetCount;
    uint64_t allocationTime;
    uint64_t resetTime;
    double averageAllocationTime;
    double averageResetTime;
    uint32_t allocationFailures;
    uint32_t corruptionDetections;
    
    LinearAllocatorStats() : totalAllocations(0), currentAllocations(0), peakAllocations(0), 
                           totalBytesAllocated(0), currentBytesAllocated(0), peakBytesAllocated(0), 
                           allocationOverhead(0), fragmentationLoss(0), totalCapacity(0), 
                           usedCapacity(0), freeCapacity(0), growthCount(0), resetCount(0), 
                           allocationTime(0), resetTime(0), averageAllocationTime(0.0), 
                           averageResetTime(0.0), allocationFailures(0), corruptionDetections(0) {}
};

// Linear memory allocator
class LinearAllocator : public Allocator {
public:
    // Constructors
    explicit LinearAllocator(size_t capacity);
    LinearAllocator(void* memory, size_t capacity);
    LinearAllocator(const LinearAllocatorConfig& config);
    
    // Destructor
    virtual ~LinearAllocator();
    
    // Core allocation interface
    void* allocate(size_t size, size_t alignment = Alignment::DEFAULT_ALIGNMENT, 
                  uint32_t flags = static_cast<uint32_t>(AllocationFlags::NONE)) override;
    void deallocate(void* pointer) override;
    void* reallocate(void* pointer, size_t newSize, size_t alignment = Alignment::DEFAULT_ALIGNMENT) override;
    
    // Advanced allocation methods
    void* allocateAligned(size_t size, size_t alignment) override;
    void* allocateZeroed(size_t size, size_t alignment = Alignment::DEFAULT_ALIGNMENT) override;
    void* allocateArray(size_t count, size_t elementSize, size_t alignment = Alignment::DEFAULT_ALIGNMENT) override;
    
    // Memory management
    void reset() override;
    void purge() override;
    void trim() override;
    void compact() override;
    
    // Memory information
    size_t getTotalMemory() const override;
    size_t getUsedMemory() const override;
    size_t getFreeMemory() const override;
    size_t getPeakMemory() const override;
    size_t getAllocationCount() const override;
    size_t getFragmentationLoss() const override;
    
    // Statistics and debugging
    AllocatorStats getStatistics() const override;
    AllocationInfo getAllocationInfo() const override;
    std::vector<MemoryBlock> getAllocations() const override;
    bool containsPointer(const void* pointer) const override;
    MemoryBlock getBlockInfo(const void* pointer) const override;
    bool isValidPointer(const void* pointer) const override;
    
    // Validation and debugging
    bool validate() const override;
    bool validatePointer(const void* pointer) const override;
    bool detectLeaks() const override;
    bool detectCorruption() const override;
    void dumpAllocations() const override;
    std::string dumpAllocationsToString() const override;
    void dumpStatistics() const override;
    std::string dumpStatisticsToString() const override;
    
    // Configuration
    void setDebugMode(bool enabled) override;
    bool isDebugMode() const override;
    void setLeakDetection(bool enabled) override;
    bool isLeakDetectionEnabled() const override;
    void setCorruptionDetection(bool enabled) override;
    bool isCorruptionDetectionEnabled() const override;
    
    // Utility methods
    const char* getName() const override;
    const char* getType() const override;
    uint64_t getId() const override;
    bool canAllocate(size_t size, size_t alignment = Alignment::DEFAULT_ALIGNMENT) const override;
    bool canReallocate(void* pointer, size_t newSize) const override;
    
    // Linear allocator specific methods
    void setCapacity(size_t newCapacity);
    size_t getCapacity() const;
    void setGrowthEnabled(bool enabled);
    bool isGrowthEnabled() const;
    void setGrowthIncrement(size_t increment);
    size_t getGrowthIncrement() const;
    void setPosition(size_t position);
    size_t getPosition() const;
    void rewindToPosition(size_t position);
    size_t getAllocationOverhead() const;
    void setZeroOnReset(bool enabled);
    bool isZeroOnResetEnabled() const;
    
    // Event handling
    void setAllocationCallback(AllocationCallback callback) override;
    void setDeallocationCallback(DeallocationCallback callback) override;
    void setReallocationCallback(ReallocationCallback callback) override;
    void clearCallbacks() override;
    
    // Linear allocator specific events
    using GrowthCallback = std::function<void(size_t oldCapacity, size_t newCapacity)>;
    using ResetCallback = std::function<void(size_t position)>;
    
    void setGrowthCallback(GrowthCallback callback);
    void setResetCallback(ResetCallback callback);
    
protected:
    // Protected helper methods
    void onAllocate(const MemoryBlock& block) override;
    void onDeallocate(const MemoryBlock& block) override;
    void onReallocate(const MemoryBlock& oldBlock, const MemoryBlock& newBlock) override;
    
    // Internal allocation helpers
    void* allocateInternal(size_t size, size_t alignment, uint32_t flags);
    void* allocateWithHeader(size_t size, size_t alignment, uint32_t flags);
    void* allocateWithoutHeader(size_t size, size_t alignment, uint32_t flags);
    
    // Memory management helpers
    bool growMemory(size_t requiredSize);
    void initializeMemory();
    void cleanupMemory();
    void writeGuardPages();
    void validateGuardPages() const;
    void writeCanaryBytes(void* pointer, size_t size);
    bool validateCanaryBytes(const void* pointer, size_t size) const;
    
    // Statistics helpers
    void updateAllocationStatistics(size_t size, uint64_t allocationTime);
    void updateDeallocationStatistics(size_t size);
    void updateResetStatistics(uint64_t resetTime);
    void calculateFragmentation();
    
    // Validation helpers
    bool validateMemoryRange(const void* pointer) const;
    bool validateAllocationHeader(const LinearAllocationHeader* header) const;
    bool validateAlignment(const void* pointer, size_t alignment) const override;
    
    // Debug helpers
    void logAllocation(const MemoryBlock& block) override;
    void logDeallocation(const MemoryBlock& block) override;
    void logReset(size_t position);
    void logGrowth(size_t oldCapacity, size_t newCapacity);
    void addAllocationToTracking(const MemoryBlock& block);
    void removeAllocationFromTracking(const void* pointer);
    
    // Thread safety
    mutable std::mutex mutex_;
    
private:
    // Memory management
    void* memory_;
    size_t capacity_;
    size_t position_;
    size_t peakPosition_;
    bool ownsMemory_;
    bool allowGrowth_;
    size_t growthIncrement_;
    
    // Configuration
    bool useGuardPages_;
    bool useCanaryBytes_;
    bool zeroOnReset_;
    size_t maxAllocations_;
    uint32_t defaultAlignment_;
    
    // Statistics
    mutable LinearAllocatorStats linearStats_;
    mutable std::vector<MemoryBlock> allocationTracking_;
    
    // Identification
    static uint64_t nextId_;
    uint64_t id_;
    
    // Callbacks
    GrowthCallback growthCallback_;
    ResetCallback resetCallback_;
    
    // Allocation ID tracking
    uint64_t nextAllocationId_;
    
    // Disabled operations
    LinearAllocator(const LinearAllocator&) = delete;
    LinearAllocator& operator=(const LinearAllocator&) = delete;
};

// Linear allocator factory
class LinearAllocatorFactory {
public:
    // Create linear allocators
    static std::unique_ptr<LinearAllocator> create(size_t capacity);
    static std::unique_ptr<LinearAllocator> create(void* memory, size_t capacity);
    static std::unique_ptr<LinearAllocator> create(const LinearAllocatorConfig& config);
    
    // Create with predefined configurations
    static std::unique_ptr<LinearAllocator> createSmall(size_t baseCapacity = 1024);
    static std::unique_ptr<LinearAllocator> createMedium(size_t baseCapacity = 64 * 1024);
    static std::unique_ptr<LinearAllocator> createLarge(size_t baseCapacity = 1024 * 1024);
    static std::unique_ptr<LinearAllocator> createHuge(size_t baseCapacity = 16 * 1024 * 1024);
    
    // Create with growth enabled
    static std::unique_ptr<LinearAllocator> createWithGrowth(size_t initialCapacity, size_t growthIncrement);
    static std::unique_ptr<LinearAllocator> createWithAutoGrowth(size_t initialCapacity);
    
    // Utility methods
    static size_t calculateOptimalCapacity(size_t estimatedUsage, bool allowGrowth = false);
    static size_t calculateGrowthIncrement(size_t baseCapacity);
    static LinearAllocatorConfig createDefaultConfig(size_t capacity);
    static LinearAllocatorConfig createGrowthConfig(size_t initialCapacity, size_t growthIncrement);
    static LinearAllocatorConfig createDebugConfig(size_t capacity);
};

// Linear allocator utilities
namespace LinearAllocatorUtils {
    // Memory layout analysis
    struct MemoryLayout {
        size_t totalSize;
        size_t usedSize;
        size_t freeSize;
        size_t overheadSize;
        size_t fragmentationSize;
        double utilizationRatio;
        double fragmentationRatio;
        std::vector<size_t> allocationSizes;
        std::vector<size_t> alignmentGaps;
    };
    
    MemoryLayout analyzeMemoryLayout(const LinearAllocator& allocator);
    void dumpMemoryLayout(const LinearAllocator& allocator);
    std::string dumpMemoryLayoutToString(const LinearAllocator& allocator);
    
    // Performance analysis
    struct PerformanceMetrics {
        double allocationSpeed; // allocations per second
        double averageAllocationTime; // microseconds
        double memoryUtilization; // percentage
        size_t fragmentationLoss; // bytes
        double growthEfficiency; // percentage
        uint32_t allocationFailures;
        uint32_t growthOperations;
    };
    
    PerformanceMetrics analyzePerformance(const LinearAllocator& allocator);
    void dumpPerformanceMetrics(const LinearAllocator& allocator);
    std::string dumpPerformanceMetricsToString(const LinearAllocator& allocator);
    
    // Memory optimization
    bool optimizeMemoryLayout(LinearAllocator& allocator);
    bool compactMemory(LinearAllocator& allocator);
    size_t estimateOptimalCapacity(const LinearAllocator& allocator);
    size_t estimateOptimalGrowthIncrement(const LinearAllocator& allocator);
    
    // Debugging utilities
    void validateLinearAllocator(const LinearAllocator& allocator);
    void dumpLinearAllocatorState(const LinearAllocator& allocator);
    std::string dumpLinearAllocatorStateToString(const LinearAllocator& allocator);
    bool detectLinearAllocatorCorruption(const LinearAllocator& allocator);
    void repairLinearAllocatorCorruption(LinearAllocator& allocator);
}

// RAII linear allocator scope
class LinearAllocatorScope {
public:
    explicit LinearAllocatorScope(LinearAllocator* allocator)
        : allocator_(allocator), startPosition_(0) {
        if (allocator_) {
            startPosition_ = allocator_->getPosition();
        }
    }
    
    ~LinearAllocatorScope() {
        if (allocator_) {
            allocator_->rewindToPosition(startPosition_);
        }
    }
    
    LinearAllocator* getAllocator() const { return allocator_; }
    size_t getStartPosition() const { return startPosition_; }
    size_t getCurrentPosition() const { return allocator_ ? allocator_->getPosition() : 0; }
    size_t getUsedBytes() const { return allocator_ ? allocator_->getPosition() - startPosition_ : 0; }
    
    // Reset to start position manually
    void reset() {
        if (allocator_) {
            allocator_->rewindToPosition(startPosition_);
        }
    }
    
    // Move constructor and assignment
    LinearAllocatorScope(LinearAllocatorScope&& other) noexcept
        : allocator_(other.allocator_), startPosition_(other.startPosition_) {
        other.allocator_ = nullptr;
        other.startPosition_ = 0;
    }
    
    LinearAllocatorScope& operator=(LinearAllocatorScope&& other) noexcept {
        if (this != &other) {
            reset(); // Reset current scope before moving
            allocator_ = other.allocator_;
            startPosition_ = other.startPosition_;
            other.allocator_ = nullptr;
            other.startPosition_ = 0;
        }
        return *this;
    }
    
private:
    LinearAllocator* allocator_;
    size_t startPosition_;
    
    // Disabled copy operations
    LinearAllocatorScope(const LinearAllocatorScope&) = delete;
    LinearAllocatorScope& operator=(const LinearAllocatorScope&) = delete;
};

// Linear allocator macros for convenience
#define RF_LINEAR_ALLOCATOR_SCOPE(allocator) \
    LinearAllocatorScope _linear_scope(allocator)

#define RF_LINEAR_ALLOCATOR_SCOPE_NAMED(allocator, name) \
    LinearAllocatorScope name(allocator)

} // namespace RFMemory