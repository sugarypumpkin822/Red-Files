#pragma once

#include <vector>
#include <memory>
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

} // namespace TexturePacker
