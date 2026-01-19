#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFCore {

// Character map types
enum class CharacterMapType {
    UNICODE,
    ASCII,
    UTF8,
    UTF16,
    UTF32,
    CUSTOM
};

// Character encoding types
enum class CharacterEncoding {
    UTF8,
    UTF16,
    UTF32,
    ASCII,
    ISO_8859_1,
    WINDOWS_1252,
    CUSTOM
};

// Character map entry
struct CharacterMapEntry {
    uint32_t charCode;
    uint32_t glyphIndex;
    std::string glyphName;
    CharacterEncoding encoding;
    bool isValid;
    std::map<std::string, std::any> metadata;
    
    CharacterMapEntry() : charCode(0), glyphIndex(0), encoding(CharacterEncoding::UTF8), isValid(false) {}
    CharacterMapEntry(uint32_t code, uint32_t glyph, const std::string& name, CharacterEncoding enc, bool valid)
        : charCode(code), glyphIndex(glyph), glyphName(name), encoding(enc), isValid(valid) {}
};

// Character range
struct CharacterRange {
    uint32_t start;
    uint32_t end;
    std::string description;
    bool isContiguous;
    
    CharacterRange() : start(0), end(0), isContiguous(true) {}
    CharacterRange(uint32_t s, uint32_t e, const std::string& desc, bool contiguous)
        : start(s), end(e), description(desc), isContiguous(contiguous) {}
};

// Character map statistics
struct CharacterMapStatistics {
    uint32_t totalEntries;
    uint32_t validEntries;
    uint32_t invalidEntries;
    uint32_t unicodeEntries;
    uint32_t asciiEntries;
    uint32_t utf8Entries;
    uint32_t utf16Entries;
    uint32_t utf32Entries;
    uint32_t customEntries;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    float32 hitRatio;
    
    CharacterMapStatistics() : totalEntries(0), validEntries(0), invalidEntries(0), 
                             unicodeEntries(0), asciiEntries(0), utf8Entries(0), 
                             utf16Entries(0), utf32Entries(0), customEntries(0), 
                             cacheHits(0), cacheMisses(0), hitRatio(0.0f) {}
};

// Character map configuration
struct CharacterMapConfig {
    CharacterMapType type;
    CharacterEncoding defaultEncoding;
    bool enableCache;
    bool enableValidation;
    bool enableStatistics;
    bool enableAutoMapping;
    bool enableUnicodeNormalization;
    size_t maxCacheSize;
    uint32_t cleanupInterval;
    
    CharacterMapConfig() : type(CharacterMapType::UNICODE), defaultEncoding(CharacterEncoding::UTF8), 
                           enableCache(true), enableValidation(true), enableStatistics(true), 
                           enableAutoMapping(true), enableUnicodeNormalization(true), 
                           maxCacheSize(10000), cleanupInterval(60) {}
};

// Character map
class CharacterMap {
public:
    CharacterMap();
    virtual ~CharacterMap() = default;
    
    // Character map management
    void initialize(const CharacterMapConfig& config);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Configuration
    void setConfiguration(const CharacterMapConfig& config);
    const CharacterMapConfig& getConfiguration() const;
    void setDefaultEncoding(CharacterEncoding encoding);
    void setCharacterMapType(CharacterMapType type);
    
    // Character mapping
    bool addMapping(uint32_t charCode, uint32_t glyphIndex, const std::string& glyphName = "", 
                    CharacterEncoding encoding = CharacterEncoding::UTF8);
    bool addMapping(const CharacterMapEntry& entry);
    bool removeMapping(uint32_t charCode, CharacterEncoding encoding = CharacterEncoding::UTF8);
    bool hasMapping(uint32_t charCode, CharacterEncoding encoding = CharacterEncoding::UTF8) const;
    bool getMapping(uint32_t charCode, uint32_t& glyphIndex, CharacterEncoding encoding = CharacterEncoding::UTF8) const;
    bool getMapping(uint32_t charCode, CharacterMapEntry& entry, CharacterEncoding encoding = CharacterEncoding::UTF8) const;
    
    // Batch mapping operations
    bool addMappings(const std::vector<CharacterMapEntry>& entries);
    bool addMappings(const std::map<uint32_t, CharacterMapEntry>& entries);
    void removeMappings(const std::vector<uint32_t>& charCodes);
    void removeMappings(const std::vector<uint32_t>& charCodes, CharacterEncoding encoding);
    
    // Character range operations
    bool addRange(const CharacterRange& range, uint32_t glyphIndexOffset = 0);
    bool addRange(uint32_t start, uint32_t end, uint32_t glyphIndexOffset = 0, const std::string& description = "");
    bool removeRange(uint32_t start, uint32_t end);
    std::vector<CharacterRange> getRanges() const;
    bool hasRange(uint32_t charCode) const;
    
    // Unicode support
    bool addUnicodeMapping(uint32_t charCode, uint32_t glyphIndex, const std::string& glyphName = "");
    bool hasUnicodeMapping(uint32_t charCode) const;
    bool getUnicodeMapping(uint32_t charCode, uint32_t& glyphIndex) const;
    bool getUnicodeMapping(uint32_t charCode, CharacterMapEntry& entry) const;
    
    // ASCII support
    bool addASCIIMapping(uint8_t charCode, uint32_t glyphIndex, const std::string& glyphName = "");
    bool hasASCIIMapping(uint8_t charCode) const;
    bool getASCIIMapping(uint8_t charCode, uint32_t& glyphIndex) const;
    bool getASCIIMapping(uint8_t charCode, CharacterMapEntry& entry) const;
    
    // UTF-8 support
    bool addUTF8Mapping(const std::string& utf8Char, uint32_t glyphIndex, const std::string& glyphName = "");
    bool hasUTF8Mapping(const std::string& utf8Char) const;
    bool getUTF8Mapping(const std::string& utf8Char, uint32_t& glyphIndex) const;
    bool getUTF8Mapping(const std::string& utf8Char, CharacterMapEntry& entry) const;
    
    // UTF-16 support
    bool addUTF16Mapping(uint16_t charCode, uint32_t glyphIndex, const std::string& glyphName = "");
    bool hasUTF16Mapping(uint16_t charCode) const;
    bool getUTF16Mapping(uint16_t charCode, uint32_t& glyphIndex) const;
    bool getUTF16Mapping(uint16_t charCode, CharacterMapEntry& entry) const;
    
    // UTF-32 support
    bool addUTF32Mapping(uint32_t charCode, uint32_t glyphIndex, const std::string& glyphName = "");
    bool hasUTF32Mapping(uint32_t charCode) const;
    bool getUTF32Mapping(uint32_t charCode, uint32_t glyphIndex) const;
    bool getUTF32Mapping(uint32_t charCode, CharacterMapEntry& entry) const;
    
    // String mapping
    std::vector<uint32_t> mapString(const std::string& text, CharacterEncoding encoding = CharacterEncoding::UTF8) const;
    std::vector<uint32_t> mapString(const std::wstring& text, CharacterEncoding encoding = CharacterEncoding::UTF16) const;
    std::vector<uint32_t> mapString(const std::u32string& text, CharacterEncoding encoding = CharacterEncoding::UTF32) const;
    std::vector<CharacterMapEntry> mapStringToEntries(const std::string& text, CharacterEncoding encoding = CharacterEncoding::UTF8) const;
    
    // Character lookup
    uint32_t findGlyphIndex(uint32_t charCode, CharacterEncoding encoding = CharacterEncoding::UTF8) const;
    std::string findGlyphName(uint32_t charCode, CharacterEncoding encoding = CharacterEncoding::UTF8) const;
    CharacterEncoding detectEncoding(const std::string& text) const;
    CharacterEncoding detectEncoding(const std::wstring& text) const;
    CharacterEncoding detectEncoding(const std::u32string& text) const;
    
    // Character validation
    bool isValidCharacter(uint32_t charCode) const;
    bool isValidCharacter(uint32_t charCode, CharacterEncoding encoding) const;
    bool isValidString(const std::string& text, CharacterEncoding encoding = CharacterEncoding::UTF8) const;
    std::vector<uint32_t> getInvalidCharacters(const std::string& text, CharacterEncoding encoding = CharacterEncoding::UTF8) const;
    
    // Character conversion
    uint32_t convertCharacter(uint32_t charCode, CharacterEncoding fromEncoding, CharacterEncoding toEncoding) const;
    std::string convertString(const std::string& text, CharacterEncoding fromEncoding, CharacterEncoding toEncoding) const;
    std::wstring convertToWideString(const std::string& text, CharacterEncoding encoding = CharacterEncoding::UTF8) const;
    std::u32string convertToUTF32String(const std::string& text, CharacterEncoding encoding = CharacterEncoding::UTF8) const;
    
    // Unicode normalization
    std::string normalizeUnicode(const std::string& text) const;
    std::wstring normalizeUnicode(const std::wstring& text) const;
    std::u32string normalizeUnicode(const std::u32string& text) const;
    uint32_t normalizeUnicodeCharacter(uint32_t charCode) const;
    
    // Character map validation
    bool validateMap() const;
    bool validateEntry(const CharacterMapEntry& entry) const;
    std::vector<CharacterMapEntry> getInvalidEntries() const;
    void removeInvalidEntries();
    
    // Character map cache
    void enableCache(bool enabled);
    bool isCacheEnabled() const;
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    void clearCache();
    
    // Character map statistics
    CharacterMapStatistics getStatistics() const;
    void updateStatistics();
    void resetStatistics();
    void enableStatistics(bool enabled);
    bool isStatisticsEnabled() const;
    
    // Character map maintenance
    void cleanup();
    void optimize();
    void compact();
    void rebuild();
    
    // Character map debugging
    void enableDebugging(bool enabled);
    bool isDebuggingEnabled() const;
    std::vector<std::string> getDebugMessages() const;
    void clearDebugMessages();
    void dumpMap() const;
    std::string dumpMapToString() const;
    
    // Event handling
    void addCharacterMapEventListener(const std::string& eventType, std::function<void()> callback);
    void removeCharacterMapEventListener(const std::string& eventType, std::function<void()> callback);
    void clearCharacterMapEventListeners();
    
    // Utility methods
    void cloneFrom(const CharacterMap& other);
    virtual std::unique_ptr<CharacterMap> clone() const;
    bool equals(const CharacterMap& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::pair<uint32_t, CharacterEncoding>, CharacterMapEntry> characterMap_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    std::vector<CharacterRange> ranges_;
    CharacterMapConfig config_;
    CharacterMapStatistics statistics_;
    
    bool isInitialized_;
    bool statisticsEnabled_;
    bool debuggingEnabled_;
    bool cacheEnabled_;
    
    size_t currentCacheSize_;
    std::vector<std::string> debugMessages_;
    
    // Protected helper methods
    virtual void triggerCharacterMapEvent(const std::string& eventType);
    virtual std::string generateCacheKey(uint32_t charCode, CharacterEncoding encoding) const;
    virtual bool shouldCache() const;
    virtual void updateCache();
    virtual void cleanupCache();
    
    // Character conversion helpers
    virtual uint32_t convertToUnicode(uint32_t charCode, CharacterEncoding encoding) const;
    virtual uint32_t convertFromUnicode(uint32_t charCode, CharacterEncoding encoding) const;
    virtual std::vector<uint32_t> parseUTF8String(const std::string& text) const;
    virtual std::vector<uint32_t> parseUTF16String(const std::wstring& text) const;
    virtual std::vector<uint32_t> parseUTF32String(const std::u32string& text) const;
    
    // Validation helpers
    virtual bool validateCharacterCode(uint32_t charCode) const;
    virtual bool validateCharacterCode(uint32_t charCode, CharacterEncoding encoding) const;
    virtual bool validateCharacterEntry(const CharacterMapEntry& entry) const;
    virtual bool isContiguousRange(uint32_t start, uint32_t end) const;
    
    // Unicode helpers
    virtual uint32_t normalizeUnicodeCharacterInternal(uint32_t charCode) const;
    virtual bool isUnicodeCharacter(uint32_t charCode) const;
    virtual bool isASCIICharacter(uint32_t charCode) const;
    virtual bool isUTF8Character(uint32_t charCode) const;
    virtual bool isUTF16Character(uint32_t charCode) const;
    virtual bool isUTF32Character(uint32_t charCode) const;
    
    // Statistics helpers
    virtual void updateMappingStatistics(bool hit);
    virtual void updateCacheStatistics(bool hit);
    virtual void calculateStatistics();
    
    // Debug helpers
    virtual void addDebugMessage(const std::string& message);
    virtual void logMapOperation(const std::string& operation, uint32_t charCode, CharacterEncoding encoding);
    virtual std::string formatMapEntry(const CharacterMapEntry& entry) const;
};

// Specialized character maps
class UnicodeCharacterMap : public CharacterMap {
public:
    UnicodeCharacterMap();
    bool addUnicodeMapping(uint32_t charCode, uint32_t glyphIndex, const std::string& glyphName = "") override;
    bool hasUnicodeMapping(uint32_t charCode) const override;
    bool getUnicodeMapping(uint32_t charCode, uint32_t glyphIndex) const override;
    std::unique_ptr<CharacterMap> clone() const override;
    
protected:
    bool validateCharacterCode(uint32_t charCode, CharacterEncoding encoding) const override;
    bool isUnicodeCharacter(uint32_t charCode) const override;
};

class ASCIICCharacterMap : public CharacterMap {
public:
    ASCIICCharacterMap();
    bool addASCIIMapping(uint8_t charCode, uint32_t glyphIndex, const std::string& glyphName = "") override;
    bool hasASCIIMapping(uint8_t charCode) const override;
    bool getASCIIMapping(uint8_t charCode, uint32_t glyphIndex) const override;
    std::unique_ptr<CharacterMap> clone() const override;
    
protected:
    bool validateCharacterCode(uint32_t charCode, CharacterEncoding encoding) const override;
    bool isASCIICharacter(uint32_t charCode) const override;
};

class UTF8CharacterMap : public CharacterMap {
public:
    UTF8CharacterMap();
    bool addUTF8Mapping(const std::string& utf8Char, uint32_t glyphIndex, const std::string& glyphName = "") override;
    bool hasUTF8Mapping(const std::string& utf8Char) const override;
    bool getUTF8Mapping(const std::string& utf8Char, uint32_t glyphIndex) const override;
    std::vector<uint32_t> mapString(const std::string& text, CharacterEncoding encoding = CharacterEncoding::UTF8) const override;
    std::unique_ptr<CharacterMap> clone() const override;
    
protected:
    std::vector<uint32_t> parseUTF8String(const std::string& text) const override;
    bool isUTF8Character(uint32_t charCode) const override;
};

class UTF16CharacterMap : public CharacterMap {
public:
    UTF16CharacterMap();
    bool addUTF16Mapping(uint16_t charCode, uint32_t glyphIndex, const std::string& glyphName = "") override;
    bool hasUTF16Mapping(uint16_t charCode) const override;
    bool getUTF16Mapping(uint16_t charCode, uint32_t glyphIndex) const override;
    std::vector<uint32_t> mapString(const std::wstring& text, CharacterEncoding encoding = CharacterEncoding::UTF16) const override;
    std::unique_ptr<CharacterMap> clone() const override;
    
protected:
    std::vector<uint32_t> parseUTF16String(const std::wstring& text) const override;
    bool isUTF16Character(uint32_t charCode) const override;
};

class UTF32CharacterMap : public CharacterMap {
public:
    UTF32CharacterMap();
    bool addUTF32Mapping(uint32_t charCode, uint32_t glyphIndex, const std::string& glyphName = "") override;
    bool hasUTF32Mapping(uint32_t charCode) const override;
    bool getUTF32Mapping(uint32_t charCode, uint32_t glyphIndex) const override;
    std::vector<uint32_t> mapString(const std::u32string& text, CharacterEncoding encoding = CharacterEncoding::UTF32) const override;
    std::unique_ptr<CharacterMap> clone() const override;
    
protected:
    std::vector<uint32_t> parseUTF32String(const std::u32string& text) const override;
    bool isUTF32Character(uint32_t charCode) const override;
};

// Character map factory
class CharacterMapFactory {
public:
    static std::unique_ptr<CharacterMap> createCharacterMap(CharacterMapType type);
    static std::unique_ptr<UnicodeCharacterMap> createUnicodeCharacterMap();
    static std::unique_ptr<ASCIICCharacterMap> createASCIICCharacterMap();
    static std::unique_ptr<UTF8CharacterMap> createUTF8CharacterMap();
    static std::unique_ptr<UTF16CharacterMap> createUTF16CharacterMap();
    static std::unique_ptr<UTF32CharacterMap> createUTF32CharacterMap();
    static CharacterMapConfig createDefaultConfig(CharacterMapType type);
    static std::vector<CharacterMapType> getSupportedMapTypes();
    static std::vector<CharacterEncoding> getSupportedEncodings();
    static CharacterEncoding detectBestEncoding(const std::string& text);
    static CharacterEncoding detectBestEncoding(const std::wstring& text);
    static CharacterEncoding detectBestEncoding(const std::u32string& text);
};

} // namespace RFCore