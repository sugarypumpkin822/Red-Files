#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFCore {

// Unicode categories
enum class UnicodeCategory {
    UPPERCASE_LETTER,
    LOWERCASE_LETTER,
    TITLECASE_LETTER,
    MODIFIER_LETTER,
    OTHER_LETTER,
    NON_SPACING_MARK,
    SPACING_COMBINING_MARK,
    ENCLOSING_MARK,
    DECIMAL_DIGIT_NUMBER,
    LETTER_NUMBER,
    OTHER_NUMBER,
    SPACE_SEPARATOR,
    LINE_SEPARATOR,
    PARAGRAPH_SEPARATOR,
    CONTROL,
    FORMAT,
    PRIVATE_USE,
    SURROGATE,
    DASH_PUNCTUATION,
    OPEN_PUNCTUATION,
    CLOSE_PUNCTUATION,
    INITIAL_PUNCTUATION,
    FINAL_PUNCTUATION,
    OTHER_PUNCTUATION,
    MATH_SYMBOL,
    CURRENCY_SYMBOL,
    MODIFIER_SYMBOL,
    OTHER_SYMBOL,
    INITIAL_QUOTE_PUNCTUATION,
    FINAL_QUOTE_PUNCTUATION,
    CONNECTOR_PUNCTUATION,
    DASH,
    START_PUNCTUATION,
    END_PUNCTUATION,
    OTHER,
    CUSTOM
};

// Unicode scripts
enum class UnicodeScript {
    LATIN,
    GREEK,
    CYRILLIC,
    ARMENIAN,
    HEBREW,
    ARABIC,
    SYRIAC,
    THAANA,
    DEVANAGARI,
    BENGALI,
    GURMUKHI,
    GUJARATI,
    ORIYA,
    TAMIL,
    TELUGU,
    KANNADA,
    MALAYALAM,
    SINHALA,
    THAI,
    LAO,
    TIBETAN,
    MYANMAR,
    GEORGIAN,
    HANGUL,
    ETHIOPIC,
    CHEROKEE,
    CANADIAN_ABORIGINAL,
    OGHAM,
    RUNIC,
    KHMER,
    MONGOLIAN,
    HIRAGANA,
    KATAKANA,
    BOPOMOFO,
    HAN,
    YI,
    OLD_ITALIC,
    GOTHIC,
    DESERET,
    INHERITED,
    TAGALOG,
    HANUNOO,
    BUHID,
    TAGBANWA,
    LIMBU,
    TAI_LE,
    LINEAR_B,
    UGARITIC,
    SHAVIAN,
    OSMANYA,
    CYPRIOT,
    BUGINESE,
    COPTIC,
    GLAGOLITIC,
    TIFINAGH,
    SYRIAC,
    NKO,
    PHAGS_PA,
    KAYAH_LI,
    LEPACHA,
    REJANG,
    SUNDANESE,
    Saurashtra,
    CHAM,
    OL_CHIKI,
    VAI,
    CARIAN,
    LYCIAN,
    LYDIAN,
    TAI_THAM,
    TAI_VIET,
    AVESTAN,
    EGYPTIAN_HIEROGLYPHS,
    SAMARITAN,
    MANDAIC,
    LISU,
    BAMUM,
    TAKRI,
    BATAK,
    BRAHMI,
    MEITEI_MAYEK,
    SORA_SOMPENG,
    CHAKMA,
    SHARADA,
    TAI_ALE,
    TAI_HAM,
    MEROITIC_CURSIVE,
    MEROITIC_HIEROGLYPHS,
    MIAO,
    KANA_SUPPLEMENT,
    BASSA_VAH,
    PAHAWH_HMONG,
    DUPLOYAN_SHORTHAND,
    MENDE_KIKAKUI,
    MRO,
    NUSHU,
    SYMBOLS,
    CUSTOM
};

// Unicode bidirectional types
enum class UnicodeBidiType {
    LEFT_TO_RIGHT,
    RIGHT_TO_LEFT,
    ARABIC_LETTER,
    EUROPEAN_NUMBER,
    EUROPEAN_SEPARATOR,
    EUROPEAN_TERMINATOR,
    ARABIC_NUMBER,
    COMMON_SEPARATOR,
    NONSPACING_MARK,
    BOUNDARY_NEUTRAL,
    PARAGRAPH_SEPARATOR,
    SEGMENT_SEPARATOR,
    WHITESPACE,
    OTHER_NEUTRAL,
    LEFT_TO_RIGHT_EMBEDDING,
    LEFT_TO_RIGHT_OVERRIDE,
    RIGHT_TO_LEFT_ARABIC,
    RIGHT_TO_LEFT_EMBEDDING,
    RIGHT_TO_LEFT_OVERRIDE,
    POP_DIRECTIONAL_FORMAT,
    CUSTOM
};

// Unicode normalization forms
enum class UnicodeNormalizationForm {
    NONE,
    NFC,
    NFD,
    NFKC,
    NFKD,
    CUSTOM
};

// Unicode character properties
struct UnicodeProperties {
    uint32_t codePoint;
    UnicodeCategory category;
    UnicodeScript script;
    UnicodeBidiType bidiType;
    bool isLetter;
    bool isDigit;
    bool isPunctuation;
    bool isSymbol;
    bool isSpace;
    bool isControl;
    bool isFormat;
    bool isSurrogate;
    bool isPrivateUse;
    bool isNoncharacter;
    bool isIgnorable;
    bool isDeprecated;
    bool isMirrored;
    bool isCombining;
    bool isEmoji;
    bool isZeroWidth;
    bool isDoubleWidth;
    bool isAmbiguousWidth;
    uint32_t combiningClass;
    uint32_t numericValue;
    std::string name;
    std::string block;
    
    UnicodeProperties() : codePoint(0), category(UnicodeCategory::OTHER), script(UnicodeScript::LATIN), 
                         bidiType(UnicodeBidiType::LEFT_TO_RIGHT), isLetter(false), isDigit(false), 
                         isPunctuation(false), isSymbol(false), isSpace(false), isControl(false), 
                         isFormat(false), isSurrogate(false), isPrivateUse(false), isNoncharacter(false), 
                         isIgnorable(false), isDeprecated(false), isMirrored(false), isCombining(false), 
                         isEmoji(false), isZeroWidth(false), isDoubleWidth(false), isAmbiguousWidth(false), 
                         combiningClass(0), numericValue(0) {}
};

// Unicode range
struct UnicodeRange {
    uint32_t start;
    uint32_t end;
    UnicodeScript script;
    std::string name;
    std::string description;
    
    UnicodeRange() : start(0), end(0), script(UnicodeScript::LATIN) {}
    UnicodeRange(uint32_t s, uint32_t e, UnicodeScript scr, const std::string& n, const std::string& desc)
        : start(s), end(e), script(scr), name(n), description(desc) {}
};

// Unicode block
struct UnicodeBlock {
    uint32_t start;
    uint32_t end;
    std::string name;
    std::string description;
    UnicodeScript script;
    
    UnicodeBlock() : start(0), end(0), script(UnicodeScript::LATIN) {}
    UnicodeBlock(uint32_t s, uint32_t e, const std::string& n, const std::string& desc, UnicodeScript scr)
        : start(s), end(e), name(n), description(desc), script(scr) {}
};

// Unicode handler configuration
struct UnicodeHandlerConfig {
    bool enableNormalization;
    bool enableBidirectional;
    bool enableScriptDetection;
    bool enableCategoryDetection;
    bool enableEmojiSupport;
    bool enableWidthDetection;
    bool enableCache;
    bool enableStatistics;
    bool enableDebugging;
    UnicodeNormalizationForm defaultNormalization;
    size_t maxCacheSize;
    uint32_t cleanupInterval;
    
    UnicodeHandlerConfig() : enableNormalization(true), enableBidirectional(true), 
                            enableScriptDetection(true), enableCategoryDetection(true), 
                            enableEmojiSupport(true), enableWidthDetection(true), 
                            enableCache(true), enableStatistics(true), enableDebugging(false), 
                            defaultNormalization(UnicodeNormalizationForm::NFC), maxCacheSize(10000), 
                            cleanupInterval(60) {}
};

// Unicode handler statistics
struct UnicodeHandlerStatistics {
    uint32_t totalQueries;
    uint32_t cacheHits;
    uint32_t cacheMisses;
    float32 cacheHitRatio;
    uint32_t normalizationCount;
    uint32_t bidiProcessingCount;
    uint32_t scriptDetectionCount;
    uint32_t categoryDetectionCount;
    uint32_t emojiDetectionCount;
    uint32_t widthDetectionCount;
    uint64_t totalProcessingTime;
    float32 averageProcessingTime;
    
    UnicodeHandlerStatistics() : totalQueries(0), cacheHits(0), cacheMisses(0), cacheHitRatio(0.0f), 
                               normalizationCount(0), bidiProcessingCount(0), scriptDetectionCount(0), 
                               categoryDetectionCount(0), emojiDetectionCount(0), widthDetectionCount(0), 
                               totalProcessingTime(0), averageProcessingTime(0.0f) {}
};

// Unicode handler
class UnicodeHandler {
public:
    UnicodeHandler();
    virtual ~UnicodeHandler() = default;
    
    // Unicode handler management
    void initialize(const UnicodeHandlerConfig& config);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Configuration
    void setConfiguration(const UnicodeHandlerConfig& config);
    const UnicodeHandlerConfig& getConfiguration() const;
    void setDefaultNormalization(UnicodeNormalizationForm form);
    void enableNormalization(bool enabled);
    void enableBidirectional(bool enabled);
    void enableScriptDetection(bool enabled);
    void enableCategoryDetection(bool enabled);
    void enableEmojiSupport(bool enabled);
    void enableWidthDetection(bool enabled);
    
    // Character properties
    UnicodeProperties getCharacterProperties(uint32_t codePoint) const;
    UnicodeCategory getCategory(uint32_t codePoint) const;
    UnicodeScript getScript(uint32_t codePoint) const;
    UnicodeBidiType getBidiType(uint32_t codePoint) const;
    std::string getCharacterName(uint32_t codePoint) const;
    std::string getBlockName(uint32_t codePoint) const;
    
    // Character validation
    bool isValidCodePoint(uint32_t codePoint) const;
    bool isLetter(uint32_t codePoint) const;
    bool isDigit(uint32_t codePoint) const;
    bool isPunctuation(uint32_t codePoint) const;
    bool isSymbol(uint32_t codePoint) const;
    bool isSpace(uint32_t codePoint) const;
    bool isControl(uint32_t codePoint) const;
    bool isFormat(uint32_t codePoint) const;
    bool isSurrogate(uint32_t codePoint) const;
    bool isPrivateUse(uint32_t codePoint) const;
    bool isNoncharacter(uint32_t codePoint) const;
    bool isIgnorable(uint32_t codePoint) const;
    bool isDeprecated(uint32_t codePoint) const;
    bool isMirrored(uint32_t codePoint) const;
    bool isCombining(uint32_t codePoint) const;
    bool isEmoji(uint32_t codePoint) const;
    bool isZeroWidth(uint32_t codePoint) const;
    bool isDoubleWidth(uint32_t codePoint) const;
    bool isAmbiguousWidth(uint32_t codePoint) const;
    
    // Numeric properties
    uint32_t getNumericValue(uint32_t codePoint) const;
    bool isNumeric(uint32_t codePoint) const;
    bool isDecimalDigit(uint32_t codePoint) const;
    bool isLetterNumber(uint32_t codePoint) const;
    bool isOtherNumber(uint32_t codePoint) const;
    
    // Combining properties
    uint32_t getCombiningClass(uint32_t codePoint) const;
    bool isNonspacingMark(uint32_t codePoint) const;
    bool isSpacingCombiningMark(uint32_t codePoint) const;
    bool isEnclosingMark(uint32_t codePoint) const;
    
    // Block and range operations
    UnicodeBlock getBlock(uint32_t codePoint) const;
    std::vector<UnicodeBlock> getAllBlocks() const;
    std::vector<UnicodeRange> getRanges(UnicodeScript script) const;
    bool isInBlock(uint32_t codePoint, const UnicodeBlock& block) const;
    bool isInRange(uint32_t codePoint, const UnicodeRange& range) const;
    
    // String operations
    std::vector<uint32_t> utf8ToCodePoints(const std::string& text) const;
    std::vector<uint32_t> utf16ToCodePoints(const std::wstring& text) const;
    std::vector<uint32_t> utf32ToCodePoints(const std::u32string& text) const;
    std::string codePointsToUtf8(const std::vector<uint32_t>& codePoints) const;
    std::wstring codePointsToUtf16(const std::vector<uint32_t>& codePoints) const;
    std::u32string codePointsToUtf32(const std::vector<uint32_t>& codePoints) const;
    
    // Normalization
    std::vector<uint32_t> normalize(const std::vector<uint32_t>& codePoints, UnicodeNormalizationForm form = UnicodeNormalizationForm::NFC) const;
    std::string normalize(const std::string& text, UnicodeNormalizationForm form = UnicodeNormalizationForm::NFC) const;
    std::wstring normalize(const std::wstring& text, UnicodeNormalizationForm form = UnicodeNormalizationForm::NFC) const;
    std::u32string normalize(const std::u32string& text, UnicodeNormalizationForm form = UnicodeNormalizationForm::NFC) const;
    bool isNormalized(const std::vector<uint32_t>& codePoints, UnicodeNormalizationForm form = UnicodeNormalizationForm::NFC) const;
    bool isNormalized(const std::string& text, UnicodeNormalizationForm form = UnicodeNormalizationForm::NFC) const;
    
    // Bidirectional processing
    std::vector<UnicodeBidiType> getBidiTypes(const std::vector<uint32_t>& codePoints) const;
    std::vector<UnicodeBidiType> getBidiTypes(const std::string& text) const;
    std::vector<UnicodeBidiType> getBidiTypes(const std::wstring& text) const;
    std::vector<UnicodeBidiType> getBidiTypes(const std::u32string& text) const;
    UnicodeBidiType getParagraphDirection(const std::vector<uint32_t>& codePoints) const;
    UnicodeBidiType getParagraphDirection(const std::string& text) const;
    std::vector<uint32_t> reorderBidi(const std::vector<uint32_t>& codePoints) const;
    std::string reorderBidi(const std::string& text) const;
    
    // Script detection
    UnicodeScript detectScript(const std::vector<uint32_t>& codePoints) const;
    UnicodeScript detectScript(const std::string& text) const;
    UnicodeScript detectScript(const std::wstring& text) const;
    UnicodeScript detectScript(const std::u32string& text) const;
    std::vector<UnicodeScript> detectScripts(const std::vector<uint32_t>& codePoints) const;
    std::vector<UnicodeScript> detectScripts(const std::string& text) const;
    std::map<UnicodeScript, uint32_t> getScriptDistribution(const std::vector<uint32_t>& codePoints) const;
    
    // Category detection
    std::vector<UnicodeCategory> getCategories(const std::vector<uint32_t>& codePoints) const;
    std::vector<UnicodeCategory> getCategories(const std::string& text) const;
    std::map<UnicodeCategory, uint32_t> getCategoryDistribution(const std::vector<uint32_t>& codePoints) const;
    std::vector<uint32_t> filterByCategory(const std::vector<uint32_t>& codePoints, UnicodeCategory category) const;
    std::vector<uint32_t> filterByCategories(const std::vector<uint32_t>& codePoints, const std::vector<UnicodeCategory>& categories) const;
    
    // Width detection
    uint32_t getStringWidth(const std::vector<uint32_t>& codePoints) const;
    uint32_t getStringWidth(const std::string& text) const;
    uint32_t getStringWidth(const std::wstring& text) const;
    uint32_t getStringWidth(const std::u32string& text) const;
    std::vector<uint32_t> getCharacterWidths(const std::vector<uint32_t>& codePoints) const;
    std::vector<uint32_t> getCharacterWidths(const std::string& text) const;
    
    // Emoji detection
    bool isEmojiSequence(const std::vector<uint32_t>& codePoints) const;
    bool isEmojiSequence(const std::string& text) const;
    std::vector<std::vector<uint32_t>> findEmojiSequences(const std::vector<uint32_t>& codePoints) const;
    std::vector<std::string> findEmojiSequences(const std::string& text) const;
    std::vector<uint32_t> filterEmoji(const std::vector<uint32_t>& codePoints) const;
    std::vector<uint32_t> filterNonEmoji(const std::vector<uint32_t>& codePoints) const;
    
    // Caching
    void enableCache(bool enabled);
    bool isCacheEnabled() const;
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    void clearCache();
    
    // Statistics
    UnicodeHandlerStatistics getStatistics() const;
    void updateStatistics();
    void resetStatistics();
    void enableStatistics(bool enabled);
    bool isStatisticsEnabled() const;
    
    // Debugging
    void enableDebugging(bool enabled);
    bool isDebuggingEnabled() const;
    std::vector<std::string> getDebugMessages() const;
    void clearDebugMessages();
    void dumpHandler() const;
    std::string dumpHandlerToString() const;
    
    // Event handling
    void addUnicodeHandlerEventListener(const std::string& eventType, std::function<void()> callback);
    void removeUnicodeHandlerEventListener(const std::string& eventType, std::function<void()> callback);
    void clearUnicodeHandlerEventListeners();
    
    // Utility methods
    void cloneFrom(const UnicodeHandler& other);
    virtual std::unique_ptr<UnicodeHandler> clone() const;
    bool equals(const UnicodeHandler& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    UnicodeHandlerConfig config_;
    UnicodeHandlerStatistics statistics_;
    
    bool isInitialized_;
    bool statisticsEnabled_;
    bool debuggingEnabled_;
    bool cacheEnabled_;
    bool normalizationEnabled_;
    bool bidiEnabled_;
    bool scriptDetectionEnabled_;
    bool categoryDetectionEnabled_;
    bool emojiSupportEnabled_;
    bool widthDetectionEnabled_;
    
    size_t currentCacheSize_;
    std::vector<std::string> debugMessages_;
    
    // Unicode data
    std::vector<UnicodeBlock> blocks_;
    std::vector<UnicodeRange> ranges_;
    std::map<uint32_t, UnicodeProperties> characterProperties_;
    
    // Protected helper methods
    virtual void triggerUnicodeHandlerEvent(const std::string& eventType);
    virtual UnicodeProperties getCharacterPropertiesInternal(uint32_t codePoint) const;
    virtual UnicodeCategory getCategoryInternal(uint32_t codePoint) const;
    virtual UnicodeScript getScriptInternal(uint32_t codePoint) const;
    virtual UnicodeBidiType getBidiTypeInternal(uint32_t codePoint) const;
    
    // Validation helpers
    virtual bool isValidCodePointInternal(uint32_t codePoint) const;
    virtual bool isSurrogatePair(uint32_t high, uint32_t low) const;
    virtual bool isHighSurrogate(uint32_t codePoint) const;
    virtual bool isLowSurrogate(uint32_t codePoint) const;
    
    // UTF conversion helpers
    virtual std::vector<uint32_t> utf8ToCodePointsInternal(const std::string& text) const;
    virtual std::vector<uint32_t> utf16ToCodePointsInternal(const std::wstring& text) const;
    virtual std::vector<uint32_t> utf32ToCodePointsInternal(const std::u32string& text) const;
    virtual std::string codePointsToUtf8Internal(const std::vector<uint32_t>& codePoints) const;
    virtual std::wstring codePointsToUtf16Internal(const std::vector<uint32_t>& codePoints) const;
    virtual std::u32string codePointsToUtf32Internal(const std::vector<uint32_t>& codePoints) const;
    
    // Normalization helpers
    virtual std::vector<uint32_t> normalizeInternal(const std::vector<uint32_t>& codePoints, UnicodeNormalizationForm form) const;
    virtual bool isNormalizedInternal(const std::vector<uint32_t>& codePoints, UnicodeNormalizationForm form) const;
    virtual std::vector<uint32_t> compose(const std::vector<uint32_t>& codePoints) const;
    virtual std::vector<uint32_t> decompose(const std::vector<uint32_t>& codePoints, bool compatibility) const;
    
    // Bidirectional helpers
    virtual std::vector<UnicodeBidiType> getBidiTypesInternal(const std::vector<uint32_t>& codePoints) const;
    virtual UnicodeBidiType getParagraphDirectionInternal(const std::vector<uint32_t>& codePoints) const;
    virtual std::vector<uint32_t> reorderBidiInternal(const std::vector<uint32_t>& codePoints) const;
    
    // Script detection helpers
    virtual UnicodeScript detectScriptInternal(const std::vector<uint32_t>& codePoints) const;
    virtual std::vector<UnicodeScript> detectScriptsInternal(const std::vector<uint32_t>& codePoints) const;
    virtual std::map<UnicodeScript, uint32_t> getScriptDistributionInternal(const std::vector<uint32_t>& codePoints) const;
    
    // Category detection helpers
    virtual std::vector<UnicodeCategory> getCategoriesInternal(const std::vector<uint32_t>& codePoints) const;
    virtual std::map<UnicodeCategory, uint32_t> getCategoryDistributionInternal(const std::vector<uint32_t>& codePoints) const;
    
    // Width detection helpers
    virtual uint32_t getStringWidthInternal(const std::vector<uint32_t>& codePoints) const;
    virtual std::vector<uint32_t> getCharacterWidthsInternal(const std::vector<uint32_t>& codePoints) const;
    
    // Emoji detection helpers
    virtual bool isEmojiSequenceInternal(const std::vector<uint32_t>& codePoints) const;
    virtual std::vector<std::vector<uint32_t>> findEmojiSequencesInternal(const std::vector<uint32_t>& codePoints) const;
    
    // Cache helpers
    virtual std::string generateCacheKey(uint32_t codePoint) const;
    virtual bool getFromCache(uint32_t codePoint, UnicodeProperties& properties) const;
    virtual void addToCache(uint32_t codePoint, const UnicodeProperties& properties);
    virtual void removeFromCache(uint32_t codePoint);
    virtual void updateCache();
    virtual void cleanupCache();
    
    // Statistics helpers
    virtual void updateQueryStatistics(bool cacheHit);
    virtual void updateProcessingStatistics(uint64_t processingTime);
    virtual void calculateStatistics();
    
    // Debug helpers
    virtual void addDebugMessage(const std::string& message);
    virtual void logUnicodeOperation(const std::string& operation, uint32_t codePoint);
    virtual std::string formatUnicodeProperties(const UnicodeProperties& properties) const;
    
    // Data initialization helpers
    virtual void initializeUnicodeData();
    virtual void initializeBlocks();
    virtual void initializeRanges();
    virtual void initializeCharacterProperties();
};

// Specialized Unicode handlers
class BasicUnicodeHandler : public UnicodeHandler {
public:
    BasicUnicodeHandler();
    UnicodeProperties getCharacterProperties(uint32_t codePoint) const override;
    std::unique_ptr<UnicodeHandler> clone() const override;
    
protected:
    UnicodeProperties getCharacterPropertiesInternal(uint32_t codePoint) const override;
};

class AdvancedUnicodeHandler : public UnicodeHandler {
public:
    AdvancedUnicodeHandler();
    std::vector<uint32_t> normalize(const std::vector<uint32_t>& codePoints, UnicodeNormalizationForm form = UnicodeNormalizationForm::NFC) const override;
    std::vector<UnicodeBidiType> getBidiTypes(const std::vector<uint32_t>& codePoints) const override;
    UnicodeScript detectScript(const std::vector<uint32_t>& codePoints) const override;
    std::unique_ptr<UnicodeHandler> clone() const override;
    
protected:
    std::vector<uint32_t> normalizeInternal(const std::vector<uint32_t>& codePoints, UnicodeNormalizationForm form) const override;
    std::vector<UnicodeBidiType> getBidiTypesInternal(const std::vector<uint32_t>& codePoints) const override;
    UnicodeScript detectScriptInternal(const std::vector<uint32_t>& codePoints) const override;
};

class EmojiUnicodeHandler : public UnicodeHandler {
public:
    EmojiUnicodeHandler();
    bool isEmoji(uint32_t codePoint) const override;
    bool isEmojiSequence(const std::vector<uint32_t>& codePoints) const override;
    std::vector<std::vector<uint32_t>> findEmojiSequences(const std::vector<uint32_t>& codePoints) const override;
    std::unique_ptr<UnicodeHandler> clone() const override;
    
protected:
    bool isEmojiSequenceInternal(const std::vector<uint32_t>& codePoints) const override;
    std::vector<std::vector<uint32_t>> findEmojiSequencesInternal(const std::vector<uint32_t>& codePoints) const override;
};

// Unicode handler factory
class UnicodeHandlerFactory {
public:
    static std::unique_ptr<UnicodeHandler> createUnicodeHandler();
    static std::unique_ptr<BasicUnicodeHandler> createBasicUnicodeHandler();
    static std::unique_ptr<AdvancedUnicodeHandler> createAdvancedUnicodeHandler();
    static std::unique_ptr<EmojiUnicodeHandler> createEmojiUnicodeHandler();
    static UnicodeHandlerConfig createDefaultConfig();
    static std::vector<UnicodeCategory> getSupportedCategories();
    static std::vector<UnicodeScript> getSupportedScripts();
    static std::vector<UnicodeBidiType> getSupportedBidiTypes();
    static std::vector<UnicodeNormalizationForm> getSupportedNormalizationForms();
    static UnicodeHandlerType detectBestHandler(const std::string& text);
    static UnicodeHandlerType detectBestHandler(const std::wstring& text);
    static UnicodeHandlerType detectBestHandler(const std::u32string& text);
};

} // namespace RFCore