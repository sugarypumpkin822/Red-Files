#include "rf_memory_tracker.h"
#include "rf_logger.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

// Global memory tracker instance
static RF_MemoryTracker g_tracker = {0};

// Allocation record structure
typedef struct RF_AllocationRecord
{
    void* pointer;
    size_t size;
    const char* file;
    int line;
    const char* function;
    uint64_t timestamp;
    uint32_t magic;
    struct RF_AllocationRecord* next;
    struct RF_AllocationRecord* prev;
} RF_AllocationRecord;

#define RF_ALLOCATION_MAGIC 0xABCDEF01
#define RF_ALLOCATION_FREE_MAGIC 0xDEADBEEF

// Hash table for fast lookups
#define RF_HASH_TABLE_SIZE 1024
static RF_AllocationRecord* g_hashTable[RF_HASH_TABLE_SIZE] = {NULL};

// Utility functions
static uint32_t HashPointer(void* ptr);
static RF_AllocationRecord* FindRecord(void* ptr);
static void AddRecord(RF_AllocationRecord* record);
static void RemoveRecord(RF_AllocationRecord* record);
static uint64_t GetTimestamp();

bool RF_MemoryTracker_Initialize(RF_MemoryTracker* tracker)
{
    if (!tracker)
    {
        RF_LOG_ERROR("RF_MemoryTracker: Cannot initialize null tracker");
        return false;
    }

    // Initialize tracker
    tracker->totalAllocations = 0;
    tracker->totalDeallocations = 0;
    tracker->currentAllocations = 0;
    tracker->peakAllocations = 0;
    tracker->totalBytesAllocated = 0;
    tracker->totalBytesDeallocated = 0;
    tracker->currentBytesAllocated = 0;
    tracker->peakBytesAllocated = 0;
    tracker->allocationRecordCount = 0;
    tracker->enabled = true;
    tracker->trackingLevel = RF_TRACKING_LEVEL_NORMAL;
    
    // Initialize hash table
    memset(g_hashTable, 0, sizeof(g_hashTable));

    RF_LOG_INFO("RF_MemoryTracker: Initialized memory tracking");
    return true;
}

void RF_MemoryTracker_Destroy(RF_MemoryTracker* tracker)
{
    if (!tracker)
    {
        RF_LOG_WARNING("RF_MemoryTracker: Cannot destroy null tracker");
        return;
    }

    if (tracker->currentAllocations > 0)
    {
        RF_LOG_WARNING("RF_MemoryTracker: Destroying tracker with ", 
                      tracker->currentAllocations, " allocations still active");
        RF_MemoryTracker_ReportLeaks(tracker);
    }

    // Clear all tracking data
    memset(tracker, 0, sizeof(RF_MemoryTracker));
    memset(g_hashTable, 0, sizeof(g_hashTable));

    RF_LOG_INFO("RF_MemoryTracker: Destroyed memory tracker");
}

void RF_MemoryTracker_TrackAllocation(RF_MemoryTracker* tracker, 
                                    void* ptr, size_t size,
                                    const char* file, int line, const char* function)
{
    if (!tracker || !tracker->enabled || !ptr)
        return;

    if (tracker->trackingLevel == RF_TRACKING_LEVEL_NONE)
        return;

    // Create allocation record
    RF_AllocationRecord* record = (RF_AllocationRecord*)malloc(sizeof(RF_AllocationRecord));
    if (!record)
    {
        RF_LOG_ERROR("RF_MemoryTracker: Failed to allocate record for tracking");
        return;
    }

    // Fill record
    record->pointer = ptr;
    record->size = size;
    record->file = file ? strdup(file) : NULL;
    record->line = line;
    record->function = function ? strdup(function) : NULL;
    record->timestamp = GetTimestamp();
    record->magic = RF_ALLOCATION_MAGIC;
    record->next = NULL;
    record->prev = NULL;

    // Add to hash table
    AddRecord(record);

    // Update statistics
    tracker->totalAllocations++;
    tracker->currentAllocations++;
    tracker->totalBytesAllocated += size;
    tracker->currentBytesAllocated += size;
    tracker->allocationRecordCount++;

    if (tracker->currentAllocations > tracker->peakAllocations)
    {
        tracker->peakAllocations = tracker->currentAllocations;
    }

    if (tracker->currentBytesAllocated > tracker->peakBytesAllocated)
    {
        tracker->peakBytesAllocated = tracker->currentBytesAllocated;
    }

    RF_LOG_DEBUG("RF_MemoryTracker: Tracked allocation ", ptr, " (", size, 
                " bytes) at ", file, ":", line, " in ", function);
}

void RF_MemoryTracker_TrackDeallocation(RF_MemoryTracker* tracker, void* ptr)
{
    if (!tracker || !tracker->enabled || !ptr)
        return;

    if (tracker->trackingLevel == RF_TRACKING_LEVEL_NONE)
        return;

    // Find allocation record
    RF_AllocationRecord* record = FindRecord(ptr);
    if (!record)
    {
        RF_LOG_WARNING("RF_MemoryTracker: Attempted to track deallocation of untracked pointer ", ptr);
        return;
    }

    // Validate magic
    if (record->magic != RF_ALLOCATION_MAGIC)
    {
        RF_LOG_ERROR("RF_MemoryTracker: Corrupted allocation record for pointer ", ptr);
        return;
    }

    size_t size = record->size;

    // Update statistics
    tracker->totalDeallocations++;
    tracker->currentAllocations--;
    tracker->totalBytesDeallocated += size;
    tracker->currentBytesAllocated -= size;
    tracker->allocationRecordCount--;

    // Remove from hash table
    RemoveRecord(record);

    // Free record data
    if (record->file)
        free((void*)record->file);
    if (record->function)
        free((void*)record->function);
    
    // Mark as free and free record
    record->magic = RF_ALLOCATION_FREE_MAGIC;
    free(record);

    RF_LOG_DEBUG("RF_MemoryTracker: Tracked deallocation ", ptr, " (", size, " bytes)");
}

void RF_MemoryTracker_SetEnabled(RF_MemoryTracker* tracker, bool enabled)
{
    if (!tracker)
        return;

    tracker->enabled = enabled;
    RF_LOG_INFO("RF_MemoryTracker: ", enabled ? "Enabled" : "Disabled", " memory tracking");
}

void RF_MemoryTracker_SetTrackingLevel(RF_MemoryTracker* tracker, RF_TrackingLevel level)
{
    if (!tracker)
        return;

    tracker->trackingLevel = level;
    const char* levelNames[] = {"None", "Minimal", "Normal", "Verbose", "Debug"};
    RF_LOG_INFO("RF_MemoryTracker: Set tracking level to ", levelNames[level]);
}

bool RF_MemoryTracker_IsEnabled(RF_MemoryTracker* tracker)
{
    if (!tracker)
        return false;
    return tracker->enabled;
}

RF_TrackingLevel RF_MemoryTracker_GetTrackingLevel(RF_MemoryTracker* tracker)
{
    if (!tracker)
        return RF_TRACKING_LEVEL_NONE;
    return tracker->trackingLevel;
}

uint32_t RF_MemoryTracker_GetTotalAllocations(RF_MemoryTracker* tracker)
{
    if (!tracker)
        return 0;
    return tracker->totalAllocations;
}

uint32_t RF_MemoryTracker_GetTotalDeallocations(RF_MemoryTracker* tracker)
{
    if (!tracker)
        return 0;
    return tracker->totalDeallocations;
}

uint32_t RF_MemoryTracker_GetCurrentAllocations(RF_MemoryTracker* tracker)
{
    if (!tracker)
        return 0;
    return tracker->currentAllocations;
}

uint32_t RF_MemoryTracker_GetPeakAllocations(RF_MemoryTracker* tracker)
{
    if (!tracker)
        return 0;
    return tracker->peakAllocations;
}

size_t RF_MemoryTracker_GetTotalBytesAllocated(RF_MemoryTracker* tracker)
{
    if (!tracker)
        return 0;
    return tracker->totalBytesAllocated;
}

size_t RF_MemoryTracker_GetTotalBytesDeallocated(RF_MemoryTracker* tracker)
{
    if (!tracker)
        return 0;
    return tracker->totalBytesDeallocated;
}

size_t RF_MemoryTracker_GetCurrentBytesAllocated(RF_MemoryTracker* tracker)
{
    if (!tracker)
        return 0;
    return tracker->currentBytesAllocated;
}

size_t RF_MemoryTracker_GetPeakBytesAllocated(RF_MemoryTracker* tracker)
{
    if (!tracker)
        return 0;
    return tracker->peakBytesAllocated;
}

float RF_MemoryTracker_GetAverageAllocationSize(RF_MemoryTracker* tracker)
{
    if (!tracker || tracker->totalAllocations == 0)
        return 0.0f;
    return (float)tracker->totalBytesAllocated / (float)tracker->totalAllocations;
}

float RF_MemoryTracker_GetAllocationEfficiency(RF_MemoryTracker* tracker)
{
    if (!tracker || tracker->totalAllocations == 0)
        return 0.0f;
    return (float)tracker->totalDeallocations / (float)tracker->totalAllocations;
}

void RF_MemoryTracker_PrintStatistics(RF_MemoryTracker* tracker)
{
    if (!tracker)
    {
        RF_LOG_WARNING("RF_MemoryTracker: Cannot print stats for null tracker");
        return;
    }

    RF_LOG_INFO("=== Memory Tracker Statistics ===");
    RF_LOG_INFO("Enabled: ", tracker->enabled ? "Yes" : "No");
    RF_LOG_INFO("Tracking Level: ", (int)tracker->trackingLevel);
    RF_LOG_INFO("Total Allocations: ", tracker->totalAllocations);
    RF_LOG_INFO("Total Deallocations: ", tracker->totalDeallocations);
    RF_LOG_INFO("Current Allocations: ", tracker->currentAllocations);
    RF_LOG_INFO("Peak Allocations: ", tracker->peakAllocations);
    RF_LOG_INFO("Total Bytes Allocated: ", tracker->totalBytesAllocated);
    RF_LOG_INFO("Total Bytes Deallocated: ", tracker->totalBytesDeallocated);
    RF_LOG_INFO("Current Bytes Allocated: ", tracker->currentBytesAllocated);
    RF_LOG_INFO("Peak Bytes Allocated: ", tracker->peakBytesAllocated);
    RF_LOG_INFO("Average Allocation Size: ", RF_MemoryTracker_GetAverageAllocationSize(tracker));
    RF_LOG_INFO("Allocation Efficiency: ", RF_MemoryTracker_GetAllocationEfficiency(tracker) * 100.0f, "%");
    RF_LOG_INFO("Active Records: ", tracker->allocationRecordCount);
    RF_LOG_INFO("================================");
}

void RF_MemoryTracker_ReportLeaks(RF_MemoryTracker* tracker)
{
    if (!tracker || tracker->currentAllocations == 0)
    {
        RF_LOG_INFO("RF_MemoryTracker: No memory leaks detected");
        return;
    }

    RF_LOG_WARNING("=== Memory Leak Report ===");
    RF_LOG_WARNING("Total Leaks: ", tracker->currentAllocations, " allocations");
    RF_LOG_WARNING("Leaked Bytes: ", tracker->currentBytesAllocated);

    // Iterate through all hash table buckets
    for (uint32_t i = 0; i < RF_HASH_TABLE_SIZE; ++i)
    {
        RF_AllocationRecord* record = g_hashTable[i];
        while (record)
        {
            if (record->magic == RF_ALLOCATION_MAGIC)
            {
                RF_LOG_WARNING("Leak: ", record->pointer, " (", record->size, 
                            " bytes) allocated at ", record->file, ":", 
                            record->line, " in ", record->function);
            }
            record = record->next;
        }
    }

    RF_LOG_WARNING("==========================");
}

void RF_MemoryTracker_DumpAllocations(RF_MemoryTracker* tracker)
{
    if (!tracker)
    {
        RF_LOG_WARNING("RF_MemoryTracker: Cannot dump allocations for null tracker");
        return;
    }

    RF_LOG_INFO("=== Current Allocations Dump ===");
    RF_LOG_INFO("Total Active: ", tracker->currentAllocations);

    uint32_t count = 0;
    for (uint32_t i = 0; i < RF_HASH_TABLE_SIZE; ++i)
    {
        RF_AllocationRecord* record = g_hashTable[i];
        while (record && count < 100) // Limit to 100 for readability
        {
            if (record->magic == RF_ALLOCATION_MAGIC)
            {
                RF_LOG_INFO(count++, ": ", record->pointer, " (", record->size, 
                            " bytes) at ", record->file, ":", record->line, 
                            " in ", record->function);
            }
            record = record->next;
        }
    }

    if (tracker->currentAllocations > 100)
    {
        RF_LOG_INFO("... (", tracker->currentAllocations - 100, " more allocations)");
    }

    RF_LOG_INFO("===============================");
}

void RF_MemoryTracker_Reset(RF_MemoryTracker* tracker)
{
    if (!tracker)
        return;

    // Clear all records
    for (uint32_t i = 0; i < RF_HASH_TABLE_SIZE; ++i)
    {
        RF_AllocationRecord* record = g_hashTable[i];
        while (record)
        {
            RF_AllocationRecord* next = record->next;
            
            // Free record data
            if (record->file)
                free((void*)record->file);
            if (record->function)
                free((void*)record->function);
            
            free(record);
            record = next;
        }
        g_hashTable[i] = NULL;
    }

    // Reset statistics
    tracker->totalAllocations = 0;
    tracker->totalDeallocations = 0;
    tracker->currentAllocations = 0;
    tracker->peakAllocations = 0;
    tracker->totalBytesAllocated = 0;
    tracker->totalBytesDeallocated = 0;
    tracker->currentBytesAllocated = 0;
    tracker->peakBytesAllocated = 0;
    tracker->allocationRecordCount = 0;

    RF_LOG_INFO("RF_MemoryTracker: Reset all tracking data");
}

void RF_MemoryTracker_Validate(RF_MemoryTracker* tracker)
{
    if (!tracker)
    {
        RF_LOG_ERROR("RF_MemoryTracker: Null tracker");
        return;
    }

    uint32_t actualCount = 0;
    size_t actualBytes = 0;
    uint32_t corruptedRecords = 0;

    // Count all valid records
    for (uint32_t i = 0; i < RF_HASH_TABLE_SIZE; ++i)
    {
        RF_AllocationRecord* record = g_hashTable[i];
        while (record)
        {
            if (record->magic == RF_ALLOCATION_MAGIC)
            {
                actualCount++;
                actualBytes += record->size;
            }
            else if (record->magic != RF_ALLOCATION_FREE_MAGIC)
            {
                corruptedRecords++;
                RF_LOG_ERROR("RF_MemoryTracker: Corrupted record found at ", record);
            }
            record = record->next;
        }
    }

    // Validate counts
    if (actualCount != tracker->currentAllocations)
    {
        RF_LOG_ERROR("RF_MemoryTracker: Allocation count mismatch. Expected: ", 
                    tracker->currentAllocations, ", Actual: ", actualCount);
    }

    if (actualBytes != tracker->currentBytesAllocated)
    {
        RF_LOG_ERROR("RF_MemoryTracker: Byte count mismatch. Expected: ", 
                    tracker->currentBytesAllocated, ", Actual: ", actualBytes);
    }

    if (corruptedRecords > 0)
    {
        RF_LOG_ERROR("RF_MemoryTracker: Found ", corruptedRecords, " corrupted records");
    }

    if (actualCount == tracker->currentAllocations && 
        actualBytes == tracker->currentBytesAllocated && 
        corruptedRecords == 0)
    {
        RF_LOG_DEBUG("RF_MemoryTracker: Validation passed");
    }
}

RF_MemoryTracker* RF_MemoryTracker_GetGlobal(void)
{
    if (!g_tracker.enabled)
    {
        RF_MemoryTracker_Initialize(&g_tracker);
    }
    return &g_tracker;
}

// Convenience macros implementation
void* RF_TrackedMalloc(size_t size, const char* file, int line, const char* function)
{
    void* ptr = malloc(size);
    if (ptr)
    {
        RF_MemoryTracker_TrackAllocation(RF_MemoryTracker_GetGlobal(), 
                                       ptr, size, file, line, function);
    }
    return ptr;
}

void* RF_TrackedRealloc(void* ptr, size_t newSize, const char* file, int line, const char* function)
{
    RF_MemoryTracker* tracker = RF_MemoryTracker_GetGlobal();
    
    // Track deallocation of old pointer
    if (ptr)
    {
        RF_MemoryTracker_TrackDeallocation(tracker, ptr);
    }

    // Allocate new memory
    void* newPtr = realloc(ptr, newSize);
    if (newPtr)
    {
        RF_MemoryTracker_TrackAllocation(tracker, newPtr, newSize, file, line, function);
    }

    return newPtr;
}

void RF_TrackedFree(void* ptr, const char* file, int line, const char* function)
{
    if (ptr)
    {
        RF_MemoryTracker_TrackDeallocation(RF_MemoryTracker_GetGlobal(), ptr);
        free(ptr);
    }
}

// Utility function implementations
static uint32_t HashPointer(void* ptr)
{
    uintptr_t addr = (uintptr_t)ptr;
    return (uint32_t)((addr >> 3) % RF_HASH_TABLE_SIZE);
}

static RF_AllocationRecord* FindRecord(void* ptr)
{
    uint32_t hash = HashPointer(ptr);
    RF_AllocationRecord* record = g_hashTable[hash];
    
    while (record)
    {
        if (record->pointer == ptr && record->magic == RF_ALLOCATION_MAGIC)
        {
            return record;
        }
        record = record->next;
    }
    
    return NULL;
}

static void AddRecord(RF_AllocationRecord* record)
{
    uint32_t hash = HashPointer(record->pointer);
    
    // Add to head of list
    record->next = g_hashTable[hash];
    record->prev = NULL;
    
    if (g_hashTable[hash])
    {
        g_hashTable[hash]->prev = record;
    }
    
    g_hashTable[hash] = record;
}

static void RemoveRecord(RF_AllocationRecord* record)
{
    uint32_t hash = HashPointer(record->pointer);
    
    if (record->prev)
    {
        record->prev->next = record->next;
    }
    else
    {
        // Record is head of list
        g_hashTable[hash] = record->next;
    }
    
    if (record->next)
    {
        record->next->prev = record->prev;
    }
}

static uint64_t GetTimestamp()
{
    // Simple timestamp implementation
    // In a real implementation, this would use high-resolution timers
    static uint64_t counter = 0;
    return ++counter;
}

// Advanced analysis functions
void RF_MemoryTracker_AnalyzePatterns(RF_MemoryTracker* tracker)
{
    if (!tracker || tracker->currentAllocations == 0)
    {
        RF_LOG_INFO("RF_MemoryTracker: No allocations to analyze");
        return;
    }

    RF_LOG_INFO("=== Memory Allocation Pattern Analysis ===");

    // Size distribution analysis
    size_t sizeBuckets[8] = {0}; // 0-16, 17-64, 65-256, 257-1K, 1K-4K, 4K-16K, 16K-64K, 64K+
    const char* sizeLabels[] = {"0-16B", "17-64B", "65-256B", "257-1KB", 
                               "1KB-4KB", "4KB-16KB", "16KB-64KB", "64KB+"};

    // File/location analysis
    uint32_t locationCounts[10] = {0};
    const char* locationFiles[10] = {NULL};
    uint32_t locationIndex = 0;

    for (uint32_t i = 0; i < RF_HASH_TABLE_SIZE; ++i)
    {
        RF_AllocationRecord* record = g_hashTable[i];
        while (record)
        {
            if (record->magic == RF_ALLOCATION_MAGIC)
            {
                // Size distribution
                size_t size = record->size;
                int bucket = 0;
                if (size <= 16) bucket = 0;
                else if (size <= 64) bucket = 1;
                else if (size <= 256) bucket = 2;
                else if (size <= 1024) bucket = 3;
                else if (size <= 4096) bucket = 4;
                else if (size <= 16384) bucket = 5;
                else if (size <= 65536) bucket = 6;
                else bucket = 7;
                
                sizeBuckets[bucket]++;

                // Location analysis (simplified)
                if (record->file && locationIndex < 10)
                {
                    bool found = false;
                    for (uint32_t j = 0; j < locationIndex; ++j)
                    {
                        if (locationFiles[j] && strcmp(locationFiles[j], record->file) == 0)
                        {
                            locationCounts[j]++;
                            found = true;
                            break;
                        }
                    }
                    
                    if (!found)
                    {
                        locationFiles[locationIndex] = record->file;
                        locationCounts[locationIndex] = 1;
                        locationIndex++;
                    }
                }
            }
            record = record->next;
        }
    }

    // Print size distribution
    RF_LOG_INFO("Size Distribution:");
    for (int i = 0; i < 8; ++i)
    {
        if (sizeBuckets[i] > 0)
        {
            RF_LOG_INFO("  ", sizeLabels[i], ": ", sizeBuckets[i], " allocations (", 
                        (float)sizeBuckets[i] / tracker->currentAllocations * 100.0f, "%)");
        }
    }

    // Print top locations
    RF_LOG_INFO("Top Allocation Locations:");
    for (uint32_t i = 0; i < locationIndex; ++i)
    {
        RF_LOG_INFO("  ", locationFiles[i], ": ", locationCounts[i], " allocations");
    }

    RF_LOG_INFO("========================================");
}

void RF_MemoryTracker_GenerateReport(RF_MemoryTracker* tracker, const char* filename)
{
    if (!tracker)
        return;

    FILE* file = fopen(filename, "w");
    if (!file)
    {
        RF_LOG_ERROR("RF_MemoryTracker: Failed to open report file ", filename);
        return;
    }

    fprintf(file, "=== Memory Tracker Report ===\n");
    fprintf(file, "Generated at: %llu\n", GetTimestamp());
    fprintf(file, "Enabled: %s\n", tracker->enabled ? "Yes" : "No");
    fprintf(file, "Tracking Level: %d\n", (int)tracker->trackingLevel);
    fprintf(file, "\n");

    fprintf(file, "Statistics:\n");
    fprintf(file, "  Total Allocations: %u\n", tracker->totalAllocations);
    fprintf(file, "  Total Deallocations: %u\n", tracker->totalDeallocations);
    fprintf(file, "  Current Allocations: %u\n", tracker->currentAllocations);
    fprintf(file, "  Peak Allocations: %u\n", tracker->peakAllocations);
    fprintf(file, "  Total Bytes Allocated: %zu\n", tracker->totalBytesAllocated);
    fprintf(file, "  Total Bytes Deallocated: %zu\n", tracker->totalBytesDeallocated);
    fprintf(file, "  Current Bytes Allocated: %zu\n", tracker->currentBytesAllocated);
    fprintf(file, "  Peak Bytes Allocated: %zu\n", tracker->peakBytesAllocated);
    fprintf(file, "  Average Allocation Size: %.2f\n", RF_MemoryTracker_GetAverageAllocationSize(tracker));
    fprintf(file, "  Allocation Efficiency: %.2f%%\n", RF_MemoryTracker_GetAllocationEfficiency(tracker) * 100.0f);
    fprintf(file, "\n");

    if (tracker->currentAllocations > 0)
    {
        fprintf(file, "Current Allocations:\n");
        uint32_t count = 0;
        for (uint32_t i = 0; i < RF_HASH_TABLE_SIZE; ++i)
        {
            RF_AllocationRecord* record = g_hashTable[i];
            while (record && count < 1000) // Limit for report size
            {
                if (record->magic == RF_ALLOCATION_MAGIC)
                {
                    fprintf(file, "  %u: %p (%zu bytes) at %s:%d in %s\n",
                           count++, record->pointer, record->size,
                           record->file ? record->file : "unknown",
                           record->line,
                           record->function ? record->function : "unknown");
                }
                record = record->next;
            }
        }
        
        if (tracker->currentAllocations > 1000)
        {
            fprintf(file, "  ... (%u more allocations)\n", tracker->currentAllocations - 1000);
        }
    }

    fprintf(file, "===============================\n");
    fclose(file);

    RF_LOG_INFO("RF_MemoryTracker: Generated report to ", filename);
}