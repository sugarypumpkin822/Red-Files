#include "baker.h"
#include "rasterizer.h"
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>

namespace FontBaker {

class KerningExtractorImpl {
public:
    struct KerningSettings {
        bool extractHorizontal = true;
        bool extractVertical = false;
        bool extractCrossStream = false;
        float minKerningAmount = 0.1f;    // Minimum kerning amount to extract
        float maxKerningDistance = 100.0f; // Maximum distance to consider
        bool optimizePairs = true;          // Remove redundant pairs
        bool includeZeroKerning = false;   // Include zero kerning pairs
    };
    
    KerningExtractorImpl();
    ~KerningExtractorImpl() = default;
    
    std::vector<KerningPair> extractKerningPairs(const FontFace& font, 
                                                const KerningSettings& settings);
    
    // Advanced kerning analysis
    std::vector<KerningPair> extractOpticalKerning(const FontFace& font, 
                                                   const KerningSettings& settings);
    std::vector<KerningPair> extractContextualKerning(const FontFace& font, 
                                                      const KerningSettings& settings);
    
    // Kerning optimization
    void optimizeKerningPairs(std::vector<KerningPair>& kerningPairs, const KerningSettings& settings);
    void removeRedundantPairs(std::vector<KerningPair>& kerningPairs);
    void mergeSimilarPairs(std::vector<KerningPair>& kerningPairs, float threshold = 0.1f);
    
    // Kerning analysis
    struct KerningStatistics {
        float averageKerning;
        float maxKerning;
        float minKerning;
        size_t totalPairs;
        size_t positivePairs;
        size_t negativePairs;
        std::map<uint32_t, size_t> leftGlyphFrequency;
        std::map<uint32_t, size_t> rightGlyphFrequency;
    };
    
    KerningStatistics analyzeKerning(const std::vector<KerningPair>& kerningPairs);
    
private:
    // Pair extraction helpers
    std::vector<KerningPair> extractFromKernTable(const FontFace& font);
    std::vector<KerningPair> extractFromGlyphMetrics(const FontFace& font, const KerningSettings& settings);
    std::vector<KerningPair> extractFromGlyphShapes(const FontFace& font, const KerningSettings& settings);
    
    // Optical kerning
    float calculateOpticalKerning(const Glyph& leftGlyph, const Glyph& rightGlyph, 
                                  const KerningSettings& settings);
    float calculateGlyphOverlap(const Glyph& glyph1, const Glyph& glyph2);
    std::vector<float> calculateGlyphProfile(const Glyph& glyph, bool horizontal = true);
    
    // Contextual kerning
    struct ContextualKerningRule {
        uint32_t leftGlyph;
        uint32_t centerGlyph;
        uint32_t rightGlyph;
        float kerningAmount;
        std::string context;
    };
    
    std::vector<ContextualKerningRule> extractContextualRules(const FontFace& font, 
                                                           const KerningSettings& settings);
    float calculateContextualKerning(uint32_t left, uint32_t center, uint32_t right, 
                                     const FontFace& font);
    
    // Shape analysis
    struct GlyphShape {
        std::vector<float> leftProfile;
        std::vector<float> rightProfile;
        std::vector<float> topProfile;
        std::vector<float> bottomProfile;
        float boundingBoxWidth;
        float boundingBoxHeight;
    };
    
    GlyphShape analyzeGlyphShape(const Glyph& glyph);
    float calculateProfileDistance(const std::vector<float>& profile1, 
                                const std::vector<float>& profile2);
    
    // Utility functions
    bool isKerningPairSignificant(const KerningPair& pair, const KerningSettings& settings);
    float normalizeKerningAmount(float amount, float unitsPerEm);
    uint32_t getGlyphIndex(const FontFace& font, uint32_t characterCode);
    
    // Caching
    std::map<uint32_t, GlyphShape> glyphShapeCache_;
    GlyphShape getCachedGlyphShape(const FontFace& font, uint32_t glyphIndex);
};

KerningExtractorImpl::KerningExtractorImpl() {}

std::vector<KerningPair> KerningExtractorImpl::extractKerningPairs(const FontFace& font, 
                                                                   const KerningSettings& settings) {
    std::vector<KerningPair> allPairs;
    
    // Extract from font's kerning table
    if (!font.kerningPairs.empty()) {
        auto tablePairs = extractFromKernTable(font);
        allPairs.insert(allPairs.end(), tablePairs.begin(), tablePairs.end());
    }
    
    // Extract from glyph metrics (fallback for fonts without kerning tables)
    auto metricPairs = extractFromGlyphMetrics(font, settings);
    allPairs.insert(allPairs.end(), metricPairs.begin(), metricPairs.end());
    
    // Extract from glyph shapes (advanced analysis)
    auto shapePairs = extractFromGlyphShapes(font, settings);
    allPairs.insert(allPairs.end(), shapePairs.begin(), shapePairs.end());
    
    // Optimize and filter pairs
    if (settings.optimizePairs) {
        optimizeKerningPairs(allPairs, settings);
    }
    
    // Filter by minimum kerning amount
    if (!settings.includeZeroKerning) {
        allPairs.erase(
            std::remove_if(allPairs.begin(), allPairs.end(),
                [&settings](const KerningPair& pair) {
                    return std::abs(pair.kerningAmount) < settings.minKerningAmount;
                }),
            allPairs.end()
        );
    }
    
    // Remove duplicates
    std::sort(allPairs.begin(), allPairs.end(),
        [](const KerningPair& a, const KerningPair& b) {
            if (a.leftGlyph != b.leftGlyph) return a.leftGlyph < b.leftGlyph;
            return a.rightGlyph < b.rightGlyph;
        });
    
    allPairs.erase(
        std::unique(allPairs.begin(), allPairs.end(),
            [](const KerningPair& a, const KerningPair& b) {
                return a.leftGlyph == b.leftGlyph && a.rightGlyph == b.rightGlyph;
            }),
        allPairs.end()
    );
    
    return allPairs;
}

std::vector<KerningPair> KerningExtractorImpl::extractOpticalKerning(const FontFace& font, 
                                                                const KerningSettings& settings) {
    std::vector<KerningPair> opticalPairs;
    
    // Calculate optical kerning for common glyph combinations
    for (const auto& leftGlyph : font.glyphs) {
        for (const auto& rightGlyph : font.glyphs) {
            float kerningAmount = calculateOpticalKerning(leftGlyph, rightGlyph, settings);
            
            if (isKerningPairSignificant({leftGlyph.index, rightGlyph.index, kerningAmount}, settings)) {
                opticalPairs.push_back({leftGlyph.index, rightGlyph.index, kerningAmount});
            }
        }
    }
    
    return opticalPairs;
}

std::vector<KerningPair> KerningExtractorImpl::extractContextualKerning(const FontFace& font, 
                                                                   const KerningSettings& settings) {
    std::vector<KerningPair> contextualPairs;
    
    // Extract contextual kerning rules
    auto rules = extractContextualRules(font, settings);
    
    // Convert rules to pairs (simplified)
    for (const auto& rule : rules) {
        if (isKerningPairSignificant({rule.leftGlyph, rule.rightGlyph, rule.kerningAmount}, settings)) {
            contextualPairs.push_back({rule.leftGlyph, rule.rightGlyph, rule.kerningAmount});
        }
    }
    
    return contextualPairs;
}

void KerningExtractorImpl::optimizeKerningPairs(std::vector<KerningPair>& kerningPairs, 
                                                const KerningSettings& settings) {
    // Remove redundant pairs
    removeRedundantPairs(kerningPairs);
    
    // Merge similar pairs
    mergeSimilarPairs(kerningPairs);
    
    // Sort by importance (absolute kerning amount)
    std::sort(kerningPairs.begin(), kerningPairs.end(),
        [](const KerningPair& a, const KerningPair& b) {
            return std::abs(a.kerningAmount) > std::abs(b.kerningAmount);
        });
}

void KerningExtractorImpl::removeRedundantPairs(std::vector<KerningPair>& kerningPairs) {
    // Remove pairs where kerning amount is effectively zero
    kerningPairs.erase(
        std::remove_if(kerningPairs.begin(), kerningPairs.end(),
            [](const KerningPair& pair) {
                return std::abs(pair.kerningAmount) < 0.01f;
            }),
        kerningPairs.end()
    );
    
    // Remove symmetric pairs with opposite signs (they cancel out)
    std::set<std::pair<uint32_t, uint32_t>> processed;
    
    for (auto it = kerningPairs.begin(); it != kerningPairs.end();) {
        auto pair = *it;
        auto key = std::make_pair(pair.leftGlyph, pair.rightGlyph);
        auto reverseKey = std::make_pair(pair.rightGlyph, pair.leftGlyph);
        
        if (processed.count(reverseKey)) {
            // Find the reverse pair
            auto reverseIt = std::find_if(kerningPairs.begin(), kerningPairs.end(),
                [&reverseKey](const KerningPair& p) {
                    return p.leftGlyph == reverseKey.first && p.rightGlyph == reverseKey.second;
                });
            
            if (reverseIt != kerningPairs.end()) {
                // Check if they cancel out
                float sum = pair.kerningAmount + reverseIt->kerningAmount;
                if (std::abs(sum) < 0.1f) {
                    // Remove both pairs
                    kerningPairs.erase(it);
                    kerningPairs.erase(reverseIt);
                    continue;
                }
            }
        }
        
        processed.insert(key);
        ++it;
    }
}

void KerningExtractorImpl::mergeSimilarPairs(std::vector<KerningPair>& kerningPairs, float threshold) {
    // Group pairs with similar kerning amounts
    std::map<std::pair<uint32_t, uint32_t>, std::vector<KerningPair>> groupedPairs;
    
    for (const auto& pair : kerningPairs) {
        auto key = std::make_pair(pair.leftGlyph, pair.rightGlyph);
        groupedPairs[key].push_back(pair);
    }
    
    // Average similar pairs
    std::vector<KerningPair> mergedPairs;
    for (const auto& group : groupedPairs) {
        if (group.second.size() > 1) {
            float totalAmount = 0.0f;
            for (const auto& pair : group.second) {
                totalAmount += pair.kerningAmount;
            }
            float averageAmount = totalAmount / group.second.size();
            
            // Check if variation is within threshold
            bool shouldMerge = true;
            for (const auto& pair : group.second) {
                if (std::abs(pair.kerningAmount - averageAmount) > threshold) {
                    shouldMerge = false;
                    break;
                }
            }
            
            if (shouldMerge) {
                mergedPairs.push_back({group.first.first, group.first.second, averageAmount});
            } else {
                // Keep individual pairs
                mergedPairs.insert(mergedPairs.end(), group.second.begin(), group.second.end());
            }
        } else {
            mergedPairs.push_back(group.second[0]);
        }
    }
    
    kerningPairs = mergedPairs;
}

KerningExtractorImpl::KerningStatistics KerningExtractorImpl::analyzeKerning(const std::vector<KerningPair>& kerningPairs) {
    KerningStatistics stats;
    
    if (kerningPairs.empty()) {
        return stats;
    }
    
    stats.totalPairs = kerningPairs.size();
    stats.positivePairs = 0;
    stats.negativePairs = 0;
    
    float totalKerning = 0.0f;
    stats.maxKerning = kerningPairs[0].kerningAmount;
    stats.minKerning = kerningPairs[0].kerningAmount;
    
    for (const auto& pair : kerningPairs) {
        totalKerning += pair.kerningAmount;
        stats.maxKerning = std::max(stats.maxKerning, pair.kerningAmount);
        stats.minKerning = std::min(stats.minKerning, pair.kerningAmount);
        
        if (pair.kerningAmount > 0) {
            stats.positivePairs++;
        } else if (pair.kerningAmount < 0) {
            stats.negativePairs++;
        }
        
        stats.leftGlyphFrequency[pair.leftGlyph]++;
        stats.rightGlyphFrequency[pair.rightGlyph]++;
    }
    
    stats.averageKerning = totalKerning / kerningPairs.size();
    
    return stats;
}

std::vector<KerningPair> KerningExtractorImpl::extractFromKernTable(const FontFace& font) {
    // Direct extraction from font's kerning table
    return font.kerningPairs;
}

std::vector<KerningPair> KerningExtractorImpl::extractFromGlyphMetrics(const FontFace& font, 
                                                                  const KerningSettings& settings) {
    std::vector<KerningPair> metricPairs;
    
    // Calculate kerning based on glyph metrics and bounding boxes
    for (const auto& leftGlyph : font.glyphs) {
        for (const auto& rightGlyph : font.glyphs) {
            // Simple metric-based kerning
            float kerningAmount = leftGlyph.advanceX + rightGlyph.bearingX - 
                                (leftGlyph.boundingBox.x + leftGlyph.boundingBox.width);
            
            if (isKerningPairSignificant({leftGlyph.index, rightGlyph.index, kerningAmount}, settings)) {
                metricPairs.push_back({leftGlyph.index, rightGlyph.index, kerningAmount});
            }
        }
    }
    
    return metricPairs;
}

std::vector<KerningPair> KerningExtractorImpl::extractFromGlyphShapes(const FontFace& font, 
                                                                  const KerningSettings& settings) {
    std::vector<KerningPair> shapePairs;
    
    // Calculate kerning based on glyph shapes
    for (const auto& leftGlyph : font.glyphs) {
        auto leftShape = getCachedGlyphShape(font, leftGlyph.index);
        
        for (const auto& rightGlyph : font.glyphs) {
            auto rightShape = getCachedGlyphShape(font, rightGlyph.index);
            
            // Calculate overlap-based kerning
            float overlap = calculateGlyphOverlap(leftGlyph, rightGlyph);
            float kerningAmount = -overlap * 0.5f; // Adjust by overlap
            
            if (isKerningPairSignificant({leftGlyph.index, rightGlyph.index, kerningAmount}, settings)) {
                shapePairs.push_back({leftGlyph.index, rightGlyph.index, kerningAmount});
            }
        }
    }
    
    return shapePairs;
}

float KerningExtractorImpl::calculateOpticalKerning(const Glyph& leftGlyph, const Glyph& rightGlyph, 
                                                   const KerningSettings& settings) {
    // Calculate optical kerning based on glyph shapes
    auto leftShape = analyzeGlyphShape(leftGlyph);
    auto rightShape = analyzeGlyphShape(rightGlyph);
    
    // Compare right edge of left glyph with left edge of right glyph
    float rightEdgeDistance = calculateProfileDistance(leftShape.rightProfile, rightShape.leftProfile);
    
    // Normalize by font units
    float normalizedKerning = rightEdgeDistance / leftGlyph.boundingBox.width;
    
    return normalizedKerning * settings.maxKerningDistance;
}

float KerningExtractorImpl::calculateGlyphOverlap(const Glyph& glyph1, const Glyph& glyph2) {
    // Simple overlap calculation based on bounding boxes
    float overlap = 0.0f;
    
    // Horizontal overlap
    float horizontalOverlap = (glyph1.boundingBox.x + glyph1.boundingBox.width) - glyph2.boundingBox.x;
    if (horizontalOverlap > 0) {
        overlap += horizontalOverlap;
    }
    
    return overlap;
}

std::vector<float> KerningExtractorImpl::calculateGlyphProfile(const Glyph& glyph, bool horizontal) {
    std::vector<float> profile;
    
    if (horizontal) {
        // Calculate horizontal profile (left to right)
        int width = static_cast<int>(glyph.boundingBox.width);
        profile.resize(width);
        
        for (int x = 0; x < width; ++x) {
            float minY = glyph.boundingBox.height;
            float maxY = 0.0f;
            
            // Find min and max Y at this X position
            for (const auto& point : glyph.outlinePoints) {
                if (std::abs(point.x - glyph.boundingBox.x - x) < 1.0f) {
                    minY = std::min(minY, point.y - glyph.boundingBox.y);
                    maxY = std::max(maxY, point.y - glyph.boundingBox.y);
                }
            }
            
            profile[x] = maxY - minY;
        }
    } else {
        // Calculate vertical profile (top to bottom)
        int height = static_cast<int>(glyph.boundingBox.height);
        profile.resize(height);
        
        for (int y = 0; y < height; ++y) {
            float minX = glyph.boundingBox.width;
            float maxX = 0.0f;
            
            // Find min and max X at this Y position
            for (const auto& point : glyph.outlinePoints) {
                if (std::abs(point.y - glyph.boundingBox.y - y) < 1.0f) {
                    minX = std::min(minX, point.x - glyph.boundingBox.x);
                    maxX = std::max(maxX, point.x - glyph.boundingBox.x);
                }
            }
            
            profile[y] = maxX - minX;
        }
    }
    
    return profile;
}

std::vector<KerningExtractorImpl::ContextualKerningRule> KerningExtractorImpl::extractContextualRules(
    const FontFace& font, const KerningSettings& settings) {
    
    std::vector<ContextualKerningRule> rules;
    
    // Extract common contextual patterns (simplified)
    for (const auto& leftGlyph : font.glyphs) {
        for (const auto& centerGlyph : font.glyphs) {
            for (const auto& rightGlyph : font.glyphs) {
                float kerningAmount = calculateContextualKerning(
                    leftGlyph.index, centerGlyph.index, rightGlyph.index, font);
                
                if (isKerningPairSignificant({leftGlyph.index, rightGlyph.index, kerningAmount}, settings)) {
                    ContextualKerningRule rule;
                    rule.leftGlyph = leftGlyph.index;
                    rule.centerGlyph = centerGlyph.index;
                    rule.rightGlyph = rightGlyph.index;
                    rule.kerningAmount = kerningAmount;
                    rule.context = "triplet";
                    rules.push_back(rule);
                }
            }
        }
    }
    
    return rules;
}

float KerningExtractorImpl::calculateContextualKerning(uint32_t left, uint32_t center, uint32_t right, 
                                                     const FontFace& font) {
    // Simplified contextual kerning calculation
    float baseKerning = 0.0f;
    
    // Find base kerning for left-center pair
    for (const auto& pair : font.kerningPairs) {
        if (pair.leftGlyph == left && pair.rightGlyph == center) {
            baseKerning = pair.kerningAmount;
            break;
        }
    }
    
    // Adjust based on right glyph context
    float contextAdjustment = 0.0f;
    
    // Simple heuristic: adjust based on right glyph shape
    if (right < font.glyphs.size()) {
        const auto& rightGlyph = font.glyphs[right];
        // Adjust based on right glyph width
        contextAdjustment = rightGlyph.boundingBox.width * 0.1f;
    }
    
    return baseKerning + contextAdjustment;
}

KerningExtractorImpl::GlyphShape KerningExtractorImpl::analyzeGlyphShape(const Glyph& glyph) {
    GlyphShape shape;
    
    shape.boundingBoxWidth = glyph.boundingBox.width;
    shape.boundingBoxHeight = glyph.boundingBox.height;
    
    // Calculate profiles
    shape.leftProfile = calculateGlyphProfile(glyph, false);
    shape.rightProfile = calculateGlyphProfile(glyph, false);
    shape.topProfile = calculateGlyphProfile(glyph, true);
    shape.bottomProfile = calculateGlyphProfile(glyph, true);
    
    return shape;
}

float KerningExtractorImpl::calculateProfileDistance(const std::vector<float>& profile1, 
                                                  const std::vector<float>& profile2) {
    // Calculate distance between two profiles
    float distance = 0.0f;
    int comparisons = std::min(profile1.size(), profile2.size());
    
    for (int i = 0; i < comparisons; ++i) {
        distance += std::abs(profile1[i] - profile2[i]);
    }
    
    return comparisons > 0 ? distance / comparisons : 0.0f;
}

bool KerningExtractorImpl::isKerningPairSignificant(const KerningPair& pair, const KerningSettings& settings) {
    return std::abs(pair.kerningAmount) >= settings.minKerningAmount;
}

float KerningExtractorImpl::normalizeKerningAmount(float amount, float unitsPerEm) {
    return amount / unitsPerEm;
}

uint32_t KerningExtractorImpl::getGlyphIndex(const FontFace& font, uint32_t characterCode) {
    for (const auto& glyph : font.glyphs) {
        if (glyph.characterCode == characterCode) {
            return glyph.index;
        }
    }
    return 0;
}

KerningExtractorImpl::GlyphShape KerningExtractorImpl::getCachedGlyphShape(const FontFace& font, uint32_t glyphIndex) {
    auto it = glyphShapeCache_.find(glyphIndex);
    if (it != glyphShapeCache_.end()) {
        return it->second;
    }
    
    // Find glyph by index
    for (const auto& glyph : font.glyphs) {
        if (glyph.index == glyphIndex) {
            GlyphShape shape = analyzeGlyphShape(glyph);
            glyphShapeCache_[glyphIndex] = shape;
            return shape;
        }
    }
    
    // Return empty shape if not found
    return GlyphShape{};
}

// KerningExtractor interface implementation
KerningExtractor::KerningExtractor() : impl_(std::make_unique<KerningExtractorImpl>()) {}

KerningExtractor::~KerningExtractor() = default;

std::vector<KerningPair> KerningExtractor::extractKerningPairs(const FontFace& font) {
    KerningExtractorImpl::KerningSettings settings;
    return impl_->extractKerningPairs(font, settings);
}

std::vector<KerningPair> KerningExtractor::extractOpticalKerning(const FontFace& font) {
    KerningExtractorImpl::KerningSettings settings;
    return impl_->extractOpticalKerning(font, settings);
}

std::vector<KerningPair> KerningExtractor::extractContextualKerning(const FontFace& font) {
    KerningExtractorImpl::KerningSettings settings;
    return impl_->extractContextualKerning(font, settings);
}

} // namespace FontBaker