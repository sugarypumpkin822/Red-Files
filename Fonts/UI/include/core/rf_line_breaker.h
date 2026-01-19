#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFCore {

// Line breaker types
enum class LineBreakType {
    UNICODE,
    ASCII,
    CUSTOM
};

// Break strategies
enum class BreakStrategy {
    GREEDY,
    OPTIMAL,
    COMPROMISE
    STRICT
    CUSTOM
};

// Break modes
enum class BreakMode {
    NORMAL,
    STRICT,
    LOOSE,
    CUSTOM
};

// Break units
enum class BreakUnit {
    CHARACTER,
    WORD,
    CLUSTER,
    LINE,
    CUSTOM
};

// Break opportunity
struct BreakOpportunity {
    uint32_t position;
    float32 penalty;
    float32 width;
    bool isRequired;
    bool isPreferred;
    std::string reason;
    
    BreakOpportunity() : position(0), penalty(0.0f), width(0.0f), isRequired(false), isPreferred(false) {}
    BreakOpportunity(uint32_t pos, float32 pen, float32 w, bool req, bool pref, const std::string& r)
        : position(pos), penalty(pen), width(w), isRequired(req), isPreferred(pref), reason(r) {}
};

// Break result
struct BreakResult {
    std::vector<uint32_t> breakPositions;
    std::vector<std::string> lines;
    std::vector<float32> lineWidths;
    float32 totalPenalty;
    bool success;
    std::string errorMessage;
    
    BreakResult() : totalPenalty(0.0f), success(false) {}
    BreakResult(const std::vector<uint32_t>& positions, const std::vector<std::string>& lines, 
               const std::vector<float32>& widths, float32 penalty, bool succ, const std::string& err)
        : breakPositions(positions), lines(lines), lineWidths(widths), totalPenalty(penalty), success(succ), errorMessage(err) {}
};

// Line breaker configuration
struct LineBreakerConfig {
    LineBreakType type;
    BreakStrategy strategy;
    BreakMode mode;
    BreakUnit unit;
    float32 maxWidth;
    float32 minWidth;
    float32 idealWidth;
    float32 tolerance;
    bool enableHyphenation;
    bool enableKerning;
    bool enableLigatures;
    bool enableUnicode;
    bool enableOptimization;
    bool enableCaching;
    bool enableStatistics;
    uint32_t maxLineLength;
    uint32_t minLineLength;
    std::string hyphenationCharacter;
    std::vector<std::string> breakCharacters;
    
    LineBreakerConfig() : type(LineBreakType::UNICODE), strategy(BreakStrategy::GREEDY), mode(BreakMode::NORMAL), 
                        unit(BreakUnit::WORD), maxWidth(80.0f), minWidth(10.0f), idealWidth(40.0f), 
                        tolerance(0.1f), enableHyphenation(true), enableKerning(false), enableLigatures(false), 
                        enableUnicode(true), enableOptimization(true), enableCaching(true), enableStatistics(true), 
                        maxLineLength(1000), minLineLength(1), hyphenationCharacter("-"), breakCharacters{" ", "\t", "-"} {}
};

// Line breaker statistics
struct LineBreakerStatistics {
    uint32_t totalBreaks;
    uint32_t successfulBreaks;
    uint32_t failedBreaks;
    uint32_t cacheHits;
    uint32_t cacheMisses;
    float32 cacheHitRatio;
    float32 averagePenalty;
    float32 averageLineLength;
    float32 averageLineWidth;
    uint32_t totalCharacters;
    uint32_t totalLines;
    uint32_t hyphenatedLines;
    uint32_t ligatureExpansions;
    
    LineBreakerStatistics() : totalBreaks(0), successfulBreaks(0), failedBreaks(0), 
                             cacheHits(0), cacheMisses(0), cacheHitRatio(0.0f), averagePenalty(0.0f), 
                             averageLineLength(0.0f), averageLineWidth(0.0f), totalCharacters(0), 
                             totalLines(0), hyphenatedLines(0), ligatureExpansions(0) {}
};

// Line breaker
class LineBreaker {
public:
    LineBreaker();
    virtual ~LineBreaker() = default;
    
    // Line breaker management
    void initialize(const LineBreakerConfig& config);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Configuration
    void setConfiguration(const LineBreakerConfig& config);
    const LineBreakerConfig& getConfiguration() const;
    void setBreakStrategy(BreakStrategy strategy);
    void setBreakMode(BreakMode mode);
    void setBreakUnit(BreakUnit unit);
    void setMaxWidth(float32 maxWidth);
    void setMinWidth(float32 minWidth);
    void setIdealWidth(float32 idealWidth);
    void setTolerance(float32 tolerance);
    
    // Line breaking
    BreakResult breakLines(const std::string& text) const;
    BreakResult breakLines(const std::wstring& text) const;
    BreakResult breakLines(const std::u32string& text) const;
    std::vector<std::string> breakText(const std::string& text) const;
    std::vector<std::wstring> breakText(const std::wstring& text) const;
    std::vector<std::u32string> breakText(const std::u32string& text) const;
    
    // Break opportunity detection
    std::vector<BreakOpportunity> detectBreakOpportunities(const std::string& text) const;
    std::vector<BreakOpportunity> detectBreakOpportunities(const std::wstring& text) const;
    std::vector<BreakOpportunity> detectBreakOpportunities(const std::u32string& text) const;
    bool isBreakOpportunity(uint32_t position, const std::string& text) const;
    bool isRequiredBreak(uint32_t position, const std::string& text) const;
    bool isPreferredBreak(uint32_t position, const std::string& text) const;
    
    // Hyphenation
    void enableHyphenation(bool enabled);
    bool isHyphenationEnabled() const;
    void setHyphenationCharacter(const std::string& character);
    std::string getHyphenationCharacter() const;
    bool canHyphenateAt(uint32_t position, const std::string& text) const;
    std::string hyphenateAt(uint32_t position, const std::string& text) const;
    
    // Kerning support
    void enableKerning(bool enabled);
    bool isKerningEnabled() const;
    float32 getKerningWidth(const std::string& text, uint32_t position) const;
    float32 getKerningWidth(const std::wstring& text, uint32_t position) const;
    float32 getKerningWidth(const std::u32string& text, uint32_t position) const;
    
    // Ligature support
    void enableLigatures(bool enabled);
    bool isLigaturesEnabled() const;
    std::vector<uint32_t> findLigatures(const std::string& text) const;
    std::vector<uint32_t> findLigatures(const std::wstring& text) const;
    std::vector<uint32_t> findLigatures(const std::u32string& text) const;
    std::string expandLigatures(const std::string& text) const;
    std::wstring expandLigatures(const std::wstring& text) const;
    std::u32string expandLigatures(const std::u32string& text) const;
    
    // Unicode support
    void enableUnicode(bool enabled);
    bool isUnicodeEnabled() const;
    bool isUnicodeCharacter(uint32_t charCode) const;
    bool isWhitespace(uint32_t charCode) const;
    bool isPunctuation(uint32_t charCode) const;
    bool isBreakCharacter(uint32_t charCode) const;
    bool isLineBreakCharacter(uint32_t charCode) const;
    
    // Text measurement
    float32 getTextWidth(const std::string& text) const;
    float32 getTextWidth(const std::wstring& text) const;
    float32 getTextWidth(const std::u32string& text) const;
    float32 getLineWidth(const std::string& text, uint32_t start, uint32_t end) const;
    float32 getLineWidth(const std::wstring& text, uint32_t start, uint32_t end) const;
    float32 getLineWidth(const std::u32string& text, uint32_t start, uint32_t end) const;
    
    // Line validation
    bool validateBreakResult(const BreakResult& result) const;
    bool validateLine(const std::string& line) const;
    bool validateLine(const std::wstring& line) const;
    bool validateLine(const std::u32string& line) const;
    std::vector<std::string> getInvalidLines(const std::vector<std::string>& lines) const;
    
    // Optimization
    void enableOptimization(bool enabled);
    bool isOptimizationEnabled() const;
    void setOptimizationLevel(uint32_t level);
    uint32_t getOptimizationLevel() const;
    BreakResult optimizeBreakResult(const BreakResult& result) const;
    
    // Caching
    void enableCache(bool enabled);
    bool isCacheEnabled() const;
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    void clearCache();
    
    // Statistics
    LineBreakerStatistics getStatistics() const;
    void updateStatistics();
    void resetStatistics();
    void enableStatistics(bool enabled);
    bool isStatisticsEnabled() const;
    
    // Debugging
    void enableDebugging(bool enabled);
    bool isDebuggingEnabled() const;
    std::vector<std::string> getDebugMessages() const;
    void clearDebugMessages();
    void dumpBreaker() const;
    std::string dumpBreakerToString() const;
    
    // Event handling
    void addLineBreakerEventListener(const std::string& eventType, std::function<void()> callback);
    void removeLineBreakerEventListener(const std::string& eventType, std::function<void()> callback);
    void clearLineBreakerEventListeners();
    
    // Utility methods
    void cloneFrom(const LineBreaker& other);
    virtual std::unique_ptr<LineBreaker> clone() const;
    bool equals(const LineBreaker& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    LineBreakerConfig config_;
    LineBreakerStatistics statistics_;
    
    bool isInitialized_;
    bool statisticsEnabled_;
    bool debuggingEnabled_;
    bool cacheEnabled_;
    bool optimizationEnabled_;
    bool hyphenationEnabled_;
    bool kerningEnabled_;
    bool ligaturesEnabled_;
    bool unicodeEnabled_;
    
    size_t currentCacheSize_;
    std::vector<std::string> debugMessages_;
    
    // Protected helper methods
    virtual void triggerLineBreakerEvent(const std::string& eventType);
    virtual BreakResult breakLinesInternal(const std::string& text) const;
    virtual BreakResult breakLinesInternal(const std::wstring& text) const;
    virtual BreakResult breakLinesInternal(const std::u32string& text) const;
    
    // Break opportunity detection helpers
    virtual std::vector<BreakOpportunity> detectBreakOpportunitiesInternal(const std::string& text) const;
    virtual std::vector<BreakOpportunity> detectBreakOpportunitiesInternal(const std::wstring& text) const;
    virtual std::vector<BreakOpportunity> detectBreakOpportunitiesInternal(const std::u32string& text) const;
    virtual bool isBreakOpportunityInternal(uint32_t position, const std::string& text) const;
    virtual bool isRequiredBreakInternal(uint32_t position, const std::string& text) const;
    virtual bool isPreferredBreakInternal(uint32_t position, const std::string& text) const;
    
    // Hyphenation helpers
    virtual bool canHyphenateAtInternal(uint32_t position, const std::string& text) const;
    virtual std::string hyphenateAtInternal(uint32_t position, const std::string& text) const;
    virtual std::string findHyphenationPoint(const std::string& word) const;
    
    // Kerning helpers
    virtual float32 calculateKerningWidth(const std::string& text, uint32_t position) const;
    virtual float32 calculateKerningWidth(const std::wstring& text, uint32_t position) const;
    virtual float32 calculateKerningWidth(const std::u32string& text, uint32_t position) const;
    
    // Ligature helpers
    virtual std::vector<uint32_t> findLigaturesInternal(const std::string& text) const;
    virtual std::vector<uint32_t> findLigaturesInternal(const std::wstring& text) const;
    virtual std::vector<uint32_t> findLigaturesInternal(const std::u32string& text) const;
    virtual std::string expandLigaturesInternal(const std::string& text) const;
    virtual std::wstring expandLigaturesInternal(const std::wstring& text) const;
    virtual std::u32string expandLigaturesInternal(const std::u32string& text) const;
    
    // Unicode helpers
    virtual bool isUnicodeCharacterInternal(uint32_t charCode) const;
    virtual bool isWhitespaceInternal(uint32_t charCode) const;
    virtual bool isPunctuationInternal(uint32_t charCode) const;
    virtual bool isBreakCharacterInternal(uint32_t charCode) const;
    virtual bool isLineBreakCharacterInternal(uint32_t charCode) const;
    
    // Text measurement helpers
    virtual float32 calculateTextWidth(const std::string& text) const;
    virtual float32 calculateTextWidth(const std::wstring& text) const;
    virtual float32 calculateTextWidth(const std::u32string& text) const;
    virtual float32 calculateLineWidth(const std::string& text, uint32_t start, uint32_t end) const;
    virtual float32 calculateLineWidth(const std::wstring& text, uint32_t start, uint32_t end) const;
    virtual float32 calculateLineWidth(const std::std::u32string& text, uint32_t start, uint32_t end) const;
    
    // Break algorithm helpers
    virtual std::vector<uint32_t> greedyBreak(const std::string& text) const;
    virtual std::vector<uint32_t> optimalBreak(const std::string& text) const;
    virtual std::vector<uint32_t> compromiseBreak(const std::string& text) const;
    virtual std::vector<uint32_t> strictBreak(const std::string& text) const;
    virtual std::vector<uint32_t> customBreak(const std::string& text) const;
    
    // Penalty calculation helpers
    virtual float32 calculateBreakPenalty(const std::string& line, float32 lineWidth, float32 idealWidth) const;
    virtual float32 calculateHyphenationPenalty(const std::string& line) const;
    virtual float32 calculateKerningPenalty(const std::string& line) const;
    virtual float32 calculateLigaturePenalty(const std::string& line) const;
    virtual float32 calculateWidthPenalty(float32 lineWidth, float32 idealWidth) const;
    
    // Validation helpers
    virtual bool validateBreakResultInternal(const BreakResult& result) const;
    virtual bool validateLineInternal(const std::string& line) const;
    virtual bool validateLineInternal(const std::wstring& line) const;
    virtual bool validateLineInternal(const std::std::u32string& line) const;
    
    // Optimization helpers
    virtual BreakResult optimizeBreakResultInternal(const BreakResult& result) const;
    virtual std::vector<uint32_t> removeRedundantBreaks(const std::vector<uint32_t>& breakPositions, const std::string& text) const;
    virtual std::vector<uint32_t> addMissingBreaks(const std::vector<uint32_t>& breakPositions, const std::string& text) const;
    
    // Cache helpers
    virtual std::string generateCacheKey(const std::string& text, const LineBreakerConfig& config) const;
    virtual bool getFromCache(const std::string& key, BreakResult& result) const;
    virtual void addToCache(const std::string& key, const BreakResult& result);
    virtual void removeFromCache(const std::string& key);
    virtual void updateCache();
    virtual void cleanupCache();
    
    // Statistics helpers
    virtual void updateBreakStatistics(bool success, const BreakResult& result);
    virtual void updateCacheStatistics(bool hit);
    virtual void calculateStatistics();
    
    // Debug helpers
    virtual void addDebugMessage(const std::string& message);
    virtual void logBreakOperation(const std::string& operation, const std::string& text);
    virtual std::string formatBreakResult(const BreakResult& result) const;
};

// Specialized line breakers
class UnicodeLineBreaker : public LineBreaker {
public:
    UnicodeLineBreaker();
    std::vector<BreakOpportunity> detectBreakOpportunities(const std::string& text) const override;
    bool isUnicodeCharacter(uint32_t charCode) const override;
    bool isWhitespace(uint32_t charCode) const override;
    bool isPunctuation(uint32_t charCode) const override;
    bool isBreakCharacter(uint32_t charCode) const override;
    bool isLineBreakCharacter(uint32_t charCode) const override;
    std::unique_ptr<LineBreaker> clone() const override;
    
protected:
    std::vector<BreakOpportunity> detectBreakOpportunitiesInternal(const std::string& text) const override;
    bool isUnicodeCharacterInternal(uint32_t charCode) const override;
    bool isWhitespaceInternal(uint32_t charCode) const override;
    bool isPunctuationInternal(uint32_t charCode) const;
    bool isBreakCharacterInternal(uint32_t charCode) const override;
    bool isLineBreakCharacterInternal(uint32_t charCode) const override;
};

class ASCIILineBreaker : public LineBreaker {
public:
    ASCIILineBreaker();
    std::vector<BreakOpportunity> detectBreakOpportunities(const std::string& text) const override;
    bool isUnicodeCharacter(uint32_t charCode) const override;
    bool isWhitespace(uint32_t charCode) const override;
    bool isPunctuation(uint32_t charCode) const override;
    bool isBreakCharacter(uint32_t charCode) const override;
    bool isLineBreakCharacter(uint32_t charCode) const override;
    std::unique_ptr<LineBreaker> clone() const override;
    
protected:
    std::vector<BreakOpportunity> detectBreakOpportunitiesInternal(const std::string& text) const override;
    bool isUnicodeCharacterInternal(uint32_t charCode) const override;
    bool isWhitespaceInternal(uint32_t charCode) const override;
    bool isPunctuationInternal(uint32_t charCode) const override;
    bool isBreakCharacterInternal(uint32_t charCode) const override;
    bool isLineBreakCharacterInternal(uint32_t charCode) const override;
};

// Line breaker factory
class LineBreakerFactory {
public:
    static std::unique_ptr<LineBreaker> createLineBreaker(LineBreakType type);
    static std::unique_ptr<UnicodeLineBreaker> createUnicodeLineBreaker();
    static std::unique_ptr<ASCIILineBreaker> createASCIILineBreaker();
    static LineBreakerConfig createDefaultConfig(LineBreakType type);
    static std::vector<LineBreakType> getSupportedBreakerTypes();
    static std::vector<BreakStrategy> getSupportedBreakStrategies();
    static std::vector<BreakMode> getSupportedBreakModes();
    static std::vector<BreakUnit> getSupportedBreakUnits();
    static LineBreakType detectBestType(const std::string& text);
    static LineBreakType detectBestType(const std::wstring& text);
    static LineBreakType detectBestType(const std::u32string& text);
};

} // namespace RFCore