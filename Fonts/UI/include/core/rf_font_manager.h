#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFCore {

// Font manager types
enum class FontManagerType {
    BASIC,
    ADVANCED,
    CACHED,
    STREAMING,
    CUSTOM
};

// Font loading strategies
enum class FontLoadingStrategy {
    EAGER,
    LAZY,
    ON_DEMAND,
    PRELOAD,
    STREAMING
};

// Font caching strategies
enum class FontCachingStrategy {
    NONE,
    LRU,
    LFU,
    FIFO,
    CUSTOM
};

// Font priority
enum class FontPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};

// Font manager configuration
struct FontManagerConfig {
    FontManagerType type;
    FontLoadingStrategy loadingStrategy;
    FontCachingStrategy cachingStrategy;
    size_t maxCacheSize;
    size_t maxLoadedFonts;
    size_t preloadSize;
    bool enableStreaming;
    bool enableCompression;
    bool enableEncryption;
    bool enableValidation;
    bool enableMetrics;
    bool enableKerning;
    bool enableVariations;
    bool enableFeatures;
    
    FontManagerConfig() : type(FontManagerType::ADVANCED), loadingStrategy(FontLoadingStrategy::LAZY), 
                         cachingStrategy(FontCachingStrategy::LRU), maxCacheSize(100), maxLoadedFonts(50), 
                         preloadSize(10), enableStreaming(false), enableCompression(false), 
                         enableEncryption(false), enableValidation(true), enableMetrics(true), 
                         enableKerning(true), enableVariations(true), enableFeatures(true) {}
};

// Font entry
struct FontEntry {
    std::string filename;
    std::string familyName;
    std::string styleName;
    FontType type;
    FontStyle style;
    FontWeight weight;
    FontStretch stretch;
    uint32_t faceIndex;
    FontPriority priority;
    bool isLoaded;
    bool isCached;
    uint64_t loadTime;
    uint64_t accessTime;
    uint32_t accessCount;
    size_t memoryUsage;
    
    FontEntry() : type(FontType::TRUETYPE), style(FontStyle::NORMAL), weight(FontWeight::NORMAL), 
                 stretch(FontStretch::NORMAL), faceIndex(0), priority(FontPriority::NORMAL), 
                 isLoaded(false), isCached(false), loadTime(0), accessTime(0), accessCount(0), 
                 memoryUsage(0) {}
};

// Font cache entry
struct FontCacheEntry {
    std::string key;
    std::unique_ptr<Font> font;
    uint64_t timestamp;
    uint64_t accessTime;
    uint32_t accessCount;
    size_t memoryUsage;
    FontPriority priority;
    
    FontCacheEntry() : timestamp(0), accessTime(0), accessCount(0), memoryUsage(0), 
                      priority(FontPriority::NORMAL) {}
    FontCacheEntry(const std::string& k, std::unique_ptr<Font> f, uint64_t ts, FontPriority prio)
        : key(k), font(std::move(f)), timestamp(ts), accessTime(ts), accessCount(1), 
          memoryUsage(0), priority(prio) {}
};

// Font manager statistics
struct FontManagerStatistics {
    uint32_t totalFonts;
    uint32_t loadedFonts;
    uint32_t cachedFonts;
    uint32_t activeFonts;
    uint32_t failedLoads;
    uint64_t totalMemoryUsage;
    uint64_t cacheMemoryUsage;
    float32 averageLoadTime;
    float32 averageAccessTime;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    float32 cacheHitRatio;
    uint64_t totalAccesses;
    
    FontManagerStatistics() : totalFonts(0), loadedFonts(0), cachedFonts(0), activeFonts(0), 
                             failedLoads(0), totalMemoryUsage(0), cacheMemoryUsage(0), 
                             averageLoadTime(0.0f), averageAccessTime(0.0f), cacheHits(0), 
                             cacheMisses(0), cacheHitRatio(0.0f), totalAccesses(0) {}
};

// Font manager
class FontManager {
public:
    FontManager();
    virtual ~FontManager() = default;
    
    // Font manager management
    void initialize(const FontManagerConfig& config);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Configuration
    void setConfiguration(const FontManagerConfig& config);
    const FontManagerConfig& getConfiguration() const;
    void setMaxCacheSize(size_t maxSize);
    void setMaxLoadedFonts(size_t maxFonts);
    void setLoadingStrategy(FontLoadingStrategy strategy);
    void setCachingStrategy(FontCachingStrategy strategy);
    
    // Font registration
    bool registerFont(const std::string& filename, const std::string& familyName, 
                     const std::string& styleName = "", uint32_t faceIndex = 0);
    bool registerFont(const FontEntry& entry);
    bool unregisterFont(const std::string& filename);
    bool unregisterFont(const std::string& familyName, const std::string& styleName = "");
    bool isFontRegistered(const std::string& filename) const;
    bool isFontRegistered(const std::string& familyName, const std::string& styleName = "") const;
    
    // Font loading
    std::shared_ptr<Font> loadFont(const std::string& filename, uint32_t faceIndex = 0);
    std::shared_ptr<Font> loadFont(const std::string& familyName, const std::string& styleName = "");
    std::shared_ptr<Font> loadFont(const FontEntry& entry);
    bool preloadFont(const std::string& filename, uint32_t faceIndex = 0);
    bool preloadFont(const std::string& familyName, const std::string& styleName = "");
    void preloadFonts(const std::vector<std::string>& filenames);
    void unloadFont(const std::string& filename);
    void unloadFont(const std::string& familyName, const std::string& styleName = "");
    void unloadAllFonts();
    
    // Font access
    std::shared_ptr<Font> getFont(const std::string& filename, uint32_t faceIndex = 0);
    std::shared_ptr<Font> getFont(const std::string& familyName, const std::string& styleName = "");
    std::shared_ptr<Font> getFont(const FontEntry& entry);
    bool hasFont(const std::string& filename, uint32_t faceIndex = 0) const;
    bool hasFont(const std::string& familyName, const std::string& styleName = "") const;
    
    // Font discovery
    std::vector<std::string> getFontFamilies() const;
    std::vector<std::string> getFontStyles(const std::string& familyName) const;
    std::vector<FontEntry> getFontEntries() const;
    std::vector<FontEntry> getFontEntries(FontType type) const;
    std::vector<FontEntry> getFontEntries(FontStyle style) const;
    std::vector<FontEntry> getFontEntries(FontWeight weight) const;
    FontEntry findFont(const std::string& familyName, const std::string& styleName = "") const;
    FontEntry findClosestFont(const std::string& familyName, FontStyle style = FontStyle::NORMAL, 
                            FontWeight weight = FontWeight::NORMAL) const;
    
    // Font matching
    std::shared_ptr<Font> matchFont(const std::string& familyName, FontStyle style = FontStyle::NORMAL, 
                                  FontWeight weight = FontWeight::NORMAL, FontStretch stretch = FontStretch::NORMAL);
    std::shared_ptr<Font> matchFont(const std::vector<std::string>& familyNames, FontStyle style = FontStyle::NORMAL, 
                                  FontWeight weight = FontWeight::NORMAL, FontStretch stretch = FontStretch::NORMAL);
    std::vector<std::shared_ptr<Font>> matchFonts(const std::string& familyName, FontStyle style = FontStyle::NORMAL, 
                                                 FontWeight weight = FontWeight::NORMAL, FontStretch stretch = FontStretch::NORMAL);
    
    // Font fallback
    std::shared_ptr<Font> getFallbackFont(uint32_t charCode, const std::string& preferredFamily = "");
    std::shared_ptr<Font> getFallbackFont(const std::string& text, const std::string& preferredFamily = "");
    void setFallbackFont(const std::string& familyName, const std::string& styleName = "");
    void addFallbackFont(const std::string& familyName, const std::string& styleName = "");
    void removeFallbackFont(const std::string& familyName, const std::string& styleName = "");
    std::vector<std::string> getFallbackFonts() const;
    
    // Font streaming
    bool enableStreaming(const std::string& filename);
    bool disableStreaming(const std::string& filename);
    bool isStreamingEnabled(const std::string& filename) const;
    void streamFontAsync(const std::string& filename, std::function<void(std::shared_ptr<Font>)> callback);
    void streamFontAsync(const std::string& familyName, const std::string& styleName, 
                       std::function<void(std::shared_ptr<Font>)> callback);
    
    // Font validation
    bool validateFont(const std::string& filename) const;
    bool validateFont(const std::string& familyName, const std::string& styleName = "") const;
    bool validateFont(const FontEntry& entry) const;
    std::vector<std::string> getValidationErrors(const std::string& filename) const;
    
    // Font metrics
    bool getFontMetrics(const std::string& filename, FontMetrics& metrics, float32 fontSize = 12.0f) const;
    bool getFontMetrics(const std::string& familyName, const std::string& styleName, 
                      FontMetrics& metrics, float32 fontSize = 12.0f) const;
    
    // Font kerning
    bool hasKerning(const std::string& filename) const;
    bool hasKerning(const std::string& familyName, const std::string& styleName = "") const;
    
    // Font variations
    bool hasVariations(const std::string& filename) const;
    bool hasVariations(const std::string& familyName, const std::string& styleName = "") const;
    std::vector<FontVariation> getVariations(const std::string& filename) const;
    std::vector<FontVariation> getVariations(const std::string& familyName, const std::string& styleName = "") const;
    
    // Font features
    bool hasFeatures(const std::string& filename) const;
    bool hasFeatures(const std::string& familyName, const std::string& styleName = "") const;
    std::vector<FontFeature> getFeatures(const std::string& filename) const;
    std::vector<FontFeature> getFeatures(const std::string& familyName, const std::string& styleName = "") const;
    
    // Cache management
    void clearCache();
    void clearCache(const std::string& filename);
    void clearCache(const std::string& familyName, const std::string& styleName = "");
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    float32 getCacheUtilization() const;
    void optimizeCache();
    void compactCache();
    
    // Memory management
    size_t getMemoryUsage() const;
    size_t getFontMemoryUsage(const std::string& filename) const;
    size_t getFontMemoryUsage(const std::string& familyName, const std::string& styleName = "") const;
    void setMemoryLimit(size_t limit);
    size_t getMemoryLimit() const;
    void cleanupMemory();
    
    // Statistics
    FontManagerStatistics getStatistics() const;
    void updateStatistics();
    void resetStatistics();
    void enableStatistics(bool enabled);
    bool isStatisticsEnabled() const;
    
    // Batch operations
    void loadFontsAsync(const std::vector<std::string>& filenames, 
                       std::function<void(std::shared_ptr<Font>)> callback);
    void unloadFonts(const std::vector<std::string>& filenames);
    void preloadFonts(const std::vector<FontEntry>& entries);
    
    // Event handling
    void addFontManagerEventListener(const std::string& eventType, std::function<void()> callback);
    void removeFontManagerEventListener(const std::string& eventType, std::function<void()> callback);
    void clearFontManagerEventListeners();
    
    // Utility methods
    void cloneFrom(const FontManager& other);
    virtual std::unique_ptr<FontManager> clone() const;
    bool equals(const FontManager& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    FontManagerConfig config_;
    std::map<std::string, FontEntry> fontEntries_;
    std::map<std::string, std::shared_ptr<Font>> loadedFonts_;
    std::map<std::string, FontCacheEntry> fontCache_;
    std::vector<std::string> fallbackFonts_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    FontManagerStatistics statistics_;
    bool isInitialized_;
    bool statisticsEnabled_;
    
    // Protected helper methods
    virtual void triggerFontManagerEvent(const std::string& eventType);
    virtual std::shared_ptr<Font> loadFontInternal(const FontEntry& entry);
    virtual std::shared_ptr<Font> loadFontFromCache(const std::string& key);
    virtual void cacheFont(const std::string& key, std::shared_ptr<Font> font);
    virtual void uncacheFont(const std::string& key);
    virtual void updateCache();
    virtual void cleanupCache();
    virtual bool shouldCacheFont(const FontEntry& entry) const;
    virtual bool shouldUnloadFont(const std::string& key) const;
    virtual std::string generateCacheKey(const FontEntry& entry) const;
    
    // Font matching helpers
    virtual bool matchesStyle(const FontEntry& entry, FontStyle style) const;
    virtual bool matchesWeight(const FontEntry& entry, FontWeight weight) const;
    virtual bool matchesStretch(const FontEntry& entry, FontStretch stretch) const;
    virtual float32 calculateMatchScore(const FontEntry& entry, FontStyle style, 
                                      FontWeight weight, FontStretch stretch) const;
    
    // Validation helpers
    virtual bool validateFontFile(const std::string& filename) const;
    virtual bool validateFontEntry(const FontEntry& entry) const;
    virtual std::vector<std::string> getFontValidationErrors(const FontEntry& entry) const;
    
    // Statistics helpers
    virtual void updateLoadStatistics(uint64_t loadTime, size_t memoryUsage);
    virtual void updateAccessStatistics(uint64_t accessTime);
    virtual void updateCacheStatistics(bool hit);
    virtual void calculateStatistics();
};

// Specialized font managers
class BasicFontManager : public FontManager {
public:
    BasicFontManager();
    std::shared_ptr<Font> loadFont(const std::string& filename, uint32_t faceIndex = 0) override;
    std::shared_ptr<Font> getFont(const std::string& filename, uint32_t faceIndex = 0) override;
    std::unique_ptr<FontManager> clone() const override;
    
protected:
    void triggerFontManagerEvent(const std::string& eventType) override;
    std::shared_ptr<Font> loadFontInternal(const FontEntry& entry) override;
};

class CachedFontManager : public FontManager {
public:
    CachedFontManager();
    void setMaxCacheSize(size_t maxSize) override;
    void clearCache() override;
    size_t getCacheSize() const override;
    size_t getCacheUsage() const override;
    std::unique_ptr<FontManager> clone() const override;
    
protected:
    void triggerFontManagerEvent(const std::string& eventType) override;
    void updateCache() override;
    void cleanupCache() override;
    bool shouldCacheFont(const FontEntry& entry) const override;
    bool shouldUnloadFont(const std::string& key) const override;
};

class StreamingFontManager : public FontManager {
public:
    StreamingFontManager();
    bool enableStreaming(const std::string& filename) override;
    void streamFontAsync(const std::string& filename, std::function<void(std::shared_ptr<Font>)> callback) override;
    std::unique_ptr<FontManager> clone() const override;
    
protected:
    void triggerFontManagerEvent(const std::string& eventType) override;
    std::shared_ptr<Font> loadFontInternal(const FontEntry& entry) override;
};

// Font manager factory
class FontManagerFactory {
public:
    static std::unique_ptr<FontManager> createFontManager(FontManagerType type);
    static std::unique_ptr<BasicFontManager> createBasicFontManager();
    static std::unique_ptr<CachedFontManager> createCachedFontManager();
    static std::unique_ptr<StreamingFontManager> createStreamingFontManager();
    static FontManagerConfig createDefaultConfig(FontManagerType type);
    static std::vector<FontManagerType> getSupportedFontManagerTypes();
    static std::vector<FontLoadingStrategy> getSupportedLoadingStrategies();
    static std::vector<FontCachingStrategy> getSupportedCachingStrategies();
};

} // namespace RFCore