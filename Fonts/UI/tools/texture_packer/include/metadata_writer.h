#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>

namespace TexturePacker {

struct Rect {
    int x, y, width, height;
    bool flipped;
    int textureID;
    
    Rect() : x(0), y(0), width(0), height(0), flipped(false), textureID(-1) {}
    Rect(int x_, int y_, int w_, int h_, int id) 
        : x(x_), y(y_), width(w_), height(h_), flipped(false), textureID(id) {}
};

struct TextureInfo {
    std::string filename;
    int width, height;
    int channels;
    size_t size;
    
    TextureInfo() : width(0), height(0), channels(0), size(0) {}
    TextureInfo(const std::string& file, int w, int h, int c, size_t s)
        : filename(file), width(w), height(h), channels(c), size(s) {}
};

class MetadataWriter {
public:
    enum class Format {
        JSON,
        XML,
        CSV,
        BINARY,
        CUSTOM
    };
    
    struct Settings {
        Format format = Format::JSON;
        bool includeTextureInfo = true;
        bool includeAtlasInfo = true;
        bool includeRectangles = true;
        bool includeStatistics = true;
        bool includeTimestamp = true;
        bool includeVersion = true;
        std::string customTemplate = "";
        bool prettyPrint = true;
        bool compressOutput = false;
    };
    
    struct AtlasInfo {
        int width, height;
        int channels;
        std::string format;
        float efficiency;
        int totalArea;
        int usedArea;
        std::string algorithm;
        double processingTime;
        size_t textureCount;
        size_t rectangleCount;
    };
    
    struct Statistics {
        int totalTextures;
        int packedTextures;
        int skippedTextures;
        int totalArea;
        int usedArea;
        float packingEfficiency;
        double averageTextureSize;
        int largestTextureArea;
        int smallestTextureArea;
        std::map<std::string, int> textureFormats;
    };
    
    MetadataWriter();
    ~MetadataWriter();
    
    // Main writing functions
    bool writeMetadata(const std::string& filename, const std::vector<Rect>& rectangles, 
                     const std::vector<TextureInfo>& textures, const AtlasInfo& atlasInfo, 
                     const Settings& settings);
    
    // Format-specific writers
    bool writeJSON(const std::string& filename, const std::vector<Rect>& rectangles, 
                   const std::vector<TextureInfo>& textures, const AtlasInfo& atlasInfo, 
                   const Settings& settings);
    bool writeXML(const std::string& filename, const std::vector<Rect>& rectangles, 
                 const std::vector<TextureInfo>& textures, const AtlasInfo& atlasInfo, 
                 const Settings& settings);
    bool writeCSV(const std::string& filename, const std::vector<Rect>& rectangles, 
                 const std::vector<TextureInfo>& textures, const AtlasInfo& atlasInfo, 
                 const Settings& settings);
    bool writeBinary(const std::string& filename, const std::vector<Rect>& rectangles, 
                   const std::vector<TextureInfo>& textures, const AtlasInfo& atlasInfo, 
                   const Settings& settings);
    bool writeCustom(const std::string& filename, const std::vector<Rect>& rectangles, 
                    const std::vector<TextureInfo>& textures, const AtlasInfo& atlasInfo, 
                    const Settings& settings);
    
    // Utility functions
    static std::string formatSize(size_t size);
    static std::string formatTime(double seconds);
    static std::string formatPercentage(float percentage);
    static std::string escapeJSONString(const std::string& str);
    static std::string escapeXMLString(const std::string& str);
    
    // Statistics calculation
    Statistics calculateStatistics(const std::vector<Rect>& rectangles, 
                               const std::vector<TextureInfo>& textures, const AtlasInfo& atlasInfo);
    
    // Template processing
    bool loadTemplate(const std::string& templateFile);
    bool processTemplate(const std::string& templateContent, std::string& output, 
                        const std::vector<Rect>& rectangles, const std::vector<TextureInfo>& textures, 
                        const AtlasInfo& atlasInfo, const Statistics& stats);
    
    // Error handling
    static std::string getLastError();
    static void clearError();
    
private:
    // JSON helpers
    void writeJSONValue(std::ofstream& file, const std::string& key, const std::string& value, bool last = false);
    void writeJSONValue(std::ofstream& file, const std::string& key, int value, bool last = false);
    void writeJSONValue(std::ofstream& file, const std::string& key, float value, bool last = false);
    void writeJSONValue(std::ofstream& file, const std::string& key, bool value, bool last = false);
    void writeJSONArrayStart(std::ofstream& file, const std::string& key);
    void writeJSONArrayEnd(std::ofstream& file);
    void writeJSONObjectStart(std::ofstream& file, const std::string& key);
    void writeJSONObjectEnd(std::ofstream& file);
    
    // XML helpers
    void writeXMLHeader(std::ofstream& file);
    void writeXMLFooter(std::ofstream& file);
    void writeXMLElement(std::ofstream& file, const std::string& tag, const std::string& content);
    void writeXMLElementStart(std::ofstream& file, const std::string& tag, const std::map<std::string, std::string>& attributes = {});
    void writeXMLElementEnd(std::ofstream& file, const std::string& tag);
    void writeXMLAttribute(std::ofstream& file, const std::string& name, const std::string& value);
    
    // CSV helpers
    void writeCSVHeader(std::ofstream& file);
    void writeCSVRow(std::ofstream& file, const std::vector<std::string>& values);
    std::string escapeCSVField(const std::string& field);
    
    // Binary helpers
    bool writeBinaryHeader(std::ofstream& file, const std::vector<Rect>& rectangles, 
                         const std::vector<TextureInfo>& textures, const AtlasInfo& atlasInfo);
    bool writeBinaryData(std::ofstream& file, const std::vector<Rect>& rectangles, 
                       const std::vector<TextureInfo>& textures);
    
    // Template variables
    std::map<std::string, std::string> templateVariables_;
    
    // Error management
    static std::string lastError_;
    static void setError(const std::string& error);
    
    // Compression
    bool compressOutput(const std::string& filename, const std::string& content);
    std::string compressString(const std::string& input);
};

} // namespace TexturePacker
