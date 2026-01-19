#pragma once

#include "rf_allocator.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <functional>
#include <mutex>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>

namespace RFMemory {

// Memory tracking configuration
struct MemoryTrackerConfig {
    bool trackAllocations;
    bool trackLeaks;
    bool trackCorruption;
    bool trackPeakUsage;
    bool trackFragmentation;
    bool trackCallStacks;
    bool trackThreads;
    bool trackTimestamps;
    bool trackLifetime;
    bool enableStackTrace;
    bool enableGuardPages;
    bool enableCanaryBytes;
    size_t maxStackTraceDepth;
    size_t maxTrackedAllocations;
    size_t maxCallStackEntries;
    std::string logFileName;
    bool enableLogging;
    bool enableRealTimeMonitoring;
    uint32_t checkIntervalMs;
    bool enableAutoDump;
    bool enableStatistics;
    bool enableReporting;
    
    MemoryTrackerConfig() : trackAllocations(true), trackLeaks(true), trackCorruption(true), 
                           trackPeakUsage(true), trackFragmentation(true), trackCallStacks(false), 
                           trackThreads(true), trackTimestamps(true), trackLifetime(true), 
                           enableStackTrace(false), enableGuardPages(false), enableCanaryBytes(true), 
                           maxStackTraceDepth(32), maxTrackedAllocations(100000), maxCallStackEntries(1000), 
                           logFileName(""), enableLogging(false), enableRealTimeMonitoring(false), 
                           checkIntervalMs(1000), enableAutoDump(false), enableStatistics(true), 
                           enableReporting(true) {}
};

// Memory allocation record
struct MemoryAllocationRecord {
    void* pointer;
    size_t size;
    size_t alignment;
    uint32_t flags;
    uint64_t allocationId;
    uint64_t threadId;
    uint64_t timestamp;
    const char* file;
    int line;
    const char* function;
    std::vector<void*> callStack;
    uint64_t lifetimeStart;
    uint64_t lifetimeEnd;
    bool isLeaked;
    bool isCorrupted;
    bool isDoubleFreed;
    bool isInvalidFree;
    uint8_t canaryStart[4];
    uint8_t canaryEnd[4];
    
    static constexpr uint32_t CANARY_VALUE = 0xFEEDFACE;
    
    MemoryAllocationRecord() : pointer(nullptr), size(0), alignment(Alignment::DEFAULT_ALIGNMENT), 
                             flags(0), allocationId(0), threadId(0), timestamp(0), file(nullptr), 
                             line(0), function(nullptr), lifetimeStart(0), lifetimeEnd(0), 
                             isLeaked(false), isCorrupted(false), isDoubleFreed(false), isInvalidFree(false) {
        setCanary();
    }
    
    MemoryAllocationRecord(void* ptr, size_t sz, size_t align, uint32_t flg, uint64_t id, uint64_t tid, 
                          uint64_t ts, const char* f = nullptr, int l = 0, const char* fn = nullptr)
        : pointer(ptr), size(sz), alignment(align), flags(flg), allocationId(id), threadId(tid), 
          timestamp(ts), file(f), line(l), function(fn), lifetimeStart(ts), lifetimeEnd(0), 
          isLeaked(false), isCorrupted(false), isDoubleFreed(false), isInvalidFree(false) {
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
    
    uint64_t getLifetime() const {
        return lifetimeEnd > lifetimeStart ? lifetimeEnd - lifetimeStart : 0;
    }
    
    std::string getCallStackString() const;
    std::string getLocationString() const;
    std::string getInfoString() const;
};

// Thread allocation information
struct ThreadAllocationInfo {
    std::thread::id threadId;
    std::string threadName;
    size_t totalAllocations;
    size_t totalBytes;
    size_t currentAllocations;
    size_t currentBytes;
    size_t peakAllocations;
    size_t peakBytes;
    uint64_t firstAllocationTime;
    uint64_t lastAllocationTime;
    std::vector<uint64_t> allocationIds;
    
    ThreadAllocationInfo() : threadId(), totalAllocations(0), totalBytes(0), 
                           currentAllocations(0), currentBytes(0), peakAllocations(0), 
                           peakBytes(0), firstAllocationTime(0), lastAllocationTime(0) {}
};

// Memory leak report
struct MemoryLeakReport {
    size_t totalLeaks;
    size_t totalLeakedBytes;
    size_t peakLeaks;
    size_t peakLeakedBytes;
    std::vector<MemoryAllocationRecord> leakedAllocations;
    std::map<std::string, size_t> leaksByFile;
    std::map<std::string, size_t> leaksByFunction;
    std::map<uint64_t, size_t> leaksByThread;
    std::map<size_t, size_t> leaksBySize;
    std::vector<std::string> callStacks;
    
    MemoryLeakReport() : totalLeaks(0), totalLeakedBytes(0), peakLeaks(0), peakLeakedBytes(0) {}
    
    void addLeak(const MemoryAllocationRecord& record);
    std::string generateReport() const;
    void dumpToFile(const std::string& filename) const;
};

// Memory corruption report
struct MemoryCorruptionReport {
    size_t totalCorruptions;
    size_t totalCorruptedBytes;
    std::vector<MemoryAllocationRecord> corruptedAllocations;
    std::map<std::string, size_t> corruptionsByType;
    std::map<uint64_t, size_t> corruptionsByThread;
    std::map<uint64_t, size_t> corruptionsByAllocationId;
    std::vector<std::string> corruptionDetails;
    
    MemoryCorruptionReport() : totalCorruptions(0), totalCorruptedBytes(0) {}
    
    void addCorruption(const MemoryAllocationRecord& record, const std::string& type);
    std::string generateReport() const;
    void dumpToFile(const std::string& filename) const;
};

// Memory statistics
struct MemoryStatistics {
    size_t totalAllocations;
    size_t totalDeallocations;
    size_t currentAllocations;
    size_t peakAllocations;
    size_t totalBytesAllocated;
    size_t totalBytesDeallocated;
    size_t currentBytesAllocated;
    size_t peakBytesAllocated;
    size_t allocationOverhead;
    size_t fragmentationLoss;
    uint64_t totalAllocationTime;
    uint64_t totalDeallocationTime;
    double averageAllocationTime;
    double averageDeallocationTime;
    uint32_t allocationFailures;
    uint32_t deallocationFailures;
    uint32_t corruptionDetections;
    uint32_t leakDetections;
    uint32_t doubleFreeDetections;
    uint32_t invalidFreeDetections;
    std::map<uint64_t, ThreadAllocationInfo> threadInfo;
    std::map<size_t, size_t> allocationSizeDistribution;
    std::map<std::string, size_t> allocationLocationDistribution;
    
    MemoryStatistics() : totalAllocations(0), totalDeallocations(0), currentAllocations(0), 
                        peakAllocations(0), totalBytesAllocated(0), totalBytesDeallocated(0), 
                        currentBytesAllocated(0), peakBytesAllocated(0), allocationOverhead(0), 
                        fragmentationLoss(0), totalAllocationTime(0), totalDeallocationTime(0), 
                        averageAllocationTime(0.0), averageDeallocationTime(0.0), allocationFailures(0), 
                        deallocationFailures(0), corruptionDetections(0), leakDetections(0), 
                        doubleFreeDetections(0), invalidFreeDetections(0) {}
    
    void updateAllocation(const MemoryAllocationRecord& record);
    void updateDeallocation(const MemoryAllocationRecord& record);
    void updatePeakUsage();
    void updateFragmentation();
    std::string generateReport() const;
    void dumpToFile(const std::string& filename) const;
};

// Memory tracker
class MemoryTracker {
public:
    // Singleton access
    static MemoryTracker& getInstance();
    
    // Configuration
    void configure(const MemoryTrackerConfig& config);
    MemoryTrackerConfig getConfiguration() const;
    void setTrackAllocations(bool enabled);
    void setTrackLeaks(bool enabled);
    void setTrackCorruption(bool enabled);
    void setTrackCallStacks(bool enabled);
    void setEnableStackTrace(bool enabled);
    void setEnableGuardPages(bool enabled);
    void setEnableCanaryBytes(bool enabled);
    void setMaxTrackedAllocations(size_t maxAllocations);
    void setLogFileName(const std::string& filename);
    void setEnableLogging(bool enabled);
    void setEnableRealTimeMonitoring(bool enabled);
    void setCheckInterval(uint32_t intervalMs);
    void setEnableAutoDump(bool enabled);
    
    // Tracking operations
    void trackAllocation(void* pointer, size_t size, size_t alignment, uint32_t flags, 
                        const char* file = nullptr, int line = 0, const char* function = nullptr);
    void trackDeallocation(void* pointer, const char* file = nullptr, int line = 0, const char* function = nullptr);
    void trackReallocation(void* oldPointer, void* newPointer, size_t newSize, 
                          const char* file = nullptr, int line = 0, const char* function = nullptr);
    
    // Advanced tracking
    void trackAllocationWithCallStack(void* pointer, size_t size, size_t alignment, uint32_t flags, 
                                    const char* file = nullptr, int line = 0, const char* function = nullptr);
    void trackAllocationWithGuardPages(void* pointer, size_t size, size_t alignment, uint32_t flags, 
                                     const char* file = nullptr, int line = 0, const char* function = nullptr);
    void trackAllocationWithCanaryBytes(void* pointer, size_t size, size_t alignment, uint32_t flags, 
                                       const char* file = nullptr, int line = 0, const char* function = nullptr);
    
    // Query operations
    bool isTrackingAllocation(void* pointer) const;
    MemoryAllocationRecord* getAllocationRecord(void* pointer);
    const MemoryAllocationRecord* getAllocationRecord(void* pointer) const;
    std::vector<MemoryAllocationRecord> getAllAllocations() const;
    std::vector<MemoryAllocationRecord> getAllocationsByThread(uint64_t threadId) const;
    std::vector<MemoryAllocationRecord> getAllocationsBySize(size_t minSize, size_t maxSize) const;
    std::vector<MemoryAllocationRecord> getAllocationsByLocation(const std::string& file, const std::string& function) const;
    std::vector<MemoryAllocationRecord> getLeakedAllocations() const;
    std::vector<MemoryAllocationRecord> getCorruptedAllocations() const;
    
    // Statistics
    MemoryStatistics getStatistics() const;
    ThreadAllocationInfo getThreadInfo(uint64_t threadId) const;
    std::map<uint64_t, ThreadAllocationInfo> getAllThreadInfo() const;
    size_t getCurrentAllocationCount() const;
    size_t getCurrentAllocatedBytes() const;
    size_t getPeakAllocationCount() const;
    size_t getPeakAllocatedBytes() const;
    double getFragmentationRatio() const;
    
    // Leak detection
    bool detectLeaks();
    MemoryLeakReport generateLeakReport();
    void dumpLeakReport(const std::string& filename = "") const;
    std::string getLeakReportString() const;
    size_t getLeakCount() const;
    size_t getLeakedBytes() const;
    
    // Corruption detection
    bool detectCorruption();
    MemoryCorruptionReport generateCorruptionReport();
    void dumpCorruptionReport(const std::string& filename = "") const;
    std::string getCorruptionReportString() const;
    size_t getCorruptionCount() const;
    size_t getCorruptedBytes() const;
    
    // Validation
    bool validateAllAllocations();
    bool validateAllocation(void* pointer);
    bool validateCanaryBytes(void* pointer);
    bool validateGuardPages(void* pointer);
    bool validateCallStack(void* pointer);
    
    // Reporting and dumping
    void dumpStatistics(const std::string& filename = "") const;
    void dumpAllocations(const std::string& filename = "") const;
    void dumpThreadInfo(const std::string& filename = "") const;
    void dumpCallStacks(const std::string& filename = "") const;
    void dumpFullReport(const std::string& filename = "") const;
    std::string getStatisticsString() const;
    std::string getAllocationsString() const;
    std::string getThreadInfoString() const;
    std::string getCallStacksString() const;
    std::string getFullReportString() const;
    
    // Real-time monitoring
    void startRealTimeMonitoring();
    void stopRealTimeMonitoring();
    bool isRealTimeMonitoringActive() const;
    void setMonitoringCallback(std::function<void(const MemoryStatistics&)> callback);
    
    // Memory analysis
    std::map<size_t, size_t> getAllocationSizeDistribution() const;
    std::map<std::string, size_t> getAllocationLocationDistribution() const;
    std::vector<std::pair<std::string, size_t>> getTopLeakingLocations(size_t count = 10) const;
    std::vector<std::pair<std::string, size_t>> getTopCorruptingLocations(size_t count = 10) const;
    std::vector<std::pair<uint64_t, size_t>> getTopLeakingThreads(size_t count = 10) const;
    std::vector<std::pair<uint64_t, size_t>> getTopAllocatingThreads(size_t count = 10) const;
    
    // Memory optimization suggestions
    std::vector<std::string> getOptimizationSuggestions() const;
    std::string getOptimizationReport() const;
    
    // Cleanup and reset
    void reset();
    void clear();
    void cleanup();
    void compact();
    
    // Advanced features
    void enableGlobalTracking();
    void disableGlobalTracking();
    bool isGlobalTrackingEnabled() const;
    void setGlobalAllocator(Allocator* allocator);
    Allocator* getGlobalAllocator() const;
    
    // Event handling
    using AllocationCallback = std::function<void(const MemoryAllocationRecord&)>;
    using DeallocationCallback = std::function<void(const MemoryAllocationRecord&)>;
    using LeakDetectedCallback = std::function<void(const MemoryAllocationRecord&)>;
    using CorruptionDetectedCallback = std::function<void(const MemoryAllocationRecord&, const std::string&)>;
    using StatisticsUpdateCallback = std::function<void(const MemoryStatistics&)>;
    
    void setAllocationCallback(AllocationCallback callback);
    void setDeallocationCallback(DeallocationCallback callback);
    void setLeakDetectedCallback(LeakDetectedCallback callback);
    void setCorruptionDetectedCallback(CorruptionDetectedCallback callback);
    void setStatisticsUpdateCallback(StatisticsUpdateCallback callback);
    void clearCallbacks();
    
protected:
    // Protected constructor for singleton
    MemoryTracker();
    virtual ~MemoryTracker();
    
    // Internal helpers
    void trackAllocationInternal(void* pointer, size_t size, size_t alignment, uint32_t flags, 
                                const char* file, int line, const char* function);
    void trackDeallocationInternal(void* pointer, const char* file, int line, const char* function);
    void updateStatistics(const MemoryAllocationRecord& record, bool isAllocation);
    void checkForLeaks();
    void checkForCorruption();
    void performRealTimeCheck();
    void logMessage(const std::string& message);
    void logAllocation(const MemoryAllocationRecord& record);
    void logDeallocation(const MemoryAllocationRecord& record);
    void logLeak(const MemoryAllocationRecord& record);
    void logCorruption(const MemoryAllocationRecord& record, const std::string& type);
    
    // Call stack capture
    std::vector<void*> captureCallStack(size_t maxDepth = 32);
    std::string callStackToString(const std::vector<void*>& callStack) const;
    
    // Thread management
    uint64_t getCurrentThreadId() const;
    std::string getCurrentThreadName() const;
    void updateThreadInfo(uint64_t threadId, const MemoryAllocationRecord& record, bool isAllocation);
    
    // File I/O
    void dumpToFile(const std::string& filename, const std::string& content) const;
    std::string getCurrentTimestamp() const;
    std::string generateReportHeader() const;
    std::string generateReportFooter() const;
    
    // Validation helpers
    bool validateAllocationRecord(const MemoryAllocationRecord& record) const;
    bool validateMemoryRange(void* pointer, size_t size) const;
    void writeCanaryBytes(void* pointer, size_t size);
    bool checkCanaryBytes(void* pointer, size_t size);
    void setupGuardPages(void* pointer, size_t size);
    void cleanupGuardPages(void* pointer, size_t size);
    bool checkGuardPages(void* pointer, size_t size);
    
    // Statistics helpers
    void updateAllocationStatistics(const MemoryAllocationRecord& record);
    void updateDeallocationStatistics(const MemoryAllocationRecord& record);
    void updatePeakStatistics();
    void updateFragmentationStatistics();
    void updateThreadStatistics(const MemoryAllocationRecord& record, bool isAllocation);
    void calculateOptimizationSuggestions();
    
    // Thread safety
    mutable std::mutex mutex_;
    
private:
    // Configuration
    MemoryTrackerConfig config_;
    
    // Tracking data
    std::unordered_map<void*, MemoryAllocationRecord> allocationRecords_;
    std::map<uint64_t, ThreadAllocationInfo> threadInfo_;
    MemoryStatistics statistics_;
    MemoryLeakReport leakReport_;
    MemoryCorruptionReport corruptionReport_;
    
    // Global tracking
    bool globalTrackingEnabled_;
    Allocator* globalAllocator_;
    
    // Real-time monitoring
    std::thread monitoringThread_;
    std::atomic<bool> monitoringActive_;
    std::function<void(const MemoryStatistics&)> monitoringCallback_;
    
    // Callbacks
    AllocationCallback allocationCallback_;
    DeallocationCallback deallocationCallback_;
    LeakDetectedCallback leakDetectedCallback_;
    CorruptionDetectedCallback corruptionDetectedCallback_;
    StatisticsUpdateCallback statisticsUpdateCallback_;
    
    // Identification
    std::atomic<uint64_t> nextAllocationId_;
    uint64_t trackerId_;
    
    // Optimization
    std::vector<std::string> optimizationSuggestions_;
    bool suggestionsCalculated_;
    
    // Disabled operations
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;
    
    // Singleton instance
    static std::unique_ptr<MemoryTracker> instance_;
    static std::mutex instanceMutex_;
};

// Memory tracker utilities
namespace MemoryTrackerUtils {
    // Analysis utilities
    struct AllocationPattern {
        size_t averageSize;
        size_t medianSize;
        size_t modeSize;
        double sizeVariance;
        size_t allocationFrequency;
        size_t deallocationFrequency;
        double averageLifetime;
        std::vector<size_t> sizeDistribution;
        std::map<std::string, size_t> locationDistribution;
    };
    
    AllocationPattern analyzeAllocationPattern(const MemoryTracker& tracker);
    AllocationPattern analyzeAllocationPattern(const MemoryTracker& tracker, uint64_t threadId);
    AllocationPattern analyzeAllocationPattern(const MemoryTracker& tracker, const std::string& location);
    
    // Memory usage analysis
    struct MemoryUsageAnalysis {
        size_t totalMemory;
        size_t usedMemory;
        size_t peakMemory;
        double utilizationRatio;
        double fragmentationRatio;
        size_t allocationOverhead;
        std::vector<std::pair<uint64_t, size_t>> memoryOverTime;
        std::vector<std::pair<std::string, size_t>> memoryByLocation;
        std::vector<std::pair<uint64_t, size_t>> memoryByThread;
    };
    
    MemoryUsageAnalysis analyzeMemoryUsage(const MemoryTracker& tracker);
    void dumpMemoryUsageAnalysis(const MemoryTracker& tracker, const std::string& filename = "");
    std::string getMemoryUsageAnalysisString(const MemoryTracker& tracker);
    
    // Performance analysis
    struct PerformanceMetrics {
        double allocationSpeed; // allocations per second
        double deallocationSpeed; // deallocations per second
        double averageAllocationTime; // microseconds
        double averageDeallocationTime; // microseconds
        double trackingOverhead; // percentage
        uint32_t trackingFailures;
        uint32_t validationFailures;
    };
    
    PerformanceMetrics analyzePerformance(const MemoryTracker& tracker);
    void dumpPerformanceMetrics(const MemoryTracker& tracker, const std::string& filename = "");
    std::string getPerformanceMetricsString(const MemoryTracker& tracker);
    
    // Comparison utilities
    void compareTrackers(const MemoryTracker& tracker1, const MemoryTracker& tracker2);
    void mergeTrackers(MemoryTracker& target, const MemoryTracker& source);
    void copyTrackerConfiguration(MemoryTracker& target, const MemoryTracker& source);
    
    // Debugging utilities
    void validateMemoryTracker(const MemoryTracker& tracker);
    void dumpMemoryTrackerState(const MemoryTracker& tracker);
    std::string dumpMemoryTrackerStateToString(const MemoryTracker& tracker);
    bool detectMemoryTrackerCorruption(const MemoryTracker& tracker);
    void repairMemoryTrackerCorruption(MemoryTracker& tracker);
    
    // Report generation
    std::string generateHTMLReport(const MemoryTracker& tracker);
    std::string generateJSONReport(const MemoryTracker& tracker);
    std::string generateXMLReport(const MemoryTracker& tracker);
    std::string generateCSVReport(const MemoryTracker& tracker);
    void generateReportFile(const MemoryTracker& tracker, const std::string& filename, const std::string& format);
}

// RAII memory tracking
class TrackedAllocation {
public:
    explicit TrackedAllocation(size_t size, size_t alignment = Alignment::DEFAULT_ALIGNMENT, 
                             uint32_t flags = static_cast<uint32_t>(AllocationFlags::NONE),
                             const char* file = nullptr, int line = 0, const char* function = nullptr);
    
    ~TrackedAllocation();
    
    void* get() const { return pointer_; }
    template<typename T> T* getAs() const { return static_cast<T*>(pointer_); }
    size_t getSize() const { return size_; }
    size_t getAlignment() const { return alignment_; }
    bool isValid() const { return pointer_ != nullptr; }
    
    // Move constructor and assignment
    TrackedAllocation(TrackedAllocation&& other) noexcept;
    TrackedAllocation& operator=(TrackedAllocation&& other) noexcept;
    
private:
    void* pointer_;
    size_t size_;
    size_t alignment_;
    uint32_t flags_;
    
    // Disabled copy operations
    TrackedAllocation(const TrackedAllocation&) = delete;
    TrackedAllocation& operator=(const TrackedAllocation&) = delete;
};

// Memory tracker macros for convenience
#ifdef RF_MEMORY_TRACKING
    #define RF_TRACKED_ALLOCATE(size) \
        TrackedAllocation _tracked_alloc(size, Alignment::DEFAULT_ALIGNMENT, \
            static_cast<uint32_t>(AllocationFlags::NONE), __FILE__, __LINE__, __FUNCTION__)
    
    #define RF_TRACKED_ALLOCATE_ALIGNED(size, alignment) \
        TrackedAllocation _tracked_alloc(size, alignment, \
            static_cast<uint32_t>(AllocationFlags::NONE), __FILE__, __LINE__, __FUNCTION__)
    
    #define RF_TRACKED_ALLOCATE_NAMED(size, name) \
        TrackedAllocation name(size, Alignment::DEFAULT_ALIGNMENT, \
            static_cast<uint32_t>(AllocationFlags::NONE), __FILE__, __LINE__, __FUNCTION__)
    
    #define RF_TRACKED_ALLOCATE_ALIGNED_NAMED(size, alignment, name) \
        TrackedAllocation name(size, alignment, \
            static_cast<uint32_t>(AllocationFlags::NONE), __FILE__, __LINE__, __FUNCTION__)
#else
    #define RF_TRACKED_ALLOCATE(size) \
        TrackedAllocation()
    
    #define RF_TRACKED_ALLOCATE_ALIGNED(size, alignment) \
        TrackedAllocation()
    
    #define RF_TRACKED_ALLOCATE_NAMED(size, name) \
        TrackedAllocation name()
    
    #define RF_TRACKED_ALLOCATE_ALIGNED_NAMED(size, alignment, name) \
        TrackedAllocation name()
#endif

// Global memory tracking functions
namespace GlobalMemoryTracking {
    void enable();
    void disable();
    bool isEnabled();
    void setAllocator(Allocator* allocator);
    Allocator* getAllocator();
    MemoryTracker& getTracker();
    void dumpFullReport(const std::string& filename = "");
    void checkForLeaks();
    void checkForCorruption();
    void reset();
}

} // namespace RFMemory