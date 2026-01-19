#include "rf_object_pool.h"
#include "rf_logger.h"
#include <cstring>

// Object pool node structure
typedef struct RF_ObjectPoolNode
{
    void* object;
    struct RF_ObjectPoolNode* next;
    uint32_t magic;
} RF_ObjectPoolNode;

#define RF_OBJECT_POOL_MAGIC 0xCAFEBABE
#define RF_OBJECT_POOL_FREE_MAGIC 0xDEADC0DE

// Utility functions
static RF_ObjectPoolNode* CreateNode(RF_ObjectPool* pool);
static void DestroyNode(RF_ObjectPool* pool, RF_ObjectPoolNode* node);
static void AddToFreeList(RF_ObjectPool* pool, RF_ObjectPoolNode* node);
static RF_ObjectPoolNode* RemoveFromFreeList(RF_ObjectPool* pool);

bool RF_ObjectPool_Initialize(RF_ObjectPool* pool, 
                             size_t objectSize, 
                             uint32_t initialCapacity,
                             uint32_t maxCapacity,
                             RF_ObjectPoolCreateFunc createFunc,
                             RF_ObjectPoolDestroyFunc destroyFunc,
                             RF_ObjectPoolResetFunc resetFunc)
{
    if (!pool || objectSize == 0 || !createFunc || !destroyFunc)
    {
        RF_LOG_ERROR("RF_ObjectPool: Invalid parameters for initialization");
        return false;
    }

    if (initialCapacity > maxCapacity)
    {
        RF_LOG_ERROR("RF_ObjectPool: Initial capacity cannot exceed max capacity");
        return false;
    }

    // Initialize pool
    pool->objectSize = objectSize;
    pool->initialCapacity = initialCapacity;
    pool->maxCapacity = maxCapacity;
    pool->currentCapacity = 0;
    pool->activeObjects = 0;
    pool->totalCreated = 0;
    pool->totalDestroyed = 0;
    pool->peakUsage = 0;
    pool->createFunc = createFunc;
    pool->destroyFunc = destroyFunc;
    pool->resetFunc = resetFunc;
    pool->userData = NULL;
    pool->freeList = NULL;
    pool->allNodes = NULL;

    // Pre-allocate initial objects
    for (uint32_t i = 0; i < initialCapacity; ++i)
    {
        RF_ObjectPoolNode* node = CreateNode(pool);
        if (!node)
        {
            RF_LOG_ERROR("RF_ObjectPool: Failed to create initial objects");
            RF_ObjectPool_Destroy(pool);
            return false;
        }
        AddToFreeList(pool, node);
    }

    RF_LOG_INFO("RF_ObjectPool: Initialized with ", initialCapacity, " objects (max: ", 
                maxCapacity, ", size: ", objectSize, " bytes)");
    return true;
}

void RF_ObjectPool_Destroy(RF_ObjectPool* pool)
{
    if (!pool)
    {
        RF_LOG_WARNING("RF_ObjectPool: Cannot destroy null pool");
        return;
    }

    if (pool->activeObjects > 0)
    {
        RF_LOG_WARNING("RF_ObjectPool: Destroying pool with ", pool->activeObjects, 
                      " objects still active");
    }

    // Destroy all nodes
    RF_ObjectPoolNode* current = pool->allNodes;
    while (current)
    {
        RF_ObjectPoolNode* next = current->next;
        DestroyNode(pool, current);
        current = next;
    }

    // Clear all data
    memset(pool, 0, sizeof(RF_ObjectPool));

    RF_LOG_INFO("RF_ObjectPool: Destroyed");
}

void* RF_ObjectPool_Acquire(RF_ObjectPool* pool)
{
    if (!pool)
    {
        RF_LOG_ERROR("RF_ObjectPool: Cannot acquire from null pool");
        return NULL;
    }

    RF_ObjectPoolNode* node = RemoveFromFreeList(pool);
    
    // If no free objects available and we can create more
    if (!node && pool->currentCapacity < pool->maxCapacity)
    {
        node = CreateNode(pool);
        if (!node)
        {
            RF_LOG_ERROR("RF_ObjectPool: Failed to create new object");
            return NULL;
        }
    }

    if (!node)
    {
        RF_LOG_WARNING("RF_ObjectPool: Pool exhausted. Active: ", pool->activeObjects, 
                      "/", pool->currentCapacity);
        return NULL;
    }

    // Reset object if reset function is provided
    if (pool->resetFunc && node->object)
    {
        pool->resetFunc(node->object, pool->userData);
    }

    pool->activeObjects++;
    
    if (pool->activeObjects > pool->peakUsage)
    {
        pool->peakUsage = pool->activeObjects;
    }

    RF_LOG_DEBUG("RF_ObjectPool: Acquired object ", node->object, 
                " (active: ", pool->activeObjects, "/", pool->currentCapacity, ")");
    return node->object;
}

void RF_ObjectPool_Release(RF_ObjectPool* pool, void* object)
{
    if (!pool || !object)
    {
        RF_LOG_WARNING("RF_ObjectPool: Invalid parameters for release");
        return;
    }

    // Find the node containing this object
    RF_ObjectPoolNode* current = pool->allNodes;
    while (current)
    {
        if (current->object == object && current->magic == RF_OBJECT_POOL_MAGIC)
        {
            // Reset object if reset function is provided
            if (pool->resetFunc && object)
            {
                pool->resetFunc(object, pool->userData);
            }

            // Add back to free list
            AddToFreeList(pool, current);
            pool->activeObjects--;

            RF_LOG_DEBUG("RF_ObjectPool: Released object ", object, 
                        " (active: ", pool->activeObjects, "/", pool->currentCapacity, ")");
            return;
        }
        current = current->next;
    }

    RF_LOG_ERROR("RF_ObjectPool: Attempted to release object not owned by pool: ", object);
}

void RF_ObjectPool_SetUserData(RF_ObjectPool* pool, void* userData)
{
    if (!pool)
        return;
    
    pool->userData = userData;
    RF_LOG_DEBUG("RF_ObjectPool: Set user data");
}

void* RF_ObjectPool_GetUserData(RF_ObjectPool* pool)
{
    if (!pool)
        return NULL;
    return pool->userData;
}

size_t RF_ObjectPool_GetObjectSize(RF_ObjectPool* pool)
{
    if (!pool)
        return 0;
    return pool->objectSize;
}

uint32_t RF_ObjectPool_GetInitialCapacity(RF_ObjectPool* pool)
{
    if (!pool)
        return 0;
    return pool->initialCapacity;
}

uint32_t RF_ObjectPool_GetMaxCapacity(RF_ObjectPool* pool)
{
    if (!pool)
        return 0;
    return pool->maxCapacity;
}

uint32_t RF_ObjectPool_GetCurrentCapacity(RF_ObjectPool* pool)
{
    if (!pool)
        return 0;
    return pool->currentCapacity;
}

uint32_t RF_ObjectPool_GetActiveObjects(RF_ObjectPool* pool)
{
    if (!pool)
        return 0;
    return pool->activeObjects;
}

uint32_t RF_ObjectPool_GetAvailableObjects(RF_ObjectPool* pool)
{
    if (!pool)
        return 0;
    return pool->currentCapacity - pool->activeObjects;
}

uint32_t RF_ObjectPool_GetPeakUsage(RF_ObjectPool* pool)
{
    if (!pool)
        return 0;
    return pool->peakUsage;
}

uint32_t RF_ObjectPool_GetTotalCreated(RF_ObjectPool* pool)
{
    if (!pool)
        return 0;
    return pool->totalCreated;
}

uint32_t RF_ObjectPool_GetTotalDestroyed(RF_ObjectPool* pool)
{
    if (!pool)
        return 0;
    return pool->totalDestroyed;
}

float RF_ObjectPool_GetUsageRatio(RF_ObjectPool* pool)
{
    if (!pool || pool->currentCapacity == 0)
        return 0.0f;
    return (float)pool->activeObjects / (float)pool->currentCapacity;
}

float RF_ObjectPool_GetEfficiency(RF_ObjectPool* pool)
{
    if (!pool || pool->totalCreated == 0)
        return 0.0f;
    return (float)pool->totalDestroyed / (float)pool->totalCreated;
}

bool RF_ObjectPool_CanAcquire(RF_ObjectPool* pool)
{
    if (!pool)
        return false;
    return (pool->activeObjects < pool->currentCapacity) || 
           (pool->currentCapacity < pool->maxCapacity);
}

void RF_ObjectPool_Reset(RF_ObjectPool* pool)
{
    if (!pool)
        return;

    // Move all active objects back to free list
    RF_ObjectPoolNode* current = pool->allNodes;
    while (current)
    {
        if (current->magic == RF_OBJECT_POOL_MAGIC)
        {
            // Reset object if reset function is provided
            if (pool->resetFunc && current->object)
            {
                pool->resetFunc(current->object, pool->userData);
            }

            // Add to free list if not already there
            bool inFreeList = false;
            RF_ObjectPoolNode* freeNode = pool->freeList;
            while (freeNode)
            {
                if (freeNode == current)
                {
                    inFreeList = true;
                    break;
                }
                freeNode = freeNode->next;
            }

            if (!inFreeList)
            {
                AddToFreeList(pool, current);
            }
        }
        current = current->next;
    }

    pool->activeObjects = 0;

    RF_LOG_INFO("RF_ObjectPool: Reset. All objects returned to pool");
}

void RF_ObjectPool_Clear(RF_ObjectPool* pool)
{
    if (!pool)
        return;

    // Destroy all objects and recreate initial capacity
    RF_ObjectPoolNode* current = pool->allNodes;
    while (current)
    {
        RF_ObjectPoolNode* next = current->next;
        DestroyNode(pool, current);
        current = next;
    }

    pool->currentCapacity = 0;
    pool->activeObjects = 0;
    pool->freeList = NULL;
    pool->allNodes = NULL;

    // Recreate initial objects
    for (uint32_t i = 0; i < pool->initialCapacity; ++i)
    {
        RF_ObjectPoolNode* node = CreateNode(pool);
        if (node)
        {
            AddToFreeList(pool, node);
        }
    }

    RF_LOG_INFO("RF_ObjectPool: Cleared and recreated initial capacity");
}

void RF_ObjectPool_ShrinkToFit(RF_ObjectPool* pool)
{
    if (!pool)
        return;

    // Calculate minimum capacity needed
    uint32_t minCapacity = pool->activeObjects;
    if (minCapacity < pool->initialCapacity)
    {
        minCapacity = pool->initialCapacity;
    }

    // Destroy excess objects
    uint32_t excess = pool->currentCapacity - minCapacity;
    for (uint32_t i = 0; i < excess; ++i)
    {
        RF_ObjectPoolNode* node = RemoveFromFreeList(pool);
        if (node)
        {
            DestroyNode(pool, node);
        }
    }

    RF_LOG_INFO("RF_ObjectPool: Shrunk from ", pool->currentCapacity + excess, 
                " to ", pool->currentCapacity, " objects");
}

void RF_ObjectPool_PrintStatistics(RF_ObjectPool* pool)
{
    if (!pool)
    {
        RF_LOG_WARNING("RF_ObjectPool: Cannot print stats for null pool");
        return;
    }

    RF_LOG_INFO("=== Object Pool Statistics ===");
    RF_LOG_INFO("Object Size: ", pool->objectSize, " bytes");
    RF_LOG_INFO("Initial Capacity: ", pool->initialCapacity);
    RF_LOG_INFO("Max Capacity: ", pool->maxCapacity);
    RF_LOG_INFO("Current Capacity: ", pool->currentCapacity);
    RF_LOG_INFO("Active Objects: ", pool->activeObjects);
    RF_LOG_INFO("Available Objects: ", pool->currentCapacity - pool->activeObjects);
    RF_LOG_INFO("Peak Usage: ", pool->peakUsage);
    RF_LOG_INFO("Total Created: ", pool->totalCreated);
    RF_LOG_INFO("Total Destroyed: ", pool->totalDestroyed);
    RF_LOG_INFO("Usage Ratio: ", RF_ObjectPool_GetUsageRatio(pool) * 100.0f, "%");
    RF_LOG_INFO("Efficiency: ", RF_ObjectPool_GetEfficiency(pool) * 100.0f, "%");
    RF_LOG_INFO("Memory Usage: ", pool->currentCapacity * pool->objectSize, " bytes");
    RF_LOG_INFO("==============================");
}

void RF_ObjectPool_Validate(RF_ObjectPool* pool)
{
    if (!pool)
    {
        RF_LOG_ERROR("RF_ObjectPool: Null pool");
        return;
    }

    if (!pool->createFunc || !pool->destroyFunc)
    {
        RF_LOG_ERROR("RF_ObjectPool: Missing required functions");
        return;
    }

    uint32_t freeCount = 0;
    uint32_t validNodes = 0;
    uint32_t corruptedNodes = 0;

    // Count free objects
    RF_ObjectPoolNode* current = pool->freeList;
    while (current)
    {
        freeCount++;
        
        if (current->magic != RF_OBJECT_POOL_MAGIC)
        {
            corruptedNodes++;
            RF_LOG_ERROR("RF_ObjectPool: Corrupted node in free list");
        }
        
        current = current->next;
    }

    // Count all nodes
    current = pool->allNodes;
    while (current)
    {
        validNodes++;
        
        if (current->magic != RF_OBJECT_POOL_MAGIC)
        {
            corruptedNodes++;
            RF_LOG_ERROR("RF_ObjectPool: Corrupted node in all nodes list");
        }
        
        current = current->next;
    }

    // Validate counts
    if (validNodes != pool->currentCapacity)
    {
        RF_LOG_ERROR("RF_ObjectPool: Capacity mismatch. Expected: ", 
                    pool->currentCapacity, ", Actual: ", validNodes);
    }

    if (pool->activeObjects + freeCount != pool->currentCapacity)
    {
        RF_LOG_ERROR("RF_ObjectPool: Object count mismatch. Active: ", 
                    pool->activeObjects, ", Free: ", freeCount, 
                    ", Total: ", pool->currentCapacity);
    }

    if (corruptedNodes > 0)
    {
        RF_LOG_ERROR("RF_ObjectPool: Found ", corruptedNodes, " corrupted nodes");
    }

    if (validNodes == pool->currentCapacity && 
        pool->activeObjects + freeCount == pool->currentCapacity && 
        corruptedNodes == 0)
    {
        RF_LOG_DEBUG("RF_ObjectPool: Validation passed");
    }
}

void RF_ObjectPool_DumpObjects(RF_ObjectPool* pool)
{
    if (!pool)
    {
        RF_LOG_WARNING("RF_ObjectPool: Cannot dump objects for null pool");
        return;
    }

    RF_LOG_INFO("=== Object Pool Dump ===");
    RF_LOG_INFO("Total Objects: ", pool->currentCapacity);
    RF_LOG_INFO("Active Objects: ", pool->activeObjects);

    uint32_t count = 0;
    RF_ObjectPoolNode* current = pool->allNodes;
    while (current && count < 50) // Limit for readability
    {
        bool isActive = false;
        RF_ObjectPoolNode* freeNode = pool->freeList;
        while (freeNode)
        {
            if (freeNode == current)
            {
                isActive = false;
                break;
            }
            freeNode = freeNode->next;
        }
        isActive = (freeNode == NULL);

        RF_LOG_INFO(count++, ": ", current->object, " - ", 
                    isActive ? "ACTIVE" : "FREE");
        current = current->next;
    }

    if (pool->currentCapacity > 50)
    {
        RF_LOG_INFO("... (", pool->currentCapacity - 50, " more objects)");
    }

    RF_LOG_INFO("======================");
}

// Convenience functions for creating pools with malloc
RF_ObjectPool* RF_CreateObjectPool(size_t objectSize,
                                   uint32_t initialCapacity,
                                   uint32_t maxCapacity,
                                   RF_ObjectPoolCreateFunc createFunc,
                                   RF_ObjectPoolDestroyFunc destroyFunc,
                                   RF_ObjectPoolResetFunc resetFunc)
{
    RF_ObjectPool* pool = (RF_ObjectPool*)malloc(sizeof(RF_ObjectPool));
    if (!pool)
    {
        RF_LOG_ERROR("RF_ObjectPool: Failed to allocate pool structure");
        return NULL;
    }

    if (!RF_ObjectPool_Initialize(pool, objectSize, initialCapacity, maxCapacity,
                                 createFunc, destroyFunc, resetFunc))
    {
        free(pool);
        return NULL;
    }

    RF_LOG_INFO("RF_ObjectPool: Created pool with malloc");
    return pool;
}

void RF_DestroyObjectPool(RF_ObjectPool* pool)
{
    if (!pool)
        return;

    RF_ObjectPool_Destroy(pool);
    free(pool);
    RF_LOG_INFO("RF_ObjectPool: Destroyed pool created with malloc");
}

// Advanced utilities
void RF_ObjectPool_ForEachActive(RF_ObjectPool* pool, RF_ObjectPoolForEachFunc func, void* userData)
{
    if (!pool || !func)
        return;

    RF_ObjectPoolNode* current = pool->allNodes;
    while (current)
    {
        if (current->magic == RF_OBJECT_POOL_MAGIC)
        {
            // Check if object is active (not in free list)
            bool isActive = true;
            RF_ObjectPoolNode* freeNode = pool->freeList;
            while (freeNode)
            {
                if (freeNode == current)
                {
                    isActive = false;
                    break;
                }
                freeNode = freeNode->next;
            }

            if (isActive && current->object)
            {
                func(current->object, userData);
            }
        }
        current = current->next;
    }
}

void RF_ObjectPool_ForEachAll(RF_ObjectPool* pool, RF_ObjectPoolForEachFunc func, void* userData)
{
    if (!pool || !func)
        return;

    RF_ObjectPoolNode* current = pool->allNodes;
    while (current)
    {
        if (current->magic == RF_OBJECT_POOL_MAGIC && current->object)
        {
            func(current->object, userData);
        }
        current = current->next;
    }
}

uint32_t RF_ObjectPool_CountActiveObjects(RF_ObjectPool* pool)
{
    if (!pool)
        return 0;
    return pool->activeObjects;
}

bool RF_ObjectPool_IsObjectFromPool(RF_ObjectPool* pool, void* object)
{
    if (!pool || !object)
        return false;

    RF_ObjectPoolNode* current = pool->allNodes;
    while (current)
    {
        if (current->object == object && current->magic == RF_OBJECT_POOL_MAGIC)
        {
            return true;
        }
        current = current->next;
    }

    return false;
}

bool RF_ObjectPool_IsObjectActive(RF_ObjectPool* pool, void* object)
{
    if (!pool || !object)
        return false;

    // First check if object is from this pool
    if (!RF_ObjectPool_IsObjectFromPool(pool, object))
        return false;

    // Check if object is in free list
    RF_ObjectPoolNode* current = pool->freeList;
    while (current)
    {
        if (current->object == object)
        {
            return false; // Object is in free list, so it's not active
        }
        current = current->next;
    }

    return true; // Object is from pool but not in free list, so it's active
}

// Performance monitoring
void RF_ObjectPool_ResetPerformanceCounters(RF_ObjectPool* pool)
{
    if (!pool)
        return;

    pool->totalCreated = 0;
    pool->totalDestroyed = 0;
    pool->peakUsage = pool->activeObjects;
    
    RF_LOG_INFO("RF_ObjectPool: Reset performance counters");
}

void RF_ObjectPool_PrintPerformanceInfo(RF_ObjectPool* pool)
{
    if (!pool)
    {
        RF_LOG_WARNING("RF_ObjectPool: Cannot print performance info for null pool");
        return;
    }

    RF_LOG_INFO("=== Object Pool Performance ===");
    RF_LOG_INFO("Total Created: ", pool->totalCreated);
    RF_LOG_INFO("Total Destroyed: ", pool->totalDestroyed);
    RF_LOG_INFO("Current Active: ", pool->activeObjects);
    RF_LOG_INFO("Peak Usage: ", pool->peakUsage);
    RF_LOG_INFO("Efficiency: ", RF_ObjectPool_GetEfficiency(pool) * 100.0f, "%");
    RF_LOG_INFO("Reuse Ratio: ", pool->totalCreated > 0 ? 
                (float)(pool->totalCreated - pool->currentCapacity) / pool->totalCreated * 100.0f : 0.0f, "%");
    RF_LOG_INFO("Memory Efficiency: ", RF_ObjectPool_GetUsageRatio(pool) * 100.0f, "%");
    RF_LOG_INFO("==============================");
}

// Internal utility function implementations
static RF_ObjectPoolNode* CreateNode(RF_ObjectPool* pool)
{
    if (!pool)
        return NULL;

    // Allocate node structure
    RF_ObjectPoolNode* node = (RF_ObjectPoolNode*)malloc(sizeof(RF_ObjectPoolNode));
    if (!node)
    {
        RF_LOG_ERROR("RF_ObjectPool: Failed to allocate node structure");
        return NULL;
    }

    // Create object using user-provided function
    void* object = pool->createFunc(pool->userData);
    if (!object)
    {
        free(node);
        RF_LOG_ERROR("RF_ObjectPool: Failed to create object");
        return NULL;
    }

    // Initialize node
    node->object = object;
    node->next = NULL;
    node->magic = RF_OBJECT_POOL_MAGIC;

    // Add to all nodes list
    node->next = pool->allNodes;
    pool->allNodes = node;

    // Update statistics
    pool->currentCapacity++;
    pool->totalCreated++;

    RF_LOG_DEBUG("RF_ObjectPool: Created new object ", object);
    return node;
}

static void DestroyNode(RF_ObjectPool* pool, RF_ObjectPoolNode* node)
{
    if (!pool || !node)
        return;

    // Validate magic
    if (node->magic != RF_OBJECT_POOL_MAGIC)
    {
        RF_LOG_ERROR("RF_ObjectPool: Attempted to destroy corrupted node");
        return;
    }

    // Destroy object using user-provided function
    if (node->object && pool->destroyFunc)
    {
        pool->destroyFunc(node->object, pool->userData);
    }

    // Remove from all nodes list
    if (pool->allNodes == node)
    {
        pool->allNodes = node->next;
    }
    else
    {
        RF_ObjectPoolNode* current = pool->allNodes;
        while (current && current->next != node)
        {
            current = current->next;
        }
        if (current)
        {
            current->next = node->next;
        }
    }

    // Mark as corrupted and free
    node->magic = RF_OBJECT_POOL_FREE_MAGIC;
    free(node);

    // Update statistics
    pool->currentCapacity--;
    pool->totalDestroyed++;

    RF_LOG_DEBUG("RF_ObjectPool: Destroyed object");
}

static void AddToFreeList(RF_ObjectPool* pool, RF_ObjectPoolNode* node)
{
    if (!pool || !node)
        return;

    // Add to head of free list
    node->next = pool->freeList;
    pool->freeList = node;

    RF_LOG_DEBUG("RF_ObjectPool: Added object to free list");
}

static RF_ObjectPoolNode* RemoveFromFreeList(RF_ObjectPool* pool)
{
    if (!pool || !pool->freeList)
        return NULL;

    // Remove from head of free list
    RF_ObjectPoolNode* node = pool->freeList;
    pool->freeList = node->next;
    node->next = NULL;

    RF_LOG_DEBUG("RF_ObjectPool: Removed object from free list");
    return node;
}