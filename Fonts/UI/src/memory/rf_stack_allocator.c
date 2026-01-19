#include "rf_stack_allocator.h"
#include "rf_logger.h"

// Stack allocation header
typedef struct RF_StackAllocationHeader
{
    size_t size;
    uint32_t marker;
} RF_StackAllocationHeader;

#define RF_STACK_MARKER_MAGIC 0xABCDEF01

bool RF_StackAllocator_Initialize(RF_StackAllocator* allocator, void* memory, size_t size)
{
    if (!allocator || !memory || size == 0)
    {
        RF_LOG_ERROR("RF_StackAllocator: Invalid parameters for initialization");
        return false;
    }

    // Validate minimum size
    if (size < sizeof(RF_StackAllocationHeader))
    {
        RF_LOG_ERROR("RF_StackAllocator: Size too small (minimum ", 
                    sizeof(RF_StackAllocationHeader), " bytes)");
        return false;
    }

    // Initialize allocator
    allocator->memory = memory;
    allocator->totalSize = size;
    allocator->usedSize = 0;
    allocator->currentPtr = memory;
    allocator->allocationCount = 0;
    allocator->peakUsage = 0;
    allocator->markerStack = NULL;
    allocator->markerCount = 0;
    allocator->peakMarkerCount = 0;

    RF_LOG_INFO("RF_StackAllocator: Initialized with ", size, " bytes at ", memory);
    return true;
}

void RF_StackAllocator_Destroy(RF_StackAllocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_WARNING("RF_StackAllocator: Cannot destroy null allocator");
        return;
    }

    if (allocator->usedSize > 0)
    {
        RF_LOG_WARNING("RF_StackAllocator: Destroying allocator with ", 
                      allocator->usedSize, " bytes still allocated");
    }

    if (allocator->markerCount > 0)
    {
        RF_LOG_WARNING("RF_StackAllocator: Destroying allocator with ", 
                      allocator->markerCount, " markers still active");
    }

    // Clear all data
    allocator->memory = NULL;
    allocator->totalSize = 0;
    allocator->usedSize = 0;
    allocator->currentPtr = NULL;
    allocator->allocationCount = 0;
    allocator->peakUsage = 0;
    allocator->markerStack = NULL;
    allocator->markerCount = 0;
    allocator->peakMarkerCount = 0;

    RF_LOG_INFO("RF_StackAllocator: Destroyed");
}

void* RF_StackAllocator_Allocate(RF_StackAllocator* allocator, size_t size, size_t alignment)
{
    if (!allocator || !allocator->memory)
    {
        RF_LOG_ERROR("RF_StackAllocator: Invalid allocator or memory");
        return NULL;
    }

    if (size == 0)
    {
        RF_LOG_WARNING("RF_StackAllocator: Attempted to allocate 0 bytes");
        return NULL;
    }

    // Calculate aligned address and header
    uintptr_t currentAddr = (uintptr_t)allocator->currentPtr;
    uintptr_t alignedAddr = RF_AlignUp((void*)(currentAddr + sizeof(RF_StackAllocationHeader)), alignment);
    size_t headerOffset = alignedAddr - currentAddr;
    size_t totalSize = headerOffset + sizeof(RF_StackAllocationHeader) + size;

    // Check if we have enough space
    if (allocator->usedSize + totalSize > allocator->totalSize)
    {
        RF_LOG_ERROR("RF_StackAllocator: Out of memory. Need ", totalSize, 
                    " bytes, have ", allocator->totalSize - allocator->usedSize, " bytes available");
        return NULL;
    }

    // Set up header
    RF_StackAllocationHeader* header = (RF_StackAllocationHeader*)(alignedAddr - sizeof(RF_StackAllocationHeader));
    header->size = totalSize;
    header->marker = RF_STACK_MARKER_MAGIC;

    // Update allocator state
    void* ptr = (void*)alignedAddr;
    allocator->currentPtr = (void*)(alignedAddr + size);
    allocator->usedSize += totalSize;
    allocator->allocationCount++;

    if (allocator->usedSize > allocator->peakUsage)
    {
        allocator->peakUsage = allocator->usedSize;
    }

    RF_LOG_DEBUG("RF_StackAllocator: Allocated ", size, " bytes (", totalSize, 
                " total with header and alignment) at ", ptr);
    return ptr;
}

void RF_StackAllocator_Deallocate(RF_StackAllocator* allocator, void* ptr)
{
    if (!allocator || !ptr)
    {
        RF_LOG_WARNING("RF_StackAllocator: Invalid parameters for deallocation");
        return;
    }

    // Stack allocators don't support individual deallocation
    // This is a no-op - memory is freed when resetting or popping to a marker
    RF_LOG_DEBUG("RF_StackAllocator: Individual deallocation not supported (no-op)");
}

uint32_t RF_StackAllocator_PushMarker(RF_StackAllocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_ERROR("RF_StackAllocator: Cannot push marker to null allocator");
        return 0;
    }

    // Create marker
    RF_StackMarker* marker = (RF_StackMarker*)malloc(sizeof(RF_StackMarker));
    if (!marker)
    {
        RF_LOG_ERROR("RF_StackAllocator: Failed to allocate marker");
        return 0;
    }

    // Set marker data
    marker->position = allocator->usedSize;
    marker->ptr = allocator->currentPtr;
    marker->allocationCount = allocator->allocationCount;
    marker->next = allocator->markerStack;

    // Add to marker stack
    allocator->markerStack = marker;
    allocator->markerCount++;

    if (allocator->markerCount > allocator->peakMarkerCount)
    {
        allocator->peakMarkerCount = allocator->markerCount;
    }

    uint32_t markerId = allocator->markerCount;
    RF_LOG_DEBUG("RF_StackAllocator: Pushed marker ", markerId, " at position ", marker->position);
    return markerId;
}

void RF_StackAllocator_PopToMarker(RF_StackAllocator* allocator, uint32_t markerId)
{
    if (!allocator)
    {
        RF_LOG_ERROR("RF_StackAllocator: Cannot pop to marker in null allocator");
        return;
    }

    if (markerId == 0 || markerId > allocator->markerCount)
    {
        RF_LOG_WARNING("RF_StackAllocator: Invalid marker ID ", markerId);
        return;
    }

    // Find the marker
    RF_StackMarker* targetMarker = NULL;
    RF_StackMarker* current = allocator->markerStack;
    uint32_t currentId = allocator->markerCount;

    while (current && currentId >= markerId)
    {
        if (currentId == markerId)
        {
            targetMarker = current;
            break;
        }
        current = current->next;
        currentId--;
    }

    if (!targetMarker)
    {
        RF_LOG_ERROR("RF_StackAllocator: Marker ", markerId, " not found");
        return;
    }

    // Reset to marker position
    size_t oldUsedSize = allocator->usedSize;
    uint32_t oldAllocationCount = allocator->allocationCount;

    allocator->usedSize = targetMarker->position;
    allocator->currentPtr = targetMarker->ptr;
    allocator->allocationCount = targetMarker->allocationCount;

    // Remove markers from stack
    current = allocator->markerStack;
    while (current && allocator->markerCount >= markerId)
    {
        RF_StackMarker* next = current->next;
        free(current);
        current = next;
        allocator->markerCount--;
    }

    allocator->markerStack = current;

    RF_LOG_INFO("RF_StackAllocator: Popped to marker ", markerId, 
                ", freed ", oldUsedSize - targetMarker->position, " bytes from ", 
                oldAllocationCount - targetMarker->allocationCount, " allocations");
}

void RF_StackAllocator_PopMarker(RF_StackAllocator* allocator)
{
    if (!allocator || !allocator->markerStack)
    {
        RF_LOG_WARNING("RF_StackAllocator: No marker to pop");
        return;
    }

    RF_StackMarker* marker = allocator->markerStack;
    uint32_t markerId = allocator->markerCount;

    // Reset to marker position
    size_t oldUsedSize = allocator->usedSize;
    uint32_t oldAllocationCount = allocator->allocationCount;

    allocator->usedSize = marker->position;
    allocator->currentPtr = marker->ptr;
    allocator->allocationCount = marker->allocationCount;

    // Remove marker from stack
    allocator->markerStack = marker->next;
    allocator->markerCount--;

    free(marker);

    RF_LOG_INFO("RF_StackAllocator: Popped marker ", markerId, 
                ", freed ", oldUsedSize - marker->position, " bytes from ", 
                oldAllocationCount - marker->allocationCount, " allocations");
}

void RF_StackAllocator_Reset(RF_StackAllocator* allocator)
{
    if (!allocator)
        return;

    size_t oldUsedSize = allocator->usedSize;
    uint32_t oldAllocationCount = allocator->allocationCount;
    uint32_t oldMarkerCount = allocator->markerCount;

    // Reset allocator state
    allocator->usedSize = 0;
    allocator->currentPtr = allocator->memory;
    allocator->allocationCount = 0;

    // Clear all markers
    RF_StackMarker* current = allocator->markerStack;
    while (current)
    {
        RF_StackMarker* next = current->next;
        free(current);
        current = next;
    }
    allocator->markerStack = NULL;
    allocator->markerCount = 0;

    RF_LOG_INFO("RF_StackAllocator: Reset. Freed ", oldUsedSize, " bytes from ", 
                oldAllocationCount, " allocations and ", oldMarkerCount, " markers");
}

void RF_StackAllocator_Clear(RF_StackAllocator* allocator)
{
    RF_StackAllocator_Reset(allocator);
}

size_t RF_StackAllocator_GetUsedSize(RF_StackAllocator* allocator)
{
    if (!allocator)
        return 0;
    return allocator->usedSize;
}

size_t RF_StackAllocator_GetAvailableSize(RF_StackAllocator* allocator)
{
    if (!allocator)
        return 0;
    return allocator->totalSize - allocator->usedSize;
}

size_t RF_StackAllocator_GetTotalSize(RF_StackAllocator* allocator)
{
    if (!allocator)
        return 0;
    return allocator->totalSize;
}

uint32_t RF_StackAllocator_GetAllocationCount(RF_StackAllocator* allocator)
{
    if (!allocator)
        return 0;
    return allocator->allocationCount;
}

uint32_t RF_StackAllocator_GetMarkerCount(RF_StackAllocator* allocator)
{
    if (!allocator)
        return 0;
    return allocator->markerCount;
}

float RF_StackAllocator_GetUsageRatio(RF_StackAllocator* allocator)
{
    if (!allocator || allocator->totalSize == 0)
        return 0.0f;
    return (float)allocator->usedSize / (float)allocator->totalSize;
}

void* RF_StackAllocator_GetCurrentPointer(RF_StackAllocator* allocator)
{
    if (!allocator)
        return NULL;
    return allocator->currentPtr;
}

void* RF_StackAllocator_GetBasePointer(RF_StackAllocator* allocator)
{
    if (!allocator)
        return NULL;
    return allocator->memory;
}

size_t RF_StackAllocator_GetPeakUsage(RF_StackAllocator* allocator)
{
    if (!allocator)
        return 0;
    return allocator->peakUsage;
}

uint32_t RF_StackAllocator_GetPeakMarkerCount(RF_StackAllocator* allocator)
{
    if (!allocator)
        return 0;
    return allocator->peakMarkerCount;
}

bool RF_StackAllocator_CanAllocate(RF_StackAllocator* allocator, size_t size, size_t alignment)
{
    if (!allocator || !allocator->memory)
        return false;

    uintptr_t currentAddr = (uintptr_t)allocator->currentPtr;
    uintptr_t alignedAddr = RF_AlignUp((void*)(currentAddr + sizeof(RF_StackAllocationHeader)), alignment);
    size_t headerOffset = alignedAddr - currentAddr;
    size_t totalSize = headerOffset + sizeof(RF_StackAllocationHeader) + size;

    return (allocator->usedSize + totalSize) <= allocator->totalSize;
}

void RF_StackAllocator_PrintStatistics(RF_StackAllocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_WARNING("RF_StackAllocator: Cannot print stats for null allocator");
        return;
    }

    RF_LOG_INFO("=== Stack Allocator Statistics ===");
    RF_LOG_INFO("Base Address: ", allocator->memory);
    RF_LOG_INFO("Current Pointer: ", allocator->currentPtr);
    RF_LOG_INFO("Total Size: ", allocator->totalSize, " bytes");
    RF_LOG_INFO("Used Size: ", allocator->usedSize, " bytes");
    RF_LOG_INFO("Available Size: ", allocator->totalSize - allocator->usedSize, " bytes");
    RF_LOG_INFO("Usage Ratio: ", RF_StackAllocator_GetUsageRatio(allocator) * 100.0f, "%");
    RF_LOG_INFO("Allocation Count: ", allocator->allocationCount);
    RF_LOG_INFO("Marker Count: ", allocator->markerCount);
    RF_LOG_INFO("Peak Usage: ", allocator->peakUsage, " bytes");
    RF_LOG_INFO("Peak Marker Count: ", allocator->peakMarkerCount);
    RF_LOG_INFO("Average Allocation Size: ", allocator->allocationCount > 0 ? 
                allocator->usedSize / allocator->allocationCount : 0, " bytes");
    RF_LOG_INFO("==================================");
}

void RF_StackAllocator_Validate(RF_StackAllocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_ERROR("RF_StackAllocator: Null allocator");
        return;
    }

    if (!allocator->memory)
    {
        RF_LOG_ERROR("RF_StackAllocator: Null memory pointer");
        return;
    }

    if (allocator->totalSize == 0)
    {
        RF_LOG_ERROR("RF_StackAllocator: Zero total size");
        return;
    }

    if (allocator->usedSize > allocator->totalSize)
    {
        RF_LOG_ERROR("RF_StackAllocator: Used size (", allocator->usedSize, 
                    ") exceeds total size (", allocator->totalSize, ")");
        return;
    }

    uintptr_t baseAddr = (uintptr_t)allocator->memory;
    uintptr_t currentAddr = (uintptr_t)allocator->currentPtr;
    
    if (currentAddr < baseAddr || currentAddr > baseAddr + allocator->totalSize)
    {
        RF_LOG_ERROR("RF_StackAllocator: Current pointer out of bounds");
        return;
    }

    // Validate marker chain
    uint32_t expectedCount = 0;
    RF_StackMarker* current = allocator->markerStack;
    while (current)
    {
        expectedCount++;
        
        if (current->position > allocator->usedSize)
        {
            RF_LOG_ERROR("RF_StackAllocator: Marker position out of bounds");
            return;
        }
        
        uintptr_t markerAddr = (uintptr_t)current->ptr;
        if (markerAddr < baseAddr || markerAddr > baseAddr + allocator->totalSize)
        {
            RF_LOG_ERROR("RF_StackAllocator: Marker pointer out of bounds");
            return;
        }
        
        current = current->next;
    }

    if (expectedCount != allocator->markerCount)
    {
        RF_LOG_ERROR("RF_StackAllocator: Marker count mismatch. Expected: ", 
                    expectedCount, ", Actual: ", allocator->markerCount);
        return;
    }

    RF_LOG_DEBUG("RF_StackAllocator: Validation passed");
}

void RF_StackAllocator_DumpMemory(RF_StackAllocator* allocator, const char* label)
{
    if (!allocator || !allocator->memory)
    {
        RF_LOG_WARNING("RF_StackAllocator: Cannot dump null allocator memory");
        return;
    }

    RF_LOG_INFO("=== Stack Allocator Memory Dump: ", label ? label : "Unnamed", " ===");
    RF_LOG_INFO("Base Address: ", allocator->memory);
    RF_LOG_INFO("Current Pointer: ", allocator->currentPtr);
    RF_LOG_INFO("Used Size: ", allocator->usedSize, " bytes");
    RF_LOG_INFO("Marker Count: ", allocator->markerCount);
    
    // Dump markers
    if (allocator->markerCount > 0)
    {
        RF_LOG_INFO("Markers:");
        RF_StackMarker* current = allocator->markerStack;
        uint32_t id = allocator->markerCount;
        while (current)
        {
            RF_LOG_INFO("  Marker ", id, ": position=", current->position, 
                        ", ptr=", current->ptr, ", allocations=", current->allocationCount);
            current = current->next;
            id--;
        }
    }
    
    // Dump memory content (limited)
    const uint8_t* bytes = (const uint8_t*)allocator->memory;
    for (size_t i = 0; i < allocator->usedSize && i < 256; i += 16)
    {
        RF_LOG_INFO("0x", (uintptr_t)(allocator->memory + i), ": ");
        
        // Print hex bytes
        for (size_t j = 0; j < 16 && (i + j) < allocator->usedSize && (i + j) < 256; ++j)
        {
            RF_LOG_INFO(bytes[i + j], " ");
        }
        
        // Print ASCII representation
        RF_LOG_INFO(" | ");
        for (size_t j = 0; j < 16 && (i + j) < allocator->usedSize && (i + j) < 256; ++j)
        {
            char c = bytes[i + j];
            RF_LOG_INFO((c >= 32 && c <= 126) ? c : '.');
        }
        RF_LOG_INFO("");
    }
    
    if (allocator->usedSize > 256)
    {
        RF_LOG_INFO("... (", allocator->usedSize - 256, " more bytes)");
    }
    
    RF_LOG_INFO("=== End Dump ===");
}

// Convenience functions for creating allocators with malloc
RF_StackAllocator* RF_CreateStackAllocator(size_t size)
{
    void* memory = malloc(size);
    if (!memory)
    {
        RF_LOG_ERROR("RF_StackAllocator: Failed to allocate ", size, " bytes for allocator");
        return NULL;
    }

    RF_StackAllocator* allocator = malloc(sizeof(RF_StackAllocator));
    if (!allocator)
    {
        free(memory);
        RF_LOG_ERROR("RF_StackAllocator: Failed to allocate allocator structure");
        return NULL;
    }

    if (!RF_StackAllocator_Initialize(allocator, memory, size))
    {
        free(memory);
        free(allocator);
        return NULL;
    }

    RF_LOG_INFO("RF_StackAllocator: Created allocator with malloc");
    return allocator;
}

void RF_DestroyStackAllocator(RF_StackAllocator* allocator)
{
    if (!allocator)
        return;

    if (allocator->memory)
    {
        free(allocator->memory);
    }
    
    free(allocator);
    RF_LOG_INFO("RF_StackAllocator: Destroyed allocator created with malloc");
}

// Memory alignment utilities
void* RF_AlignUp(void* ptr, size_t alignment)
{
    uintptr_t addr = (uintptr_t)ptr;
    size_t mask = alignment - 1;
    return (void*)((addr + mask) & ~mask);
}

size_t RF_AlignSize(size_t size, size_t alignment)
{
    size_t mask = alignment - 1;
    return (size + mask) & ~mask;
}

bool RF_IsAligned(const void* ptr, size_t alignment)
{
    return ((uintptr_t)ptr & (alignment - 1)) == 0;
}

// Advanced utilities
size_t RF_StackAllocator_GetLargestFreeBlock(RF_StackAllocator* allocator)
{
    if (!allocator)
        return 0;
    return allocator->totalSize - allocator->usedSize;
}

uint32_t RF_StackAllocator_GetFreeBlockCount(RF_StackAllocator* allocator)
{
    // Stack allocator has one contiguous free block
    if (!allocator || allocator->usedSize >= allocator->totalSize)
        return 0;
    return 1;
}

void RF_StackAllocator_GetMemoryMap(RF_StackAllocator* allocator, 
                                   RF_MemoryBlockInfo* blocks, uint32_t maxBlocks, 
                                   uint32_t* actualCount)
{
    if (!allocator || !blocks || !actualCount)
    {
        if (actualCount) *actualCount = 0;
        return;
    }

    *actualCount = 0;

    if (maxBlocks >= 1 && allocator->usedSize < allocator->totalSize)
    {
        // Used block
        blocks[0].address = allocator->memory;
        blocks[0].size = allocator->usedSize;
        blocks[0].isFree = false;
        (*actualCount)++;

        if (maxBlocks >= 2)
        {
            // Free block
            blocks[1].address = allocator->currentPtr;
            blocks[1].size = allocator->totalSize - allocator->usedSize;
            blocks[1].isFree = true;
            (*actualCount)++;
        }
    }
    else if (maxBlocks >= 1 && allocator->usedSize == 0)
    {
        // Entire block is free
        blocks[0].address = allocator->memory;
        blocks[0].size = allocator->totalSize;
        blocks[0].isFree = true;
        (*actualCount)++;
    }
}

void RF_StackAllocator_PrintMemoryMap(RF_StackAllocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_WARNING("RF_StackAllocator: Cannot print memory map for null allocator");
        return;
    }

    RF_LOG_INFO("=== Stack Allocator Memory Map ===");
    RF_LOG_INFO("Base: ", allocator->memory, " (", allocator->totalSize, " bytes)");
    
    if (allocator->usedSize > 0)
    {
        RF_LOG_INFO("  Used: ", allocator->memory, " - ", allocator->currentPtr, 
                    " (", allocator->usedSize, " bytes)");
    }
    
    if (allocator->usedSize < allocator->totalSize)
    {
        RF_LOG_INFO("  Free: ", allocator->currentPtr, " - ", 
                    (void*)((uintptr_t)allocator->memory + allocator->totalSize), 
                    " (", allocator->totalSize - allocator->usedSize, " bytes)");
    }
    
    // Print marker information
    if (allocator->markerCount > 0)
    {
        RF_LOG_INFO("Markers:");
        RF_StackMarker* current = allocator->markerStack;
        uint32_t id = allocator->markerCount;
        while (current)
        {
            RF_LOG_INFO("  ", id, ": ", current->ptr, " (position: ", current->position, ")");
            current = current->next;
            id--;
        }
    }
    
    RF_LOG_INFO("=================================");
}

// Fragmentation analysis (stack allocators don't fragment)
float RF_StackAllocator_GetFragmentation(RF_StackAllocator* allocator)
{
    // Stack allocators don't fragment, so fragmentation is always 0
    (void)allocator; // Suppress unused parameter warning
    return 0.0f;
}

void RF_StackAllocator_PrintFragmentationInfo(RF_StackAllocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_WARNING("RF_StackAllocator: Cannot analyze fragmentation for null allocator");
        return;
    }

    RF_LOG_INFO("=== Stack Allocator Fragmentation Analysis ===");
    RF_LOG_INFO("Fragmentation: 0.0% (Stack allocators don't fragment)");
    RF_LOG_INFO("Memory Efficiency: ", RF_StackAllocator_GetUsageRatio(allocator) * 100.0f, "%");
    RF_LOG_INFO("Marker Efficiency: ", allocator->markerCount > 0 ? 
                (float)allocator->allocationCount / allocator->markerCount : 0.0f, " allocations per marker");
    RF_LOG_INFO("===========================================");
}

// Performance monitoring
void RF_StackAllocator_ResetPerformanceCounters(RF_StackAllocator* allocator)
{
    if (!allocator)
        return;

    allocator->peakUsage = allocator->usedSize;
    allocator->peakMarkerCount = allocator->markerCount;
    
    RF_LOG_INFO("RF_StackAllocator: Reset performance counters");
}

void RF_StackAllocator_PrintPerformanceInfo(RF_StackAllocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_WARNING("RF_StackAllocator: Cannot print performance info for null allocator");
        return;
    }

    RF_LOG_INFO("=== Stack Allocator Performance ===");
    RF_LOG_INFO("Current Usage: ", allocator->usedSize, " bytes");
    RF_LOG_INFO("Peak Usage: ", allocator->peakUsage, " bytes");
    RF_LOG_INFO("Efficiency: ", allocator->peakUsage > 0 ? 
                (float)allocator->usedSize / allocator->peakUsage * 100.0f : 0.0f, "%");
    RF_LOG_INFO("Current Markers: ", allocator->markerCount);
    RF_LOG_INFO("Peak Markers: ", allocator->peakMarkerCount);
    RF_LOG_INFO("Total Memory: ", allocator->totalSize, " bytes");
    RF_LOG_INFO("Average Allocation Size: ", allocator->allocationCount > 0 ? 
                (float)allocator->usedSize / allocator->allocationCount : 0.0f, " bytes");
    RF_LOG_INFO("===============================");
}

// Debug utilities
bool RF_StackAllocator_IsValidPointer(RF_StackAllocator* allocator, void* ptr)
{
    if (!allocator || !ptr)
        return false;

    uintptr_t baseAddr = (uintptr_t)allocator->memory;
    uintptr_t endAddr = baseAddr + allocator->totalSize;
    uintptr_t ptrAddr = (uintptr_t)ptr;

    // Check if pointer is within allocator bounds
    if (ptrAddr < baseAddr || ptrAddr >= endAddr)
        return false;

    // Check if pointer is before current pointer (allocated)
    if (ptrAddr >= (uintptr_t)allocator->currentPtr)
        return false;

    return true;
}

void RF_StackAllocator_PrintAllocationChain(RF_StackAllocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_WARNING("RF_StackAllocator: Cannot print allocation chain for null allocator");
        return;
    }

    RF_LOG_INFO("=== Allocation Chain ===");
    RF_LOG_INFO("Base: ", allocator->memory);
    RF_LOG_INFO("Current: ", allocator->currentPtr);
    
    if (allocator->usedSize == 0)
    {
        RF_LOG_INFO("No allocations");
        RF_LOG_INFO("=====================");
        return;
    }

    // Walk through allocations from the beginning
    uintptr_t current = (uintptr_t)allocator->memory;
    uint32_t allocationIndex = 0;

    while (current < (uintptr_t)allocator->currentPtr && allocationIndex < 50)
    {
        // Read header
        RF_StackAllocationHeader* header = (RF_StackAllocationHeader*)current;
        
        // Validate header magic
        if (header->marker != RF_STACK_MARKER_MAGIC)
        {
            RF_LOG_ERROR("Corrupted allocation header at ", current);
            break;
        }

        void* allocationPtr = (void*)(current + sizeof(RF_StackAllocationHeader));
        RF_LOG_INFO("Allocation ", allocationIndex++, ": ", allocationPtr, 
                    " (", header->size - sizeof(RF_StackAllocationHeader), " bytes)");

        current += header->size;
    }

    if (allocationIndex >= 50)
    {
        RF_LOG_INFO("... (more allocations)");
    }

    RF_LOG_INFO("=====================");
}