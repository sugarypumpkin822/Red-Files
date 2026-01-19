#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFCore {

// Kerning types
enum class KerningType {
    HORIZONTAL,
    VERTICAL,
    BOTH
};

// Kerning directions
enum class KerningDirection {
    LEFT_TO_RIGHT,
    RIGHT_TO_LEFT,
    TOP_TO_BOTTOM,
    BOTTOM_TO_TOP
};

// Kerning lookup types
enum class KerningLookupType {
    SIMPLE_PAIR,
    CLASS_BASED,
    CONTEXTUAL,
    GPOS,
    CUSTOM
};

// Kerning pair
struct KerningPair {
    uint32_t leftGlyph;
    uint32_t rightGlyph;
    float32 xAdvance;
    float32 yAdvance;
    KerningDirection direction;
    
    KerningPair() : leftGlyph(0), rightGlyph(0), xAdvance(0.0f), yAdvance(0.0f), 
                   direction(KerningDirection::LEFT_TO_RIGHT) {}
    KerningPair(uint32_t left, uint32_t right, float32 xAdv, float32 yAdv, KerningDirection dir)
        : leftGlyph(left), rightGlyph(right), xAdvance(xAdv), yAdvance(yAdv), direction(dir) {}
};

// Kerning class
struct KerningClass {
    uint32_t classIndex;
    std::vector<uint32_t> glyphs;
    std::string className;
    
    KerningClass() : classIndex(0) {}
    KerningClass(uint32_t idx, const std::vector<uint32_t>& glyphs, const std::string& name)
        : classIndex(idx), glyphs(glyphs), className(name) {}
};

// Kerning class pair
struct KerningClassPair {
    uint32_t leftClass;
    uint32_t rightClass;
    float32 xAdvance;
    float32 yAdvance;
    KerningDirection direction;
    
    KerningClassPair() : leftClass(0), rightClass(0), xAdvance(0.0f), yAdvance(0.0f), 
                        direction(KerningDirection::LEFT_TO_RIGHT) {}
    KerningClassPair(uint32_t left, uint32_t right, float32 xAdv, float32 yAdv, KerningDirection dir)
        : leftClass(left), rightClass(right), xAdvance(xAdv), yAdvance(yAdv), direction(dir) {}
};

// Contextual kerning rule
struct ContextualKerningRule {
    std::vector<uint32_t> context;
    std::vector<KerningPair> substitutions;
    std::string ruleName;
    bool isLookahead;
    bool isLookbehind;
    
    ContextualKerningRule() : ruleName(""), isLookahead(false), isLookbehind(false) {}
    ContextualKerningRule(const std::vector<uint32_t>& ctx, const std::vector<KerningPair>& subs, 
                        const std::string& name, bool lookahead, bool lookbehind)
        : context(ctx), substitutions(subs), ruleName(name), isLookahead(lookahead), isLookbehind(lookbehind) {}
};

// Kerning cache entry
struct KerningCacheEntry {
    std::pair<uint32_t, uint32_t> glyphPair;
    KerningPair kerningPair;
    uint64_t timestamp;
    uint32_t accessCount;
    
    KerningCacheEntry() : glyphPair(0, 0), timestamp(0), accessCount(0) {}
    KerningCacheEntry(const std::pair<uint32_t, uint32_t>& pair, const KerningPair& kerning, uint64_t ts)
        : glyphPair(pair), kerningPair(kerning), timestamp(ts), accessCount(1) {}
};

// Kerning statistics
struct KerningStatistics {
    uint32_t totalPairs;
    uint32_t totalClasses;
    uint32_t totalRules;
    uint32_t cachedPairs;
    float32 averageXAdvance;
    float32 averageYAdvance;
    float32 maxXAdvance;
    float32 maxYAdvance;
    float32 minXAdvance;
    float32 minYAdvance;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    float32 cacheHitRatio;
    
    KerningStatistics() : totalPairs(0), totalClasses(0), totalRules(0), cachedPairs(0), 
                         averageXAdvance(0.0f), averageYAdvance(0.0f), maxXAdvance(0.0f), 
                         maxYAdvance(0.0f), minXAdvance(0.0f), minYAdvance(0.0f), 
                         cacheHits(0), cacheMisses(0), cacheHitRatio(0.0f) {}
};

// Kerning manager
class KerningManager {
public:
    KerningManager();
    virtual ~KerningManager() = default;
    
    // Kerning management
    void initialize();
    void reset();
    void clearCache();
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    
    // Kerning pair access
    bool getKerningPair(uint32_t leftGlyph, uint32_t rightGlyph, KerningPair& pair) const;
    bool getKerningPair(uint32_t leftGlyph, uint32_t rightGlyph, KerningDirection direction, KerningPair& pair) const;
    float32 getKerningXAdvance(uint32_t leftGlyph, uint32_t rightGlyph) const;
    float32 getKerningYAdvance(uint32_t leftGlyph, uint32_t rightGlyph) const;
    float32 getKerningXAdvance(uint32_t leftGlyph, uint32_t rightGlyph, KerningDirection direction) const;
    float32 getKerningYAdvance(uint32_t leftGlyph, uint32_t rightGlyph, KerningDirection direction) const;
    
    // Kerning pair management
    void setKerningPair(const KerningPair& pair);
    void setKerningPair(uint32_t leftGlyph, uint32_t rightGlyph, float32 xAdvance, float32 yAdvance = 0.0f);
    void setKerningPair(uint32_t leftGlyph, uint32_t rightGlyph, float32 xAdvance, float32 yAdvance, KerningDirection direction);
    void removeKerningPair(uint32_t leftGlyph, uint32_t rightGlyph);
    void removeKerningPair(uint32_t leftGlyph, uint32_t rightGlyph, KerningDirection direction);
    bool hasKerningPair(uint32_t leftGlyph, uint32_t rightGlyph) const;
    bool hasKerningPair(uint32_t leftGlyph, uint32_t rightGlyph, KerningDirection direction) const;
    
    // Batch operations
    void setKerningPairs(const std::vector<KerningPair>& pairs);
    void setKerningPairs(const std::map<std::pair<uint32_t, uint32_t>, KerningPair>& pairs);
    void removeKerningPairs(const std::vector<std::pair<uint32_t, uint32_t>>& pairs);
    std::vector<KerningPair> getAllKerningPairs() const;
    std::vector<KerningPair> getKerningPairs(KerningDirection direction) const;
    
    // Class-based kerning
    bool getKerningClass(uint32_t classIndex, KerningClass& kerningClass) const;
    bool getKerningClassPair(uint32_t leftClass, uint32_t rightClass, KerningClassPair& pair) const;
    float32 getKerningClassXAdvance(uint32_t leftClass, uint32_t rightClass) const;
    float32 getKerningClassYAdvance(uint32_t leftClass, uint32_t rightClass) const;
    void setKerningClass(const KerningClass& kerningClass);
    void setKerningClassPair(const KerningClassPair& pair);
    void removeKerningClass(uint32_t classIndex);
    void removeKerningClassPair(uint32_t leftClass, uint32_t rightClass);
    bool hasKerningClass(uint32_t classIndex) const;
    bool hasKerningClassPair(uint32_t leftClass, uint32_t rightClass) const;
    uint32_t getGlyphClass(uint32_t glyph) const;
    
    // Contextual kerning
    bool getContextualKerningRule(const std::string& ruleName, ContextualKerningRule& rule) const;
    std::vector<KerningPair> applyContextualKerning(const std::vector<uint32_t>& glyphs) const;
    std::vector<KerningPair> applyContextualKerning(const std::vector<uint32_t>& glyphs, KerningDirection direction) const;
    void setContextualKerningRule(const ContextualKerningRule& rule);
    void removeContextualKerningRule(const std::string& ruleName);
    bool hasContextualKerningRule(const std::string& ruleName) const;
    std::vector<ContextualKerningRule> getAllContextualKerningRules() const;
    
    // Text kerning
    std::vector<KerningPair> getTextKerning(const std::string& text) const;
    std::vector<KerningPair> getTextKerning(const std::vector<uint32_t>& glyphIndices) const;
    std::vector<KerningPair> getTextKerning(const std::string& text, KerningDirection direction) const;
    std::vector<KerningPair> getTextKerning(const std::vector<uint32_t>& glyphIndices, KerningDirection direction) const;
    float32 getTextKerningAdvance(const std::string& text) const;
    float32 getTextKerningAdvance(const std::vector<uint32_t>& glyphIndices) const;
    float32 getTextKerningAdvance(const std::string& text, KerningDirection direction) const;
    float32 getTextKerningAdvance(const std::vector<uint32_t>& glyphIndices, KerningDirection direction) const;
    
    // Kerning validation
    bool validateKerningPair(const KerningPair& pair) const;
    bool validateKerningClass(const KerningClass& kerningClass) const;
    bool validateKerningClassPair(const KerningClassPair& pair) const;
    bool validateContextualKerningRule(const ContextualKerningRule& rule) const;
    
    // Kerning comparison
    bool compareKerningPairs(const KerningPair& a, const KerningPair& b, float32 tolerance = 0.001f) const;
    bool compareKerningClasses(const KerningClass& a, const KerningClass& b) const;
    bool compareKerningClassPairs(const KerningClassPair& a, const KerningClassPair& b, float32 tolerance = 0.001f) const;
    
    // Statistics
    KerningStatistics getStatistics() const;
    void updateStatistics();
    void resetStatistics();
    
    // Import/Export
    bool importKerningPairs(const std::string& filename);
    bool exportKerningPairs(const std::string& filename) const;
    bool importKerningClasses(const std::string& filename);
    bool exportKerningClasses(const std::string& filename) const;
    bool importContextualKerningRules(const std::string& filename);
    bool exportContextualKerningRules(const std::string& filename) const;
    
    // Event handling
    void addKerningEventListener(const std::string& eventType, std::function<void()> callback);
    void removeKerningEventListener(const std::string& eventType, std::function<void()> callback);
    void clearKerningEventListeners();
    
    // Utility methods
    void cloneFrom(const KerningManager& other);
    virtual std::unique_ptr<KerningManager> clone() const;
    bool equals(const KerningManager& other) const;
    size_t hash() const;
    
protected:
    // Protected members
    std::map<std::pair<uint32_t, uint32_t>, KerningPair> kerningPairs_;
    std::map<uint32_t, KerningClass> kerningClasses_;
    std::map<std::pair<uint32_t, uint32_t>, KerningClassPair> kerningClassPairs_;
    std::map<std::string, ContextualKerningRule> contextualKerningRules_;
    std::map<std::pair<uint32_t, uint32_t>, KerningCacheEntry> kerningCache_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    KerningStatistics statistics_;
    size_t maxCacheSize_;
    
    // Protected helper methods
    virtual void triggerKerningEvent(const std::string& eventType);
    virtual void updateCache();
    virtual void cleanupCache();
    virtual bool getFromCache(uint32_t leftGlyph, uint32_t rightGlyph, KerningPair& pair) const;
    virtual void addToCache(uint32_t leftGlyph, uint32_t rightGlyph, const KerningPair& pair);
    virtual void removeFromCache(uint32_t leftGlyph, uint32_t rightGlyph);
    
    // Calculation helpers
    virtual KerningPair calculateKerningPair(uint32_t leftGlyph, uint32_t rightGlyph) const;
    virtual KerningPair calculateKerningPair(uint32_t leftGlyph, uint32_t rightGlyph, KerningDirection direction) const;
    virtual std::vector<KerningPair> calculateTextKerning(const std::vector<uint32_t>& glyphIndices) const;
    virtual std::vector<KerningPair> calculateTextKerning(const std::vector<uint32_t>& glyphIndices, KerningDirection direction) const;
    
    // Validation helpers
    virtual bool isValidGlyphIndex(uint32_t glyphIndex) const;
    virtual bool isValidAdvance(float32 advance) const;
    virtual bool isValidClassIndex(uint32_t classIndex) const;
    virtual bool isValidDirection(KerningDirection direction) const;
    
    // Import/Export helpers
    virtual bool parseKerningPair(const std::string& line, KerningPair& pair) const;
    virtual bool parseKerningClass(const std::string& line, KerningClass& kerningClass) const;
    virtual bool parseContextualKerningRule(const std::string& line, ContextualKerningRule& rule) const;
    virtual std::string formatKerningPair(const KerningPair& pair) const;
    virtual std::string formatKerningClass(const KerningClass& kerningClass) const;
    virtual std::string formatContextualKerningRule(const ContextualKerningRule& rule) const;
};

// Specialized kerning managers
class HorizontalKerningManager : public KerningManager {
public:
    HorizontalKerningManager();
    bool getKerningPair(uint32_t leftGlyph, uint32_t rightGlyph, KerningPair& pair) const override;
    float32 getKerningXAdvance(uint32_t leftGlyph, uint32_t rightGlyph) const override;
    std::vector<KerningPair> getTextKerning(const std::vector<uint32_t>& glyphIndices) const override;
    std::unique_ptr<KerningManager> clone() const override;
    
protected:
    void triggerKerningEvent(const std::string& eventType) override;
    void updateCache() override;
};

class VerticalKerningManager : public KerningManager {
public:
    VerticalKerningManager();
    bool getKerningPair(uint32_t leftGlyph, uint32_t rightGlyph, KerningPair& pair) const override;
    float32 getKerningYAdvance(uint32_t leftGlyph, uint32_t rightGlyph) const override;
    std::vector<KerningPair> getTextKerning(const std::vector<uint32_t>& glyphIndices) const override;
    std::unique_ptr<KerningManager> clone() const override;
    
protected:
    void triggerKerningEvent(const std::string& eventType) override;
    void updateCache() override;
};

class ContextualKerningManager : public KerningManager {
public:
    ContextualKerningManager();
    std::vector<KerningPair> applyContextualKerning(const std::vector<uint32_t>& glyphs) const override;
    std::vector<KerningPair> getTextKerning(const std::vector<uint32_t>& glyphIndices) const override;
    std::unique_ptr<KerningManager> clone() const override;
    
protected:
    void triggerKerningEvent(const std::string& eventType) override;
    void updateCache() override;
};

// Kerning factory
class KerningFactory {
public:
    static std::unique_ptr<KerningManager> createKerningManager(KerningType type);
    static std::unique_ptr<HorizontalKerningManager> createHorizontalKerningManager();
    static std::unique_ptr<VerticalKerningManager> createVerticalKerningManager();
    static std::unique_ptr<ContextualKerningManager> createContextualKerningManager();
    static KerningPair createDefaultKerningPair(uint32_t leftGlyph, uint32_t rightGlyph);
    static KerningClass createDefaultKerningClass(uint32_t classIndex);
    static KerningClassPair createDefaultKerningClassPair(uint32_t leftClass, uint32_t rightClass);
    static ContextualKerningRule createDefaultContextualKerningRule(const std::string& name);
    static std::vector<KerningType> getSupportedKerningTypes();
    static std::vector<KerningDirection> getSupportedKerningDirections();
    static std::vector<KerningLookupType> getSupportedKerningLookupTypes();
};

} // namespace RFCore