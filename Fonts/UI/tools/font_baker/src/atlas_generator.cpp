#include "baker.h"
#include "rasterizer.h"
#include <vector>
#include <algorithm>
#include <cmath>

namespace FontBaker {

class AtlasGeneratorImpl {
public:
    struct AtlasNode {
        int x, y;
        int width, height;
        bool used;
        std::unique_ptr<AtlasNode> right;
        std::unique_ptr<AtlasNode> down;
    };
    
    struct GlyphPlacement {
        int x, y;
        int width, height;
        uint32_t glyphIndex;
        bool rotated;
    };
    
    enum class PackingAlgorithm {
        BEST_FIT,
        TOP_LEFT,
        BOTTOM_LEFT,
        MAX_RECTS,
        SKYLINE
    };
    
    struct AtlasSettings {
        int maxWidth = 2048;
        int maxHeight = 2048;
        int padding = 2;
        bool enableRotation = false;
        bool forcePowerOfTwo = true;
        PackingAlgorithm algorithm = PackingAlgorithm::BEST_FIT;
        bool allowOverflow = false;
        int borderSize = 0;
    };
    
    AtlasGeneratorImpl();
    ~AtlasGeneratorImpl() = default;
    
    TextureAtlas generateAtlas(const std::vector<Glyph>& glyphs, const AtlasSettings& settings);
    
private:
    std::unique_ptr<AtlasNode> createNode(int x, int y, int width, int height);
    bool insertNode(std::unique_ptr<AtlasNode>& root, int width, int height, int& x, int& y);
    
    // Packing algorithms
    std::vector<GlyphPlacement> packWithBestFit(const std::vector<Glyph>& glyphs, const AtlasSettings& settings);
    std::vector<GlyphPlacement> packWithSkyline(const std::vector<Glyph>& glyphs, const AtlasSettings& settings);
    std::vector<GlyphPlacement> packWithMaxRects(const std::vector<Glyph>& glyphs, const AtlasSettings& settings);
    
    // Skyline algorithm helpers
    struct SkylineNode {
        int x, y, width;
    };
    
    std::vector<SkylineNode> skyline_;
    int skylineWidth_;
    
    bool skylineInsertRectangle(int width, int height, int& bestX, int& bestY, int& bestHeight);
    void updateSkyline(int x, int y, int width, int height);
    
    // MaxRects algorithm helpers
    struct Rectangle {
        int x, y, width, height;
        bool rotated;
    };
    
    std::vector<Rectangle> freeRects_;
    
    void initFreeRects(int atlasWidth, int atlasHeight);
    void placeRectangle(const Rectangle& rect);
    std::vector<Rectangle> findFreeRectsForRectangle(int width, int height);
    int getCommonArea(const Rectangle& a, const Rectangle& b);
    void pruneFreeRects();
    
    // Utility functions
    bool isPowerOfTwo(int value);
    int nextPowerOfTwo(int value);
    void applyPadding(std::vector<GlyphPlacement>& placements, int padding);
    void applyBorder(std::vector<uint8_t>& atlasData, int width, int height, int borderSize);
    
    // Atlas generation
    void renderGlyphsToAtlas(const std::vector<Glyph>& glyphs, 
                            const std::vector<GlyphPlacement>& placements,
                            std::vector<uint8_t>& atlasData, int atlasWidth, int atlasHeight);
    
    // Optimization
    void optimizeAtlas(std::vector<uint8_t>& atlasData, int width, int height);
    void removeDuplicateGlyphs(std::vector<Glyph>& glyphs);
};

AtlasGeneratorImpl::AtlasGeneratorImpl() : skylineWidth_(0) {}

TextureAtlas AtlasGeneratorImpl::generateAtlas(const std::vector<Glyph>& glyphs, const AtlasSettings& settings) {
    TextureAtlas atlas;
    
    if (glyphs.empty()) {
        return atlas;
    }
    
    // Remove duplicate glyphs
    std::vector<Glyph> uniqueGlyphs = glyphs;
    removeDuplicateGlyphs(uniqueGlyphs);
    
    // Pack glyphs
    std::vector<GlyphPlacement> placements;
    
    switch (settings.algorithm) {
        case PackingAlgorithm::BEST_FIT:
            placements = packWithBestFit(uniqueGlyphs, settings);
            break;
        case PackingAlgorithm::SKYLINE:
            placements = packWithSkyline(uniqueGlyphs, settings);
            break;
        case PackingAlgorithm::MAX_RECTS:
            placements = packWithMaxRects(uniqueGlyphs, settings);
            break;
        default:
            placements = packWithBestFit(uniqueGlyphs, settings);
            break;
    }
    
    if (placements.empty()) {
        return atlas;
    }
    
    // Calculate atlas dimensions
    int atlasWidth = 0, atlasHeight = 0;
    for (const auto& placement : placements) {
        atlasWidth = std::max(atlasWidth, placement.x + placement.width);
        atlasHeight = std::max(atlasHeight, placement.y + placement.height);
    }
    
    // Apply power of two constraint if needed
    if (settings.forcePowerOfTwo) {
        atlasWidth = nextPowerOfTwo(atlasWidth);
        atlasHeight = nextPowerOfTwo(atlasHeight);
    }
    
    // Apply padding
    if (settings.padding > 0) {
        applyPadding(placements, settings.padding);
        // Recalculate dimensions with padding
        atlasWidth = 0;
        atlasHeight = 0;
        for (const auto& placement : placements) {
            atlasWidth = std::max(atlasWidth, placement.x + placement.width);
            atlasHeight = std::max(atlasHeight, placement.y + placement.height);
        }
    }
    
    // Create atlas data
    std::vector<uint8_t> atlasData(atlasWidth * atlasHeight, 0);
    
    // Render glyphs to atlas
    renderGlyphsToAtlas(uniqueGlyphs, placements, atlasData, atlasWidth, atlasHeight);
    
    // Apply border if needed
    if (settings.borderSize > 0) {
        applyBorder(atlasData, atlasWidth, atlasHeight, settings.borderSize);
    }
    
    // Optimize atlas
    optimizeAtlas(atlasData, atlasWidth, atlasHeight);
    
    // Set atlas properties
    atlas.width = atlasWidth;
    atlas.height = atlasHeight;
    atlas.channels = 1; // Grayscale
    atlas.data = std::move(atlasData);
    
    // Update glyph UV coordinates
    atlas.glyphs.resize(uniqueGlyphs.size());
    for (size_t i = 0; i < placements.size(); ++i) {
        const auto& placement = placements[i];
        if (placement.glyphIndex < uniqueGlyphs.size()) {
            auto& atlasGlyph = atlas.glyphs[placement.glyphIndex];
            atlasGlyph = uniqueGlyphs[placement.glyphIndex];
            
            // Calculate UV coordinates
            atlasGlyph.uvX = static_cast<float>(placement.x) / atlasWidth;
            atlasGlyph.uvY = static_cast<float>(placement.y) / atlasHeight;
            atlasGlyph.uvWidth = static_cast<float>(placement.width) / atlasWidth;
            atlasGlyph.uvHeight = static_cast<float>(placement.height) / atlasHeight;
        }
    }
    
    return atlas;
}

std::unique_ptr<AtlasGeneratorImpl::AtlasNode> AtlasGeneratorImpl::createNode(int x, int y, int width, int height) {
    auto node = std::make_unique<AtlasNode>();
    node->x = x;
    node->y = y;
    node->width = width;
    node->height = height;
    node->used = false;
    return node;
}

bool AtlasGeneratorImpl::insertNode(std::unique_ptr<AtlasNode>& root, int width, int height, int& x, int& y) {
    if (!root) {
        return false;
    }
    
    if (root->used) {
        // Try to insert in right child first
        if (insertNode(root->right, width, height, x, y)) {
            return true;
        }
        return insertNode(root->down, width, height, x, y);
    }
    
    // Check if rectangle fits in this node
    if (width > root->width || height > root->height) {
        return false;
    }
    
    // Mark this node as used
    root->used = true;
    
    // Create children
    root->right = createNode(root->x + width, root->y, root->width - width, height);
    root->down = createNode(root->x, root->y + height, width, root->height - height);
    
    x = root->x;
    y = root->y;
    return true;
}

std::vector<AtlasGeneratorImpl::GlyphPlacement> AtlasGeneratorImpl::packWithBestFit(
    const std::vector<Glyph>& glyphs, const AtlasSettings& settings) {
    
    std::vector<GlyphPlacement> placements;
    
    // Sort glyphs by area (largest first) for better packing
    std::vector<std::pair<size_t, int>> sortedGlyphs;
    for (size_t i = 0; i < glyphs.size(); ++i) {
        int area = glyphs[i].width * glyphs[i].height;
        sortedGlyphs.emplace_back(i, area);
    }
    std::sort(sortedGlyphs.begin(), sortedGlyphs.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Try different atlas sizes
    int atlasWidth = settings.maxWidth;
    int atlasHeight = settings.maxHeight;
    
    // Start with a reasonable size
    int totalArea = 0;
    for (const auto& glyph : glyphs) {
        totalArea += glyph.width * glyph.height;
    }
    
    int estimatedSize = static_cast<int>(std::sqrt(totalArea) * 1.5f);
    atlasWidth = std::min(estimatedSize, settings.maxWidth);
    atlasHeight = std::min(estimatedSize, settings.maxHeight);
    
    // Try to pack with increasing atlas sizes
    while (atlasWidth <= settings.maxWidth && atlasHeight <= settings.maxHeight) {
        auto root = createNode(0, 0, atlasWidth, atlasHeight);
        placements.clear();
        bool success = true;
        
        for (const auto& sortedGlyph : sortedGlyphs) {
            const auto& glyph = glyphs[sortedGlyph.first];
            int x, y;
            
            if (insertNode(root, glyph.width, glyph.height, x, y)) {
                GlyphPlacement placement;
                placement.x = x;
                placement.y = y;
                placement.width = glyph.width;
                placement.height = glyph.height;
                placement.glyphIndex = static_cast<uint32_t>(sortedGlyph.first);
                placement.rotated = false;
                placements.push_back(placement);
            } else {
                success = false;
                break;
            }
        }
        
        if (success) {
            break;
        }
        
        // Increase atlas size
        if (atlasWidth < atlasHeight) {
            atlasWidth = std::min(atlasWidth * 2, settings.maxWidth);
        } else {
            atlasHeight = std::min(atlasHeight * 2, settings.maxHeight);
        }
    }
    
    return placements;
}

std::vector<AtlasGeneratorImpl::GlyphPlacement> AtlasGeneratorImpl::packWithSkyline(
    const std::vector<Glyph>& glyphs, const AtlasSettings& settings) {
    
    std::vector<GlyphPlacement> placements;
    
    // Initialize skyline
    skyline_.clear();
    skyline_.push_back({0, 0, settings.maxWidth});
    skylineWidth_ = settings.maxWidth;
    
    // Sort glyphs by height (tallest first)
    std::vector<std::pair<size_t, int>> sortedGlyphs;
    for (size_t i = 0; i < glyphs.size(); ++i) {
        sortedGlyphs.emplace_back(i, glyphs[i].height);
    }
    std::sort(sortedGlyphs.begin(), sortedGlyphs.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Place each glyph
    for (const auto& sortedGlyph : sortedGlyphs) {
        const auto& glyph = glyphs[sortedGlyph.first];
        int x, y, height;
        
        if (skylineInsertRectangle(glyph.width, glyph.height, x, y, height)) {
            updateSkyline(x, y, glyph.width, glyph.height);
            
            GlyphPlacement placement;
            placement.x = x;
            placement.y = y;
            placement.width = glyph.width;
            placement.height = glyph.height;
            placement.glyphIndex = static_cast<uint32_t>(sortedGlyph.first);
            placement.rotated = false;
            placements.push_back(placement);
        }
    }
    
    return placements;
}

std::vector<AtlasGeneratorImpl::GlyphPlacement> AtlasGeneratorImpl::packWithMaxRects(
    const std::vector<Glyph>& glyphs, const AtlasSettings& settings) {
    
    std::vector<GlyphPlacement> placements;
    
    // Initialize free rectangles
    initFreeRects(settings.maxWidth, settings.maxHeight);
    
    // Sort glyphs by area (largest first)
    std::vector<std::pair<size_t, int>> sortedGlyphs;
    for (size_t i = 0; i < glyphs.size(); ++i) {
        int area = glyphs[i].width * glyphs[i].height;
        sortedGlyphs.emplace_back(i, area);
    }
    std::sort(sortedGlyphs.begin(), sortedGlyphs.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Place each glyph
    for (const auto& sortedGlyph : sortedGlyphs) {
        const auto& glyph = glyphs[sortedGlyph.first];
        
        // Find best position for this glyph
        int bestX = 0, bestY = 0;
        int bestArea = settings.maxWidth * settings.maxHeight;
        bool found = false;
        
        for (const auto& freeRect : freeRects_) {
            if (freeRect.width >= glyph.width && freeRect.height >= glyph.height) {
                int area = freeRect.width * freeRect.height;
                if (area < bestArea) {
                    bestX = freeRect.x;
                    bestY = freeRect.y;
                    bestArea = area;
                    found = true;
                }
            }
        }
        
        if (found) {
            Rectangle placedRect;
            placedRect.x = bestX;
            placedRect.y = bestY;
            placedRect.width = glyph.width;
            placedRect.height = glyph.height;
            placedRect.rotated = false;
            
            placeRectangle(placedRect);
            
            GlyphPlacement placement;
            placement.x = bestX;
            placement.y = bestY;
            placement.width = glyph.width;
            placement.height = glyph.height;
            placement.glyphIndex = static_cast<uint32_t>(sortedGlyph.first);
            placement.rotated = false;
            placements.push_back(placement);
        }
    }
    
    return placements;
}

bool AtlasGeneratorImpl::skylineInsertRectangle(int width, int height, int& bestX, int& bestY, int& bestHeight) {
    bestX = -1;
    bestY = -1;
    bestHeight = std::numeric_limits<int>::max();
    
    for (size_t i = 0; i < skyline_.size(); ++i) {
        int x = skyline_[i].x;
        int y = skyline_[i].y;
        
        if (x + width > skylineWidth_) {
            continue;
        }
        
        // Check if rectangle fits at this position
        int minY = y;
        for (size_t j = 0; j < i; ++j) {
            if (skyline_[j].x < x + width && skyline_[j].x + skyline_[j].width > x) {
                minY = std::max(minY, skyline_[j].y + skyline_[j].height);
            }
        }
        
        if (minY + height <= settings.maxHeight) {
            if (minY + height < bestHeight || (minY + height == bestHeight && x < bestX)) {
                bestX = x;
                bestY = minY;
                bestHeight = minY + height;
            }
        }
    }
    
    return bestX != -1;
}

void AtlasGeneratorImpl::updateSkyline(int x, int y, int width, int height) {
    // Find the skyline segment that contains this x position
    size_t i = 0;
    while (i < skyline_.size() && skyline_[i].x < x) {
        ++i;
    }
    
    // Insert new skyline node
    SkylineNode newNode;
    newNode.x = x;
    newNode.y = y + height;
    newNode.width = width;
    skyline_.insert(skyline_.begin() + i, newNode);
    
    // Merge overlapping skyline nodes
    for (size_t j = 0; j < skyline_.size() - 1; ++j) {
        if (skyline_[j].y == skyline_[j + 1].y) {
            skyline_[j].width += skyline_[j + 1].width;
            skyline_.erase(skyline_.begin() + j + 1);
            --j;
        }
    }
}

void AtlasGeneratorImpl::initFreeRects(int atlasWidth, int atlasHeight) {
    freeRects_.clear();
    freeRects_.push_back({0, 0, atlasWidth, atlasHeight, false});
}

void AtlasGeneratorImpl::placeRectangle(const Rectangle& rect) {
    // Split the free rectangle
    std::vector<Rectangle> newFreeRects;
    
    for (const auto& freeRect : freeRects_) {
        if (freeRect.x < rect.x + rect.width && freeRect.x + freeRect.width > rect.x &&
            freeRect.y < rect.y + rect.height && freeRect.y + freeRect.height > rect.y) {
            
            // Split the free rectangle
            if (rect.x > freeRect.x) {
                newFreeRects.push_back({freeRect.x, freeRect.y, rect.x - freeRect.x, freeRect.height, false});
            }
            if (rect.y > freeRect.y) {
                newFreeRects.push_back({freeRect.x, freeRect.y, freeRect.width, rect.y - freeRect.y, false});
            }
            if (rect.x + rect.width < freeRect.x + freeRect.width) {
                newFreeRects.push_back({rect.x + rect.width, freeRect.y, 
                                       (freeRect.x + freeRect.width) - (rect.x + rect.width), 
                                       freeRect.height, false});
            }
            if (rect.y + rect.height < freeRect.y + freeRect.height) {
                newFreeRects.push_back({freeRect.x, rect.y + rect.height, 
                                       freeRect.width, 
                                       (freeRect.y + freeRect.height) - (rect.y + rect.height), 
                                       false});
            }
        } else {
            newFreeRects.push_back(freeRect);
        }
    }
    
    freeRects_ = newFreeRects;
    pruneFreeRects();
}

void AtlasGeneratorImpl::pruneFreeRects() {
    // Remove contained rectangles
    for (size_t i = 0; i < freeRects_.size(); ++i) {
        for (size_t j = i + 1; j < freeRects_.size(); ++j) {
            if (getCommonArea(freeRects_[i], freeRects_[j]) == 
                freeRects_[j].width * freeRects_[j].height) {
                freeRects_.erase(freeRects_.begin() + j);
                --j;
            } else if (getCommonArea(freeRects_[i], freeRects_[j]) == 
                       freeRects_[i].width * freeRects_[i].height) {
                freeRects_.erase(freeRects_.begin() + i);
                --i;
                break;
            }
        }
    }
}

int AtlasGeneratorImpl::getCommonArea(const Rectangle& a, const Rectangle& b) {
    int x1 = std::max(a.x, b.x);
    int y1 = std::max(a.y, b.y);
    int x2 = std::min(a.x + a.width, b.x + b.width);
    int y2 = std::min(a.y + a.height, b.y + b.height);
    
    if (x2 <= x1 || y2 <= y1) {
        return 0;
    }
    
    return (x2 - x1) * (y2 - y1);
}

bool AtlasGeneratorImpl::isPowerOfTwo(int value) {
    return value > 0 && (value & (value - 1)) == 0;
}

int AtlasGeneratorImpl::nextPowerOfTwo(int value) {
    if (isPowerOfTwo(value)) {
        return value;
    }
    
    int power = 1;
    while (power < value) {
        power <<= 1;
    }
    return power;
}

void AtlasGeneratorImpl::applyPadding(std::vector<GlyphPlacement>& placements, int padding) {
    for (auto& placement : placements) {
        placement.x += padding;
        placement.y += padding;
        placement.width += padding * 2;
        placement.height += padding * 2;
    }
}

void AtlasGeneratorImpl::applyBorder(std::vector<uint8_t>& atlasData, int width, int height, int borderSize) {
    std::vector<uint8_t> original = atlasData;
    atlasData.assign(width * height, 0);
    
    for (int y = borderSize; y < height - borderSize; ++y) {
        for (int x = borderSize; x < width - borderSize; ++x) {
            atlasData[y * width + x] = original[(y - borderSize) * (width - borderSize * 2) + (x - borderSize)];
        }
    }
}

void AtlasGeneratorImpl::renderGlyphsToAtlas(const std::vector<Glyph>& glyphs, 
                                         const std::vector<GlyphPlacement>& placements,
                                         std::vector<uint8_t>& atlasData, int atlasWidth, int atlasHeight) {
    for (const auto& placement : placements) {
        if (placement.glyphIndex < glyphs.size()) {
            const auto& glyph = glyphs[placement.glyphIndex];
            
            // Copy glyph bitmap to atlas
            for (int y = 0; y < glyph.height; ++y) {
                for (int x = 0; x < glyph.width; ++x) {
                    int srcX = x;
                    int srcY = y;
                    int dstX = placement.x + x;
                    int dstY = placement.y + y;
                    
                    if (dstX >= 0 && dstX < atlasWidth && dstY >= 0 && dstY < atlasHeight) {
                        // Assuming glyph data is available (would be rasterized beforehand)
                        atlasData[dstY * atlasWidth + dstX] = 255; // Placeholder
                    }
                }
            }
        }
    }
}

void AtlasGeneratorImpl::optimizeAtlas(std::vector<uint8_t>& atlasData, int width, int height) {
    // Simple optimization: remove isolated pixels
    std::vector<uint8_t> original = atlasData;
    
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            int idx = y * width + x;
            if (original[idx] > 0) {
                int neighbors = 0;
                neighbors += original[(y-1) * width + (x-1)] > 0;
                neighbors += original[(y-1) * width + x] > 0;
                neighbors += original[(y-1) * width + (x+1)] > 0;
                neighbors += original[y * width + (x-1)] > 0;
                neighbors += original[y * width + (x+1)] > 0;
                neighbors += original[(y+1) * width + (x-1)] > 0;
                neighbors += original[(y+1) * width + x] > 0;
                neighbors += original[(y+1) * width + (x+1)] > 0;
                
                // Remove isolated pixels
                if (neighbors < 2) {
                    atlasData[idx] = 0;
                }
            }
        }
    }
}

void AtlasGeneratorImpl::removeDuplicateGlyphs(std::vector<Glyph>& glyphs) {
    // Simple duplicate removal based on character code
    std::map<uint32_t, size_t> charToIndex;
    std::vector<Glyph> uniqueGlyphs;
    
    for (size_t i = 0; i < glyphs.size(); ++i) {
        auto it = charToIndex.find(glyphs[i].characterCode);
        if (it == charToIndex.end()) {
            charToIndex[glyphs[i].characterCode] = uniqueGlyphs.size();
            uniqueGlyphs.push_back(glyphs[i]);
        }
    }
    
    glyphs = std::move(uniqueGlyphs);
}

// AtlasGenerator interface implementation
AtlasGenerator::AtlasGenerator() : impl_(std::make_unique<AtlasGeneratorImpl>()) {}

AtlasGenerator::~AtlasGenerator() = default;

TextureAtlas AtlasGenerator::generateAtlas(const std::vector<Glyph>& glyphs, 
                                       int maxWidth, int maxHeight, int padding) {
    AtlasGeneratorImpl::AtlasSettings settings;
    settings.maxWidth = maxWidth;
    settings.maxHeight = maxHeight;
    settings.padding = padding;
    return impl_->generateAtlas(glyphs, settings);
}

TextureAtlas AtlasGenerator::generateAtlasWithRotation(const std::vector<Glyph>& glyphs, 
                                                   int maxWidth, int maxHeight, int padding) {
    AtlasGeneratorImpl::AtlasSettings settings;
    settings.maxWidth = maxWidth;
    settings.maxHeight = maxHeight;
    settings.padding = padding;
    settings.enableRotation = true;
    return impl_->generateAtlas(glyphs, settings);
}

} // namespace FontBaker