#include "rasterizer.h"
#include "baker.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>

namespace FontBaker {

class MSDFGeneratorImpl {
public:
    struct MSDFSettings {
        float spread = 8.0f;        // Distance field spread
        float range = 4.0f;         // Distance field range
        float angleThreshold = 3.0f;  // Angle threshold for edge coloring
        bool normalize = true;        // Normalize distances
        bool medianFilter = true;      // Apply median filter
        int padding = 2;              // Padding around the glyph
        float edgeThreshold = 0.5f;   // Edge detection threshold
    };
    
    struct Edge {
        float x1, y1, x2, y2;     // Edge endpoints
        float nx, ny;                 // Edge normal
        float length;                  // Edge length
        int channel;                   // Color channel (0=R, 1=G, 2=B)
    };
    
    struct Contour {
        std::vector<Edge> edges;
        bool clockwise;
    };
    
    MSDFGeneratorImpl();
    ~MSDFGeneratorImpl() = default;
    
    std::vector<uint8_t> generateMSDF(const std::vector<uint8_t>& bitmap, 
                                    int width, int height, const MSDFSettings& settings);
    
private:
    // Edge detection and coloring
    std::vector<Contour> detectContours(const std::vector<uint8_t>& bitmap, int width, int height);
    void colorEdges(std::vector<Contour>& contours, const MSDFSettings& settings);
    float computeEdgeAngle(const Edge& edge);
    
    // Distance field computation
    std::vector<float> computeChannelDistance(const std::vector<Contour>& contours, 
                                           int width, int height, int channel);
    float computeSignedDistance(const std::vector<Contour>& contours, float x, float y);
    float pointToSegmentDistance(float px, float py, float x1, float y1, float x2, float y2);
    
    // Multi-channel distance field
    struct MSDFPixel {
        float r, g, b;
        float median;
    };
    
    std::vector<MSDFPixel> computeMSDF(const std::vector<Contour>& contours, 
                                       int width, int height, const MSDFSettings& settings);
    void blendChannels(std::vector<MSDFPixel>& msdfPixels, const MSDFSettings& settings);
    
    // Post-processing
    void applyMedianFilter(std::vector<MSDFPixel>& msdfPixels, int width, int height);
    void clampDistances(std::vector<MSDFPixel>& msdfPixels, const MSDFSettings& settings);
    std::vector<uint8_t> convertTo8BitMSDF(const std::vector<MSDFPixel>& msdfPixels);
    
    // Helper functions
    bool isEdgePixel(const std::vector<uint8_t>& bitmap, int width, int height, int x, int y);
    float crossProduct(float x1, float y1, float x2, float y2);
    float dotProduct(float x1, float y1, float x2, float y2);
    void normalizeVector(float& x, float& y);
    float computeAngle(float x1, float y1, float x2, float y2);
    
    // Contour tracing
    void traceContour(const std::vector<uint8_t>& bitmap, int width, int height, 
                    int startX, int startY, Contour& contour);
    std::vector<std::pair<int, int>> findNeighbors(int x, int y, int width, int height);
    
    // Edge processing
    Edge createEdge(float x1, float y1, float x2, float y2);
    void computeEdgeNormal(Edge& edge);
};

MSDFGeneratorImpl::MSDFGeneratorImpl() {}

std::vector<uint8_t> MSDFGeneratorImpl::generateMSDF(const std::vector<uint8_t>& bitmap, 
                                                    int width, int height, const MSDFSettings& settings) {
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
    
    // Detect contours
    std::vector<Contour> contours = detectContours(paddedBitmap, paddedWidth, paddedHeight);
    
    // Color edges for multi-channel generation
    colorEdges(contours, settings);
    
    // Compute MSDF
    std::vector<MSDFPixel> msdfPixels = computeMSDF(contours, paddedWidth, paddedHeight, settings);
    
    // Apply post-processing
    if (settings.medianFilter) {
        applyMedianFilter(msdfPixels, paddedWidth, paddedHeight);
    }
    
    clampDistances(msdfPixels, settings);
    
    // Convert to 8-bit
    std::vector<uint8_t> msdfBitmap = convertTo8BitMSDF(msdfPixels);
    
    // Remove padding
    if (settings.padding > 0) {
        std::vector<uint8_t> finalBitmap(width * height * 3); // RGB
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int srcIdx = (y + settings.padding) * paddedWidth + (x + settings.padding);
                int dstIdx = y * width + x;
                
                // Copy RGB channels
                finalBitmap[dstIdx * 3] = msdfBitmap[srcIdx * 3];     // R
                finalBitmap[dstIdx * 3 + 1] = msdfBitmap[srcIdx * 3 + 1]; // G
                finalBitmap[dstIdx * 3 + 2] = msdfBitmap[srcIdx * 3 + 2]; // B
            }
        }
        return finalBitmap;
    }
    
    return msdfBitmap;
}

std::vector<MSDFGeneratorImpl::Contour> MSDFGeneratorImpl::detectContours(const std::vector<uint8_t>& bitmap, 
                                                                   int width, int height) {
    std::vector<Contour> contours;
    std::vector<std::vector<bool>> visited(width, std::vector<bool>(height, false));
    
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            if (!visited[x][y] && isEdgePixel(bitmap, width, height, x, y)) {
                Contour contour;
                traceContour(bitmap, width, height, x, y, contour);
                
                if (!contour.edges.empty()) {
                    // Determine if contour is clockwise
                    float area = 0.0f;
                    for (const auto& edge : contour.edges) {
                        area += (edge.x1 * edge.y2 - edge.x2 * edge.y1);
                    }
                    contour.clockwise = area < 0;
                    
                    contours.push_back(contour);
                    
                    // Mark visited pixels
                    for (const auto& edge : contour.edges) {
                        int x1 = static_cast<int>(edge.x1);
                        int y1 = static_cast<int>(edge.y1);
                        int x2 = static_cast<int>(edge.x2);
                        int y2 = static_cast<int>(edge.y2);
                        
                        if (x1 >= 0 && x1 < width && y1 >= 0 && y1 < height) {
                            visited[x1][y1] = true;
                        }
                        if (x2 >= 0 && x2 < width && y2 >= 0 && y2 < height) {
                            visited[x2][y2] = true;
                        }
                    }
                }
            }
        }
    }
    
    return contours;
}

void MSDFGeneratorImpl::traceContour(const std::vector<uint8_t>& bitmap, int width, int height, 
                                  int startX, int startY, Contour& contour) {
    std::vector<std::pair<int, int>> stack;
    stack.emplace_back(startX, startY);
    
    while (!stack.empty()) {
        auto current = stack.back();
        stack.pop_back();
        
        int x = current.first;
        int y = current.second;
        
        // Find neighboring edge pixels
        auto neighbors = findNeighbors(x, y, width, height);
        
        for (const auto& neighbor : neighbors) {
            int nx = neighbor.first;
            int ny = neighbor.second;
            
            if (isEdgePixel(bitmap, width, height, nx, ny)) {
                // Create edge between current and neighbor
                Edge edge = createEdge(static_cast<float>(x), static_cast<float>(y),
                                       static_cast<float>(nx), static_cast<float>(ny));
                computeEdgeNormal(edge);
                contour.edges.push_back(edge);
                
                // Add neighbor to stack if not processed
                bool processed = false;
                for (const auto& existingEdge : contour.edges) {
                    if ((static_cast<int>(existingEdge.x1) == nx && 
                         static_cast<int>(existingEdge.y1) == ny) ||
                        (static_cast<int>(existingEdge.x2) == nx && 
                         static_cast<int>(existingEdge.y2) == ny)) {
                        processed = true;
                        break;
                    }
                }
                
                if (!processed) {
                    stack.emplace_back(nx, ny);
                }
            }
        }
    }
}

std::vector<std::pair<int, int>> MSDFGeneratorImpl::findNeighbors(int x, int y, int width, int height) {
    std::vector<std::pair<int, int>> neighbors;
    
    // 8-connected neighbors
    const int offsets[8][2] = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1,  0},           {1,  0},
        {-1,  1}, {0,  1}, {1,  1}
    };
    
    for (int i = 0; i < 8; ++i) {
        int nx = x + offsets[i][0];
        int ny = y + offsets[i][1];
        
        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
            neighbors.emplace_back(nx, ny);
        }
    }
    
    return neighbors;
}

void MSDFGeneratorImpl::colorEdges(std::vector<Contour>& contours, const MSDFSettings& settings) {
    for (auto& contour : contours) {
        for (auto& edge : contour.edges) {
            float angle = computeEdgeAngle(edge);
            
            // Assign color channel based on angle
            if (angle < -settings.angleThreshold) {
                edge.channel = 0; // Red
            } else if (angle < settings.angleThreshold) {
                edge.channel = 1; // Green
            } else {
                edge.channel = 2; // Blue
            }
        }
    }
}

float MSDFGeneratorImpl::computeEdgeAngle(const Edge& edge) {
    float dx = edge.x2 - edge.x1;
    float dy = edge.y2 - edge.y1;
    return std::atan2(dy, dx);
}

std::vector<MSDFGeneratorImpl::MSDFPixel> MSDFGeneratorImpl::computeMSDF(const std::vector<Contour>& contours, 
                                                                 int width, int height, const MSDFSettings& settings) {
    std::vector<MSDFPixel> msdfPixels(width * height);
    
    // Compute distance for each channel
    std::vector<float> redDist = computeChannelDistance(contours, width, height, 0);
    std::vector<float> greenDist = computeChannelDistance(contours, width, height, 1);
    std::vector<float> blueDist = computeChannelDistance(contours, width, height, 2);
    
    // Combine channels
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = y * width + x;
            
            MSDFPixel pixel;
            pixel.r = redDist[idx];
            pixel.g = greenDist[idx];
            pixel.b = blueDist[idx];
            pixel.median = (pixel.r + pixel.g + pixel.b) / 3.0f;
            
            msdfPixels[idx] = pixel;
        }
    }
    
    // Blend channels for smooth transitions
    blendChannels(msdfPixels, settings);
    
    return msdfPixels;
}

std::vector<float> MSDFGeneratorImpl::computeChannelDistance(const std::vector<Contour>& contours, 
                                                          int width, int height, int channel) {
    std::vector<float> distanceField(width * height, std::numeric_limits<float>::max());
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float minDistance = std::numeric_limits<float>::max();
            
            // Find minimum distance to edges of this channel
            for (const auto& contour : contours) {
                for (const auto& edge : contour.edges) {
                    if (edge.channel == channel) {
                        float dist = pointToSegmentDistance(
                            static_cast<float>(x), static_cast<float>(y),
                            edge.x1, edge.y1, edge.x2, edge.y2
                        );
                        
                        // Apply sign based on edge normal
                        float nx = static_cast<float>(x) - (edge.x1 + edge.x2) * 0.5f;
                        float ny = static_cast<float>(y) - (edge.y1 + edge.y2) * 0.5f;
                        float dot = nx * edge.nx + ny * edge.ny;
                        
                        if (dot < 0) {
                            dist = -dist;
                        }
                        
                        minDistance = std::min(minDistance, dist);
                    }
                }
            }
            
            distanceField[y * width + x] = minDistance;
        }
    }
    
    return distanceField;
}

float MSDFGeneratorImpl::computeSignedDistance(const std::vector<Contour>& contours, float x, float y) {
    float minDistance = std::numeric_limits<float>::max();
    
    for (const auto& contour : contours) {
        for (const auto& edge : contour.edges) {
            float dist = pointToSegmentDistance(x, y, edge.x1, edge.y1, edge.x2, edge.y2);
            
            // Apply sign based on contour orientation
            float nx = x - (edge.x1 + edge.x2) * 0.5f;
            float ny = y - (edge.y1 + edge.y2) * 0.5f;
            float dot = nx * edge.nx + ny * edge.ny;
            
            if (dot < 0) {
                dist = -dist;
            }
            
            minDistance = std::min(minDistance, dist);
        }
    }
    
    return minDistance;
}

float MSDFGeneratorImpl::pointToSegmentDistance(float px, float py, float x1, float y1, float x2, float y2) {
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

void MSDFGeneratorImpl::blendChannels(std::vector<MSDFPixel>& msdfPixels, const MSDFSettings& settings) {
    // Simple channel blending - more sophisticated blending could be implemented
    for (auto& pixel : msdfPixels) {
        // Use median for stability
        float median = pixel.median;
        
        // Blend based on distance magnitude
        float weight = std::exp(-std::abs(median) / settings.spread);
        
        pixel.r = pixel.r * weight + median * (1.0f - weight);
        pixel.g = pixel.g * weight + median * (1.0f - weight);
        pixel.b = pixel.b * weight + median * (1.0f - weight);
    }
}

void MSDFGeneratorImpl::applyMedianFilter(std::vector<MSDFPixel>& msdfPixels, int width, int height) {
    std::vector<MSDFPixel> filtered = msdfPixels;
    
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            std::vector<float> redValues, greenValues, blueValues;
            
            // Collect 3x3 neighborhood
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    int idx = (y + dy) * width + (x + dx);
                    redValues.push_back(msdfPixels[idx].r);
                    greenValues.push_back(msdfPixels[idx].g);
                    blueValues.push_back(msdfPixels[idx].b);
                }
            }
            
            // Apply median filter
            std::sort(redValues.begin(), redValues.end());
            std::sort(greenValues.begin(), greenValues.end());
            std::sort(blueValues.begin(), blueValues.end());
            
            int idx = y * width + x;
            filtered[idx].r = redValues[4]; // Median of 9 values
            filtered[idx].g = greenValues[4];
            filtered[idx].b = blueValues[4];
        }
    }
    
    msdfPixels = filtered;
}

void MSDFGeneratorImpl::clampDistances(std::vector<MSDFPixel>& msdfPixels, const MSDFSettings& settings) {
    for (auto& pixel : msdfPixels) {
        pixel.r = std::clamp(pixel.r, -settings.spread, settings.spread);
        pixel.g = std::clamp(pixel.g, -settings.spread, settings.spread);
        pixel.b = std::clamp(pixel.b, -settings.spread, settings.spread);
    }
}

std::vector<uint8_t> MSDFGeneratorImpl::convertTo8BitMSDF(const std::vector<MSDFPixel>& msdfPixels) {
    std::vector<uint8_t> result(msdfPixels.size() * 3); // RGB
    
    for (size_t i = 0; i < msdfPixels.size(); ++i) {
        const auto& pixel = msdfPixels[i];
        
        // Map each channel from [-spread, spread] to [0, 255]
        result[i * 3] = static_cast<uint8_t>(std::clamp((pixel.r + 1.0f) * 127.5f, 0.0f, 255.0f));
        result[i * 3 + 1] = static_cast<uint8_t>(std::clamp((pixel.g + 1.0f) * 127.5f, 0.0f, 255.0f));
        result[i * 3 + 2] = static_cast<uint8_t>(std::clamp((pixel.b + 1.0f) * 127.5f, 0.0f, 255.0f));
    }
    
    return result;
}

bool MSDFGeneratorImpl::isEdgePixel(const std::vector<uint8_t>& bitmap, int width, int height, int x, int y) {
    if (x <= 0 || x >= width - 1 || y <= 0 || y >= height - 1) {
        return false;
    }
    
    uint8_t center = bitmap[y * width + x];
    
    // Check 8 neighbors
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            
            uint8_t neighbor = bitmap[(y + dy) * width + (x + dx)];
            if ((center > 128) != (neighbor > 128)) {
                return true;
            }
        }
    }
    
    return false;
}

Edge MSDFGeneratorImpl::createEdge(float x1, float y1, float x2, float y2) {
    Edge edge;
    edge.x1 = x1;
    edge.y1 = y1;
    edge.x2 = x2;
    edge.y2 = y2;
    edge.length = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    edge.channel = 0; // Will be set later
    computeEdgeNormal(edge);
    return edge;
}

void MSDFGeneratorImpl::computeEdgeNormal(Edge& edge) {
    float dx = edge.x2 - edge.x1;
    float dy = edge.y2 - edge.y1;
    float length = std::sqrt(dx * dx + dy * dy);
    
    if (length > 0.0f) {
        edge.nx = -dy / length;
        edge.ny = dx / length;
    } else {
        edge.nx = 0.0f;
        edge.ny = 0.0f;
    }
}

float MSDFGeneratorImpl::crossProduct(float x1, float y1, float x2, float y2) {
    return x1 * y2 - y1 * x2;
}

float MSDFGeneratorImpl::dotProduct(float x1, float y1, float x2, float y2) {
    return x1 * x2 + y1 * y2;
}

void MSDFGeneratorImpl::normalizeVector(float& x, float& y) {
    float length = std::sqrt(x * x + y * y);
    if (length > 0.0f) {
        x /= length;
        y /= length;
    }
}

float MSDFGeneratorImpl::computeAngle(float x1, float y1, float x2, float y2) {
    return std::atan2(y2 - y1, x2 - x1);
}

// MSDFGenerator interface implementation
MSDFGenerator::MSDFGenerator() : impl_(std::make_unique<MSDFGeneratorImpl>()) {}

MSDFGenerator::~MSDFGenerator() = default;

std::vector<uint8_t> MSDFGenerator::generateMSDF(const std::vector<uint8_t>& bitmap, 
                                               int width, int height, float spread, float range) {
    MSDFGeneratorImpl::MSDFSettings settings;
    settings.spread = spread;
    settings.range = range;
    return impl_->generateMSDF(bitmap, width, height, settings);
}

std::vector<uint8_t> MSDFGenerator::generateMSDFWithMedianFilter(const std::vector<uint8_t>& bitmap, 
                                                              int width, int height, float spread, float range) {
    MSDFGeneratorImpl::MSDFSettings settings;
    settings.spread = spread;
    settings.range = range;
    settings.medianFilter = true;
    return impl_->generateMSDF(bitmap, width, height, settings);
}

} // namespace FontBaker