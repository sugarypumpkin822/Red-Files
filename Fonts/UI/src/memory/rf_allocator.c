#include "rf_allocator.h"
#include "rf_logger.h"

// Global allocator instance
static RF_Allocator g_allocator = {0};

// Memory tracking
static size_t g_totalAllocated = 0;
static size_t g_peakAllocated = 0;
static uint32_t g_allocationCount = 0;

// Forward declarations
static void* RF_AllocateDefault(size_t size);
static void* RF_ReallocateDefault(void* ptr, size_t newSize);
static void RF_DeallocateDefault(void* ptr);

bool RF_Allocator_Initialize(RF_Allocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_ERROR("RF_Allocator: Cannot initialize null allocator");
        return false;
    }

    // Set default allocation functions if not provided
    if (!allocator->allocate)
        allocator->allocate = RF_AllocateDefault;
    if (!allocator->reallocate)
        allocator->reallocate = RF_ReallocateDefault;
    if (!allocator->deallocate)
        allocator->deallocate = RF_DeallocateDefault;

    // Initialize tracking
    allocator->totalAllocated = 0;
    allocator->peakAllocated = 0;
    allocator->allocationCount = 0;
    allocator->userData = NULL;

    RF_LOG_INFO("RF_Allocator: Successfully initialized allocator");
    return true;
}

void RF_Allocator_Destroy(RF_Allocator* allocator)
{
    if (!allocator)
    {
        RF_LOG_WARNING("RF_Allocator: Cannot destroy null allocator");
        return;
    }

    if (allocator->totalAllocated > 0)
    {
        RF_LOG_WARNING("RF_Allocator: Destroying allocator with ", allocator->totalAllocated, 
                      " bytes still allocated");
    }

    allocator->allocate = NULL;
    allocator->reallocate = NULL;
    allocator->deallocate = NULL;
    allocator->userData = NULL;

    RF_LOG_INFO("RF_Allocator: Destroyed allocator");
}

void* RF_Allocate(RF_Allocator* allocator, size_t size)
{
    if (!allocator || !allocator->allocate)
    {
        RF_LOG_ERROR("RF_Allocator: Invalid allocator or allocate function");
        return NULL;
    }

    if (size == 0)
    {
        RF_LOG_WARNING("RF_Allocator: Attempted to allocate 0 bytes");
        return NULL;
    }

    void* ptr = allocator->allocate(allocator, size);
    if (ptr)
    {
        allocator->totalAllocated += size;
        allocator->allocationCount++;
        
        if (allocator->totalAllocated > allocator->peakAllocated)
        {
            allocator->peakAllocated = allocator->totalAllocated;
        }

        // Update global tracking
        g_totalAllocated += size;
        g_allocationCount++;
        if (g_totalAllocated > g_peakAllocated)
        {
            g_peakAllocated = g_totalAllocated;
        }

        RF_LOG_DEBUG("RF_Allocator: Allocated ", size, " bytes at ", ptr);
    }
    else
    {
        RF_LOG_ERROR("RF_Allocator: Failed to allocate ", size, " bytes");
    }

    return ptr;
}

void* RF_Reallocate(RF_Allocator* allocator, void* ptr, size_t newSize)
{
    if (!allocator || !allocator->reallocate)
    {
        RF_LOG_ERROR("RF_Allocator: Invalid allocator or reallocate function");
        return NULL;
    }

    if (newSize == 0)
    {
        RF_LOG_WARNING("RF_Allocator: Attempted to reallocate to 0 bytes, deallocating instead");
        if (ptr)
        {
            RF_Deallocate(allocator, ptr);
        }
        return NULL;
    }

    // Get old size for tracking (this is approximate since we don't store sizes)
    size_t oldSize = ptr ? 0 : 0; // In a real implementation, we'd track this

    void* newPtr = allocator->reallocate(allocator, ptr, newSize);
    if (newPtr)
    {
        // Update tracking
        if (ptr)
        {
            // Reallocation - adjust totals
            allocator->totalAllocated = allocator->totalAllocated - oldSize + newSize;
            g_totalAllocated = g_totalAllocated - oldSize + newSize;
        }
        else
        {
            // New allocation
            allocator->totalAllocated += newSize;
            allocator->allocationCount++;
            g_totalAllocated += newSize;
            g_allocationCount++;
        }

        if (allocator->totalAllocated > allocator->peakAllocated)
        {
            allocator->peakAllocated = allocator->totalAllocated;
        }
        if (g_totalAllocated > g_peakAllocated)
        {
            g_peakAllocated = g_totalAllocated;
        }

        RF_LOG_DEBUG("RF_Allocator: Reallocated to ", newSize, " bytes at ", newPtr);
    }
    else
    {
        RF_LOG_ERROR("RF_Allocator: Failed to reallocate to ", newSize, " bytes");
    }

    return newPtr;
}

void RF_Deallocate(RF_Allocator* allocator, void* ptr)
{
    if (!allocator || !allocator->deallocate)
    {
        RF_LOG_ERROR("RF_Allocator: Invalid allocator or deallocate function");
        return;
    }

    if (!ptr)
    {
        RF_LOG_WARNING("RF_Allocator: Attempted to deallocate null pointer");
        return;
    }

    allocator->deallocate(allocator, ptr);
    
    // Update tracking (approximate)
    allocator->totalAllocated = 0; // In a real implementation, we'd track actual sizes
    allocator->allocationCount--;
    
    g_totalAllocated = 0; // In a real implementation, we'd track actual sizes
    g_allocationCount--;

    RF_LOG_DEBUG("RF_Allocator: Deallocated memory at ", ptr);
}

RF_Allocator* RF_GetDefaultAllocator(void)
{
    if (!g_allocator.allocate)
    {
        RF_Allocator_Initialize(&g_allocator);
    }
    return &g_allocator;
}

size_t RF_GetTotalAllocated(RF_Allocator* allocator)
{
    if (!allocator)
        return g_totalAllocated;
    return allocator->totalAllocated;
}

size_t RF_GetPeakAllocated(RF_Allocator* allocator)
{
    if (!allocator)
        return g_peakAllocated;
    return allocator->peakAllocated;
}

uint32_t RF_GetAllocationCount(RF_Allocator* allocator)
{
    if (!allocator)
        return g_allocationCount;
    return allocator->allocationCount;
}

void RF_ResetTracking(RF_Allocator* allocator)
{
    if (allocator)
    {
        allocator->totalAllocated = 0;
        allocator->peakAllocated = 0;
        allocator->allocationCount = 0;
    }
    else
    {
        g_totalAllocated = 0;
        g_peakAllocated = 0;
        g_allocationCount = 0;
    }
    
    RF_LOG_INFO("RF_Allocator: Reset memory tracking");
}

void RF_PrintMemoryStats(RF_Allocator* allocator)
{
    size_t total = RF_GetTotalAllocated(allocator);
    size_t peak = RF_GetPeakAllocated(allocator);
    uint32_t count = RF_GetAllocationCount(allocator);

    RF_LOG_INFO("=== Memory Statistics ===");
    RF_LOG_INFO("Total Allocated: ", total, " bytes");
    RF_LOG_INFO("Peak Allocated: ", peak, " bytes");
    RF_LOG_INFO("Allocation Count: ", count);
    RF_LOG_INFO("Average Size: ", count > 0 ? total / count : 0, " bytes");
    RF_LOG_INFO("========================");
}

// Default allocation functions using malloc/free
static void* RF_AllocateDefault(size_t size)
{
    return malloc(size);
}

static void* RF_ReallocateDefault(void* ptr, size_t newSize)
{
    return realloc(ptr, newSize);
}

static void RF_DeallocateDefault(void* ptr)
{
    free(ptr);
}

// Convenience functions using default allocator
void* RF_Malloc(size_t size)
{
    return RF_Allocate(RF_GetDefaultAllocator(), size);
}

void* RF_Realloc(void* ptr, size_t newSize)
{
    return RF_Reallocate(RF_GetDefaultAllocator(), ptr, newSize);
}

void RF_Free(void* ptr)
{
    RF_Deallocate(RF_GetDefaultAllocator(), ptr);
}

// Memory utilities
void* RF_Memcpy(void* dest, const void* src, size_t n)
{
    return memcpy(dest, src, n);
}

void* RF_Memmove(void* dest, const void* src, size_t n)
{
    return memmove(dest, src, n);
}

void* RF_Memset(void* ptr, int value, size_t n)
{
    return memset(ptr, value, n);
}

int RF_Memcmp(const void* ptr1, const void* ptr2, size_t n)
{
    return memcmp(ptr1, ptr2, n);
}

// Alignment utilities
bool RF_IsAligned(const void* ptr, size_t alignment)
{
    return ((uintptr_t)ptr & (alignment - 1)) == 0;
}

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

// Memory validation
bool RF_ValidatePointer(const void* ptr)
{
    if (!ptr)
        return false;
    
    // Basic validation - in a real implementation, this would be more sophisticated
    return true;
}

bool RF_ValidateMemoryRange(const void* ptr, size_t size)
{
    if (!ptr || size == 0)
        return false;
    
    // Basic validation - in a real implementation, this would check actual memory bounds
    return RF_ValidatePointer(ptr);
}

// Memory debugging
void RF_MemoryDump(const void* ptr, size_t size, const char* label)
{
    if (!ptr || size == 0)
    {
        RF_LOG_WARNING("RF_Allocator: Cannot dump null or empty memory");
        return;
    }

    RF_LOG_INFO("=== Memory Dump: ", label ? label : "Unnamed", " ===");
    RF_LOG_INFO("Address: ", ptr);
    RF_LOG_INFO("Size: ", size, " bytes");
    
    const uint8_t* bytes = (const uint8_t*)ptr;
    for (size_t i = 0; i < size; i += 16)
    {
        RF_LOG_INFO("0x", (uintptr_t)(ptr + i), ": ");
        
        // Print hex bytes
        for (size_t j = 0; j < 16 && (i + j) < size; ++j)
        {
            RF_LOG_INFO(bytes[i + j], " ");
        }
        
        // Print ASCII representation
        RF_LOG_INFO(" | ");
        for (size_t j = 0; j < 16 && (i + j) < size; ++j)
        {
            char c = bytes[i + j];
            RF_LOG_INFO((c >= 32 && c <= 126) ? c : '.');
        }
        RF_LOG_INFO("");
    }
    RF_LOG_INFO("=== End Dump ===");
}

// Memory leak detection
void RF_CheckMemoryLeaks(RF_Allocator* allocator)
{
    size_t total = RF_GetTotalAllocated(allocator);
    uint32_t count = RF_GetAllocationCount(allocator);
    
    if (total > 0 || count > 0)
    {
        RF_LOG_WARNING("=== Memory Leak Detected ===");
        RF_LOG_WARNING("Leaked bytes: ", total);
        RF_LOG_WARNING("Leaked allocations: ", count);
        RF_LOG_WARNING("==========================");
    }
    else
    {
        RF_LOG_INFO("RF_Allocator: No memory leaks detected");
    }
}

// Memory pool utilities
RF_MemoryPool* RF_CreateMemoryPool(RF_Allocator* allocator, size_t blockSize, uint32_t blockCount)
{
    if (!allocator)
        allocator = RF_GetDefaultAllocator();
    
    RF_MemoryPool* pool = (RF_MemoryPool*)RF_Allocate(allocator, sizeof(RF_MemoryPool));
    if (!pool)
        return NULL;
    
    pool->allocator = allocator;
    pool->blockSize = blockSize;
    pool->blockCount = blockCount;
    pool->usedBlocks = 0;
    pool->memory = RF_Allocate(allocator, blockSize * blockCount);
    
    if (!pool->memory)
    {
        RF_Deallocate(allocator, pool);
        return NULL;
    }
    
    // Initialize free list
    pool->freeList = pool->memory;
    for (uint32_t i = 0; i < blockCount - 1; ++i)
    {
        void** next = (void**)((uint8_t*)pool->freeList + i * blockSize);
        *next = (void*)((uint8_t*)pool->freeList + (i + 1) * blockSize);
    }
    void** last = (void**)((uint8_t*)pool->freeList + (blockCount - 1) * blockSize);
    *last = NULL;
    
    RF_LOG_INFO("RF_Allocator: Created memory pool with ", blockCount, " blocks of ", blockSize, " bytes");
    return pool;
}

void RF_DestroyMemoryPool(RF_MemoryPool* pool)
{
    if (!pool)
        return;
    
    if (pool->usedBlocks > 0)
    {
        RF_LOG_WARNING("RF_Allocator: Destroying memory pool with ", pool->usedBlocks, " blocks still in use");
    }
    
    RF_Deallocate(pool->allocator, pool->memory);
    RF_Deallocate(pool->allocator, pool);
    
    RF_LOG_INFO("RF_Allocator: Destroyed memory pool");
}

void* RF_AllocateFromPool(RF_MemoryPool* pool)
{
    if (!pool || !pool->freeList)
        return NULL;
    
    void* block = pool->freeList;
    pool->freeList = *(void**)block;
    pool->usedBlocks++;
    
    RF_LOG_DEBUG("RF_Allocator: Allocated block from pool, used: ", pool->usedBlocks, "/", pool->blockCount);
    return block;
}

void RF_DeallocateToPool(RF_MemoryPool* pool, void* block)
{
    if (!pool || !block)
        return;
    
    // Verify block is within pool bounds (basic check)
    if (block < pool->memory || block >= (uint8_t*)pool->memory + pool->blockSize * pool->blockCount)
    {
        RF_LOG_ERROR("RF_Allocator: Attempted to deallocate block outside pool bounds");
        return;
    }
    
    // Add to free list
    *(void**)block = pool->freeList;
    pool->freeList = block;
    pool->usedBlocks--;
    
    RF_LOG_DEBUG("RF_Allocator: Deallocated block to pool, used: ", pool->usedBlocks, "/", pool->blockCount);
}

bool RF_IsPoolBlock(RF_MemoryPool* pool, void* block)
{
    if (!pool || !block)
        return false;
    
    return block >= pool->memory && block < (uint8_t*)pool->memory + pool->blockSize * pool->blockCount;
}

void RF_PrintPoolStats(RF_MemoryPool* pool)
{
    if (!pool)
    {
        RF_LOG_WARNING("RF_Allocator: Cannot print stats for null pool");
        return;
    }
    
    RF_LOG_INFO("=== Memory Pool Statistics ===");
    RF_LOG_INFO("Block Size: ", pool->blockSize, " bytes");
    RF_LOG_INFO("Total Blocks: ", pool->blockCount);
    RF_LOG_INFO("Used Blocks: ", pool->usedBlocks);
    RF_LOG_INFO("Free Blocks: ", pool->blockCount - pool->usedBlocks);
    RF_LOG_INFO("Usage: ", (float)pool->usedBlocks / pool->blockCount * 100.0f, "%");
    RF_LOG_INFO("Total Memory: ", pool->blockSize * pool->blockCount, " bytes");
    RF_LOG_INFO("Used Memory: ", pool->blockSize * pool->usedBlocks, " bytes");
    RF_LOG_INFO("=============================");
}