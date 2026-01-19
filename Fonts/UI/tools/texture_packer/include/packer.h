#pragma once

#include <vector>
#include <string>
#include <memory>

namespace TexturePacker {

struct Rect {
    int x, y, width, height;
    bool flipped;
    int textureID;
    
    Rect() : x(0), y(0), width(0), height(0), flipped(false), textureID(-1) {}
    Rect(int x_, int y_, int w_, int h_, int id) 
        : x(x_), y(y_), width(w_), height(h_), flipped(false), textureID(id) {}
};

struct Texture {
    std::string filename;
    int width, height;
    int channels;
    std::vector<unsigned char> data;
    
    Texture() : width(0), height(0), channels(0) {}
    Texture(const std::string& file, int w, int h, int c, const std::vector<unsigned char>& d)
        : filename(file), width(w), height(h), channels(c), data(d) {}
};

class Packer {
public:
    enum class Algorithm {
        BEST_FIT,
        TOP_LEFT,
        BOTTOM_LEFT,
        MAX_RECTS,
        SKYLINE
    };
    
    struct Settings {
        int maxWidth = 2048;
        int maxHeight = 2048;
        int padding = 2;
        bool enableRotation = false;
        bool forcePowerOfTwo = true;
        Algorithm algorithm = Algorithm::BEST_FIT;
        std::string outputFormat = "png";
        bool generateMipmaps = false;
        bool trimTransparent = true;
        float trimThreshold = 0.01f;
        bool generateMetadata = true;
        std::string metadataFormat = "json";
        bool verbose = false;
    };
    
    struct PackResult {
        std::vector<Rect> rectangles;
        int atlasWidth, atlasHeight;
        float efficiency;
        int totalArea;
        int usedArea;
        std::string algorithmUsed;
        double processingTime;
        size_t texturesProcessed;
        size_t texturesPacked;
    };
    
    Packer();
    ~Packer();
    
    // Main packing function
    PackResult packTextures(const std::vector<std::string>& inputFiles, const std::string& outputFile, const Settings& settings);
    
    // Utility functions
    bool validateInputFiles(const std::vector<std::string>& files);
    std::vector<std::string> getSupportedFormats();
    void printStatistics(const PackResult& result, const Settings& settings);
    
private:
    std::vector<Texture> textures_;
    std::vector<Rect> packedRects_;
    
    // Core packing functions
    bool loadTextures(const std::vector<std::string>& filenames);
    void trimTextures(float threshold);
    std::vector<Rect> packWithAlgorithm(Algorithm algo, int maxWidth, int maxHeight, int padding);
    
    // Algorithm implementations
    std::vector<Rect> bestFitAlgorithm(int maxWidth, int maxHeight, int padding);
    std::vector<Rect> topLeftAlgorithm(int maxWidth, int maxHeight, int padding);
    std::vector<Rect> bottomLeftAlgorithm(int maxWidth, int maxHeight, int padding);
    std::vector<Rect> maxRectsAlgorithm(int maxWidth, int maxHeight, int padding);
    std::vector<Rect> skylineAlgorithm(int maxWidth, int maxHeight, int padding);
    
    // Output functions
    bool saveAtlas(const std::string& filename, const std::vector<Rect>& rects, const Settings& settings);
    bool generateMetadata(const std::string& filename, const std::vector<Rect>& rects, const Settings& settings);
    bool saveMipmaps(const std::string& baseFilename, const std::vector<Rect>& rects, const Settings& settings);
    
    // Utility functions
    void applyPowerOfTwo(std::vector<Rect>& rects, int& width, int& height);
    void applyPadding(std::vector<Rect>& rects, int padding);
    float calculateEfficiency(const std::vector<Rect>& rects, int atlasWidth, int atlasHeight);
    std::string getAlgorithmName(Algorithm algo);
    
    // Image processing
    bool loadImage(const std::string& filename, Texture& texture);
    bool saveImage(const std::string& filename, const std::vector<unsigned char>& data, 
                   int width, int height, int channels, const std::string& format);
    std::vector<unsigned char> trimTexture(const Texture& texture, float threshold);
    
    // Error handling
    void logError(const std::string& message);
    void logInfo(const std::string& message);
    void logWarning(const std::string& message);
    
    // Performance monitoring
    double getCurrentTime();
    void updateProgress(size_t current, size_t total);
};

} // namespace TexturePacker
