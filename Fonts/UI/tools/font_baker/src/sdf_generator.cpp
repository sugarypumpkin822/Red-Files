#include "rasterizer.h"
#include "baker.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <queue>

namespace FontBaker {

class SDFGeneratorImpl {
public:
    struct SDFSettings {
        float spread = 8.0f;        // Distance field spread
        float range = 4.0f;         // Distance field range
        bool normalize = true;        // Normalize distances
        bool invert = false;          // Invert the distance field
        float scale = 1.0f;          // Scale factor for distances
        int padding = 2;              // Padding around the glyph
    };
    
    SDFGeneratorImpl();
    ~SDFGeneratorImpl() = default;
    
    std::vector<uint8_t> generateSDF(const std::vector<uint8_t>& bitmap, 
                                   int width, int height, const SDFSettings& settings);
    
private:
    // Distance transform algorithms
    std::vector<float> computeDistanceField(const std::vector<uint8_t>& bitmap, 
                                         int width, int height);
    std::vector<float> computeDistanceField8SSE(const std::vector<uint8_t>& bitmap, 
                                             int width, int height);
    std::vector<float> computeDistanceFieldBruteForce(const std::vector<uint8_t>& bitmap, 
                                                 int width, int height);
    
    // Fast sweeping algorithm
    void sweepPass(std::vector<float>& distanceField, int width, int height, 
                  int startX, int endX, int startY, int endY, int stepX, int stepY);
    float computeDistance(const std::vector<float>& distanceField, int width, int height,
                       int x, int y, int neighborX, int neighborY);
    
    // Distance field processing
    void normalizeDistanceField(std::vector<float>& distanceField, const SDFSettings& settings);
    void applyDistanceRange(std::vector<float>& distanceField, const SDFSettings& settings);
    std::vector<uint8_t> convertTo8Bit(const std::vector<float>& distanceField);
    
    // Helper functions
    float euclideanDistance(int x1, int y1, int x2, int y2);
    bool isInside(const std::vector<uint8_t>& bitmap, int width, int height, int x, int y);
    float getPixel(const std::vector<float>& field, int width, int height, int x, int y);
    void setPixel(std::vector<float>& field, int width, int height, int x, int y, float value);
    
    // Optimization structures
    struct Pixel {
        int x, y;
        float distance;
        bool operator>(const Pixel& other) const { return distance > other.distance; }
    };
    
    // 8-connected neighbor offsets
    static const int neighborOffsets[8][2];
};

const int SDFGeneratorImpl::neighborOffsets[8][2] = {
    {-1, -1}, {0, -1}, {1, -1},
    {-1,  0},           {1,  0},
    {-1,  1}, {0,  1}, {1,  1}
};

SDFGeneratorImpl::SDFGeneratorImpl() {}

std::vector<uint8_t> SDFGeneratorImpl::generateSDF(const std::vector<uint8_t>& bitmap, 
                                                     int width, int height, const SDFSettings& settings) {
    if (bitmap.empty() || width <= 0 || height <= 0) {
        return std::vector<uint8_t>();
    }
    
    // Add padding if needed
    int paddedWidth = width + settings.padding * 2;
    int paddedHeight = height + settings.padding * 2;
    std::vector<uint8_t> paddedBitmap(paddedWidth * paddedHeight, 0);
    
    // Copy original bitmap to center of padded bitmap
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int srcIdx = y * width + x;
            int dstIdx = (y + settings.padding) * paddedWidth + (x + settings.padding);
            paddedBitmap[dstIdx] = bitmap[srcIdx];
        }
    }
    
    // Compute distance field
    std::vector<float> distanceField = computeDistanceField(paddedBitmap, paddedWidth, paddedHeight);
    
    // Apply settings
    if (settings.normalize) {
        normalizeDistanceField(distanceField, settings);
    }
    
    applyDistanceRange(distanceField, settings);
    
    if (settings.invert) {
        for (float& distance : distanceField) {
            distance = -distance;
        }
    }
    
    // Apply scale
    if (settings.scale != 1.0f) {
        for (float& distance : distanceField) {
            distance *= settings.scale;
        }
    }
    
    // Convert back to 8-bit
    std::vector<uint8_t> sdfBitmap = convertTo8Bit(distanceField);
    
    // Remove padding
    if (settings.padding > 0) {
        std::vector<uint8_t> finalBitmap(width * height);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int srcIdx = (y + settings.padding) * paddedWidth + (x + settings.padding);
                int dstIdx = y * width + x;
                finalBitmap[dstIdx] = sdfBitmap[srcIdx];
            }
        }
        return finalBitmap;
    }
    
    return sdfBitmap;
}

std::vector<float> SDFGeneratorImpl::computeDistanceField(const std::vector<uint8_t>& bitmap, 
                                                       int width, int height) {
    // Use fast sweeping algorithm for performance
    std::vector<float> distanceField(width * height);
    
    // Initialize distance field
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (isInside(bitmap, width, height, x, y)) {
                distanceField[y * width + x] = 0.0f;
            } else {
                distanceField[y * width + x] = std::numeric_limits<float>::max();
            }
        }
    }
    
    // Forward pass
    sweepPass(distanceField, width, height, 0, width, 0, height, 1, 1);
    
    // Backward pass
    sweepPass(distanceField, width, height, width - 1, -1, height - 1, -1, -1, -1);
    
    return distanceField;
}

void SDFGeneratorImpl::sweepPass(std::vector<float>& distanceField, int width, int height,
                                int startX, int endX, int startY, int endY, int stepX, int stepY) {
    for (int y = startY; y != endY; y += stepY) {
        for (int x = startX; x != endX; x += stepX) {
            float minDistance = distanceField[y * width + x];
            
            // Check all 8 neighbors
            for (int i = 0; i < 8; ++i) {
                int nx = x + neighborOffsets[i][0];
                int ny = y + neighborOffsets[i][1];
                
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    float neighborDist = distanceField[ny * width + nx];
                    if (neighborDist != std::numeric_limits<float>::max()) {
                        float dist = computeDistance(distanceField, width, height, x, y, nx, ny);
                        minDistance = std::min(minDistance, dist);
                    }
                }
            }
            
            distanceField[y * width + x] = minDistance;
        }
    }
}

float SDFGeneratorImpl::computeDistance(const std::vector<float>& distanceField, int width, int height,
                                     int x, int y, int neighborX, int neighborY) {
    float neighborDist = distanceField[neighborY * width + neighborX];
    float dx = static_cast<float>(x - neighborX);
    float dy = static_cast<float>(y - neighborY);
    float edgeDistance = std::sqrt(dx * dx + dy * dy);
    
    return neighborDist + edgeDistance;
}

std::vector<float> SDFGeneratorImpl::computeDistanceField8SSE(const std::vector<uint8_t>& bitmap, 
                                                           int width, int height) {
    // SIMD-optimized version would go here
    // For now, fall back to regular implementation
    return computeDistanceField(bitmap, width, height);
}

std::vector<float> SDFGeneratorImpl::computeDistanceFieldBruteForce(const std::vector<uint8_t>& bitmap, 
                                                               int width, int height) {
    std::vector<float> distanceField(width * height);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float minDistance = std::numeric_limits<float>::max();
            
            // Find distance to nearest edge
            for (int ey = 0; ey < height; ++ey) {
                for (int ex = 0; ex < width; ++ex) {
                    bool insidePixel = isInside(bitmap, width, height, ex, ey);
                    bool currentPixel = isInside(bitmap, width, height, x, y);
                    
                    if (insidePixel != currentPixel) {
                        float dist = euclideanDistance(x, y, ex, ey);
                        minDistance = std::min(minDistance, dist);
                    }
                }
            }
            
            // Sign the distance
            if (isInside(bitmap, width, height, x, y)) {
                distanceField[y * width + x] = minDistance;
            } else {
                distanceField[y * width + x] = -minDistance;
            }
        }
    }
    
    return distanceField;
}

void SDFGeneratorImpl::normalizeDistanceField(std::vector<float>& distanceField, const SDFSettings& settings) {
    if (distanceField.empty()) return;
    
    // Find min and max distances
    float minDist = std::numeric_limits<float>::max();
    float maxDist = -std::numeric_limits<float>::max();
    
    for (float distance : distanceField) {
        if (distance != std::numeric_limits<float>::max()) {
            minDist = std::min(minDist, distance);
            maxDist = std::max(maxDist, distance);
        }
    }
    
    // Normalize to [-1, 1] range
    float range = maxDist - minDist;
    if (range > 0.0f) {
        for (float& distance : distanceField) {
            if (distance != std::numeric_limits<float>::max()) {
                distance = (distance - minDist) / range * 2.0f - 1.0f;
            }
        }
    }
}

void SDFGeneratorImpl::applyDistanceRange(std::vector<float>& distanceField, const SDFSettings& settings) {
    for (float& distance : distanceField) {
        // Clamp to range and scale
        distance = std::clamp(distance, -settings.spread, settings.spread);
        distance = distance / settings.spread * settings.range;
    }
}

std::vector<uint8_t> SDFGeneratorImpl::convertTo8Bit(const std::vector<float>& distanceField) {
    std::vector<uint8_t> result(distanceField.size());
    
    for (size_t i = 0; i < distanceField.size(); ++i) {
        // Map from [-range, range] to [0, 255]
        float normalized = (distanceField[i] + 1.0f) * 0.5f; // Map to [0, 1]
        result[i] = static_cast<uint8_t>(std::clamp(normalized * 255.0f, 0.0f, 255.0f));
    }
    
    return result;
}

float SDFGeneratorImpl::euclideanDistance(int x1, int y1, int x2, int y2) {
    float dx = static_cast<float>(x2 - x1);
    float dy = static_cast<float>(y2 - y1);
    return std::sqrt(dx * dx + dy * dy);
}

bool SDFGeneratorImpl::isInside(const std::vector<uint8_t>& bitmap, int width, int height, int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    
    return bitmap[y * width + x] > 128;
}

float SDFGeneratorImpl::getPixel(const std::vector<float>& field, int width, int height, int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return std::numeric_limits<float>::max();
    }
    
    return field[y * width + x];
}

void SDFGeneratorImpl::setPixel(std::vector<float>& field, int width, int height, int x, int y, float value) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        field[y * width + x] = value;
    }
}

// SDFGenerator interface implementation
SDFGenerator::SDFGenerator() : impl_(std::make_unique<SDFGeneratorImpl>()) {}

SDFGenerator::~SDFGenerator() = default;

std::vector<uint8_t> SDFGenerator::generateSDF(const std::vector<uint8_t>& bitmap, 
                                               int width, int height, float spread, float range) {
    SDFGeneratorImpl::SDFSettings settings;
    settings.spread = spread;
    settings.range = range;
    return impl_->generateSDF(bitmap, width, height, settings);
}

std::vector<uint8_t> SDFGenerator::generateSDFWithPadding(const std::vector<uint8_t>& bitmap, 
                                                           int width, int height, float spread, float range, int padding) {
    SDFGeneratorImpl::SDFSettings settings;
    settings.spread = spread;
    settings.range = range;
    settings.padding = padding;
    return impl_->generateSDF(bitmap, width, height, settings);
}

std::vector<uint8_t> SDFGenerator::generateNormalizedSDF(const std::vector<uint8_t>& bitmap, 
                                                         int width, int height, float spread) {
    SDFGeneratorImpl::SDFSettings settings;
    settings.spread = spread;
    settings.range = 1.0f;
    settings.normalize = true;
    return impl_->generateSDF(bitmap, width, height, settings);
}

} // namespace FontBaker