#pragma once

#include "rf_allocator.h"
#include <vector>
#include <list>
#include <queue>
#include <unordered_set>
#include <mutex>
#include <atomic>

namespace RFMemory {

// Memory pool configuration
struct MemoryPoolConfig {
    size_t blockSize;
    size_t blockCount;
    size_t alignment;
    bool ownsMemory;
    bool allowGrowth;
    size_t growthIncrement;
    bool useGuardPages;
    bool useCanaryBytes;
    bool zeroOnFree;
    bool trackAllocations;
    size_t maxBlocks;
    uint32_t flags;
    
    MemoryPoolConfig() : blockSize(0), blockCount(0), alignment(Alignment::DEFAULT_ALIGNMENT), 
                        ownsMemory(true), allowGrowth(false), growthIncrement(0), useGuardPages(false), 
                        useCanaryBytes(false), zeroOnFree(false), trackAllocations(false), 
                        maxBlocks(SIZE_MAX), flags(0) {}
};

// Memory block header
struct MemoryBlockHeader {
    size_t blockSize;
    size_t alignment;
    uint32_t flags;
    uint64_t allocationId;
    uint64_t poolId;
    bool isInUse;
    uint32_t magicNumber;
    uint8_t canaryStart[4];
    uint8_t canaryEnd[4];
    
    static constexpr uint32_t MAGIC_NUMBER = 0xDEADC0DE;
    static constexpr uint32_t CANARY_VALUE = 0xC0FFEE;
    
    MemoryBlockHeader() : blockSize(0), alignment(Alignment::DEFAULT_ALIGNMENT), 
                         flags(0), allocationId(0), poolId(0), isInUse(false), magicNumber(MAGIC_NUMBER) {
        setCanary();
    }
    
    MemoryBlockHeader(size_t size, size_t align, uint32_t flg, uint64_t allocId, uint64_t poolId, bool inUse = false)
        : blockSize(size), alignment(align), flags(flg), allocationId(allocId), 
          poolId(poolId), isInUse(inUse), magicNumber(MAGIC_NUMBER) {
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
    
    bool validateMagic() const {
        return magicNumber == MAGIC_NUMBER;
    }
    
    bool validate() const {
        return validateMagic() && validateCanary();
    }
};

// Memory block information
struct MemoryBlockInfo {
    void* pointer;
    size_t blockSize;
    size_t alignment;
    uint32_t flags;
    uint64_t allocationId;
    uint64_t timestamp;
    const char* file;
    int line;
    const char* function;
    
    MemoryBlockInfo() : pointer(nullptr), blockSize(0), alignment(Alignment::DEFAULT_ALIGNMENT), 
                       flags(0), allocationId(0), timestamp(0), file(nullptr), line(0), function(nullptr) {}
    
    MemoryBlockInfo(void* ptr, size_t size, size_t align, uint32_t flg, uint64_t id, uint64_t ts,
                   const char* f = nullptr, int l = 0, const char* fn = nullptr)
        : pointer(ptr), blockSize(size), alignment(align), flags(flg), allocationId(id), 
          timestamp(ts), file(f), line(l), function(fn) {}
};

// Memory pool statistics
struct MemoryPoolStats {
    size_t totalBlocks;
    size_t usedBlocks;
    size_t freeBlocks;
    size_t peakBlocks;
    size_t totalMemory;
    size_t usedMemory;
    size_t freeMemory;
    size_t peakMemory;
    size_t allocationOverhead;
    size_t fragmentationLoss;
    size_t growthCount;
    size_t shrinkCount;
    uint64_t allocationCount;
    uint64_t deallocationCount;
    uint64_t allocationTime;
    uint64_t deallocationTime;
    double averageAllocationTime;
    double averageDeallocationTime;
    uint32_t allocationFailures;
    uint32_t deallocationFailures;
    uint32_t corruptionDetections;
    uint32_t doubleFreeDetections;
    uint32_t invalidFreeDetections;
    double utilizationRatio;
    
    MemoryPoolStats() : totalBlocks(0), usedBlocks(0), freeBlocks(0), peakBlocks(0), 
                       totalMemory(0), usedMemory(0), freeMemory(0), peakMemory(0), 
                       allocationOverhead(0), fragmentationLoss(0), growthCount(0), shrinkCount(0), 
                       allocationCount(0), deallocationCount(0), allocationTime(0), deallocationTime(0), 
                       averageAllocationTime(0.0), averageDeallocationTime(0.0), allocationFailures(0), 
                       deallocationFailures(0), corruptionDetections(0), doubleFreeDetections(0), 
                       invalidFreeDetections(0), utilizationRatio(0.0) {}
};

// Memory pool chunk
struct MemoryPoolChunk {
    void* memory;
    size_t size;
    size_t blockCount;
    std::vector<MemoryBlockHeader> headers;
    std::vector<void*> freeBlocks;
    std::unordered_set<void*> usedBlocks;
    uint64_t chunkId;
    uint64_t timestamp;
    
    MemoryPoolChunk() : memory(nullptr), size(0), blockCount(0), chunkId(0), timestamp(0) {}
    
    MemoryPoolChunk(void* mem, size_t sz, size_t count, uint64_t id, uint64_t ts)
        : memory(mem), size(sz), blockCount(count), chunkId(id), timestamp(ts) {
        headers.resize(count);
        freeBlocks.reserve(count);
    }
    
    bool isValid() const {
        return memory != nullptr && size > 0 && blockCount > 0 && headers.size() == blockCount;
    }
};

// Memory pool
class MemoryPool : public Allocator {
public:
    // Constructors
    explicit MemoryPool(size_t blockSize, size_t blockCount);
    MemoryPool(void* memory, size_t blockSize, size_t blockCount);
    MemoryPool(const MemoryPoolConfig& config);
    
    // Destructor
    virtual ~MemoryPool();
    
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
    
    // Pool-specific operations
    void grow(size_t additionalBlocks);
    void shrink(size_t blocksToRemove);
    void resize(size_t newBlockCount);
    void defragment();
    void compactFreeBlocks();
    
    // Memory information
    size_t getTotalMemory() const override;
    size_t getUsedMemory() const override;
    size_t getFreeMemory() const override;
    size_t getPeakMemory() const override;
    size_t getAllocationCount() const override;
    size_t getFragmentationLoss() const override;
    
    // Pool-specific information
    size_t getBlockSize() const;
    size_t getBlockCount() const;
    size_t getUsedBlockCount() const;
    size_t getFreeBlockCount() const;
    size_t getPeakBlockCount() const;
    size_t getChunkCount() const;
    size_t getAlignment() const;
    bool isGrowthEnabled() const;
    size_t getGrowthIncrement() const;
    size_t getMaxBlocks() const;
    
    // Statistics and debugging
    AllocatorStats getStatistics() const override;
    AllocationInfo getAllocationInfo() const override;
    std::vector<MemoryBlock> getAllocations() const override;
    bool containsPointer(const void* pointer) const override;
    MemoryBlock getBlockInfo(const void* pointer) const override;
    bool isValidPointer(const void* pointer) const override;
    
    // Pool-specific statistics
    MemoryPoolStats getPoolStatistics() const;
    std::vector<MemoryBlockInfo> getBlockInfos() const;
    std::vector<MemoryPoolChunk> getChunks() const;
    const MemoryPoolChunk& getChunk(size_t index) const;
    size_t getChunkIndex(const void* pointer) const;
    
    // Validation and debugging
    bool validate() const override;
    bool validatePointer(const void* pointer) const override;
    bool detectLeaks() const override;
    bool detectCorruption() const override;
    void dumpAllocations() const override;
    std::string dumpAllocationsToString() const override;
    void dumpStatistics() const override;
    std::string dumpStatisticsToString() const override;
    
    // Pool-specific debugging
    void dumpChunks() const;
    std::string dumpChunksToString() const;
    void dumpFreeBlocks() const;
    std::string dumpFreeBlocksToString() const;
    void dumpUsedBlocks() const;
    std::string dumpUsedBlocksToString() const;
    bool validateChunkIntegrity() const;
    bool validateBlockHeaders() const;
    bool validateFreeList() const;
    bool detectDoubleFree(const void* pointer) const;
    bool detectInvalidFree(const void* pointer) const;
    
    // Configuration
    void setDebugMode(bool enabled) override;
    bool isDebugMode() const override;
    void setLeakDetection(bool enabled) override;
    bool isLeakDetectionEnabled() const override;
    void setCorruptionDetection(bool enabled) override;
    bool isCorruptionDetectionEnabled() const override;
    
    // Pool-specific configuration
    void setZeroOnFree(bool enabled);
    bool isZeroOnFreeEnabled() const;
    void setTrackAllocations(bool enabled);
    bool isTrackAllocationsEnabled() const;
    void setMaxBlocks(size_t maxBlocks);
    void setGrowthEnabled(bool enabled);
    void setGrowthIncrement(size_t increment);
    
    // Utility methods
    const char* getName() const override;
    const char* getType() const override;
    uint64_t getId() const override;
    bool canAllocate(size_t size, size_t alignment = Alignment::DEFAULT_ALIGNMENT) const override;
    bool canReallocate(void* pointer, size_t newSize) const override;
    
    // Pool-specific utility methods
    bool isFromPool(const void* pointer) const;
    size_t getBlockIndex(const void* pointer) const;
    MemoryBlockHeader* getBlockHeader(const void* pointer) const;
    void* getBlockData(const MemoryBlockHeader* header) const;
    size_t getAllocationOverhead() const;
    double getUtilizationRatio() const;
    double getFragmentationRatio() const;
    
    // Event handling
    void setAllocationCallback(AllocationCallback callback) override;
    void setDeallocationCallback(DeallocationCallback callback) override;
    void setReallocationCallback(ReallocationCallback callback) override;
    void clearCallbacks() override;
    
    // Pool-specific events
    using GrowthCallback = std::function<void(size_t oldBlockCount, size_t newBlockCount)>;
    using ShrinkCallback = std::function<void(size_t oldBlockCount, size_t newBlockCount)>;
    using DefragmentationCallback = std::function<void(size_t movedBlocks)>;
    using ChunkCreateCallback = std::function<void(uint64_t chunkId, size_t chunkSize)>;
    using ChunkDestroyCallback = std::function<void(uint64_t chunkId)>;
    
    void setGrowthCallback(GrowthCallback callback);
    void setShrinkCallback(ShrinkCallback callback);
    void setDefragmentationCallback(DefragmentationCallback callback);
    void setChunkCreateCallback(ChunkCreateCallback callback);
    void setChunkDestroyCallback(ChunkDestroyCallback callback);
    
protected:
    // Protected helper methods
    void onAllocate(const MemoryBlock& block) override;
    void onDeallocate(const MemoryBlock& block) override;
    void onReallocate(const MemoryBlock& oldBlock, const MemoryBlock& newBlock) override;
    
    // Internal allocation helpers
    void* allocateInternal(size_t size, size_t alignment, uint32_t flags);
    void* allocateFromFreeList(size_t alignment);
    void* allocateFromNewChunk();
    void deallocateInternal(void* pointer);
    
    // Chunk management
    MemoryPoolChunk* createChunk(size_t blockCount);
    void destroyChunk(MemoryPoolChunk* chunk);
    void initializeChunk(MemoryPoolChunk* chunk);
    void addChunkToFreeList(MemoryPoolChunk* chunk);
    void removeChunkFromFreeList(MemoryPoolChunk* chunk);
    
    // Block management
    void addBlockToFreeList(MemoryPoolChunk* chunk, size_t blockIndex);
    void removeBlockFromFreeList(MemoryPoolChunk* chunk, size_t blockIndex);
    void markBlockAsUsed(MemoryPoolChunk* chunk, size_t blockIndex);
    void markBlockAsFree(MemoryPoolChunk* chunk, size_t blockIndex);
    void zeroBlock(MemoryPoolChunk* chunk, size_t blockIndex);
    
    // Memory management helpers
    bool growPool(size_t additionalBlocks);
    bool shrinkPool(size_t blocksToRemove);
    void defragmentPool();
    void compactFreeList();
    void validatePoolIntegrity() const;
    
    // Statistics helpers
    void updateAllocationStatistics(size_t size, uint64_t allocationTime);
    void updateDeallocationStatistics(size_t size);
    void updateGrowthStatistics(size_t oldBlockCount, size_t newBlockCount);
    void updateShrinkStatistics(size_t oldBlockCount, size_t newBlockCount);
    void updateDefragmentationStatistics(size_t movedBlocks);
    void calculateUtilizationRatio();
    void calculateFragmentationRatio();
    
    // Validation helpers
    bool validateMemoryRange(const void* pointer) const;
    bool validateBlockHeader(const MemoryBlockHeader* header) const;
    bool validateAlignment(const void* pointer, size_t alignment) const override;
    bool validateChunk(const MemoryPoolChunk* chunk) const;
    bool validateFreeListIntegrity() const;
    bool validateUsedBlocksIntegrity() const;
    
    // Debug helpers
    void logAllocation(const MemoryBlock& block) override;
    void logDeallocation(const MemoryBlock& block) override;
    void logGrowth(size_t oldBlockCount, size_t newBlockCount);
    void logShrink(size_t oldBlockCount, size_t newBlockCount);
    void logDefragmentation(size_t movedBlocks);
    void logChunkCreate(uint64_t chunkId, size_t chunkSize);
    void logChunkDestroy(uint64_t chunkId);
    void addAllocationToTracking(const MemoryBlock& block);
    void removeAllocationFromTracking(const void* pointer);
    
    // Thread safety
    mutable std::mutex mutex_;
    
private:
    // Pool configuration
    size_t blockSize_;
    size_t blockCount_;
    size_t alignment_;
    bool ownsMemory_;
    bool allowGrowth_;
    size_t growthIncrement_;
    size_t maxBlocks_;
    uint32_t flags_;
    
    // Pool configuration flags
    bool useGuardPages_;
    bool useCanaryBytes_;
    bool zeroOnFree_;
    bool trackAllocations_;
    
    // Memory management
    std::vector<MemoryPoolChunk*> chunks_;
    std::queue<void*> freeBlocks_;
    std::unordered_set<void*> usedBlocks_;
    size_t totalBlockCount_;
    size_t usedBlockCount_;
    size_t peakBlockCount_;
    
    // Statistics
    mutable MemoryPoolStats poolStats_;
    mutable std::vector<MemoryBlockInfo> allocationTracking_;
    
    // Identification
    static uint64_t nextId_;
    static uint64_t nextChunkId_;
    uint64_t id_;
    
    // Callbacks
    GrowthCallback growthCallback_;
    ShrinkCallback shrinkCallback_;
    DefragmentationCallback defragmentationCallback_;
    ChunkCreateCallback chunkCreateCallback_;
    ChunkDestroyCallback chunkDestroyCallback_;
    
    // Allocation ID tracking
    std::atomic<uint64_t> nextAllocationId_;
    
    // Disabled operations
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
};

// Memory pool factory
class MemoryPoolFactory {
public:
    // Create memory pools
    static std::unique_ptr<MemoryPool> create(size_t blockSize, size_t blockCount);
    static std::unique_ptr<MemoryPool> create(void* memory, size_t blockSize, size_t blockCount);
    static std::unique_ptr<MemoryPool> create(const MemoryPoolConfig& config);
    
    // Create with predefined configurations
    static std::unique_ptr<MemoryPool> createSmall(size_t blockSize = 64, size_t blockCount = 1024);
    static std::unique_ptr<MemoryPool> createMedium(size_t blockSize = 256, size_t blockCount = 4096);
    static std::unique_ptr<MemoryPool> createLarge(size_t blockSize = 1024, size_t blockCount = 16384);
    static std::unique_ptr<MemoryPool> createHuge(size_t blockSize = 4096, size_t blockCount = 65536);
    
    // Create with growth enabled
    static std::unique_ptr<MemoryPool> createWithGrowth(size_t blockSize, size_t initialBlockCount, 
                                                       size_t growthIncrement = 0);
    static std::unique_ptr<MemoryPool> createWithAutoGrowth(size_t blockSize, size_t initialBlockCount);
    
    // Create with debugging
    static std::unique_ptr<MemoryPool> createWithDebugging(size_t blockSize, size_t blockCount);
    static std::unique_ptr<MemoryPool> createWithTracking(size_t blockSize, size_t blockCount);
    
    // Utility methods
    static size_t calculateOptimalBlockSize(size_t averageAllocationSize);
    static size_t calculateOptimalBlockCount(size_t estimatedUsage, size_t blockSize);
    static size_t calculateGrowthIncrement(size_t baseBlockCount);
    static MemoryPoolConfig createDefaultConfig(size_t blockSize, size_t blockCount);
    static MemoryPoolConfig createGrowthConfig(size_t blockSize, size_t initialBlockCount, size_t growthIncrement);
    static MemoryPoolConfig createDebugConfig(size_t blockSize, size_t blockCount);
    static MemoryPoolConfig createTrackingConfig(size_t blockSize, size_t blockCount);
};

// Memory pool utilities
namespace MemoryPoolUtils {
    // Pool analysis
    struct PoolAnalysis {
        size_t totalChunks;
        size_t totalBlocks;
        size_t usedBlocks;
        size_t freeBlocks;
        size_t totalMemory;
        size_t usedMemory;
        size_t freeMemory;
        size_t overheadMemory;
        double utilizationRatio;
        double fragmentationRatio;
        std::vector<size_t> chunkSizes;
        std::vector<size_t> chunkUtilizations;
        std::vector<size_t> freeBlockCounts;
    };
    
    PoolAnalysis analyzePool(const MemoryPool& pool);
    void dumpPoolAnalysis(const MemoryPool& pool);
    std::string dumpPoolAnalysisToString(const MemoryPool& pool);
    
    // Performance analysis
    struct PoolPerformanceMetrics {
        double allocationSpeed; // allocations per second
        double deallocationSpeed; // deallocations per second
        double averageAllocationTime; // microseconds
        double averageDeallocationTime; // microseconds
        double memoryUtilization; // percentage
        double fragmentationRatio; // percentage
        double growthEfficiency; // percentage
        uint32_t allocationFailures;
        uint32_t deallocationFailures;
        uint32_t growthOperations;
        uint32_t shrinkOperations;
        uint32_t defragmentationOperations;
    };
    
    PoolPerformanceMetrics analyzePerformance(const MemoryPool& pool);
    void dumpPerformanceMetrics(const MemoryPool& pool);
    std::string dumpPerformanceMetricsToString(const MemoryPool& pool);
    
    // Memory optimization
    bool optimizePoolLayout(MemoryPool& pool);
    bool compactPoolMemory(MemoryPool& pool);
    bool defragmentPool(MemoryPool& pool);
    size_t estimateOptimalBlockSize(const MemoryPool& pool);
    size_t estimateOptimalBlockCount(const MemoryPool& pool);
    size_t estimateOptimalGrowthIncrement(const MemoryPool& pool);
    
    // Debugging utilities
    void validateMemoryPool(const MemoryPool& pool);
    void dumpMemoryPoolState(const MemoryPool& pool);
    std::string dumpMemoryPoolStateToString(const MemoryPool& pool);
    bool detectMemoryPoolCorruption(const MemoryPool& pool);
    void repairMemoryPoolCorruption(MemoryPool& pool);
    
    // Pool comparison
    void comparePools(const MemoryPool& pool1, const MemoryPool& pool2);
    void mergePools(MemoryPool& target, const MemoryPool& source);
    void copyPoolConfiguration(MemoryPool& target, const MemoryPool& source);
}

// RAII memory pool allocation
class PoolAllocation {
public:
    explicit PoolAllocation(MemoryPool* pool, size_t size, size_t alignment = Alignment::DEFAULT_ALIGNMENT)
        : pool_(pool), pointer_(nullptr), size_(size), alignment_(alignment) {
        if (pool_) {
            pointer_ = pool_->allocate(size, alignment);
        }
    }
    
    ~PoolAllocation() {
        if (pool_ && pointer_) {
            pool_->deallocate(pointer_);
        }
    }
    
    void* get() const { return pointer_; }
    template<typename T> T* getAs() const { return static_cast<T*>(pointer_); }
    size_t getSize() const { return size_; }
    size_t getAlignment() const { return alignment_; }
    bool isValid() const { return pointer_ != nullptr; }
    
    // Reallocate
    bool reallocate(size_t newSize, size_t newAlignment = Alignment::DEFAULT_ALIGNMENT) {
        if (pool_ && pointer_) {
            void* newPointer = pool_->reallocate(pointer_, newSize, newAlignment);
            if (newPointer) {
                pointer_ = newPointer;
                size_ = newSize;
                alignment_ = newAlignment;
                return true;
            }
        }
        return false;
    }
    
    // Move constructor and assignment
    PoolAllocation(PoolAllocation&& other) noexcept
        : pool_(other.pool_), pointer_(other.pointer_), size_(other.size_), alignment_(other.alignment_) {
        other.pool_ = nullptr;
        other.pointer_ = nullptr;
        other.size_ = 0;
        other.alignment_ = 0;
    }
    
    PoolAllocation& operator=(PoolAllocation&& other) noexcept {
        if (this != &other) {
            if (pool_ && pointer_) {
                pool_->deallocate(pointer_);
            }
            pool_ = other.pool_;
            pointer_ = other.pointer_;
            size_ = other.size_;
            alignment_ = other.alignment_;
            other.pool_ = nullptr;
            other.pointer_ = nullptr;
            other.size_ = 0;
            other.alignment_ = 0;
        }
        return *this;
    }
    
private:
    MemoryPool* pool_;
    void* pointer_;
    size_t size_;
    size_t alignment_;
    
    // Disabled copy operations
    PoolAllocation(const PoolAllocation&) = delete;
    PoolAllocation& operator=(const PoolAllocation&) = delete;
};

// Memory pool macros for convenience
#define RF_POOL_ALLOCATE(pool, size) \
    PoolAllocation _pool_alloc(pool, size)

#define RF_POOL_ALLOCATE_ALIGNED(pool, size, alignment) \
    PoolAllocation _pool_alloc(pool, size, alignment)

#define RF_POOL_ALLOCATE_NAMED(pool, size, name) \
    PoolAllocation name(pool, size)

#define RF_POOL_ALLOCATE_ALIGNED_NAMED(pool, size, alignment, name) \
    PoolAllocation name(pool, size, alignment)

} // namespace RFMemory