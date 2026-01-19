#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFCore {

// Glyph cache types
enum class GlyphCacheType {
    BITMAP,
    OUTLINE,
    SDF,
    MSDF,
    METRICS,
    KERNING,
    PATH,
    COLOR,
    CUSTOM
};

// Glyph cache formats
enum class GlyphCacheFormat {
    MONOCHROME,
    GRAYSCALE,
    RGB,
    RGBA,
    FLOAT32,
    CUSTOM
};

// Glyph cache quality levels
enum class GlyphCacheQuality {
    LOW,
    MEDIUM,
    HIGH,
    ULTRA,
    CUSTOM
};

// Glyph cache entry
struct GlyphCacheEntry {
    uint32_t glyphIndex;
    GlyphCacheType type;
    GlyphCacheFormat format;
    GlyphCacheQuality quality;
    float32 size;
    std::any data;
    uint64_t timestamp;
    uint64_t lastAccess;
    uint32_t accessCount;
    size_t memoryUsage;
    bool isValid;
    
    GlyphCacheEntry() : glyphIndex(0), type(GlyphCacheType::CUSTOM), format(GlyphCacheFormat::CUSTOM), 
                       quality(GlyphCacheQuality::MEDIUM), size(0.0f), timestamp(0), lastAccess(0), 
                       accessCount(0), memoryUsage(0), isValid(false) {}
    GlyphCacheEntry(uint32_t idx, GlyphCacheType t, GlyphCacheFormat fmt, GlyphCacheQuality qual,
                   float32 sz, const std::any& d, uint64_t ts, uint64_t la, uint32_t ac, size_t mem, bool valid)
        : glyphIndex(idx), type(t), format(fmt), quality(qual), size(sz), data(d), timestamp(ts), 
          lastAccess(la), accessCount(ac), memoryUsage(mem), isValid(valid) {}
};

// Glyph cache statistics
struct GlyphCacheStatistics {
    uint32_t totalEntries;
    uint32_t validEntries;
    uint32_t invalidEntries;
    uint64_t totalMemoryUsage;
    uint64_t validMemoryUsage;
    float32 averageAccessCount;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    float32 hitRatio;
    uint32_t bitmapEntries;
    uint32_t outlineEntries;
    uint32_t sdfEntries;
    uint32_t msdfEntries;
    uint32_t metricsEntries;
    uint32_t kerningEntries;
    
    GlyphCacheStatistics() : totalEntries(0), validEntries(0), invalidEntries(0), 
                           totalMemoryUsage(0), validMemoryUsage(0), averageAccessCount(0.0f), 
                           cacheHits(0), cacheMisses(0), hitRatio(0.0f), bitmapEntries(0), 
                           outlineEntries(0), sdfEntries(0), msdfEntries(0), metricsEntries(0), 
                           kerningEntries(0) {}
};

// Glyph cache configuration
struct GlyphCacheConfig {
    size_t maxEntries;
    size_t maxMemoryUsage;
    GlyphCacheQuality defaultQuality;
    bool enableBitmapCache;
    bool enableOutlineCache;
    bool enableSDFCache;
    bool enableMSDFCache;
    bool enableMetricsCache;
    bool enableKerningCache;
    bool enablePathCache;
    bool enableColorCache;
    bool enableCompression;
    bool enableValidation;
    bool enableStatistics;
    float32 compressionLevel;
    uint32_t cleanupInterval;
    
    GlyphCacheConfig() : maxEntries(10000), maxMemoryUsage(1024 * 1024 * 100), 
                        defaultQuality(GlyphCacheQuality::MEDIUM), enableBitmapCache(true), 
                        enableOutlineCache(true), enableSDFCache(false), enableMSDFCache(false), 
                        enableMetricsCache(true), enableKerningCache(true), enablePathCache(false), 
                        enableColorCache(false), enableCompression(false), enableValidation(true), 
                        enableStatistics(true), compressionLevel(0.5f), cleanupInterval(60) {}
};

// Glyph cache
class GlyphCache {
public:
    GlyphCache();
    virtual ~GlyphCache() = default;
    
    // Cache management
    void initialize(const GlyphCacheConfig& config);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Configuration
    void setConfiguration(const GlyphCacheConfig& config);
    const GlyphCacheConfig& getConfiguration() const;
    void setMaxEntries(size_t maxEntries);
    void setMaxMemoryUsage(size_t maxMemoryUsage);
    void setDefaultQuality(GlyphCacheQuality quality);
    
    // Glyph bitmap caching
    bool putGlyphBitmap(uint32_t glyphIndex, float32 size, const GlyphBitmap& bitmap, 
                       GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    bool getGlyphBitmap(uint32_t glyphIndex, float32 size, GlyphBitmap& bitmap, 
                       GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    bool hasGlyphBitmap(uint32_t glyphIndex, float32 size, GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    void removeGlyphBitmap(uint32_t glyphIndex, float32 size, GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    
    // Glyph outline caching
    bool putGlyphOutline(uint32_t glyphIndex, float32 size, const GlyphOutline& outline, 
                        GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    bool getGlyphOutline(uint32_t glyphIndex, float32 size, GlyphOutline& outline, 
                        GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    bool hasGlyphOutline(uint32_t glyphIndex, float32 size, GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    void removeGlyphOutline(uint32_t glyphIndex, float32 size, GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    
    // Glyph SDF caching
    bool putGlyphSDF(uint32_t glyphIndex, float32 size, const GlyphSDF& sdf, 
                    GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    bool getGlyphSDF(uint32_t glyphIndex, float32 size, GlyphSDF& sdf, 
                    GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    bool hasGlyphSDF(uint32_t glyphIndex, float32 size, GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    void removeGlyphSDF(uint32_t glyphIndex, float32 size, GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    
    // Glyph MSDF caching
    bool putGlyphMSDF(uint32_t glyphIndex, float32 size, const GlyphMSDF& msdf, 
                     GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    bool getGlyphMSDF(uint32_t glyphIndex, float32 size, GlyphMSDF& msdf, 
                     GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    bool hasGlyphMSDF(uint32_t glyphIndex, float32 size, GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    void removeGlyphMSDF(uint32_t glyphIndex, float32 size, GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    
    // Glyph metrics caching
    bool putGlyphMetrics(uint32_t glyphIndex, float32 size, const GlyphMetrics& metrics, 
                        GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    bool getGlyphMetrics(uint32_t glyphIndex, float32 size, GlyphMetrics& metrics, 
                        GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    bool hasGlyphMetrics(uint32_t glyphIndex, float32 size, GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    void removeGlyphMetrics(uint32_t glyphIndex, float32 size, GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    
    // Glyph path caching
    bool putGlyphPath(uint32_t glyphIndex, float32 size, const GlyphPath& path, 
                     GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    bool getGlyphPath(uint32_t glyphIndex, float32 size, GlyphPath& path, 
                     GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    bool hasGlyphPath(uint32_t glyphIndex, float32 size, GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    void removeGlyphPath(uint32_t glyphIndex, float32 size, GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    
    // Generic cache operations
    bool putGlyphData(uint32_t glyphIndex, float32 size, GlyphCacheType type, const std::any& data, 
                     GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    bool getGlyphData(uint32_t glyphIndex, float32 size, GlyphCacheType type, std::any& data, 
                     GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    bool hasGlyphData(uint32_t glyphIndex, float32 size, GlyphCacheType type, GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    void removeGlyphData(uint32_t glyphIndex, float32 size, GlyphCacheType type, GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    
    // Batch operations
    void putGlyphBitmaps(const std::vector<std::tuple<uint32_t, float32, GlyphBitmap>>& bitmaps, 
                         GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    void putGlyphOutlines(const std::vector<std::tuple<uint32_t, float32, GlyphOutline>>& outlines, 
                         GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    void putGlyphSDFs(const std::vector<std::tuple<uint32_t, float32, GlyphSDF>>& sdfs, 
                     GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    void putGlyphMSDFs(const std::vector<std::tuple<uint32_t, float32, GlyphMSDF>>& msdfs, 
                      GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    void putGlyphMetricsBatch(const std::vector<std::tuple<uint32_t, float32, GlyphMetrics>>& metrics, 
                            GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM);
    
    // Cache management
    void clear();
    void clear(GlyphCacheType type);
    void clear(GlyphCacheQuality quality);
    void clear(uint32_t glyphIndex);
    void cleanup();
    void optimize();
    void compact();
    
    // Cache validation
    bool validateEntry(const GlyphCacheEntry& entry) const;
    bool validateGlyphBitmap(const GlyphBitmap& bitmap) const;
    bool validateGlyphOutline(const GlyphOutline& outline) const;
    bool validateGlyphSDF(const GlyphSDF& sdf) const;
    bool validateGlyphMSDF(const GlyphMSDF& msdf) const;
    bool validateGlyphMetrics(const GlyphMetrics& metrics) const;
    std::vector<GlyphCacheEntry> getInvalidEntries() const;
    void removeInvalidEntries();
    
    // Cache compression
    void enableCompression(bool enabled);
    bool isCompressionEnabled() const;
    void setCompressionLevel(float32 level);
    float32 getCompressionLevel() const;
    bool compressEntry(uint32_t glyphIndex, float32 size, GlyphCacheType type);
    bool decompressEntry(uint32_t glyphIndex, float32 size, GlyphCacheType type);
    
    // Cache statistics
    GlyphCacheStatistics getStatistics() const;
    void updateStatistics();
    void resetStatistics();
    void enableStatistics(bool enabled);
    bool isStatisticsEnabled() const;
    
    // Cache monitoring
    size_t getEntryCount() const;
    size_t getEntryCount(GlyphCacheType type) const;
    size_t getEntryCount(GlyphCacheQuality quality) const;
    size_t getMemoryUsage() const;
    size_t getMemoryUsage(GlyphCacheType type) const;
    size_t getMemoryUsage(GlyphCacheQuality quality) const;
    float32 getHitRatio() const;
    float32 getHitRatio(GlyphCacheType type) const;
    std::vector<uint32_t> getCachedGlyphs() const;
    std::vector<uint32_t> getCachedGlyphs(GlyphCacheType type) const;
    
    // Cache debugging
    void enableDebugging(bool enabled);
    bool isDebuggingEnabled() const;
    std::vector<std::string> getDebugMessages() const;
    void clearDebugMessages();
    void dumpCache() const;
    std::string dumpCacheToString() const;
    
    // Event handling
    void addGlyphCacheEventListener(const std::string& eventType, std::function<void()> callback);
    void removeGlyphCacheEventListener(const std::string& eventType, std::function<void()> callback);
    void clearGlyphCacheEventListeners();
    
    // Utility methods
    void cloneFrom(const GlyphCache& other);
    virtual std::unique_ptr<GlyphCache> clone() const;
    bool equals(const GlyphCache& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::tuple<uint32_t, float32, GlyphCacheType, GlyphCacheQuality>, GlyphCacheEntry> cache_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    GlyphCacheConfig config_;
    GlyphCacheStatistics statistics_;
    
    bool isInitialized_;
    bool statisticsEnabled_;
    bool debuggingEnabled_;
    bool compressionEnabled_;
    
    size_t currentMemoryUsage_;
    std::vector<std::string> debugMessages_;
    
    // Protected helper methods
    virtual void triggerGlyphCacheEvent(const std::string& eventType);
    virtual std::string generateCacheKey(uint32_t glyphIndex, float32 size, GlyphCacheType type, GlyphCacheQuality quality) const;
    virtual bool shouldEvict() const;
    virtual std::tuple<uint32_t, float32, GlyphCacheType, GlyphCacheQuality> selectEvictionCandidate();
    virtual void evictEntry(const std::tuple<uint32_t, float32, GlyphCacheType, GlyphCacheQuality>& key);
    virtual void updateEntryAccess(const std::tuple<uint32_t, float32, GlyphCacheType, GlyphCacheQuality>& key);
    virtual void cleanupExpiredEntries();
    virtual void updateStatisticsInternal();
    
    // Validation helpers
    virtual bool validateCacheEntry(const GlyphCacheEntry& entry) const;
    virtual bool isValidGlyphIndex(uint32_t glyphIndex) const;
    virtual bool isValidSize(float32 size) const;
    virtual bool isValidType(GlyphCacheType type) const;
    virtual bool isValidQuality(GlyphCacheQuality quality) const;
    
    // Compression helpers
    virtual std::vector<uint8> compressData(const std::vector<uint8>& data) const;
    virtual std::vector<uint8> decompressData(const std::vector<uint8>& data) const;
    virtual size_t getCompressedSize(const std::vector<uint8>& data) const;
    
    // Statistics helpers
    virtual void updateHitStatistics(bool hit);
    virtual void updateMemoryStatistics(size_t oldSize, size_t newSize);
    virtual void updateAccessStatistics(const GlyphCacheEntry& entry);
    virtual void calculateStatistics();
    
    // Debug helpers
    virtual void addDebugMessage(const std::string& message);
    virtual void logCacheOperation(const std::string& operation, uint32_t glyphIndex, float32 size, GlyphCacheType type);
    virtual std::string formatCacheEntry(const GlyphCacheEntry& entry) const;
};

// Specialized glyph caches
class BitmapGlyphCache : public GlyphCache {
public:
    BitmapGlyphCache();
    bool putGlyphBitmap(uint32_t glyphIndex, float32 size, const GlyphBitmap& bitmap, 
                       GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM) override;
    bool getGlyphBitmap(uint32_t glyphIndex, float32 size, GlyphBitmap& bitmap, 
                       GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM) override;
    std::unique_ptr<GlyphCache> clone() const override;
    
protected:
    bool validateCacheEntry(const GlyphCacheEntry& entry) const override;
};

class OutlineGlyphCache : public GlyphCache {
public:
    OutlineGlyphCache();
    bool putGlyphOutline(uint32_t glyphIndex, float32 size, const GlyphOutline& outline, 
                        GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM) override;
    bool getGlyphOutline(uint32_t glyphIndex, float32 size, GlyphOutline& outline, 
                        GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM) override;
    std::unique_ptr<GlyphCache> clone() const override;
    
protected:
    bool validateCacheEntry(const GlyphCacheEntry& entry) const override;
};

class SDFGlyphCache : public GlyphCache {
public:
    SDFGlyphCache();
    bool putGlyphSDF(uint32_t glyphIndex, float32 size, const GlyphSDF& sdf, 
                    GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM) override;
    bool getGlyphSDF(uint32_t glyphIndex, float32 size, GlyphSDF& sdf, 
                    GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM) override;
    std::unique_ptr<GlyphCache> clone() const override;
    
protected:
    bool validateCacheEntry(const GlyphCacheEntry& entry) const override;
};

class MSDFGlyphCache : public GlyphCache {
public:
    MSDFGlyphCache();
    bool putGlyphMSDF(uint32_t glyphIndex, float32 size, const GlyphMSDF& msdf, 
                     GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM) override;
    bool getGlyphMSDF(uint32_t glyphIndex, float32 size, GlyphMSDF& msdf, 
                     GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM) override;
    std::unique_ptr<GlyphCache> clone() const override;
    
protected:
    bool validateCacheEntry(const GlyphCacheEntry& entry) const override;
};

class MetricsGlyphCache : public GlyphCache {
public:
    MetricsGlyphCache();
    bool putGlyphMetrics(uint32_t glyphIndex, float32 size, const GlyphMetrics& metrics, 
                        GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM) override;
    bool getGlyphMetrics(uint32_t glyphIndex, float32 size, GlyphMetrics& metrics, 
                        GlyphCacheQuality quality = GlyphCacheQuality::MEDIUM) override;
    std::unique_ptr<GlyphCache> clone() const override;
    
protected:
    bool validateCacheEntry(const GlyphCacheEntry& entry) const override;
};

// Glyph cache factory
class GlyphCacheFactory {
public:
    static std::unique_ptr<GlyphCache> createGlyphCache(GlyphCacheType type);
    static std::unique_ptr<BitmapGlyphCache> createBitmapGlyphCache();
    static std::unique_ptr<OutlineGlyphCache> createOutlineGlyphCache();
    static std::unique_ptr<SDFGlyphCache> createSDFGlyphCache();
    static std::unique_ptr<MSDFGlyphCache> createMSDFGlyphCache();
    static std::unique_ptr<MetricsGlyphCache> createMetricsGlyphCache();
    static GlyphCacheConfig createDefaultConfig(GlyphCacheType type);
    static std::vector<GlyphCacheType> getSupportedCacheTypes();
    static std::vector<GlyphCacheQuality> getSupportedQualityLevels();
    static std::vector<GlyphCacheFormat> getSupportedFormats();
};

} // namespace RFCore