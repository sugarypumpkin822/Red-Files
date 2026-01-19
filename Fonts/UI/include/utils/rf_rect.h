#pragma once

#include <cstdint>
#include <string>
#include <algorithm>
#include <cmath>

namespace RFUtils {

// Forward declarations
class Vector2;
class Vector3;
class Vector4;
class Color;

// Rectangle class
class Rect {
public:
    float32 x, y, width, height;
    
    // Constructors
    Rect() : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {}
    Rect(float32 x, float32 y, float32 width, float32 height) : x(x), y(y), width(width), height(height) {}
    Rect(const Vector2& position, const Vector2& size);
    Rect(const Vector2& topLeft, const Vector2& bottomRight);
    Rect(const Vector4& values); // x, y, width, height
    Rect(const float32* values) : x(values[0]), y(values[1]), width(values[2]), height(values[3]) {}
    
    // Copy constructor
    Rect(const Rect& other) : x(other.x), y(other.y), width(other.width), height(other.height) {}
    
    // Assignment operator
    Rect& operator=(const Rect& other) {
        x = other.x;
        y = other.y;
        width = other.width;
        height = other.height;
        return *this;
    }
    
    // Component access
    float32& operator[](size_t index) { return (&x)[index]; }
    const float32& operator[](size_t index) const { return (&x)[index]; }
    
    // Array access
    float32* data() { return &x; }
    const float32* data() const { return &x; }
    
    // Comparison operators
    bool operator==(const Rect& other) const {
        return x == other.x && y == other.y && width == other.width && height == other.height;
    }
    
    bool operator!=(const Rect& other) const {
        return !(*this == other);
    }
    
    // Arithmetic operators
    Rect operator+(const Rect& other) const {
        return Rect(x + other.x, y + other.y, width + other.width, height + other.height);
    }
    
    Rect operator-(const Rect& other) const {
        return Rect(x - other.x, y - other.y, width - other.width, height - other.height);
    }
    
    Rect operator*(const Rect& other) const {
        return Rect(x * other.x, y * other.y, width * other.width, height * other.height);
    }
    
    Rect operator/(const Rect& other) const {
        return Rect(x / other.x, y / other.y, width / other.width, height / other.height);
    }
    
    Rect operator*(float32 scalar) const {
        return Rect(x * scalar, y * scalar, width * scalar, height * scalar);
    }
    
    Rect operator/(float32 scalar) const {
        return Rect(x / scalar, y / scalar, width / scalar, height / scalar);
    }
    
    // Assignment arithmetic operators
    Rect& operator+=(const Rect& other) {
        x += other.x;
        y += other.y;
        width += other.width;
        height += other.height;
        return *this;
    }
    
    Rect& operator-=(const Rect& other) {
        x -= other.x;
        y -= other.y;
        width -= other.width;
        height -= other.height;
        return *this;
    }
    
    Rect& operator*=(const Rect& other) {
        x *= other.x;
        y *= other.y;
        width *= other.width;
        height *= other.height;
        return *this;
    }
    
    Rect& operator/=(const Rect& other) {
        x /= other.x;
        y /= other.y;
        width /= other.width;
        height /= other.height;
        return *this;
    }
    
    Rect& operator*=(float32 scalar) {
        x *= scalar;
        y *= scalar;
        width *= scalar;
        height *= scalar;
        return *this;
    }
    
    Rect& operator/=(float32 scalar) {
        x /= scalar;
        y /= scalar;
        width /= scalar;
        height /= scalar;
        return *this;
    }
    
    // Unary operators
    Rect operator+() const {
        return *this;
    }
    
    Rect operator-() const {
        return Rect(-x, -y, -width, -height);
    }
    
    // Position access
    Vector2 getPosition() const;
    void setPosition(const Vector2& position);
    void setPosition(float32 x, float32 y);
    
    // Size access
    Vector2 getSize() const;
    void setSize(const Vector2& size);
    void setSize(float32 width, float32 height);
    
    // Corner access
    Vector2 getTopLeft() const;
    void setTopLeft(const Vector2& topLeft);
    
    Vector2 getTopRight() const;
    void setTopRight(const Vector2& topRight);
    
    Vector2 getBottomLeft() const;
    void setBottomLeft(const Vector2& bottomLeft);
    
    Vector2 getBottomRight() const;
    void setBottomRight(const Vector2& bottomRight);
    
    // Center access
    Vector2 getCenter() const;
    void setCenter(const Vector2& center);
    
    // Edge access
    float32 getLeft() const { return x; }
    float32 getRight() const { return x + width; }
    float32 getTop() const { return y; }
    float32 getBottom() const { return y + height; }
    
    void setLeft(float32 left) { x = left; }
    void setRight(float32 right) { width = right - x; }
    void setTop(float32 top) { y = top; }
    void setBottom(float32 bottom) { height = bottom - y; }
    
    // Utility methods
    bool isEmpty() const {
        return width <= 0.0f || height <= 0.0f;
    }
    
    bool isValid() const {
        return width >= 0.0f && height >= 0.0f;
    }
    
    float32 getArea() const {
        return width * height;
    }
    
    float32 getPerimeter() const {
        return 2.0f * (width + height);
    }
    
    float32 getAspect() const {
        return height != 0.0f ? width / height : 0.0f;
    }
    
    // Containment tests
    bool contains(const Vector2& point) const;
    bool contains(float32 px, float32 py) const;
    bool contains(const Rect& other) const;
    
    // Intersection tests
    bool intersects(const Rect& other) const;
    Rect intersection(const Rect& other) const;
    
    // Union operations
    Rect unite(const Rect& other) const;
    void uniteWith(const Rect& other);
    
    // Expansion and contraction
    Rect expand(float32 amount) const;
    Rect expand(float32 horizontal, float32 vertical) const;
    Rect expand(const Vector2& amount) const;
    Rect expand(const Rect& padding) const;
    
    Rect contract(float32 amount) const;
    Rect contract(float32 horizontal, float32 vertical) const;
    Rect contract(const Vector2& amount) const;
    Rect contract(const Rect& margin) const;
    
    // Inset and outset
    Rect inset(float32 amount) const { return contract(amount); }
    Rect inset(float32 horizontal, float32 vertical) const { return contract(horizontal, vertical); }
    Rect inset(const Vector2& amount) const { return contract(amount); }
    Rect inset(const Rect& margin) const { return contract(margin); }
    
    Rect outset(float32 amount) const { return expand(amount); }
    Rect outset(float32 horizontal, float32 vertical) const { return expand(horizontal, vertical); }
    Rect outset(const Vector2& amount) const { return expand(amount); }
    Rect outset(const Rect& padding) const { return expand(padding); }
    
    // Fitting
    Rect fit(const Rect& other) const;
    Rect fitInside(const Rect& other) const;
    Rect fitOutside(const Rect& other) const;
    
    // Scaling
    Rect scale(float32 scale) const;
    Rect scale(float32 scaleX, float32 scaleY) const;
    Rect scale(const Vector2& scale) const;
    Rect scaleFromCenter(float32 scale) const;
    Rect scaleFromCenter(float32 scaleX, float32 scaleY) const;
    Rect scaleFromCenter(const Vector2& scale) const;
    Rect scaleFromOrigin(float32 scale) const;
    Rect scaleFromOrigin(float32 scaleX, float32 scaleY) const;
    Rect scaleFromOrigin(const Vector2& scale) const;
    
    // Translation
    Rect translate(const Vector2& offset) const;
    Rect translate(float32 offsetX, float32 offsetY) const;
    void translateBy(const Vector2& offset);
    void translateBy(float32 offsetX, float32 offsetY);
    
    // Rotation (around center)
    Rect rotate(float32 angle) const;
    Rect rotate(float32 angle, const Vector2& pivot) const;
    
    // Clipping
    Rect clip(const Rect& bounds) const;
    void clipTo(const Rect& bounds);
    
    // Snapping
    Rect snapToPixel() const;
    Rect snapToGrid(float32 gridSize) const;
    
    // Rounding
    Rect round() const;
    Rect floor() const;
    Rect ceil() const;
    
    // Absolute
    Rect abs() const;
    
    // Min/Max
    Rect min(const Rect& other) const;
    Rect max(const Rect& other) const;
    Rect clamp(const Rect& min, const Rect& max) const;
    
    // Lerp
    static Rect lerp(const Rect& a, const Rect& b, float32 t);
    static Rect slerp(const Rect& a, const Rect& b, float32 t);
    static Rect nlerp(const Rect& a, const Rect& b, float32 t);
    
    // Static methods
    static Rect zero() { return Rect(0.0f, 0.0f, 0.0f, 0.0f); }
    static Rect one() { return Rect(0.0f, 0.0f, 1.0f, 1.0f); }
    static Rect unit() { return Rect(0.0f, 0.0f, 1.0f, 1.0f); }
    
    static Rect fromCenter(const Vector2& center, const Vector2& size);
    static Rect fromCenter(float32 centerX, float32 centerY, float32 width, float32 height);
    static Rect fromCorners(const Vector2& topLeft, const Vector2& bottomRight);
    static Rect fromCorners(float32 left, float32 top, float32 right, float32 bottom);
    static Rect fromPoints(const std::vector<Vector2>& points);
    static Rect boundingBox(const std::vector<Vector2>& points);
    static Rect boundingBox(const Vector2* points, size_t count);
    
    // String conversion
    std::string toString() const;
    
    // Constants
    static constexpr float32 EPSILON = 0.0001f;
};

// Global operators for scalar operations
inline Rect operator*(float32 scalar, const Rect& rect) {
    return rect * scalar;
}

inline Rect operator/(float32 scalar, const Rect& rect) {
    return Rect(scalar / rect.x, scalar / rect.y, scalar / rect.width, scalar / rect.height);
}

// Utility functions
inline Rect abs(const Rect& rect) {
    return rect.abs();
}

inline Rect min(const Rect& a, const Rect& b) {
    return a.min(b);
}

inline Rect max(const Rect& a, const Rect& b) {
    return a.max(b);
}

inline Rect clamp(const Rect& value, const Rect& min, const Rect& max) {
    return value.clamp(min, max);
}

inline Rect lerp(const Rect& a, const Rect& b, float32 t) {
    return Rect::lerp(a, b, t);
}

inline Rect slerp(const Rect& a, const Rect& b, float32 t) {
    return Rect::slerp(a, b, t);
}

inline Rect nlerp(const Rect& a, const Rect& b, float32 t) {
    return Rect::nlerp(a, b, t);
}

// Rectangle utilities namespace
namespace RectUtils {
    // Rectangle operations
    Rect unionRect(const Rect& a, const Rect& b);
    Rect unionRects(const std::vector<Rect>& rects);
    Rect unionRects(const Rect* rects, size_t count);
    
    Rect intersectRect(const Rect& a, const Rect& b);
    Rect intersectRects(const std::vector<Rect>& rects);
    Rect intersectRects(const Rect* rects, size_t count);
    
    // Rectangle containment
    bool containsPoint(const Rect& rect, const Vector2& point);
    bool containsPoint(const Rect& rect, float32 x, float32 y);
    bool containsRect(const Rect& container, const Rect& contained);
    
    bool containsPoints(const Rect& rect, const std::vector<Vector2>& points);
    bool containsPoints(const Rect& rect, const Vector2* points, size_t count);
    
    // Rectangle intersection
    bool intersectsRect(const Rect& a, const Rect& b);
    bool intersectsLine(const Rect& rect, const Vector2& start, const Vector2& end);
    bool intersectsLine(const Rect& rect, float32 x1, float32 y1, float32 x2, float32 y2);
    bool intersectsCircle(const Rect& rect, const Vector2& center, float32 radius);
    bool intersectsCircle(const Rect& rect, float32 centerX, float32 centerY, float32 radius);
    bool intersectsTriangle(const Rect& rect, const Vector2& a, const Vector2& b, const Vector2& c);
    
    // Rectangle distance
    float32 distanceToPoint(const Rect& rect, const Vector2& point);
    float32 distanceToPoint(const Rect& rect, float32 x, float32 y);
    float32 distanceToRect(const Rect& a, const Rect& b);
    
    // Rectangle clipping
    Rect clipRect(const Rect& rect, const Rect& bounds);
    Rect clipLine(const Rect& rect, const Vector2& start, const Vector2& end, Vector2& clippedStart, Vector2& clippedEnd);
    Rect clipLine(const Rect& rect, float32 x1, float32 y1, float32 x2, float32 y2, float32& cx1, float32& cy1, float32& cx2, float32& cy2);
    
    // Rectangle transformation
    Rect transformRect(const Rect& rect, const Matrix3& transform);
    Rect transformRect(const Rect& rect, const Matrix4& transform);
    
    // Rectangle partitioning
    std::vector<Rect> partitionRect(const Rect& rect, size_t rows, size_t cols);
    std::vector<Rect> partitionRect(const Rect& rect, const Vector2& cellSize);
    std::vector<Rect> partitionRectGrid(const Rect& rect, size_t rows, size_t cols, float32 padding = 0.0f);
    
    // Rectangle subdivision
    std::vector<Rect> subdivideRect(const Rect& rect, size_t subdivisions);
    std::vector<Rect> subdivideRectHorizontal(const Rect& rect, size_t subdivisions);
    std::vector<Rect> subdivideRectVertical(const Rect& rect, size_t subdivisions);
    
    // Rectangle packing
    class RectanglePacker {
    public:
        RectanglePacker(const Rect& bounds);
        
        bool pack(const Vector2& size, Vector2& position);
        bool pack(const std::vector<Vector2>& sizes, std::vector<Vector2>& positions);
        bool pack(const Vector2* sizes, size_t count, Vector2* positions);
        
        void reset();
        void clear();
        
        Rect getBounds() const { return bounds_; }
        float32 getUsedArea() const;
        float32 getFreeArea() const;
        float32 getUtilization() const;
        size_t getRectangleCount() const;
        
    private:
        Rect bounds_;
        std::vector<Rect> freeRects_;
        
        void splitFreeRect(const Rect& rect, const Vector2& size);
        void pruneFreeRects();
        bool findBestFit(const Vector2& size, Vector2& position, Rect& freeRect);
    };
    
    // Rectangle grid
    class RectangleGrid {
    public:
        RectangleGrid(const Rect& bounds, size_t rows, size_t cols);
        
        Rect getCell(size_t row, size_t col) const;
        Rect getCell(size_t index) const;
        Vector2 getCellPosition(size_t row, size_t col) const;
        Vector2 getCellPosition(size_t index) const;
        Vector2 getCellSize() const;
        
        size_t getRows() const { return rows_; }
        size_t getCols() const { return cols_; }
        size_t getCellCount() const { return rows_ * cols_; }
        
        Rect getBounds() const { return bounds_; }
        
        bool containsPoint(const Vector2& point) const;
        bool containsPoint(float32 x, float32 y) const;
        
        size_t getCellAt(const Vector2& point) const;
        size_t getCellAt(float32 x, float32 y) const;
        void getCellAt(const Vector2& point, size_t& row, size_t& col) const;
        void getCellAt(float32 x, float32 y, size_t& row, size_t& col) const;
        
        std::vector<size_t> getCellsInRect(const Rect& rect) const;
        std::vector<size_t> getCellsInCircle(const Vector2& center, float32 radius) const;
        
    private:
        Rect bounds_;
        size_t rows_;
        size_t cols_;
        Vector2 cellSize_;
        
        void validateCell(size_t row, size_t col) const;
        size_t cellIndex(size_t row, size_t col) const;
        void cellIndex(size_t index, size_t& row, size_t& col) const;
    };
    
    // Rectangle atlas
    class RectangleAtlas {
    public:
        RectangleAtlas(const Vector2& size);
        
        int32_t addRectangle(const Vector2& size);
        bool removeRectangle(int32_t id);
        void clear();
        
        Rect getRectangle(int32_t id) const;
        Vector2 getSize() const { return size_; }
        int32_t getRectangleCount() const;
        
        float32 getUtilization() const;
        float32 getFreeArea() const;
        
        bool pack();
        bool pack(const std::vector<Vector2>& sizes, std::vector<Rect>& rects);
        
    private:
        struct RectangleInfo {
            int32_t id;
            Vector2 size;
            Rect rect;
            bool placed;
        };
        
        Vector2 size_;
        std::vector<RectangleInfo> rectangles_;
        std::vector<Rect> freeRects_;
        
        bool findBestFit(const Vector2& size, Rect& rect);
        void splitFreeRect(const Rect& rect, const Vector2& size);
        void pruneFreeRects();
    };
    
    // Rectangle tree (quadtree)
    class RectangleTree {
    public:
        RectangleTree(const Rect& bounds, size_t maxDepth = 8, size_t maxObjects = 10);
        
        void insert(const Rect& rect, int32_t id);
        void remove(const Rect& rect, int32_t id);
        void clear();
        
        std::vector<int32_t> query(const Rect& rect) const;
        std::vector<int32_t> query(const Vector2& point) const;
        std::vector<int32_t> query(const Vector2& center, float32 radius) const;
        
        Rect getBounds() const { return bounds_; }
        size_t getMaxDepth() const { return maxDepth_; }
        size_t getMaxObjects() const { return maxObjects_; }
        size_t getNodeCount() const;
        size_t getObjectCount() const;
        
    private:
        struct Node {
            Rect bounds;
            std::vector<std::pair<Rect, int32_t>> objects;
            std::unique_ptr<Node> children[4];
            size_t depth;
            
            Node(const Rect& bounds, size_t depth) : bounds(bounds), depth(depth) {}
            
            bool isLeaf() const {
                return children[0] == nullptr;
            }
            
            void subdivide();
            void insert(const Rect& rect, int32_t id, size_t maxDepth, size_t maxObjects);
            void remove(const Rect& rect, int32_t id);
            void query(const Rect& rect, std::vector<int32_t>& results) const;
            void query(const Vector2& point, std::vector<int32_t>& results) const;
            void query(const Vector2& center, float32 radius, std::vector<int32_t>& results) const;
            void clear();
            size_t getNodeCount() const;
            size_t getObjectCount() const;
        };
        
        Rect bounds_;
        size_t maxDepth_;
        size_t maxObjects_;
        std::unique_ptr<Node> root_;
    };
    
    // Rectangle spatial hash
    class RectangleSpatialHash {
    public:
        RectangleSpatialHash(const Vector2& cellSize, size_t tableSize = 1024);
        
        void insert(const Rect& rect, int32_t id);
        void remove(const Rect& rect, int32_t id);
        void update(const Rect& oldRect, const Rect& newRect, int32_t id);
        void clear();
        
        std::vector<int32_t> query(const Rect& rect) const;
        std::vector<int32_t> query(const Vector2& point) const;
        std::vector<int32_t> query(const Vector2& center, float32 radius) const;
        
        Vector2 getCellSize() const { return cellSize_; }
        size_t getTableSize() const { return tableSize_; }
        size_t getObjectCount() const;
        
    private:
        struct HashEntry {
            Rect rect;
            int32_t id;
            size_t hash;
        };
        
        Vector2 cellSize_;
        size_t tableSize_;
        std::vector<std::vector<HashEntry>> table_;
        
        size_t hash(const Vector2& position) const;
        std::vector<size_t> getHashes(const Rect& rect) const;
        std::vector<size_t> getHashes(const Vector2& center, float32 radius) const;
    };
    
    // Rectangle utilities
    bool isPointInRect(const Vector2& point, const Rect& rect);
    bool isPointInRect(float32 x, float32 y, const Rect& rect);
    bool isRectInRect(const Rect& inner, const Rect& outer);
    bool doRectsIntersect(const Rect& a, const Rect& b);
    
    Rect inflateRect(const Rect& rect, float32 amount);
    Rect inflateRect(const Rect& rect, float32 horizontal, float32 vertical);
    Rect inflateRect(const Rect& rect, const Vector2& amount);
    
    Rect deflateRect(const Rect& rect, float32 amount);
    Rect deflateRect(const Rect& rect, float32 horizontal, float32 vertical);
    Rect deflateRect(const Rect& rect, const Vector2& amount);
    
    Rect offsetRect(const Rect& rect, const Vector2& offset);
    Rect offsetRect(const Rect& rect, float32 offsetX, float32 offsetY);
    
    Rect scaleRect(const Rect& rect, float32 scale);
    Rect scaleRect(const Rect& rect, const Vector2& scale);
    Rect scaleRect(const Rect& rect, float32 scaleX, float32 scaleY);
    
    Rect rotateRect(const Rect& rect, float32 angle);
    Rect rotateRect(const Rect& rect, float32 angle, const Vector2& pivot);
    
    Rect normalizeRect(const Rect& rect);
    Rect absRect(const Rect& rect);
    
    Rect clampRect(const Rect& rect, const Rect& bounds);
    Rect clampRect(const Rect& rect, const Vector2& min, const Vector2& max);
    
    // Rectangle analysis
    float32 rectArea(const Rect& rect);
    float32 rectPerimeter(const Rect& rect);
    float32 rectAspect(const Rect& rect);
    Vector2 rectCenter(const Rect& rect);
    float32 rectDiagonal(const Rect& rect);
    
    // Rectangle comparison
    bool rectEquals(const Rect& a, const Rect& b, float32 epsilon = Rect::EPSILON);
    bool rectContains(const Rect& container, const Rect& contained, float32 epsilon = Rect::EPSILON);
    bool rectIntersects(const Rect& a, const Rect& b, float32 epsilon = Rect::EPSILON);
    
    // Rectangle conversion
    Rect rectFromPoints(const std::vector<Vector2>& points);
    Rect rectFromPoints(const Vector2* points, size_t count);
    Rect rectFromCenter(const Vector2& center, const Vector2& size);
    Rect rectFromCorners(const Vector2& topLeft, const Vector2& bottomRight);
    
    // Rectangle validation
    bool isValidRect(const Rect& rect);
    bool isEmptyRect(const Rect& rect);
    bool isNormalizedRect(const Rect& rect);
    
    // Rectangle debugging
    std::string rectToString(const Rect& rect);
    void rectDebugPrint(const Rect& rect);
    void rectDebugDraw(const Rect& rect, const Color& color = Color::white());
}

// Hash function for Rect
struct RectHash {
    size_t operator()(const Rect& rect) const noexcept {
        size_t h1 = std::hash<float32>{}(rect.x);
        size_t h2 = std::hash<float32>{}(rect.y);
        size_t h3 = std::hash<float32>{}(rect.width);
        size_t h4 = std::hash<float32>{}(rect.height);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
};

} // namespace RFUtils