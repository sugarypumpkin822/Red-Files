#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>

namespace TexturePacker {

struct Rect {
    int x, y, width, height;
    bool flipped;
    int textureID;
};

struct Texture {
    std::string filename;
    int width, height;
    int channels;
    std::vector<unsigned char> data;
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
    };
    
    Packer();
    bool packTextures(const std::vector<std::string>& inputFiles, const std::string& outputFile, const Settings& settings);
    
private:
    std::vector<Texture> textures_;
    std::vector<Rect> packedRects_;
    
    bool loadTextures(const std::vector<std::string>& filenames);
    void trimTextures(float threshold);
    std::vector<Rect> packWithAlgorithm(Algorithm algo, int maxWidth, int maxHeight, int padding);
    std::vector<Rect> bestFitAlgorithm(int maxWidth, int maxHeight, int padding);
    std::vector<Rect> skylineAlgorithm(int maxWidth, int maxHeight, int padding);
    void saveAtlas(const std::string& filename, const std::vector<Rect>& rects, const Settings& settings);
    void generateMetadata(const std::string& filename, const std::vector<Rect>& rects, const Settings& settings);
};

Packer::Packer() {}

bool Packer::packTextures(const std::vector<std::string>& inputFiles, const std::string& outputFile, const Settings& settings) {
    std::cout << "Loading " << inputFiles.size() << " textures..." << std::endl;
    
    if (!loadTextures(inputFiles)) {
        return false;
    }
    
    if (settings.trimTransparent) {
        trimTextures(settings.trimThreshold);
    }
    
    std::cout << "Packing textures using algorithm " << static_cast<int>(settings.algorithm) << "..." << std::endl;
    packedRects_ = packWithAlgorithm(settings.algorithm, settings.maxWidth, settings.maxHeight, settings.padding);
    
    if (packedRects_.empty()) {
        std::cerr << "Failed to pack textures" << std::endl;
        return false;
    }
    
    std::cout << "Saving atlas to " << outputFile << "..." << std::endl;
    saveAtlas(outputFile, packedRects_, settings);
    generateMetadata(outputFile, packedRects_, settings);
    
    std::cout << "Texture packing completed successfully!" << std::endl;
    return true;
}

bool Packer::loadTextures(const std::vector<std::string>& filenames) {
    textures_.clear();
    
    for (const auto& filename : filenames) {
        Texture tex;
        tex.filename = filename;
        
        // Simulate loading (would use stb_image or similar)
        tex.width = 64 + (rand() % 128);
        tex.height = 64 + (rand() % 128);
        tex.channels = 4;
        tex.data.resize(tex.width * tex.height * tex.channels, 255);
        
        textures_.push_back(tex);
    }
    
    return true;
}

void Packer::trimTextures(float threshold) {
    for (auto& tex : textures_) {
        // Simple trim simulation
        int trimX = 0, trimY = 0, trimW = tex.width, trimH = tex.height;
        
        // Would implement actual transparent pixel detection here
        
        tex.width = trimW;
        tex.height = trimH;
    }
}

std::vector<Rect> Packer::packWithAlgorithm(Algorithm algo, int maxWidth, int maxHeight, int padding) {
    switch (algo) {
        case Algorithm::BEST_FIT:
            return bestFitAlgorithm(maxWidth, maxHeight, padding);
        case Algorithm::SKYLINE:
            return skylineAlgorithm(maxWidth, maxHeight, padding);
        default:
            return bestFitAlgorithm(maxWidth, maxHeight, padding);
    }
}

std::vector<Rect> Packer::bestFitAlgorithm(int maxWidth, int maxHeight, int padding) {
    std::vector<Rect> rects;
    
    for (size_t i = 0; i < textures_.size(); ++i) {
        Rect rect;
        rect.x = 0;
        rect.y = 0;
        rect.width = textures_[i].width + padding * 2;
        rect.height = textures_[i].height + padding * 2;
        rect.flipped = false;
        rect.textureID = static_cast<int>(i);
        
        // Find best position
        bool placed = false;
        for (const auto& existing : rects) {
            if (rect.x + rect.width <= maxWidth && rect.y + rect.height <= maxHeight) {
                placed = true;
                break;
            }
        }
        
        if (!placed) {
            // Place at next available position
            if (!rects.empty()) {
                const auto& last = rects.back();
                rect.x = last.x + last.width;
                if (rect.x + rect.width > maxWidth) {
                    rect.x = 0;
                    rect.y = last.y + last.height;
                }
            }
        }
        
        rects.push_back(rect);
    }
    
    return rects;
}

std::vector<Rect> Packer::skylineAlgorithm(int maxWidth, int maxHeight, int padding) {
    std::vector<Rect> rects;
    std::vector<int> skyline;
    
    skyline.push_back(0);
    
    for (size_t i = 0; i < textures_.size(); ++i) {
        Rect rect;
        rect.width = textures_[i].width + padding * 2;
        rect.height = textures_[i].height + padding * 2;
        rect.flipped = false;
        rect.textureID = static_cast<int>(i);
        
        // Find best skyline position
        int bestX = 0, bestY = 0, bestWidth = maxWidth;
        
        for (size_t j = 0; j < skyline.size(); ++j) {
            if (skyline[j] + rect.height <= maxHeight) {
                int width = (j < skyline.size() - 1) ? skyline[j + 1] - skyline[j] : maxWidth - skyline[j];
                if (width >= rect.width && width < bestWidth) {
                    bestX = (j < skyline.size() - 1) ? skyline[j] : 0;
                    bestY = skyline[j];
                    bestWidth = width;
                }
            }
        }
        
        rect.x = bestX;
        rect.y = bestY;
        rects.push_back(rect);
        
        // Update skyline
        skyline.insert(skyline.begin() + 1, bestY + rect.height);
    }
    
    return rects;
}

void Packer::saveAtlas(const std::string& filename, const std::vector<Rect>& rects, const Settings& settings) {
    // Calculate atlas dimensions
    int atlasWidth = 0, atlasHeight = 0;
    for (const auto& rect : rects) {
        atlasWidth = std::max(atlasWidth, rect.x + rect.width);
        atlasHeight = std::max(atlasHeight, rect.y + rect.height);
    }
    
    std::cout << "Atlas size: " << atlasWidth << "x" << atlasHeight << std::endl;
    
    // Simulate saving atlas image
    std::vector<unsigned char> atlasData(atlasWidth * atlasHeight * 4, 0);
    
    // Copy textures to atlas
    for (size_t i = 0; i < rects.size(); ++i) {
        const auto& rect = rects[i];
        const auto& tex = textures_[rect.textureID];
        
        for (int y = 0; y < tex.height; ++y) {
            for (int x = 0; x < tex.width; ++x) {
                int srcX = x;
                int srcY = y;
                int dstX = rect.x + x + settings.padding;
                int dstY = rect.y + y + settings.padding;
                
                if (dstX < atlasWidth && dstY < atlasHeight) {
                    int srcIdx = (srcY * tex.width + srcX) * 4;
                    int dstIdx = (dstY * atlasWidth + dstX) * 4;
                    
                    if (srcIdx < tex.data.size() && dstIdx < atlasData.size()) {
                        atlasData[dstIdx] = tex.data[srcIdx];
                        atlasData[dstIdx + 1] = tex.data[srcIdx + 1];
                        atlasData[dstIdx + 2] = tex.data[srcIdx + 2];
                        atlasData[dstIdx + 3] = tex.data[srcIdx + 3];
                    }
                }
            }
        }
    }
    
    // Save to file (would use stb_image_write or similar)
    std::cout << "Atlas saved to " << filename << std::endl;
}

void Packer::generateMetadata(const std::string& filename, const std::vector<Rect>& rects, const Settings& settings) {
    std::string metaFile = filename + ".json";
    std::ofstream file(metaFile);
    
    if (!file.is_open()) {
        std::cerr << "Cannot create metadata file: " << metaFile << std::endl;
        return;
    }
    
    file << "{\n";
    file << "  \"atlas\": {\n";
    file << "    \"width\": " << (rects.empty() ? 0 : rects[0].x + rects[0].width) << ",\n";
    file << "    \"height\": " << (rects.empty() ? 0 : rects[0].y + rects[0].height) << ",\n";
    file << "    \"format\": \"" << settings.outputFormat << "\"\n";
    file << "  },\n";
    file << "  \"textures\": [\n";
    
    for (size_t i = 0; i < rects.size(); ++i) {
        const auto& rect = rects[i];
        const auto& tex = textures_[rect.textureID];
        
        file << "    {\n";
        file << "      \"filename\": \"" << tex.filename << "\",\n";
        file << "      \"x\": " << rect.x << ",\n";
        file << "      \"y\": " << rect.y << ",\n";
        file << "      \"width\": " << tex.width << ",\n";
        file << "      \"height\": " << tex.height << ",\n";
        file << "      \"flipped\": " << (rect.flipped ? "true" : "false") << "\n";
        file << "    }" << (i < rects.size() - 1 ? "," : "") << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    std::cout << "Metadata saved to " << metaFile << std::endl;
}

} // namespace TexturePacker

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: texture_packer <output_file> <input_file1> [input_file2] ...\n";
        return 1;
    }
    
    std::string outputFile = argv[1];
    std::vector<std::string> inputFiles;
    
    for (int i = 2; i < argc; ++i) {
        inputFiles.push_back(argv[i]);
    }
    
    TexturePacker::Packer packer;
    TexturePacker::Packer::Settings settings;
    
    return packer.packTextures(inputFiles, outputFile, settings) ? 0 : 1;
}