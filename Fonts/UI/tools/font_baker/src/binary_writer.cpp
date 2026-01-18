#include "writer.h"
#include "baker.h"
#include <fstream>
#include <vector>
#include <cstring>

namespace FontBaker {

class BinaryWriterImpl {
public:
    struct BinaryHeader {
        char signature[4];      // "FONT"
        uint32_t version;        // File format version
        uint32_t glyphCount;     // Number of glyphs
        uint32_t kerningCount;   // Number of kerning pairs
        uint32_t atlasWidth;      // Atlas width
        uint32_t atlasHeight;     // Atlas height
        uint32_t atlasChannels;   // Atlas channels
        uint32_t fontSize;        // Font size used for baking
        float unitsPerEm;         // Units per EM
        float ascent;             // Font ascent
        float descent;            // Font descent
        float lineGap;           // Line gap
        char fontName[256];      // Font name (null-terminated)
        char fontFamily[256];     // Font family (null-terminated)
    };
    
    struct BinaryGlyph {
        uint32_t characterCode;   // Unicode character code
        float x;                 // Glyph X position in atlas (normalized)
        float y;                 // Glyph Y position in atlas (normalized)
        float width;              // Glyph width in atlas (normalized)
        float height;             // Glyph height in atlas (normalized)
        float advanceX;           // Horizontal advance
        float advanceY;           // Vertical advance
        float bearingX;           // Horizontal bearing
        float bearingY;           // Vertical bearing
        uint32_t outlinePoints;   // Number of outline points
        uint32_t boundingBoxX;    // Bounding box X
        uint32_t boundingBoxY;    // Bounding box Y
        uint32_t boundingBoxW;    // Bounding box width
        uint32_t boundingBoxH;    // Bounding box height
    };
    
    struct BinaryKerningPair {
        uint32_t leftGlyph;       // Left glyph index
        uint32_t rightGlyph;      // Right glyph index
        float kerningAmount;      // Kerning amount
    };
    
    struct BinaryOutlinePoint {
        float x;                 // Point X coordinate
        float y;                 // Point Y coordinate
        uint8_t onCurve;         // Whether point is on curve
        uint8_t reserved[3];     // Reserved for future use
    };
    
    BinaryWriterImpl();
    ~BinaryWriterImpl() = default;
    
    bool writeFont(const FontFace& font, const TextureAtlas& atlas, const std::string& filename);
    bool readFont(FontFace& font, TextureAtlas& atlas, const std::string& filename);
    
private:
    void writeHeader(std::ofstream& file, const FontFace& font, const TextureAtlas& atlas);
    void writeGlyphs(std::ofstream& file, const std::vector<Glyph>& glyphs);
    void writeKerningPairs(std::ofstream& file, const std::vector<KerningPair>& kerningPairs);
    void writeAtlas(std::ofstream& file, const TextureAtlas& atlas);
    void writeString(std::ofstream& file, const std::string& str, size_t maxLength);
    
    bool readHeader(std::ifstream& file, FontFace& font, TextureAtlas& atlas);
    bool readGlyphs(std::ifstream& file, std::vector<Glyph>& glyphs);
    bool readKerningPairs(std::ifstream& file, std::vector<KerningPair>& kerningPairs);
    bool readAtlas(std::ifstream& file, TextureAtlas& atlas);
    std::string readString(std::ifstream& file, size_t maxLength);
    
    // Utility functions
    void writeFloat(std::ofstream& file, float value);
    void writeUInt32(std::ofstream& file, uint32_t value);
    void writeUInt16(std::ofstream& file, uint16_t value);
    void writeUInt8(std::ofstream& file, uint8_t value);
    
    float readFloat(std::ifstream& file);
    uint32_t readUInt32(std::ifstream& file);
    uint16_t readUInt16(std::ifstream& file);
    uint8_t readUInt8(std::ifstream& file);
    
    // Compression
    std::vector<uint8_t> compressData(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decompressData(const std::vector<uint8_t>& data);
    
    // Validation
    bool validateHeader(const BinaryHeader& header);
    uint32_t calculateChecksum(const void* data, size_t size);
};

BinaryWriterImpl::BinaryWriterImpl() {}

bool BinaryWriterImpl::writeFont(const FontFace& font, const TextureAtlas& atlas, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    writeHeader(file, font, atlas);
    
    // Write glyphs
    writeGlyphs(file, font.glyphs);
    
    // Write kerning pairs
    writeKerningPairs(file, font.kerningPairs);
    
    // Write atlas data
    writeAtlas(file, atlas);
    
    // Write checksum
    uint32_t checksum = 0; // Would calculate actual checksum
    writeUInt32(file, checksum);
    
    file.close();
    return true;
}

bool BinaryWriterImpl::readFont(FontFace& font, TextureAtlas& atlas, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Read header
    if (!readHeader(file, font, atlas)) {
        file.close();
        return false;
    }
    
    // Read glyphs
    if (!readGlyphs(file, font.glyphs)) {
        file.close();
        return false;
    }
    
    // Read kerning pairs
    if (!readKerningPairs(file, font.kerningPairs)) {
        file.close();
        return false;
    }
    
    // Read atlas data
    if (!readAtlas(file, atlas)) {
        file.close();
        return false;
    }
    
    // Read and verify checksum
    uint32_t storedChecksum = readUInt32(file);
    uint32_t calculatedChecksum = 0; // Would calculate actual checksum
    
    file.close();
    
    return storedChecksum == calculatedChecksum;
}

void BinaryWriterImpl::writeHeader(std::ofstream& file, const FontFace& font, const TextureAtlas& atlas) {
    BinaryHeader header;
    
    // Set signature
    header.signature[0] = 'F';
    header.signature[1] = 'O';
    header.signature[2] = 'N';
    header.signature[3] = 'T';
    
    header.version = 1;
    header.glyphCount = static_cast<uint32_t>(font.glyphs.size());
    header.kerningCount = static_cast<uint32_t>(font.kerningPairs.size());
    header.atlasWidth = atlas.width;
    header.atlasHeight = atlas.height;
    header.atlasChannels = atlas.channels;
    header.fontSize = 32; // Would get from bake settings
    header.unitsPerEm = font.unitsPerEm;
    header.ascent = font.metrics.ascent;
    header.descent = font.metrics.descent;
    header.lineGap = font.metrics.lineGap;
    
    // Clear and set font name
    memset(header.fontName, 0, sizeof(header.fontName));
    writeString(file, font.name, sizeof(header.fontName));
    
    // Clear and set font family
    memset(header.fontFamily, 0, sizeof(header.fontFamily));
    writeString(file, font.family, sizeof(header.fontFamily));
    
    // Write header data
    file.write(reinterpret_cast<const char*>(&header), sizeof(BinaryHeader));
}

void BinaryWriterImpl::writeGlyphs(std::ofstream& file, const std::vector<Glyph>& glyphs) {
    for (const auto& glyph : glyphs) {
        BinaryGlyph binaryGlyph;
        
        binaryGlyph.characterCode = glyph.characterCode;
        binaryGlyph.x = glyph.uvX;
        binaryGlyph.y = glyph.uvY;
        binaryGlyph.width = glyph.uvWidth;
        binaryGlyph.height = glyph.uvHeight;
        binaryGlyph.advanceX = glyph.advanceX;
        binaryGlyph.advanceY = glyph.advanceY;
        binaryGlyph.bearingX = glyph.bearingX;
        binaryGlyph.bearingY = glyph.bearingY;
        binaryGlyph.outlinePoints = static_cast<uint32_t>(glyph.outlinePoints.size());
        binaryGlyph.boundingBoxX = static_cast<uint32_t>(glyph.boundingBox.x);
        binaryGlyph.boundingBoxY = static_cast<uint32_t>(glyph.boundingBox.y);
        binaryGlyph.boundingBoxW = static_cast<uint32_t>(glyph.boundingBox.width);
        binaryGlyph.boundingBoxH = static_cast<uint32_t>(glyph.boundingBox.height);
        
        // Write glyph header
        file.write(reinterpret_cast<const char*>(&binaryGlyph), sizeof(BinaryGlyph));
        
        // Write outline points
        for (const auto& point : glyph.outlinePoints) {
            BinaryOutlinePoint binaryPoint;
            binaryPoint.x = point.x;
            binaryPoint.y = point.y;
            binaryPoint.onCurve = point.onCurve ? 1 : 0;
            memset(binaryPoint.reserved, 0, sizeof(binaryPoint.reserved));
            
            file.write(reinterpret_cast<const char*>(&binaryPoint), sizeof(BinaryOutlinePoint));
        }
    }
}

void BinaryWriterImpl::writeKerningPairs(std::ofstream& file, const std::vector<KerningPair>& kerningPairs) {
    for (const auto& kerning : kerningPairs) {
        BinaryKerningPair binaryKerning;
        
        binaryKerning.leftGlyph = kerning.leftGlyph;
        binaryKerning.rightGlyph = kerning.rightGlyph;
        binaryKerning.kerningAmount = kerning.kerningAmount;
        
        file.write(reinterpret_cast<const char*>(&binaryKerning), sizeof(BinaryKerningPair));
    }
}

void BinaryWriterImpl::writeAtlas(std::ofstream& file, const TextureAtlas& atlas) {
    // Write atlas dimensions
    writeUInt32(file, atlas.width);
    writeUInt32(file, atlas.height);
    writeUInt32(file, atlas.channels);
    
    // Write atlas data
    file.write(reinterpret_cast<const char*>(atlas.data.data()), atlas.data.size());
}

void BinaryWriterImpl::writeString(std::ofstream& file, const std::string& str, size_t maxLength) {
    size_t writeLength = std::min(str.length(), maxLength - 1);
    file.write(str.c_str(), writeLength);
    
    // Pad with zeros if needed
    if (writeLength < maxLength) {
        std::vector<char> padding(maxLength - writeLength, 0);
        file.write(padding.data(), padding.size());
    }
}

bool BinaryWriterImpl::readHeader(std::ifstream& file, FontFace& font, TextureAtlas& atlas) {
    BinaryHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(BinaryHeader));
    
    // Validate signature
    if (header.signature[0] != 'F' || header.signature[1] != 'O' ||
        header.signature[2] != 'N' || header.signature[3] != 'T') {
        return false;
    }
    
    // Validate version
    if (header.version != 1) {
        return false;
    }
    
    if (!validateHeader(header)) {
        return false;
    }
    
    // Set font properties
    font.name = readString(file, sizeof(header.fontName));
    font.family = readString(file, sizeof(header.fontFamily));
    font.unitsPerEm = header.unitsPerEm;
    font.metrics.ascent = header.ascent;
    font.metrics.descent = header.descent;
    font.metrics.lineGap = header.lineGap;
    font.metrics.numGlyphs = header.glyphCount;
    
    // Set atlas properties
    atlas.width = header.atlasWidth;
    atlas.height = header.atlasHeight;
    atlas.channels = header.atlasChannels;
    
    return true;
}

bool BinaryWriterImpl::readGlyphs(std::ifstream& file, std::vector<Glyph>& glyphs) {
    glyphs.clear();
    
    for (uint32_t i = 0; i < glyphs.capacity(); ++i) {
        BinaryGlyph binaryGlyph;
        file.read(reinterpret_cast<char*>(&binaryGlyph), sizeof(BinaryGlyph));
        
        Glyph glyph;
        glyph.characterCode = binaryGlyph.characterCode;
        glyph.uvX = binaryGlyph.x;
        glyph.uvY = binaryGlyph.y;
        glyph.uvWidth = binaryGlyph.width;
        glyph.uvHeight = binaryGlyph.height;
        glyph.advanceX = binaryGlyph.advanceX;
        glyph.advanceY = binaryGlyph.advanceY;
        glyph.bearingX = binaryGlyph.bearingX;
        glyph.bearingY = binaryGlyph.bearingY;
        glyph.boundingBox.x = static_cast<float>(binaryGlyph.boundingBoxX);
        glyph.boundingBox.y = static_cast<float>(binaryGlyph.boundingBoxY);
        glyph.boundingBox.width = static_cast<float>(binaryGlyph.boundingBoxW);
        glyph.boundingBox.height = static_cast<float>(binaryGlyph.boundingBoxH);
        
        // Read outline points
        glyph.outlinePoints.resize(binaryGlyph.outlinePoints);
        for (uint32_t j = 0; j < binaryGlyph.outlinePoints; ++j) {
            BinaryOutlinePoint binaryPoint;
            file.read(reinterpret_cast<char*>(&binaryPoint), sizeof(BinaryOutlinePoint));
            
            OutlinePoint point;
            point.x = binaryPoint.x;
            point.y = binaryPoint.y;
            point.onCurve = binaryPoint.onCurve != 0;
            
            glyph.outlinePoints[j] = point;
        }
        
        glyphs.push_back(glyph);
    }
    
    return true;
}

bool BinaryWriterImpl::readKerningPairs(std::ifstream& file, std::vector<KerningPair>& kerningPairs) {
    kerningPairs.clear();
    
    for (uint32_t i = 0; i < kerningPairs.capacity(); ++i) {
        BinaryKerningPair binaryKerning;
        file.read(reinterpret_cast<char*>(&binaryKerning), sizeof(BinaryKerningPair));
        
        KerningPair kerning;
        kerning.leftGlyph = binaryKerning.leftGlyph;
        kerning.rightGlyph = binaryKerning.rightGlyph;
        kerning.kerningAmount = binaryKerning.kerningAmount;
        
        kerningPairs.push_back(kerning);
    }
    
    return true;
}

bool BinaryWriterImpl::readAtlas(std::ifstream& file, TextureAtlas& atlas) {
    // Read atlas dimensions
    uint32_t width = readUInt32(file);
    uint32_t height = readUInt32(file);
    uint32_t channels = readUInt32(file);
    
    // Read atlas data
    size_t dataSize = width * height * channels;
    atlas.data.resize(dataSize);
    file.read(reinterpret_cast<char*>(atlas.data.data()), dataSize);
    
    return true;
}

std::string BinaryWriterImpl::readString(std::ifstream& file, size_t maxLength) {
    std::vector<char> buffer(maxLength);
    file.read(buffer.data(), maxLength);
    
    // Find null terminator
    size_t length = 0;
    for (size_t i = 0; i < maxLength; ++i) {
        if (buffer[i] == '\0') {
            length = i;
            break;
        }
    }
    
    return std::string(buffer.data(), length);
}

void BinaryWriterImpl::writeFloat(std::ofstream& file, float value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(float));
}

void BinaryWriterImpl::writeUInt32(std::ofstream& file, uint32_t value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(uint32_t));
}

void BinaryWriterImpl::writeUInt16(std::ofstream& file, uint16_t value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(uint16_t));
}

void BinaryWriterImpl::writeUInt8(std::ofstream& file, uint8_t value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(uint8_t));
}

float BinaryWriterImpl::readFloat(std::ifstream& file) {
    float value;
    file.read(reinterpret_cast<char*>(&value), sizeof(float));
    return value;
}

uint32_t BinaryWriterImpl::readUInt32(std::ifstream& file) {
    uint32_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
    return value;
}

uint16_t BinaryWriterImpl::readUInt16(std::ifstream& file) {
    uint16_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    return value;
}

uint8_t BinaryWriterImpl::readUInt8(std::ifstream& file) {
    uint8_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(uint8_t));
    return value;
}

std::vector<uint8_t> BinaryWriterImpl::compressData(const std::vector<uint8_t>& data) {
    // Simple compression - would use real compression library
    return data;
}

std::vector<uint8_t> BinaryWriterImpl::decompressData(const std::vector<uint8_t>& data) {
    // Simple decompression - would use real compression library
    return data;
}

bool BinaryWriterImpl::validateHeader(const BinaryHeader& header) {
    // Check for reasonable values
    if (header.glyphCount > 100000) return false;
    if (header.kerningCount > 1000000) return false;
    if (header.atlasWidth > 8192 || header.atlasHeight > 8192) return false;
    if (header.atlasChannels > 4) return false;
    if (header.fontSize > 1024) return false;
    if (header.unitsPerEm <= 0 || header.unitsPerEm > 10000) return false;
    
    return true;
}

uint32_t BinaryWriterImpl::calculateChecksum(const void* data, size_t size) {
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    uint32_t checksum = 0;
    
    for (size_t i = 0; i < size; ++i) {
        checksum = (checksum << 1) | (checksum >> 31);
        checksum += bytes[i];
    }
    
    return checksum;
}

// BinaryWriter interface implementation
BinaryWriter::BinaryWriter() : impl_(std::make_unique<BinaryWriterImpl>()) {}

BinaryWriter::~BinaryWriter() = default;

bool BinaryWriter::writeFont(const FontFace& font, const TextureAtlas& atlas, const std::string& filename) {
    return impl_->writeFont(font, atlas, filename);
}

bool BinaryWriter::readFont(FontFace& font, TextureAtlas& atlas, const std::string& filename) {
    return impl_->readFont(font, atlas, filename);
}

} // namespace FontBaker