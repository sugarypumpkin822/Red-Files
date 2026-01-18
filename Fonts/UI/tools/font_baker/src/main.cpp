#include "baker.h"
#include "rasterizer.h"
#include "writer.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>

namespace FontBaker {

class FontBakerApp {
public:
    FontBakerApp();
    ~FontBakerApp() = default;
    
    int run(int argc, char* argv[]);
    
private:
    std::unique_ptr<FontBaker> baker_;
    std::unique_ptr<GlyphRasterizer> rasterizer_;
    std::unique_ptr<FileWriter> writer_;
    
    void printUsage();
    void printVersion();
    void printSupportedFormats();
    
    bool parseArguments(int argc, char* argv[]);
    void processBatchFile(const std::string& batchFile);
    
    struct CommandLineArgs {
        std::string inputFile;
        std::string outputFile;
        std::string format = "binary";
        std::string configFile;
        std::string batchFile;
        uint32_t fontSize = 32;
        uint32_t atlasSize = 1024;
        bool generateSDF = false;
        bool generateMSDF = false;
        bool verbose = false;
        bool help = false;
        bool version = false;
        bool listFormats = false;
        std::vector<uint32_t> characterSet;
        std::string presetName;
        bool preview = false;
        float sdfSpread = 8.0f;
        uint32_t sdfRange = 4;
        bool optimizeAtlas = true;
        bool includeKerning = true;
        std::string outputDirectory;
    } args_;
    
    FontBaker::BakeSettings createBakeSettings() const;
    void loadCharacterSet();
    void loadPreset(const std::string& presetName);
    void saveConfiguration(const std::string& configFile) const;
    void loadConfiguration(const std::string& configFile);
    void generatePreview(const std::string& inputFile, const std::string& outputFile);
    
    // Progress reporting
    void reportProgress(const std::string& stage, float progress);
    void reportError(const std::string& error);
    void reportWarning(const std::string& warning);
    
    // Performance monitoring
    struct PerformanceMetrics {
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        size_t glyphsProcessed;
        size_t atlasSize;
        float compressionRatio;
        std::string fontName;
        uint32_t fontSize;
    };
    
    PerformanceMetrics metrics_;
    void startPerformanceTracking();
    void endPerformanceTracking();
    void printPerformanceReport() const;
    
    // Validation
    bool validateArguments();
    bool validateFiles();
    bool validateOutputPath();
    
    // Logging
    void logInfo(const std::string& message) const;
    void logError(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logDebug(const std::string& message) const;
    
    // Interactive mode
    void runInteractiveMode();
    void displayInteractiveMenu();
    void handleInteractiveCommand(const std::string& command);
    
    // Batch processing
    struct BatchJob {
        std::string inputFile;
        std::string outputFile;
        FontBaker::BakeSettings settings;
    };
    
    std::vector<BatchJob> batchJobs_;
    void parseBatchFile(const std::string& filename);
    void executeBatchJobs();
    bool processBatchJob(const BatchJob& job);
};

FontBakerApp::FontBakerApp() {
    baker_ = std::make_unique<FontBaker>();
    rasterizer_ = std::make_unique<GlyphRasterizer>();
    writer_ = std::make_unique<FileWriter>();
}

int FontBakerApp::run(int argc, char* argv[]) {
    startPerformanceTracking();
    
    logInfo("Font Baker v1.0.0 - Advanced Font Baking Tool");
    logInfo("===============================================");
    
    if (!parseArguments(argc, argv)) {
        return 1;
    }
    
    if (args_.help) {
        printUsage();
        return 0;
    }
    
    if (args_.version) {
        printVersion();
        return 0;
    }
    
    if (args_.listFormats) {
        printSupportedFormats();
        return 0;
    }
    
    if (!validateArguments()) {
        return 1;
    }
    
    if (!validateFiles()) {
        return 1;
    }
    
    if (!args_.batchFile.empty()) {
        processBatchFile(args_.batchFile);
    } else if (args_.inputFile.empty()) {
        runInteractiveMode();
    } else {
        // Single file processing
        auto settings = createBakeSettings();
        
        reportProgress("Loading font", 0.0f);
        if (!baker_->bakeFont(args_.inputFile, args_.outputFile, settings)) {
            reportError("Failed to bake font: " + args_.inputFile);
            return 1;
        }
        
        reportProgress("Saving output", 0.8f);
        
        if (args_.preview) {
            generatePreview(args_.inputFile, args_.outputFile);
        }
        
        reportProgress("Complete", 1.0f);
        logInfo("Font successfully baked: " + args_.outputFile);
    }
    
    endPerformanceTracking();
    printPerformanceReport();
    
    return 0;
}

void FontBakerApp::printUsage() {
    std::cout << "Font Baker - Advanced Font Baking Tool\n";
    std::cout << "Usage: fontbaker [options] <input_file> [output_file]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -v, --version           Show version information\n";
    std::cout << "  -f, --format <format>   Output format (binary, json, xml, custom) [default: binary]\n";
    std::cout << "  -s, --size <size>       Font size in pixels [default: 32]\n";
    std::cout << "  -a, --atlas <size>      Atlas size in pixels [default: 1024]\n";
    std::cout << "  --sdf                   Generate Signed Distance Field\n";
    std::cout << "  --msdf                  Generate Multi-channel Signed Distance Field\n";
    std::cout << "  --sdf-spread <value>   SDF spread radius [default: 8.0]\n";
    std::cout << "  --sdf-range <value>    SDF pixel range [default: 4]\n";
    std::cout << "  --no-kerning            Disable kerning pairs\n";
    std::cout << "  --no-optimize           Disable atlas optimization\n";
    std::cout << "  --charset <file>        Load character set from file\n";
    std::cout << "  --preset <name>         Use predefined preset\n";
    std::cout << "  --config <file>         Load configuration from file\n";
    std::cout << "  --batch <file>          Process batch file\n";
    std::cout << "  --preview               Generate preview image\n";
    std::cout << "  --output-dir <dir>      Output directory\n";
    std::cout << "  --verbose               Enable verbose output\n";
    std::cout << "  --list-formats          List supported formats\n\n";
    std::cout << "Examples:\n";
    std::cout << "  fontbaker font.ttf output.font\n";
    std::cout << "  fontbaker --format json --size 48 font.ttf output.json\n";
    std::cout << "  fontbaker --sdf --atlas 2048 font.ttf output.font\n";
    std::cout << "  fontbaker --batch jobs.txt\n";
    std::cout << "  fontbaker --preset ui font.ttf output.font\n";
}

void FontBakerApp::printVersion() {
    std::cout << "Font Baker v1.0.0\n";
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n";
    std::cout << "Copyright (c) 2024 Font Baker Team\n";
}

void FontBakerApp::printSupportedFormats() {
    std::cout << "Supported Output Formats:\n";
    std::cout << "  binary   - Custom binary format (recommended)\n";
    std::cout << "  json     - JSON format (human readable)\n";
    std::cout << "  xml      - XML format (structured)\n";
    std::cout << "  custom   - Custom template-based format\n\n";
    std::cout << "Supported Input Formats:\n";
    std::cout << "  TTF      - TrueType Font\n";
    std::cout << "  OTF      - OpenType Font\n";
    std::cout << "  WOFF     - Web Open Font Format\n";
    std::cout << "  WOFF2    - Web Open Font Format 2.0\n";
}

bool FontBakerApp::parseArguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            args_.help = true;
        } else if (arg == "-v" || arg == "--version") {
            args_.version = true;
        } else if (arg == "--list-formats") {
            args_.listFormats = true;
        } else if (arg == "-f" || arg == "--format") {
            if (i + 1 < argc) {
                args_.format = argv[++i];
            }
        } else if (arg == "-s" || arg == "--size") {
            if (i + 1 < argc) {
                args_.fontSize = std::stoul(argv[++i]);
            }
        } else if (arg == "-a" || arg == "--atlas") {
            if (i + 1 < argc) {
                args_.atlasSize = std::stoul(argv[++i]);
            }
        } else if (arg == "--sdf") {
            args_.generateSDF = true;
        } else if (arg == "--msdf") {
            args_.generateMSDF = true;
        } else if (arg == "--sdf-spread") {
            if (i + 1 < argc) {
                args_.sdfSpread = std::stof(argv[++i]);
            }
        } else if (arg == "--sdf-range") {
            if (i + 1 < argc) {
                args_.sdfRange = std::stoul(argv[++i]);
            }
        } else if (arg == "--no-kerning") {
            args_.includeKerning = false;
        } else if (arg == "--no-optimize") {
            args_.optimizeAtlas = false;
        } else if (arg == "--charset") {
            if (i + 1 < argc) {
                loadCharacterSet();
                ++i;
            }
        } else if (arg == "--preset") {
            if (i + 1 < argc) {
                args_.presetName = argv[++i];
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                args_.configFile = argv[++i];
            }
        } else if (arg == "--batch") {
            if (i + 1 < argc) {
                args_.batchFile = argv[++i];
            }
        } else if (arg == "--preview") {
            args_.preview = true;
        } else if (arg == "--output-dir") {
            if (i + 1 < argc) {
                args_.outputDirectory = argv[++i];
            }
        } else if (arg == "--verbose") {
            args_.verbose = true;
        } else if (arg[0] != '-') {
            // Positional argument (input file)
            if (args_.inputFile.empty()) {
                args_.inputFile = arg;
            } else if (args_.outputFile.empty()) {
                args_.outputFile = arg;
            }
        }
    }
    
    return true;
}

FontBaker::BakeSettings FontBakerApp::createBakeSettings() const {
    FontBaker::BakeSettings settings;
    
    settings.fontSize = args_.fontSize;
    settings.atlasSize = args_.atlasSize;
    settings.generateSDF = args_.generateSDF;
    settings.generateMSDF = args_.generateMSDF;
    settings.sdfSpread = args_.sdfSpread;
    settings.sdfRange = args_.sdfRange;
    settings.includeKerning = args_.includeKerning;
    settings.characterSet = args_.characterSet;
    
    // Set output format
    if (args_.format == "json") {
        settings.outputFormat = FontBaker::BakeFormat::JSON;
    } else if (args_.format == "xml") {
        settings.outputFormat = FontBaker::BakeFormat::XML;
    } else if (args_.format == "custom") {
        settings.outputFormat = FontBaker::BakeFormat::CUSTOM;
    } else {
        settings.outputFormat = FontBaker::BakeFormat::BINARY;
    }
    
    return settings;
}

void FontBakerApp::loadCharacterSet() {
    // Default character set (ASCII printable)
    for (uint32_t i = 32; i <= 126; ++i) {
        args_.characterSet.push_back(i);
    }
    
    // Add common Unicode characters
    args_.characterSet.push_back(0x00A9); // ©
    args_.characterSet.push_back(0x00AE); // ®
    args_.characterSet.push_back(0x2122); // ™
    args_.characterSet.push_back(0x2026); // …
    args_.characterSet.push_back(0x2014); // —
    args_.characterSet.push_back(0x2013); // –
    args_.characterSet.push_back(0x201C); // "
    args_.characterSet.push_back(0x201D); // "
    args_.characterSet.push_back(0x2018); // '
    args_.characterSet.push_back(0x2019); // '
    
    logInfo("Loaded default character set with " + std::to_string(args_.characterSet.size()) + " characters");
}

void FontBakerApp::loadPreset(const std::string& presetName) {
    // Define common presets
    if (presetName == "ui") {
        args_.fontSize = 24;
        args_.atlasSize = 512;
        args_.generateSDF = true;
        args_.sdfSpread = 6.0f;
        args_.sdfRange = 3;
    } else if (presetName == "game") {
        args_.fontSize = 32;
        args_.atlasSize = 1024;
        args_.generateSDF = false;
        args_.includeKerning = true;
    } else if (presetName == "web") {
        args_.fontSize = 16;
        args_.atlasSize = 256;
        args_.generateMSDF = true;
        args_.sdfSpread = 8.0f;
        args_.sdfRange = 4;
    } else if (presetName == "hd") {
        args_.fontSize = 64;
        args_.atlasSize = 2048;
        args_.generateSDF = true;
        args_.sdfSpread = 12.0f;
        args_.sdfRange = 8;
    }
    
    logInfo("Applied preset: " + presetName);
}

bool FontBakerApp::validateArguments() {
    if (args_.inputFile.empty() && args_.batchFile.empty()) {
        reportError("No input file specified");
        return false;
    }
    
    if (args_.fontSize < 8 || args_.fontSize > 512) {
        reportError("Font size must be between 8 and 512 pixels");
        return false;
    }
    
    if (args_.atlasSize < 128 || args_.atlasSize > 8192) {
        reportError("Atlas size must be between 128 and 8192 pixels");
        return false;
    }
    
    if (args_.sdfSpread < 1.0f || args_.sdfSpread > 32.0f) {
        reportError("SDF spread must be between 1.0 and 32.0");
        return false;
    }
    
    if (args_.sdfRange < 1 || args_.sdfRange > 16) {
        reportError("SDF range must be between 1 and 16");
        return false;
    }
    
    return true;
}

bool FontBakerApp::validateFiles() {
    if (!args_.inputFile.empty()) {
        std::ifstream file(args_.inputFile);
        if (!file.good()) {
            reportError("Cannot open input file: " + args_.inputFile);
            return false;
        }
    }
    
    if (!args_.batchFile.empty()) {
        std::ifstream file(args_.batchFile);
        if (!file.good()) {
            reportError("Cannot open batch file: " + args_.batchFile);
            return false;
        }
    }
    
    return true;
}

void FontBakerApp::processBatchFile(const std::string& batchFile) {
    logInfo("Processing batch file: " + batchFile);
    
    parseBatchFile(batchFile);
    executeBatchJobs();
    
    logInfo("Batch processing completed. Jobs processed: " + std::to_string(batchJobs_.size()));
}

void FontBakerApp::parseBatchFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        reportError("Cannot open batch file: " + filename);
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;
        
        while (iss >> token) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 2) {
            BatchJob job;
            job.inputFile = tokens[0];
            job.outputFile = tokens[1];
            
            // Parse additional options
            for (size_t i = 2; i < tokens.size(); ++i) {
                if (tokens[i] == "--size" && i + 1 < tokens.size()) {
                    job.settings.fontSize = std::stoul(tokens[++i]);
                } else if (tokens[i] == "--format" && i + 1 < tokens.size()) {
                    std::string format = tokens[++i];
                    if (format == "json") job.settings.outputFormat = FontBaker::BakeFormat::JSON;
                    else if (format == "xml") job.settings.outputFormat = FontBaker::BakeFormat::XML;
                    else if (format == "custom") job.settings.outputFormat = FontBaker::BakeFormat::CUSTOM;
                    else job.settings.outputFormat = FontBaker::BakeFormat::BINARY;
                }
                // Add more option parsing as needed
            }
            
            batchJobs_.push_back(job);
        }
    }
}

void FontBakerApp::executeBatchJobs() {
    size_t successCount = 0;
    size_t totalCount = batchJobs_.size();
    
    for (size_t i = 0; i < batchJobs_.size(); ++i) {
        const auto& job = batchJobs_[i];
        
        reportProgress("Batch job " + std::to_string(i + 1) + "/" + std::to_string(totalCount), 
                     static_cast<float>(i) / totalCount);
        
        if (processBatchJob(job)) {
            successCount++;
        } else {
            logError("Failed to process job: " + job.inputFile);
        }
    }
    
    logInfo("Batch processing completed: " + std::to_string(successCount) + "/" + 
            std::to_string(totalCount) + " jobs successful");
}

bool FontBakerApp::processBatchJob(const BatchJob& job) {
    return baker_->bakeFont(job.inputFile, job.outputFile, job.settings);
}

void FontBakerApp::runInteractiveMode() {
    logInfo("Entering interactive mode. Type 'help' for commands.");
    
    std::string command;
    while (true) {
        std::cout << "fontbaker> ";
        std::getline(std::cin, command);
        
        if (command == "exit" || command == "quit") {
            break;
        } else if (command == "help") {
            displayInteractiveMenu();
        } else {
            handleInteractiveCommand(command);
        }
    }
}

void FontBakerApp::displayInteractiveMenu() {
    std::cout << "\nInteractive Mode Commands:\n";
    std::cout << "  help                 - Show this help\n";
    std::cout << "  load <file>          - Load font file\n";
    std::cout << "  bake <output>        - Bake loaded font\n";
    std::cout << "  size <pixels>         - Set font size\n";
    std::cout << "  atlas <pixels>        - Set atlas size\n";
    std::cout << "  format <type>         - Set output format\n";
    std::cout << "  sdf                  - Enable SDF generation\n";
    std::cout << "  msdf                 - Enable MSDF generation\n";
    std::cout << "  preset <name>         - Load preset\n";
    std::cout << "  info                 - Show current settings\n";
    std::cout << "  preview              - Generate preview\n";
    std::cout << "  exit/quit            - Exit interactive mode\n";
}

void FontBakerApp::handleInteractiveCommand(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    if (cmd == "load") {
        std::string filename;
        iss >> filename;
        args_.inputFile = filename;
        logInfo("Loaded font: " + filename);
    } else if (cmd == "bake") {
        std::string output;
        iss >> output;
        args_.outputFile = output;
        
        auto settings = createBakeSettings();
        if (baker_->bakeFont(args_.inputFile, args_.outputFile, settings)) {
            logInfo("Font successfully baked: " + output);
        } else {
            logError("Failed to bake font");
        }
    } else if (cmd == "size") {
        std::string sizeStr;
        iss >> sizeStr;
        args_.fontSize = std::stoul(sizeStr);
        logInfo("Font size set to: " + sizeStr);
    } else if (cmd == "atlas") {
        std::string sizeStr;
        iss >> sizeStr;
        args_.atlasSize = std::stoul(sizeStr);
        logInfo("Atlas size set to: " + sizeStr);
    } else if (cmd == "format") {
        std::string format;
        iss >> format;
        args_.format = format;
        logInfo("Output format set to: " + format);
    } else if (cmd == "sdf") {
        args_.generateSDF = true;
        args_.generateMSDF = false;
        logInfo("SDF generation enabled");
    } else if (cmd == "msdf") {
        args_.generateMSDF = true;
        args_.generateSDF = false;
        logInfo("MSDF generation enabled");
    } else if (cmd == "preset") {
        std::string preset;
        iss >> preset;
        loadPreset(preset);
    } else if (cmd == "info") {
        std::cout << "\nCurrent Settings:\n";
        std::cout << "  Input File: " << (args_.inputFile.empty() ? "None" : args_.inputFile) << "\n";
        std::cout << "  Output File: " << (args_.outputFile.empty() ? "None" : args_.outputFile) << "\n";
        std::cout << "  Font Size: " << args_.fontSize << "px\n";
        std::cout << "  Atlas Size: " << args_.atlasSize << "px\n";
        std::cout << "  Format: " << args_.format << "\n";
        std::cout << "  SDF: " << (args_.generateSDF ? "Enabled" : "Disabled") << "\n";
        std::cout << "  MSDF: " << (args_.generateMSDF ? "Enabled" : "Disabled") << "\n";
        std::cout << "  Kerning: " << (args_.includeKerning ? "Enabled" : "Disabled") << "\n";
    } else if (cmd == "preview") {
        if (!args_.inputFile.empty()) {
            generatePreview(args_.inputFile, "preview.png");
        } else {
            logError("No font loaded. Use 'load <file>' first.");
        }
    } else {
        logError("Unknown command: " + cmd);
    }
}

void FontBakerApp::generatePreview(const std::string& inputFile, const std::string& outputFile) {
    logInfo("Generating preview: " + outputFile);
    
    auto settings = createBakeSettings();
    settings.atlasSize = 512; // Smaller atlas for preview
    settings.fontSize = 32;
    
    if (baker_->bakeFont(inputFile, outputFile, settings)) {
        logInfo("Preview generated successfully: " + outputFile);
    } else {
        logError("Failed to generate preview");
    }
}

void FontBakerApp::reportProgress(const std::string& stage, float progress) {
    int percent = static_cast<int>(progress * 100);
    std::cout << "\r[" << std::setw(3) << percent << "%] " << stage << std::flush;
    
    if (progress >= 1.0f) {
        std::cout << std::endl;
    }
}

void FontBakerApp::reportError(const std::string& error) {
    std::cerr << "ERROR: " << error << std::endl;
}

void FontBakerApp::reportWarning(const std::string& warning) {
    std::cout << "WARNING: " << warning << std::endl;
}

void FontBakerApp::startPerformanceTracking() {
    metrics_.startTime = std::chrono::high_resolution_clock::now();
    metrics_.glyphsProcessed = 0;
    metrics_.atlasSize = 0;
    metrics_.compressionRatio = 0.0f;
}

void FontBakerApp::endPerformanceTracking() {
    metrics_.endTime = std::chrono::high_resolution_clock::now();
}

void FontBakerApp::printPerformanceReport() const {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        metrics_.endTime - metrics_.startTime).count();
    
    std::cout << "\nPerformance Report:\n";
    std::cout << "==================\n";
    std::cout << "Processing Time: " << duration << " ms\n";
    std::cout << "Glyphs Processed: " << metrics_.glyphsProcessed << "\n";
    std::cout << "Atlas Size: " << metrics_.atlasSize << " bytes\n";
    if (metrics_.compressionRatio > 0.0f) {
        std::cout << "Compression Ratio: " << std::fixed << std::setprecision(2) 
                  << metrics_.compressionRatio << ":1\n";
    }
    std::cout << "Font: " << metrics_.fontName << "\n";
    std::cout << "Size: " << metrics_.fontSize << "px\n";
}

void FontBakerApp::logInfo(const std::string& message) const {
    if (args_.verbose) {
        std::cout << "[INFO] " << message << std::endl;
    }
}

void FontBakerApp::logError(const std::string& message) const {
    std::cerr << "[ERROR] " << message << std::endl;
}

void FontBakerApp::logWarning(const std::string& message) const {
    std::cout << "[WARNING] " << message << std::endl;
}

void FontBakerApp::logDebug(const std::string& message) const {
    if (args_.verbose) {
        std::cout << "[DEBUG] " << message << std::endl;
    }
}

} // namespace FontBaker

int main(int argc, char* argv[]) {
    try {
        FontBaker::FontBakerApp app;
        return app.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}