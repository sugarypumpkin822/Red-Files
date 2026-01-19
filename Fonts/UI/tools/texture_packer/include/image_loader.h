#pragma once

#include <string>
#include <vector>
#include <memory>

namespace TexturePacker {

struct ImageData {
    int width, height, channels;
    std::vector<unsigned char> data;
    std::string filename;
    
    ImageData() : width(0), height(0), channels(0) {}
    ImageData(const std::string& file, int w, int h, int c, const std::vector<unsigned char>& d)
        : filename(file), width(w), height(h), channels(c), data(d) {}
};

class ImageLoader {
public:
    enum class Format {
        UNKNOWN,
        PNG,
        JPEG,
        BMP,
        TGA,
        TIFF,
        WEBP
    };
    
    struct LoadSettings {
        bool flipVertically = false;
        bool flipHorizontally = false;
        bool convertToRGBA = true;
        bool generateMipmaps = false;
        int maxMipmapLevels = 8;
        float gammaCorrection = 1.0f;
        bool premultiplyAlpha = false;
    };
    
    ImageLoader();
    ~ImageLoader();
    
    // Main loading functions
    bool loadImage(const std::string& filename, ImageData& imageData, const LoadSettings& settings = LoadSettings());
    bool loadImages(const std::vector<std::string>& filenames, std::vector<ImageData>& images, 
                    const LoadSettings& settings = LoadSettings());
    
    // Format detection
    static Format detectFormat(const std::string& filename);
    static std::vector<std::string> getSupportedFormats();
    static bool isFormatSupported(const std::string& filename);
    
    // Utility functions
    static bool validateImage(const ImageData& image);
    static void flipImage(ImageData& image, bool vertical, bool horizontal);
    static void convertFormat(ImageData& image, int targetChannels);
    static void applyGamma(ImageData& image, float gamma);
    static void premultiplyAlpha(ImageData& image);
    
    // Image processing
    static void trimImage(ImageData& image, float threshold);
    static std::vector<ImageData> generateMipmaps(const ImageData& image, int maxLevels);
    static void resizeImage(ImageData& image, int newWidth, int newHeight);
    
    // Error handling
    static std::string getLastError();
    static void clearError();
    
private:
    // Format-specific loaders
    static bool loadPNG(const std::string& filename, ImageData& image, const LoadSettings& settings);
    static bool loadJPEG(const std::string& filename, ImageData& image, const LoadSettings& settings);
    static bool loadBMP(const std::string& filename, ImageData& image, const LoadSettings& settings);
    static bool loadTGA(const std::string& filename, ImageData& image, const LoadSettings& settings);
    
    // Format-specific savers
    static bool savePNG(const std::string& filename, const ImageData& image);
    static bool saveJPEG(const std::string& filename, const ImageData& image, int quality = 90);
    static bool saveBMP(const std::string& filename, const ImageData& image);
    static bool saveTGA(const std::string& filename, const ImageData& image);
    
    // Image processing utilities
    static void bilinearResize(const ImageData& src, ImageData& dst, int newWidth, int newHeight);
    static void nearestNeighborResize(const ImageData& src, ImageData& dst, int newWidth, int newHeight);
    static void boxFilterResize(const ImageData& src, ImageData& dst, int newWidth, int newHeight);
    
    // Alpha channel processing
    static void processAlphaChannel(ImageData& image, bool premultiply);
    static void removeAlphaChannel(ImageData& image);
    static void addAlphaChannel(ImageData& image, unsigned char alphaValue = 255);
    
    // Color space conversion
    static void RGBtoRGBA(ImageData& image);
    static void RGBAtoRGB(ImageData& image);
    static void GrayscaleToRGBA(ImageData& image);
    static void RGBAToGrayscale(ImageData& image);
    
    // Utility functions
    static int calculateStride(int width, int channels);
    static bool isValidImageSize(int width, int height);
    static void clampImageDimensions(int& width, int& height, int maxWidth, int maxHeight);
    
    // Error management
    static std::string lastError_;
    static void setError(const std::string& error);
    
    // Memory management
    static void allocateImageData(ImageData& image, int width, int height, int channels);
    static void deallocateImageData(ImageData& image);
    static void copyImageData(const ImageData& src, ImageData& dst);
};

} // namespace TexturePacker
