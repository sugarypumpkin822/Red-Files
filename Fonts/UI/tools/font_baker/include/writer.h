#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "baker.h"

namespace FontBaker {

class BinaryWriter {
public:
    BinaryWriter();
    ~BinaryWriter();
    
    bool writeFontData(const std::string& filename, const FontFace* fontFace, const TextureAtlas* atlas);
    bool writeAtlasData(const std::string& filename, const TextureAtlas* atlas);
    
    void setCompressionEnabled(bool enabled) { compressionEnabled_ = enabled; }
    bool isCompressionEnabled() const { return compressionEnabled_; }
    
private:
    bool compressionEnabled_;
    
    struct Header {
        uint32_t magic;              // File format identifier
        uint32_t version;             // File format version
        uint32_t glyphCount;         // Number of glyphs
        uint32_t kerningCount;        // Number of kerning pairs
        uint32_t atlasPageCount;     // Number of atlas pages
        FontMetrics metrics;          // Font metrics
    };
    
    void writeHeader(std::ofstream& file, const Header& header);
    void writeGlyph(std::ofstream& file, const Glyph& glyph);
    void writeKerningPair(std::ofstream& file, const KerningPair& kerning);
    void writeAtlasPage(std::ofstream& file, const TextureAtlas::Page& page);
    
    uint32_t calculateChecksum(const std::vector<uint8_t>& data);
};

class JSONWriter {
public:
    JSONWriter();
    ~JSONWriter() = default;
    
    bool writeFontData(const std::string& filename, const FontFace* fontFace, const TextureAtlas* atlas);
    bool writeAtlasData(const std::string& filename, const TextureAtlas* atlas);
    
    void setPrettyPrint(bool enabled) { prettyPrint_ = enabled; }
    bool isPrettyPrint() const { return prettyPrint_; }
    
    void setIndentation(const std::string& indent) { indentation_ = indent; }
    const std::string& getIndentation() const { return indentation_; }
    
private:
    bool prettyPrint_;
    std::string indentation_;
    
    std::string writeFontObject(const FontFace* fontFace, const TextureAtlas* atlas);
    std::string writeGlyphArray(const std::vector<Glyph>& glyphs);
    std::string writeKerningArray(const std::vector<KerningPair>& kerningPairs);
    std::string writeMetricsObject(const FontMetrics& metrics);
    std::string writeAtlasArray(const std::vector<TextureAtlas::Page>& pages);
    std::string writeGlyphObject(const Glyph& glyph);
    std::string writeKerningObject(const KerningPair& kerning);
    std::string writeAtlasPageObject(const TextureAtlas::Page& page);
    
    std::string writeVector2(const glm::vec2& vec);
    std::string writeVector2Int(const glm::ivec2& vec);
    std::string writeByteArray(const std::vector<uint8_t>& data);
    
    std::string escapeString(const std::string& str);
    std::string indent(int level);
};

class XMLWriter {
public:
    XMLWriter();
    ~XMLWriter() = default;
    
    bool writeFontData(const std::string& filename, const FontFace* fontFace, const TextureAtlas* atlas);
    bool writeAtlasData(const std::string& filename, const TextureAtlas* atlas);
    
    void setIndentation(const std::string& indent) { indentation_ = indent; }
    const std::string& getIndentation() const { return indentation_; }
    
private:
    std::string indentation_;
    
    std::string writeFontDocument(const FontFace* fontFace, const TextureAtlas* atlas);
    std::string writeGlyphElement(const Glyph& glyph, int indentLevel);
    std::string writeKerningElement(const KerningPair& kerning, int indentLevel);
    std::string writeMetricsElement(const FontMetrics& metrics, int indentLevel);
    std::string writeAtlasElement(const std::vector<TextureAtlas::Page>& pages, int indentLevel);
    std::string writeAtlasPageElement(const TextureAtlas::Page& page, int indentLevel);
    
    std::string writeVector2Attribute(const glm::vec2& vec, const std::string& name);
    std::string writeVector2IntAttribute(const glm::ivec2& vec, const std::string& name);
    std::string writeByteArrayAttribute(const std::vector<uint8_t>& data, const std::string& name);
    
    std::string escapeXML(const std::string& str);
    std::string indent(int level);
};

class CustomWriter {
public:
    CustomWriter();
    ~CustomWriter() = default;
    
    bool writeFontData(const std::string& filename, const FontFace* fontFace, const TextureAtlas* atlas, 
                      const std::string& format = "custom");
    
    void setFormatTemplate(const std::string& templatePath) { templatePath_ = templatePath; }
    const std::string& getFormatTemplate() const { return templatePath_; }
    
private:
    std::string templatePath_;
    
    std::string processTemplate(const std::string& templateContent, const FontFace* fontFace, const TextureAtlas* atlas);
    std::string replaceTemplateVariable(const std::string& content, const std::string& variable, const std::string& value);
    
    std::string generateGlyphData(const std::vector<Glyph>& glyphs);
    std::string generateKerningData(const std::vector<KerningPair>& kerningPairs);
    std::string generateMetricsData(const FontMetrics& metrics);
    std::string generateAtlasData(const std::vector<TextureAtlas::Page>& pages);
};

class FileWriter {
public:
    enum class Format {
        BINARY,
        JSON,
        XML,
        CUSTOM
    };
    
    FileWriter();
    ~FileWriter() = default;
    
    bool writeFont(const std::string& filename, const FontFace* fontFace, const TextureAtlas* atlas, 
                   Format format = Format::BINARY);
    bool writeAtlas(const std::string& filename, const TextureAtlas* atlas, 
                   Format format = Format::BINARY);
    
    void setFormat(Format format) { format_ = format; }
    Format getFormat() const { return format_; }
    
    void setCompressionEnabled(bool enabled) { compressionEnabled_ = enabled; }
    bool isCompressionEnabled() const { return compressionEnabled_; }
    
    // Format-specific settings
    void setPrettyPrint(bool enabled) { prettyPrint_ = enabled; }
    void setIndentation(const std::string& indent) { indentation_ = indent; }
    void setCustomTemplate(const std::string& templatePath) { customTemplate_ = templatePath; }
    
private:
    Format format_;
    bool compressionEnabled_;
    bool prettyPrint_;
    std::string indentation_;
    std::string customTemplate_;
    
    bool writeBinary(const std::string& filename, const FontFace* fontFace, const TextureAtlas* atlas);
    bool writeJSON(const std::string& filename, const FontFace* fontFace, const TextureAtlas* atlas);
    bool writeXML(const std::string& filename, const FontFace* fontFace, const TextureAtlas* atlas);
    bool writeCustom(const std::string& filename, const FontFace* fontFace, const TextureAtlas* atlas);
    
    std::string generateFilename(const std::string& baseFilename, const std::string& extension);
    bool compressData(const std::vector<uint8_t>& data, std::vector<uint8_t>& compressed);
    bool decompressData(const std::vector<uint8_t>& compressed, std::vector<uint8_t>& data);
};

} // namespace FontBaker