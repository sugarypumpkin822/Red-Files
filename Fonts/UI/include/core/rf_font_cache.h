#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>
#include <chrono>

namespace RFCore {

// Cache types
enum class FontCacheType {
    GLYPH_BITMAP,
    GLYPH_OUTLINE,
    GLYPH_SDF,
    GLYPH_MSDF,
    FONT_METRICS,
    KERNING_PAIRS,
    FONT_INFO,
    CUSTOM
};

// Cache strategies
enum class FontCacheStrategy {
    LRU,
    LFU,
    FIFO,
    RANDOM,
    CUSTOM
};

// Cache eviction policies
enum class FontCacheEvictionPolicy {
    LEAST_RECENTLY_USED,
    LEAST_FREQUENTLY_USED,
    FIRST_IN_FIRST_OUT,
    RANDOM_REPLACEMENT,
    SIZE_BASED,
    TIME_BASED,
    CUSTOM
};

// Cache entry states
enum class FontCacheEntryState {
    LOADING,
    LOADED,
    EVICTED,
    INVALID,
    UPDATING
};

// Cache entry priority
enum class FontCacheEntryPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};

// Cache entry
struct FontCacheEntry {
    std::string key;
    std::any data;
    FontCacheType type;
    FontCacheEntryState state;
    FontCacheEntryPriority priority;
    size_t size;
    uint64_t timestamp;
    uint64_t lastAccess;
    uint32_t accessCount;
    uint64_t expirationTime;
    std::map<std::string, std::any> metadata;
    
    FontCacheEntry() : type(FontCacheType::CUSTOM), state(FontCacheEntryState::LOADED), 
                      priority(FontCacheEntryPriority::NORMAL), size(0), timestamp(0), 
                      lastAccess(0), accessCount(0), expirationTime(0) {}
    FontCacheEntry(const std::string& k, const std::any& d, FontCacheType t, FontCacheEntryState st,
                 FontCacheEntryPriority prio, size_t sz, uint64_t ts, uint64_t la, uint32_t ac, uint64_t exp)
        : key(k), data(d), type(t), state(st), priority(prio), size(sz), timestamp(ts), 
          lastAccess(la), accessCount(ac), expirationTime(exp) {}
};

// Cache statistics
struct FontCacheStatistics {
    uint32_t totalEntries;
    uint32_t loadedEntries;
    uint32_t evictedEntries;
    uint32_t invalidEntries;
    uint64_t totalSize;
    uint64_t usedSize;
    uint64_t maxSize;
    float32 utilizationRatio;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    float32 hitRatio;
    uint64_t evictions;
    float32 averageAccessTime;
    uint64_t totalAccessTime;
    uint64_t averageEntryLifetime;
    uint64_t totalLifetime;
    
    FontCacheStatistics() : totalEntries(0), loadedEntries(0), evictedEntries(0), 
                           invalidEntries(0), totalSize(0), usedSize(0), maxSize(0), 
                           utilizationRatio(0.0f), cacheHits(0), cacheMisses(0), 
                           hitRatio(0.0f), evictions(0), averageAccessTime(0.0f), 
                           totalAccessTime(0), averageEntryLifetime(0), totalLifetime(0) {}
};

// Cache configuration
struct FontCacheConfig {
    FontCacheStrategy strategy;
    FontCacheEvictionPolicy evictionPolicy;
    size_t maxSize;
    size_t maxEntries;
    uint64_t defaultExpiration;
    bool enableCompression;
    bool enableEncryption;
    bool enableValidation;
    bool enableStatistics;
    bool enablePersistence;
    bool enableAsyncLoading;
    float32 compressionLevel;
    uint32_t cleanupInterval;
    uint32_t statisticsUpdateInterval;
    
    FontCacheConfig() : strategy(FontCacheStrategy::LRU), evictionPolicy(FontCacheEvictionPolicy::LEAST_RECENTLY_USED),
                        maxSize(1024 * 1024 * 100), maxEntries(10000), defaultExpiration(3600), 
                        enableCompression(false), enableEncryption(false), enableValidation(true), 
                        enableStatistics(true), enablePersistence(false), enableAsyncLoading(false), 
                        compressionLevel(0.5f), cleanupInterval(60), statisticsUpdateInterval(10) {}
};

// Font cache
class FontCache {
public:
    FontCache();
    virtual ~FontCache() = default;
    
    // Cache management
    void initialize(const FontCacheConfig& config);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Configuration
    void setConfiguration(const FontCacheConfig& config);
    const FontCacheConfig& getConfiguration() const;
    void setMaxSize(size_t maxSize);
    void setMaxEntries(size_t maxEntries);
    void setStrategy(FontCacheStrategy strategy);
    void setEvictionPolicy(FontCacheEvictionPolicy policy);
    
    // Cache operations
    bool put(const std::string& key, const std::any& data, FontCacheType type, 
            FontCacheEntryPriority priority = FontCacheEntryPriority::NORMAL, uint64_t expiration = 0);
    bool get(const std::string& key, std::any& data);
    bool remove(const std::string& key);
    bool contains(const std::string& key) const;
    void clear();
    void clear(FontCacheType type);
    
    // Batch operations
    bool putBatch(const std::map<std::string, std::pair<std::any, FontCacheType>>& entries);
    std::vector<std::any> getBatch(const std::vector<std::string>& keys);
    void removeBatch(const std::vector<std::string>& keys);
    
    // Async operations
    void putAsync(const std::string& key, const std::any& data, FontCacheType type,
                 std::function<void(bool)> callback = nullptr);
    void getAsync(const std::string& key, std::function<void(bool, const std::any&)> callback);
    void removeAsync(const std::string& key, std::function<void(bool)> callback = nullptr);
    
    // Cache entry management
    FontCacheEntry getEntry(const std::string& key) const;
    bool updateEntry(const std::string& key, const std::any& data);
    bool updateEntryPriority(const std::string& key, FontCacheEntryPriority priority);
    bool updateEntryExpiration(const std::string& key, uint64_t expiration);
    bool touchEntry(const std::string& key);
    std::vector<std::string> getKeys(FontCacheType type = FontCacheType::CUSTOM) const;
    std::vector<FontCacheEntry> getEntries(FontCacheType type = FontCacheType::CUSTOM) const;
    
    // Cache validation
    bool validateEntry(const std::string& key) const;
    bool validateEntry(const FontCacheEntry& entry) const;
    std::vector<std::string> getInvalidEntries() const;
    void removeInvalidEntries();
    
    // Cache eviction
    bool evictEntry(const std::string& key);
    bool evictEntries(size_t count);
    bool evictEntriesByType(FontCacheType type, size_t count);
    bool evictEntriesByPriority(FontCacheEntryPriority priority, size_t count);
    bool evictExpiredEntries();
    void setEvictionPolicy(FontCacheEvictionPolicy policy);
    
    // Cache compression
    void enableCompression(bool enabled);
    bool isCompressionEnabled() const;
    void setCompressionLevel(float32 level);
    float32 getCompressionLevel() const;
    bool compressEntry(const std::string& key);
    bool decompressEntry(const std::string& key);
    
    // Cache encryption
    void enableEncryption(bool enabled);
    bool isEncryptionEnabled() const;
    void setEncryptionKey(const std::vector<uint8>& key);
    std::vector<uint8> getEncryptionKey() const;
    bool encryptEntry(const std::string& key);
    bool decryptEntry(const std::string& key);
    
    // Cache persistence
    void enablePersistence(bool enabled);
    bool isPersistenceEnabled() const;
    void setPersistenceFile(const std::string& filename);
    std::string getPersistenceFile() const;
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
    bool saveToFile() const;
    bool loadFromFile();
    
    // Cache statistics
    FontCacheStatistics getStatistics() const;
    void updateStatistics();
    void resetStatistics();
    void enableStatistics(bool enabled);
    bool isStatisticsEnabled() const;
    
    // Cache monitoring
    size_t getSize() const;
    size_t getUsedSize() const;
    size_t getFreeSize() const;
    size_t getEntryCount() const;
    size_t getEntryCount(FontCacheType type) const;
    float32 getUtilizationRatio() const;
    float32 getHitRatio() const;
    uint64_t getOldestEntryAge() const;
    uint64_t getNewestEntryAge() const;
    
    // Cache maintenance
    void cleanup();
    void optimize();
    void compact();
    void defragment();
    void rebuild();
    
    // Cache debugging
    void enableDebugging(bool enabled);
    bool isDebuggingEnabled() const;
    std::vector<std::string> getDebugMessages() const;
    void clearDebugMessages();
    void dumpCache() const;
    std::string dumpCacheToString() const;
    
    // Event handling
    void addCacheEventListener(const std::string& eventType, std::function<void()> callback);
    void removeCacheEventListener(const std::string& eventType, std::function<void()> callback);
    void clearCacheEventListeners();
    
    // Utility methods
    void cloneFrom(const FontCache& other);
    virtual std::unique_ptr<FontCache> clone() const;
    bool equals(const FontCache& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::string, FontCacheEntry> cache_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    FontCacheConfig config_;
    FontCacheStatistics statistics_;
    
    bool isInitialized_;
    bool statisticsEnabled_;
    bool debuggingEnabled_;
    bool compressionEnabled_;
    bool encryptionEnabled_;
    bool persistenceEnabled_;
    
    size_t currentSize_;
    uint64_t nextCleanupTime_;
    uint64_t nextStatisticsUpdateTime_;
    std::vector<uint8> encryptionKey_;
    std::string persistenceFile_;
    std::vector<std::string> debugMessages_;
    
    // Protected helper methods
    virtual void triggerCacheEvent(const std::string& eventType);
    virtual bool shouldEvict() const;
    virtual std::string selectEvictionCandidate();
    virtual void evictEntryInternal(const std::string& key);
    virtual void updateEntryAccess(const std::string& key);
    virtual void cleanupExpiredEntries();
    virtual void updateStatisticsInternal();
    
    // Compression helpers
    virtual std::vector<uint8> compressData(const std::vector<uint8>& data) const;
    virtual std::vector<uint8> decompressData(const std::vector<uint8>& data) const;
    virtual size_t getCompressedSize(const std::vector<uint8>& data) const;
    
    // Encryption helpers
    virtual std::vector<uint8> encryptData(const std::vector<uint8>& data) const;
    virtual std::vector<uint8> decryptData(const std::vector<uint8>& data) const;
    
    // Validation helpers
    virtual bool validateCacheEntry(const FontCacheEntry& entry) const;
    virtual bool validateData(const std::any& data, FontCacheType type) const;
    virtual bool isDataExpired(const FontCacheEntry& entry) const;
    
    // Persistence helpers
    virtual bool serializeEntry(const FontCacheEntry& entry, std::vector<uint8>& data) const;
    virtual bool deserializeEntry(const std::vector<uint8>& data, FontCacheEntry& entry) const;
    virtual std::string generatePersistenceKey(const FontCacheEntry& entry) const;
    
    // Statistics helpers
    virtual void updateHitStatistics(bool hit);
    virtual void updateEvictionStatistics();
    virtual void updateSizeStatistics(size_t oldSize, size_t newSize);
    virtual void updateAccessStatistics(const FontCacheEntry& entry);
    virtual void calculateStatistics();
    
    // Debug helpers
    virtual void addDebugMessage(const std::string& message);
    virtual void logCacheOperation(const std::string& operation, const std::string& key);
    virtual std::string formatCacheEntry(const FontCacheEntry& entry) const;
};

// Specialized cache implementations
class GlyphBitmapCache : public FontCache {
public:
    GlyphBitmapCache();
    bool put(const std::string& key, const GlyphBitmap& bitmap, 
            FontCacheEntryPriority priority = FontCacheEntryPriority::NORMAL, uint64_t expiration = 0);
    bool get(const std::string& key, GlyphBitmap& bitmap);
    std::unique_ptr<FontCache> clone() const override;
    
protected:
    bool validateData(const std::any& data, FontCacheType type) const override;
};

class GlyphOutlineCache : public FontCache {
public:
    GlyphOutlineCache();
    bool put(const std::string& key, const GlyphOutline& outline, 
            FontCacheEntryPriority priority = FontCacheEntryPriority::NORMAL, uint64_t expiration = 0);
    bool get(const std::string& key, GlyphOutline& outline);
    std::unique_ptr<FontCache> clone() const override;
    
protected:
    bool validateData(const std::any& data, FontCacheType type) const override;
};

class GlyphSDFCache : public FontCache {
public:
    GlyphSDFCache();
    bool put(const std::string& key, const GlyphSDF& sdf, 
            FontCacheEntryPriority priority = FontCacheEntryPriority::NORMAL, uint64_t expiration = 0);
    bool get(const std::string& key, GlyphSDF& sdf);
    std::unique_ptr<FontCache> clone() const override;
    
protected:
    bool validateData(const std::any& data, FontCacheType type) const override;
};

class FontMetricsCache : public FontCache {
public:
    FontMetricsCache();
    bool put(const std::string& key, const FontMetrics& metrics, 
            FontCacheEntryPriority priority = FontCacheEntryPriority::NORMAL, uint64_t expiration = 0);
    bool get(const std::string& key, FontMetrics& metrics);
    std::unique_ptr<FontCache> clone() const override;
    
protected:
    bool validateData(const std::any& data, FontCacheType type) const override;
};

// Cache factory
class FontCacheFactory {
public:
    static std::unique_ptr<FontCache> createCache(FontCacheType type);
    static std::unique_ptr<GlyphBitmapCache> createGlyphBitmapCache();
    static std::unique_ptr<GlyphOutlineCache> createGlyphOutlineCache();
    static std::unique_ptr<GlyphSDFCache> createGlyphSDFCache();
    static std::unique_ptr<FontMetricsCache> createFontMetricsCache();
    static FontCacheConfig createDefaultConfig(FontCacheType type);
    static std::vector<FontCacheType> getSupportedCacheTypes();
    static std::vector<FontCacheStrategy> getSupportedCacheStrategies();
    static std::vector<FontCacheEvictionPolicy> getSupportedEvictionPolicies();
};

} // namespace RFCore