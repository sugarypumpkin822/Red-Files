#include "rasterizer.h"
#include "baker.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstring>

namespace FontBaker {

class GlyphRasterizerImpl {
public:
    struct RasterizationSettings {
        uint32_t fontSize = 32;
        float scale = 1.0f;
        bool antiAliasing = true;
        bool subpixelRendering = false;
        float gamma = 1.0f;
        bool autoHinting = true;
        bool hinting = true;
        uint32_t renderMode = 0; // Normal, LCD, SDF, MSDF
    };
    
    GlyphRasterizerImpl();
    ~GlyphRasterizerImpl() = default;
    
    // Standard rasterization
    std::vector<uint8_t> rasterizeGlyph(const Glyph& glyph, const RasterizationSettings& settings);
    std::vector<uint8_t> rasterizeGlyphSubpixel(const Glyph& glyph, const RasterizationSettings& settings);
    
    // Advanced rasterization
    std::vector<uint8_t> rasterizeGlyphLCD(const Glyph& glyph, const RasterizationSettings& settings);
    std::vector<uint8_t> rasterizeGlyphSDF(const Glyph& glyph, const RasterizationSettings& settings);
    std::vector<uint8_t> rasterizeGlyphMSDF(const Glyph& glyph, const RasterizationSettings& settings);
    
    // Utility functions
    void applyGammaCorrection(std::vector<uint8_t>& bitmap, float gamma);
    void applyContrast(std::vector<uint8_t>& bitmap, float contrast);
    void applyFilter(std::vector<uint8_t>& bitmap, const std::vector<float>& kernel, int width, int height);
    
private:
    // Scanline rasterization
    void rasterizeOutline(const std::vector<OutlinePoint>& outline, std::vector<uint8_t>& bitmap, 
                        int width, int height, float scale);
    void fillScanline(std::vector<uint8_t>& bitmap, int width, int y, int x1, int x2, uint8_t value);
    
    // Anti-aliasing
    void applySupersampling(std::vector<uint8_t>& bitmap, int width, int height, int samples);
    void applyGaussianBlur(std::vector<uint8_t>& bitmap, int width, int height, float sigma);
    
    // Edge detection for SDF
    std::vector<float> computeDistanceField(const std::vector<uint8_t>& bitmap, int width, int height);
    float computePixelDistance(const std::vector<uint8_t>& bitmap, int width, int height, int x, int y);
    
    // MSDF generation
    struct MSDFPixel {
        float r, g, b;
        float distance;
    };
    
    std::vector<MSDFPixel> computeMultiChannelDistanceField(const std::vector<uint8_t>& bitmap, 
                                                         int width, int height);
    void computeChannelDistances(const std::vector<uint8_t>& bitmap, int width, int height,
                               std::vector<float>& redDist, std::vector<float>& greenDist, 
                               std::vector<float>& blueDist);
    
    // Helper functions
    float lerp(float a, float b, float t);
    uint8_t floatToByte(float value);
    float byteToFloat(uint8_t value);
    bool isInsideOutline(const std::vector<OutlinePoint>& outline, float x, float y);
    float pointToLineDistance(float px, float py, float x1, float y1, float x2, float y2);
};

GlyphRasterizerImpl::GlyphRasterizerImpl() {}

std::vector<uint8_t> GlyphRasterizerImpl::rasterizeGlyph(const Glyph& glyph, const RasterizationSettings& settings) {
    // Calculate bitmap dimensions
    int width = static_cast<int>(std::ceil(glyph.boundingBox.width * settings.scale * settings.fontSize));
    int height = static_cast<int>(std::ceil(glyph.boundingBox.height * settings.scale * settings.fontSize));
    
    if (width <= 0 || height <= 0) {
        return std::vector<uint8_t>();
    }
    
    // Create bitmap buffer
    std::vector<uint8_t> bitmap(width * height, 0);
    
    // Rasterize the glyph outline
    rasterizeOutline(glyph.outlinePoints, bitmap, width, height, settings.scale * settings.fontSize);
    
    // Apply anti-aliasing if enabled
    if (settings.antiAliasing) {
        if (settings.subpixelRendering) {
            applySupersampling(bitmap, width, height, 4);
        } else {
            applyGaussianBlur(bitmap, width, height, 0.5f);
        }
    }
    
    // Apply gamma correction
    if (std::abs(settings.gamma - 1.0f) > 0.001f) {
        applyGammaCorrection(bitmap, settings.gamma);
    }
    
    return bitmap;
}

std::vector<uint8_t> GlyphRasterizerImpl::rasterizeGlyphSubpixel(const Glyph& glyph, const RasterizationSettings& settings) {
    // Subpixel rendering uses 3x horizontal samples
    int width = static_cast<int>(std::ceil(glyph.boundingBox.width * settings.scale * settings.fontSize)) * 3;
    int height = static_cast<int>(std::ceil(glyph.boundingBox.height * settings.scale * settings.fontSize));
    
    if (width <= 0 || height <= 0) {
        return std::vector<uint8_t>();
    }
    
    std::vector<uint8_t> bitmap(width * height, 0);
    
    // Rasterize at 3x horizontal resolution
    RasterizationSettings subpixelSettings = settings;
    subpixelSettings.scale *= 3.0f;
    rasterizeOutline(glyph.outlinePoints, bitmap, width, height, subpixelSettings.scale * settings.fontSize);
    
    // Downsample with proper subpixel filtering
    std::vector<uint8_t> finalBitmap(width / 3 * height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width / 3; ++x) {
            int srcX = x * 3;
            float sum = 0.0f;
            
            // Weighted average for subpixel filtering
            sum += bitmap[y * width + srcX] * 0.25f;     // Red channel
            sum += bitmap[y * width + srcX + 1] * 0.5f; // Green channel
            sum += bitmap[y * width + srcX + 2] * 0.25f; // Blue channel
            
            finalBitmap[y * (width / 3) + x] = static_cast<uint8_t>(std::min(255.0f, sum));
        }
    }
    
    return finalBitmap;
}

std::vector<uint8_t> GlyphRasterizerImpl::rasterizeGlyphLCD(const Glyph& glyph, const RasterizationSettings& settings) {
    // LCD rendering uses 3x horizontal resolution for RGB subpixels
    int width = static_cast<int>(std::ceil(glyph.boundingBox.width * settings.scale * settings.fontSize)) * 3;
    int height = static_cast<int>(std::ceil(glyph.boundingBox.height * settings.scale * settings.fontSize));
    
    if (width <= 0 || height <= 0) {
        return std::vector<uint8_t>();
    }
    
    std::vector<uint8_t> bitmap(width * height, 0);
    
    // Rasterize at 3x horizontal resolution
    RasterizationSettings lcdSettings = settings;
    lcdSettings.scale *= 3.0f;
    rasterizeOutline(glyph.outlinePoints, bitmap, width, height, lcdSettings.scale * settings.fontSize);
    
    // Apply LCD-specific filtering
    applyGaussianBlur(bitmap, width, height, 0.3f);
    
    return bitmap;
}

std::vector<uint8_t> GlyphRasterizerImpl::rasterizeGlyphSDF(const Glyph& glyph, const RasterizationSettings& settings) {
    // First rasterize normally
    std::vector<uint8_t> normalBitmap = rasterizeGlyph(glyph, settings);
    
    if (normalBitmap.empty()) {
        return std::vector<uint8_t>();
    }
    
    int width = static_cast<int>(std::ceil(glyph.boundingBox.width * settings.scale * settings.fontSize));
    int height = static_cast<int>(std::ceil(glyph.boundingBox.height * settings.scale * settings.fontSize));
    
    // Compute signed distance field
    std::vector<float> distanceField = computeDistanceField(normalBitmap, width, height);
    
    // Convert distance field back to 8-bit
    std::vector<uint8_t> sdfBitmap(width * height);
    for (size_t i = 0; i < distanceField.size(); ++i) {
        // Map distance to [0, 255] range
        float normalizedDistance = (distanceField[i] + 8.0f) / 16.0f; // Assuming 8 pixel spread
        sdfBitmap[i] = floatToByte(std::clamp(normalizedDistance, 0.0f, 1.0f));
    }
    
    return sdfBitmap;
}

std::vector<uint8_t> GlyphRasterizerImpl::rasterizeGlyphMSDF(const Glyph& glyph, const RasterizationSettings& settings) {
    // First rasterize normally
    std::vector<uint8_t> normalBitmap = rasterizeGlyph(glyph, settings);
    
    if (normalBitmap.empty()) {
        return std::vector<uint8_t>();
    }
    
    int width = static_cast<int>(std::ceil(glyph.boundingBox.width * settings.scale * settings.fontSize));
    int height = static_cast<int>(std::ceil(glyph.boundingBox.height * settings.scale * settings.fontSize));
    
    // Compute multi-channel signed distance field
    std::vector<MSDFPixel> msdfField = computeMultiChannelDistanceField(normalBitmap, width, height);
    
    // Convert MSDF to 8-bit (average of channels for simplicity)
    std::vector<uint8_t> msdfBitmap(width * height);
    for (size_t i = 0; i < msdfField.size(); ++i) {
        float average = (msdfField[i].r + msdfField[i].g + msdfField[i].b) / 3.0f;
        float normalizedDistance = (average + 8.0f) / 16.0f;
        msdfBitmap[i] = floatToByte(std::clamp(normalizedDistance, 0.0f, 1.0f));
    }
    
    return msdfBitmap;
}

void GlyphRasterizerImpl::rasterizeOutline(const std::vector<OutlinePoint>& outline, 
                                        std::vector<uint8_t>& bitmap, int width, int height, float scale) {
    if (outline.empty()) return;
    
    // Clear bitmap
    std::fill(bitmap.begin(), bitmap.end(), 0);
    
    // Simple scanline rasterization
    for (int y = 0; y < height; ++y) {
        std::vector<float> intersections;
        
        // Find all intersections of the scanline with the outline
        for (size_t i = 0; i < outline.size(); ++i) {
            size_t next = (i + 1) % outline.size();
            
            float y1 = outline[i].y * scale;
            float y2 = outline[next].y * scale;
            float x1 = outline[i].x * scale;
            float x2 = outline[next].x * scale;
            
            // Check if scanline intersects with this edge
            if ((y1 <= static_cast<float>(y) && y2 > static_cast<float>(y)) ||
                (y2 <= static_cast<float>(y) && y1 > static_cast<float>(y))) {
                
                // Calculate x intersection
                float t = (static_cast<float>(y) - y1) / (y2 - y1);
                float x = x1 + t * (x2 - x1);
                intersections.push_back(x);
            }
        }
        
        // Sort intersections
        std::sort(intersections.begin(), intersections.end());
        
        // Fill between pairs of intersections
        for (size_t i = 0; i < intersections.size(); i += 2) {
            if (i + 1 < intersections.size()) {
                int x1 = static_cast<int>(std::floor(intersections[i]));
                int x2 = static_cast<int>(std::ceil(intersections[i + 1]));
                
                x1 = std::max(0, x1);
                x2 = std::min(width - 1, x2);
                
                for (int x = x1; x <= x2; ++x) {
                    bitmap[y * width + x] = 255;
                }
            }
        }
    }
}

void GlyphRasterizerImpl::fillScanline(std::vector<uint8_t>& bitmap, int width, int y, int x1, int x2, uint8_t value) {
    x1 = std::max(0, std::min(width - 1, x1));
    x2 = std::max(0, std::min(width - 1, x2));
    
    if (x1 > x2) std::swap(x1, x2);
    
    for (int x = x1; x <= x2; ++x) {
        bitmap[y * width + x] = value;
    }
}

void GlyphRasterizerImpl::applySupersampling(std::vector<uint8_t>& bitmap, int width, int height, int samples) {
    std::vector<uint8_t> original = bitmap;
    bitmap.assign(width * height, 0);
    
    float sampleScale = 1.0f / (samples * samples);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float sum = 0.0f;
            
            for (int sy = 0; sy < samples; ++sy) {
                for (int sx = 0; sx < samples; ++sx) {
                    float sampleX = (x + sx / static_cast<float>(samples));
                    float sampleY = (y + sy / static_cast<float>(samples));
                    
                    int srcX = static_cast<int>(sampleX);
                    int srcY = static_cast<int>(sampleY);
                    
                    if (srcX >= 0 && srcX < width && srcY >= 0 && srcY < height) {
                        sum += original[srcY * width + srcX];
                    }
                }
            }
            
            bitmap[y * width + x] = static_cast<uint8_t>(sum * sampleScale);
        }
    }
}

void GlyphRasterizerImpl::applyGaussianBlur(std::vector<uint8_t>& bitmap, int width, int height, float sigma) {
    // Create Gaussian kernel
    int kernelSize = static_cast<int>(std::ceil(sigma * 3.0f)) * 2 + 1;
    std::vector<float> kernel(kernelSize);
    float sum = 0.0f;
    
    int center = kernelSize / 2;
    for (int i = 0; i < kernelSize; ++i) {
        float x = i - center;
        kernel[i] = std::exp(-(x * x) / (2.0f * sigma * sigma));
        sum += kernel[i];
    }
    
    // Normalize kernel
    for (float& value : kernel) {
        value /= sum;
    }
    
    // Apply kernel
    std::vector<uint8_t> original = bitmap;
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float blurredValue = 0.0f;
            
            for (int ky = 0; ky < kernelSize; ++ky) {
                for (int kx = 0; kx < kernelSize; ++kx) {
                    int srcX = x + kx - center;
                    int srcY = y + ky - center;
                    
                    if (srcX >= 0 && srcX < width && srcY >= 0 && srcY < height) {
                        blurredValue += original[srcY * width + srcX] * kernel[ky] * kernel[kx];
                    }
                }
            }
            
            bitmap[y * width + x] = static_cast<uint8_t>(std::round(blurredValue));
        }
    }
}

std::vector<float> GlyphRasterizerImpl::computeDistanceField(const std::vector<uint8_t>& bitmap, int width, int height) {
    std::vector<float> distanceField(width * height);
    
    // Initialize distance field
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            distanceField[y * width + x] = computePixelDistance(bitmap, width, height, x, y);
        }
    }
    
    return distanceField;
}

float GlyphRasterizerImpl::computePixelDistance(const std::vector<uint8_t>& bitmap, int width, int height, int x, int y) {
    bool isInside = bitmap[y * width + x] > 128;
    
    float minDistance = std::numeric_limits<float>::max();
    
    // Search for nearest edge pixel
    int searchRadius = 8; // Limit search radius for performance
    for (int dy = -searchRadius; dy <= searchRadius; ++dy) {
        for (int dx = -searchRadius; dx <= searchRadius; ++dx) {
            int nx = x + dx;
            int ny = y + dy;
            
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                bool neighborInside = bitmap[ny * width + nx] > 128;
                
                if (isInside != neighborInside) {
                    float distance = std::sqrt(dx * dx + dy * dy);
                    minDistance = std::min(minDistance, distance);
                }
            }
        }
    }
    
    return isInside ? minDistance : -minDistance;
}

std::vector<GlyphRasterizerImpl::MSDFPixel> GlyphRasterizerImpl::computeMultiChannelDistanceField(
    const std::vector<uint8_t>& bitmap, int width, int height) {
    
    std::vector<MSDFPixel> msdfField(width * height);
    
    // Compute separate distance fields for each channel
    std::vector<float> redDist(width * height);
    std::vector<float> greenDist(width * height);
    std::vector<float> blueDist(width * height);
    
    computeChannelDistances(bitmap, width, height, redDist, greenDist, blueDist);
    
    // Combine into MSDF
    for (size_t i = 0; i < msdfField.size(); ++i) {
        msdfField[i].r = redDist[i];
        msdfField[i].g = greenDist[i];
        msdfField[i].b = blueDist[i];
        msdfField[i].distance = (redDist[i] + greenDist[i] + blueDist[i]) / 3.0f;
    }
    
    return msdfField;
}

void GlyphRasterizerImpl::computeChannelDistances(const std::vector<uint8_t>& bitmap, int width, int height,
                                              std::vector<float>& redDist, std::vector<float>& greenDist, 
                                              std::vector<float>& blueDist) {
    // Simplified MSDF - in a real implementation, this would use proper edge coloring
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float distance = computePixelDistance(bitmap, width, height, x, y);
            
            // Assign different channels based on position (simplified)
            if ((x + y) % 3 == 0) {
                redDist[y * width + x] = distance;
                greenDist[y * width + x] = std::abs(distance) * 0.5f;
                blueDist[y * width + x] = std::abs(distance) * 0.5f;
            } else if ((x + y) % 3 == 1) {
                redDist[y * width + x] = std::abs(distance) * 0.5f;
                greenDist[y * width + x] = distance;
                blueDist[y * width + x] = std::abs(distance) * 0.5f;
            } else {
                redDist[y * width + x] = std::abs(distance) * 0.5f;
                greenDist[y * width + x] = std::abs(distance) * 0.5f;
                blueDist[y * width + x] = distance;
            }
        }
    }
}

void GlyphRasterizerImpl::applyGammaCorrection(std::vector<uint8_t>& bitmap, float gamma) {
    for (uint8_t& pixel : bitmap) {
        float normalized = pixel / 255.0f;
        float corrected = std::pow(normalized, 1.0f / gamma);
        pixel = floatToByte(corrected);
    }
}

void GlyphRasterizerImpl::applyContrast(std::vector<uint8_t>& bitmap, float contrast) {
    float factor = (259.0f * (contrast + 255.0f)) / (255.0f * (259.0f - contrast));
    
    for (uint8_t& pixel : bitmap) {
        float value = factor * (pixel - 128.0f) + 128.0f;
        pixel = static_cast<uint8_t>(std::clamp(value, 0.0f, 255.0f));
    }
}

void GlyphRasterizerImpl::applyFilter(std::vector<uint8_t>& bitmap, const std::vector<float>& kernel, int width, int height) {
    int kernelSize = static_cast<int>(std::sqrt(kernel.size()));
    int center = kernelSize / 2;
    
    std::vector<uint8_t> original = bitmap;
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float filteredValue = 0.0f;
            
            for (int ky = 0; ky < kernelSize; ++ky) {
                for (int kx = 0; kx < kernelSize; ++kx) {
                    int srcX = x + kx - center;
                    int srcY = y + ky - center;
                    
                    if (srcX >= 0 && srcX < width && srcY >= 0 && srcY < height) {
                        filteredValue += original[srcY * width + srcX] * kernel[ky * kernelSize + kx];
                    }
                }
            }
            
            bitmap[y * width + x] = static_cast<uint8_t>(std::clamp(filteredValue, 0.0f, 255.0f));
        }
    }
}

float GlyphRasterizerImpl::lerp(float a, float b, float t) {
    return a + t * (b - a);
}

uint8_t GlyphRasterizerImpl::floatToByte(float value) {
    return static_cast<uint8_t>(std::clamp(value * 255.0f, 0.0f, 255.0f));
}

float GlyphRasterizerImpl::byteToFloat(uint8_t value) {
    return value / 255.0f;
}

bool GlyphRasterizerImpl::isInsideOutline(const std::vector<OutlinePoint>& outline, float x, float y) {
    // Ray casting algorithm
    int intersections = 0;
    
    for (size_t i = 0; i < outline.size(); ++i) {
        size_t next = (i + 1) % outline.size();
        
        float y1 = outline[i].y;
        float y2 = outline[next].y;
        float x1 = outline[i].x;
        float x2 = outline[next].x;
        
        if ((y1 <= y && y2 > y) || (y2 <= y && y1 > y)) {
            float xIntersect = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
            if (xIntersect > x) {
                ++intersections;
            }
        }
    }
    
    return (intersections % 2) == 1;
}

float GlyphRasterizerImpl::pointToLineDistance(float px, float py, float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float lengthSquared = dx * dx + dy * dy;
    
    if (lengthSquared == 0.0f) {
        return std::sqrt((px - x1) * (px - x1) + (py - y1) * (py - y1));
    }
    
    float t = std::clamp(((px - x1) * dx + (py - y1) * dy) / lengthSquared, 0.0f, 1.0f);
    float projectionX = x1 + t * dx;
    float projectionY = y1 + t * dy;
    
    return std::sqrt((px - projectionX) * (px - projectionX) + (py - projectionY) * (py - projectionY));
}

// GlyphRasterizer interface implementation
GlyphRasterizer::GlyphRasterizer() : impl_(std::make_unique<GlyphRasterizerImpl>()) {}

GlyphRasterizer::~GlyphRasterizer() = default;

std::vector<uint8_t> GlyphRasterizer::rasterizeGlyph(const Glyph& glyph, uint32_t fontSize, bool antiAliasing) {
    GlyphRasterizerImpl::RasterizationSettings settings;
    settings.fontSize = fontSize;
    settings.antiAliasing = antiAliasing;
    return impl_->rasterizeGlyph(glyph, settings);
}

std::vector<uint8_t> GlyphRasterizer::rasterizeGlyphSubpixel(const Glyph& glyph, uint32_t fontSize) {
    GlyphRasterizerImpl::RasterizationSettings settings;
    settings.fontSize = fontSize;
    settings.subpixelRendering = true;
    settings.antiAliasing = true;
    return impl_->rasterizeGlyphSubpixel(glyph, settings);
}

std::vector<uint8_t> GlyphRasterizer::rasterizeGlyphLCD(const Glyph& glyph, uint32_t fontSize) {
    GlyphRasterizerImpl::RasterizationSettings settings;
    settings.fontSize = fontSize;
    settings.renderMode = 1; // LCD mode
    return impl_->rasterizeGlyphLCD(glyph, settings);
}

std::vector<uint8_t> GlyphRasterizer::rasterizeGlyphSDF(const Glyph& glyph, uint32_t fontSize, float spread) {
    GlyphRasterizerImpl::RasterizationSettings settings;
    settings.fontSize = fontSize;
    settings.renderMode = 2; // SDF mode
    return impl_->rasterizeGlyphSDF(glyph, settings);
}

std::vector<uint8_t> GlyphRasterizer::rasterizeGlyphMSDF(const Glyph& glyph, uint32_t fontSize, float spread) {
    GlyphRasterizerImpl::RasterizationSettings settings;
    settings.fontSize = fontSize;
    settings.renderMode = 3; // MSDF mode
    return impl_->rasterizeGlyphMSDF(glyph, settings);
}

} // namespace FontBaker