#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace TexturePacker {

class Utils {
public:
    // String utilities
    static std::string getFileExtension(const std::string& filename);
    static std::string getFileName(const std::string& filepath);
    static std::string getDirectoryPath(const std::string& filepath);
    static std::string combinePath(const std::string& directory, const std::string& filename);
    static bool fileExists(const std::string& filename);
    static bool createDirectory(const std::string& path);
    static std::string normalizePath(const std::string& path);
    
    // Math utilities
    static bool isPowerOfTwo(int value);
    static int nextPowerOfTwo(int value);
    static int previousPowerOfTwo(int value);
    static int clamp(int value, int min, int max);
    static float clamp(float value, float min, float max);
    static double clamp(double value, double min, double max);
    static int roundUp(int value, int multiple);
    static int roundDown(int value, int multiple);
    static float lerp(float a, float b, float t);
    static double lerp(double a, double b, double t);
    
    // Rectangle utilities
    struct Rectangle {
        int x, y, width, height;
        Rectangle() : x(0), y(0), width(0), height(0) {}
        Rectangle(int x_, int y_, int w_, int h_) : x(x_), y(y_), width(w_), height(h_) {}
    };
    
    static bool rectanglesOverlap(const Rectangle& a, const Rectangle& b);
    static bool rectangleContains(const Rectangle& container, const Rectangle& contained);
    static Rectangle getIntersection(const Rectangle& a, const Rectangle& b);
    static Rectangle getUnion(const Rectangle& a, const Rectangle& b);
    static int calculateArea(const Rectangle& rect);
    static int calculatePerimeter(const Rectangle& rect);
    static Rectangle inflateRect(const Rectangle& rect, int amount);
    static Rectangle deflateRect(const Rectangle& rect, int amount);
    
    // Color utilities
    struct Color {
        unsigned char r, g, b, a;
        Color() : r(0), g(0), b(0), a(255) {}
        Color(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_ = 255)
            : r(r_), g(g_), b(b_), a(a_) {}
    };
    
    static Color lerpColor(const Color& a, const Color& b, float t);
    static Color multiplyColor(const Color& color, float factor);
    static Color addColor(const Color& a, const Color& b);
    static Color premultiplyAlpha(const Color& color);
    static unsigned char calculateLuminance(const Color& color);
    
    // Time utilities
    static double getCurrentTime();
    static std::string formatTime(double seconds);
    static std::string formatDate(const std::string& format = "%Y-%m-%d %H:%M:%S");
    static std::string formatDuration(double seconds);
    
    // Performance utilities
    class Timer {
    public:
        Timer();
        void start();
        void stop();
        void reset();
        double getElapsedSeconds() const;
        double getElapsedMilliseconds() const;
        std::string getElapsedString() const;
        
    private:
        std::chrono::high_resolution_clock::time_point startTime_;
        std::chrono::high_resolution_clock::time_point endTime_;
        bool running_;
    };
    
    class PerformanceMonitor {
    public:
        PerformanceMonitor();
        void startMeasurement(const std::string& name);
        void endMeasurement(const std::string& name);
        void reset();
        std::string getReport() const;
        double getTotalTime() const;
        
    private:
        struct Measurement {
            std::string name;
            double startTime;
            double endTime;
            double duration;
        };
        
        std::vector<Measurement> measurements_;
        double totalTime_;
    };
    
    // Memory utilities
    static size_t getMemoryUsage();
    static std::string formatMemorySize(size_t bytes);
    static bool allocateAligned(void** ptr, size_t size, size_t alignment);
    static void freeAligned(void* ptr);
    
    // File utilities
    static bool copyFile(const std::string& source, const std::string& destination);
    static bool moveFile(const std::string& source, const std::string& destination);
    static bool deleteFile(const std::string& filename);
    static size_t getFileSize(const std::string& filename);
    static std::string getFileHash(const std::string& filename);
    
    // Logging utilities
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };
    
    static void setLogLevel(LogLevel level);
    static void log(LogLevel level, const std::string& message);
    static void logDebug(const std::string& message);
    static void logInfo(const std::string& message);
    static void logWarning(const std::string& message);
    static void logError(const std::string& message);
    
    // Validation utilities
    static bool validateFilename(const std::string& filename);
    static bool validateDimensions(int width, int height, int maxWidth = INT_MAX, int maxHeight = INT_MAX);
    static bool validateColorChannels(int channels);
    static bool validateRectangle(const Rectangle& rect);
    
    // String formatting utilities
    static std::string padString(const std::string& str, int width, char padChar = ' ');
    static std::string centerString(const std::string& str, int width, char padChar = ' ');
    static std::string truncateString(const std::string& str, int maxLength, const std::string& suffix = "...");
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::vector<std::string> splitString(const std::string& str, char delimiter);
    static std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter);
    
    // Random utilities
    static void seedRandom(unsigned int seed = 0);
    static int randomInt(int min, int max);
    static float randomFloat(float min = 0.0f, float max = 1.0f);
    static double randomDouble(double min = 0.0, double max = 1.0);
    static void shuffleVector(std::vector<int>& vector);
    
    // Conversion utilities
    static std::string intToString(int value);
    static std::string floatToString(float value, int precision = 2);
    static std::string doubleToString(double value, int precision = 6);
    static int stringToInt(const std::string& str, int defaultValue = 0);
    static float stringToFloat(const std::string& str, float defaultValue = 0.0f);
    static double stringToDouble(const std::string& str, double defaultValue = 0.0);
    
    // Platform utilities
    static std::string getOperatingSystem();
    static std::string getArchitecture();
    static std::string getCompiler();
    static bool isWindows();
    static bool isLinux();
    static bool isMacOS();
    
private:
    static LogLevel currentLogLevel_;
    static unsigned int randomSeed_;
};

} // namespace TexturePacker
