#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <type_traits>

namespace RFMemory {

// Forward declarations
class Allocator;
class MemoryBlock;
class AllocationInfo;
class AllocatorStats;

// Allocation flags
enum class AllocationFlags : uint32_t {
    NONE = 0x00000000,
    ZERO_MEMORY = 0x00000001,
    ALIGN_16 = 0x00000002,
    ALIGN_32 = 0x00000004,
    ALIGN_64 = 0x00000008,
    ALIGN_128 = 0x00000010,
    ALIGN_256 = 0x00000020,
    ALIGN_512 = 0x00000040,
    ALIGN_1024 = 0x00000080,
    NO_MERGE = 0x00000100,
    TEMPORARY = 0x00000200,
    PERSISTENT = 0x00000400,
    DEBUG_INFO = 0x00000800,
    GUARD_PAGES = 0x00001000,
    CANARY_BYTES = 0x00002000
};

// Allocation result codes
enum class AllocationResult : int32_t {
    SUCCESS = 0,
    OUT_OF_MEMORY = -1,
    INVALID_SIZE = -2,
    INVALID_ALIGNMENT = -3,
    INVALID_FLAGS = -4,
    ALLOCATION_FAILED = -5,
    DEALLOCATION_FAILED = -6,
    CORRUPTION_DETECTED = -7,
    DOUBLE_FREE = -8,
    INVALID_POINTER = -9,
    LEAK_DETECTED = -10,
    OVERFLOW_DETECTED = -11,
    UNDERFLOW_DETECTED = -12
};

// Memory alignment utilities
namespace Alignment {
    constexpr size_t MIN_ALIGNMENT = sizeof(void*);
    constexpr size_t DEFAULT_ALIGNMENT = 16;
    constexpr size_t CACHE_LINE_SIZE = 64;
    constexpr size_t PAGE_SIZE = 4096;
    
    inline bool isPowerOfTwo(size_t value) {
        return value != 0 && (value & (value - 1)) == 0;
    }
    
    inline size_t alignUp(size_t size, size_t alignment) {
        return (size + alignment - 1) & ~(alignment - 1);
    }
    
    inline size_t alignDown(size_t size, size_t alignment) {
        return size & ~(alignment - 1);
    }
    
    inline bool isAligned(const void* ptr, size_t alignment) {
        return (reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0;
    }
    
    inline void* alignPointer(void* ptr, size_t alignment) {
        uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
        return reinterpret_cast<void*>((address + alignment - 1) & ~(alignment - 1));
    }
}

// Memory block information
struct MemoryBlock {
    void* pointer;
    size_t size;
    size_t alignment;
    uint32_t flags;
    uint64_t allocationId;
    const char* file;
    int line;
    const char* function;
    uint64_t timestamp;
    
    MemoryBlock() : pointer(nullptr), size(0), alignment(Alignment::DEFAULT_ALIGNMENT), 
                   flags(0), allocationId(0), file(nullptr), line(0), function(nullptr), timestamp(0) {}
    
    MemoryBlock(void* ptr, size_t sz, size_t align = Alignment::DEFAULT_ALIGNMENT, 
                uint32_t flg = 0, uint64_t id = 0, const char* f = nullptr, 
                int l = 0, const char* fn = nullptr, uint64_t ts = 0)
        : pointer(ptr), size(sz), alignment(align), flags(flg), allocationId(id), 
          file(f), line(l), function(fn), timestamp(ts) {}
};

// Allocation tracking information
struct AllocationInfo {
    size_t totalAllocations;
    size_t totalDeallocations;
    size_t currentAllocations;
    size_t peakAllocations;
    size_t totalBytesAllocated;
    size_t totalBytesDeallocated;
    size_t currentBytesAllocated;
    size_t peakBytesAllocated;
    size_t largestAllocation;
    size_t smallestAllocation;
    uint64_t allocationCount;
    uint64_t deallocationCount;
    double averageAllocationSize;
    double fragmentationRatio;
    
    AllocationInfo() : totalAllocations(0), totalDeallocations(0), currentAllocations(0), 
                       peakAllocations(0), totalBytesAllocated(0), totalBytesDeallocated(0), 
                       currentBytesAllocated(0), peakBytesAllocated(0), largestAllocation(0), 
                       smallestAllocation(SIZE_MAX), allocationCount(0), deallocationCount(0), 
                       averageAllocationSize(0.0), fragmentationRatio(0.0) {}
    
    void updateAllocation(size_t size) {
        totalAllocations++;
        currentAllocations++;
        allocationCount++;
        totalBytesAllocated += size;
        currentBytesAllocated += size;
        
        if (currentAllocations > peakAllocations) {
            peakAllocations = currentAllocations;
        }
        
        if (currentBytesAllocated > peakBytesAllocated) {
            peakBytesAllocated = currentBytesAllocated;
        }
        
        if (size > largestAllocation) {
            largestAllocation = size;
        }
        
        if (size < smallestAllocation) {
            smallestAllocation = size;
        }
        
        averageAllocationSize = static_cast<double>(totalBytesAllocated) / allocationCount;
    }
    
    void updateDeallocation(size_t size) {
        totalDeallocations++;
        currentAllocations--;
        deallocationCount++;
        totalBytesDeallocated += size;
        currentBytesAllocated -= size;
        
        averageAllocationSize = static_cast<double>(totalBytesAllocated) / allocationCount;
        
        if (totalBytesAllocated > 0) {
            fragmentationRatio = 1.0 - (static_cast<double>(currentBytesAllocated) / totalBytesAllocated);
        }
    }
};

// Allocator statistics
struct AllocatorStats {
    AllocationInfo allocationInfo;
    size_t totalMemory;
    size_t usedMemory;
    size_t freeMemory;
    size_t peakMemory;
    size_t allocationOverhead;
    size_t fragmentationLoss;
    uint64_t allocationTime;
    uint64_t deallocationTime;
    uint64_t totalAllocationTime;
    uint64_t totalDeallocationTime;
    double averageAllocationTime;
    double averageDeallocationTime;
    uint32_t allocationFailures;
    uint32_t deallocationFailures;
    uint32_t corruptionDetections;
    uint32_t leakDetections;
    
    AllocatorStats() : totalMemory(0), usedMemory(0), freeMemory(0), peakMemory(0), 
                       allocationOverhead(0), fragmentationLoss(0), allocationTime(0), 
                       deallocationTime(0), totalAllocationTime(0), totalDeallocationTime(0), 
                       averageAllocationTime(0.0), averageDeallocationTime(0.0), 
                       allocationFailures(0), deallocationFailures(0), corruptionDetections(0), 
                       leakDetections(0) {}
};

// Base allocator interface
class Allocator {
public:
    Allocator();
    virtual ~Allocator();
    
    // Core allocation interface
    virtual void* allocate(size_t size, size_t alignment = Alignment::DEFAULT_ALIGNMENT, 
                          uint32_t flags = static_cast<uint32_t>(AllocationFlags::NONE)) = 0;
    virtual void deallocate(void* pointer) = 0;
    virtual void* reallocate(void* pointer, size_t newSize, size_t alignment = Alignment::DEFAULT_ALIGNMENT) = 0;
    
    // Advanced allocation methods
    virtual void* allocateAligned(size_t size, size_t alignment);
    virtual void deallocateAligned(void* pointer);
    virtual void* allocateZeroed(size_t size, size_t alignment = Alignment::DEFAULT_ALIGNMENT);
    virtual void* allocateArray(size_t count, size_t elementSize, size_t alignment = Alignment::DEFAULT_ALIGNMENT);
    virtual void deallocateArray(void* pointer);
    
    // Memory management
    virtual void reset() = 0;
    virtual void purge() = 0;
    virtual void trim() = 0;
    virtual void compact() = 0;
    
    // Memory information
    virtual size_t getTotalMemory() const = 0;
    virtual size_t getUsedMemory() const = 0;
    virtual size_t getFreeMemory() const = 0;
    virtual size_t getPeakMemory() const = 0;
    virtual size_t getAllocationCount() const = 0;
    virtual size_t getFragmentationLoss() const = 0;
    
    // Statistics and debugging
    virtual AllocatorStats getStatistics() const = 0;
    virtual AllocationInfo getAllocationInfo() const = 0;
    virtual std::vector<MemoryBlock> getAllocations() const = 0;
    virtual bool containsPointer(const void* pointer) const = 0;
    virtual MemoryBlock getBlockInfo(const void* pointer) const = 0;
    virtual bool isValidPointer(const void* pointer) const = 0;
    
    // Validation and debugging
    virtual bool validate() const = 0;
    virtual bool validatePointer(const void* pointer) const = 0;
    virtual bool detectLeaks() const = 0;
    virtual bool detectCorruption() const = 0;
    virtual void dumpAllocations() const = 0;
    virtual std::string dumpAllocationsToString() const = 0;
    virtual void dumpStatistics() const = 0;
    virtual std::string dumpStatisticsToString() const = 0;
    
    // Configuration
    virtual void setDebugMode(bool enabled) = 0;
    virtual bool isDebugMode() const = 0;
    virtual void setLeakDetection(bool enabled) = 0;
    virtual bool isLeakDetectionEnabled() const = 0;
    virtual void setCorruptionDetection(bool enabled) = 0;
    virtual bool isCorruptionDetectionEnabled() const = 0;
    
    // Utility methods
    virtual const char* getName() const = 0;
    virtual const char* getType() const = 0;
    virtual uint64_t getId() const = 0;
    virtual bool canAllocate(size_t size, size_t alignment = Alignment::DEFAULT_ALIGNMENT) const = 0;
    virtual bool canReallocate(void* pointer, size_t newSize) const = 0;
    
    // Event handling
    using AllocationCallback = std::function<void(const MemoryBlock&)>;
    using DeallocationCallback = std::function<void(const MemoryBlock&)>;
    using ReallocationCallback = std::function<void(const MemoryBlock&, const MemoryBlock&)>;
    
    virtual void setAllocationCallback(AllocationCallback callback) = 0;
    virtual void setDeallocationCallback(DeallocationCallback callback) = 0;
    virtual void setReallocationCallback(ReallocationCallback callback) = 0;
    virtual void clearCallbacks() = 0;
    
    // Comparison operators
    bool operator==(const Allocator& other) const;
    bool operator!=(const Allocator& other) const;
    
protected:
    // Protected helper methods
    virtual void onAllocate(const MemoryBlock& block);
    virtual void onDeallocate(const MemoryBlock& block);
    virtual void onReallocate(const MemoryBlock& oldBlock, const MemoryBlock& newBlock);
    virtual void updateStatistics(size_t allocatedSize, size_t deallocatedSize);
    virtual void recordAllocationFailure(AllocationResult result);
    virtual void recordDeallocationFailure(AllocationResult result);
    
    // Debug helpers
    virtual void addDebugInfo(const std::string& message);
    virtual void logAllocation(const MemoryBlock& block);
    virtual void logDeallocation(const MemoryBlock& block);
    virtual void logReallocation(const MemoryBlock& oldBlock, const MemoryBlock& newBlock);
    
    // Validation helpers
    virtual bool validateAlignment(const void* pointer, size_t alignment) const;
    virtual bool validateCanaryBytes(const void* pointer, size_t size) const;
    virtual bool validateGuardPages(const void* pointer, size_t size) const;
    virtual void writeCanaryBytes(void* pointer, size_t size);
    virtual void writeGuardPages(void* pointer, size_t size);
    
    // Statistics tracking
    mutable AllocatorStats stats_;
    mutable AllocationInfo allocationInfo_;
    
    // Configuration
    bool debugMode_;
    bool leakDetection_;
    bool corruptionDetection_;
    
    // Callbacks
    AllocationCallback allocationCallback_;
    DeallocationCallback deallocationCallback_;
    ReallocationCallback reallocationCallback_;
    
    // Identification
    const char* name_;
    const char* type_;
    uint64_t id_;
    
private:
    // Disabled operations
    Allocator(const Allocator&) = delete;
    Allocator& operator=(const Allocator&) = delete;
};

// Allocator factory
class AllocatorFactory {
public:
    // Create different types of allocators
    static std::unique_ptr<Allocator> createLinearAllocator(size_t capacity);
    static std::unique_ptr<Allocator> createStackAllocator(size_t capacity);
    static std::unique_ptr<Allocator> createMemoryPool(size_t blockSize, size_t blockCount);
    static std::unique_ptr<Allocator> createObjectPool(size_t objectSize, size_t objectCount);
    static std::unique_ptr<Allocator> createTrackingAllocator(std::unique_ptr<Allocator> baseAllocator);
    
    // Create allocators with custom configuration
    static std::unique_ptr<Allocator> createLinearAllocator(void* memory, size_t capacity);
    static std::unique_ptr<Allocator> createStackAllocator(void* memory, size_t capacity);
    static std::unique_ptr<Allocator> createMemoryPool(void* memory, size_t blockSize, size_t blockCount);
    static std::unique_ptr<Allocator> createObjectPool(void* memory, size_t objectSize, size_t objectCount);
    
    // Utility methods
    static std::vector<std::string> getSupportedAllocatorTypes();
    static bool isAllocatorTypeSupported(const std::string& type);
    static size_t getRecommendedAllocatorSize(const std::string& type, size_t estimatedUsage);
};

// Memory utilities
namespace MemoryUtils {
    // Memory operations
    void* memoryCopy(void* destination, const void* source, size_t size);
    void* memoryMove(void* destination, const void* source, size_t size);
    void* memorySet(void* destination, int value, size_t size);
    int memoryCompare(const void* pointer1, const void* pointer2, size_t size);
    
    // Memory alignment
    void* alignedAllocate(size_t size, size_t alignment);
    void alignedDeallocate(void* pointer);
    void* alignedReallocate(void* pointer, size_t newSize, size_t alignment);
    
    // Memory information
    size_t getPageSize();
    size_t getCacheLineSize();
    size_t getSystemMemorySize();
    size_t getAvailableMemorySize();
    bool isMemoryAvailable(size_t requiredSize);
    
    // Memory protection
    bool protectMemory(void* pointer, size_t size, uint32_t protection);
    bool unprotectMemory(void* pointer, size_t size);
    bool lockMemory(void* pointer, size_t size);
    bool unlockMemory(void* pointer, size_t size);
    
    // Memory debugging
    void fillMemory(void* pointer, size_t size, uint8_t pattern);
    void fillMemoryPattern(void* pointer, size_t size, const uint8_t* pattern, size_t patternSize);
    bool verifyMemoryPattern(const void* pointer, size_t size, const uint8_t* pattern, size_t patternSize);
    void dumpMemory(const void* pointer, size_t size, size_t bytesPerLine = 16);
    std::string dumpMemoryToString(const void* pointer, size_t size, size_t bytesPerLine = 16);
}

// RAII memory management
template<typename T>
class ScopedMemory {
public:
    explicit ScopedMemory(Allocator* allocator, size_t count = 1)
        : allocator_(allocator), pointer_(nullptr), count_(count) {
        if (allocator_) {
            pointer_ = allocator_->allocate(sizeof(T) * count, alignof(T));
        }
    }
    
    ~ScopedMemory() {
        if (allocator_ && pointer_) {
            allocator_->deallocate(pointer_);
        }
    }
    
    T* get() const { return static_cast<T*>(pointer_); }
    T& operator*() const { return *get(); }
    T* operator->() const { return get(); }
    T& operator[](size_t index) const { return get()[index]; }
    
    bool isValid() const { return pointer_ != nullptr; }
    size_t getCount() const { return count_; }
    
    // Move constructor and assignment
    ScopedMemory(ScopedMemory&& other) noexcept
        : allocator_(other.allocator_), pointer_(other.pointer_), count_(other.count_) {
        other.allocator_ = nullptr;
        other.pointer_ = nullptr;
        other.count_ = 0;
    }
    
    ScopedMemory& operator=(ScopedMemory&& other) noexcept {
        if (this != &other) {
            if (allocator_ && pointer_) {
                allocator_->deallocate(pointer_);
            }
            allocator_ = other.allocator_;
            pointer_ = other.pointer_;
            count_ = other.count_;
            other.allocator_ = nullptr;
            other.pointer_ = nullptr;
            other.count_ = 0;
        }
        return *this;
    }
    
private:
    Allocator* allocator_;
    void* pointer_;
    size_t count_;
    
    // Disabled copy operations
    ScopedMemory(const ScopedMemory&) = delete;
    ScopedMemory& operator=(const ScopedMemory&) = delete;
};

// Memory allocation macros for debugging
#ifdef RF_MEMORY_DEBUG
    #define RF_ALLOCATE(allocator, size) \
        (allocator)->allocate((size), Alignment::DEFAULT_ALIGNMENT, \
            static_cast<uint32_t>(AllocationFlags::DEBUG_INFO), __FILE__, __LINE__, __FUNCTION__)
    
    #define RF_ALLOCATE_ALIGNED(allocator, size, alignment) \
        (allocator)->allocate((size), (alignment), \
            static_cast<uint32_t>(AllocationFlags::DEBUG_INFO), __FILE__, __LINE__, __FUNCTION__)
    
    #define RF_DEALLOCATE(allocator, pointer) \
        (allocator)->deallocate((pointer))
    
    #define RF_REALLOCATE(allocator, pointer, newSize) \
        (allocator)->reallocate((pointer), (newSize), Alignment::DEFAULT_ALIGNMENT)
#else
    #define RF_ALLOCATE(allocator, size) \
        (allocator)->allocate((size))
    
    #define RF_ALLOCATE_ALIGNED(allocator, size, alignment) \
        (allocator)->allocate((size), (alignment))
    
    #define RF_DEALLOCATE(allocator, pointer) \
        (allocator)->deallocate((pointer))
    
    #define RF_REALLOCATE(allocator, pointer, newSize) \
        (allocator)->reallocate((pointer), (newSize))
#endif

} // namespace RFMemory