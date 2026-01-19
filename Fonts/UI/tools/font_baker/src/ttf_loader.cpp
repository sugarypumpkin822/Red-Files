#include "baker.h"
#include "rasterizer.h"
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>

namespace FontBaker {

class TTFLoader {
public:
    struct TTFHeader {
        uint32_t sfntVersion;
        uint16_t numTables;
        uint16_t searchRange;
        uint16_t entrySelector;
        uint16_t rangeShift;
    };
    
    struct TableEntry {
        uint32_t tag;
        uint32_t checksum;
        uint32_t offset;
        uint32_t length;
    };
    
    struct GlyphHeader {
        int16_t numberOfContours;
        int16_t xMin;
        int16_t yMin;
        int16_t xMax;
        int16_t yMax;
    };
    
    TTFLoader();
    ~TTFLoader() = default;
    
    bool loadFont(const std::string& filename);
    FontFace getFontFace() const;
    
    // Glyph data extraction
    std::vector<Glyph> extractGlyphs(const std::vector<uint32_t>& characterSet);
    std::vector<KerningPair> extractKerningPairs();
    FontMetrics getFontMetrics() const;
    
    // Font information
    std::string getFontName() const;
    std::string getFontFamily() const;
    uint32_t getUnitsPerEm() const;
    
private:
    std::vector<uint8_t> fontData_;
    TTFHeader header_;
    std::vector<TableEntry> tables_;
    
    // Table parsing
    bool parseHeader();
    bool parseTableDirectory();
    TableEntry* findTable(uint32_t tag);
    
    // Specific table parsers
    bool parseHeadTable();
    bool parseHheaTable();
    bool parseMaxpTable();
    bool parseCmapTable();
    bool parseGlyfTable();
    bool parseHmtxTable();
    bool parseKernTable();
    bool parseNameTable();
    bool parseOS2Table();
    
    // Glyph parsing
    Glyph parseGlyph(uint16_t glyphIndex);
    void parseSimpleGlyph(Glyph& glyph, const uint8_t* data, size_t offset);
    void parseCompositeGlyph(Glyph& glyph, const uint8_t* data, size_t offset);
    
    // Helper functions
    uint16_t readUInt16(const uint8_t* data, size_t offset);
    uint32_t readUInt32(const uint8_t* data, size_t offset);
    int16_t readInt16(const uint8_t* data, size_t offset);
    int32_t readInt32(const uint8_t* data, size_t offset);
    uint32_t calculateChecksum(const uint8_t* data, size_t length);
    
    // Font metrics storage
    FontMetrics metrics_;
    std::string fontName_;
    std::string fontFamily_;
    uint32_t unitsPerEm_;
    
    // Character to glyph mapping
    std::map<uint32_t, uint16_t> charToGlyphMap_;
    
    // Glyph data cache
    std::vector<Glyph> glyphs_;
    std::vector<KerningPair> kerningPairs_;
    
    // Table offsets
    size_t glyfTableOffset_;
    size_t hmtxTableOffset_;
    size_t locaTableOffset_;
    size_t cmapTableOffset_;
    size_t kernTableOffset_;
    
    bool isShortIndex_;
};

TTFLoader::TTFLoader() : unitsPerEm_(0), isShortIndex_(false) {}

bool TTFLoader::loadFont(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    fontData_.resize(fileSize);
    file.read(reinterpret_cast<char*>(fontData_.data()), fileSize);
    file.close();
    
    // Parse TTF structure
    if (!parseHeader()) {
        return false;
    }
    
    if (!parseTableDirectory()) {
        return false;
    }
    
    // Parse required tables
    if (!parseHeadTable() || !parseHheaTable() || !parseMaxpTable() || 
        !parseCmapTable() || !parseNameTable()) {
        return false;
    }
    
    // Parse optional tables
    parseOS2Table();
    parseKernTable();
    
    return true;
}

bool TTFLoader::parseHeader() {
    if (fontData_.size() < sizeof(TTFHeader)) {
        return false;
    }
    
    const uint8_t* data = fontData_.data();
    header_.sfntVersion = readUInt32(data, 0);
    header_.numTables = readUInt16(data, 4);
    header_.searchRange = readUInt16(data, 6);
    header_.entrySelector = readUInt16(data, 8);
    header_.rangeShift = readUInt16(data, 10);
    
    // Check for valid TTF/OTF signature
    if (header_.sfntVersion != 0x00010000 && header_.sfntVersion != 0x74727565) {
        return false;
    }
    
    return true;
}

bool TTFLoader::parseTableDirectory() {
    const uint8_t* data = fontData_.data();
    size_t offset = 12; // After header
    
    tables_.resize(header_.numTables);
    
    for (uint16_t i = 0; i < header_.numTables; ++i) {
        tables_[i].tag = readUInt32(data, offset);
        tables_[i].checksum = readUInt32(data, offset + 4);
        tables_[i].offset = readUInt32(data, offset + 8);
        tables_[i].length = readUInt32(data, offset + 12);
        offset += 16;
    }
    
    return true;
}

TableEntry* TTFLoader::findTable(uint32_t tag) {
    for (auto& table : tables_) {
        if (table.tag == tag) {
            return &table;
        }
    }
    return nullptr;
}

bool TTFLoader::parseHeadTable() {
    TableEntry* headTable = findTable(0x68656164); // 'head'
    if (!headTable) return false;
    
    const uint8_t* data = fontData_.data() + headTable->offset;
    
    unitsPerEm_ = readUInt16(data, 18);
    
    // Check index format
    uint16_t indexFormat = readUInt16(data, 50);
    isShortIndex_ = (indexFormat == 0);
    
    return true;
}

bool TTFLoader::parseHheaTable() {
    TableEntry* hheaTable = findTable(0x68656161); // 'hhea'
    if (!hheaTable) return false;
    
    const uint8_t* data = fontData_.data() + hheaTable->offset;
    
    metrics_.ascent = readInt16(data, 4);
    metrics_.descent = readInt16(data, 6);
    metrics_.lineGap = readInt16(data, 8);
    
    return true;
}

bool TTFLoader::parseMaxpTable() {
    TableEntry* maxpTable = findTable(0x6D617870); // 'maxp'
    if (!maxpTable) return false;
    
    const uint8_t* data = fontData_.data() + maxpTable->offset;
    
    metrics_.numGlyphs = readUInt16(data, 4);
    
    return true;
}

bool TTFLoader::parseCmapTable() {
    TableEntry* cmapTable = findTable(0x636D6170); // 'cmap'
    if (!cmapTable) return false;
    
    const uint8_t* data = fontData_.data() + cmapTable->offset;
    uint16_t numSubtables = readUInt16(data, 2);
    
    // Find best subtable (Unicode, preferred)
    size_t subtableOffset = 0;
    for (uint16_t i = 0; i < numSubtables; ++i) {
        size_t entryOffset = 4 + i * 8;
        uint16_t platformID = readUInt16(data, entryOffset);
        uint16_t encodingID = readUInt16(data, entryOffset + 2);
        
        // Prefer Unicode platform
        if (platformID == 0 || (platformID == 3 && encodingID == 1)) {
            uint32_t subtableOffsetValue = readUInt32(data, entryOffset + 4);
            subtableOffset = cmapTable->offset + subtableOffsetValue;
            break;
        }
    }
    
    if (subtableOffset == 0) return false;
    
    // Parse format 4 subtable (most common)
    const uint8_t* subtableData = fontData_.data() + subtableOffset;
    uint16_t format = readUInt16(subtableData, 0);
    
    if (format == 4) {
        uint16_t segCount = readUInt16(subtableData, 6) / 2;
        size_t endCodesOffset = 14;
        size_t startCodesOffset = endCodesOffset + segCount * 2 + 2;
        size_t idDeltasOffset = startCodesOffset + segCount * 2;
        size_t idRangeOffsetsOffset = idDeltasOffset + segCount * 2;
        
        for (uint16_t i = 0; i < segCount; ++i) {
            uint16_t endCode = readUInt16(subtableData, endCodesOffset + i * 2);
            uint16_t startCode = readUInt16(subtableData, startCodesOffset + i * 2);
            int16_t idDelta = readInt16(subtableData, idDeltasOffset + i * 2);
            uint16_t idRangeOffset = readUInt16(subtableData, idRangeOffsetsOffset + i * 2);
            
            for (uint32_t charCode = startCode; charCode <= endCode; ++charCode) {
                uint16_t glyphIndex;
                
                if (idRangeOffset == 0) {
                    glyphIndex = (charCode + idDelta) & 0xFFFF;
                } else {
                    size_t glyphIndexOffset = idRangeOffsetsOffset + i * 2 + idRangeOffset + 
                                           (charCode - startCode) * 2;
                    glyphIndex = readUInt16(subtableData, glyphIndexOffset);
                    if (glyphIndex != 0) {
                        glyphIndex = (glyphIndex + idDelta) & 0xFFFF;
                    }
                }
                
                if (glyphIndex != 0) {
                    charToGlyphMap_[charCode] = glyphIndex;
                }
            }
        }
    }
    
    return true;
}

bool TTFLoader::parseNameTable() {
    TableEntry* nameTable = findTable(0x6E616D65); // 'name'
    if (!nameTable) return false;
    
    const uint8_t* data = fontData_.data() + nameTable->offset;
    uint16_t format = readUInt16(data, 0);
    uint16_t count = readUInt16(data, 2);
    uint16_t stringOffset = readUInt16(data, 4);
    
    for (uint16_t i = 0; i < count; ++i) {
        size_t entryOffset = 6 + i * 12;
        uint16_t platformID = readUInt16(data, entryOffset);
        uint16_t encodingID = readUInt16(data, entryOffset + 2);
        uint16_t languageID = readUInt16(data, entryOffset + 4);
        uint16_t nameID = readUInt16(data, entryOffset + 6);
        uint16_t length = readUInt16(data, entryOffset + 8);
        uint16_t offset = readUInt16(data, entryOffset + 10);
        
        if (nameID == 1) { // Font family
            size_t stringStart = stringOffset + offset;
            fontFamily_.assign(reinterpret_cast<const char*>(data + stringStart), length);
        } else if (nameID == 4) { // Full font name
            size_t stringStart = stringOffset + offset;
            fontName_.assign(reinterpret_cast<const char*>(data + stringStart), length);
        }
    }
    
    return true;
}

bool TTFLoader::parseKernTable() {
    TableEntry* kernTable = findTable(0x6B65726E); // 'kern'
    if (!kernTable) return true; // Optional table
    
    const uint8_t* data = fontData_.data() + kernTable->offset;
    uint16_t version = readUInt16(data, 0);
    uint16_t nTables = readUInt16(data, 2);
    
    size_t offset = 4;
    for (uint16_t i = 0; i < nTables; ++i) {
        uint16_t subtableVersion = readUInt16(data, offset);
        uint16_t length = readUInt16(data, offset + 2);
        uint16_t coverage = readUInt16(data, offset + 4);
        
        bool horizontal = (coverage & 0x01) != 0;
        bool minimum = (coverage & 0x02) != 0;
        bool crossStream = (coverage & 0x04) != 0;
        bool override = (coverage & 0x08) != 0;
        
        if (horizontal && !minimum && !crossStream) {
            uint16_t nPairs = readUInt16(data, offset + 6);
            uint16_t entrySize = readUInt16(data, offset + 8);
            uint16_t searchRange = readUInt16(data, offset + 10);
            uint16_t entrySelector = readUInt16(data, offset + 12);
            uint16_t rangeShift = readUInt16(data, offset + 14);
            
            size_t pairOffset = offset + 16;
            for (uint16_t j = 0; j < nPairs; ++j) {
                uint16_t left = readUInt16(data, pairOffset);
                uint16_t right = readUInt16(data, pairOffset + 2);
                int16_t value = readInt16(data, pairOffset + 4);
                
                KerningPair pair;
                pair.leftGlyph = left;
                pair.rightGlyph = right;
                pair.kerningAmount = static_cast<float>(value) / unitsPerEm_;
                
                kerningPairs_.push_back(pair);
                pairOffset += entrySize;
            }
        }
        
        offset += length;
    }
    
    return true;
}

std::vector<Glyph> TTFLoader::extractGlyphs(const std::vector<uint32_t>& characterSet) {
    glyphs_.clear();
    glyphs_.reserve(characterSet.size());
    
    // Get table offsets
    TableEntry* glyfTable = findTable(0x676C7966); // 'glyf'
    TableEntry* locaTable = findTable(0x6C6F6361); // 'loca'
    TableEntry* hmtxTable = findTable(0x686D7478); // 'hmtx'
    
    if (!glyfTable || !locaTable || !hmtxTable) {
        return glyphs_;
    }
    
    glyfTableOffset_ = glyfTable->offset;
    locaTableOffset_ = locaTable->offset;
    hmtxTableOffset_ = hmtxTable->offset;
    
    for (uint32_t charCode : characterSet) {
        auto it = charToGlyphMap_.find(charCode);
        if (it != charToGlyphMap_.end()) {
            uint16_t glyphIndex = it->second;
            Glyph glyph = parseGlyph(glyphIndex);
            glyph.characterCode = charCode;
            glyph.index = glyphIndex;
            glyphs_.push_back(glyph);
        }
    }
    
    return glyphs_;
}

Glyph TTFLoader::parseGlyph(uint16_t glyphIndex) {
    Glyph glyph;
    
    // Get glyph offset from loca table
    const uint8_t* locaData = fontData_.data() + locaTableOffset_;
    size_t glyphOffset;
    
    if (isShortIndex_) {
        glyphOffset = readUInt16(locaData, glyphIndex * 2) * 2;
    } else {
        glyphOffset = readUInt32(locaData, glyphIndex * 4);
    }
    
    if (glyphOffset == 0) {
        // Empty glyph
        glyph.width = 0;
        glyph.height = 0;
        glyph.advanceX = 0;
        glyph.advanceY = 0;
        glyph.bearingX = 0;
        glyph.bearingY = 0;
        return glyph;
    }
    
    const uint8_t* glyfData = fontData_.data() + glyfTableOffset_ + glyphOffset;
    
    // Parse glyph header
    GlyphHeader header;
    header.numberOfContours = readInt16(glyfData, 0);
    header.xMin = readInt16(glyfData, 2);
    header.yMin = readInt16(glyfData, 4);
    header.xMax = readInt16(glyfData, 6);
    header.yMax = readInt16(glyfData, 8);
    
    glyph.boundingBox.x = header.xMin;
    glyph.boundingBox.y = header.yMin;
    glyph.boundingBox.width = header.xMax - header.xMin;
    glyph.boundingBox.height = header.yMax - header.yMin;
    
    if (header.numberOfContours >= 0) {
        parseSimpleGlyph(glyph, glyfData, 10);
    } else {
        parseCompositeGlyph(glyph, glyfData, 10);
    }
    
    // Get horizontal metrics from hmtx table
    const uint8_t* hmtxData = fontData_.data() + hmtxTableOffset_;
    if (glyphIndex < metrics_.numGlyphs) {
        glyph.advanceX = static_cast<float>(readUInt16(hmtxData, glyphIndex * 4)) / unitsPerEm_;
        glyph.bearingX = static_cast<float>(readInt16(hmtxData, glyphIndex * 4 + 2)) / unitsPerEm_;
    }
    
    glyph.advanceY = 0.0f;
    glyph.bearingY = 0.0f;
    
    return glyph;
}

void TTFLoader::parseSimpleGlyph(Glyph& glyph, const uint8_t* data, size_t offset) {
    int16_t numberOfContours = readInt16(data, offset - 10);
    
    if (numberOfContours == 0) {
        return;
    }
    
    // Read contour endpoints
    std::vector<uint16_t> endPtsOfContours(numberOfContours);
    for (int i = 0; i < numberOfContours; ++i) {
        endPtsOfContours[i] = readUInt16(data, offset);
        offset += 2;
    }
    
    // Skip instruction length and instructions
    uint16_t instructionLength = readUInt16(data, offset);
    offset += 2 + instructionLength;
    
    // Read flags and coordinates
    std::vector<uint8_t> flags;
    std::vector<int16_t> xCoordinates;
    std::vector<int16_t> yCoordinates;
    
    uint16_t pointCount = endPtsOfContours.back() + 1;
    
    // Read flags
    for (uint16_t i = 0; i < pointCount; ++i) {
        uint8_t flag = data[offset++];
        flags.push_back(flag);
        
        // Repeat flag if bit 3 is set
        if (flag & 0x08) {
            uint8_t repeatCount = data[offset++];
            for (uint8_t r = 0; r < repeatCount; ++r) {
                flags.push_back(flag);
                ++i;
            }
        }
    }
    
    // Read x coordinates
    int16_t lastX = 0;
    for (size_t i = 0; i < flags.size(); ++i) {
        if (flags[i] & 0x02) {
            // 8-bit delta
            int8_t deltaX = static_cast<int8_t>(data[offset++]);
            lastX += (flags[i] & 0x10) ? deltaX : -deltaX;
        } else if (!(flags[i] & 0x10)) {
            // 16-bit delta
            int16_t deltaX = readInt16(data, offset);
            offset += 2;
            lastX += deltaX;
        }
        xCoordinates.push_back(lastX);
    }
    
    // Read y coordinates
    int16_t lastY = 0;
    for (size_t i = 0; i < flags.size(); ++i) {
        if (flags[i] & 0x04) {
            // 8-bit delta
            int8_t deltaY = static_cast<int8_t>(data[offset++]);
            lastY += (flags[i] & 0x20) ? deltaY : -deltaY;
        } else if (!(flags[i] & 0x40)) {
            // 16-bit delta
            int16_t deltaY = readInt16(data, offset);
            offset += 2;
            lastY += deltaY;
        }
        yCoordinates.push_back(lastY);
    }
    
    // Store outline points
    glyph.outlinePoints.resize(xCoordinates.size());
    for (size_t i = 0; i < xCoordinates.size(); ++i) {
        glyph.outlinePoints[i].x = static_cast<float>(xCoordinates[i]) / unitsPerEm_;
        glyph.outlinePoints[i].y = static_cast<float>(yCoordinates[i]) / unitsPerEm_;
        glyph.outlinePoints[i].onCurve = (flags[i] & 0x01) != 0;
    }
}

void TTFLoader::parseCompositeGlyph(Glyph& glyph, const uint8_t* data, size_t offset) {
    // Composite glyph parsing - simplified version
    // In a full implementation, this would parse component glyphs and combine them
    glyph.outlinePoints.clear();
}

FontFace TTFLoader::getFontFace() const {
    FontFace face;
    face.metrics = metrics_;
    face.name = fontName_;
    face.family = fontFamily_;
    face.unitsPerEm = unitsPerEm_;
    face.glyphs = glyphs_;
    face.kerningPairs = kerningPairs_;
    return face;
}

FontMetrics TTFLoader::getFontMetrics() const {
    return metrics_;
}

std::string TTFLoader::getFontName() const {
    return fontName_;
}

std::string TTFLoader::getFontFamily() const {
    return fontFamily_;
}

uint32_t TTFLoader::getUnitsPerEm() const {
    return unitsPerEm_;
}

std::vector<KerningPair> TTFLoader::extractKerningPairs() {
    return kerningPairs_;
}

uint16_t TTFLoader::readUInt16(const uint8_t* data, size_t offset) {
    return (static_cast<uint16_t>(data[offset]) << 8) | 
           static_cast<uint16_t>(data[offset + 1]);
}

uint32_t TTFLoader::readUInt32(const uint8_t* data, size_t offset) {
    return (static_cast<uint32_t>(data[offset]) << 24) |
           (static_cast<uint32_t>(data[offset + 1]) << 16) |
           (static_cast<uint32_t>(data[offset + 2]) << 8) |
           static_cast<uint32_t>(data[offset + 3]);
}

int16_t TTFLoader::readInt16(const uint8_t* data, size_t offset) {
    return static_cast<int16_t>(readUInt16(data, offset));
}

int32_t TTFLoader::readInt32(const uint8_t* data, size_t offset) {
    return static_cast<int32_t>(readUInt32(data, offset));
}

uint32_t TTFLoader::calculateChecksum(const uint8_t* data, size_t length) {
    uint32_t checksum = 0;
    for (size_t i = 0; i < length; i += 4) {
        uint32_t value = 0;
        for (size_t j = 0; j < 4 && (i + j) < length; ++j) {
            value = (value << 8) | data[i + j];
        }
        checksum += value;
    }
    return checksum;
}

} // namespace FontBaker