#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFCore {

// Glyph types
enum class GlyphType {
    OUTLINE,
    BITMAP,
    COMPOSITE,
    COLOR,
    SDF,
    MSDF,
    CUSTOM
};

// Glyph formats
enum class GlyphFormat {
    MONOCHROME,
    GRAYSCALE,
    RGB,
    RGBA,
    BGRA,
    ARGB,
    INDEXED,
    CUSTOM
};

// Glyph render modes
enum class GlyphRenderMode {
    NORMAL,
    EMBOLDEN,
    OBLIQUE,
    LIGHT,
    OUTLINE,
    SHADOW,
    CUSTOM
};

// Glyph metrics
struct GlyphMetrics {
    uint32_t glyphIndex;
    uint32_t charCode;
    float32 width;
    float32 height;
    float32 horizontalBearingX;
    float32 horizontalBearingY;
    float32 horizontalAdvance;
    float32 verticalBearingX;
    float32 verticalBearingY;
    float32 verticalAdvance;
    float32 leftSideBearing;
    float32 rightSideBearing;
    float32 topSideBearing;
    float32 bottomSideBearing;
    bool isWhitespace;
    bool isControl;
    
    GlyphMetrics() : glyphIndex(0), charCode(0), width(0.0f), height(0.0f), 
                    horizontalBearingX(0.0f), horizontalBearingY(0.0f), 
                    horizontalAdvance(0.0f), verticalBearingX(0.0f), 
                    verticalBearingY(0.0f), verticalAdvance(0.0f), 
                    leftSideBearing(0.0f), rightSideBearing(0.0f), 
                    topSideBearing(0.0f), bottomSideBearing(0.0f), 
                    isWhitespace(false), isControl(false) {}
};

// Glyph bitmap
struct GlyphBitmap {
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    GlyphFormat format;
    std::vector<uint8> data;
    float32 left;
    float32 top;
    GlyphRenderMode renderMode;
    
    GlyphBitmap() : width(0), height(0), stride(0), format(GlyphFormat::GRAYSCALE), 
                   left(0.0f), top(0.0f), renderMode(GlyphRenderMode::NORMAL) {}
    GlyphBitmap(uint32_t w, uint32_t h, uint32_t s, GlyphFormat fmt, 
               const std::vector<uint8>& d, float32 l, float32 t, GlyphRenderMode mode)
        : width(w), height(h), stride(s), format(fmt), data(d), left(l), top(t), renderMode(mode) {}
};

// Glyph outline point
struct OutlinePoint {
    float32 x, y;
    bool onCurve;
    
    OutlinePoint() : x(0.0f), y(0.0f), onCurve(false) {}
    OutlinePoint(float32 x_, float32 y_, bool on) : x(x_), y(y_), onCurve(on) {}
};

// Glyph outline contour
struct OutlineContour {
    std::vector<OutlinePoint> points;
    bool isClosed;
    
    OutlineContour() : isClosed(false) {}
    OutlineContour(const std::vector<OutlinePoint>& pts, bool closed) 
        : points(pts), isClosed(closed) {}
};

// Glyph outline
struct GlyphOutline {
    std::vector<OutlineContour> contours;
    float32 advanceWidth;
    float32 advanceHeight;
    bool hasContours;
    
    GlyphOutline() : advanceWidth(0.0f), advanceHeight(0.0f), hasContours(false) {}
    GlyphOutline(const std::vector<OutlineContour>& ctrs, float32 advW, float32 advH, bool has)
        : contours(ctrs), advanceWidth(advW), advanceHeight(advH), hasContours(has) {}
};

// Glyph path command types
enum class PathCommand {
    MOVE_TO,
    LINE_TO,
    QUAD_TO,
    CUBIC_TO,
    CLOSE_PATH
};

// Glyph path command
struct PathCommandData {
    PathCommand command;
    std::vector<float32> points;
    
    PathCommandData() : command(PathCommand::MOVE_TO) {}
    PathCommandData(PathCommand cmd, const std::vector<float32>& pts) 
        : command(cmd), points(pts) {}
};

// Glyph path
struct GlyphPath {
    std::vector<PathCommandData> commands;
    float32 advanceWidth;
    float32 advanceHeight;
    bool hasPath;
    
    GlyphPath() : advanceWidth(0.0f), advanceHeight(0.0f), hasPath(false) {}
    GlyphPath(const std::vector<PathCommandData>& cmds, float32 advW, float32 advH, bool has)
        : commands(cmds), advanceWidth(advW), advanceHeight(advH), hasPath(has) {}
};

// Glyph SDF data
struct GlyphSDF {
    uint32_t width;
    uint32_t height;
    float32 scale;
    float32 range;
    std::vector<float32> data;
    float32 left;
    float32 top;
    
    GlyphSDF() : width(0), height(0), scale(1.0f), range(1.0f), left(0.0f), top(0.0f) {}
    GlyphSDF(uint32_t w, uint32_t h, float32 s, float32 r, 
            const std::vector<float32>& d, float32 l, float32 t)
        : width(w), height(h), scale(s), range(r), data(d), left(l), top(t) {}
};

// Glyph MSDF data
struct GlyphMSDF {
    uint32_t width;
    uint32_t height;
    float32 scale;
    float32 range;
    std::vector<float32> data; // RGB channels
    float32 left;
    float32 top;
    
    GlyphMSDF() : width(0), height(0), scale(1.0f), range(1.0f), left(0.0f), top(0.0f) {}
    GlyphMSDF(uint32_t w, uint32_t h, float32 s, float32 r, 
             const std::vector<float32>& d, float32 l, float32 t)
        : width(w), height(h), scale(s), range(r), data(d), left(l), top(t) {}
};

// Glyph color layers
struct GlyphColorLayer {
    uint32_t colorIndex;
    GlyphBitmap bitmap;
    
    GlyphColorLayer() : colorIndex(0) {}
    GlyphColorLayer(uint32_t idx, const GlyphBitmap& bmp) : colorIndex(idx), bitmap(bmp) {}
};

// Glyph color data
struct GlyphColorData {
    std::vector<GlyphColorLayer> layers;
    std::vector<std::array<float32, 4>> palettes;
    
    GlyphColorData() {}
    GlyphColorData(const std::vector<GlyphColorLayer>& lys, 
                  const std::vector<std::array<float32, 4>>& pals)
        : layers(lys), palettes(pals) {}
};

// Glyph variation data
struct GlyphVariation {
    std::string tag;
    float32 minValue;
    float32 defaultValue;
    float32 maxValue;
    
    GlyphVariation() : tag(""), minValue(0.0f), defaultValue(0.0f), maxValue(0.0f) {}
    GlyphVariation(const std::string& t, float32 min, float32 def, float32 max)
        : tag(t), minValue(min), defaultValue(def), maxValue(max) {}
};

// Glyph class
class Glyph {
public:
    Glyph();
    virtual ~Glyph() = default;
    
    // Glyph initialization
    virtual void initialize(uint32_t glyphIndex, uint32_t charCode) = 0;
    virtual void setMetrics(const GlyphMetrics& metrics) = 0;
    virtual void setBitmap(const GlyphBitmap& bitmap) = 0;
    virtual void setOutline(const GlyphOutline& outline) = 0;
    virtual void setPath(const GlyphPath& path) = 0;
    virtual void setSDF(const GlyphSDF& sdf) = 0;
    virtual void setMSDF(const GlyphMSDF& msdf) = 0;
    virtual void setColorData(const GlyphColorData& colorData) = 0;
    virtual void setVariations(const std::vector<GlyphVariation>& variations) = 0;
    
    // Glyph information
    virtual uint32_t getGlyphIndex() const = 0;
    virtual uint32_t getCharCode() const = 0;
    virtual GlyphType getType() const = 0;
    virtual const GlyphMetrics& getMetrics() const = 0;
    virtual bool hasBitmap() const = 0;
    virtual bool hasOutline() const = 0;
    virtual bool hasPath() const = 0;
    virtual bool hasSDF() const = 0;
    virtual bool hasMSDF() const = 0;
    virtual bool hasColorData() const = 0;
    virtual bool hasVariations() const = 0;
    
    // Glyph data access
    virtual const GlyphBitmap& getBitmap() const = 0;
    virtual const GlyphOutline& getOutline() const = 0;
    virtual const GlyphPath& getPath() const = 0;
    virtual const GlyphSDF& getSDF() const = 0;
    virtual const GlyphMSDF& getMSDF() const = 0;
    virtual const GlyphColorData& getColorData() const = 0;
    virtual const std::vector<GlyphVariation>& getVariations() const = 0;
    
    // Glyph rendering
    virtual GlyphBitmap renderBitmap(float32 size, GlyphRenderMode mode = GlyphRenderMode::NORMAL) const = 0;
    virtual GlyphSDF renderSDF(float32 size, float32 scale = 1.0f, float32 range = 4.0f) const = 0;
    virtual GlyphMSDF renderMSDF(float32 size, float32 scale = 1.0f, float32 range = 4.0f) const = 0;
    virtual GlyphOutline renderOutline(float32 size) const = 0;
    virtual GlyphPath renderPath(float32 size) const = 0;
    
    // Glyph transformation
    virtual GlyphBitmap transformBitmap(const GlyphBitmap& bitmap, 
                                       float32 scaleX, float32 scaleY, 
                                       float32 translateX, float32 translateY,
                                       float32 rotation = 0.0f) const = 0;
    virtual GlyphOutline transformOutline(const GlyphOutline& outline, 
                                          float32 scaleX, float32 scaleY, 
                                          float32 translateX, float32 translateY,
                                          float32 rotation = 0.0f) const = 0;
    virtual GlyphPath transformPath(const GlyphPath& path, 
                                   float32 scaleX, float32 scaleY, 
                                   float32 translateX, float32 translateY,
                                   float32 rotation = 0.0f) const = 0;
    
    // Glyph variation
    virtual GlyphBitmap renderVariation(const std::map<std::string, float32>& variations, 
                                       float32 size, GlyphRenderMode mode = GlyphRenderMode::NORMAL) const = 0;
    virtual GlyphOutline renderVariationOutline(const std::map<std::string, float32>& variations, 
                                              float32 size) const = 0;
    virtual GlyphPath renderVariationPath(const std::map<std::string, float32>& variations, 
                                         float32 size) const = 0;
    
    // Glyph optimization
    virtual void optimizeForSize(float32 size) = 0;
    virtual void optimizeForRenderMode(GlyphRenderMode mode) = 0;
    virtual void optimizeForFormat(GlyphFormat format) = 0;
    virtual void clearCache() = 0;
    
    // Glyph validation
    virtual bool isValid() const = 0;
    virtual bool validateMetrics() const = 0;
    virtual bool validateBitmap() const = 0;
    virtual bool validateOutline() const = 0;
    virtual bool validatePath() const = 0;
    
    // Glyph comparison
    virtual bool equals(const Glyph& other) const = 0;
    virtual bool equalsMetrics(const Glyph& other) const = 0;
    virtual bool equalsBitmap(const Glyph& other) const = 0;
    virtual bool equalsOutline(const Glyph& other) const = 0;
    virtual bool equalsPath(const Glyph& other) const = 0;
    
    // Glyph serialization
    virtual std::vector<uint8> serialize() const = 0;
    virtual bool deserialize(const std::vector<uint8>& data) = 0;
    virtual size_t getSerializedSize() const = 0;
    
    // Event handling
    virtual void addGlyphEventListener(const std::string& eventType, std::function<void()> callback) = 0;
    virtual void removeGlyphEventListener(const std::string& eventType, std::function<void()> callback) = 0;
    virtual void clearGlyphEventListeners() = 0;
    
    // Utility methods
    virtual void cloneFrom(const Glyph& other) = 0;
    virtual std::unique_ptr<Glyph> clone() const = 0;
    virtual size_t hash() const = 0;
    virtual std::string toString() const = 0;
    
protected:
    // Protected members
    uint32_t glyphIndex_;
    uint32_t charCode_;
    GlyphType type_;
    GlyphMetrics metrics_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    // Protected helper methods
    virtual void triggerGlyphEvent(const std::string& eventType) = 0;
    virtual void updateMetrics() = 0;
    virtual void validateData() = 0;
};

// Specialized glyph classes
class OutlineGlyph : public Glyph {
public:
    OutlineGlyph();
    GlyphType getType() const override;
    bool hasOutline() const override;
    const GlyphOutline& getOutline() const override;
    GlyphOutline renderOutline(float32 size) const override;
    GlyphOutline transformOutline(const GlyphOutline& outline, 
                                 float32 scaleX, float32 scaleY, 
                                 float32 translateX, float32 translateY,
                                 float32 rotation = 0.0f) const override;
    GlyphOutline renderVariationOutline(const std::map<std::string, float32>& variations, 
                                      float32 size) const override;
    bool equalsOutline(const Glyph& other) const override;
    std::unique_ptr<Glyph> clone() const override;
    
protected:
    GlyphOutline outline_;
    
    void triggerGlyphEvent(const std::string& eventType) override;
    void updateMetrics() override;
    void validateData() override;
};

class BitmapGlyph : public Glyph {
public:
    BitmapGlyph();
    GlyphType getType() const override;
    bool hasBitmap() const override;
    const GlyphBitmap& getBitmap() const override;
    GlyphBitmap renderBitmap(float32 size, GlyphRenderMode mode = GlyphRenderMode::NORMAL) const override;
    GlyphBitmap transformBitmap(const GlyphBitmap& bitmap, 
                               float32 scaleX, float32 scaleY, 
                               float32 translateX, float32 translateY,
                               float32 rotation = 0.0f) const override;
    bool equalsBitmap(const Glyph& other) const override;
    std::unique_ptr<Glyph> clone() const override;
    
protected:
    GlyphBitmap bitmap_;
    
    void triggerGlyphEvent(const std::string& eventType) override;
    void updateMetrics() override;
    void validateData() override;
};

class CompositeGlyph : public Glyph {
public:
    CompositeGlyph();
    void addComponent(uint32_t glyphIndex, float32 x, float32 y);
    void removeComponent(uint32_t glyphIndex);
    void clearComponents();
    const std::vector<std::pair<uint32_t, std::pair<float32, float32>>>& getComponents() const;
    GlyphType getType() const override;
    std::unique_ptr<Glyph> clone() const override;
    
protected:
    std::vector<std::pair<uint32_t, std::pair<float32, float32>>> components_;
    
    void triggerGlyphEvent(const std::string& eventType) override;
    void updateMetrics() override;
    void validateData() override;
};

class ColorGlyph : public Glyph {
public:
    ColorGlyph();
    GlyphType getType() const override;
    bool hasColorData() const override;
    const GlyphColorData& getColorData() const override;
    GlyphBitmap renderBitmap(float32 size, GlyphRenderMode mode = GlyphRenderMode::NORMAL) const override;
    std::unique_ptr<Glyph> clone() const override;
    
protected:
    GlyphColorData colorData_;
    
    void triggerGlyphEvent(const std::string& eventType) override;
    void updateMetrics() override;
    void validateData() override;
};

class SDFGlyph : public Glyph {
public:
    SDFGlyph();
    GlyphType getType() const override;
    bool hasSDF() const override;
    const GlyphSDF& getSDF() const override;
    GlyphSDF renderSDF(float32 size, float32 scale = 1.0f, float32 range = 4.0f) const override;
    GlyphBitmap renderBitmap(float32 size, GlyphRenderMode mode = GlyphRenderMode::NORMAL) const override;
    std::unique_ptr<Glyph> clone() const override;
    
protected:
    GlyphSDF sdf_;
    
    void triggerGlyphEvent(const std::string& eventType) override;
    void updateMetrics() override;
    void validateData() override;
};

class MSDFGlyph : public Glyph {
public:
    MSDFGlyph();
    GlyphType getType() const override;
    bool hasMSDF() const override;
    const GlyphMSDF& getMSDF() const override;
    GlyphMSDF renderMSDF(float32 size, float32 scale = 1.0f, float32 range = 4.0f) const override;
    GlyphBitmap renderBitmap(float32 size, GlyphRenderMode mode = GlyphRenderMode::NORMAL) const override;
    std::unique_ptr<Glyph> clone() const override;
    
protected:
    GlyphMSDF msdf_;
    
    void triggerGlyphEvent(const std::string& eventType) override;
    void updateMetrics() override;
    void validateData() override;
};

// Glyph factory
class GlyphFactory {
public:
    static std::unique_ptr<Glyph> createGlyph(GlyphType type);
    static std::unique_ptr<OutlineGlyph> createOutlineGlyph();
    static std::unique_ptr<BitmapGlyph> createBitmapGlyph();
    static std::unique_ptr<CompositeGlyph> createCompositeGlyph();
    static std::unique_ptr<ColorGlyph> createColorGlyph();
    static std::unique_ptr<SDFGlyph> createSDFGlyph();
    static std::unique_ptr<MSDFGlyph> createMSDFGlyph();
    static std::unique_ptr<Glyph> createGlyphFromData(const std::vector<uint8>& data);
    static GlyphType detectGlyphType(const std::vector<uint8>& data);
    static std::vector<GlyphType> getSupportedGlyphTypes();
    static bool isGlyphTypeSupported(GlyphType type);
};

} // namespace RFCore