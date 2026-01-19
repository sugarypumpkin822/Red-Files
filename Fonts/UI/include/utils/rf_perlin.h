#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>

namespace RFUtils {

// Forward declarations
class Vector2;
class Vector3;
class Vector4;

// Perlin noise implementation
class PerlinNoise {
public:
    // Constructors
    PerlinNoise();
    PerlinNoise(uint32_t seed);
    PerlinNoise(uint32_t seed, float32 frequency, float32 amplitude, int32_t octaves, float32 persistence, float32 lacunarity);
    
    // Copy constructor
    PerlinNoise(const PerlinNoise& other);
    
    // Assignment operator
    PerlinNoise& operator=(const PerlinNoise& other);
    
    // Core noise functions
    float32 noise1D(float32 x) const;
    float32 noise2D(float32 x, float32 y) const;
    float32 noise3D(float32 x, float32 y, float32 z) const;
    float32 noise4D(float32 x, float32 y, float32 z, float32 w) const;
    
    // Fractal noise functions
    float32 fractalNoise1D(float32 x) const;
    float32 fractalNoise2D(float32 x, float32 y) const;
    float32 fractalNoise3D(float32 x, float32 y, float32 z) const;
    float32 fractalNoise4D(float32 x, float32 y, float32 z, float32 w) const;
    
    // Turbulence noise functions
    float32 turbulence1D(float32 x) const;
    float32 turbulence2D(float32 x, float32 y) const;
    float32 turbulence3D(float32 x, float32 y, float32 z) const;
    float32 turbulence4D(float32 x, float32 y, float32 z, float32 w) const;
    
    // Ridged noise functions
    float32 ridgedNoise1D(float32 x) const;
    float32 ridgedNoise2D(float32 x, float32 y) const;
    float32 ridgedNoise3D(float32 x, float32 y, float32 z) const;
    float32 ridgedNoise4D(float32 x, float32 y, float32 z, float32 w) const;
    
    // Billow noise functions
    float32 billowNoise1D(float32 x) const;
    float32 billowNoise2D(float32 x, float32 y) const;
    float32 billowNoise3D(float32 x, float32 y, float32 z) const;
    float32 billowNoise4D(float32 x, float32 y, float32 z, float32 w) const;
    
    // Properties
    void setSeed(uint32_t seed);
    uint32_t getSeed() const { return seed_; }
    
    void setFrequency(float32 frequency);
    float32 getFrequency() const { return frequency_; }
    
    void setAmplitude(float32 amplitude);
    float32 getAmplitude() const { return amplitude_; }
    
    void setOctaves(int32_t octaves);
    int32_t getOctaves() const { return octaves_; }
    
    void setPersistence(float32 persistence);
    float32 getPersistence() const { return persistence_; }
    
    void setLacunarity(float32 lacunarity);
    float32 getLacunarity() const { return lacunarity_; }
    
    // Utility methods
    float32 getMinValue() const { return -amplitude_; }
    float32 getMaxValue() const { return amplitude_; }
    float32 getRange() const { return amplitude_ * 2.0f; }
    
    bool isValid() const;
    void normalize();
    
    // String conversion
    std::string toString() const;
    
    // Static methods
    static PerlinNoise createDefault();
    static PerlinNoise createTerrain(uint32_t seed = 0);
    static PerlinNoise createMarble(uint32_t seed = 0);
    static PerlinNoise createWood(uint32_t seed = 0);
    static PerlinNoise createCloud(uint32_t seed = 0);
    static PerlinNoise createFire(uint32_t seed = 0);
    static PerlinNoise createWater(uint32_t seed = 0);
    
    // Constants
    static constexpr float32 DEFAULT_FREQUENCY = 1.0f;
    static constexpr float32 DEFAULT_AMPLITUDE = 1.0f;
    static constexpr int32_t DEFAULT_OCTAVES = 4;
    static constexpr float32 DEFAULT_PERSISTENCE = 0.5f;
    static constexpr float32 DEFAULT_LACUNARITY = 2.0f;
    static constexpr uint32_t DEFAULT_SEED = 0;
    static constexpr int32_t PERMUTATION_SIZE = 256;
    
private:
    uint32_t seed_;
    float32 frequency_;
    float32 amplitude_;
    int32_t octaves_;
    float32 persistence_;
    float32 lacunarity_;
    
    std::vector<int32_t> permutation_;
    
    void generatePermutation();
    
    // Gradient functions
    float32 grad(int32_t hash, float32 x) const;
    float32 grad(int32_t hash, float32 x, float32 y) const;
    float32 grad(int32_t hash, float32 x, float32 y, float32 z) const;
    float32 grad(int32_t hash, float32 x, float32 y, float32 z, float32 w) const;
    
    // Interpolation functions
    float32 fade(float32 t) const;
    float32 lerp(float32 a, float32 b, float32 t) const;
    float32 cubicInterpolate(float32 a, float32 b, float32 c, float32 d, float32 t) const;
    float32 quinticInterpolate(float32 a, float32 b, float32 c, float32 d, float32 e, float32 t) const;
    
    // Base noise functions
    float32 baseNoise1D(float32 x) const;
    float32 baseNoise2D(float32 x, float32 y) const;
    float32 baseNoise3D(float32 x, float32 y, float32 z) const;
    float32 baseNoise4D(float32 x, float32 y, float32 z, float32 w) const;
    
    // Helper functions
    int32_t fastFloor(float32 x) const;
    float32 dot(const float32* g, float32 x) const;
    float32 dot(const float32* g, float32 x, float32 y) const;
    float32 dot(const float32* g, float32 x, float32 y, float32 z) const;
    float32 dot(const float32* g, float32 x, float32 y, float32 z, float32 w) const;
    
    // Gradient tables
    static const float32 GRADIENTS_1D[2];
    static const float32 GRADIENTS_2D[8][2];
    static const float32 GRADIENTS_3D[12][3];
    static const float32 GRADIENTS_4D[32][4];
};

// Perlin noise utilities namespace
namespace PerlinNoiseUtils {
    // Noise evaluation
    float32 evaluate(const PerlinNoise& noise, float32 x);
    float32 evaluate(const PerlinNoise& noise, float32 x, float32 y);
    float32 evaluate(const PerlinNoise& noise, float32 x, float32 y, float32 z);
    float32 evaluate(const PerlinNoise& noise, float32 x, float32 y, float32 z, float32 w);
    
    // Fractal evaluation
    float32 evaluateFractal(const PerlinNoise& noise, float32 x);
    float32 evaluateFractal(const PerlinNoise& noise, float32 x, float32 y);
    float32 evaluateFractal(const PerlinNoise& noise, float32 x, float32 y, float32 z);
    float32 evaluateFractal(const PerlinNoise& noise, float32 x, float32 y, float32 z, float32 w);
    
    // Turbulence evaluation
    float32 evaluateTurbulence(const PerlinNoise& noise, float32 x);
    float32 evaluateTurbulence(const PerlinNoise& noise, float32 x, float32 y);
    float32 evaluateTurbulence(const PerlinNoise& noise, float32 x, float32 y, float32 z);
    float32 evaluateTurbulence(const PerlinNoise& noise, float32 x, float32 y, float32 z, float32 w);
    
    // Ridged evaluation
    float32 evaluateRidged(const PerlinNoise& noise, float32 x);
    float32 evaluateRidged(const PerlinNoise& noise, float32 x, float32 y);
    float32 evaluateRidged(const PerlinNoise& noise, float32 x, float32 y, float32 z);
    float32 evaluateRidged(const PerlinNoise& noise, float32 x, float32 y, float32 z, float32 w);
    
    // Billow evaluation
    float32 evaluateBillow(const PerlinNoise& noise, float32 x);
    float32 evaluateBillow(const PerlinNoise& noise, float32 x, float32 y);
    float32 evaluateBillow(const PerlinNoise& noise, float32 x, float32 y, float32 z);
    float32 evaluateBillow(const PerlinNoise& noise, float32 x, float32 y, float32 z, float32 w);
    
    // Batch evaluation
    std::vector<float32> evaluateBatch(const PerlinNoise& noise, const std::vector<float32>& positions);
    std::vector<float32> evaluateBatch(const PerlinNoise& noise, const std::vector<Vector2>& positions);
    std::vector<float32> evaluateBatch(const PerlinNoise& noise, const std::vector<Vector3>& positions);
    std::vector<float32> evaluateBatch(const PerlinNoise& noise, const std::vector<Vector4>& positions);
    
    std::vector<float32> evaluateFractalBatch(const PerlinNoise& noise, const std::vector<float32>& positions);
    std::vector<float32> evaluateFractalBatch(const PerlinNoise& noise, const std::vector<Vector2>& positions);
    std::vector<float32> evaluateFractalBatch(const PerlinNoise& noise, const std::vector<Vector3>& positions);
    std::vector<float32> evaluateFractalBatch(const PerlinNoise& noise, const std::vector<Vector4>& positions);
    
    // Noise combinations
    class PerlinCombiner {
    public:
        enum class Operation {
            Add,
            Subtract,
            Multiply,
            Divide,
            Min,
            Max,
            Average,
            WeightedAverage,
            Blend,
            Select
        };
        
        PerlinCombiner();
        PerlinCombiner(Operation operation);
        
        void addNoise(const PerlinNoise& noise, float32 weight = 1.0f);
        void removeNoise(size_t index);
        void clearNoises();
        
        float32 evaluate(float32 x) const;
        float32 evaluate(float32 x, float32 y) const;
        float32 evaluate(float32 x, float32 y, float32 z) const;
        float32 evaluate(float32 x, float32 y, float32 z, float32 w) const;
        
        void setOperation(Operation operation);
        Operation getOperation() const { return operation_; }
        
        void setWeight(size_t index, float32 weight);
        float32 getWeight(size_t index) const;
        
        size_t getNoiseCount() const;
        const PerlinNoise& getNoise(size_t index) const;
        
        float32 getMinValue() const;
        float32 getMaxValue() const;
        float32 getRange() const;
        
        std::string toString() const;
        
    private:
        Operation operation_;
        std::vector<std::pair<PerlinNoise, float32>> noises_;
        
        float32 combineValues(const std::vector<float32>& values) const;
    };
    
    // Noise modifiers
    class PerlinModifier {
    public:
        enum class Type {
            Normalize,
            Clamp,
            Bias,
            Gain,
            Curve,
            Invert,
            Abs,
            Power,
            Sqrt,
            Log,
            Exp,
            Sin,
            Cos,
            Tan,
            Wrap,
            Mirror,
            Quantize,
            Threshold,
            Smoothstep,
            Smootherstep
        };
        
        PerlinModifier();
        PerlinModifier(Type type);
        PerlinModifier(Type type, float32 param);
        
        float32 apply(float32 value) const;
        PerlinNoise apply(const PerlinNoise& noise) const;
        
        void setType(Type type);
        Type getType() const { return type_; }
        
        void setParameter(float32 param);
        float32 getParameter() const { return param_; }
        
    private:
        Type type_;
        float32 param_;
        
        float32 applyNormalize(float32 value) const;
        float32 applyClamp(float32 value) const;
        float32 applyBias(float32 value) const;
        float32 applyGain(float32 value) const;
        float32 applyCurve(float32 value) const;
        float32 applyInvert(float32 value) const;
        float32 applyAbs(float32 value) const;
        float32 applyPower(float32 value) const;
        float32 applySqrt(float32 value) const;
        float32 applyLog(float32 value) const;
        float32 applyExp(float32 value) const;
        float32 applySin(float32 value) const;
        float32 applyCos(float32 value) const;
        float32 applyTan(float32 value) const;
        float32 applyWrap(float32 value) const;
        float32 applyMirror(float32 value) const;
        float32 applyQuantize(float32 value) const;
        float32 applyThreshold(float32 value) const;
        float32 applySmoothstep(float32 value) const;
        float32 applySmootherstep(float32 value) const;
    };
    
    // Noise generators
    PerlinNoise createDefault(uint32_t seed = 0);
    PerlinNoise createTerrain(uint32_t seed = 0);
    PerlinNoise createMarble(uint32_t seed = 0);
    PerlinNoise createWood(uint32_t seed = 0);
    PerlinNoise createCloud(uint32_t seed = 0);
    PerlinNoise createFire(uint32_t seed = 0);
    PerlinNoise createWater(uint32_t seed = 0);
    PerlinNoise createLava(uint32_t seed = 0);
    PerlinNoise createIce(uint32_t seed = 0);
    PerlinNoise createSand(uint32_t seed = 0);
    PerlinNoise createGrass(uint32_t seed = 0);
    PerlinNoise createRock(uint32_t seed = 0);
    PerlinNoise createMetal(uint32_t seed = 0);
    PerlinNoise createFabric(uint32_t seed = 0);
    PerlinNoise createPaper(uint32_t seed = 0);
    PerlinNoise createConcrete(uint32_t seed = 0);
    PerlinNoise createBrick(uint32_t seed = 0);
    PerlinNoise createStone(uint32_t seed = 0);
    PerlinNoise createDirt(uint32_t seed = 0);
    PerlinNoise createSnow(uint32_t seed = 0);
    PerlinNoise createMud(uint32_t seed = 0);
    PerlinNoise createGravel(uint32_t seed = 0);
    
    // Noise analysis
    struct PerlinAnalysis {
        uint32_t seed;
        float32 frequency;
        float32 amplitude;
        int32_t octaves;
        float32 persistence;
        float32 lacunarity;
        float32 minValue;
        float32 maxValue;
        float32 meanValue;
        float32 variance;
        float32 standardDeviation;
        float32 range;
        bool isValid;
        std::string description;
    };
    
    PerlinAnalysis analyze(const PerlinNoise& noise, size_t samples = 10000);
    
    // Noise validation
    bool validate(const PerlinNoise& noise);
    bool validateRange(const PerlinNoise& noise, float32 expectedMin, float32 expectedMax, float32 tolerance = 0.01f);
    bool validateContinuity(const PerlinNoise& noise, float32 tolerance = 0.01f);
    bool validatePeriodicity(const PerlinNoise& noise, float32 tolerance = 0.01f);
    
    // Noise debugging
    std::string toString(const PerlinNoise& noise);
    void debugPrint(const PerlinNoise& noise);
    void debugPrintAnalysis(const PerlinNoise& noise);
    
    // Noise sampling
    std::vector<float32> sampleNoise(const PerlinNoise& noise, const Vector2& min, const Vector2& max, size_t width, size_t height);
    std::vector<float32> sampleNoise(const PerlinNoise& noise, const Vector3& min, const Vector3& max, size_t width, size_t height, size_t depth);
    std::vector<float32> sampleNoise(const PerlinNoise& noise, const Vector4& min, const Vector4& max, size_t width, size_t height, size_t depth, size_t time);
    
    std::vector<float32> sampleFractalNoise(const PerlinNoise& noise, const Vector2& min, const Vector2& max, size_t width, size_t height);
    std::vector<float32> sampleFractalNoise(const PerlinNoise& noise, const Vector3& min, const Vector3& max, size_t width, size_t height, size_t depth);
    std::vector<float32> sampleFractalNoise(const PerlinNoise& noise, const Vector4& min, const Vector4& max, size_t width, size_t height, size_t depth, size_t time);
    
    // Noise visualization
    class PerlinVisualizer {
    public:
        PerlinVisualizer();
        
        void setNoise(const PerlinNoise& noise);
        void setRange(const Vector2& min, const Vector2& max);
        void setResolution(size_t width, size_t height);
        
        std::vector<float32> generateHeightmap() const;
        std::vector<uint8_t> generateGrayscaleImage() const;
        std::vector<uint8_t> generateColorImage() const;
        
        void saveHeightmap(const std::string& filename) const;
        void saveImage(const std::string& filename) const;
        
    private:
        PerlinNoise noise_;
        Vector2 min_;
        Vector2 max_;
        size_t width_;
        size_t height_;
        
        float32 normalizeValue(float32 value) const;
        uint8_t valueToByte(float32 value) const;
        void valueToRGB(float32 value, uint8_t& r, uint8_t& g, uint8_t& b) const;
    };
    
    // Noise cache
    class PerlinCache {
    public:
        PerlinCache(size_t maxSize = 1000);
        
        void put(const std::string& key, const PerlinNoise& noise);
        PerlinNoise get(const std::string& key) const;
        bool contains(const std::string& key) const;
        void remove(const std::string& key);
        void clear();
        
        size_t size() const;
        size_t maxSize() const;
        void setMaxSize(size_t maxSize);
        
        double getHitRate() const;
        size_t getHitCount() const;
        size_t getMissCount() const;
        
    private:
        struct CacheEntry {
            PerlinNoise noise;
            size_t accessCount;
            uint64_t lastAccessTime;
        };
        
        std::map<std::string, CacheEntry> cache_;
        size_t maxSize_;
        size_t hitCount_;
        size_t missCount_;
        
        void evictLeastRecentlyUsed();
    };
    
    // Noise pool
    class PerlinPool {
    public:
        PerlinPool(size_t initialSize = 100);
        
        PerlinNoise* acquire();
        void release(PerlinNoise* noise);
        void clear();
        
        size_t getPoolSize() const;
        size_t getUsedCount() const;
        size_t getAvailableCount() const;
        
    private:
        std::vector<PerlinNoise> pool_;
        std::vector<PerlinNoise*> available_;
        std::vector<PerlinNoise*> used_;
        
        void expandPool(size_t newSize);
    };
    
    // Noise presets
    namespace Presets {
        // Terrain presets
        PerlinNoise createMountainTerrain(uint32_t seed = 0);
        PerlinNoise createHillsTerrain(uint32_t seed = 0);
        PerlinNoise createPlainsTerrain(uint32_t seed = 0);
        PerlinNoise createDesertTerrain(uint32_t seed = 0);
        PerlinNoise createIslandTerrain(uint32_t seed = 0);
        PerlinNoise createCanyonTerrain(uint32_t seed = 0);
        PerlinNoise createRiverTerrain(uint32_t seed = 0);
        PerlinNoise createCaveTerrain(uint32_t seed = 0);
        
        // Material presets
        PerlinNoise createMarbleMaterial(uint32_t seed = 0);
        PerlinNoise createWoodMaterial(uint32_t seed = 0);
        PerlinNoise createStoneMaterial(uint32_t seed = 0);
        PerlinNoise createMetalMaterial(uint32_t seed = 0);
        PerlinNoise createFabricMaterial(uint32_t seed = 0);
        PerlinNoise createPaperMaterial(uint32_t seed = 0);
        PerlinNoise createConcreteMaterial(uint32_t seed = 0);
        PerlinNoise createBrickMaterial(uint32_t seed = 0);
        
        // Natural presets
        PerlinNoise createCloudPattern(uint32_t seed = 0);
        PerlinNoise createFirePattern(uint32_t seed = 0);
        PerlinNoise createWaterPattern(uint32_t seed = 0);
        PerlinNoise createLavaPattern(uint32_t seed = 0);
        PerlinNoise createIcePattern(uint32_t seed = 0);
        PerlinNoise createSandPattern(uint32_t seed = 0);
        PerlinNoise createGrassPattern(uint32_t seed = 0);
        PerlinNoise createSnowPattern(uint32_t seed = 0);
        PerlinNoise createMudPattern(uint32_t seed = 0);
        PerlinNoise createGravelPattern(uint32_t seed = 0);
        
        // Artistic presets
        PerlinNoise createAbstractPattern(uint32_t seed = 0);
        PerlinNoise createOrganicPattern(uint32_t seed = 0);
        PerlinNoise createGeometricPattern(uint32_t seed = 0);
        PerlinNoise createFluidPattern(uint32_t seed = 0);
        PerlinNoise createCrystallinePattern(uint32_t seed = 0);
        PerlinNoise createFractalPattern(uint32_t seed = 0);
        PerlinNoise createCellularPattern(uint32_t seed = 0);
        PerlinNoise createVoronoiPattern(uint32_t seed = 0);
    }
    
    // Noise optimization
    PerlinNoise optimize(const PerlinNoise& noise);
    PerlinNoise compress(const PerlinNoise& noise);
    PerlinNoise decompress(const PerlinNoise& noise);
    
    // Noise validation
    bool validate(const PerlinNoise& noise);
    bool validatePerlin(const PerlinNoise& noise);
    bool validateFractal(const PerlinNoise& noise);
    bool validateTurbulence(const PerlinNoise& noise);
    bool validateRidged(const PerlinNoise& noise);
    bool validateBillow(const PerlinNoise& noise);
    
    // Noise debugging
    void debugPrintPerlin(const PerlinNoise& noise);
    void debugPrintPerlinFormatted(const PerlinNoise& noise);
    void debugPrintPerlinAnalysis(const PerlinNoise& noise);
    void debugPrintPerlinAnalysis(const PerlinAnalysis& analysis);
    
    // Noise math utilities
    float32 bias(float32 value, float32 bias);
    float32 gain(float32 value, float32 gain);
    float32 smoothstep(float32 edge0, float32 edge1, float32 x);
    float32 smootherstep(float32 edge0, float32 edge1, float32 x);
    
    float32 quantize(float32 value, float32 step);
    float32 threshold(float32 value, float32 threshold);
    float32 clamp(float32 value, float32 min, float32 max);
    float32 wrap(float32 value, float32 min, float32 max);
    float32 mirror(float32 value, float32 min, float32 max);
    
    // Noise interpolation utilities
    float32 interpolateLinear(float32 a, float32 b, float32 t);
    float32 interpolateCosine(float32 a, float32 b, float32 t);
    float32 interpolateCubic(float32 a, float32 b, float32 c, float32 d, float32 t);
    float32 interpolateQuintic(float32 a, float32 b, float32 c, float32 d, float32 e, float32 t);
    float32 interpolateHermite(float32 a, float32 b, float32 c, float32 d, float32 t);
    
    // Noise gradient utilities
    float32 gradientDot1D(const float32* gradient, float32 x);
    float32 gradientDot2D(const float32* gradient, float32 x, float32 y);
    float32 gradientDot3D(const float32* gradient, float32 x, float32 y, float32 z);
    float32 gradientDot4D(const float32* gradient, float32 x, float32 y, float32 z, float32 w);
    
    // Noise hash utilities
    int32_t hash1D(int32_t x);
    int32_t hash2D(int32_t x, int32_t y);
    int32_t hash3D(int32_t x, int32_t y, int32_t z);
    int32_t hash4D(int32_t x, int32_t y, int32_t z, int32_t w);
    
    uint32_t hashUInt1D(uint32_t x);
    uint32_t hashUInt2D(uint32_t x, uint32_t y);
    uint32_t hashUInt3D(uint32_t x, uint32_t y, uint32_t z);
    uint32_t hashUInt4D(uint32_t x, uint32_t y, uint32_t z, uint32_t w);
    
    // Noise permutation utilities
    std::vector<int32_t> generatePermutation(uint32_t seed);
    std::vector<int32_t> generatePermutation(uint32_t seed, size_t size);
    void shufflePermutation(std::vector<int32_t>& permutation, uint32_t seed);
    
    // Noise quality utilities
    bool isFastQuality(const PerlinNoise& noise);
    bool isStandardQuality(const PerlinNoise& noise);
    bool isHighQuality(const PerlinNoise& noise);
    bool isUltraQuality(const PerlinNoise& noise);
    
    // Noise performance utilities
    float32 benchmarkNoise(const PerlinNoise& noise, size_t iterations = 1000000);
    float32 benchmarkFractalNoise(const PerlinNoise& noise, size_t iterations = 1000000);
    float32 benchmarkTurbulenceNoise(const PerlinNoise& noise, size_t iterations = 1000000);
    float32 benchmarkRidgedNoise(const PerlinNoise& noise, size_t iterations = 1000000);
    float32 benchmarkBillowNoise(const PerlinNoise& noise, size_t iterations = 1000000);
    
    // Noise memory utilities
    size_t getMemoryUsage(const PerlinNoise& noise);
    size_t getPermutationMemoryUsage(const PerlinNoise& noise);
    size_t getCacheMemoryUsage(const PerlinNoise& noise);
    size_t getPoolMemoryUsage(const PerlinNoise& noise);
    
    // Noise serialization
    std::vector<uint8_t> serialize(const PerlinNoise& noise);
    PerlinNoise deserialize(const std::vector<uint8_t>& data);
    
    void saveToFile(const PerlinNoise& noise, const std::string& filename);
    PerlinNoise loadFromFile(const std::string& filename);
    
    // Noise comparison
    bool equals(const PerlinNoise& a, const PerlinNoise& b, float32 tolerance = 0.0001f);
    bool approximatelyEquals(const PerlinNoise& a, const PerlinNoise& b, float32 tolerance = 0.0001f);
    
    float32 distance(const PerlinNoise& a, const PerlinNoise& b);
    float32 similarity(const PerlinNoise& a, const PerlinNoise& b);
    
    // Noise blending
    PerlinNoise blend(const PerlinNoise& a, const PerlinNoise& b, float32 weight);
    PerlinNoise blendWeighted(const std::vector<PerlinNoise>& noises, const std::vector<float32>& weights);
    PerlinNoise blendAdditive(const PerlinNoise& base, const PerlinNoise& additive, float32 weight);
    
    // Noise morphing
    PerlinNoise morph(const PerlinNoise& from, const PerlinNoise& to, float32 t);
    PerlinNoise morphLinear(const PerlinNoise& from, const PerlinNoise& to, float32 t);
    PerlinNoise morphSmooth(const PerlinNoise& from, const PerlinNoise& to, float32 t);
    
    // Noise filtering
    PerlinNoise filterLowPass(const PerlinNoise& noise, float32 cutoff);
    PerlinNoise filterHighPass(const PerlinNoise& noise, float32 cutoff);
    PerlinNoise filterBandPass(const PerlinNoise& noise, float32 lowCutoff, float32 highCutoff);
    PerlinNoise filterGaussian(const PerlinNoise& noise, float32 sigma);
    
    // Noise transformation
    PerlinNoise transformScale(const PerlinNoise& noise, float32 scale);
    PerlinNoise transformTranslate(const PerlinNoise& noise, float32 offset);
    PerlinNoise transformRotate(const PerlinNoise& noise, float32 angle);
    PerlinNoise transformSkew(const PerlinNoise& noise, float32 skew);
    
    // Noise composition
    PerlinNoise compose(const std::vector<PerlinNoise>& layers);
    PerlinNoise composeWeighted(const std::vector<std::pair<PerlinNoise, float32>>& layers);
    PerlinNoise composeFractal(const PerlinNoise& base, int32_t octaves, float32 persistence, float32 lacunarity);
    PerlinNoise composeTurbulence(const PerlinNoise& base, int32_t octaves, float32 persistence, float32 lacunarity);
    PerlinNoise composeRidged(const PerlinNoise& base, int32_t octaves, float32 persistence, float32 lacunarity);
    PerlinNoise composeBillow(const PerlinNoise& base, int32_t octaves, float32 persistence, float32 lacunarity);
}

} // namespace RFUtils