#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFCore {

// Font types
enum class FontType {
    TRUETYPE,
    OPENTYPE,
    WOFF,
    WOFF2,
    TYPE1,
    CFF,
    BITMAP,
    CUSTOM
};

// Font styles
enum class FontStyle {
    NORMAL,
    ITALIC,
    OBLIQUE
};

// Font weights
enum class FontWeight {
    THIN = 100,
    EXTRA_LIGHT = 200,
    LIGHT = 300,
    NORMAL = 400,
    MEDIUM = 500,
    SEMI_BOLD = 600,
    BOLD = 700,
    EXTRA_BOLD = 800,
    BLACK = 900
};

// Font stretch
enum class FontStretch {
    ULTRA_CONDENSED,
    EXTRA_CONDENSED,
    CONDENSED,
    SEMI_CONDENSED,
    NORMAL,
    SEMI_EXPANDED,
    EXPANDED,
    EXTRA_EXPANDED,
    ULTRA_EXPANDED
};

// Font rendering modes
enum class FontRenderMode {
    RASTER,
    VECTOR,
    SDF,
    MSDF,
    CUSTOM
};

// Font hinting
enum class FontHinting {
    NONE,
    SLIGHT,
    MEDIUM,
    FULL,
    AUTO
};

// Font subpixel positioning
enum class FontSubpixel {
    NONE,
    HORIZONTAL,
    VERTICAL,
    BOTH
};

// Font information
struct FontInfo {
    std::string familyName;
    std::string styleName;
    std::string fullName;
    std::string postscriptName;
    FontType type;
    FontStyle style;
    FontWeight weight;
    FontStretch stretch;
    uint32_t unitsPerEm;
    float32 ascender;
    float32 descender;
    float32 lineGap;
    float32 capHeight;
    float32 xHeight;
    float32 underlinePosition;
    float32 underlineThickness;
    bool hasKerning;
    bool hasLigatures;
    uint32_t glyphCount;
    uint32_t faceIndex;
    std::string copyright;
    std::string version;
    std::string manufacturer;
    
    FontInfo() : type(FontType::TRUETYPE), style(FontStyle::NORMAL), weight(FontWeight::NORMAL), 
                stretch(FontStretch::NORMAL), unitsPerEm(1000), ascender(0.0f), descender(0.0f), 
                lineGap(0.0f), capHeight(0.0f), xHeight(0.0f), underlinePosition(0.0f), 
                underlineThickness(0.0f), hasKerning(false), hasLigatures(false), glyphCount(0), 
                faceIndex(0) {}
};

// Font metrics
struct FontMetrics {
    float32 fontSize;
    float32 scale;
    float32 ascent;
    float32 descent;
    float32 leading;
    float32 lineHeight;
    float32 maxAdvanceWidth;
    float32 maxAdvanceHeight;
    float32 underlinePosition;
    float32 underlineThickness;
    float32 strikethroughPosition;
    float32 strikethroughThickness;
    
    FontMetrics() : fontSize(12.0f), scale(1.0f), ascent(0.0f), descent(0.0f), leading(0.0f), 
                    lineHeight(0.0f), maxAdvanceWidth(0.0f), maxAdvanceHeight(0.0f), 
                    underlinePosition(0.0f), underlineThickness(0.0f), strikethroughPosition(0.0f), 
                    strikethroughThickness(0.0f) {}
};

// Font rendering settings
struct FontRenderSettings {
    FontRenderMode renderMode;
    FontHinting hinting;
    FontSubpixel subpixel;
    float32 pixelSize;
    float32 dpiScale;
    bool antiAliasing;
    bool subpixelAA;
    bool gammaCorrection;
    float32 gamma;
    float32 contrast;
    float32 brightness;
    bool useColorGlyphs;
    bool useVariations;
    std::array<float32, 4> variationCoordinates;
    
    FontRenderSettings() : renderMode(FontRenderMode::RASTER), hinting(FontHinting::MEDIUM), 
                          subpixel(FontSubpixel::HORIZONTAL), pixelSize(12.0f), dpiScale(1.0f), 
                          antiAliasing(true), subpixelAA(true), gammaCorrection(true), 
                          gamma(2.2f), contrast(1.0f), brightness(1.0f), useColorGlyphs(true), 
                          useVariations(false), variationCoordinates{0.0f, 0.0f, 0.0f, 0.0f} {}
};

// Font class
class Font {
public:
    Font();
    virtual ~Font() = default;
    
    // Font loading and management
    virtual bool loadFromFile(const std::string& filename, uint32_t faceIndex = 0) = 0;
    virtual bool loadFromMemory(const uint8* data, size_t size, uint32_t faceIndex = 0) = 0;
    virtual bool loadFromStream(std::istream& stream, uint32_t faceIndex = 0) = 0;
    virtual void unload() = 0;
    virtual bool isLoaded() const = 0;
    
    // Font information
    virtual const FontInfo& getFontInfo() const = 0;
    virtual FontMetrics getFontMetrics(float32 fontSize) const = 0;
    virtual FontType getFontType() const = 0;
    virtual uint32_t getGlyphCount() const = 0;
    virtual bool hasKerning() const = 0;
    virtual bool hasLigatures() const = 0;
    
    // Glyph access
    virtual uint32_t getGlyphIndex(uint32_t charCode) const = 0;
    virtual bool getGlyphMetrics(uint32_t glyphIndex, GlyphMetrics& metrics) const = 0;
    virtual bool getGlyphBitmap(uint32_t glyphIndex, float32 size, GlyphBitmap& bitmap) const = 0;
    virtual bool getGlyphOutline(uint32_t glyphIndex, float32 size, GlyphOutline& outline) const = 0;
    virtual bool getGlyphPath(uint32_t glyphIndex, GlyphPath& path) const = 0;
    
    // Kerning
    virtual float32 getKerning(uint32_t leftGlyph, uint32_t rightGlyph) const = 0;
    virtual float32 getKerning(uint32_t leftChar, uint32_t rightChar) const = 0;
    
    // Text measurement
    virtual float32 getStringWidth(const std::string& text, float32 fontSize) const = 0;
    virtual float32 getStringHeight(const std::string& text, float32 fontSize) const = 0;
    virtual TextMetrics getTextMetrics(const std::string& text, float32 fontSize) const = 0;
    virtual std::vector<TextRun> getTextRuns(const std::string& text, float32 fontSize) const = 0;
    
    // Font variations
    virtual bool hasVariations() const = 0;
    virtual std::vector<FontVariation> getVariations() const = 0;
    virtual bool setVariation(const std::string& tag, float32 value) = 0;
    virtual bool setVariations(const std::map<std::string, float32>& variations) = 0;
    virtual float32 getVariation(const std::string& tag) const = 0;
    
    // Font features
    virtual bool hasFeatures() const = 0;
    virtual std::vector<FontFeature> getFeatures() const = 0;
    virtual bool enableFeature(const std::string& tag) = 0;
    virtual bool disableFeature(const std::string& tag) = 0;
    virtual bool isFeatureEnabled(const std::string& tag) const = 0;
    
    // Rendering settings
    virtual void setRenderSettings(const FontRenderSettings& settings) = 0;
    virtual const FontRenderSettings& getRenderSettings() const = 0;
    virtual void setRenderMode(FontRenderMode mode) = 0;
    virtual void setHinting(FontHinting hinting) = 0;
    virtual void setSubpixel(FontSubpixel subpixel) = 0;
    virtual void setAntiAliasing(bool enabled) = 0;
    virtual void setSubpixelAA(bool enabled) = 0;
    virtual void setGamma(float32 gamma) = 0;
    virtual void setContrast(float32 contrast) = 0;
    virtual void setBrightness(float32 brightness) = 0;
    
    // Font caching
    virtual void setCacheSize(size_t maxSize) = 0;
    virtual size_t getCacheSize() const = 0;
    virtual void clearCache() = 0;
    virtual CacheStats getCacheStats() const = 0;
    
    // Font optimization
    virtual void optimizeForSize(float32 fontSize) = 0;
    virtual void optimizeForText(const std::string& text) = 0;
    virtual void optimizeForGlyphs(const std::vector<uint32_t>& glyphIndices) = 0;
    
    // Event handling
    virtual void addFontEventListener(const std::string& eventType, std::function<void()> callback) = 0;
    virtual void removeFontEventListener(const std::string& eventType, std::function<void()> callback) = 0;
    virtual void clearFontEventListeners() = 0;
    
    // Utility methods
    virtual void cloneFrom(const Font& other) = 0;
    virtual std::unique_ptr<Font> clone() const = 0;
    virtual bool equals(const Font& other) const = 0;
    virtual size_t hash() const = 0;
    
protected:
    // Protected members
    FontInfo fontInfo_;
    FontRenderSettings renderSettings_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    // Protected helper methods
    virtual void triggerFontEvent(const std::string& eventType) = 0;
    virtual void updateFontMetrics() = 0;
    virtual void updateRenderSettings() = 0;
};

// Specialized font classes
class TrueTypeFont : public Font {
public:
    TrueTypeFont();
    bool loadFromFile(const std::string& filename, uint32_t faceIndex = 0) override;
    bool loadFromMemory(const uint8* data, size_t size, uint32_t faceIndex = 0) override;
    bool loadFromStream(std::istream& stream, uint32_t faceIndex = 0) override;
    void unload() override;
    bool isLoaded() const override;
    const FontInfo& getFontInfo() const override;
    FontMetrics getFontMetrics(float32 fontSize) const override;
    FontType getFontType() const override;
    uint32_t getGlyphCount() const override;
    bool hasKerning() const override;
    bool hasLigatures() const override;
    uint32_t getGlyphIndex(uint32_t charCode) const override;
    bool getGlyphMetrics(uint32_t glyphIndex, GlyphMetrics& metrics) const override;
    bool getGlyphBitmap(uint32_t glyphIndex, float32 size, GlyphBitmap& bitmap) const override;
    bool getGlyphOutline(uint32_t glyphIndex, float32 size, GlyphOutline& outline) const override;
    bool getGlyphPath(uint32_t glyphIndex, GlyphPath& path) const override;
    float32 getKerning(uint32_t leftGlyph, uint32_t rightGlyph) const override;
    float32 getKerning(uint32_t leftChar, uint32_t rightChar) const override;
    float32 getStringWidth(const std::string& text, float32 fontSize) const override;
    float32 getStringHeight(const std::string& text, float32 fontSize) const override;
    TextMetrics getTextMetrics(const std::string& text, float32 fontSize) const override;
    std::vector<TextRun> getTextRuns(const std::string& text, float32 fontSize) const override;
    bool hasVariations() const override;
    std::vector<FontVariation> getVariations() const override;
    bool setVariation(const std::string& tag, float32 value) override;
    bool setVariations(const std::map<std::string, float32>& variations) override;
    float32 getVariation(const std::string& tag) const override;
    bool hasFeatures() const override;
    std::vector<FontFeature> getFeatures() const override;
    bool enableFeature(const std::string& tag) override;
    bool disableFeature(const std::string& tag) override;
    bool isFeatureEnabled(const std::string& tag) const override;
    void setRenderSettings(const FontRenderSettings& settings) override;
    const FontRenderSettings& getRenderSettings() const override;
    void setRenderMode(FontRenderMode mode) override;
    void setHinting(FontHinting hinting) override;
    void setSubpixel(FontSubpixel subpixel) override;
    void setAntiAliasing(bool enabled) override;
    void setSubpixelAA(bool enabled) override;
    void setGamma(float32 gamma) override;
    void setContrast(float32 contrast) override;
    void setBrightness(float32 brightness) override;
    void setCacheSize(size_t maxSize) override;
    size_t getCacheSize() const override;
    void clearCache() override;
    CacheStats getCacheStats() const override;
    void optimizeForSize(float32 fontSize) override;
    void optimizeForText(const std::string& text) override;
    void optimizeForGlyphs(const std::vector<uint32_t>& glyphIndices) override;
    void addFontEventListener(const std::string& eventType, std::function<void()> callback) override;
    void removeFontEventListener(const std::string& eventType, std::function<void()> callback) override;
    void clearFontEventListeners() override;
    void cloneFrom(const Font& other) override;
    std::unique_ptr<Font> clone() const override;
    bool equals(const Font& other) const override;
    size_t hash() const override;
    
private:
    void* face_;
    bool isLoaded_;
    std::map<uint32_t, GlyphMetrics> glyphMetricsCache_;
    std::map<std::pair<uint32_t, float32>, GlyphBitmap> glyphBitmapCache_;
    std::map<std::string, float32> variations_;
    std::set<std::string> enabledFeatures_;
    
    void triggerFontEvent(const std::string& eventType) override;
    void updateFontMetrics() override;
    void updateRenderSettings() override;
};

class OpenTypeFont : public TrueTypeFont {
public:
    OpenTypeFont();
    FontType getFontType() const override;
    bool hasVariations() const override;
    std::vector<FontVariation> getVariations() const override;
    bool hasFeatures() const override;
    std::vector<FontFeature> getFeatures() const override;
    std::unique_ptr<Font> clone() const override;
};

class BitmapFont : public Font {
public:
    BitmapFont();
    bool loadFromFile(const std::string& filename, uint32_t faceIndex = 0) override;
    bool loadFromMemory(const uint8* data, size_t size, uint32_t faceIndex = 0) override;
    bool loadFromStream(std::istream& stream, uint32_t faceIndex = 0) override;
    void unload() override;
    bool isLoaded() const override;
    const FontInfo& getFontInfo() const override;
    FontMetrics getFontMetrics(float32 fontSize) const override;
    FontType getFontType() const override;
    uint32_t getGlyphCount() const override;
    bool hasKerning() const override;
    bool hasLigatures() const override;
    uint32_t getGlyphIndex(uint32_t charCode) const override;
    bool getGlyphMetrics(uint32_t glyphIndex, GlyphMetrics& metrics) const override;
    bool getGlyphBitmap(uint32_t glyphIndex, float32 size, GlyphBitmap& bitmap) const override;
    bool getGlyphOutline(uint32_t glyphIndex, float32 size, GlyphOutline& outline) const override;
    bool getGlyphPath(uint32_t glyphIndex, GlyphPath& path) const override;
    float32 getKerning(uint32_t leftGlyph, uint32_t rightGlyph) const override;
    float32 getKerning(uint32_t leftChar, uint32_t rightChar) const override;
    float32 getStringWidth(const std::string& text, float32 fontSize) const override;
    float32 getStringHeight(const std::string& text, float32 fontSize) const override;
    TextMetrics getTextMetrics(const std::string& text, float32 fontSize) const override;
    std::vector<TextRun> getTextRuns(const std::string& text, float32 fontSize) const override;
    bool hasVariations() const override;
    std::vector<FontVariation> getVariations() const override;
    bool setVariation(const std::string& tag, float32 value) override;
    bool setVariations(const std::map<std::string, float32>& variations) override;
    float32 getVariation(const std::string& tag) const override;
    bool hasFeatures() const override;
    std::vector<FontFeature> getFeatures() const override;
    bool enableFeature(const std::string& tag) override;
    bool disableFeature(const std::string& tag) override;
    bool isFeatureEnabled(const std::string& tag) const override;
    void setRenderSettings(const FontRenderSettings& settings) override;
    const FontRenderSettings& getRenderSettings() const override;
    void setRenderMode(FontRenderMode mode) override;
    void setHinting(FontHinting hinting) override;
    void setSubpixel(FontSubpixel subpixel) override;
    void setAntiAliasing(bool enabled) override;
    void setSubpixelAA(bool enabled) override;
    void setGamma(float32 gamma) override;
    void setContrast(float32 contrast) override;
    void setBrightness(float32 brightness) override;
    void setCacheSize(size_t maxSize) override;
    size_t getCacheSize() const override;
    void clearCache() override;
    CacheStats getCacheStats() const override;
    void optimizeForSize(float32 fontSize) override;
    void optimizeForText(const std::string& text) override;
    void optimizeForGlyphs(const std::vector<uint32_t>& glyphIndices) override;
    void addFontEventListener(const std::string& eventType, std::function<void()> callback) override;
    void removeFontEventListener(const std::string& eventType, std::function<void()> callback) override;
    void clearFontEventListeners() override;
    void cloneFrom(const Font& other) override;
    std::unique_ptr<Font> clone() const override;
    bool equals(const Font& other) const override;
    size_t hash() const override;
    
private:
    std::map<uint32_t, GlyphBitmap> glyphBitmaps_;
    std::map<uint32_t, GlyphMetrics> glyphMetrics_;
    std::map<std::pair<uint32_t, uint32_t>, float32> kerningPairs_;
    bool isLoaded_;
    
    void triggerFontEvent(const std::string& eventType) override;
    void updateFontMetrics() override;
    void updateRenderSettings() override;
};

// Font factory
class FontFactory {
public:
    static std::unique_ptr<Font> createFont(FontType type);
    static std::unique_ptr<TrueTypeFont> createTrueTypeFont();
    static std::unique_ptr<OpenTypeFont> createOpenTypeFont();
    static std::unique_ptr<BitmapFont> createBitmapFont();
    static std::unique_ptr<Font> loadFont(const std::string& filename, uint32_t faceIndex = 0);
    static std::unique_ptr<Font> loadFontFromMemory(const uint8* data, size_t size, uint32_t faceIndex = 0);
    static std::unique_ptr<Font> loadFontFromStream(std::istream& stream, uint32_t faceIndex = 0);
    static FontType detectFontType(const std::string& filename);
    static FontType detectFontType(const uint8* data, size_t size);
    static std::vector<FontType> getSupportedFontTypes();
    static std::vector<std::string> getSupportedFontExtensions();
    static bool isFontSupported(const std::string& filename);
    static bool isFontSupported(const uint8* data, size_t size);
};

} // namespace RFCore