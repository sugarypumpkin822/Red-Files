#pragma once

#include <string>
#include <cstdint>
#include <array>

namespace RFPlatform {

// Platform types
enum class PlatformType {
    WINDOWS,
    LINUX,
    MACOS,
    IOS,
    ANDROID,
    WEB,
    CONSOLE,
    UNKNOWN
};

// Architecture types
enum class ArchitectureType {
    X86,
    X86_64,
    ARM,
    ARM64,
    RISCV,
    POWERPC,
    SPARC,
    UNKNOWN
};

// Compiler types
enum class CompilerType {
    MSVC,
    GCC,
    CLANG,
    INTEL,
    EMSCRIPTEN,
    UNKNOWN
};

// Endianness types
enum class EndiannessType {
    LITTLE,
    BIG,
    UNKNOWN
};

// Platform capabilities
struct PlatformCapabilities {
    bool supportsMultithreading;
    bool supportsSIMD;
    bool supportsAVX;
    bool supportsAVX2;
    bool supportsAVX512;
    bool supportsNEON;
    bool supportsSSE;
    bool supportsSSE2;
    bool supportsSSE3;
    bool supportsSSE4_1;
    bool supportsSSE4_2;
    bool supportsFMA;
    bool supportsFMA4;
    bool supportsOpenGL;
    bool supportsDirectX;
    bool supportsVulkan;
    bool supportsMetal;
    bool supportsOpenCL;
    bool supportsCUDA;
    bool supportsOpenMP;
    bool supportsC++11;
    bool supportsC++14;
    bool supportsC++17;
    bool supportsC++20;
    bool supportsC++23;
    
    PlatformCapabilities() : supportsMultithreading(false), supportsSIMD(false), 
                           supportsAVX(false), supportsAVX2(false), supportsAVX512(false), 
                           supportsNEON(false), supportsSSE(false), supportsSSE2(false), 
                           supportsSSE3(false), supportsSSE4_1(false), supportsSSE4_2(false), 
                           supportsFMA(false), supportsFMA4(false), supportsOpenGL(false), 
                           supportsDirectX(false), supportsVulkan(false), supportsMetal(false), 
                           supportsOpenCL(false), supportsCUDA(false), supportsOpenMP(false), 
                           supportsC++11(false), supportsC++14(false), supportsC++17(false), 
                           supportsC++20(false), supportsC++23(false) {}
};

// Platform information
struct PlatformInfo {
    PlatformType platform;
    ArchitectureType architecture;
    CompilerType compiler;
    EndiannessType endianness;
    std::string platformName;
    std::string architectureName;
    std::string compilerName;
    std::string compilerVersion;
    std::string operatingSystem;
    std::string osVersion;
    std::string cpuName;
    std::string gpuName;
    uint32_t cpuCores;
    uint32_t logicalCores;
    uint64_t totalMemory;
    uint64_t availableMemory;
    uint32_t cacheLineSize;
    uint32_t l1CacheSize;
    uint32_t l2CacheSize;
    uint32_t l3CacheSize;
    PlatformCapabilities capabilities;
    
    PlatformInfo() : platform(PlatformType::UNKNOWN), architecture(ArchitectureType::UNKNOWN), 
                   compiler(CompilerType::UNKNOWN), endianness(EndiannessType::UNKNOWN), 
                   platformName(""), architectureName(""), compilerName(""), 
                   compilerVersion(""), operatingSystem(""), osVersion(""), cpuName(""), 
                   gpuName(""), cpuCores(0), logicalCores(0), totalMemory(0), 
                   availableMemory(0), cacheLineSize(0), l1CacheSize(0), 
                   l2CacheSize(0), l3CacheSize(0) {}
};

// Platform detection
class PlatformDetector {
public:
    static PlatformInfo detectPlatform();
    static PlatformType getPlatformType();
    static ArchitectureType getArchitectureType();
    static CompilerType getCompilerType();
    static EndiannessType getEndiannessType();
    static std::string getPlatformName();
    static std::string getArchitectureName();
    static std::string getCompilerName();
    static std::string getCompilerVersion();
    static std::string getOperatingSystem();
    static std::string getOSVersion();
    static std::string getCPUName();
    static std::string getGPUName();
    static uint32_t getCPUCores();
    static uint32_t getLogicalCores();
    static uint64_t getTotalMemory();
    static uint64_t getAvailableMemory();
    static uint32_t getCacheLineSize();
    static uint32_t getL1CacheSize();
    static uint32_t getL2CacheSize();
    static uint32_t getL3CacheSize();
    static PlatformCapabilities getCapabilities();
    
private:
    static void detectCapabilities(PlatformCapabilities& capabilities);
    static bool detectSIMDSupport();
    static bool detectAVXSupport();
    static bool detectAVX2Support();
    static bool detectAVX512Support();
    static bool detectNESupport();
    static bool detectSSESupport();
    static bool detectSSE2Support();
    static bool detectSSE3Support();
    static bool detectSSE4_1Support();
    static bool detectSSE4_2Support();
    static bool detectFMASupport();
    static bool detectFMA4Support();
    static bool detectOpenGLSupport();
    static bool detectDirectXSupport();
    static bool detectVulkanSupport();
    static bool detectMetalSupport();
    static bool detectOpenCLSupport();
    static bool detectCUDASupport();
    static bool detectOpenMPSupport();
    static bool detectCxx11Support();
    static bool detectCxx14Support();
    static bool detectCxx17Support();
    static bool detectCxx20Support();
    static bool detectCxx23Support();
};

// Platform utilities
class PlatformUtils {
public:
    // Memory utilities
    static void* alignedAlloc(size_t size, size_t alignment);
    static void alignedFree(void* ptr);
    static size_t getPageSize();
    static void* allocateExecutableMemory(size_t size);
    static void freeExecutableMemory(void* ptr);
    
    // Thread utilities
    static uint32_t getCurrentThreadId();
    static uint32_t getCurrentProcessId();
    static void setThreadAffinity(uint32_t core);
    static void setThreadPriority(int priority);
    static void yieldThread();
    static void sleepThread(uint32_t milliseconds);
    
    // File system utilities
    static std::string getCurrentDirectory();
    static std::string getExecutablePath();
    static std::string getTempDirectory();
    static std::string getUserDirectory();
    static std::string getHomeDirectory();
    static std::string getConfigDirectory();
    static std::string getDataDirectory();
    static bool createDirectory(const std::string& path);
    static bool deleteDirectory(const std::string& path);
    static bool fileExists(const std::string& path);
    static bool isDirectory(const std::string& path);
    static uint64_t getFileSize(const std::string& path);
    static uint64_t getFileTime(const std::string& path);
    
    // System utilities
    static uint64_t getCurrentTime();
    static uint64_t getHighResolutionTime();
    static uint64_t getUpTime();
    static std::string getEnvironmentVariable(const std::string& name);
    static bool setEnvironmentVariable(const std::string& name, const std::string& value);
    static std::string getComputerName();
    static std::string getUserName();
    static void openURL(const std::string& url);
    static void openFile(const std::string& path);
    static void showMessageBox(const std::string& title, const std::string& message);
    
    // Performance utilities
    static uint64_t getCycleCount();
    static uint64_t getInstructionCount();
    static float getCpuUsage();
    static float getMemoryUsage();
    static float getDiskUsage();
    static float getNetworkUsage();
    static void startProfiler();
    static void stopProfiler();
    static std::string getProfilerReport();
    
private:
    static PlatformInfo platformInfo_;
    static bool platformInfoInitialized_;
    static void initializePlatformInfo();
};

// Platform-specific macros
#ifdef _WIN32
    #define RF_PLATFORM_WINDOWS
    #define RF_PLATFORM_NAME "Windows"
    #ifdef _WIN64
        #define RF_ARCHITECTURE_X86_64
        #define RF_ARCHITECTURE_NAME "x86_64"
    #else
        #define RF_ARCHITECTURE_X86
        #define RF_ARCHITECTURE_NAME "x86"
    #endif
    #ifdef _MSC_VER
        #define RF_COMPILER_MSVC
        #define RF_COMPILER_NAME "MSVC"
        #define RF_COMPILER_VERSION _MSC_VER
    #endif
    #define RF_ENDIANNESS_LITTLE
    #define RF_CALLINGCONV __stdcall
    #define RF_INLINE __forceinline
    #define RF_NOINLINE __declspec(noinline)
    #define RF_THREAD_LOCAL __declspec(thread)
    #define RF_DLL_EXPORT __declspec(dllexport)
    #define RF_DLL_IMPORT __declspec(dllimport)
#elif defined(__linux__)
    #define RF_PLATFORM_LINUX
    #define RF_PLATFORM_NAME "Linux"
    #ifdef __x86_64__
        #define RF_ARCHITECTURE_X86_64
        #define RF_ARCHITECTURE_NAME "x86_64"
    #elif defined(__i386__)
        #define RF_ARCHITECTURE_X86
        #define RF_ARCHITECTURE_NAME "x86"
    #elif defined(__aarch64__)
        #define RF_ARCHITECTURE_ARM64
        #define RF_ARCHITECTURE_NAME "ARM64"
    #elif defined(__arm__)
        #define RF_ARCHITECTURE_ARM
        #define RF_ARCHITECTURE_NAME "ARM"
    #endif
    #ifdef __GNUC__
        #define RF_COMPILER_GCC
        #define RF_COMPILER_NAME "GCC"
        #define RF_COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
    #elif defined(__clang__)
        #define RF_COMPILER_CLANG
        #define RF_COMPILER_NAME "Clang"
        #define RF_COMPILER_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
    #endif
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        #define RF_ENDIANNESS_LITTLE
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        #define RF_ENDIANNESS_BIG
    #endif
    #define RF_CALLINGCONV
    #define RF_INLINE inline
    #define RF_NOINLINE __attribute__((noinline))
    #define RF_THREAD_LOCAL __thread
    #define RF_DLL_EXPORT __attribute__((visibility("default")))
    #define RF_DLL_IMPORT
#elif defined(__APPLE__)
    #define RF_PLATFORM_MACOS
    #define RF_PLATFORM_NAME "macOS"
    #ifdef __x86_64__
        #define RF_ARCHITECTURE_X86_64
        #define RF_ARCHITECTURE_NAME "x86_64"
    #elif defined(__aarch64__)
        #define RF_ARCHITECTURE_ARM64
        #define RF_ARCHITECTURE_NAME "ARM64"
    #endif
    #ifdef __GNUC__
        #define RF_COMPILER_GCC
        #define RF_COMPILER_NAME "GCC"
        #define RF_COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
    #elif defined(__clang__)
        #define RF_COMPILER_CLANG
        #define RF_COMPILER_NAME "Clang"
        #define RF_COMPILER_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
    #endif
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        #define RF_ENDIANNESS_LITTLE
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        #define RF_ENDIANNESS_BIG
    #endif
    #define RF_CALLINGCONV
    #define RF_INLINE inline
    #define RF_NOINLINE __attribute__((noinline))
    #define RF_THREAD_LOCAL __thread
    #define RF_DLL_EXPORT __attribute__((visibility("default")))
    #define RF_DLL_IMPORT
#elif defined(__EMSCRIPTEN__)
    #define RF_PLATFORM_WEB
    #define RF_PLATFORM_NAME "Web"
    #define RF_ARCHITECTURE_WASM
    #define RF_ARCHITECTURE_NAME "WebAssembly"
    #define RF_COMPILER_EMSCRIPTEN
    #define RF_COMPILER_NAME "Emscripten"
    #define RF_ENDIANNESS_LITTLE
    #define RF_CALLINGCONV
    #define RF_INLINE inline
    #define RF_NOINLINE __attribute__((noinline))
    #define RF_THREAD_LOCAL __thread
    #define RF_DLL_EXPORT __attribute__((visibility("default")))
    #define RF_DLL_IMPORT
#else
    #define RF_PLATFORM_UNKNOWN
    #define RF_PLATFORM_NAME "Unknown"
    #define RF_ARCHITECTURE_UNKNOWN
    #define RF_ARCHITECTURE_NAME "Unknown"
    #define RF_COMPILER_UNKNOWN
    #define RF_COMPILER_NAME "Unknown"
    #define RF_ENDIANNESS_UNKNOWN
    #define RF_CALLINGCONV
    #define RF_INLINE inline
    #define RF_NOINLINE
    #define RF_THREAD_LOCAL
    #define RF_DLL_EXPORT
    #define RF_DLL_IMPORT
#endif

// Feature detection macros
#if defined(__AVX512F__)
    #define RF_HAS_AVX512
#endif
#if defined(__AVX2__)
    #define RF_HAS_AVX2
#endif
#if defined(__AVX__)
    #define RF_HAS_AVX
#endif
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
    #define RF_HAS_NEON
#endif
#if defined(__SSE4_2__)
    #define RF_HAS_SSE4_2
#endif
#if defined(__SSE4_1__)
    #define RF_HAS_SSE4_1
#endif
#if defined(__SSE3__)
    #define RF_HAS_SSE3
#endif
#if defined(__SSE2__)
    #define RF_HAS_SSE2
#endif
#if defined(__SSE__)
    #define RF_HAS_SSE
#endif
#if defined(__FMA__)
    #define RF_HAS_FMA
#endif
#if defined(__FMA4__)
    #define RF_HAS_FMA4
#endif

// C++ standard detection macros
#if __cplusplus >= 202302L
    #define RF_HAS_CXX23
#elif __cplusplus >= 202002L
    #define RF_HAS_CXX20
#elif __cplusplus >= 201703L
    #define RF_HAS_CXX17
#elif __cplusplus >= 201402L
    #define RF_HAS_CXX14
#elif __cplusplus >= 201103L
    #define RF_HAS_CXX11
#endif

// Utility macros
#define RF_STRINGIFY(x) #x
#define RF_TOSTRING(x) RF_STRINGIFY(x)
#define RF_CONCAT(a, b) a##b
#define RF_UNIQUE_NAME(prefix) RF_CONCAT(prefix, __LINE__)
#define RF_COUNT_OF(array) (sizeof(array) / sizeof(array[0]))
#define RF_OFFSET_OF(type, member) ((size_t)&(((type*)0)->member))
#define RF_ALIGN_OF(type) alignof(type)

// Compiler-specific attributes
#ifdef RF_COMPILER_GCC
    #define RF_FORCE_INLINE __attribute__((always_inline)) inline
    #define RF_LIKELY(x) __builtin_expect(!!(x), 1)
    #define RF_UNLIKELY(x) __builtin_expect(!!(x), 0)
    #define RF_PREFETCH(addr) __builtin_prefetch(addr)
    #define RF_ASSUME(x) __builtin_assume(x)
#elif defined(RF_COMPILER_CLANG)
    #define RF_FORCE_INLINE __attribute__((always_inline)) inline
    #define RF_LIKELY(x) __builtin_expect(!!(x), 1)
    #define RF_UNLIKELY(x) __builtin_expect(!!(x), 0)
    #define RF_PREFETCH(addr) __builtin_prefetch(addr)
    #define RF_ASSUME(x) __builtin_assume(x)
#elif defined(RF_COMPILER_MSVC)
    #define RF_FORCE_INLINE __forceinline
    #define RF_LIKELY(x) (x)
    #define RF_UNLIKELY(x) (x)
    #define RF_PREFETCH(addr) _mm_prefetch(addr, _MM_HINT_T0)
    #define RF_ASSUME(x) __assume(x)
#else
    #define RF_FORCE_INLINE inline
    #define RF_LIKELY(x) (x)
    #define RF_UNLIKELY(x) (x)
    #define RF_PREFETCH(addr)
    #define RF_ASSUME(x)
#endif

// Debug/Release macros
#ifdef _DEBUG
    #define RF_DEBUG
    #define RF_DEBUG_BREAK() __debugbreak()
#else
    #define RF_RELEASE
    #define RF_DEBUG_BREAK()
#endif

// Memory alignment
#define RF_CACHE_LINE_SIZE 64
#define RF_ALIGNED_CACHE_LINE alignas(RF_CACHE_LINE_SIZE)

} // namespace RFPlatform