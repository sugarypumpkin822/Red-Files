#pragma once

#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

namespace FontBaker {

class GlyphRasterizer {
public:
    enum class RasterizationMode {
        STANDARD,    // Standard bitmap rasterization
        SDF,        // Signed Distance Field
        MSDF         // Multi-channel Signed Distance Field
    };
    
    GlyphRasterizer();
    ~GlyphRasterizer();
    
    std::vector<uint8_t> rasterizeGlyph(
        void* fontFace,
        uint32_t glyphIndex,
        uint32_t fontSize,
        RasterizationMode mode = RasterizationMode::STANDARD,
        float sdfSpread = 8.0f,
        uint32_t sdfRange = 4
    );
    
    glm::ivec2 getGlyphSize(void* fontFace, uint32_t glyphIndex, uint32_t fontSize);
    glm::ivec2 getGlyphBearing(void* fontFace, uint32_t glyphIndex, uint32_t fontSize);
    uint32_t getGlyphAdvance(void* fontFace, uint32_t glyphIndex, uint32_t fontSize);
    
    void setPadding(uint32_t padding) { padding_ = padding; }
    uint32_t getPadding() const { return padding_; }
    
    void setScale(float scale) { scale_ = scale; }
    float getScale() const { return scale_; }
    
private:
    uint32_t padding_;
    float scale_;
    
    // Standard rasterization
    std::vector<uint8_t> rasterizeStandard(void* fontFace, uint32_t glyphIndex, uint32_t fontSize);
    
    // SDF rasterization
    std::vector<uint8_t> rasterizeSDF(void* fontFace, uint32_t glyphIndex, uint32_t fontSize, float spread, uint32_t range);
    float computeDistanceField(const std::vector<uint8_t>& bitmap, uint32_t width, uint32_t height, uint32_t x, uint32_t y);
    
    // MSDF rasterization
    std::vector<uint8_t> rasterizeMSDF(void* fontFace, uint32_t glyphIndex, uint32_t fontSize, float spread, uint32_t range);
    glm::vec3 computeMSDF(const std::vector<uint8_t>& bitmap, uint32_t width, uint32_t height, uint32_t x, uint32_t y);
    
    // Helper functions
    std::vector<uint8_t> applyPadding(const std::vector<uint8_t>& bitmap, uint32_t width, uint32_t height, uint32_t padding);
    glm::vec2 computeGradient(const std::vector<uint8_t>& bitmap, uint32_t width, uint32_t height, uint32_t x, uint32_t y);
    float computeEdgeDistance(const std::vector<uint8_t>& bitmap, uint32_t width, uint32_t height, float x, float y);
};

class SDFGenerator {
public:
    SDFGenerator();
    ~SDFGenerator() = default;
    
    std::vector<uint8_t> generateSDF(
        const std::vector<uint8_t>& inputBitmap,
        uint32_t width,
        uint32_t height,
        float spread = 8.0f,
        uint32_t range = 4
    );
    
    std::vector<float> generateSDFFloat(
        const std::vector<uint8_t>& inputBitmap,
        uint32_t width,
        uint32_t height,
        float spread = 8.0f
    );
    
private:
    struct Point {
        float x, y;
        float distance;
        bool inside;
    };
    
    float computeDistance(const std::vector<uint8_t>& bitmap, uint32_t width, uint32_t height, float x, float y);
    Point getNearestPoint(const std::vector<uint8_t>& bitmap, uint32_t width, uint32_t height, float x, float y);
    std::vector<Point> getEdgePoints(const std::vector<uint8_t>& bitmap, uint32_t width, uint32_t height);
    
    bool isInside(const std::vector<uint8_t>& bitmap, uint32_t width, uint32_t height, int x, int y);
    uint8_t sampleBitmap(const std::vector<uint8_t>& bitmap, uint32_t width, uint32_t height, int x, int y);
};

class MSDFGenerator {
public:
    MSDFGenerator();
    ~MSDFGenerator() = default;
    
    std::vector<uint8_t> generateMSDF(
        const std::vector<uint8_t>& inputBitmap,
        uint32_t width,
        uint32_t height,
        float spread = 8.0f,
        uint32_t range = 4
    );
    
private:
    struct Edge {
        glm::vec2 start, end;
        glm::vec2 normal;
    };
    
    struct Contour {
        std::vector<Edge> edges;
    };
    
    std::vector<Contour> extractContours(const std::vector<uint8_t>& bitmap, uint32_t width, uint32_t height);
    std::vector<Edge> findEdges(const std::vector<uint8_t>& bitmap, uint32_t width, uint32_t height);
    
    glm::vec3 computeMSDFAtPoint(
        const std::vector<Contour>& contours,
        float x, float y,
        float spread, uint32_t range
    );
    
    float signedDistanceToEdges(const std::vector<Edge>& edges, const glm::vec2& point);
    glm::vec2 computeMedianNormal(const std::vector<Edge>& edges, const glm::vec2& point);
    
    void simplifyContour(Contour& contour, float tolerance = 1.0f);
    bool collinear(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, float tolerance = 0.001f);
};

} // namespace FontBaker