#include <vector>
#include <algorithm>
#include <cmath>
#include <climits>

namespace TexturePacker {

class PackerAlgorithm {
public:
    struct Rectangle {
        int x, y, width, height;
        bool rotated;
        int textureID;
        
        Rectangle() : x(0), y(0), width(0), height(0), rotated(false), textureID(-1) {}
        Rectangle(int x_, int y_, int w_, int h_, int id) 
            : x(x_), y(y_), width(w_), height(h_), rotated(false), textureID(id) {}
    };
    
    struct PackResult {
        std::vector<Rectangle> rectangles;
        int atlasWidth, atlasHeight;
        float efficiency;
        int totalArea;
        int usedArea;
    };
    
    // Guillotine cutting algorithm
    static PackResult guillotinePack(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight, bool rotate);
    
    // MaxRects algorithm
    static PackResult maxRectsPack(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight, bool rotate);
    
    // Skyline algorithm
    static PackResult skylinePack(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight, bool rotate);
    
    // Shelf algorithm
    static PackResult shelfPack(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight, bool rotate);
    
    // Best-fit algorithm
    static PackResult bestFitPack(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight, bool rotate);
    
private:
    // Helper functions
    static bool fits(const Rectangle& a, const Rectangle& b);
    static int compareArea(const Rectangle& a, const Rectangle& b);
    static int compareMaxSide(const Rectangle& a, const Rectangle& b);
    static int compareMaxWidth(const Rectangle& a, const Rectangle& b);
    static int compareMaxHeight(const Rectangle& a, const Rectangle& b);
    
    // Guillotine helpers
    struct FreeRect {
        int x, y, width, height;
        FreeRect(int x_, int y_, int w_, int h_) : x(x_), y(y_), width(w_), height(h_) {}
    };
    
    struct Node {
        Rectangle rect;
        std::unique_ptr<Node> right;
        std::unique_ptr<Node> down;
        Node(const Rectangle& r) : rect(r), right(nullptr), down(nullptr) {}
    };
    
    static std::vector<Rectangle> guillotineSplit(const Rectangle& rect, const std::vector<FreeRect>& freeRects);
    static std::vector<FreeRect> splitFreeRect(const FreeRect& freeRect, const Rectangle& rect);
    static bool guillotineInsert(std::unique_ptr<Node>& root, const Rectangle& rect);
    
    // MaxRects helpers
    static std::vector<Rectangle> findFreeRectsForRect(const Rectangle& rect, const std::vector<Rectangle>& freeRects);
    static void pruneFreeRects(std::vector<Rectangle>& freeRects);
    static int commonArea(const Rectangle& a, const Rectangle& b);
    
    // Skyline helpers
    struct SkylineNode {
        int x, y, width;
        SkylineNode(int x_, int y_, int w_) : x(x_), y(y_), width(w_) {}
    };
    
    static std::vector<Rectangle> skylineInsert(const std::vector<Rectangle>& rects, 
                                           std::vector<SkylineNode>& skyline, 
                                           int width, int height, int rectIndex);
    static void updateSkyline(std::vector<SkylineNode>& skyline, const Rectangle& rect);
    
    // Shelf helpers
    struct Shelf {
        int x, y, width, height;
        std::vector<int> rectIndices;
        Shelf(int x_, int y_, int w_, int h_) : x(x_), y(y_), width(w_), height(h_) {}
    };
    
    static std::vector<Rectangle> shelfPack(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight);
};

PackerAlgorithm::PackResult PackerAlgorithm::guillotinePack(const std::vector<Rectangle>& rects, 
                                                          int maxWidth, int maxHeight, bool rotate) {
    PackResult result;
    result.atlasWidth = maxWidth;
    result.atlasHeight = maxHeight;
    
    // Sort rectangles by area (largest first)
    std::vector<Rectangle> sortedRects = rects;
    std::sort(sortedRects.begin(), sortedRects.end(), compareArea);
    
    std::unique_ptr<Node> root;
    std::vector<FreeRect> freeRects;
    freeRects.emplace_back(0, 0, maxWidth, maxHeight);
    
    for (const auto& rect : sortedRects) {
        Rectangle bestRect = rect;
        bool bestRotated = false;
        
        if (rotate) {
            // Try both orientations
            Rectangle rotatedRect(rect.x, rect.y, rect.height, rect.width, rect.textureID);
            if (guillotineInsert(root, rotatedRect)) {
                bestRect = rotatedRect;
                bestRotated = true;
            }
        }
        
        if (guillotineInsert(root, bestRect)) {
            bestRect.rotated = bestRotated;
            result.rectangles.push_back(bestRect);
        }
    }
    
    // Calculate efficiency
    result.totalArea = maxWidth * maxHeight;
    result.usedArea = 0;
    for (const auto& rect : result.rectangles) {
        result.usedArea += rect.width * rect.height;
    }
    result.efficiency = static_cast<float>(result.usedArea) / result.totalArea;
    
    return result;
}

PackerAlgorithm::PackResult PackerAlgorithm::maxRectsPack(const std::vector<Rectangle>& rects, 
                                                         int maxWidth, int maxHeight, bool rotate) {
    PackResult result;
    result.atlasWidth = maxWidth;
    result.atlasHeight = maxHeight;
    
    // Sort rectangles by max side (largest first)
    std::vector<Rectangle> sortedRects = rects;
    std::sort(sortedRects.begin(), sortedRects.end(), compareMaxSide);
    
    std::vector<Rectangle> freeRects;
    freeRects.emplace_back(0, 0, maxWidth, maxHeight);
    
    for (const auto& rect : sortedRects) {
        std::vector<Rectangle> bestFits = findFreeRectsForRect(rect, freeRects);
        
        if (!bestFits.empty()) {
            Rectangle bestFit = bestFits[0];
            bestFit.rotated = false;
            
            if (rotate) {
                Rectangle rotatedRect(rect.x, rect.y, rect.height, rect.width, rect.textureID);
                std::vector<Rectangle> rotatedFits = findFreeRectsForRect(rotatedRect, freeRects);
                
                if (!rotatedFits.empty() && 
                    (rotatedFits[0].width * rotatedFits[0].height < bestFit.width * bestFit.height)) {
                    bestFit = rotatedFits[0];
                    bestFit.rotated = true;
                }
            }
            
            result.rectangles.push_back(bestFit);
            
            // Split the free rectangle
            for (const auto& freeRect : freeRects) {
                if (commonArea(bestFit, freeRect) == bestFit.width * bestFit.height) {
                    auto newFreeRects = splitFreeRect(freeRect, bestFit);
                    freeRects.erase(std::remove(freeRects.begin(), freeRects.end(), freeRect), 
                                   freeRects.end());
                    freeRects.insert(freeRects.end(), newFreeRects.begin(), newFreeRects.end());
                    break;
                }
            }
            
            pruneFreeRects(freeRects);
        }
    }
    
    // Calculate efficiency
    result.totalArea = maxWidth * maxHeight;
    result.usedArea = 0;
    for (const auto& rect : result.rectangles) {
        result.usedArea += rect.width * rect.height;
    }
    result.efficiency = static_cast<float>(result.usedArea) / result.totalArea;
    
    return result;
}

PackerAlgorithm::PackResult PackerAlgorithm::skylinePack(const std::vector<Rectangle>& rects, 
                                                        int maxWidth, int maxHeight, bool rotate) {
    PackResult result;
    result.atlasWidth = maxWidth;
    result.atlasHeight = maxHeight;
    
    // Sort rectangles by height (tallest first)
    std::vector<Rectangle> sortedRects = rects;
    std::sort(sortedRects.begin(), sortedRects.end(), compareMaxHeight);
    
    std::vector<SkylineNode> skyline;
    skyline.emplace_back(0, 0, maxWidth);
    
    for (size_t i = 0; i < sortedRects.size(); ++i) {
        auto packedRects = skylineInsert(sortedRects, skyline, maxWidth, maxHeight, static_cast<int>(i));
        
        if (!packedRects.empty()) {
            result.rectangles.push_back(packedRects[0]);
            updateSkyline(skyline, packedRects[0]);
        }
    }
    
    // Calculate efficiency
    result.totalArea = maxWidth * maxHeight;
    result.usedArea = 0;
    for (const auto& rect : result.rectangles) {
        result.usedArea += rect.width * rect.height;
    }
    result.efficiency = static_cast<float>(result.usedArea) / result.totalArea;
    
    return result;
}

PackerAlgorithm::PackResult PackerAlgorithm::shelfPack(const std::vector<Rectangle>& rects, 
                                                      int maxWidth, int maxHeight, bool rotate) {
    PackResult result;
    result.atlasWidth = maxWidth;
    result.atlasHeight = maxHeight;
    
    // Sort rectangles by width (widest first)
    std::vector<Rectangle> sortedRects = rects;
    std::sort(sortedRects.begin(), sortedRects.end(), compareMaxWidth);
    
    std::vector<Shelf> shelves;
    shelves.emplace_back(0, 0, maxWidth, 0);
    
    for (const auto& rect : sortedRects) {
        bool placed = false;
        
        // Try to place on existing shelves
        for (auto& shelf : shelves) {
            if (shelf.x + rect.width <= maxWidth && shelf.y + shelf.height + rect.height <= maxHeight) {
                if (shelf.x + rect.width <= shelf.x + shelf.width) {
                    shelf.rectIndices.push_back(static_cast<int>(sortedRects.size()));
                    placed = true;
                    break;
                }
            }
        }
        
        // Try to create new shelf
        if (!placed && !shelves.empty()) {
            const auto& lastShelf = shelves.back();
            int newY = lastShelf.y + lastShelf.height;
            
            if (newY + rect.height <= maxHeight) {
                shelves.emplace_back(0, newY, maxWidth, rect.height);
                shelves.back().rectIndices.push_back(static_cast<int>(sortedRects.size()));
                placed = true;
            }
        }
        
        // First shelf
        if (!placed && shelves.empty()) {
            if (rect.height <= maxHeight) {
                shelves.emplace_back(0, 0, maxWidth, rect.height);
                shelves[0].rectIndices.push_back(static_cast<int>(sortedRects.size()));
                placed = true;
            }
        }
    }
    
    // Create result rectangles
    for (const auto& shelf : shelves) {
        int currentX = shelf.x;
        for (int rectIndex : shelf.rectIndices) {
            const auto& rect = sortedRects[rectIndex];
            Rectangle packedRect(currentX, shelf.y, rect.width, rect.height, rect.textureID);
            result.rectangles.push_back(packedRect);
            currentX += rect.width;
        }
    }
    
    // Calculate efficiency
    result.totalArea = maxWidth * maxHeight;
    result.usedArea = 0;
    for (const auto& rect : result.rectangles) {
        result.usedArea += rect.width * rect.height;
    }
    result.efficiency = static_cast<float>(result.usedArea) / result.totalArea;
    
    return result;
}

PackerAlgorithm::PackResult PackerAlgorithm::bestFitPack(const std::vector<Rectangle>& rects, 
                                                         int maxWidth, int maxHeight, bool rotate) {
    // Try all algorithms and pick the best result
    auto guillotineResult = guillotinePack(rects, maxWidth, maxHeight, rotate);
    auto maxRectsResult = maxRectsPack(rects, maxWidth, maxHeight, rotate);
    auto skylineResult = skylinePack(rects, maxWidth, maxHeight, rotate);
    auto shelfResult = shelfPack(rects, maxWidth, maxHeight, rotate);
    
    // Choose the result with best efficiency
    PackResult bestResult = guillotineResult;
    if (maxRectsResult.efficiency > bestResult.efficiency) bestResult = maxRectsResult;
    if (skylineResult.efficiency > bestResult.efficiency) bestResult = skylineResult;
    if (shelfResult.efficiency > bestResult.efficiency) bestResult = shelfResult;
    
    return bestResult;
}

bool PackerAlgorithm::fits(const Rectangle& a, const Rectangle& b) {
    return a.width <= b.width && a.height <= b.height;
}

int PackerAlgorithm::compareArea(const Rectangle& a, const Rectangle& b) {
    int areaA = a.width * a.height;
    int areaB = b.width * b.height;
    return areaB - areaA;
}

int PackerAlgorithm::compareMaxSide(const Rectangle& a, const Rectangle& b) {
    int maxA = std::max(a.width, a.height);
    int maxB = std::max(b.width, b.height);
    return maxB - maxA;
}

int PackerAlgorithm::compareMaxWidth(const Rectangle& a, const Rectangle& b) {
    return b.width - a.width;
}

int PackerAlgorithm::compareMaxHeight(const Rectangle& a, const Rectangle& b) {
    return b.height - a.height;
}

std::vector<PackerAlgorithm::Rectangle> PackerAlgorithm::guillotineSplit(const Rectangle& rect, 
                                                                    const std::vector<FreeRect>& freeRects) {
    std::vector<Rectangle> result;
    
    for (const auto& freeRect : freeRects) {
        if (rect.width <= freeRect.width && rect.height <= freeRect.height) {
            Rectangle placedRect(freeRect.x, freeRect.y, rect.width, rect.height, rect.textureID);
            result.push_back(placedRect);
        }
    }
    
    return result;
}

std::vector<PackerAlgorithm::FreeRect> PackerAlgorithm::splitFreeRect(const FreeRect& freeRect, const Rectangle& rect) {
    std::vector<FreeRect> result;
    
    // Split the free rectangle around the placed rectangle
    int rightWidth = freeRect.x + freeRect.width - (rect.x + rect.width);
    if (rightWidth > 0) {
        result.emplace_back(rect.x + rect.width, freeRect.y, rightWidth, freeRect.height);
    }
    
    int bottomHeight = freeRect.y + freeRect.height - (rect.y + rect.height);
    if (bottomHeight > 0) {
        result.emplace_back(freeRect.x, rect.y + rect.height, freeRect.width, bottomHeight);
    }
    
    return result;
}

bool PackerAlgorithm::guillotineInsert(std::unique_ptr<Node>& root, const Rectangle& rect) {
    if (!root) {
        root = std::make_unique<Node>(rect);
        return true;
    }
    
    if (fits(rect, root->rect)) {
        if (guillotineInsert(root->right, rect)) return true;
        if (guillotineInsert(root->down, rect)) return true;
    }
    
    return false;
}

std::vector<PackerAlgorithm::Rectangle> PackerAlgorithm::findFreeRectsForRect(const Rectangle& rect, 
                                                                       const std::vector<Rectangle>& freeRects) {
    std::vector<Rectangle> bestFits;
    int bestArea = INT_MAX;
    
    for (const auto& freeRect : freeRects) {
        if (rect.width <= freeRect.width && rect.height <= freeRect.height) {
            int area = freeRect.width * freeRect.height;
            if (area < bestArea) {
                bestArea = area;
                bestFits.clear();
                bestFits.push_back(Rectangle(freeRect.x, freeRect.y, rect.width, rect.height, rect.textureID));
            } else if (area == bestArea) {
                bestFits.push_back(Rectangle(freeRect.x, freeRect.y, rect.width, rect.height, rect.textureID));
            }
        }
    }
    
    return bestFits;
}

void PackerAlgorithm::pruneFreeRects(std::vector<Rectangle>& freeRects) {
    for (size_t i = 0; i < freeRects.size(); ++i) {
        for (size_t j = i + 1; j < freeRects.size(); ++j) {
            if (commonArea(freeRects[i], freeRects[j]) == freeRects[j].width * freeRects[j].height) {
                freeRects.erase(freeRects.begin() + j);
                --j;
            }
        }
    }
}

int PackerAlgorithm::commonArea(const Rectangle& a, const Rectangle& b) {
    int x1 = std::max(a.x, b.x);
    int y1 = std::max(a.y, b.y);
    int x2 = std::min(a.x + a.width, b.x + b.width);
    int y2 = std::min(a.y + a.height, b.y + b.height);
    
    if (x2 <= x1 || y2 <= y1) return 0;
    
    return (x2 - x1) * (y2 - y1);
}

std::vector<PackerAlgorithm::Rectangle> PackerAlgorithm::skylineInsert(const std::vector<Rectangle>& rects, 
                                                              std::vector<SkylineNode>& skyline, 
                                                              int width, int height, int rectIndex) {
    const auto& rect = rects[rectIndex];
    
    int bestX = -1, bestY = -1;
    int bestHeight = INT_MAX;
    
    // Find best position for this rectangle
    for (size_t i = 0; i < skyline.size(); ++i) {
        int x = skyline[i].x;
        int y = skyline[i].y;
        
        if (x + rect.width <= width && y + rect.height <= height) {
            if (y + rect.height < bestHeight || (y + rect.height == bestHeight && x < bestX)) {
                bestX = x;
                bestY = y;
                bestHeight = y + rect.height;
            }
        }
    }
    
    std::vector<Rectangle> result;
    if (bestX != -1) {
        Rectangle placedRect(bestX, bestY, rect.width, rect.height, rect.textureID);
        result.push_back(placedRect);
    }
    
    return result;
}

void PackerAlgorithm::updateSkyline(std::vector<SkylineNode>& skyline, const Rectangle& rect) {
    // Find the skyline segment that contains this rectangle
    size_t insertPos = 0;
    for (size_t i = 0; i < skyline.size(); ++i) {
        if (skyline[i].x <= rect.x && skyline[i].x + skyline[i].width >= rect.x + rect.width) {
            insertPos = i;
            break;
        }
    }
    
    // Insert new skyline node
    SkylineNode newNode(rect.x, rect.y + rect.height, rect.width);
    skyline.insert(skyline.begin() + insertPos, newNode);
    
    // Merge overlapping skyline nodes
    for (size_t i = 0; i < skyline.size() - 1; ++i) {
        if (skyline[i].y == skyline[i + 1].y) {
            skyline[i].width += skyline[i + 1].width;
            skyline.erase(skyline.begin() + i + 1);
            --i;
        }
    }
}

std::vector<PackerAlgorithm::Rectangle> PackerAlgorithm::shelfPack(const std::vector<Rectangle>& rects, 
                                                              int maxWidth, int maxHeight) {
    std::vector<Rectangle> result;
    
    // Sort rectangles by width (widest first)
    std::vector<Rectangle> sortedRects = rects;
    std::sort(sortedRects.begin(), sortedRects.end(), compareMaxWidth);
    
    std::vector<Shelf> shelves;
    shelves.emplace_back(0, 0, maxWidth, 0);
    
    for (size_t i = 0; i < sortedRects.size(); ++i) {
        const auto& rect = sortedRects[i];
        bool placed = false;
        
        // Try to place on existing shelves
        for (auto& shelf : shelves) {
            if (shelf.x + rect.width <= maxWidth && shelf.y + shelf.height + rect.height <= maxHeight) {
                if (shelf.x + rect.width <= shelf.x + shelf.width) {
                    shelf.rectIndices.push_back(static_cast<int>(i));
                    placed = true;
                    break;
                }
            }
        }
        
        // Try to create new shelf
        if (!placed && !shelves.empty()) {
            const auto& lastShelf = shelves.back();
            int newY = lastShelf.y + lastShelf.height;
            
            if (newY + rect.height <= maxHeight) {
                shelves.emplace_back(0, newY, maxWidth, rect.height);
                shelves.back().rectIndices.push_back(static_cast<int>(i));
                placed = true;
            }
        }
        
        // First shelf
        if (!placed && shelves.empty()) {
            if (rect.height <= maxHeight) {
                shelves.emplace_back(0, 0, maxWidth, rect.height);
                shelves[0].rectIndices.push_back(static_cast<int>(i));
                placed = true;
            }
        }
    }
    
    // Create result rectangles
    for (const auto& shelf : shelves) {
        int currentX = shelf.x;
        for (int rectIndex : shelf.rectIndices) {
            const auto& rect = sortedRects[rectIndex];
            Rectangle packedRect(currentX, shelf.y, rect.width, rect.height, rect.textureID);
            result.push_back(packedRect);
            currentX += rect.width;
        }
    }
    
    return result;
}

} // namespace TexturePacker