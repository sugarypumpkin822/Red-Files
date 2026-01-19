#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>
#include <fstream>
#include <sstream>

namespace RFCore {

// Font loader types
enum class FontLoaderType {
    FREETYPE,
    HARFBUZZ,
    DIRECTWRITE,
    CORETEXT,
    STB_TRUETYPE,
    CUSTOM
};

// Font loading modes
enum class FontLoadingMode {
    SYNCHRONOUS,
    ASYNCHRONOUS,
    STREAMING,
    LAZY
};

// Font loading states
enum class FontLoadingState {
    IDLE,
    LOADING,
    LOADED,
    FAILED,
    CANCELLED
};

// Font loading priority
enum class FontLoadingPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};

// Font loading options
struct FontLoadingOptions {
    FontLoadingMode mode;
    FontLoadingPriority priority;
    bool enableMetrics;
    bool enableKerning;
    bool enableVariations;
    bool enableFeatures;
    bool enableColorGlyphs;
    bool enableSubpixel;
    float32 dpiScale;
    uint32_t faceIndex;
    std::map<std::string, float32> variations;
    std::vector<std::string> enabledFeatures;
    
    FontLoadingOptions() : mode(FontLoadingMode::SYNCHRONOUS), priority(FontLoadingPriority::NORMAL), 
                         enableMetrics(true), enableKerning(true), enableVariations(false), 
                         enableFeatures(false), enableColorGlyphs(true), enableSubpixel(false), 
                         dpiScale(1.0f), faceIndex(0) {}
};

// Font loading result
struct FontLoadingResult {
    bool success;
    FontLoadingState state;
    std::string errorMessage;
    std::unique_ptr<Font> font;
    uint64_t loadTime;
    size_t memoryUsage;
    FontLoadingOptions options;
    
    FontLoadingResult() : success(false), state(FontLoadingState::IDLE), loadTime(0), memoryUsage(0) {}
    FontLoadingResult(bool succ, FontLoadingState st, const std::string& err, 
                    std::unique_ptr<Font> f, uint64_t time, size_t mem, const FontLoadingOptions& opts)
        : success(succ), state(st), errorMessage(err), font(std::move(f)), 
          loadTime(time), memoryUsage(mem), options(opts) {}
};

// Font loading request
struct FontLoadingRequest {
    std::string identifier;
    std::string filename;
    std::vector<uint8> data;
    FontLoadingOptions options;
    std::function<void(const FontLoadingResult&)> callback;
    uint64_t requestId;
    uint64_t timestamp;
    FontLoadingState state;
    
    FontLoadingRequest() : requestId(0), timestamp(0), state(FontLoadingState::IDLE) {}
    FontLoadingRequest(const std::string& id, const std::string& file, const FontLoadingOptions& opts,
                     std::function<void(const FontLoadingResult&)> cb, uint64_t reqId, uint64_t ts)
        : identifier(id), filename(file), data(), options(opts), callback(cb), 
          requestId(reqId), timestamp(ts), state(FontLoadingState::IDLE) {}
};

// Font loader statistics
struct FontLoaderStatistics {
    uint32_t totalRequests;
    uint32_t successfulLoads;
    uint32_t failedLoads;
    uint32_t cancelledLoads;
    uint32_t activeRequests;
    uint64_t totalLoadTime;
    uint64_t totalMemoryUsage;
    float32 averageLoadTime;
    float32 averageMemoryUsage;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    float32 cacheHitRatio;
    
    FontLoaderStatistics() : totalRequests(0), successfulLoads(0), failedLoads(0), 
                           cancelledLoads(0), activeRequests(0), totalLoadTime(0), 
                           totalMemoryUsage(0), averageLoadTime(0.0f), averageMemoryUsage(0.0f), 
                           cacheHits(0), cacheMisses(0), cacheHitRatio(0.0f) {}
};

// Font loader
class FontLoader {
public:
    FontLoader();
    virtual ~FontLoader() = default;
    
    // Font loader management
    void initialize();
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Font loading
    FontLoadingResult loadFont(const std::string& filename, const FontLoadingOptions& options = FontLoadingOptions());
    FontLoadingResult loadFontFromMemory(const std::vector<uint8>& data, const FontLoadingOptions& options = FontLoadingOptions());
    FontLoadingResult loadFontFromStream(std::istream& stream, const FontLoadingOptions& options = FontLoadingOptions());
    
    // Asynchronous loading
    uint64_t loadFontAsync(const std::string& filename, const FontLoadingOptions& options = FontLoadingOptions(),
                          std::function<void(const FontLoadingResult&)> callback = nullptr);
    uint64_t loadFontAsync(const std::vector<uint8>& data, const FontLoadingOptions& options = FontLoadingOptions(),
                          std::function<void(const FontLoadingResult&)> callback = nullptr);
    uint64_t loadFontAsync(std::istream& stream, const FontLoadingOptions& options = FontLoadingOptions(),
                          std::function<void(const FontLoadingResult&)> callback = nullptr);
    
    // Request management
    bool cancelRequest(uint64_t requestId);
    bool cancelRequest(const std::string& identifier);
    void cancelAllRequests();
    FontLoadingState getRequestState(uint64_t requestId) const;
    FontLoadingState getRequestState(const std::string& identifier) const;
    std::vector<uint64_t> getActiveRequests() const;
    std::vector<std::string> getActiveIdentifiers() const;
    
    // Font validation
    bool validateFont(const std::string& filename) const;
    bool validateFont(const std::vector<uint8>& data) const;
    bool validateFont(std::istream& stream) const;
    std::vector<std::string> getValidationErrors(const std::string& filename) const;
    std::vector<std::string> getValidationErrors(const std::vector<uint8>& data) const;
    std::vector<std::string> getValidationErrors(std::istream& stream) const;
    
    // Font information
    bool getFontInfo(const std::string& filename, FontInfo& info) const;
    bool getFontInfo(const std::vector<uint8>& data, FontInfo& info) const;
    bool getFontInfo(std::istream& stream, FontInfo& info) const;
    
    // Font format detection
    FontType detectFontType(const std::string& filename) const;
    FontType detectFontType(const std::vector<uint8>& data) const;
    FontType detectFontType(std::istream& stream) const;
    
    // Font face enumeration
    std::vector<FontInfo> getFontFaces(const std::string& filename) const;
    std::vector<FontInfo> getFontFaces(const std::vector<uint8>& data) const;
    std::vector<FontInfo> getFontFaces(std::istream& stream) const;
    uint32_t getFaceCount(const std::string& filename) const;
    uint32_t getFaceCount(const std::vector<uint8>& data) const;
    uint32_t getFaceCount(std::istream& stream) const;
    
    // Font caching
    void enableCache(bool enabled);
    bool isCacheEnabled() const;
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    void clearCache();
    void clearCache(const std::string& filename);
    
    // Font streaming
    void enableStreaming(bool enabled);
    bool isStreamingEnabled() const;
    void setStreamChunkSize(size_t chunkSize);
    size_t getStreamChunkSize() const;
    
    // Font preprocessing
    void enablePreprocessing(bool enabled);
    bool isPreprocessingEnabled() const;
    void setPreprocessingOptions(const FontLoadingOptions& options);
    const FontLoadingOptions& getPreprocessingOptions() const;
    
    // Font postprocessing
    void enablePostprocessing(bool enabled);
    bool isPostprocessingEnabled() const;
    void setPostprocessingOptions(const FontLoadingOptions& options);
    const FontLoadingOptions& getPostprocessingOptions() const;
    
    // Font optimization
    void enableOptimization(bool enabled);
    bool isOptimizationEnabled() const;
    void setOptimizationLevel(uint32_t level);
    uint32_t getOptimizationLevel() const;
    
    // Font compression
    void enableCompression(bool enabled);
    bool isCompressionEnabled() const;
    void setCompressionLevel(uint32_t level);
    uint32_t getCompressionLevel() const;
    
    // Font encryption
    void enableEncryption(bool enabled);
    bool isEncryptionEnabled() const;
    void setEncryptionKey(const std::vector<uint8>& key);
    std::vector<uint8> getEncryptionKey() const;
    
    // Font debugging
    void enableDebugging(bool enabled);
    bool isDebuggingEnabled() const;
    void setDebugLevel(uint32_t level);
    uint32_t getDebugLevel() const;
    std::vector<std::string> getDebugMessages() const;
    void clearDebugMessages();
    
    // Statistics
    FontLoaderStatistics getStatistics() const;
    void updateStatistics();
    void resetStatistics();
    void enableStatistics(bool enabled);
    bool isStatisticsEnabled() const;
    
    // Batch operations
    std::vector<FontLoadingResult> loadFonts(const std::vector<std::string>& filenames, 
                                           const FontLoadingOptions& options = FontLoadingOptions());
    std::vector<FontLoadingResult> loadFontsFromMemory(const std::vector<std::vector<uint8>>& dataList, 
                                                      const FontLoadingOptions& options = FontLoadingOptions());
    std::vector<uint64_t> loadFontsAsync(const std::vector<std::string>& filenames, 
                                         const FontLoadingOptions& options = FontLoadingOptions(),
                                         std::function<void(const FontLoadingResult&)> callback = nullptr);
    
    // Event handling
    void addFontLoaderEventListener(const std::string& eventType, std::function<void()> callback);
    void removeFontLoaderEventListener(const std::string& eventType, std::function<void()> callback);
    void clearFontLoaderEventListeners();
    
    // Utility methods
    void cloneFrom(const FontLoader& other);
    virtual std::unique_ptr<FontLoader> clone() const;
    bool equals(const FontLoader& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<uint64_t, FontLoadingRequest> loadingRequests_;
    std::map<std::string, uint64_t> identifierToRequest_;
    std::map<std::string, std::unique_ptr<Font>> fontCache_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    FontLoaderStatistics statistics_;
    FontLoadingOptions preprocessingOptions_;
    FontLoadingOptions postprocessingOptions_;
    
    bool isInitialized_;
    bool cacheEnabled_;
    bool streamingEnabled_;
    bool preprocessingEnabled_;
    bool postprocessingEnabled_;
    bool optimizationEnabled_;
    bool compressionEnabled_;
    bool encryptionEnabled_;
    bool debuggingEnabled_;
    bool statisticsEnabled_;
    
    size_t maxCacheSize_;
    size_t streamChunkSize_;
    uint32_t optimizationLevel_;
    uint32_t compressionLevel_;
    uint32_t debugLevel_;
    uint64_t nextRequestId_;
    std::vector<uint8> encryptionKey_;
    
    // Protected helper methods
    virtual void triggerFontLoaderEvent(const std::string& eventType);
    virtual FontLoadingResult loadFontInternal(const std::string& filename, const FontLoadingOptions& options);
    virtual FontLoadingResult loadFontInternal(const std::vector<uint8>& data, const FontLoadingOptions& options);
    virtual FontLoadingResult loadFontInternal(std::istream& stream, const FontLoadingOptions& options);
    virtual void processRequest(FontLoadingRequest& request);
    virtual void processAsyncRequests();
    virtual void updateCache();
    virtual void cleanupCache();
    
    // Validation helpers
    virtual bool validateFontFile(const std::string& filename) const;
    virtual bool validateFontData(const std::vector<uint8>& data) const;
    virtual bool validateFontStream(std::istream& stream) const;
    virtual std::vector<std::string> getFontValidationErrors(const std::string& filename) const;
    virtual std::vector<std::string> getFontValidationErrors(const std::vector<uint8>& data) const;
    virtual std::vector<std::string> getFontValidationErrors(std::istream& stream) const;
    
    // Format detection helpers
    virtual FontType detectFontFormat(const std::vector<uint8>& data) const;
    virtual bool isTrueTypeFont(const std::vector<uint8>& data) const;
    virtual bool isOpenTypeFont(const std::vector<uint8>& data) const;
    virtual bool isWOFFFont(const std::vector<uint8>& data) const;
    virtual bool isBitmapFont(const std::vector<uint8>& data) const;
    
    // Cache helpers
    virtual std::string generateCacheKey(const std::string& filename, const FontLoadingOptions& options) const;
    virtual bool getFromCache(const std::string& key, std::unique_ptr<Font>& font) const;
    virtual void addToCache(const std::string& key, std::unique_ptr<Font> font);
    virtual void removeFromCache(const std::string& key);
    
    // Preprocessing helpers
    virtual FontLoadingResult preprocessFont(const FontLoadingResult& result);
    virtual FontLoadingResult preprocessMetrics(const FontLoadingResult& result);
    virtual FontLoadingResult preprocessKerning(const FontLoadingResult& result);
    virtual FontLoadingResult preprocessVariations(const FontLoadingResult& result);
    virtual FontLoadingResult preprocessFeatures(const FontLoadingResult& result);
    
    // Postprocessing helpers
    virtual FontLoadingResult postprocessFont(const FontLoadingResult& result);
    virtual FontLoadingResult optimizeFont(const FontLoadingResult& result);
    virtual FontLoadingResult compressFont(const FontLoadingResult& result);
    virtual FontLoadingResult encryptFont(const FontLoadingResult& result);
    
    // Statistics helpers
    virtual void updateLoadStatistics(uint64_t loadTime, size_t memoryUsage, bool success);
    virtual void updateCacheStatistics(bool hit);
    virtual void calculateStatistics();
};

// Specialized font loaders
class FreeTypeFontLoader : public FontLoader {
public:
    FreeTypeFontLoader();
    FontLoadingResult loadFontInternal(const std::string& filename, const FontLoadingOptions& options) override;
    FontLoadingResult loadFontInternal(const std::vector<uint8>& data, const FontLoadingOptions& options) override;
    FontType detectFontType(const std::vector<uint8>& data) const override;
    std::unique_ptr<FontLoader> clone() const override;
    
protected:
    void triggerFontLoaderEvent(const std::string& eventType) override;
    bool validateFontData(const std::vector<uint8>& data) const override;
    FontType detectFontFormat(const std::vector<uint8>& data) const override;
};

class HarfbuzzFontLoader : public FontLoader {
public:
    HarfbuzzFontLoader();
    FontLoadingResult loadFontInternal(const std::string& filename, const FontLoadingOptions& options) override;
    FontLoadingResult loadFontInternal(const std::vector<uint8>& data, const FontLoadingOptions& options) override;
    std::unique_ptr<FontLoader> clone() const override;
    
protected:
    void triggerFontLoaderEvent(const std::string& eventType) override;
    FontLoadingResult preprocessFont(const FontLoadingResult& result) override;
};

class StbTrueTypeFontLoader : public FontLoader {
public:
    StbTrueTypeFontLoader();
    FontLoadingResult loadFontInternal(const std::string& filename, const FontLoadingOptions& options) override;
    FontLoadingResult loadFontInternal(const std::vector<uint8>& data, const FontLoadingOptions& options) override;
    FontType detectFontType(const std::vector<uint8>& data) const override;
    std::unique_ptr<FontLoader> clone() const override;
    
protected:
    void triggerFontLoaderEvent(const std::string& eventType) override;
    bool validateFontData(const std::vector<uint8>& data) const override;
    FontType detectFontFormat(const std::vector<uint8>& data) const override;
};

// Font loader factory
class FontLoaderFactory {
public:
    static std::unique_ptr<FontLoader> createFontLoader(FontLoaderType type);
    static std::unique_ptr<FreeTypeFontLoader> createFreeTypeFontLoader();
    static std::unique_ptr<HarfbuzzFontLoader> createHarfbuzzFontLoader();
    static std::unique_ptr<StbTrueTypeFontLoader> createStbTrueTypeFontLoader();
    static FontLoaderType detectBestLoader(const std::string& filename);
    static FontLoaderType detectBestLoader(const std::vector<uint8>& data);
    static std::vector<FontLoaderType> getSupportedLoaderTypes();
    static std::vector<FontLoadingMode> getSupportedLoadingModes();
    static std::vector<FontLoadingPriority> getSupportedLoadingPriorities();
};

} // namespace RFCore