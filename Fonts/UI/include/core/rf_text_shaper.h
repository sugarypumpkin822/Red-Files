#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFCore {

// Text shaping types
enum class TextShaperType {
    HARFBUZZ,
    UNISCRIBE,
    CORETEXT,
    DIRECTWRITE,
    PANGO,
    CUSTOM
};

// Shaping directions
enum class ShapingDirection {
    LEFT_TO_RIGHT,
    RIGHT_TO_LEFT,
    TOP_TO_BOTTOM,
    BOTTOM_TO_TOP
};

// Shaping scripts
enum class ShapingScript {
    LATIN,
    ARABIC,
    HEBREW,
    CYRILLIC,
    CHINESE,
    JAPANESE,
    KOREAN,
    THAI,
    DEVANAGARI,
    CUSTOM
};

// Shaping features
enum class ShapingFeature {
    LIGATURES,
    KERNING,
    CONTEXTUAL,
    POSITIONING,
    SUBSTITUTION,
    MARK_POSITIONING,
    CURSIVE,
    CUSTOM
};

// Glyph position
struct GlyphPosition {
    uint32_t glyphIndex;
    float32 x;
    float32 y;
    float32 xAdvance;
    float32 yAdvance;
    float32 xOffset;
    float32 yOffset;
    
    GlyphPosition() : glyphIndex(0), x(0.0f), y(0.0f), xAdvance(0.0f), yAdvance(0.0f), 
                     xOffset(0.0f), yOffset(0.0f) {}
    GlyphPosition(uint32_t idx, float32 x_, float32 y_, float32 xAdv, float32 yAdv, 
                 float32 xOff, float32 yOff)
        : glyphIndex(idx), x(x_), y(y_), xAdvance(xAdv), yAdvance(yAdv), 
          xOffset(xOff), yOffset(yOff) {}
};

// Shaped glyph
struct ShapedGlyph {
    uint32_t glyphIndex;
    uint32_t charCode;
    GlyphPosition position;
    std::vector<uint32_t> cluster;
    ShapingScript script;
    bool isLigated;
    bool isMark;
    bool isClusterStart;
    bool isClusterEnd;
    
    ShapedGlyph() : glyphIndex(0), charCode(0), script(ShapingScript::LATIN), 
                   isLigated(false), isMark(false), isClusterStart(false), isClusterEnd(false) {}
    ShapedGlyph(uint32_t idx, uint32_t code, const GlyphPosition& pos, const std::vector<uint32_t>& clust,
               ShapingScript scr, bool lig, bool mark, bool clustStart, bool clustEnd)
        : glyphIndex(idx), charCode(code), position(pos), cluster(clust), script(scr), 
          isLigated(lig), isMark(mark), isClusterStart(clustStart), isClusterEnd(clustEnd) {}
};

// Shaped run
struct ShapedRun {
    std::vector<ShapedGlyph> glyphs;
    ShapingDirection direction;
    ShapingScript script;
    std::string language;
    float32 width;
    float32 height;
    float32 ascent;
    float32 descent;
    uint32_t startIndex;
    uint32_t endIndex;
    
    ShapedRun() : direction(ShapingDirection::LEFT_TO_RIGHT), script(ShapingScript::LATIN), 
                  width(0.0f), height(0.0f), ascent(0.0f), descent(0.0f), startIndex(0), endIndex(0) {}
    ShapedRun(const std::vector<ShapedGlyph>& gl, ShapingDirection dir, ShapingScript scr, 
              const std::string& lang, float32 w, float32 h, float32 asc, float32 desc, 
              uint32_t start, uint32_t end)
        : glyphs(gl), direction(dir), script(scr), language(lang), width(w), height(h), 
          ascent(asc), descent(desc), startIndex(start), endIndex(end) {}
};

// Shaped line
struct ShapedLine {
    std::vector<ShapedRun> runs;
    float32 width;
    float32 height;
    float32 ascent;
    float32 descent;
    float32 leading;
    uint32_t startIndex;
    uint32_t endIndex;
    
    ShapedLine() : width(0.0f), height(0.0f), ascent(0.0f), descent(0.0f), leading(0.0f), 
                   startIndex(0), endIndex(0) {}
    ShapedLine(const std::vector<ShapedRun>& r, float32 w, float32 h, float32 asc, 
               float32 desc, float32 lead, uint32_t start, uint32_t end)
        : runs(r), width(w), height(h), ascent(asc), descent(desc), leading(lead), 
          startIndex(start), endIndex(end) {}
};

// Shaped text
struct ShapedText {
    std::vector<ShapedLine> lines;
    float32 totalWidth;
    float32 totalHeight;
    float32 maxLineWidth;
    float32 maxLineHeight;
    uint32_t totalGlyphs;
    uint32_t totalLines;
    
    ShapedText() : totalWidth(0.0f), totalHeight(0.0f), maxLineWidth(0.0f), maxLineHeight(0.0f), 
                   totalGlyphs(0), totalLines(0) {}
    ShapedText(const std::vector<ShapedLine>& l, float32 totalW, float32 totalH, 
               float32 maxW, float32 maxH, uint32_t totalG, uint32_t totalL)
        : lines(l), totalWidth(totalW), totalHeight(totalH), maxLineWidth(maxW), 
          maxLineHeight(maxH), totalGlyphs(totalG), totalLines(totalL) {}
};

// Text shaper configuration
struct TextShaperConfig {
    TextShaperType type;
    ShapingDirection defaultDirection;
    ShapingScript defaultScript;
    std::string defaultLanguage;
    float32 fontSize;
    float32 dpiScale;
    bool enableLigatures;
    bool enableKerning;
    bool enableContextual;
    bool enablePositioning;
    bool enableSubstitution;
    bool enableMarkPositioning;
    bool enableCursive;
    bool enableCache;
    bool enableStatistics;
    bool enableDebugging;
    std::vector<ShapingFeature> enabledFeatures;
    std::map<std::string, std::string> fontFeatures;
    
    TextShaperConfig() : type(TextShaperType::HARFBUZZ), defaultDirection(ShapingDirection::LEFT_TO_RIGHT), 
                        defaultScript(ShapingScript::LATIN), defaultLanguage("en"), fontSize(12.0f), 
                        dpiScale(1.0f), enableLigatures(true), enableKerning(true), enableContextual(true), 
                        enablePositioning(true), enableSubstitution(true), enableMarkPositioning(true), 
                        enableCursive(false), enableCache(true), enableStatistics(true), enableDebugging(false) {}
};

// Text shaper statistics
struct TextShaperStatistics {
    uint32_t totalShapes;
    uint32_t successfulShapes;
    uint32_t failedShapes;
    uint32_t cacheHits;
    uint32_t cacheMisses;
    float32 cacheHitRatio;
    float32 averageShapeTime;
    uint64_t totalShapeTime;
    uint32_t totalGlyphs;
    uint32_t totalRuns;
    uint32_t totalLines;
    uint32_t ligatureCount;
    uint32_t kerningCount;
    uint32_t substitutionCount;
    
    TextShaperStatistics() : totalShapes(0), successfulShapes(0), failedShapes(0), 
                             cacheHits(0), cacheMisses(0), cacheHitRatio(0.0f), averageShapeTime(0.0f), 
                             totalShapeTime(0), totalGlyphs(0), totalRuns(0), totalLines(0), 
                             ligatureCount(0), kerningCount(0), substitutionCount(0) {}
};

// Text shaper
class TextShaper {
public:
    TextShaper();
    virtual ~TextShaper() = default;
    
    // Text shaper management
    void initialize(const TextShaperConfig& config);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Configuration
    void setConfiguration(const TextShaperConfig& config);
    const TextShaperConfig& getConfiguration() const;
    void setDefaultDirection(ShapingDirection direction);
    void setDefaultScript(ShapingScript script);
    void setDefaultLanguage(const std::string& language);
    void setFontSize(float32 size);
    void setDpiScale(float32 scale);
    
    // Text shaping
    ShapedText shapeText(const std::string& text) const;
    ShapedText shapeText(const std::wstring& text) const;
    ShapedText shapeText(const std::u32string& text) const;
    ShapedRun shapeRun(const std::string& text, uint32_t startIndex, uint32_t endIndex) const;
    ShapedRun shapeRun(const std::wstring& text, uint32_t startIndex, uint32_t endIndex) const;
    ShapedRun shapeRun(const std::u32string& text, uint32_t startIndex, uint32_t endIndex) const;
    
    // Glyph shaping
    std::vector<ShapedGlyph> shapeGlyphs(const std::string& text) const;
    std::vector<ShapedGlyph> shapeGlyphs(const std::wstring& text) const;
    std::vector<ShapedGlyph> shapeGlyphs(const std::u32string& text) const;
    ShapedGlyph shapeGlyph(uint32_t charCode) const;
    
    // Run detection
    std::vector<ShapedRun> detectRuns(const std::string& text) const;
    std::vector<ShapedRun> detectRuns(const std::wstring& text) const;
    std::vector<ShapedRun> detectRuns(const std::u32string& text) const;
    ShapingDirection detectDirection(const std::string& text) const;
    ShapingDirection detectDirection(const std::wstring& text) const;
    ShapingDirection detectDirection(const std::u32string& text) const;
    ShapingScript detectScript(const std::string& text) const;
    ShapingScript detectScript(const std::wstring& text) const;
    ShapingScript detectScript(const std::u32string& text) const;
    
    // Feature management
    void enableFeature(ShapingFeature feature);
    void disableFeature(ShapingFeature feature);
    bool isFeatureEnabled(ShapingFeature feature) const;
    std::vector<ShapingFeature> getEnabledFeatures() const;
    void setFontFeature(const std::string& tag, const std::string& value);
    std::string getFontFeature(const std::string& tag) const;
    std::map<std::string, std::string> getFontFeatures() const;
    
    // Ligature support
    void enableLigatures(bool enabled);
    bool isLigaturesEnabled() const;
    std::vector<ShapedGlyph> applyLigatures(const std::vector<ShapedGlyph>& glyphs) const;
    bool isLigature(uint32_t glyphIndex) const;
    
    // Kerning support
    void enableKerning(bool enabled);
    bool isKerningEnabled() const;
    std::vector<ShapedGlyph> applyKerning(const std::vector<ShapedGlyph>& glyphs) const;
    float32 getKerning(uint32_t leftGlyph, uint32_t rightGlyph) const;
    
    // Positioning support
    void enablePositioning(bool enabled);
    bool isPositioningEnabled() const;
    std::vector<ShapedGlyph> applyPositioning(const std::vector<ShapedGlyph>& glyphs) const;
    GlyphPosition getPosition(uint32_t glyphIndex) const;
    
    // Substitution support
    void enableSubstitution(bool enabled);
    bool isSubstitutionEnabled() const;
    std::vector<ShapedGlyph> applySubstitution(const std::vector<ShapedGlyph>& glyphs) const;
    bool isSubstituted(uint32_t glyphIndex) const;
    
    // Mark positioning support
    void enableMarkPositioning(bool enabled);
    bool isMarkPositioningEnabled() const;
    std::vector<ShapedGlyph> applyMarkPositioning(const std::vector<ShapedGlyph>& glyphs) const;
    bool isMark(uint32_t glyphIndex) const;
    
    // Cursive support
    void enableCursive(bool enabled);
    bool isCursiveEnabled() const;
    std::vector<ShapedGlyph> applyCursive(const std::vector<ShapedGlyph>& glyphs) const;
    bool isCursive(uint32_t glyphIndex) const;
    
    // Text measurement
    float32 getTextWidth(const std::string& text) const;
    float32 getTextWidth(const std::wstring& text) const;
    float32 getTextWidth(const std::u32string& text) const;
    float32 getTextHeight(const std::string& text) const;
    float32 getTextHeight(const std::wstring& text) const;
    float32 getTextHeight(const std::u32string& text) const;
    
    // Validation
    bool validateShapedText(const ShapedText& shapedText) const;
    bool validateShapedLine(const ShapedLine& shapedLine) const;
    bool validateShapedRun(const ShapedRun& shapedRun) const;
    bool validateShapedGlyph(const ShapedGlyph& shapedGlyph) const;
    
    // Caching
    void enableCache(bool enabled);
    bool isCacheEnabled() const;
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    void clearCache();
    
    // Statistics
    TextShaperStatistics getStatistics() const;
    void updateStatistics();
    void resetStatistics();
    void enableStatistics(bool enabled);
    bool isStatisticsEnabled() const;
    
    // Debugging
    void enableDebugging(bool enabled);
    bool isDebuggingEnabled() const;
    std::vector<std::string> getDebugMessages() const;
    void clearDebugMessages();
    void dumpShaper() const;
    std::string dumpShaperToString() const;
    
    // Event handling
    void addTextShaperEventListener(const std::string& eventType, std::function<void()> callback);
    void removeTextShaperEventListener(const std::string& eventType, std::function<void()> callback);
    void clearTextShaperEventListeners();
    
    // Utility methods
    void cloneFrom(const TextShaper& other);
    virtual std::unique_ptr<TextShaper> clone() const;
    bool equals(const TextShaper& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    TextShaperConfig config_;
    TextShaperStatistics statistics_;
    
    bool isInitialized_;
    bool statisticsEnabled_;
    bool debuggingEnabled_;
    bool cacheEnabled_;
    bool ligaturesEnabled_;
    bool kerningEnabled_;
    bool positioningEnabled_;
    bool substitutionEnabled_;
    bool markPositioningEnabled_;
    bool cursiveEnabled_;
    
    size_t currentCacheSize_;
    std::vector<std::string> debugMessages_;
    
    // Protected helper methods
    virtual void triggerTextShaperEvent(const std::string& eventType);
    virtual ShapedText shapeTextInternal(const std::string& text) const;
    virtual ShapedText shapeTextInternal(const std::wstring& text) const;
    virtual ShapedText shapeTextInternal(const std::u32string& text) const;
    virtual ShapedRun shapeRunInternal(const std::string& text, uint32_t startIndex, uint32_t endIndex) const;
    virtual ShapedRun shapeRunInternal(const std::wstring& text, uint32_t startIndex, uint32_t endIndex) const;
    virtual ShapedRun shapeRunInternal(const std::u32string& text, uint32_t startIndex, uint32_t endIndex) const;
    
    // Run detection helpers
    virtual std::vector<ShapedRun> detectRunsInternal(const std::string& text) const;
    virtual std::vector<ShapedRun> detectRunsInternal(const std::wstring& text) const;
    virtual std::vector<ShapedRun> detectRunsInternal(const std::u32string& text) const;
    virtual ShapingDirection detectDirectionInternal(const std::string& text) const;
    virtual ShapingDirection detectDirectionInternal(const std::wstring& text) const;
    virtual ShapingDirection detectDirectionInternal(const std::u32string& text) const;
    virtual ShapingScript detectScriptInternal(const std::string& text) const;
    virtual ShapingScript detectScriptInternal(const std::wstring& text) const;
    virtual ShapingScript detectScriptInternal(const std::u32string& text) const;
    
    // Feature application helpers
    virtual std::vector<ShapedGlyph> applyLigaturesInternal(const std::vector<ShapedGlyph>& glyphs) const;
    virtual std::vector<ShapedGlyph> applyKerningInternal(const std::vector<ShapedGlyph>& glyphs) const;
    virtual std::vector<ShapedGlyph> applyPositioningInternal(const std::vector<ShapedGlyph>& glyphs) const;
    virtual std::vector<ShapedGlyph> applySubstitutionInternal(const std::vector<ShapedGlyph>& glyphs) const;
    virtual std::vector<ShapedGlyph> applyMarkPositioningInternal(const std::vector<ShapedGlyph>& glyphs) const;
    virtual std::vector<ShapedGlyph> applyCursiveInternal(const std::vector<ShapedGlyph>& glyphs) const;
    
    // Text measurement helpers
    virtual float32 calculateTextWidth(const std::string& text) const;
    virtual float32 calculateTextWidth(const std::wstring& text) const;
    virtual float32 calculateTextWidth(const std::u32string& text) const;
    virtual float32 calculateTextHeight(const std::string& text) const;
    virtual float32 calculateTextHeight(const std::wstring& text) const;
    virtual float32 calculateTextHeight(const std::u32string& text) const;
    
    // Validation helpers
    virtual bool validateShapedTextInternal(const ShapedText& shapedText) const;
    virtual bool validateShapedLineInternal(const ShapedLine& shapedLine) const;
    virtual bool validateShapedRunInternal(const ShapedRun& shapedRun) const;
    virtual bool validateShapedGlyphInternal(const ShapedGlyph& shapedGlyph) const;
    
    // Cache helpers
    virtual std::string generateCacheKey(const std::string& text, const TextShaperConfig& config) const;
    virtual bool getFromCache(const std::string& key, ShapedText& shapedText) const;
    virtual void addToCache(const std::string& key, const ShapedText& shapedText);
    virtual void removeFromCache(const std::string& key);
    virtual void updateCache();
    virtual void cleanupCache();
    
    // Statistics helpers
    virtual void updateShapeStatistics(bool success, uint64_t shapeTime, uint32_t glyphCount);
    virtual void updateCacheStatistics(bool hit);
    virtual void calculateStatistics();
    
    // Debug helpers
    virtual void addDebugMessage(const std::string& message);
    virtual void logShapingOperation(const std::string& operation, const std::string& text);
    virtual std::string formatShapedText(const ShapedText& shapedText) const;
    virtual std::string formatShapedGlyph(const ShapedGlyph& shapedGlyph) const;
};

// Specialized text shapers
class HarfbuzzTextShaper : public TextShaper {
public:
    HarfbuzzTextShaper();
    ShapedText shapeText(const std::string& text) const override;
    ShapedRun shapeRun(const std::string& text, uint32_t startIndex, uint32_t endIndex) const override;
    std::vector<ShapedGlyph> shapeGlyphs(const std::string& text) const override;
    std::unique_ptr<TextShaper> clone() const override;
    
protected:
    ShapedText shapeTextInternal(const std::string& text) const override;
    ShapedRun shapeRunInternal(const std::string& text, uint32_t startIndex, uint32_t endIndex) const override;
    std::vector<ShapedGlyph> shapeGlyphsInternal(const std::string& text) const override;
};

class CoreTextShaper : public TextShaper {
public:
    CoreTextShaper();
    ShapedText shapeText(const std::string& text) const override;
    ShapedRun shapeRun(const std::string& text, uint32_t startIndex, uint32_t endIndex) const override;
    std::vector<ShapedGlyph> shapeGlyphs(const std::string& text) const override;
    std::unique_ptr<TextShaper> clone() const override;
    
protected:
    ShapedText shapeTextInternal(const std::string& text) const override;
    ShapedRun shapeRunInternal(const std::string& text, uint32_t startIndex, uint32_t endIndex) const override;
    std::vector<ShapedGlyph> shapeGlyphsInternal(const std::string& text) const override;
};

class DirectWriteShaper : public TextShaper {
public:
    DirectWriteShaper();
    ShapedText shapeText(const std::string& text) const override;
    ShapedRun shapeRun(const std::string& text, uint32_t startIndex, uint32_t endIndex) const override;
    std::vector<ShapedGlyph> shapeGlyphs(const std::string& text) const override;
    std::unique_ptr<TextShaper> clone() const override;
    
protected:
    ShapedText shapeTextInternal(const std::string& text) const override;
    ShapedRun shapeRunInternal(const std::string& text, uint32_t startIndex, uint32_t endIndex) const override;
    std::vector<ShapedGlyph> shapeGlyphsInternal(const std::string& text) const override;
};

// Text shaper factory
class TextShaperFactory {
public:
    static std::unique_ptr<TextShaper> createTextShaper(TextShaperType type);
    static std::unique_ptr<HarfbuzzTextShaper> createHarfbuzzTextShaper();
    static std::unique_ptr<CoreTextShaper> createCoreTextShaper();
    static std::unique_ptr<DirectWriteShaper> createDirectWriteShaper();
    static TextShaperConfig createDefaultConfig(TextShaperType type);
    static std::vector<TextShaperType> getSupportedShaperTypes();
    static std::vector<ShapingDirection> getSupportedDirections();
    static std::vector<ShapingScript> getSupportedScripts();
    static std::vector<ShapingFeature> getSupportedFeatures();
    static TextShaperType detectBestShaper(const std::string& text);
    static TextShaperType detectBestShaper(const std::wstring& text);
    static TextShaperType detectBestShaper(const std::u32string& text);
};

} // namespace RFCore