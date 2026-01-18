#include <vector>
#include <algorithm>
#include <cmath>
#include <climits>

namespace TexturePacker {

class RectanglePacker {
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
        std::string algorithmName;
    };
    
    // Rectangle packing algorithms
    static PackResult packRectanglesGreedy(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight);
    static PackResult packRectanglesBestFit(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight);
    static PackResult packRectanglesBottomLeft(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight);
    static PackResult packRectanglesTopLeft(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight);
    
    // Advanced algorithms
    static PackResult packRectanglesGuillotine(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight, bool rotate);
    static PackResult packRectanglesMaxRects(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight, bool rotate);
    static PackResult packRectanglesSkyline(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight, bool rotate);
    
    // Utility functions
    static bool fits(const Rectangle& rect, int width, int height);
    static int compareArea(const Rectangle& a, const Rectangle& b);
    static int compareWidth(const Rectangle& a, const Rectangle& b);
    static int compareHeight(const Rectangle& a, const Rectangle& b);
    static int compareMaxSide(const Rectangle& a, const Rectangle& b);
    static int comparePerimeter(const Rectangle& a, const Rectangle& b);
    
    // Optimization
    static void optimizePacking(std::vector<Rectangle>& rectangles, int maxWidth, int maxHeight);
    static void removeEmptyRectangles(std::vector<Rectangle>& rectangles);
    static void sortRectangles(std::vector<Rectangle>& rectangles, int sortMethod);
    
private:
    // Guillotine algorithm helpers
    struct FreeRectangle {
        int x, y, width, height;
        FreeRectangle(int x_, int y_, int w_, int h_) : x(x_), y(y_), width(w_), height(h_) {}
    };
    
    struct Node {
        Rectangle rect;
        std::unique_ptr<Node> right;
        std::unique_ptr<Node> down;
        Node(const Rectangle& r) : rect(r), right(nullptr), down(nullptr) {}
    };
    
    static bool guillotineInsert(std::unique_ptr<Node>& root, const Rectangle& rect, int maxWidth, int maxHeight);
    static std::vector<FreeRectangle> splitFreeRectangle(const FreeRectangle& freeRect, const Rectangle& rect);
    static std::vector<Rectangle> guillotineFindBestFit(const Rectangle& rect, const std::vector<FreeRectangle>& freeRects);
    
    // MaxRects algorithm helpers
    static std::vector<Rectangle> findFreeRectsForRect(const Rectangle& rect, const std::vector<Rectangle>& freeRects);
    static void pruneFreeRects(std::vector<Rectangle>& freeRects);
    static int commonArea(const Rectangle& a, const Rectangle& b);
    
    // Skyline algorithm helpers
    struct SkylineNode {
        int x, y, width;
        SkylineNode(int x_, int y_, int w_) : x(x_), y(y_), width(w_) {}
    };
    
    static std::vector<Rectangle> skylineInsert(const std::vector<Rectangle>& rects, std::vector<SkylineNode>& skyline, 
                                           int width, int height, int rectIndex);
    static void updateSkyline(std::vector<SkylineNode>& skyline, const Rectangle& rect);
    
    // Shelf algorithm helpers
    struct Shelf {
        int x, y, width, height;
        std::vector<int> rectIndices;
        Shelf(int x_, int y_, int w_, int h_) : x(x_), y(y_), width(w_), height(h_) {}
    };
    
    static std::vector<Rectangle> shelfPack(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight, bool rotate);
    
    // Sorting methods
    enum SortMethod {
        SORT_AREA,
        SORT_WIDTH,
        SORT_HEIGHT,
        SORT_MAX_SIDE,
        SORT_PERIMETER
    };
};

RectanglePacker::PackResult RectanglePacker::packRectanglesGreedy(const std::vector<Rectangle>& rects, 
                                                             int maxWidth, int maxHeight) {
    PackResult result;
    result.algorithmName = "Greedy";
    result.atlasWidth = maxWidth;
    result.atlasHeight = maxHeight;
    
    std::vector<Rectangle> sortedRects = rects;
    sortRectangles(sortedRects, SORT_AREA);
    
    std::vector<Rectangle> freeRects;
    freeRects.emplace_back(0, 0, maxWidth, maxHeight);
    
    for (const auto& rect : sortedRects) {
        bool placed = false;
        
        for (auto& freeRect : freeRects) {
            if (fits(rect, freeRect.width, freeRect.height)) {
                Rectangle placedRect(freeRect.x, freeRect.y, rect.width, rect.height, rect.textureID);
                result.rectangles.push_back(placedRect);
                
                auto newFreeRects = splitFreeRectangle(freeRect, rect);
                freeRects.erase(std::remove(freeRects.begin(), freeRects.end(), freeRect), 
                               freeRects.end());
                freeRects.insert(freeRects.end(), newFreeRects.begin(), newFreeRects.end());
                
                placed = true;
                break;
            }
        }
        
        if (!placed) {
            // Try to extend the atlas
            if (result.atlasHeight < maxHeight * 2) {
                result.atlasHeight *= 2;
                freeRects.clear();
                freeRects.emplace_back(0, 0, maxWidth, result.atlasHeight);
                continue;
            }
        }
    }
    
    // Calculate efficiency
    result.totalArea = result.atlasWidth * result.atlasHeight;
    result.usedArea = 0;
    for (const auto& rect : result.rectangles) {
        result.usedArea += rect.width * rect.height;
    }
    result.efficiency = static_cast<float>(result.usedArea) / result.totalArea;
    
    return result;
}

RectanglePacker::PackResult RectanglePacker::packRectanglesBestFit(const std::vector<Rectangle>& rects, 
                                                            int maxWidth, int maxHeight) {
    PackResult result;
    result.algorithmName = "Best Fit";
    result.atlasWidth = maxWidth;
    result.atlasHeight = maxHeight;
    
    std::vector<Rectangle> sortedRects = rects;
    sortRectangles(sortedRects, SORT_MAX_SIDE);
    
    std::vector<Rectangle> freeRects;
    freeRects.emplace_back(0, 0, maxWidth, maxHeight);
    
    for (const auto& rect : sortedRects) {
        int bestArea = INT_MAX;
        Rectangle bestFit;
        bool found = false;
        
        for (const auto& freeRect : freeRects) {
            if (fits(rect, freeRect.width, freeRect.height)) {
                int area = freeRect.width * freeRect.height;
                if (area < bestArea) {
                    bestArea = area;
                    bestFit = Rectangle(freeRect.x, freeRect.y, rect.width, rect.height, rect.textureID);
                    found = true;
                }
            }
        }
        
        if (found) {
            result.rectangles.push_back(bestFit);
            
            // Split the free rectangle
            auto newFreeRects = splitFreeRectangle(bestFit, rect);
            freeRects.erase(std::remove(freeRects.begin(), freeRects.end(), bestFit), 
                           freeRects.end());
            freeRects.insert(freeRects.end(), newFreeRects.begin(), newFreeRects.end());
        }
    }
    
    // Calculate efficiency
    result.totalArea = result.atlasWidth * result.atlasHeight;
    result.usedArea = 0;
    for (const auto& rect : result.rectangles) {
        result.usedArea += rect.width * rect.height;
    }
    result.efficiency = static_cast<float>(result.usedArea) / result.totalArea;
    
    return result;
}

RectanglePacker::PackResult RectanglePacker::packRectanglesBottomLeft(const std::vector<Rectangle>& rects, 
                                                               int maxWidth, int maxHeight) {
    PackResult result;
    result.algorithmName = "Bottom Left";
    result.atlasWidth = maxWidth;
    result.atlasHeight = maxHeight;
    
    std::vector<Rectangle> sortedRects = rects;
    sortRectangles(sortedRects, SORT_HEIGHT);
    
    std::vector<Rectangle> freeRects;
    freeRects.emplace_back(0, 0, maxWidth, maxHeight);
    
    for (const auto& rect : sortedRects) {
        bool placed = false;
        
        for (auto& freeRect : freeRects) {
            if (fits(rect, freeRect.width, freeRect.height)) {
                Rectangle placedRect(freeRect.x, freeRect.y, rect.width, rect.height, rect.textureID);
                result.rectangles.push_back(placedRect);
                
                auto newFreeRects = splitFreeRectangle(freeRect, rect);
                freeRects.erase(std::remove(freeRects.begin(), freeRects.end(), freeRect), 
                               freeRects.end());
                freeRects.insert(freeRects.end(), newFreeRects.begin(), newFreeRects.end());
                
                placed = true;
                break;
            }
        }
        
        if (!placed) {
            // Try to extend the atlas
            if (result.atlasHeight < maxHeight * 2) {
                result.atlasHeight *= 2;
                freeRects.clear();
                freeRects.emplace_back(0, 0, maxWidth, result.atlasHeight);
                continue;
            }
        }
    }
    
    // Calculate efficiency
    result.totalArea = result.atlasWidth * result.atlasHeight;
    result.usedArea = 0;
    for (const auto& rect : result.rectangles) {
        result.usedArea += rect.width * rect.height;
    }
    result.efficiency = static_cast<float>(result.usedArea) / result.totalArea;
    
    return result;
}

RectanglePacker::PackResult RectanglePacker::packRectanglesTopLeft(const std::vector<Rectangle>& rects, 
                                                              int maxWidth, int maxHeight) {
    PackResult result;
    result.algorithmName = "Top Left";
    result.atlasWidth = maxWidth;
    result.atlasHeight = maxHeight;
    
    std::vector<Rectangle> sortedRects = rects;
    sortRectangles(sortedRects, SORT_WIDTH);
    
    std::vector<Rectangle> freeRects;
    freeRects.emplace_back(0, 0, maxWidth, maxHeight);
    
    for (const auto& rect : sortedRects) {
        bool placed = false;
        
        for (auto& freeRect : freeRects) {
            if (fits(rect, freeRect.width, freeRect.height)) {
                Rectangle placedRect(freeRect.x, freeRect.y, rect.width, rect.height, rect.textureID);
                result.rectangles.push_back(placedRect);
                
                auto newFreeRects = splitFreeRectangle(freeRect, rect);
                freeRects.erase(std::remove(freeRects.begin(), freeRects.end(), freeRect), 
                               freeRects.end());
                freeRects.insert(freeRects.end(), newFreeRects.begin(), newFreeRects.end());
                
                placed = true;
                break;
            }
        }
        
        if (!placed) {
            // Try to extend the atlas
            if (result.atlasHeight < maxHeight * 2) {
                result.atlasHeight *= 2;
                freeRects.clear();
                freeRects.emplace_back(0, 0, maxWidth, result.atlasHeight);
                continue;
            }
        }
    }
    
    // Calculate efficiency
    result.totalArea = result.atlasWidth * result.atlasHeight;
    result.usedArea = 0;
    for (const auto& rect : result.rectangles) {
        result.usedArea += rect.width * rect.height;
    }
    result.efficiency = static_cast<float>(result.usedArea) / result.totalArea;
    
    return result;
}

RectanglePacker::PackResult RectanglePacker::packRectanglesGuillotine(const std::vector<Rectangle>& rects, 
                                                              int maxWidth, int maxHeight, bool rotate) {
    PackResult result;
    result.algorithmName = rotate ? "Guillotine (with rotation)" : "Guillotine";
    result.atlasWidth = maxWidth;
    result.atlasHeight = maxHeight;
    
    std::vector<Rectangle> sortedRects = rects;
    sortRectangles(sortedRects, SORT_AREA);
    
    std::unique_ptr<Node> root;
    std::vector<FreeRectangle> freeRects;
    freeRects.emplace_back(0, 0, maxWidth, maxHeight);
    
    for (const auto& rect : sortedRects) {
        Rectangle bestRect = rect;
        bool bestRotated = false;
        
        if (rotate) {
            Rectangle rotatedRect(rect.x, rect.y, rect.height, rect.width, rect.textureID);
            auto rotatedFits = guillotineFindBestFit(rotatedRect, freeRects);
            
            if (!rotatedFits.empty() && 
                (rotatedFits[0].width * rotatedFits[0].height < bestRect.width * bestRect.height)) {
                bestRect = rotatedFits[0];
                bestRotated = true;
            }
        }
        
        if (guillotineInsert(root, bestRect)) {
            bestRect.rotated = bestRotated;
            result.rectangles.push_back(bestRect);
        }
    }
    
    // Calculate efficiency
    result.totalArea = result.atlasWidth * result.atlasHeight;
    result.usedArea = 0;
    for (const auto& rect : result.rectangles) {
        result.usedArea += rect.width * rect.height;
    }
    result.efficiency = static_cast<float>(result.usedArea) / result.totalArea;
    
    return result;
}

RectanglePacker::PackResult RectanglePacker::packRectanglesMaxRects(const std::vector<Rectangle>& rects, 
                                                             int maxWidth, int maxHeight, bool rotate) {
    PackResult result;
    result.algorithmName = rotate ? "MaxRects (with rotation)" : "MaxRects";
    result.atlasWidth = maxWidth;
    result.atlasHeight = maxHeight;
    
    std::vector<Rectangle> sortedRects = rects;
    sortRectangles(sortedRects, SORT_PERIMETER);
    
    std::vector<Rectangle> freeRects;
    freeRects.emplace_back(0, 0, maxWidth, maxHeight);
    
    for (const auto& rect : sortedRects) {
        auto bestFits = findFreeRectsForRect(rect, freeRects);
        
        if (!bestFits.empty()) {
            Rectangle bestFit = bestFits[0];
            bool bestRotated = false;
            
            if (rotate) {
                Rectangle rotatedRect(rect.x, rect.y, rect.height, rect.width, rect.textureID);
                auto rotatedFits = findFreeRectsForRect(rotatedRect, freeRects);
                
                if (!rotatedFits.empty() && 
                    (rotatedFits[0].width * rotatedFits[0].height < bestFit.width * bestFit.height)) {
                    bestFit = rotatedFits[0];
                    bestRotated = true;
                }
            }
            
            result.rectangles.push_back(bestFit);
            
            // Split the free rectangle
            auto newFreeRects = splitFreeRectangle(bestFit, rect);
            freeRects.erase(std::remove(freeRects.begin(), freeRects.end(), bestFit), 
                           freeRects.end());
            freeRects.insert(freeRects.end(), newFreeRects.begin(), newFreeRects.end());
            
            // Update the rectangle in result
            result.rectangles.back().rotated = bestRotated;
        }
        
        pruneFreeRects(freeRects);
    }
    
    // Calculate efficiency
    result.totalArea = result.atlasWidth * result.atlasHeight;
    result.usedArea = 0;
    for (const auto& rect : result.rectangles) {
        result.usedArea += rect.width * rect.height;
    }
    result.efficiency = static_cast<float>(result.usedArea) / result.totalArea;
    
    return result;
}

RectanglePacker::PackResult RectanglePacker::packRectanglesSkyline(const std::vector<Rectangle>& rects, 
                                                             int maxWidth, int maxHeight, bool rotate) {
    PackResult result;
    result.algorithmName = rotate ? "Skyline (with rotation)" : "Skyline";
    result.atlasWidth = maxWidth;
    result.atlasHeight = maxHeight;
    
    std::vector<Rectangle> sortedRects = rects;
    sortRectangles(sortedRects, SORT_HEIGHT);
    
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
    result.totalArea = result.atlasWidth * result.atlasHeight;
    result.usedArea = 0;
    for (const auto& rect : result.rectangles) {
        result.usedArea += rect.width * rect.height;
    }
    result.efficiency = static_cast<float>(result.usedArea) / result.totalArea;
    
    return result;
}

bool RectanglePacker::fits(const Rectangle& rect, int width, int height) {
    return rect.width <= width && rect.height <= height;
}

int RectanglePacker::compareArea(const Rectangle& a, const Rectangle& b) {
    return b.width * b.height - a.width * a.height;
}

int RectanglePacker::compareWidth(const Rectangle& a, const Rectangle& b) {
    return b.width - a.width;
}

int RectanglePacker::compareHeight(const Rectangle& a, const Rectangle& b) {
    return b.height - a.height;
}

int RectanglePacker::compareMaxSide(const Rectangle& a, const Rectangle& b) {
    int maxA = std::max(a.width, a.height);
    int maxB = std::max(b.width, b.height);
    return maxB - maxA;
}

int RectanglePacker::comparePerimeter(const Rectangle& a, const Rectangle& b) {
    int perimeterA = 2 * (a.width + a.height);
    int perimeterB = 2 * (b.width + b.height);
    return perimeterB - perimeterA;
}

void RectanglePacker::optimizePacking(std::vector<Rectangle>& rectangles, int maxWidth, int maxHeight) {
    // Remove empty rectangles
    removeEmptyRectangles(rectangles);
    
    // Try different sorting methods and pick the best result
    std::vector<Rectangle> bestPacking = rectangles;
    float bestEfficiency = 0.0f;
    
    std::vector<SortMethod> methods = {SORT_AREA, SORT_MAX_SIDE, SORT_PERIMETER};
    
    for (SortMethod method : methods) {
        auto testRects = rectangles;
        sortRectangles(testRects, method);
        
        auto testResult = packRectanglesGreedy(testRects, maxWidth, maxHeight);
        
        if (testResult.efficiency > bestEfficiency) {
            bestPacking = testResult.rectangles;
            bestEfficiency = testResult.efficiency;
        }
    }
    
    rectangles = bestPacking;
}

void RectanglePacker::removeEmptyRectangles(std::vector<Rectangle>& rectangles) {
    rectangles.erase(
        std::remove_if(rectangles.begin(), rectangles.end(),
            [](const Rectangle& rect) {
                return rect.width <= 0 || rect.height <= 0;
            }),
        rectangles.end()
    );
}

void RectanglePacker::sortRectangles(std::vector<Rectangle>& rectangles, int sortMethod) {
    switch (sortMethod) {
        case SORT_AREA:
            std::sort(rectangles.begin(), rectangles.end(), compareArea);
            break;
        case SORT_WIDTH:
            std::sort(rectangles.begin(), rectangles.end(), compareWidth);
            break;
        case SORT_HEIGHT:
            std::sort(rectangles.begin(), rectangles.end(), compareHeight);
            break;
        case SORT_MAX_SIDE:
            std::sort(rectangles.begin(), rectangles.end(), compareMaxSide);
            break;
        case SORT_PERIMETER:
            std::sort(rectangles.begin(), rectangles.end(), comparePerimeter);
            break;
    }
}

bool RectanglePacker::guillotineInsert(std::unique_ptr<Node>& root, const Rectangle& rect, int maxWidth, int maxHeight) {
    if (!root) {
        root = std::make_unique<Node>(rect);
        return true;
    }
    
    if (fits(rect, root->rect.width, root->rect.height)) {
        if (guillotineInsert(root->right, rect, maxWidth, maxHeight)) return true;
        if (guillotineInsert(root->down, rect, maxWidth, maxHeight)) return true;
    }
    
    return false;
}

std::vector<RectanglePacker::FreeRectangle> RectanglePacker::splitFreeRectangle(const FreeRectangle& freeRect, const Rectangle& rect) {
    std::vector<FreeRectangle> result;
    
    // Split the free rectangle around the placed rectangle
    int rightWidth = freeRect.width - rect.width;
    if (rightWidth > 0) {
        result.emplace_back(rect.x + rect.width, freeRect.y, rightWidth, freeRect.height);
    }
    
    int bottomHeight = freeRect.height - rect.height;
    if (bottomHeight > 0) {
        result.emplace_back(freeRect.x, rect.y + rect.height, freeRect.width, bottomHeight);
    }
    
    return result;
}

std::vector<RectanglePacker::Rectangle> RectanglePacker::guillotineFindBestFit(const Rectangle& rect, const std::vector<FreeRectangle>& freeRects) {
    std::vector<Rectangle> bestFits;
    int bestArea = INT_MAX;
    
    for (const auto& freeRect : freeRects) {
        if (fits(rect, freeRect.width, freeRect.height)) {
            int area = freeRect.width * freeRect.height;
            if (area < bestArea) {
                bestArea = area;
                bestFits.clear();
                bestFits.push_back(Rectangle(freeRect.x, freeRect.y, rect.width, rect.height, rect.textureID));
            }
        }
    }
    
    return bestFits;
}

std::vector<RectanglePacker::Rectangle> RectanglePacker::findFreeRectsForRect(const Rectangle& rect, const std::vector<Rectangle>& freeRects) {
    std::vector<Rectangle> bestFits;
    int bestArea = INT_MAX;
    
    for (const auto& freeRect : freeRects) {
        if (fits(rect, freeRect.width, freeRect.height)) {
            int area = freeRect.width * freeRect.height;
            if (area < bestArea) {
                bestArea = area;
                bestFits.clear();
                bestFits.push_back(Rectangle(freeRect.x, freeRect.y, rect.width, rect.height, rect.textureID));
            }
        }
    }
    
    return bestFits;
}

void RectanglePacker::pruneFreeRects(std::vector<Rectangle>& freeRects) {
    for (size_t i = 0; i < freeRects.size(); ++i) {
        for (size_t j = i + 1; j < freeRects.size(); ++j) {
            if (commonArea(freeRects[i], freeRects[j]) == freeRects[j].width * freeRects[j].height) {
                freeRects.erase(freeRects.begin() + j);
                freeRects.insert(freeRects.begin() + j, freeRects.begin() + i);
                --j;
            }
        }
    }
}

int RectanglePacker::commonArea(const Rectangle& a, const Rectangle& b) {
    int x1 = std::max(a.x, b.x);
    int y1 = std::max(a.y, b.y);
    int x2 = std::min(a.x + a.width, b.x + b.width);
    int y2 = std::min(a.y + a.height, b.y + b.height);
    
    if (x2 <= x1 || y2 <= y1) return 0;
    
    return (x2 - x1) * (y2 - y1);
}

std::vector<RectanglePacker::SkylineNode> RectanglePacker::skylineInsert(const std::vector<Rectangle>& rects, 
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

void RectanglePacker::updateSkyline(std::vector<SkylineNode>& skyline, const Rectangle& rect) {
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

std::vector<Rectangle> RectanglePacker::shelfPack(const std::vector<Rectangle>& rects, int maxWidth, int maxHeight, bool rotate) {
    std::vector<Rectangle> result;
    
    // Sort rectangles by width (widest first)
    std::vector<Rectangle> sortedRects = rects;
    sortRectangles(sortedRects, SORT_WIDTH);
    
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