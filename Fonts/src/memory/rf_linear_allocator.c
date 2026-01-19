#include "rf_linear_allocator.h"
#include "rf_logger.h"

bool RF_LinearAllocator_Initialize(RF_LinearAllocator* allocator, void* memory, size_t size)
{
    if (!allocator || !memory || size == 0)
    {
        RF_LOG_ERROR("RF_LinearAllocator: Invalid parameters for initialization");
        return false;
    }

    allocator->memory = memory;
    allocator->totalSize = size;
    allocator->usedSize = 0;
    allocator->currentPtr = memory;
    allocator->allocationCount = 0;

    RF_LOG_INFO("RF_LinearAllocator: Initialized with ", size, " bytes at ", memory);
    return true;
}

void RF_LinearAllocator_Destroy(RF_LinearAllocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_WARNING("RF_LinearAllocator: Cannot destroy null allocator");
        return;
    }

    if (allocator->usedSize > 0)
    {
        RF_LOG_WARNING("RF_LinearAllocator: Destroying allocator with ", allocator->usedSize, 
                      " bytes still allocated");
    }

    allocator->memory = NULL;
    allocator->totalSize = 0;
    allocator->usedSize = 0;
    allocator->currentPtr = NULL;
    allocator->allocationCount = 0;

    RF_LOG_INFO("RF_LinearAllocator: Destroyed");
}

void* RF_LinearAllocator_Allocate(RF_LinearAllocator* allocator, size_t size, size_t alignment)
{
    if (!allocator || !allocator->memory)
    {
        RF_LOG_ERROR("RF_LinearAllocator: Invalid allocator or memory");
        return NULL;
    }

    if (size == 0)
    {
        RF_LOG_WARNING("RF_LinearAllocator: Attempted to allocate 0 bytes");
        return NULL;
    }

    // Calculate aligned address
    uintptr_t currentAddr = (uintptr_t)allocator->currentPtr;
    uintptr_t alignedAddr = RF_AlignUp((void*)currentAddr, alignment);
    size_t alignmentOffset = alignedAddr - currentAddr;
    size_t totalSize = size + alignmentOffset;

    // Check if we have enough space
    if (allocator->usedSize + totalSize > allocator->totalSize)
    {
        RF_LOG_ERROR("RF_LinearAllocator: Out of memory. Need ", totalSize, 
                    " bytes, have ", allocator->totalSize - allocator->usedSize, " bytes available");
        return NULL;
    }

    // Update allocator state
    void* ptr = (void*)alignedAddr;
    allocator->currentPtr = (void*)(alignedAddr + size);
    allocator->usedSize += totalSize;
    allocator->allocationCount++;

    RF_LOG_DEBUG("RF_LinearAllocator: Allocated ", size, " bytes (", totalSize, 
                " total with alignment) at ", ptr);
    return ptr;
}

void RF_LinearAllocator_Reset(RF_LinearAllocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_WARNING("RF_LinearAllocator: Cannot reset null allocator");
        return;
    }

    size_t oldUsedSize = allocator->usedSize;
    uint32_t oldCount = allocator->allocationCount;

    allocator->usedSize = 0;
    allocator->currentPtr = allocator->memory;
    allocator->allocationCount = 0;

    RF_LOG_INFO("RF_LinearAllocator: Reset. Freed ", oldUsedSize, " bytes from ", 
                oldCount, " allocations");
}

void RF_LinearAllocator_Clear(RF_LinearAllocator* allocator)
{
    RF_LinearAllocator_Reset(allocator);
}

size_t RF_LinearAllocator_GetUsedSize(RF_LinearAllocator* allocator)
{
    if (!allocator)
        return 0;
    return allocator->usedSize;
}

size_t RF_LinearAllocator_GetAvailableSize(RF_LinearAllocator* allocator)
{
    if (!allocator)
        return 0;
    return allocator->totalSize - allocator->usedSize;
}

size_t RF_LinearAllocator_GetTotalSize(RF_LinearAllocator* allocator)
{
    if (!allocator)
        return 0;
    return allocator->totalSize;
}

uint32_t RF_LinearAllocator_GetAllocationCount(RF_LinearAllocator* allocator)
{
    if (!allocator)
        return 0;
    return allocator->allocationCount;
}

float RF_LinearAllocator_GetUsageRatio(RF_LinearAllocator* allocator)
{
    if (!allocator || allocator->totalSize == 0)
        return 0.0f;
    return (float)allocator->usedSize / (float)allocator->totalSize;
}

void* RF_LinearAllocator_GetCurrentPointer(RF_LinearAllocator* allocator)
{
    if (!allocator)
        return NULL;
    return allocator->currentPtr;
}

void* RF_LinearAllocator_GetBasePointer(RF_LinearAllocator* allocator)
{
    if (!allocator)
        return NULL;
    return allocator->memory;
}

bool RF_LinearAllocator_CanAllocate(RF_LinearAllocator* allocator, size_t size, size_t alignment)
{
    if (!allocator || !allocator->memory)
        return false;

    uintptr_t currentAddr = (uintptr_t)allocator->currentPtr;
    uintptr_t alignedAddr = RF_AlignUp((void*)currentAddr, alignment);
    size_t alignmentOffset = alignedAddr - currentAddr;
    size_t totalSize = size + alignmentOffset;

    return (allocator->usedSize + totalSize) <= allocator->totalSize;
}

void RF_LinearAllocator_PrintStats(RF_LinearAllocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_WARNING("RF_LinearAllocator: Cannot print stats for null allocator");
        return;
    }

    RF_LOG_INFO("=== Linear Allocator Statistics ===");
    RF_LOG_INFO("Base Address: ", allocator->memory);
    RF_LOG_INFO("Current Pointer: ", allocator->currentPtr);
    RF_LOG_INFO("Total Size: ", allocator->totalSize, " bytes");
    RF_LOG_INFO("Used Size: ", allocator->usedSize, " bytes");
    RF_LOG_INFO("Available Size: ", allocator->totalSize - allocator->usedSize, " bytes");
    RF_LOG_INFO("Usage Ratio: ", RF_LinearAllocator_GetUsageRatio(allocator) * 100.0f, "%");
    RF_LOG_INFO("Allocation Count: ", allocator->allocationCount);
    RF_LOG_INFO("Average Allocation Size: ", allocator->allocationCount > 0 ? 
                allocator->usedSize / allocator->allocationCount : 0, " bytes");
    RF_LOG_INFO("===================================");
}

// Convenience functions for creating allocators with malloc
RF_LinearAllocator* RF_CreateLinearAllocator(size_t size)
{
    void* memory = malloc(size);
    if (!memory)
    {
        RF_LOG_ERROR("RF_LinearAllocator: Failed to allocate ", size, " bytes for allocator");
        return NULL;
    }

    RF_LinearAllocator* allocator = malloc(sizeof(RF_LinearAllocator));
    if (!allocator)
    {
        free(memory);
        RF_LOG_ERROR("RF_LinearAllocator: Failed to allocate allocator structure");
        return NULL;
    }

    if (!RF_LinearAllocator_Initialize(allocator, memory, size))
    {
        free(memory);
        free(allocator);
        return NULL;
    }

    RF_LOG_INFO("RF_LinearAllocator: Created allocator with malloc");
    return allocator;
}

void RF_DestroyLinearAllocator(RF_LinearAllocator* allocator)
{
    if (!allocator)
        return;

    if (allocator->memory)
    {
        free(allocator->memory);
    }
    
    free(allocator);
    RF_LOG_INFO("RF_LinearAllocator: Destroyed allocator created with malloc");
}

// Stack-based allocator utilities
typedef struct RF_StackAllocatorHeader
{
    size_t size;
    uint32_t marker;
} RF_StackAllocatorHeader;

uint32_t RF_LinearAllocator_PushMarker(RF_LinearAllocator* allocator)
{
    if (!allocator)
        return 0;

    uint32_t marker = (uint32_t)allocator->usedSize;
    
    // Store marker in allocation
    RF_StackAllocatorHeader* header = RF_LinearAllocator_Allocate(allocator, 
        sizeof(RF_StackAllocatorHeader), alignof(RF_StackAllocatorHeader));
    if (header)
    {
        header->size = 0; // Marker allocation
        header->marker = marker;
    }

    RF_LOG_DEBUG("RF_LinearAllocator: Pushed marker at position ", marker);
    return marker;
}

void RF_LinearAllocator_PopToMarker(RF_LinearAllocator* allocator, uint32_t marker)
{
    if (!allocator)
        return;

    if (marker > allocator->usedSize)
    {
        RF_LOG_WARNING("RF_LinearAllocator: Marker ", marker, " is beyond current position");
        return;
    }

    size_t oldUsedSize = allocator->usedSize;
    allocator->usedSize = marker;
    allocator->currentPtr = (void*)((uintptr_t)allocator->memory + marker);

    // Recalculate allocation count (approximate)
    allocator->allocationCount = 0; // In a real implementation, we'd track this properly

    RF_LOG_INFO("RF_LinearAllocator: Popped to marker ", marker, 
                ", freed ", oldUsedSize - marker, " bytes");
}

void RF_LinearAllocator_PopMarker(RF_LinearAllocator* allocator)
{
    if (!allocator)
        return;

    // Find the last marker (simplified - in real implementation would track markers)
    RF_LinearAllocator_Reset(allocator);
    RF_LOG_INFO("RF_LinearAllocator: Popped all markers (reset)");
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

// Debug utilities
void RF_LinearAllocator_DumpMemory(RF_LinearAllocator* allocator, const char* label)
{
    if (!allocator || !allocator->memory)
    {
        RF_LOG_WARNING("RF_LinearAllocator: Cannot dump null allocator memory");
        return;
    }

    RF_LOG_INFO("=== Linear Allocator Memory Dump: ", label ? label : "Unnamed", " ===");
    RF_LOG_INFO("Base Address: ", allocator->memory);
    RF_LOG_INFO("Current Pointer: ", allocator->currentPtr);
    RF_LOG_INFO("Used Size: ", allocator->usedSize, " bytes");
    
    const uint8_t* bytes = (const uint8_t*)allocator->memory;
    for (size_t i = 0; i < allocator->usedSize && i < 256; i += 16) // Limit to 256 bytes for readability
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

// Validation utilities
bool RF_LinearAllocator_Validate(RF_LinearAllocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_ERROR("RF_LinearAllocator: Null allocator");
        return false;
    }

    if (!allocator->memory)
    {
        RF_LOG_ERROR("RF_LinearAllocator: Null memory pointer");
        return false;
    }

    if (allocator->totalSize == 0)
    {
        RF_LOG_ERROR("RF_LinearAllocator: Zero total size");
        return false;
    }

    if (allocator->usedSize > allocator->totalSize)
    {
        RF_LOG_ERROR("RF_LinearAllocator: Used size (", allocator->usedSize, 
                    ") exceeds total size (", allocator->totalSize, ")");
        return false;
    }

    uintptr_t baseAddr = (uintptr_t)allocator->memory;
    uintptr_t currentAddr = (uintptr_t)allocator->currentPtr;
    
    if (currentAddr < baseAddr || currentAddr > baseAddr + allocator->totalSize)
    {
        RF_LOG_ERROR("RF_LinearAllocator: Current pointer out of bounds");
        return false;
    }

    RF_LOG_DEBUG("RF_LinearAllocator: Validation passed");
    return true;
}

// Memory fragmentation analysis
float RF_LinearAllocator_GetFragmentation(RF_LinearAllocator* allocator)
{
    // Linear allocators don't fragment, so fragmentation is always 0
    (void)allocator; // Suppress unused parameter warning
    return 0.0f;
}

void RF_LinearAllocator_PrintFragmentationInfo(RF_LinearAllocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_WARNING("RF_LinearAllocator: Cannot analyze fragmentation for null allocator");
        return;
    }

    RF_LOG_INFO("=== Linear Allocator Fragmentation Analysis ===");
    RF_LOG_INFO("Fragmentation: 0.0% (Linear allocators don't fragment)");
    RF_LOG_INFO("Memory Efficiency: ", RF_LinearAllocator_GetUsageRatio(allocator) * 100.0f, "%");
    RF_LOG_INFO("===========================================");
}

// Advanced utilities
size_t RF_LinearAllocator_GetLargestFreeBlock(RF_LinearAllocator* allocator)
{
    if (!allocator)
        return 0;
    return allocator->totalSize - allocator->usedSize;
}

uint32_t RF_LinearAllocator_GetFreeBlockCount(RF_LinearAllocator* allocator)
{
    // Linear allocator has one contiguous free block
    if (!allocator || allocator->usedSize >= allocator->totalSize)
        return 0;
    return 1;
}

void RF_LinearAllocator_GetMemoryMap(RF_LinearAllocator* allocator, 
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

void RF_LinearAllocator_PrintMemoryMap(RF_LinearAllocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_WARNING("RF_LinearAllocator: Cannot print memory map for null allocator");
        return;
    }

    RF_LOG_INFO("=== Linear Allocator Memory Map ===");
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
    
    RF_LOG_INFO("=================================");
}