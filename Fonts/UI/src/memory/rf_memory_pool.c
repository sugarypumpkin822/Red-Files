#include "rf_memory_pool.h"
#include "rf_logger.h"

// Header structure for allocated blocks
typedef struct RF_MemoryPoolBlock
{
    struct RF_MemoryPoolBlock* next;
    uint32_t magic;
} RF_MemoryPoolBlock;

#define RF_MEMORY_POOL_MAGIC 0xDEADBEEF
#define RF_MEMORY_POOL_FREE_MAGIC 0xFEEDFACE

bool RF_MemoryPool_Initialize(RF_MemoryPool* pool, void* memory, size_t poolSize, size_t blockSize)
{
    if (!pool || !memory || poolSize == 0 || blockSize == 0)
    {
        RF_LOG_ERROR("RF_MemoryPool: Invalid parameters for initialization");
        return false;
    }

    // Validate block size
    if (blockSize < sizeof(RF_MemoryPoolBlock))
    {
        RF_LOG_ERROR("RF_MemoryPool: Block size too small (minimum ", 
                    sizeof(RF_MemoryPoolBlock), " bytes)");
        return false;
    }

    // Calculate how many blocks we can fit
    uint32_t maxBlocks = (uint32_t)(poolSize / blockSize);
    if (maxBlocks == 0)
    {
        RF_LOG_ERROR("RF_MemoryPool: Pool too small for any blocks");
        return false;
    }

    // Initialize pool
    pool->memory = memory;
    pool->poolSize = poolSize;
    pool->blockSize = blockSize;
    pool->maxBlocks = maxBlocks;
    pool->usedBlocks = 0;
    pool->freeList = NULL;
    pool->allocationCount = 0;
    pool->peakUsage = 0;

    // Build free list
    uint8_t* current = (uint8_t*)memory;
    for (uint32_t i = 0; i < maxBlocks; ++i)
    {
        RF_MemoryPoolBlock* block = (RF_MemoryPoolBlock*)current;
        block->next = pool->freeList;
        block->magic = RF_MEMORY_POOL_FREE_MAGIC;
        pool->freeList = block;
        current += blockSize;
    }

    RF_LOG_INFO("RF_MemoryPool: Initialized with ", maxBlocks, " blocks of ", 
                blockSize, " bytes each (total: ", poolSize, " bytes)");
    return true;
}

void RF_MemoryPool_Destroy(RF_MemoryPool* pool)
{
    if (!pool)
    {
        RF_LOG_WARNING("RF_MemoryPool: Cannot destroy null pool");
        return;
    }

    if (pool->usedBlocks > 0)
    {
        RF_LOG_WARNING("RF_MemoryPool: Destroying pool with ", pool->usedBlocks, 
                      " blocks still in use");
    }

    // Clear all data
    pool->memory = NULL;
    pool->poolSize = 0;
    pool->blockSize = 0;
    pool->maxBlocks = 0;
    pool->usedBlocks = 0;
    pool->freeList = NULL;
    pool->allocationCount = 0;
    pool->peakUsage = 0;

    RF_LOG_INFO("RF_MemoryPool: Destroyed");
}

void* RF_MemoryPool_Allocate(RF_MemoryPool* pool)
{
    if (!pool || !pool->freeList)
    {
        if (pool)
        {
            RF_LOG_ERROR("RF_MemoryPool: Out of memory blocks. Used: ", 
                        pool->usedBlocks, "/", pool->maxBlocks);
        }
        else
        {
            RF_LOG_ERROR("RF_MemoryPool: Invalid pool");
        }
        return NULL;
    }

    // Get block from free list
    RF_MemoryPoolBlock* block = pool->freeList;
    pool->freeList = block->next;

    // Validate block magic
    if (block->magic != RF_MEMORY_POOL_FREE_MAGIC)
    {
        RF_LOG_ERROR("RF_MemoryPool: Corrupted free block (magic: 0x", 
                    block->magic, ")");
        return NULL;
    }

    // Mark as allocated
    block->magic = RF_MEMORY_POOL_MAGIC;
    block->next = NULL;

    // Update statistics
    pool->usedBlocks++;
    pool->allocationCount++;
    
    if (pool->usedBlocks > pool->peakUsage)
    {
        pool->peakUsage = pool->usedBlocks;
    }

    void* ptr = (void*)block;
    RF_LOG_DEBUG("RF_MemoryPool: Allocated block at ", ptr, 
                " (used: ", pool->usedBlocks, "/", pool->maxBlocks, ")");
    return ptr;
}

void RF_MemoryPool_Deallocate(RF_MemoryPool* pool, void* ptr)
{
    if (!pool || !ptr)
    {
        RF_LOG_WARNING("RF_MemoryPool: Invalid parameters for deallocation");
        return;
    }

    // Validate pointer is within pool bounds
    if (!RF_MemoryPool_IsValidPointer(pool, ptr))
    {
        RF_LOG_ERROR("RF_MemoryPool: Pointer ", ptr, " is outside pool bounds");
        return;
    }

    RF_MemoryPoolBlock* block = (RF_MemoryPoolBlock*)ptr;

    // Validate block magic
    if (block->magic != RF_MEMORY_POOL_MAGIC)
    {
        if (block->magic == RF_MEMORY_POOL_FREE_MAGIC)
        {
            RF_LOG_WARNING("RF_MemoryPool: Double free detected at ", ptr);
        }
        else
        {
            RF_LOG_ERROR("RF_MemoryPool: Corrupted block (magic: 0x", 
                        block->magic, ") at ", ptr);
        }
        return;
    }

    // Mark as free and add to free list
    block->magic = RF_MEMORY_POOL_FREE_MAGIC;
    block->next = pool->freeList;
    pool->freeList = block;

    // Update statistics
    pool->usedBlocks--;

    RF_LOG_DEBUG("RF_MemoryPool: Deallocated block at ", ptr, 
                " (used: ", pool->usedBlocks, "/", pool->maxBlocks, ")");
}

bool RF_MemoryPool_IsValidPointer(RF_MemoryPool* pool, void* ptr)
{
    if (!pool || !ptr)
        return false;

    uintptr_t poolStart = (uintptr_t)pool->memory;
    uintptr_t poolEnd = poolStart + pool->poolSize;
    uintptr_t ptrAddr = (uintptr_t)ptr;

    // Check if pointer is within pool bounds
    if (ptrAddr < poolStart || ptrAddr >= poolEnd)
        return false;

    // Check if pointer is block-aligned
    size_t offset = ptrAddr - poolStart;
    if (offset % pool->blockSize != 0)
        return false;

    return true;
}

bool RF_MemoryPool_IsAllocated(RF_MemoryPool* pool, void* ptr)
{
    if (!RF_MemoryPool_IsValidPointer(pool, ptr))
        return false;

    RF_MemoryPoolBlock* block = (RF_MemoryPoolBlock*)ptr;
    return block->magic == RF_MEMORY_POOL_MAGIC;
}

size_t RF_MemoryPool_GetUsedSize(RF_MemoryPool* pool)
{
    if (!pool)
        return 0;
    return pool->usedBlocks * pool->blockSize;
}

size_t RF_MemoryPool_GetAvailableSize(RF_MemoryPool* pool)
{
    if (!pool)
        return 0;
    return (pool->maxBlocks - pool->usedBlocks) * pool->blockSize;
}

size_t RF_MemoryPool_GetTotalSize(RF_MemoryPool* pool)
{
    if (!pool)
        return 0;
    return pool->poolSize;
}

uint32_t RF_MemoryPool_GetUsedBlocks(RF_MemoryPool* pool)
{
    if (!pool)
        return 0;
    return pool->usedBlocks;
}

uint32_t RF_MemoryPool_GetAvailableBlocks(RF_MemoryPool* pool)
{
    if (!pool)
        return 0;
    return pool->maxBlocks - pool->usedBlocks;
}

uint32_t RF_MemoryPool_GetMaxBlocks(RF_MemoryPool* pool)
{
    if (!pool)
        return 0;
    return pool->maxBlocks;
}

float RF_MemoryPool_GetUsageRatio(RF_MemoryPool* pool)
{
    if (!pool || pool->maxBlocks == 0)
        return 0.0f;
    return (float)pool->usedBlocks / (float)pool->maxBlocks;
}

uint32_t RF_MemoryPool_GetPeakUsage(RF_MemoryPool* pool)
{
    if (!pool)
        return 0;
    return pool->peakUsage;
}

void RF_MemoryPool_Reset(RF_MemoryPool* pool)
{
    if (!pool)
        return;

    if (pool->usedBlocks > 0)
    {
        RF_LOG_WARNING("RF_MemoryPool: Resetting pool with ", pool->usedBlocks, 
                      " blocks still in use");
    }

    // Rebuild free list
    pool->freeList = NULL;
    pool->usedBlocks = 0;
    pool->allocationCount = 0;

    uint8_t* current = (uint8_t*)pool->memory;
    for (uint32_t i = 0; i < pool->maxBlocks; ++i)
    {
        RF_MemoryPoolBlock* block = (RF_MemoryPoolBlock*)current;
        block->next = pool->freeList;
        block->magic = RF_MEMORY_POOL_FREE_MAGIC;
        pool->freeList = block;
        current += pool->blockSize;
    }

    RF_LOG_INFO("RF_MemoryPool: Reset. All blocks are now free");
}

void RF_MemoryPool_Clear(RF_MemoryPool* pool)
{
    RF_MemoryPool_Reset(pool);
}

void RF_MemoryPool_PrintStats(RF_MemoryPool* pool)
{
    if (!pool)
    {
        RF_LOG_WARNING("RF_MemoryPool: Cannot print stats for null pool");
        return;
    }

    RF_LOG_INFO("=== Memory Pool Statistics ===");
    RF_LOG_INFO("Base Address: ", pool->memory);
    RF_LOG_INFO("Pool Size: ", pool->poolSize, " bytes");
    RF_LOG_INFO("Block Size: ", pool->blockSize, " bytes");
    RF_LOG_INFO("Max Blocks: ", pool->maxBlocks);
    RF_LOG_INFO("Used Blocks: ", pool->usedBlocks);
    RF_LOG_INFO("Available Blocks: ", pool->maxBlocks - pool->usedBlocks);
    RF_LOG_INFO("Used Size: ", pool->usedBlocks * pool->blockSize, " bytes");
    RF_LOG_INFO("Available Size: ", (pool->maxBlocks - pool->usedBlocks) * pool->blockSize, " bytes");
    RF_LOG_INFO("Usage Ratio: ", RF_MemoryPool_GetUsageRatio(pool) * 100.0f, "%");
    RF_LOG_INFO("Peak Usage: ", pool->peakUsage, " blocks");
    RF_LOG_INFO("Total Allocations: ", pool->allocationCount);
    RF_LOG_INFO("Average Block Lifetime: ", pool->allocationCount > 0 ? 
                (float)pool->allocationCount / pool->usedBlocks : 0.0f);
    RF_LOG_INFO("===============================");
}

void RF_MemoryPool_Validate(RF_MemoryPool* pool)
{
    if (!pool)
    {
        RF_LOG_ERROR("RF_MemoryPool: Null pool");
        return;
    }

    if (!pool->memory)
    {
        RF_LOG_ERROR("RF_MemoryPool: Null memory");
        return;
    }

    if (pool->blockSize == 0 || pool->maxBlocks == 0)
    {
        RF_LOG_ERROR("RF_MemoryPool: Invalid block configuration");
        return;
    }

    // Count free blocks in free list
    uint32_t freeCount = 0;
    RF_MemoryPoolBlock* current = pool->freeList;
    while (current)
    {
        freeCount++;
        
        // Validate free block magic
        if (current->magic != RF_MEMORY_POOL_FREE_MAGIC)
        {
            RF_LOG_ERROR("RF_MemoryPool: Corrupted free block (magic: 0x", 
                        current->magic, ")");
            return;
        }

        // Validate pointer is within bounds
        if (!RF_MemoryPool_IsValidPointer(pool, current))
        {
            RF_LOG_ERROR("RF_MemoryPool: Free block pointer out of bounds");
            return;
        }

        current = current->next;
    }

    // Check if counts match
    if (pool->usedBlocks + freeCount != pool->maxBlocks)
    {
        RF_LOG_ERROR("RF_MemoryPool: Block count mismatch. Used: ", 
                    pool->usedBlocks, ", Free: ", freeCount, 
                    ", Total: ", pool->maxBlocks);
        return;
    }

    RF_LOG_DEBUG("RF_MemoryPool: Validation passed");
}

void RF_MemoryPool_DumpMemory(RF_MemoryPool* pool, const char* label)
{
    if (!pool || !pool->memory)
    {
        RF_LOG_WARNING("RF_MemoryPool: Cannot dump null pool memory");
        return;
    }

    RF_LOG_INFO("=== Memory Pool Dump: ", label ? label : "Unnamed", " ===");
    RF_LOG_INFO("Base Address: ", pool->memory);
    RF_LOG_INFO("Block Size: ", pool->blockSize, " bytes");
    RF_LOG_INFO("Total Blocks: ", pool->maxBlocks);
    
    uint8_t* current = (uint8_t*)pool->memory;
    for (uint32_t i = 0; i < pool->maxBlocks; ++i)
    {
        RF_MemoryPoolBlock* block = (RF_MemoryPoolBlock*)current;
        bool isFree = (block->magic == RF_MEMORY_POOL_FREE_MAGIC);
        bool isAllocated = (block->magic == RF_MEMORY_POOL_MAGIC);
        
        RF_LOG_INFO("Block ", i, ": ", current, " - ", 
                    isFree ? "FREE" : (isAllocated ? "ALLOCATED" : "CORRUPTED"));
        
        if (!isFree && !isAllocated)
        {
            RF_LOG_WARNING("  Magic: 0x", block->magic);
        }
        
        current += pool->blockSize;
    }
    
    RF_LOG_INFO("=== End Dump ===");
}

// Convenience functions for creating pools with malloc
RF_MemoryPool* RF_CreateMemoryPool(size_t poolSize, size_t blockSize)
{
    void* memory = malloc(poolSize);
    if (!memory)
    {
        RF_LOG_ERROR("RF_MemoryPool: Failed to allocate ", poolSize, " bytes for pool");
        return NULL;
    }

    RF_MemoryPool* pool = malloc(sizeof(RF_MemoryPool));
    if (!pool)
    {
        free(memory);
        RF_LOG_ERROR("RF_MemoryPool: Failed to allocate pool structure");
        return NULL;
    }

    if (!RF_MemoryPool_Initialize(pool, memory, poolSize, blockSize))
    {
        free(memory);
        free(pool);
        return NULL;
    }

    RF_LOG_INFO("RF_MemoryPool: Created pool with malloc");
    return pool;
}

void RF_DestroyMemoryPool(RF_MemoryPool* pool)
{
    if (!pool)
        return;

    if (pool->memory)
    {
        free(pool->memory);
    }
    
    free(pool);
    RF_LOG_INFO("RF_MemoryPool: Destroyed pool created with malloc");
}

// Advanced utilities
size_t RF_MemoryPool_GetLargestFreeBlock(RF_MemoryPool* pool)
{
    if (!pool || !pool->freeList)
        return 0;
    return pool->blockSize;
}

uint32_t RF_MemoryPool_GetFreeBlockCount(RF_MemoryPool* pool)
{
    if (!pool)
        return 0;
    return pool->maxBlocks - pool->usedBlocks;
}

void RF_MemoryPool_GetMemoryMap(RF_MemoryPool* pool, 
                               RF_MemoryBlockInfo* blocks, uint32_t maxBlocks, 
                               uint32_t* actualCount)
{
    if (!pool || !blocks || !actualCount)
    {
        if (actualCount) *actualCount = 0;
        return;
    }

    *actualCount = 0;
    uint32_t blockIndex = 0;
    uint8_t* current = (uint8_t*)pool->memory;

    for (uint32_t i = 0; i < pool->maxBlocks && blockIndex < maxBlocks; ++i)
    {
        RF_MemoryPoolBlock* block = (RF_MemoryPoolBlock*)current;
        bool isFree = (block->magic == RF_MEMORY_POOL_FREE_MAGIC);
        
        blocks[blockIndex].address = current;
        blocks[blockIndex].size = pool->blockSize;
        blocks[blockIndex].isFree = isFree;
        blockIndex++;
        
        current += pool->blockSize;
    }

    *actualCount = blockIndex;
}

void RF_MemoryPool_PrintMemoryMap(RF_MemoryPool* pool)
{
    if (!pool)
    {
        RF_LOG_WARNING("RF_MemoryPool: Cannot print memory map for null pool");
        return;
    }

    RF_LOG_INFO("=== Memory Pool Memory Map ===");
    RF_LOG_INFO("Base: ", pool->memory, " (", pool->poolSize, " bytes)");
    
    uint8_t* current = (uint8_t*)pool->memory;
    uint32_t consecutiveFree = 0;
    uint32_t consecutiveUsed = 0;
    void* freeStart = NULL;
    void* usedStart = NULL;

    for (uint32_t i = 0; i < pool->maxBlocks; ++i)
    {
        RF_MemoryPoolBlock* block = (RF_MemoryPoolBlock*)current;
        bool isFree = (block->magic == RF_MEMORY_POOL_FREE_MAGIC);
        
        if (isFree)
        {
            if (consecutiveUsed > 0)
            {
                RF_LOG_INFO("  Used: ", usedStart, " - ", current, 
                            " (", consecutiveUsed * pool->blockSize, " bytes)");
                consecutiveUsed = 0;
            }
            
            if (consecutiveFree == 0)
            {
                freeStart = current;
            }
            consecutiveFree++;
        }
        else
        {
            if (consecutiveFree > 0)
            {
                RF_LOG_INFO("  Free: ", freeStart, " - ", current, 
                            " (", consecutiveFree * pool->blockSize, " bytes)");
                consecutiveFree = 0;
            }
            
            if (consecutiveUsed == 0)
            {
                usedStart = current;
            }
            consecutiveUsed++;
        }
        
        current += pool->blockSize;
    }

    // Print remaining blocks
    if (consecutiveFree > 0)
    {
        RF_LOG_INFO("  Free: ", freeStart, " - ", current, 
                    " (", consecutiveFree * pool->blockSize, " bytes)");
    }
    else if (consecutiveUsed > 0)
    {
        RF_LOG_INFO("  Used: ", usedStart, " - ", current, 
                    " (", consecutiveUsed * pool->blockSize, " bytes)");
    }
    
    RF_LOG_INFO("===============================");
}

// Fragmentation analysis (memory pools don't fragment)
float RF_MemoryPool_GetFragmentation(RF_MemoryPool* pool)
{
    // Memory pools don't fragment since all blocks are the same size
    (void)pool; // Suppress unused parameter warning
    return 0.0f;
}

void RF_MemoryPool_PrintFragmentationInfo(RF_MemoryPool* pool)
{
    if (!pool)
    {
        RF_LOG_WARNING("RF_MemoryPool: Cannot analyze fragmentation for null pool");
        return;
    }

    RF_LOG_INFO("=== Memory Pool Fragmentation Analysis ===");
    RF_LOG_INFO("Fragmentation: 0.0% (Memory pools don't fragment)");
    RF_LOG_INFO("Memory Efficiency: ", RF_MemoryPool_GetUsageRatio(pool) * 100.0f, "%");
    RF_LOG_INFO("Block Utilization: ", pool->usedBlocks, "/", pool->maxBlocks, " blocks");
    RF_LOG_INFO("===========================================");
}

// Performance monitoring
void RF_MemoryPool_ResetPerformanceCounters(RF_MemoryPool* pool)
{
    if (!pool)
        return;

    pool->allocationCount = 0;
    pool->peakUsage = pool->usedBlocks;
    
    RF_LOG_INFO("RF_MemoryPool: Reset performance counters");
}

void RF_MemoryPool_PrintPerformanceInfo(RF_MemoryPool* pool)
{
    if (!pool)
    {
        RF_LOG_WARNING("RF_MemoryPool: Cannot print performance info for null pool");
        return;
    }

    RF_LOG_INFO("=== Memory Pool Performance ===");
    RF_LOG_INFO("Total Allocations: ", pool->allocationCount);
    RF_LOG_INFO("Peak Usage: ", pool->peakUsage, " blocks");
    RF_LOG_INFO("Current Usage: ", pool->usedBlocks, " blocks");
    RF_LOG_INFO("Efficiency: ", pool->peakUsage > 0 ? 
                (float)pool->usedBlocks / pool->peakUsage * 100.0f : 0.0f, "%");
    RF_LOG_INFO("Block Size: ", pool->blockSize, " bytes");
    RF_LOG_INFO("Total Memory: ", pool->poolSize, " bytes");
    RF_LOG_INFO("=============================");
}