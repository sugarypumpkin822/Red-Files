#pragma once

#include "../include/utils/rf_vector2.h"
#include "../include/utils/rf_vector3.h"
#include "../include/utils/rf_vector4.h"
#include "../include/utils/rf_color.h"
#include "../include/utils/rf_rect.h"
#include "../include/utils/rf_transform.h"
#include "../include/utils/rf_string_utils.h"
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace RFFont {

// Forward declarations
class Font;
class FontFace;
class Glyph;
class TextLayout;
class TextRenderer;
class FontManager;
class FontCache;

using float32 = float;
using uint32 = uint32_t;
using uint16 = uint16_t;
using uint8 = uint8_t;

// Font types
enum class FontType {
    TrueType,
    OpenType,
    Bitmap,
    Vector,
    System,
    Custom
};

// Font styles
enum class FontStyle {
    Regular,
    Bold,
    Italic,
    BoldItalic,
    Light,
    LightItalic,
    Medium,
    MediumItalic,
    Black,
    BlackItalic,
    Custom
};

// Font weights
enum class FontWeight {
    Thin = 100,
    ExtraLight = 200,
    Light = 300,
    Regular = 400,
    Medium = 500,
    SemiBold = 600,
    Bold = 700,
    ExtraBold = 800,
    Black = 900
};

// Text alignment
enum class TextAlignment {
    Left,
    Center,
    Right,
    Justify
};

// Text baseline
enum class TextBaseline {
    Top,
    Middle,
    Bottom,
    Alphabetic,
    Hanging,
    Ideographic
};

// Text direction
enum class TextDirection {
    LeftToRight,
    RightToLeft,
    TopToBottom,
    BottomToTop
};

// Text wrapping
enum class TextWrapping {
    None,
    Word,
    Character,
    WordAndCharacter
};

// Text rendering quality
enum class TextRenderingQuality {
    Low,
    Medium,
    High,
    Ultra
};

// Font hinting
enum class FontHinting {
    None,
    Slight,
    Medium,
    Full
};

// Anti-aliasing mode
enum class AntiAliasingMode {
    None,
    Grayscale,
    Subpixel,
    LCD
};

// Subpixel order
enum class SubpixelOrder {
    RGB,
    BGR,
    VRGB,
    VBGR
};

// Glyph metrics
struct GlyphMetrics {
    Vector2 advance;        // Distance to next glyph
    Vector2 bearing;        // Offset from baseline
    Vector2 size;           // Glyph dimensions
    Vector2 offset;         // Rendering offset
    Rect bounds;            // Bounding box
    float32 kerning;        // Kerning adjustment
    uint32 codepoint;       // Unicode codepoint
    uint32 index;           // Glyph index in font
    bool isRenderable;      // Can be rendered
};

// Font information
struct FontInfo {
    std::string family;         // Font family name
    std::string style;          // Font style name
    std::string fullName;       // Full font name
    std::string version;        // Font version
    std::string copyright;      // Copyright information
    std::string manufacturer;   // Font manufacturer
    std::string designer;       // Font designer
    std::string description;    // Font description
    std::string license;        // License information
    std::string licenseURL;     // License URL
    
    FontType type;              // Font type
    FontStyle styleType;        // Style type
    FontWeight weight;          // Font weight
    bool isItalic;              // Italic flag
    bool isBold;                // Bold flag
    bool isMonospace;           // Monospace flag
    bool isVariable;            // Variable font flag
    
    uint32 unitsPerEM;          // Units per EM
    uint32 glyphCount;          // Number of glyphs
    uint32 faceCount;           // Number of faces
    uint32 size;                // Font file size
    
    float32 ascender;            // Ascender height
    float32 descender;           // Descender height
    float32 lineHeight;         // Line height
    float32 capHeight;           // Capital height
    float32 xHeight;             // X-height
    float32 maxAdvanceWidth;     // Maximum advance width
    float32 maxAdvanceHeight;    // Maximum advance height
    float32 underlinePosition;   // Underline position
    float32 underlineThickness; // Underline thickness
    float32 strikethroughPosition; // Strikethrough position
    float32 strikethroughThickness; // Strikethrough thickness
    
    Vector2 globalBoundingBoxMin; // Global bounding box min
    Vector2 globalBoundingBoxMax; // Global bounding box max
    
    std::vector<uint32> supportedCodepoints; // Supported Unicode ranges
    std::vector<std::string> supportedScripts; // Supported scripts
    std::vector<std::string> supportedLanguages; // Supported languages
};

// Font configuration
struct FontConfig {
    float32 size = 12.0f;               // Font size in pixels
    float32 dpi = 96.0f;                // DPI for rendering
    float32 scale = 1.0f;               // Scale factor
    float32 rotation = 0.0f;            // Rotation angle
    float32 skewX = 0.0f;               // Horizontal skew
    float32 skewY = 0.0f;               // Vertical skew
    
    FontStyle style = FontStyle::Regular; // Font style
    FontWeight weight = FontWeight::Regular; // Font weight
    
    FontHinting hinting = FontHinting::Medium; // Hinting mode
    AntiAliasingMode antiAliasing = AntiAliasingMode::Grayscale; // Anti-aliasing
    SubpixelOrder subpixelOrder = SubpixelOrder::RGB; // Subpixel order
    
    TextRenderingQuality quality = TextRenderingQuality::High; // Rendering quality
    bool useKerning = true;           // Enable kerning
    bool useLigatures = true;          // Enable ligatures
    bool useSubpixelPositioning = true; // Subpixel positioning
    bool useColorGlyphs = true;        // Enable color glyphs
    bool useVariations = true;         // Enable font variations
    
    float32 gamma = 1.0f;              // Gamma correction
    float32 contrast = 1.0f;           // Contrast adjustment
    float32 brightness = 1.0f;         // Brightness adjustment
    
    Color foregroundColor = Color::WHITE; // Text color
    Color backgroundColor = Color::TRANSPARENT; // Background color
    Color outlineColor = Color::BLACK; // Outline color
    float32 outlineWidth = 0.0f;       // Outline width
    
    Color shadowColor = Color::BLACK;   // Shadow color
    Vector2 shadowOffset = Vector2::ZERO; // Shadow offset
    float32 shadowBlur = 0.0f;         // Shadow blur
    
    std::map<std::string, float32> variations; // Font variations
    std::map<std::string, std::string> features; // OpenType features
};

// Text layout configuration
struct TextLayoutConfig {
    Vector2 position = Vector2::ZERO;  // Layout position
    float32 maxWidth = 0.0f;           // Maximum width (0 = unlimited)
    float32 maxHeight = 0.0f;          // Maximum height (0 = unlimited)
    float32 lineHeight = 1.2f;         // Line height multiplier
    float32 letterSpacing = 0.0f;      // Letter spacing
    float32 wordSpacing = 0.0f;        // Word spacing
    float32 paragraphSpacing = 0.0f;   // Paragraph spacing
    float32 tabWidth = 4.0f;           // Tab width in spaces
    
    TextAlignment alignment = TextAlignment::Left; // Text alignment
    TextBaseline baseline = TextBaseline::Alphabetic; // Text baseline
    TextDirection direction = TextDirection::LeftToRight; // Text direction
    TextWrapping wrapping = TextWrapping::Word; // Text wrapping
    
    bool enableJustification = true;   // Enable text justification
    bool enableHyphenation = false;    // Enable hyphenation
    bool enableEllipsis = true;        // Enable text ellipsis
    bool preserveWhitespace = true;    // Preserve whitespace
    bool collapseWhitespace = false;   // Collapse whitespace
    
    std::string ellipsisString = "..."; // Ellipsis string
    std::string hyphenString = "-";     // Hyphen string
};

// Text rendering configuration
struct TextRenderingConfig {
    Transform transform = Transform::IDENTITY; // Transform matrix
    Rect viewport = Rect::ZERO;       // Rendering viewport
    float32 depth = 0.0f;             // Rendering depth
    
    bool enableDepthTest = false;     // Enable depth testing
    bool enableBlending = true;       // Enable blending
    bool enableScissoring = false;    // Enable scissoring
    
    uint32 maxGlyphsPerBatch = 1000;  // Maximum glyphs per batch
    uint32 textureSize = 2048;        // Texture atlas size
    
    bool useDistanceField = false;     // Use signed distance field
    float32 sdfScale = 1.0f;          // SDF scale factor
    float32 sdfThreshold = 0.5f;      // SDF threshold
    
    bool useInstancing = true;        // Use instanced rendering
    bool useIndexBuffer = true;       // Use index buffer
    bool useVertexCache = true;       // Use vertex cache
};

// Glyph class
class Glyph {
public:
    Glyph();
    Glyph(uint32 codepoint, uint32 index);
    ~Glyph();
    
    // Basic properties
    uint32 getCodepoint() const { return codepoint_; }
    uint32 getIndex() const { return index_; }
    bool isValid() const { return isValid_; }
    
    // Metrics
    const GlyphMetrics& getMetrics() const { return metrics_; }
    void setMetrics(const GlyphMetrics& metrics) { metrics_ = metrics; }
    
    // Rendering data
    const Rect& getUVRect() const { return uvRect_; }
    void setUVRect(const Rect& uvRect) { uvRect_ = uvRect; }
    
    const Vector2& getTextureSize() const { return textureSize_; }
    void setTextureSize(const Vector2& size) { textureSize_ = size; }
    
    uint32 getTextureID() const { return textureID_; }
    void setTextureID(uint32 textureID) { textureID_ = textureID; }
    
    // Kerning
    float32 getKerning(uint32 nextCodepoint) const;
    void setKerning(uint32 nextCodepoint, float32 kerning);
    
    // Bitmap data (for bitmap fonts)
    const uint8* getBitmapData() const { return bitmapData_; }
    const Vector2& getBitmapSize() const { return bitmapSize_; }
    void setBitmapData(const uint8* data, const Vector2& size);
    
    // Path data (for vector fonts)
    const std::string& getPathData() const { return pathData_; }
    void setPathData(const std::string& pathData) { pathData_ = pathData; }
    
    // Color glyph data
    const Color* getColorData() const { return colorData_; }
    const Vector2& getColorSize() const { return colorSize_; }
    void setColorData(const Color* data, const Vector2& size);
    
    // Utility methods
    bool isRenderable() const { return metrics_.isRenderable; }
    bool hasKerning() const { return !kerningMap_.empty(); }
    bool hasBitmap() const { return bitmapData_ != nullptr; }
    bool hasPath() const { return !pathData_.empty(); }
    bool hasColor() const { return colorData_ != nullptr; }
    
    // Comparison
    bool operator==(const Glyph& other) const;
    bool operator!=(const Glyph& other) const;
    bool operator<(const Glyph& other) const;
    
private:
    uint32 codepoint_;
    uint32 index_;
    bool isValid_;
    
    GlyphMetrics metrics_;
    Rect uvRect_;
    Vector2 textureSize_;
    uint32 textureID_;
    
    std::map<uint32, float32> kerningMap_;
    
    // Bitmap data
    const uint8* bitmapData_;
    Vector2 bitmapSize_;
    
    // Vector path data
    std::string pathData_;
    
    // Color glyph data
    const Color* colorData_;
    Vector2 colorSize_;
};

// Font face class
class FontFace {
public:
    FontFace();
    FontFace(const std::string& filePath);
    FontFace(const uint8* data, uint32 size);
    ~FontFace();
    
    // Loading
    bool loadFromFile(const std::string& filePath);
    bool loadFromMemory(const uint8* data, uint32 size);
    bool loadFromSystem(const std::string& familyName);
    void unload();
    
    bool isLoaded() const { return isLoaded_; }
    const std::string& getFilePath() const { return filePath_; }
    
    // Font information
    const FontInfo& getInfo() const { return info_; }
    FontType getType() const { return info_.type; }
    FontStyle getStyle() const { return info_.styleType; }
    FontWeight getWeight() const { return info_.weight; }
    
    // Glyph access
    Glyph* getGlyph(uint32 codepoint, float32 size = 12.0f);
    const Glyph* getGlyph(uint32 codepoint, float32 size = 12.0f) const;
    
    bool hasGlyph(uint32 codepoint) const;
    uint32 getGlyphCount() const { return info_.glyphCount; }
    
    // Metrics
    float32 getAscender(float32 size) const;
    float32 getDescender(float32 size) const;
    float32 getLineHeight(float32 size) const;
    float32 getCapHeight(float32 size) const;
    float32 getXHeight(float32 size) const;
    float32 getKerning(uint32 leftCodepoint, uint32 rightCodepoint, float32 size) const;
    
    // Text measurement
    Vector2 measureText(const std::string& text, float32 size, float32 letterSpacing = 0.0f) const;
    Vector2 measureText(const std::u32string& text, float32 size, float32 letterSpacing = 0.0f) const;
    
    // Character iteration
    std::vector<Glyph*> getGlyphs(const std::string& text, float32 size);
    std::vector<const Glyph*> getGlyphs(const std::string& text, float32 size) const;
    
    // Font variations (for variable fonts)
    bool isVariable() const { return info_.isVariable; }
    std::vector<std::string> getVariationAxes() const;
    float32 getVariationValue(const std::string& axis) const;
    void setVariationValue(const std::string& axis, float32 value);
    
    // OpenType features
    bool hasFeature(const std::string& feature) const;
    void enableFeature(const std::string& feature);
    void disableFeature(const std::string& feature);
    std::vector<std::string> getEnabledFeatures() const;
    
    // Cache management
    void clearCache();
    void clearCache(float32 size);
    uint32 getCacheSize() const;
    
private:
    bool isLoaded_;
    std::string filePath_;
    FontInfo info_;
    
    // Glyph cache (key: codepoint + size)
    mutable std::map<std::pair<uint32, float32>, std::unique_ptr<Glyph>> glyphCache_;
    
    // Font data
    std::unique_ptr<uint8[]> fontData_;
    uint32 fontDataSize_;
    
    // Implementation-specific data
    void* fontFaceImpl_;
    
    // Variation settings
    std::map<std::string, float32> variations_;
    
    // Feature settings
    std::set<std::string> enabledFeatures_;
};

// Font class
class Font {
public:
    Font();
    Font(const std::string& filePath);
    Font(const FontConfig& config);
    ~Font();
    
    // Loading
    bool loadFromFile(const std::string& filePath);
    bool loadFromMemory(const uint8* data, uint32 size);
    bool loadFromSystem(const std::string& familyName);
    void unload();
    
    bool isLoaded() const { return isLoaded_; }
    const std::string& getFilePath() const { return filePath_; }
    
    // Configuration
    const FontConfig& getConfig() const { return config_; }
    void setConfig(const FontConfig& config) { config_ = config; }
    
    // Font face access
    FontFace* getFontFace() { return fontFace_.get(); }
    const FontFace* getFontFace() const { return fontFace_.get(); }
    
    // Glyph access
    Glyph* getGlyph(uint32 codepoint);
    const Glyph* getGlyph(uint32 codepoint) const;
    
    bool hasGlyph(uint32 codepoint) const;
    
    // Metrics
    float32 getSize() const { return config_.size; }
    float32 getAscender() const;
    float32 getDescender() const;
    float32 getLineHeight() const;
    float32 getCapHeight() const;
    float32 getXHeight() const;
    float32 getKerning(uint32 leftCodepoint, uint32 rightCodepoint) const;
    
    // Text measurement
    Vector2 measureText(const std::string& text) const;
    Vector2 measureText(const std::u32string& text) const;
    
    // Character iteration
    std::vector<Glyph*> getGlyphs(const std::string& text);
    std::vector<const Glyph*> getGlyphs(const std::string& text) const;
    
    // Rendering
    void renderGlyph(const Glyph& glyph, const Vector2& position, const Color& color = Color::WHITE);
    void renderText(const std::string& text, const Vector2& position, const Color& color = Color::WHITE);
    void renderText(const std::string& text, const Transform& transform, const Color& color = Color::WHITE);
    
    // Batch rendering
    void beginBatch();
    void endBatch();
    void flushBatch();
    
    // Utility methods
    FontType getType() const;
    FontStyle getStyle() const;
    FontWeight getWeight() const;
    
    bool isMonospace() const;
    bool isVariable() const;
    
    // Cloning
    std::unique_ptr<Font> clone() const;
    std::unique_ptr<Font> clone(const FontConfig& newConfig) const;
    
private:
    bool isLoaded_;
    std::string filePath_;
    FontConfig config_;
    
    std::unique_ptr<FontFace> fontFace_;
    
    // Rendering state
    bool batchRendering_;
    std::vector<Glyph*> batchGlyphs_;
    std::vector<Vector2> batchPositions_;
    std::vector<Color> batchColors_;
};

// Text layout class
class TextLayout {
public:
    TextLayout();
    TextLayout(const std::string& text, Font* font);
    TextLayout(const std::string& text, Font* font, const TextLayoutConfig& config);
    ~TextLayout();
    
    // Text content
    const std::string& getText() const { return text_; }
    void setText(const std::string& text);
    
    // Font
    Font* getFont() { return font_; }
    const Font* getFont() const { return font_; }
    void setFont(Font* font) { font_ = font; invalidate(); }
    
    // Configuration
    const TextLayoutConfig& getConfig() const { return config_; }
    void setConfig(const TextLayoutConfig& config) { config_ = config; invalidate(); }
    
    // Layout calculation
    void layout();
    bool isLayoutValid() const { return layoutValid_; }
    void invalidate() { layoutValid_ = false; }
    
    // Layout results
    const Vector2& getSize() const { return size_; }
    const Vector2& getBaseline() const { return baseline_; }
    const Rect& getBounds() const { return bounds_; }
    
    // Line information
    struct LineInfo {
        std::string text;
        Vector2 position;
        Vector2 size;
        float32 baseline;
        uint32 startChar;
        uint32 endChar;
    };
    
    const std::vector<LineInfo>& getLines() const { return lines_; }
    uint32 getLineCount() const { return static_cast<uint32>(lines_.size()); }
    const LineInfo& getLine(uint32 index) const;
    
    // Character information
    struct CharInfo {
        uint32 codepoint;
        Glyph* glyph;
        Vector2 position;
        Vector2 size;
        Vector2 advance;
        uint32 lineIndex;
        uint32 charIndex;
    };
    
    const std::vector<CharInfo>& getCharacters() const { return characters_; }
    uint32 getCharacterCount() const { return static_cast<uint32>(characters_.size()); }
    const CharInfo& getCharacter(uint32 index) const;
    
    // Hit testing
    uint32 getCharacterAt(const Vector2& point) const;
    Vector2 getCharacterPosition(uint32 charIndex) const;
    Rect getCharacterBounds(uint32 charIndex) const;
    
    // Selection
    void setSelection(uint32 start, uint32 end);
    void clearSelection();
    const std::pair<uint32, uint32>& getSelection() const { return selection_; }
    bool hasSelection() const { return selection_.first != selection_.second; }
    
    // Caret
    uint32 getCaretPosition() const { return caretPosition_; }
    void setCaretPosition(uint32 position);
    Vector2 getCaretPosition() const;
    Rect getCaretBounds() const;
    
    // Clipping
    void setClipRect(const Rect& clipRect);
    const Rect& getClipRect() const { return clipRect_; }
    bool isClipped() const { return isClipped_; }
    
    // Rendering
    void render(const Transform& transform = Transform::IDENTITY);
    void renderSelection(const Color& selectionColor = Color(0.3f, 0.3f, 1.0f, 0.5f));
    void renderCaret(const Color& caretColor = Color::BLACK);
    
    // Utility methods
    bool isTextTruncated() const;
    std::string getTruncatedText() const;
    std::string getVisibleText() const;
    
private:
    std::string text_;
    Font* font_;
    TextLayoutConfig config_;
    
    bool layoutValid_;
    Vector2 size_;
    Vector2 baseline_;
    Rect bounds_;
    
    std::vector<LineInfo> lines_;
    std::vector<CharInfo> characters_;
    
    std::pair<uint32, uint32> selection_;
    uint32 caretPosition_;
    
    Rect clipRect_;
    bool isClipped_;
    
    void calculateLayout();
    void calculateLines();
    void calculateCharacters();
    void calculateBounds();
    void applyWrapping();
    void applyAlignment();
    void applyJustification();
    void applyHyphenation();
    void applyEllipsis();
};

// Text renderer class
class TextRenderer {
public:
    TextRenderer();
    TextRenderer(const TextRenderingConfig& config);
    ~TextRenderer();
    
    // Configuration
    const TextRenderingConfig& getConfig() const { return config_; }
    void setConfig(const TextRenderingConfig& config) { config_ = config; }
    
    // Rendering
    void begin();
    void end();
    void flush();
    
    void renderGlyph(const Glyph& glyph, const Vector2& position, const Color& color);
    void renderGlyph(const Glyph& glyph, const Transform& transform, const Color& color);
    
    void renderText(const std::string& text, Font* font, const Vector2& position, const Color& color = Color::WHITE);
    void renderText(const std::string& text, Font* font, const Transform& transform, const Color& color = Color::WHITE);
    
    void renderTextLayout(const TextLayout& layout, const Transform& transform = Transform::IDENTITY);
    
    // Batch rendering
    void beginBatch();
    void endBatch();
    void addToBatch(const Glyph& glyph, const Vector2& position, const Color& color);
    void addToBatch(const Glyph& glyph, const Transform& transform, const Color& color);
    
    // Statistics
    uint32 getRenderedGlyphCount() const { return renderedGlyphCount_; }
    uint32 getBatchCount() const { return batchCount_; }
    uint32 getDrawCallCount() const { return drawCallCount_; }
    void resetStatistics();
    
    // Utility methods
    void setViewport(const Rect& viewport);
    void setTransform(const Transform& transform);
    void setDepth(float32 depth);
    
    // Shader management
    bool loadShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    void setShaderUniforms(const std::map<std::string, float32>& uniforms);
    
private:
    TextRenderingConfig config_;
    
    bool rendering_;
    bool batchRendering_;
    
    std::vector<Glyph> batchGlyphs_;
    std::vector<Vector2> batchPositions_;
    std::vector<Color> batchColors_;
    std::vector<Transform> batchTransforms_;
    
    uint32 renderedGlyphCount_;
    uint32 batchCount_;
    uint32 drawCallCount_;
    
    // Implementation-specific data
    void* rendererImpl_;
    
    void setupRenderer();
    void cleanupRenderer();
    void renderBatch();
};

// Font manager class
class FontManager {
public:
    static FontManager& getInstance();
    
    // Font registration
    bool registerFont(const std::string& name, const std::string& filePath);
    bool registerFont(const std::string& name, const uint8* data, uint32 size);
    bool registerSystemFont(const std::string& name, const std::string& familyName);
    void unregisterFont(const std::string& name);
    
    // Font creation
    std::unique_ptr<Font> createFont(const std::string& name);
    std::unique_ptr<Font> createFont(const std::string& name, const FontConfig& config);
    
    // Font access
    bool hasFont(const std::string& name) const;
    std::vector<std::string> getFontNames() const;
    std::string getFontPath(const std::string& name) const;
    
    // System fonts
    std::vector<std::string> getSystemFontFamilies() const;
    bool hasSystemFont(const std::string& familyName) const;
    
    // Font discovery
    std::vector<std::string> discoverFonts(const std::string& directory) const;
    std::vector<std::string> discoverSystemFonts() const;
    
    // Default fonts
    void setDefaultFont(const std::string& name);
    const std::string& getDefaultFont() const { return defaultFontName_; }
    
    // Font fallback
    void addFallbackFont(const std::string& primaryFont, const std::string& fallbackFont);
    std::vector<std::string> getFallbackFonts(const std::string& primaryFont) const;
    
    // Cache management
    void clearCache();
    void clearFontCache(const std::string& name);
    uint32 getCacheSize() const;
    
private:
    FontManager();
    ~FontManager();
    
    std::map<std::string, std::string> registeredFonts_;
    std::map<std::string, std::vector<std::string>> fallbackFonts_;
    std::string defaultFontName_;
    
    // Implementation-specific data
    void* managerImpl_;
};

// Font cache class
class FontCache {
public:
    FontCache(uint32 maxSize = 100);
    ~FontCache();
    
    // Cache management
    void put(const std::string& key, std::unique_ptr<Font> font);
    std::unique_ptr<Font> get(const std::string& key);
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    void clear();
    
    // Cache properties
    uint32 getMaxSize() const { return maxSize_; }
    void setMaxSize(uint32 maxSize) { maxSize_ = maxSize; }
    uint32 getCurrentSize() const { return cache_.size(); }
    
    // Cache statistics
    uint32 getHitCount() const { return hitCount_; }
    uint32 getMissCount() const { return missCount_; }
    float32 getHitRate() const;
    void resetStatistics();
    
    // Cache policies
    enum class EvictionPolicy {
        LeastRecentlyUsed,
        LeastFrequentlyUsed,
        FirstInFirstOut
    };
    
    EvictionPolicy getEvictionPolicy() const { return evictionPolicy_; }
    void setEvictionPolicy(EvictionPolicy policy) { evictionPolicy_ = policy; }
    
private:
    uint32 maxSize_;
    EvictionPolicy evictionPolicy_;
    
    std::map<std::string, std::unique_ptr<Font>> cache_;
    std::map<std::string, uint32> accessTimes_;
    std::map<std::string, uint32> accessCounts_;
    uint32 currentTime_;
    
    uint32 hitCount_;
    uint32 missCount_;
    
    void evictIfNeeded();
    void updateAccessInfo(const std::string& key);
};

// Font API namespace
namespace FontAPI {

// System functions
bool initialize();
void shutdown();
bool isInitialized();

// Font creation and management
Font* createFont(const std::string& filePath);
Font* createFont(const std::string& filePath, const FontConfig& config);
Font* createSystemFont(const std::string& familyName);
Font* createSystemFont(const std::string& familyName, const FontConfig& config);
void destroyFont(Font* font);

// Font loading
bool loadFont(Font* font, const std::string& filePath);
bool loadFontFromMemory(Font* font, const uint8* data, uint32 size);
bool loadSystemFont(Font* font, const std::string& familyName);
void unloadFont(Font* font);

// Font information
FontInfo getFontInfo(const Font* font);
FontType getFontType(const Font* font);
FontStyle getFontStyle(const Font* font);
FontWeight getFontWeight(const Font* font);

// Glyph access
Glyph* getGlyph(Font* font, uint32 codepoint);
const Glyph* getGlyph(const Font* font, uint32 codepoint);
bool hasGlyph(const Font* font, uint32 codepoint);

// Text measurement
Vector2 measureText(const Font* font, const std::string& text);
Vector2 measureText(const Font* font, const std::string& text, float32 letterSpacing);
Rect getTextBounds(const Font* font, const std::string& text, const Vector2& position);

// Text rendering
void renderText(Font* font, const std::string& text, const Vector2& position);
void renderText(Font* font, const std::string& text, const Vector2& position, const Color& color);
void renderText(Font* font, const std::string& text, const Transform& transform);
void renderText(Font* font, const std::string& text, const Transform& transform, const Color& color);

// Text layout
TextLayout* createTextLayout(const std::string& text, Font* font);
TextLayout* createTextLayout(const std::string& text, Font* font, const TextLayoutConfig& config);
void destroyTextLayout(TextLayout* layout);

void layoutText(TextLayout* layout);
Vector2 getLayoutSize(const TextLayout* layout);
Rect getLayoutBounds(const TextLayout* layout);
void renderTextLayout(TextLayout* layout);
void renderTextLayout(TextLayout* layout, const Transform& transform);

// Font manager
FontManager* getFontManager();
bool registerFont(const std::string& name, const std::string& filePath);
bool registerSystemFont(const std::string& name, const std::string& familyName);
void unregisterFont(const std::string& name);
Font* getFont(const std::string& name);
Font* getFont(const std::string& name, const FontConfig& config);

// Font cache
FontCache* createFontCache(uint32 maxSize = 100);
void destroyFontCache(FontCache* cache);
void cacheFont(FontCache* cache, const std::string& key, Font* font);
Font* getCachedFont(FontCache* cache, const std::string& key);
bool hasCachedFont(const FontCache* cache, const std::string& key);
void clearFontCache(FontCache* cache);

// Utility functions
std::string fontTypeToString(FontType type);
std::string fontStyleToString(FontStyle style);
std::string fontWeightToString(FontWeight weight);
std::string textAlignmentToString(TextAlignment alignment);
std::string textBaselineToString(TextBaseline baseline);
std::string textDirectionToString(TextDirection direction);
std::string textWrappingToString(TextWrapping wrapping);

FontType stringToFontType(const std::string& str);
FontStyle stringToFontStyle(const std::string& str);
FontWeight stringToFontWeight(const std::string& str);
TextAlignment stringToTextAlignment(const std::string& str);
TextBaseline stringToTextBaseline(const std::string& str);
TextDirection stringToTextDirection(const std::string& str);
TextWrapping stringToTextWrapping(const std::string& str);

// Validation functions
bool isValidFont(const Font* font);
bool isValidGlyph(const Glyph* glyph);
bool isValidTextLayout(const TextLayout* layout);
bool isValidFontConfig(const FontConfig& config);
bool isValidTextLayoutConfig(const TextLayoutConfig& config);

// Default configurations
FontConfig getDefaultFontConfig();
FontConfig getHighQualityFontConfig();
FontConfig getLowQualityFontConfig();
FontConfig getMonospaceFontConfig();
FontConfig getVariableFontConfig();

TextLayoutConfig getDefaultTextLayoutConfig();
TextLayoutConfig getJustifiedTextLayoutConfig();
TextLayoutConfig getCenteredTextLayoutConfig();
TextLayoutConfig getMultilineTextLayoutConfig();

TextRenderingConfig getDefaultTextRenderingConfig();
TextRenderingConfig getHighQualityTextRenderingConfig();
TextRenderingConfig getLowQualityTextRenderingConfig();
TextRenderingConfig getBatchedTextRenderingConfig();

} // namespace FontAPI

} // namespace RFFont