#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <glm/glm.hpp>

namespace FontBaker {

struct Glyph {
    uint32_t charCode;        // Unicode character code
    uint32_t glyphIndex;      // Index in font
    glm::ivec2 size;         // Width and height of glyph
    glm::ivec2 bearing;       // Offset from baseline to left/top of glyph
    uint32_t advance;         // Horizontal advance to next glyph
    glm::vec2 uvTopLeft;     // UV coordinates in texture atlas
    glm::vec2 uvBottomRight;  // UV coordinates in texture atlas
    uint32_t texturePage;     // Which texture page this glyph is on
    std::vector<uint8_t> bitmap; // Raw bitmap data
};

struct KerningPair {
    uint32_t leftGlyph;       // Left glyph index
    uint32_t rightGlyph;      // Right glyph index
    int32_t offset;          // Kerning offset
};

struct FontMetrics {
    uint32_t fontSize;        // Font size in pixels
    int32_t ascent;          // Distance from baseline to highest point
    int32_t descent;         // Distance from baseline to lowest point
    int32_t lineGap;         // Distance between lines
    uint32_t lineHeight;      // Total line height (ascent - descent + lineGap)
    uint32_t maxAdvance;     // Maximum advance width
    glm::ivec2 maxGlyphSize; // Maximum glyph dimensions
};

class FontFace {
public:
    FontFace();
    ~FontFace();
    
    bool loadFromFile(const std::string& filename);
    bool loadFromMemory(const std::vector<uint8_t>& data);
    
    void setPixelSize(uint32_t size);
    uint32_t getPixelSize() const { return pixelSize_; }
    
    Glyph* getGlyph(uint32_t charCode);
    const Glyph* getGlyph(uint32_t charCode) const;
    
    int32_t getKerning(uint32_t leftGlyph, uint32_t rightGlyph) const;
    
    const FontMetrics& getMetrics() const { return metrics_; }
    
    const std::vector<Glyph>& getGlyphs() const { return glyphs_; }
    const std::vector<KerningPair>& getKerningPairs() const { return kerningPairs_; }
    
    bool hasKerning() const { return !kerningPairs_.empty(); }
    
private:
    std::vector<uint8_t> fontData_;
    uint32_t pixelSize_;
    FontMetrics metrics_;
    std::vector<Glyph> glyphs_;
    std::vector<KerningPair> kerningPairs_;
    std::map<uint32_t, size_t> glyphIndexMap_;
    
    void* fontFace_;         // Platform-specific font handle
    bool initialized_;
    
    void updateMetrics();
    void buildGlyphIndexMap();
};

class TextureAtlas {
public:
    struct Page {
        uint32_t width;
        uint32_t height;
        std::vector<uint8_t> data;
        std::vector<Glyph*> glyphs;
    };
    
    TextureAtlas();
    ~TextureAtlas();
    
    bool addGlyph(Glyph* glyph);
    void clear();
    
    const std::vector<Page>& getPages() const { return pages_; }
    uint32_t getPageCount() const { return static_cast<uint32_t>(pages_.size()); }
    
    void setMaxPageSize(uint32_t size) { maxPageSize_ = size; }
    uint32_t getMaxPageSize() const { return maxPageSize_; }
    
    void setPadding(uint32_t padding) { padding_ = padding; }
    uint32_t getPadding() const { return padding_; }
    
private:
    std::vector<Page> pages_;
    uint32_t maxPageSize_;
    uint32_t padding_;
    
    struct Rectangle {
        uint32_t x, y, width, height;
        bool filled;
    };
    
    std::vector<Rectangle> findFreeRectangle(Page* page, uint32_t width, uint32_t height);
    bool canFitGlyph(Page* page, uint32_t width, uint32_t height, Rectangle& rect);
    void addGlyphToPage(Page* page, Glyph* glyph, const Rectangle& rect);
};

class FontBaker {
public:
    enum class BakeFormat {
        BINARY,
        JSON,
        XML,
        CUSTOM
    };
    
    struct BakeSettings {
        uint32_t fontSize = 32;
        uint32_t atlasSize = 1024;
        uint32_t padding = 2;
        bool generateMipmaps = false;
        bool includeKerning = true;
        bool generateSDF = false;
        bool generateMSDF = false;
        float sdfSpread = 8.0f;
        uint32_t sdfRange = 4;
        BakeFormat outputFormat = BakeFormat::BINARY;
        std::string customExtension = ".font";
        std::vector<uint32_t> characterSet; // Unicode character codes to include
    };
    
    FontBaker();
    ~FontBaker();
    
    bool bakeFont(const std::string& inputFile, const std::string& outputFile, const BakeSettings& settings);
    
    void setCharacterSet(const std::vector<uint32_t>& chars);
    void addCharacter(uint32_t charCode);
    void addCharacterRange(uint32_t start, uint32_t end);
    void addASCIICharacters();
    void addCommonUnicodeCharacters();
    
    const FontFace* getFontFace() const { return fontFace_.get(); }
    const TextureAtlas* getTextureAtlas() const { return textureAtlas_.get(); }
    
    bool saveAtlas(const std::string& filename, const std::string& format = "png");
    bool saveFontData(const std::string& filename, const BakeSettings& settings);
    
private:
    std::unique_ptr<FontFace> fontFace_;
    std::unique_ptr<TextureAtlas> textureAtlas_;
    std::vector<uint32_t> characterSet_;
    
    bool loadFont(const std::string& filename, uint32_t fontSize);
    bool rasterizeGlyphs();
    bool generateSDFGlyphs();
    bool generateMSDFGlyphs();
    bool packGlyphs();
    
    void optimizeCharacterSet();
    void sortCharactersByFrequency();
    
    bool saveAsBinary(const std::string& filename, const BakeSettings& settings);
    bool saveAsJSON(const std::string& filename, const BakeSettings& settings);
    bool saveAsXML(const std::string& filename, const BakeSettings& settings);
    
    std::string generateFontInfo(const BakeSettings& settings) const;
};

} // namespace FontBaker