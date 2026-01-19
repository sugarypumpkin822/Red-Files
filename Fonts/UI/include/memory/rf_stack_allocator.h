#pragma once

#include "rf_allocator.h"
#include <vector>
#include <stack>
#include <unordered_map>
#include <mutex>

namespace RFMemory {

// Stack allocator configuration
struct StackAllocatorConfig {
    size_t capacity;
    bool ownsMemory;
    bool allowGrowth;
    size_t growthIncrement;
    bool useGuardPages;
    bool useCanaryBytes;
    bool validateOnDeallocate;
    bool trackStackFrames;
    size_t maxStackDepth;
    uint32_t defaultAlignment;
    
    StackAllocatorConfig() : capacity(0), ownsMemory(true), allowGrowth(false), 
                             growthIncrement(0), useGuardPages(false), useCanaryBytes(false), 
                             validateOnDeallocate(true), trackStackFrames(false), 
                             maxStackDepth(SIZE_MAX), defaultAlignment(Alignment::DEFAULT_ALIGNMENT) {}
};

// Stack allocation header
struct StackAllocationHeader {
    size_t size;
    size_t alignment;
    uint32_t flags;
    uint64_t allocationId;
    size_t previousPosition;
    void* userPointer;
    uint8_t canaryStart[4];
    uint8_t canaryEnd[4];
    
    static constexpr uint32_t CANARY_VALUE = 0xBADC0FFE;
    
    StackAllocationHeader() : size(0), alignment(Alignment::DEFAULT_ALIGNMENT), 
                              flags(0), allocationId(0), previousPosition(0), userPointer(nullptr) {
        setCanary();
    }
    
    StackAllocationHeader(size_t sz, size_t align, uint32_t flg, uint64_t id, size_t prevPos, void* userPtr = nullptr)
        : size(sz), alignment(align), flags(flg), allocationId(id), previousPosition(prevPos), userPointer(userPtr) {
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

// Stack frame information
struct StackFrame {
    size_t position;
    size_t allocationCount;
    size_t totalSize;
    uint64_t timestamp;
    const char* file;
    int line;
    const char* function;
    std::vector<MemoryBlock> allocations;
    
    StackFrame() : position(0), allocationCount(0), totalSize(0), timestamp(0), 
                   file(nullptr), line(0), function(nullptr) {}
    
    StackFrame(size_t pos, uint64_t ts, const char* f = nullptr, int l = 0, const char* fn = nullptr)
        : position(pos), allocationCount(0), totalSize(0), timestamp(ts), file(f), line(l), function(fn) {}
};

// Stack allocator statistics
struct StackAllocatorStats {
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
    size_t maxStackDepth;
    size_t currentStackDepth;
    uint64_t allocationTime;
    uint64_t deallocationTime;
    uint64_t resetTime;
    double averageAllocationTime;
    double averageDeallocationTime;
    double averageResetTime;
    uint32_t allocationFailures;
    uint32_t deallocationFailures;
    uint32_t corruptionDetections;
    uint32_t stackOverflowDetections;
    uint32_t stackUnderflowDetections;
    
    StackAllocatorStats() : totalAllocations(0), currentAllocations(0), peakAllocations(0), 
                           totalBytesAllocated(0), currentBytesAllocated(0), peakBytesAllocated(0), 
                           allocationOverhead(0), fragmentationLoss(0), totalCapacity(0), 
                           usedCapacity(0), freeCapacity(0), growthCount(0), resetCount(0), 
                           maxStackDepth(0), currentStackDepth(0), allocationTime(0), deallocationTime(0), 
                           resetTime(0), averageAllocationTime(0.0), averageDeallocationTime(0.0), 
                           averageResetTime(0.0), allocationFailures(0), deallocationFailures(0), 
                           corruptionDetections(0), stackOverflowDetections(0), stackUnderflowDetections(0) {}
};

// Stack memory allocator
class StackAllocator : public Allocator {
public:
    // Constructors
    explicit StackAllocator(size_t capacity);
    StackAllocator(void* memory, size_t capacity);
    StackAllocator(const StackAllocatorConfig& config);
    
    // Destructor
    virtual ~StackAllocator();
    
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
    
    // Stack-specific operations
    void push();
    void pop();
    void rewindToPosition(size_t position);
    size_t getCurrentPosition() const;
    size_t getStackSize() const;
    size_t getStackDepth() const;
    void setMaxStackDepth(size_t maxDepth);
    size_t getMaxStackDepth() const;
    
    // Stack frame management
    void pushFrame(const char* file = nullptr, int line = 0, const char* function = nullptr);
    void popFrame();
    size_t getCurrentFrameIndex() const;
    const StackFrame& getCurrentFrame() const;
    const StackFrame& getFrame(size_t index) const;
    size_t getFrameCount() const;
    std::vector<StackFrame> getAllFrames() const;
    
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
    
    // Stack-specific debugging
    void dumpStackFrames() const;
    std::string dumpStackFramesToString() const;
    void dumpCurrentFrame() const;
    std::string dumpCurrentFrameToString() const;
    bool validateStackIntegrity() const;
    bool detectStackOverflow() const;
    bool detectStackUnderflow() const;
    
    // Configuration
    void setDebugMode(bool enabled) override;
    bool isDebugMode() const override;
    void setLeakDetection(bool enabled) override;
    bool isLeakDetectionEnabled() const override;
    void setCorruptionDetection(bool enabled) override;
    bool isCorruptionDetectionEnabled() const override;
    
    // Stack-specific configuration
    void setValidateOnDeallocate(bool enabled);
    bool isValidateOnDeallocateEnabled() const;
    void setTrackStackFrames(bool enabled);
    bool isTrackStackFramesEnabled() const;
    
    // Utility methods
    const char* getName() const override;
    const char* getType() const override;
    uint64_t getId() const override;
    bool canAllocate(size_t size, size_t alignment = Alignment::DEFAULT_ALIGNMENT) const override;
    bool canReallocate(void* pointer, size_t newSize) const override;
    
    // Stack allocator specific methods
    void setCapacity(size_t newCapacity);
    size_t getCapacity() const;
    void setGrowthEnabled(bool enabled);
    bool isGrowthEnabled() const;
    void setGrowthIncrement(size_t increment);
    size_t getGrowthIncrement() const;
    size_t getAllocationOverhead() const;
    
    // Event handling
    void setAllocationCallback(AllocationCallback callback) override;
    void setDeallocationCallback(DeallocationCallback callback) override;
    void setReallocationCallback(ReallocationCallback callback) override;
    void clearCallbacks() override;
    
    // Stack allocator specific events
    using GrowthCallback = std::function<void(size_t oldCapacity, size_t newCapacity)>;
    using ResetCallback = std::function<void(size_t position)>;
    using StackOverflowCallback = std::function<void(size_t position, size_t capacity)>;
    using StackUnderflowCallback = std::function<void(size_t position, size_t previousPosition)>;
    using FramePushCallback = std::function<void(size_t frameIndex)>;
    using FramePopCallback = std::function<void(size_t frameIndex)>;
    
    void setGrowthCallback(GrowthCallback callback);
    void setResetCallback(ResetCallback callback);
    void setStackOverflowCallback(StackOverflowCallback callback);
    void setStackUnderflowCallback(StackUnderflowCallback callback);
    void setFramePushCallback(FramePushCallback callback);
    void setFramePopCallback(FramePopCallback callback);
    
protected:
    // Protected helper methods
    void onAllocate(const MemoryBlock& block) override;
    void onDeallocate(const MemoryBlock& block) override;
    void onReallocate(const MemoryBlock& oldBlock, const MemoryBlock& newBlock) override;
    
    // Internal allocation helpers
    void* allocateInternal(size_t size, size_t alignment, uint32_t flags);
    void* allocateWithHeader(size_t size, size_t alignment, uint32_t flags);
    void* allocateWithoutHeader(size_t size, size_t alignment, uint32_t flags);
    
    // Stack management helpers
    void pushFrameInternal(const char* file, int line, const char* function);
    void popFrameInternal();
    void validateStackOperation(size_t newPosition) const;
    void handleStackOverflow(size_t requiredSize);
    void handleStackUnderflow(size_t targetPosition);
    
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
    void updateStackDepthStatistics();
    void calculateFragmentation();
    
    // Validation helpers
    bool validateMemoryRange(const void* pointer) const;
    bool validateAllocationHeader(const StackAllocationHeader* header) const;
    bool validateAlignment(const void* pointer, size_t alignment) const override;
    bool validateStackFrames() const;
    bool validateFrameIntegrity(const StackFrame& frame) const;
    
    // Debug helpers
    void logAllocation(const MemoryBlock& block) override;
    void logDeallocation(const MemoryBlock& block) override;
    void logReset(size_t position);
    void logGrowth(size_t oldCapacity, size_t newCapacity);
    void logStackOverflow(size_t position, size_t capacity);
    void logStackUnderflow(size_t position, size_t previousPosition);
    void logFramePush(size_t frameIndex);
    void logFramePop(size_t frameIndex);
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
    
    // Stack management
    std::vector<StackFrame> stackFrames_;
    size_t currentFrameIndex_;
    size_t maxStackDepth_;
    
    // Configuration
    bool useGuardPages_;
    bool useCanaryBytes_;
    bool validateOnDeallocate_;
    bool trackStackFrames_;
    uint32_t defaultAlignment_;
    
    // Statistics
    mutable StackAllocatorStats stackStats_;
    mutable std::vector<MemoryBlock> allocationTracking_;
    
    // Identification
    static uint64_t nextId_;
    uint64_t id_;
    
    // Callbacks
    GrowthCallback growthCallback_;
    ResetCallback resetCallback_;
    StackOverflowCallback stackOverflowCallback_;
    StackUnderflowCallback stackUnderflowCallback_;
    FramePushCallback framePushCallback_;
    FramePopCallback framePopCallback_;
    
    // Allocation ID tracking
    uint64_t nextAllocationId_;
    
    // Disabled operations
    StackAllocator(const StackAllocator&) = delete;
    StackAllocator& operator=(const StackAllocator&) = delete;
};

// Stack allocator factory
class StackAllocatorFactory {
public:
    // Create stack allocators
    static std::unique_ptr<StackAllocator> create(size_t capacity);
    static std::unique_ptr<StackAllocator> create(void* memory, size_t capacity);
    static std::unique_ptr<StackAllocator> create(const StackAllocatorConfig& config);
    
    // Create with predefined configurations
    static std::unique_ptr<StackAllocator> createSmall(size_t baseCapacity = 1024);
    static std::unique_ptr<StackAllocator> createMedium(size_t baseCapacity = 64 * 1024);
    static std::unique_ptr<StackAllocator> createLarge(size_t baseCapacity = 1024 * 1024);
    static std::unique_ptr<StackAllocator> createHuge(size_t baseCapacity = 16 * 1024 * 1024);
    
    // Create with growth enabled
    static std::unique_ptr<StackAllocator> createWithGrowth(size_t initialCapacity, size_t growthIncrement);
    static std::unique_ptr<StackAllocator> createWithAutoGrowth(size_t initialCapacity);
    
    // Create with frame tracking
    static std::unique_ptr<StackAllocator> createWithFrameTracking(size_t capacity);
    static std::unique_ptr<StackAllocator> createWithDebugging(size_t capacity);
    
    // Utility methods
    static size_t calculateOptimalCapacity(size_t estimatedUsage, bool allowGrowth = false);
    static size_t calculateGrowthIncrement(size_t baseCapacity);
    static StackAllocatorConfig createDefaultConfig(size_t capacity);
    static StackAllocatorConfig createGrowthConfig(size_t initialCapacity, size_t growthIncrement);
    static StackAllocatorConfig createDebugConfig(size_t capacity);
    static StackAllocatorConfig createFrameTrackingConfig(size_t capacity);
};

// Stack allocator utilities
namespace StackAllocatorUtils {
    // Stack frame analysis
    struct StackFrameAnalysis {
        size_t frameCount;
        size_t maxFrameDepth;
        size_t totalFrameMemory;
        size_t averageFrameMemory;
        std::vector<size_t> frameSizes;
        std::vector<size_t> frameAllocationCounts;
        std::vector<double> frameUtilizationRatios;
    };
    
    StackFrameAnalysis analyzeStackFrames(const StackAllocator& allocator);
    void dumpStackFrameAnalysis(const StackAllocator& allocator);
    std::string dumpStackFrameAnalysisToString(const StackAllocator& allocator);
    
    // Performance analysis
    struct StackPerformanceMetrics {
        double allocationSpeed; // allocations per second
        double deallocationSpeed; // deallocations per second
        double averageAllocationTime; // microseconds
        double averageDeallocationTime; // microseconds
        double memoryUtilization; // percentage
        size_t fragmentationLoss; // bytes
        double stackEfficiency; // percentage
        uint32_t allocationFailures;
        uint32_t deallocationFailures;
        uint32_t stackOverflowDetections;
        uint32_t stackUnderflowDetections;
        uint32_t growthOperations;
    };
    
    StackPerformanceMetrics analyzePerformance(const StackAllocator& allocator);
    void dumpPerformanceMetrics(const StackAllocator& allocator);
    std::string dumpPerformanceMetricsToString(const StackAllocator& allocator);
    
    // Memory optimization
    bool optimizeStackLayout(StackAllocator& allocator);
    bool compactStackMemory(StackAllocator& allocator);
    size_t estimateOptimalCapacity(const StackAllocator& allocator);
    size_t estimateOptimalGrowthIncrement(const StackAllocator& allocator);
    size_t estimateOptimalMaxStackDepth(const StackAllocator& allocator);
    
    // Debugging utilities
    void validateStackAllocator(const StackAllocator& allocator);
    void dumpStackAllocatorState(const StackAllocator& allocator);
    std::string dumpStackAllocatorStateToString(const StackAllocator& allocator);
    bool detectStackAllocatorCorruption(const StackAllocator& allocator);
    void repairStackAllocatorCorruption(StackAllocator& allocator);
    
    // Stack frame utilities
    void compareStackFrames(const StackAllocator& allocator1, const StackAllocator& allocator2);
    void mergeStackFrames(StackAllocator& target, const StackAllocator& source);
    void copyStackFrames(StackAllocator& target, const StackAllocator& source);
}

// RAII stack allocator scope
class StackAllocatorScope {
public:
    explicit StackAllocatorScope(StackAllocator* allocator, const char* file = nullptr, 
                                 int line = 0, const char* function = nullptr)
        : allocator_(allocator), hasFrame_(false) {
        if (allocator_) {
            allocator_->pushFrame(file, line, function);
            hasFrame_ = true;
        }
    }
    
    ~StackAllocatorScope() {
        if (allocator_ && hasFrame_) {
            allocator_->popFrame();
        }
    }
    
    StackAllocator* getAllocator() const { return allocator_; }
    size_t getFrameIndex() const { return allocator_ ? allocator_->getCurrentFrameIndex() : 0; }
    size_t getUsedBytes() const { return allocator_ ? allocator_->getCurrentPosition() : 0; }
    size_t getAllocationCount() const { 
        return allocator_ ? allocator_->getCurrentFrame().allocationCount : 0; 
    }
    
    // Pop frame manually
    void pop() {
        if (allocator_ && hasFrame_) {
            allocator_->popFrame();
            hasFrame_ = false;
        }
    }
    
    // Move constructor and assignment
    StackAllocatorScope(StackAllocatorScope&& other) noexcept
        : allocator_(other.allocator_), hasFrame_(other.hasFrame_) {
        other.allocator_ = nullptr;
        other.hasFrame_ = false;
    }
    
    StackAllocatorScope& operator=(StackAllocatorScope&& other) noexcept {
        if (this != &other) {
            pop(); // Pop current frame before moving
            allocator_ = other.allocator_;
            hasFrame_ = other.hasFrame_;
            other.allocator_ = nullptr;
            other.hasFrame_ = false;
        }
        return *this;
    }
    
private:
    StackAllocator* allocator_;
    bool hasFrame_;
    
    // Disabled copy operations
    StackAllocatorScope(const StackAllocatorScope&) = delete;
    StackAllocatorScope& operator=(const StackAllocatorScope&) = delete;
};

// RAII stack position marker
class StackPositionMarker {
public:
    explicit StackPositionMarker(StackAllocator* allocator)
        : allocator_(allocator), position_(0) {
        if (allocator_) {
            position_ = allocator_->getCurrentPosition();
        }
    }
    
    ~StackPositionMarker() {
        if (allocator_) {
            allocator_->rewindToPosition(position_);
        }
    }
    
    StackAllocator* getAllocator() const { return allocator_; }
    size_t getPosition() const { return position_; }
    size_t getCurrentPosition() const { return allocator_ ? allocator_->getCurrentPosition() : 0; }
    size_t getUsedBytes() const { return allocator_ ? allocator_->getCurrentPosition() - position_ : 0; }
    
    // Rewind to marked position manually
    void rewind() {
        if (allocator_) {
            allocator_->rewindToPosition(position_);
        }
    }
    
    // Move constructor and assignment
    StackPositionMarker(StackPositionMarker&& other) noexcept
        : allocator_(other.allocator_), position_(other.position_) {
        other.allocator_ = nullptr;
        other.position_ = 0;
    }
    
    StackPositionMarker& operator=(StackPositionMarker&& other) noexcept {
        if (this != &other) {
            rewind(); // Rewind current marker before moving
            allocator_ = other.allocator_;
            position_ = other.position_;
            other.allocator_ = nullptr;
            other.position_ = 0;
        }
        return *this;
    }
    
private:
    StackAllocator* allocator_;
    size_t position_;
    
    // Disabled copy operations
    StackPositionMarker(const StackPositionMarker&) = delete;
    StackPositionMarker& operator=(const StackPositionMarker&) = delete;
};

// Stack allocator macros for convenience
#define RF_STACK_ALLOCATOR_SCOPE(allocator) \
    StackAllocatorScope _stack_scope(allocator, __FILE__, __LINE__, __FUNCTION__)

#define RF_STACK_ALLOCATOR_SCOPE_NAMED(allocator, name) \
    StackAllocatorScope name(allocator, __FILE__, __LINE__, __FUNCTION__)

#define RF_STACK_POSITION_MARKER(allocator) \
    StackPositionMarker _stack_marker(allocator)

#define RF_STACK_POSITION_MARKER_NAMED(allocator, name) \
    StackPositionMarker name(allocator)

} // namespace RFMemory