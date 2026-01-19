#pragma once

// Core texture packing library header
// Includes all necessary headers for the texture packing system

#include "packer.h"
#include "algorithms.h"
#include "image_loader.h"
#include "metadata_writer.h"
#include "utils.h"

namespace TexturePacker {

// Version information
struct Version {
    static const int MAJOR = 1;
    static const int MINOR = 0;
    static const int PATCH = 0;
    static const char* const STRING = "1.0.0";
    
    static std::string getVersionString() {
        return std::to_string(MAJOR) + "." + std::to_string(MINOR) + "." + std::to_string(PATCH);
    }
};

// Main texture packing interface
class TexturePacker {
public:
    // Convenience functions for common use cases
    static Packer::PackResult packTextures(const std::vector<std::string>& inputFiles, 
                                              const std::string& outputFile, 
                                              const Packer::Settings& settings = Packer::Settings());
    
    static Packer::PackResult packTexturesQuick(const std::vector<std::string>& inputFiles, 
                                                 const std::string& outputFile);
    
    static Packer::PackResult packTexturesMaxQuality(const std::vector<std::string>& inputFiles, 
                                                   const std::string& outputFile);
    
    // Batch processing
    static bool packBatch(const std::vector<std::vector<std::string>>& batches, 
                        const std::vector<std::string>& outputFiles, 
                        const std::vector<Packer::Settings>& settings);
    
    // Validation and information
    static bool validateInputFiles(const std::vector<std::string>& files);
    static std::vector<std::string> getSupportedImageFormats();
    static void printSystemInfo();
    
    // Utility functions
    static std::string getLastError();
    static void clearError();
    static void setLogLevel(Utils::LogLevel level);
    
private:
    // Internal helper functions
    static Packer::Settings getOptimalSettings(const std::vector<std::string>& files);
    static std::vector<std::string> filterValidFiles(const std::vector<std::string>& files);
    static void logPackStart(const std::vector<std::string>& files, const std::string& outputFile);
    static void logPackComplete(const Packer::PackResult& result);
};

// Factory functions for creating specialized packers
class PackerFactory {
public:
    enum class PackerType {
        DEFAULT,
        FAST,
        QUALITY,
        MEMORY_EFFICIENT,
        CUSTOM
    };
    
    static std::unique_ptr<Packer> createPacker(PackerType type = PackerType::DEFAULT);
    static std::unique_ptr<Packer> createCustomPacker(const std::string& algorithmName);
    static std::vector<std::string> getAvailablePackerTypes();
};

// Command line interface helpers
namespace CLI {
    struct CommandLineOptions {
        std::vector<std::string> inputFiles;
        std::string outputFile;
        Packer::Settings settings;
        bool showHelp = false;
        bool showVersion = false;
        bool verbose = false;
        std::string configFile;
    };
    
    bool parseCommandLine(int argc, char* argv[], CommandLineOptions& options);
    void printHelp();
    void printVersion();
    void printError(const std::string& message);
}

// Configuration file support
namespace Config {
    struct ConfigFile {
        Packer::Settings defaultSettings;
        std::map<std::string, Packer::Settings> presets;
        std::vector<std::string> recentFiles;
        std::string defaultOutputPath;
    };
    
    bool loadConfigFile(const std::string& filename, ConfigFile& config);
    bool saveConfigFile(const std::string& filename, const ConfigFile& config);
    std::string getDefaultConfigPath();
}

// Plugin interface for custom algorithms
class PackerPlugin {
public:
    virtual ~PackerPlugin() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    virtual PackerAlgorithm::PackResult pack(const std::vector<PackerAlgorithm::Rectangle>& rects, 
                                                   int maxWidth, int maxHeight, bool rotate) = 0;
    
    virtual bool initialize(const std::map<std::string, std::string>& parameters) = 0;
    virtual void cleanup() = 0;
};

// Plugin manager
class PluginManager {
public:
    static bool loadPlugin(const std::string& pluginPath);
    static bool unloadPlugin(const std::string& pluginName);
    static std::vector<std::string> getLoadedPlugins();
    static PackerPlugin* getPlugin(const std::string& name);
    static std::vector<std::string> getAvailablePlugins(const std::string& pluginDirectory);
};

} // namespace TexturePacker
