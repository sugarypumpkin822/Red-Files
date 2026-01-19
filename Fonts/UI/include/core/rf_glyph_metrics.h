#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFCore {

// Metric types
enum class MetricType {
    HORIZONTAL,
    VERTICAL,
    BOTH
};

// Metric units
enum class MetricUnit {
    PIXELS,
    POINTS,
    EM,
    PERCENT,
    UNITS_PER_EM,
    CUSTOM
};

// Metric precision
enum class MetricPrecision {
    INTEGER,
    FIXED_16_16,
    FIXED_26_6,
    FLOAT,
    DOUBLE
};

// Glyph metrics base
struct GlyphMetricsBase {
    uint32_t glyphIndex;
    uint32_t charCode;
    MetricType type;
    MetricUnit unit;
    MetricPrecision precision;
    
    GlyphMetricsBase() : glyphIndex(0), charCode(0), type(MetricType::HORIZONTAL), 
                        unit(MetricUnit::PIXELS), precision(MetricPrecision::FLOAT) {}
    GlyphMetricsBase(uint32_t idx, uint32_t code, MetricType t, MetricUnit u, MetricPrecision p)
        : glyphIndex(idx), charCode(code), type(t), unit(u), precision(p) {}
};

// Horizontal metrics
struct HorizontalMetrics : public GlyphMetricsBase {
    float32 advanceWidth;
    float32 leftSideBearing;
    float32 rightSideBearing;
    float32 width;
    float32 height;
    float32 horizontalBearingX;
    float32 horizontalBearingY;
    
    HorizontalMetrics() : GlyphMetricsBase(), advanceWidth(0.0f), leftSideBearing(0.0f), 
                         rightSideBearing(0.0f), width(0.0f), height(0.0f), 
                         horizontalBearingX(0.0f), horizontalBearingY(0.0f) {}
    HorizontalMetrics(uint32_t idx, uint32_t code, float32 adv, float32 lsb, float32 rsb, 
                     float32 w, float32 h, float32 bearX, float32 bearY)
        : GlyphMetricsBase(idx, code, MetricType::HORIZONTAL, MetricUnit::PIXELS, MetricPrecision::FLOAT),
          advanceWidth(adv), leftSideBearing(lsb), rightSideBearing(rsb), width(w), height(h), 
          horizontalBearingX(bearX), horizontalBearingY(bearY) {}
};

// Vertical metrics
struct VerticalMetrics : public GlyphMetricsBase {
    float32 advanceHeight;
    float32 topSideBearing;
    float32 bottomSideBearing;
    float32 width;
    float32 height;
    float32 verticalBearingX;
    float32 verticalBearingY;
    
    VerticalMetrics() : GlyphMetricsBase(), advanceHeight(0.0f), topSideBearing(0.0f), 
                       bottomSideBearing(0.0f), width(0.0f), height(0.0f), 
                       verticalBearingX(0.0f), verticalBearingY(0.0f) {}
    VerticalMetrics(uint32_t idx, uint32_t code, float32 adv, float32 tsb, float32 bsb, 
                   float32 w, float32 h, float32 bearX, float32 bearY)
        : GlyphMetricsBase(idx, code, MetricType::VERTICAL, MetricUnit::PIXELS, MetricPrecision::FLOAT),
          advanceHeight(adv), topSideBearing(tsb), bottomSideBearing(bsb), width(w), height(h), 
          verticalBearingX(bearX), verticalBearingY(bearY) {}
};

// Combined metrics
struct CombinedMetrics : public GlyphMetricsBase {
    HorizontalMetrics horizontal;
    VerticalMetrics vertical;
    
    CombinedMetrics() : GlyphMetricsBase() {}
    CombinedMetrics(uint32_t idx, uint32_t code, const HorizontalMetrics& horiz, const VerticalMetrics& vert)
        : GlyphMetricsBase(idx, code, MetricType::BOTH, MetricUnit::PIXELS, MetricPrecision::FLOAT),
          horizontal(horiz), vertical(vert) {}
};

// Scaled metrics
struct ScaledMetrics {
    float32 scale;
    float32 size;
    HorizontalMetrics horizontal;
    VerticalMetrics vertical;
    MetricUnit unit;
    
    ScaledMetrics() : scale(1.0f), size(12.0f), unit(MetricUnit::PIXELS) {}
    ScaledMetrics(float32 s, float32 sz, const HorizontalMetrics& horiz, const VerticalMetrics& vert, MetricUnit u)
        : scale(s), size(sz), horizontal(horiz), vertical(vert), unit(u) {}
};

// Metrics cache entry
struct MetricsCacheEntry {
    uint32_t glyphIndex;
    float32 size;
    MetricUnit unit;
    ScaledMetrics metrics;
    uint64_t timestamp;
    uint32_t accessCount;
    
    MetricsCacheEntry() : glyphIndex(0), size(0.0f), unit(MetricUnit::PIXELS), timestamp(0), accessCount(0) {}
    MetricsCacheEntry(uint32_t idx, float32 sz, MetricUnit u, const ScaledMetrics& m, uint64_t ts)
        : glyphIndex(idx), size(sz), unit(u), metrics(m), timestamp(ts), accessCount(1) {}
};

// Metrics statistics
struct MetricsStatistics {
    uint32_t totalGlyphs;
    uint32_t cachedGlyphs;
    float32 averageAdvanceWidth;
    float32 averageAdvanceHeight;
    float32 maxAdvanceWidth;
    float32 maxAdvanceHeight;
    float32 minAdvanceWidth;
    float32 minAdvanceHeight;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    float32 cacheHitRatio;
    
    MetricsStatistics() : totalGlyphs(0), cachedGlyphs(0), averageAdvanceWidth(0.0f), 
                         averageAdvanceHeight(0.0f), maxAdvanceWidth(0.0f), maxAdvanceHeight(0.0f), 
                         minAdvanceWidth(0.0f), minAdvanceHeight(0.0f), cacheHits(0), 
                         cacheMisses(0), cacheHitRatio(0.0f) {}
};

// Glyph metrics manager
class GlyphMetricsManager {
public:
    GlyphMetricsManager();
    virtual ~GlyphMetricsManager() = default;
    
    // Metrics management
    void initialize();
    void reset();
    void clearCache();
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    
    // Metrics access
    bool getHorizontalMetrics(uint32_t glyphIndex, HorizontalMetrics& metrics) const;
    bool getVerticalMetrics(uint32_t glyphIndex, VerticalMetrics& metrics) const;
    bool getCombinedMetrics(uint32_t glyphIndex, CombinedMetrics& metrics) const;
    bool getScaledMetrics(uint32_t glyphIndex, float32 size, ScaledMetrics& metrics) const;
    bool getScaledMetrics(uint32_t glyphIndex, float32 size, MetricUnit unit, ScaledMetrics& metrics) const;
    
    // Metrics setting
    void setHorizontalMetrics(uint32_t glyphIndex, const HorizontalMetrics& metrics);
    void setVerticalMetrics(uint32_t glyphIndex, const VerticalMetrics& metrics);
    void setCombinedMetrics(uint32_t glyphIndex, const CombinedMetrics& metrics);
    void setScaledMetrics(uint32_t glyphIndex, float32 size, const ScaledMetrics& metrics);
    
    // Metrics calculation
    HorizontalMetrics calculateHorizontalMetrics(uint32_t glyphIndex, float32 size = 1.0f) const;
    VerticalMetrics calculateVerticalMetrics(uint32_t glyphIndex, float32 size = 1.0f) const;
    CombinedMetrics calculateCombinedMetrics(uint32_t glyphIndex, float32 size = 1.0f) const;
    ScaledMetrics calculateScaledMetrics(uint32_t glyphIndex, float32 size, MetricUnit unit = MetricUnit::PIXELS) const;
    
    // Metrics conversion
    HorizontalMetrics convertHorizontalMetrics(const HorizontalMetrics& metrics, MetricUnit fromUnit, MetricUnit toUnit) const;
    VerticalMetrics convertVerticalMetrics(const VerticalMetrics& metrics, MetricUnit fromUnit, MetricUnit toUnit) const;
    ScaledMetrics convertScaledMetrics(const ScaledMetrics& metrics, MetricUnit fromUnit, MetricUnit toUnit) const;
    
    // Metrics validation
    bool validateHorizontalMetrics(const HorizontalMetrics& metrics) const;
    bool validateVerticalMetrics(const VerticalMetrics& metrics) const;
    bool validateCombinedMetrics(const CombinedMetrics& metrics) const;
    bool validateScaledMetrics(const ScaledMetrics& metrics) const;
    
    // Metrics comparison
    bool compareHorizontalMetrics(const HorizontalMetrics& a, const HorizontalMetrics& b, float32 tolerance = 0.001f) const;
    bool compareVerticalMetrics(const VerticalMetrics& a, const VerticalMetrics& b, float32 tolerance = 0.001f) const;
    bool compareCombinedMetrics(const CombinedMetrics& a, const CombinedMetrics& b, float32 tolerance = 0.001f) const;
    bool compareScaledMetrics(const ScaledMetrics& a, const ScaledMetrics& b, float32 tolerance = 0.001f) const;
    
    // Statistics
    MetricsStatistics getStatistics() const;
    void updateStatistics();
    void resetStatistics();
    
    // Batch operations
    void setHorizontalMetricsBatch(const std::map<uint32_t, HorizontalMetrics>& metrics);
    void setVerticalMetricsBatch(const std::map<uint32_t, VerticalMetrics>& metrics);
    void setCombinedMetricsBatch(const std::map<uint32_t, CombinedMetrics>& metrics);
    void getScaledMetricsBatch(const std::vector<uint32_t>& glyphIndices, float32 size, std::vector<ScaledMetrics>& metrics) const;
    
    // Event handling
    void addMetricsEventListener(const std::string& eventType, std::function<void()> callback);
    void removeMetricsEventListener(const std::string& eventType, std::function<void()> callback);
    void clearMetricsEventListeners();
    
    // Utility methods
    void cloneFrom(const GlyphMetricsManager& other);
    virtual std::unique_ptr<GlyphMetricsManager> clone() const;
    bool equals(const GlyphMetricsManager& other) const;
    size_t hash() const;
    
protected:
    // Protected members
    std::map<uint32_t, HorizontalMetrics> horizontalMetrics_;
    std::map<uint32_t, VerticalMetrics> verticalMetrics_;
    std::map<uint32_t, CombinedMetrics> combinedMetrics_;
    std::map<std::pair<uint32_t, float32>, ScaledMetrics> scaledMetrics_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    MetricsStatistics statistics_;
    size_t maxCacheSize_;
    
    // Protected helper methods
    virtual void triggerMetricsEvent(const std::string& eventType);
    virtual void updateCache();
    virtual void cleanupCache();
    virtual bool getFromCache(uint32_t glyphIndex, float32 size, ScaledMetrics& metrics) const;
    virtual void addToCache(uint32_t glyphIndex, float32 size, const ScaledMetrics& metrics);
    virtual void removeFromCache(uint32_t glyphIndex, float32 size);
    
    // Conversion helpers
    virtual float32 convertUnit(float32 value, MetricUnit fromUnit, MetricUnit toUnit, float32 size = 12.0f) const;
    virtual float32 pixelsToPoints(float32 pixels, float32 dpi = 72.0f) const;
    virtual float32 pointsToPixels(float32 points, float32 dpi = 72.0f) const;
    virtual float32 pixelsToEm(float32 pixels, float32 fontSize) const;
    virtual float32 emToPixels(float32 em, float32 fontSize) const;
    virtual float32 unitsPerEmToPixels(float32 units, float32 unitsPerEm, float32 fontSize) const;
    virtual float32 pixelsToUnitsPerEm(float32 pixels, float32 unitsPerEm, float32 fontSize) const;
    
    // Validation helpers
    virtual bool isValidAdvance(float32 advance) const;
    virtual bool isValidBearing(float32 bearing) const;
    virtual bool isValidSize(float32 size) const;
    virtual bool isValidScale(float32 scale) const;
};

// Specialized metrics managers
class HorizontalMetricsManager : public GlyphMetricsManager {
public:
    HorizontalMetricsManager();
    bool getHorizontalMetrics(uint32_t glyphIndex, HorizontalMetrics& metrics) const override;
    void setHorizontalMetrics(uint32_t glyphIndex, const HorizontalMetrics& metrics) override;
    HorizontalMetrics calculateHorizontalMetrics(uint32_t glyphIndex, float32 size = 1.0f) const override;
    std::unique_ptr<GlyphMetricsManager> clone() const override;
    
protected:
    void triggerMetricsEvent(const std::string& eventType) override;
    void updateCache() override;
};

class VerticalMetricsManager : public GlyphMetricsManager {
public:
    VerticalMetricsManager();
    bool getVerticalMetrics(uint32_t glyphIndex, VerticalMetrics& metrics) const override;
    void setVerticalMetrics(uint32_t glyphIndex, const VerticalMetrics& metrics) override;
    VerticalMetrics calculateVerticalMetrics(uint32_t glyphIndex, float32 size = 1.0f) const override;
    std::unique_ptr<GlyphMetricsManager> clone() const override;
    
protected:
    void triggerMetricsEvent(const std::string& eventType) override;
    void updateCache() override;
};

class ScaledMetricsManager : public GlyphMetricsManager {
public:
    ScaledMetricsManager();
    bool getScaledMetrics(uint32_t glyphIndex, float32 size, ScaledMetrics& metrics) const override;
    void setScaledMetrics(uint32_t glyphIndex, float32 size, const ScaledMetrics& metrics) override;
    ScaledMetrics calculateScaledMetrics(uint32_t glyphIndex, float32 size, MetricUnit unit = MetricUnit::PIXELS) const override;
    std::unique_ptr<GlyphMetricsManager> clone() const override;
    
protected:
    void triggerMetricsEvent(const std::string& eventType) override;
    void updateCache() override;
    void cleanupCache() override;
};

// Metrics factory
class GlyphMetricsFactory {
public:
    static std::unique_ptr<GlyphMetricsManager> createMetricsManager(MetricType type);
    static std::unique_ptr<HorizontalMetricsManager> createHorizontalMetricsManager();
    static std::unique_ptr<VerticalMetricsManager> createVerticalMetricsManager();
    static std::unique_ptr<ScaledMetricsManager> createScaledMetricsManager();
    static HorizontalMetrics createDefaultHorizontalMetrics(uint32_t glyphIndex);
    static VerticalMetrics createDefaultVerticalMetrics(uint32_t glyphIndex);
    static CombinedMetrics createDefaultCombinedMetrics(uint32_t glyphIndex);
    static ScaledMetrics createDefaultScaledMetrics(uint32_t glyphIndex, float32 size);
    static std::vector<MetricType> getSupportedMetricTypes();
    static std::vector<MetricUnit> getSupportedMetricUnits();
    static std::vector<MetricPrecision> getSupportedMetricPrecisions();
};

} // namespace RFCore