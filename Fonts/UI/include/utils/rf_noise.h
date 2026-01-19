#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <functional>

namespace RFUtils {

// Forward declarations
class Vector2;
class Vector3;
class Vector4;

// Noise types
enum class NoiseType {
    Perlin,
    Simplex,
    Worley,
    Fractal,
    Turbulence,
    Ridged,
    Billow,
    Value,
    Gradient,
    Cellular,
    White,
    Pink,
    Brown,
    Blue,
    Voronoi,
    Flow,
    Marble,
    Wood,
    Cloud,
    Fire,
    Water,
    Terrain
};

// Noise interpolation types
enum class NoiseInterpolation {
    Linear,
    Cosine,
    Cubic,
    Quintic,
    Hermite,
    Smoothstep,
    Smootherstep
};

// Noise quality levels
enum class NoiseQuality {
    Fast,
    Standard,
    High,
    Ultra
};

// Noise base class
class Noise {
public:
    Noise();
    virtual ~Noise() = default;
    
    // Core noise functions
    virtual float32 evaluate(float32 x) const = 0;
    virtual float32 evaluate(float32 x, float32 y) const = 0;
    virtual float32 evaluate(float32 x, float32 y, float32 z) const = 0;
    virtual float32 evaluate(float32 x, float32 y, float32 z, float32 w) const = 0;
    
    virtual Vector2 evaluate2D(float32 x, float32 y) const;
    virtual Vector3 evaluate3D(float32 x, float32 y, float32 z) const;
    virtual Vector4 evaluate4D(float32 x, float32 y, float32 z, float32 w) const;
    
    // Batch evaluation
    virtual std::vector<float32> evaluateBatch(const std::vector<float32>& positions) const;
    virtual std::vector<float32> evaluateBatch(const std::vector<Vector2>& positions) const;
    virtual std::vector<float32> evaluateBatch(const std::vector<Vector3>& positions) const;
    virtual std::vector<float32> evaluateBatch(const std::vector<Vector4>& positions) const;
    
    // Properties
    virtual NoiseType getType() const = 0;
    virtual NoiseQuality getQuality() const = 0;
    virtual NoiseInterpolation getInterpolation() const = 0;
    
    virtual void setSeed(uint32_t seed) = 0;
    virtual uint32_t getSeed() const = 0;
    
    virtual void setFrequency(float32 frequency) = 0;
    virtual float32 getFrequency() const = 0;
    
    virtual void setAmplitude(float32 amplitude) = 0;
    virtual float32 getAmplitude() const = 0;
    
    virtual void setOctaves(int32_t octaves) = 0;
    virtual int32_t getOctaves() const = 0;
    
    virtual void setPersistence(float32 persistence) = 0;
    virtual float32 getPersistence() const = 0;
    
    virtual void setLacunarity(float32 lacunarity) = 0;
    virtual float32 getLacunarity() const = 0;
    
    // Utility methods
    virtual float32 getMinValue() const = 0;
    virtual float32 getMaxValue() const = 0;
    virtual bool isNormalized() const = 0;
    
    virtual Noise* clone() const = 0;
    virtual std::string toString() const = 0;
    
    // Static factory methods
    static std::unique_ptr<Noise> create(NoiseType type);
    static std::unique_ptr<Noise> createPerlin();
    static std::unique_ptr<Noise> createSimplex();
    static std::unique_ptr<Noise> createWorley();
    static std::unique_ptr<Noise> createFractal();
    static std::unique_ptr<Noise> createTurbulence();
    static std::unique_ptr<Noise> createRidged();
    static std::unique_ptr<Noise> createBillow();
    static std::unique_ptr<Noise> createValue();
    static std::unique_ptr<Noise> createGradient();
    static std::unique_ptr<Noise> createCellular();
    static std::unique_ptr<Noise> createWhite();
    static std::unique_ptr<Noise> createPink();
    static std::unique_ptr<Noise> createBrown();
    static std::unique_ptr<Noise> createBlue();
    static std::unique_ptr<Noise> createVoronoi();
    static std::unique_ptr<Noise> createFlow();
    static std::unique_ptr<Noise> createMarble();
    static std::unique_ptr<Noise> createWood();
    static std::unique_ptr<Noise> createCloud();
    static std::unique_ptr<Noise> createFire();
    static std::unique_ptr<Noise> createWater();
    static std::unique_ptr<Noise> createTerrain();
    
    // Constants
    static constexpr float32 DEFAULT_FREQUENCY = 1.0f;
    static constexpr float32 DEFAULT_AMPLITUDE = 1.0f;
    static constexpr int32_t DEFAULT_OCTAVES = 4;
    static constexpr float32 DEFAULT_PERSISTENCE = 0.5f;
    static constexpr float32 DEFAULT_LACUNARITY = 2.0f;
    static constexpr uint32_t DEFAULT_SEED = 0;
};

// Perlin noise implementation
class PerlinNoise : public Noise {
public:
    PerlinNoise();
    PerlinNoise(uint32_t seed, float32 frequency = DEFAULT_FREQUENCY, int32_t octaves = DEFAULT_OCTAVES,
               float32 persistence = DEFAULT_PERSISTENCE, float32 lacunarity = DEFAULT_LACUNARITY,
               NoiseQuality quality = NoiseQuality::Standard, NoiseInterpolation interpolation = NoiseInterpolation::Cubic);
    
    float32 evaluate(float32 x) const override;
    float32 evaluate(float32 x, float32 y) const override;
    float32 evaluate(float32 x, float32 y, float32 z) const override;
    float32 evaluate(float32 x, float32 y, float32 z, float32 w) const override;
    
    NoiseType getType() const override { return NoiseType::Perlin; }
    NoiseQuality getQuality() const override { return quality_; }
    NoiseInterpolation getInterpolation() const override { return interpolation_; }
    
    void setSeed(uint32_t seed) override;
    uint32_t getSeed() const override { return seed_; }
    
    void setFrequency(float32 frequency) override;
    float32 getFrequency() const override { return frequency_; }
    
    void setAmplitude(float32 amplitude) override;
    float32 getAmplitude() const override { return amplitude_; }
    
    void setOctaves(int32_t octaves) override;
    int32_t getOctaves() const override { return octaves_; }
    
    void setPersistence(float32 persistence) override;
    float32 getPersistence() const override { return persistence_; }
    
    void setLacunarity(float32 lacunarity) override;
    float32 getLacunarity() const override { return lacunarity_; }
    
    float32 getMinValue() const override { return -amplitude_; }
    float32 getMaxValue() const override { return amplitude_; }
    bool isNormalized() const override { return false; }
    
    Noise* clone() const override;
    std::string toString() const override;
    
    // Perlin-specific methods
    void setQuality(NoiseQuality quality);
    void setInterpolation(NoiseInterpolation interpolation);
    
private:
    uint32_t seed_;
    float32 frequency_;
    float32 amplitude_;
    int32_t octaves_;
    float32 persistence_;
    float32 lacunarity_;
    NoiseQuality quality_;
    NoiseInterpolation interpolation_;
    
    std::vector<int32_t> permutation_;
    
    void generatePermutation();
    float32 fade(float32 t) const;
    float32 lerp(float32 a, float32 b, float32 t) const;
    float32 grad(int32_t hash, float32 x) const;
    float32 grad(int32_t hash, float32 x, float32 y) const;
    float32 grad(int32_t hash, float32 x, float32 y, float32 z) const;
    float32 grad(int32_t hash, float32 x, float32 y, float32 z, float32 w) const;
    
    float32 noise1D(float32 x) const;
    float32 noise2D(float32 x, float32 y) const;
    float32 noise3D(float32 x, float32 y, float32 z) const;
    float32 noise4D(float32 x, float32 y, float32 z, float32 w) const;
    
    float32 fractalNoise1D(float32 x) const;
    float32 fractalNoise2D(float32 x, float32 y) const;
    float32 fractalNoise3D(float32 x, float32 y, float32 z) const;
    float32 fractalNoise4D(float32 x, float32 y, float32 z, float32 w) const;
};

// Simplex noise implementation
class SimplexNoise : public Noise {
public:
    SimplexNoise();
    SimplexNoise(uint32_t seed, float32 frequency = DEFAULT_FREQUENCY, int32_t octaves = DEFAULT_OCTAVES,
                 float32 persistence = DEFAULT_PERSISTENCE, float32 lacunarity = DEFAULT_LACUNARITY);
    
    float32 evaluate(float32 x) const override;
    float32 evaluate(float32 x, float32 y) const override;
    float32 evaluate(float32 x, float32 y, float32 z) const override;
    float32 evaluate(float32 x, float32 y, float32 z, float32 w) const override;
    
    NoiseType getType() const override { return NoiseType::Simplex; }
    NoiseQuality getQuality() const override { return NoiseQuality::Standard; }
    NoiseInterpolation getInterpolation() const override { return NoiseInterpolation::Cubic; }
    
    void setSeed(uint32_t seed) override;
    uint32_t getSeed() const override { return seed_; }
    
    void setFrequency(float32 frequency) override;
    float32 getFrequency() const override { return frequency_; }
    
    void setAmplitude(float32 amplitude) override;
    float32 getAmplitude() const override { return amplitude_; }
    
    void setOctaves(int32_t octaves) override;
    int32_t getOctaves() const override { return octaves_; }
    
    void setPersistence(float32 persistence) override;
    float32 getPersistence() const override { return persistence_; }
    
    void setLacunarity(float32 lacunarity) override;
    float32 getLacunarity() const override { return lacunarity_; }
    
    float32 getMinValue() const override { return -amplitude_; }
    float32 getMaxValue() const override { return amplitude_; }
    bool isNormalized() const override { return false; }
    
    Noise* clone() const override;
    std::string toString() const override;
    
private:
    uint32_t seed_;
    float32 frequency_;
    float32 amplitude_;
    int32_t octaves_;
    float32 persistence_;
    float32 lacunarity_;
    
    std::vector<int32_t> permutation_;
    
    void generatePermutation();
    
    float32 noise2D(float32 x, float32 y) const;
    float32 noise3D(float32 x, float32 y, float32 z) const;
    float32 noise4D(float32 x, float32 y, float32 z, float32 w) const;
    
    float32 fractalNoise2D(float32 x, float32 y) const;
    float32 fractalNoise3D(float32 x, float32 y, float32 z) const;
    float32 fractalNoise4D(float32 x, float32 y, float32 z, float32 w) const;
    
    static const int32_t SIMPLEX_GRADIENTS[12][4];
    static const int32_t SIMPLEX[64][4];
};

// Worley noise implementation
class WorleyNoise : public Noise {
public:
    WorleyNoise();
    WorleyNoise(uint32_t seed, float32 frequency = DEFAULT_FREQUENCY, int32_t octaves = DEFAULT_OCTAVES,
                float32 persistence = DEFAULT_PERSISTENCE, float32 lacunarity = DEFAULT_LACUNARITY,
                NoiseQuality quality = NoiseQuality::Standard);
    
    float32 evaluate(float32 x) const override;
    float32 evaluate(float32 x, float32 y) const override;
    float32 evaluate(float32 x, float32 y, float32 z) const override;
    float32 evaluate(float32 x, float32 y, float32 z, float32 w) const override;
    
    NoiseType getType() const override { return NoiseType::Worley; }
    NoiseQuality getQuality() const override { return quality_; }
    NoiseInterpolation getInterpolation() const override { return NoiseInterpolation::Linear; }
    
    void setSeed(uint32_t seed) override;
    uint32_t getSeed() const override { return seed_; }
    
    void setFrequency(float32 frequency) override;
    float32 getFrequency() const override { return frequency_; }
    
    void setAmplitude(float32 amplitude) override;
    float32 getAmplitude() const override { return amplitude_; }
    
    void setOctaves(int32_t octaves) override;
    int32_t getOctaves() const override { return octaves_; }
    
    void setPersistence(float32 persistence) override;
    float32 getPersistence() const override { return persistence_; }
    
    void setLacunarity(float32 lacunarity) override;
    float32 getLacunarity() const override { return lacunarity_; }
    
    float32 getMinValue() const override { return 0.0f; }
    float32 getMaxValue() const override { return amplitude_; }
    bool isNormalized() const override { return false; }
    
    Noise* clone() const override;
    std::string toString() const override;
    
    // Worley-specific methods
    void setQuality(NoiseQuality quality);
    void setDistanceFunction(std::function<float32(float32, float32)> distanceFunc);
    void setDistanceFunction(std::function<float32(float32, float32, float32)> distanceFunc3D);
    void setDistanceFunction(std::function<float32(float32, float32, float32, float32)> distanceFunc4D);
    
private:
    uint32_t seed_;
    float32 frequency_;
    float32 amplitude_;
    int32_t octaves_;
    float32 persistence_;
    float32 lacunarity_;
    NoiseQuality quality_;
    
    std::function<float32(float32, float32)> distanceFunc2D_;
    std::function<float32(float32, float32, float32)> distanceFunc3D_;
    std::function<float32(float32, float32, float32, float32)> distanceFunc4D_;
    
    float32 euclideanDistance2D(float32 x1, float32 y1, float32 x2, float32 y2) const;
    float32 manhattanDistance2D(float32 x1, float32 y1, float32 x2, float32 y2) const;
    float32 chebyshevDistance2D(float32 x1, float32 y1, float32 x2, float32 y2) const;
    
    float32 euclideanDistance3D(float32 x1, float32 y1, float32 z1, float32 x2, float32 y2, float32 z2) const;
    float32 manhattanDistance3D(float32 x1, float32 y1, float32 z1, float32 x2, float32 y2, float32 z2) const;
    float32 chebyshevDistance3D(float32 x1, float32 y1, float32 z1, float32 x2, float32 y2, float32 z2) const;
    
    float32 euclideanDistance4D(float32 x1, float32 y1, float32 z1, float32 w1, float32 x2, float32 y2, float32 z2, float32 w2) const;
    float32 manhattanDistance4D(float32 x1, float32 y1, float32 z1, float32 w1, float32 x2, float32 y2, float32 z2, float32 w2) const;
    float32 chebyshevDistance4D(float32 x1, float32 y1, float32 z1, float32 w1, float32 x2, float32 y2, float32 z2, float32 w2) const;
    
    float32 noise2D(float32 x, float32 y) const;
    float32 noise3D(float32 x, float32 y, float32 z) const;
    float32 noise4D(float32 x, float32 y, float32 z, float32 w) const;
    
    float32 fractalNoise2D(float32 x, float32 y) const;
    float32 fractalNoise3D(float32 x, float32 y, float32 z) const;
    float32 fractalNoise4D(float32 x, float32 y, float32 z, float32 w) const;
    
    uint32_t hash(int32_t x, int32_t y) const;
    uint32_t hash(int32_t x, int32_t y, int32_t z) const;
    uint32_t hash(int32_t x, int32_t y, int32_t z, int32_t w) const;
};

// Fractal noise implementation
class FractalNoise : public Noise {
public:
    FractalNoise();
    FractalNoise(std::unique_ptr<Noise> baseNoise, int32_t octaves = DEFAULT_OCTAVES,
                 float32 persistence = DEFAULT_PERSISTENCE, float32 lacunarity = DEFAULT_LACUNARITY,
                 float32 gain = 1.0f, bool normalize = false);
    
    float32 evaluate(float32 x) const override;
    float32 evaluate(float32 x, float32 y) const override;
    float32 evaluate(float32 x, float32 y, float32 z) const override;
    float32 evaluate(float32 x, float32 y, float32 z, float32 w) const override;
    
    NoiseType getType() const override { return NoiseType::Fractal; }
    NoiseQuality getQuality() const override { return baseNoise_->getQuality(); }
    NoiseInterpolation getInterpolation() const override { return baseNoise_->getInterpolation(); }
    
    void setSeed(uint32_t seed) override;
    uint32_t getSeed() const override { return baseNoise_->getSeed(); }
    
    void setFrequency(float32 frequency) override;
    float32 getFrequency() const override { return baseNoise_->getFrequency(); }
    
    void setAmplitude(float32 amplitude) override;
    float32 getAmplitude() const override { return amplitude_; }
    
    void setOctaves(int32_t octaves) override;
    int32_t getOctaves() const override { return octaves_; }
    
    void setPersistence(float32 persistence) override;
    float32 getPersistence() const override { return persistence_; }
    
    void setLacunarity(float32 lacunarity) override;
    float32 getLacunarity() const override { return lacunarity_; }
    
    float32 getMinValue() const override;
    float32 getMaxValue() const override;
    bool isNormalized() const override { return normalize_; }
    
    Noise* clone() const override;
    std::string toString() const override;
    
    // Fractal-specific methods
    void setBaseNoise(std::unique_ptr<Noise> baseNoise);
    Noise* getBaseNoise() const { return baseNoise_.get(); }
    
    void setGain(float32 gain);
    float32 getGain() const { return gain_; }
    
    void setNormalize(bool normalize);
    bool getNormalize() const { return normalize_; }
    
private:
    std::unique_ptr<Noise> baseNoise_;
    int32_t octaves_;
    float32 persistence_;
    float32 lacunarity_;
    float32 amplitude_;
    float32 gain_;
    bool normalize_;
    
    mutable float32 cachedMinValue_;
    mutable float32 cachedMaxValue_;
    mutable bool valuesCached_;
    
    void computeValueRange() const;
    void invalidateCache();
};

// White noise implementation
class WhiteNoise : public Noise {
public:
    WhiteNoise();
    WhiteNoise(uint32_t seed, float32 amplitude = DEFAULT_AMPLITUDE);
    
    float32 evaluate(float32 x) const override;
    float32 evaluate(float32 x, float32 y) const override;
    float32 evaluate(float32 x, float32 y, float32 z) const override;
    float32 evaluate(float32 x, float32 y, float32 z, float32 w) const override;
    
    NoiseType getType() const override { return NoiseType::White; }
    NoiseQuality getQuality() const override { return NoiseQuality::Fast; }
    NoiseInterpolation getInterpolation() const override { return NoiseInterpolation::Linear; }
    
    void setSeed(uint32_t seed) override;
    uint32_t getSeed() const override { return seed_; }
    
    void setFrequency(float32 frequency) override {}
    float32 getFrequency() const override { return 1.0f; }
    
    void setAmplitude(float32 amplitude) override;
    float32 getAmplitude() const override { return amplitude_; }
    
    void setOctaves(int32_t octaves) override {}
    int32_t getOctaves() const override { return 1; }
    
    void setPersistence(float32 persistence) override {}
    float32 getPersistence() const override { return 0.5f; }
    
    void setLacunarity(float32 lacunarity) override {}
    float32 getLacunarity() const override { return 2.0f; }
    
    float32 getMinValue() const override { return -amplitude_; }
    float32 getMaxValue() const override { return amplitude_; }
    bool isNormalized() const override { return false; }
    
    Noise* clone() const override;
    std::string toString() const override;
    
private:
    uint32_t seed_;
    float32 amplitude_;
    
    float32 randomFloat32(float32 x) const;
    float32 randomFloat32(float32 x, float32 y) const;
    float32 randomFloat32(float32 x, float32 y, float32 z) const;
    float32 randomFloat32(float32 x, float32 y, float32 z, float32 w) const;
    
    uint32_t hash(uint32_t value) const;
};

// Noise utilities namespace
namespace NoiseUtils {
    // Noise operations
    float32 evaluate(const Noise& noise, float32 x);
    float32 evaluate(const Noise& noise, float32 x, float32 y);
    float32 evaluate(const Noise& noise, float32 x, float32 y, float32 z);
    float32 evaluate(const Noise& noise, float32 x, float32 y, float32 z, float32 w);
    
    Vector2 evaluate2D(const Noise& noise, float32 x, float32 y);
    Vector3 evaluate3D(const Noise& noise, float32 x, float32 y, float32 z);
    Vector4 evaluate4D(const Noise& noise, float32 x, float32 y, float32 z, float32 w);
    
    // Noise combinations
    class NoiseCombiner {
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
        
        NoiseCombiner();
        NoiseCombiner(Operation operation);
        
        void addNoise(std::unique_ptr<Noise> noise, float32 weight = 1.0f);
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
        Noise* getNoise(size_t index) const;
        
        NoiseType getType() const;
        NoiseQuality getQuality() const;
        NoiseInterpolation getInterpolation() const;
        
        void setSeed(uint32_t seed);
        uint32_t getSeed() const;
        
        void setFrequency(float32 frequency);
        float32 getFrequency() const;
        
        void setAmplitude(float32 amplitude);
        float32 getAmplitude() const;
        
        void setOctaves(int32_t octaves);
        int32_t getOctaves() const;
        
        void setPersistence(float32 persistence);
        float32 getPersistence() const;
        
        void setLacunarity(float32 lacunarity);
        float32 getLacunarity() const;
        
        float32 getMinValue() const;
        float32 getMaxValue() const;
        bool isNormalized() const;
        
        std::unique_ptr<Noise> clone() const;
        std::string toString() const;
        
    private:
        Operation operation_;
        std::vector<std::pair<std::unique_ptr<Noise>, float32>> noises_;
        
        float32 combineValues(const std::vector<float32>& values) const;
    };
    
    // Noise modifiers
    class NoiseModifier {
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
        
        NoiseModifier();
        NoiseModifier(Type type);
        NoiseModifier(Type type, float32 param);
        
        float32 apply(float32 value) const;
        
        void setType(Type type);
        Type getType() const { return type_; }
        
        void setParameter(float32 param);
        float32 getParameter() const { return param_; }
        
        std::unique_ptr<Noise> apply(std::unique_ptr<Noise> noise) const;
        
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
    std::unique_ptr<Noise> createPerlinNoise(uint32_t seed = 0, float32 frequency = 1.0f, int32_t octaves = 4,
                                             float32 persistence = 0.5f, float32 lacunarity = 2.0f,
                                             NoiseQuality quality = NoiseQuality::Standard);
    
    std::unique_ptr<Noise> createSimplexNoise(uint32_t seed = 0, float32 frequency = 1.0f, int32_t octaves = 4,
                                              float32 persistence = 0.5f, float32 lacunarity = 2.0f);
    
    std::unique_ptr<Noise> createWorleyNoise(uint32_t seed = 0, float32 frequency = 1.0f, int32_t octaves = 4,
                                            float32 persistence = 0.5f, float32 lacunarity = 2.0f,
                                            NoiseQuality quality = NoiseQuality::Standard);
    
    std::unique_ptr<Noise> createFractalNoise(std::unique_ptr<Noise> baseNoise, int32_t octaves = 4,
                                              float32 persistence = 0.5f, float32 lacunarity = 2.0f,
                                              float32 gain = 1.0f, bool normalize = false);
    
    std::unique_ptr<Noise> createWhiteNoise(uint32_t seed = 0, float32 amplitude = 1.0f);
    
    std::unique_ptr<Noise> createTurbulenceNoise(std::unique_ptr<Noise> baseNoise, float32 power = 1.0f);
    std::unique_ptr<Noise> createRidgedNoise(std::unique_ptr<Noise> baseNoise, float32 power = 1.0f);
    std::unique_ptr<Noise> createBillowNoise(std::unique_ptr<Noise> baseNoise, float32 power = 1.0f);
    
    // Noise analysis
    struct NoiseAnalysis {
        NoiseType type;
        NoiseQuality quality;
        NoiseInterpolation interpolation;
        float32 minValue;
        float32 maxValue;
        float32 meanValue;
        float32 variance;
        float32 standardDeviation;
        float32 frequency;
        float32 amplitude;
        int32_t octaves;
        float32 persistence;
        float32 lacunarity;
        uint32_t seed;
        bool isNormalized;
        std::string description;
    };
    
    NoiseAnalysis analyze(const Noise& noise, size_t samples = 10000);
    
    // Noise validation
    bool validate(const Noise& noise);
    bool validateRange(const Noise& noise, float32 expectedMin, float32 expectedMax, float32 tolerance = 0.01f);
    bool validateContinuity(const Noise& noise, float32 tolerance = 0.01f);
    bool validatePeriodicity(const Noise& noise, float32 tolerance = 0.01f);
    
    // Noise debugging
    std::string toString(const Noise& noise);
    void debugPrint(const Noise& noise);
    void debugPrintAnalysis(const Noise& noise);
    
    // Noise sampling
    std::vector<float32> sampleNoise(const Noise& noise, const Vector2& min, const Vector2& max, size_t width, size_t height);
    std::vector<float32> sampleNoise(const Noise& noise, const Vector3& min, const Vector3& max, size_t width, size_t height, size_t depth);
    std::vector<float32> sampleNoise(const Noise& noise, const Vector4& min, const Vector4& max, size_t width, size_t height, size_t depth, size_t time);
    
    // Noise visualization
    class NoiseVisualizer {
    public:
        NoiseVisualizer();
        
        void setNoise(std::unique_ptr<Noise> noise);
        void setRange(const Vector2& min, const Vector2& max);
        void setResolution(size_t width, size_t height);
        
        std::vector<float32> generateHeightmap() const;
        std::vector<uint8_t> generateGrayscaleImage() const;
        std::vector<uint8_t> generateColorImage() const;
        
        void saveHeightmap(const std::string& filename) const;
        void saveImage(const std::string& filename) const;
        
    private:
        std::unique_ptr<Noise> noise_;
        Vector2 min_;
        Vector2 max_;
        size_t width_;
        size_t height_;
        
        float32 normalizeValue(float32 value) const;
        uint8_t valueToByte(float32 value) const;
        void valueToRGB(float32 value, uint8_t& r, uint8_t& g, uint8_t& b) const;
    };
    
    // Noise cache
    class NoiseCache {
    public:
        NoiseCache(size_t maxSize = 1000);
        
        void put(const std::string& key, std::unique_ptr<Noise> noise);
        std::unique_ptr<Noise> get(const std::string& key);
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
            std::unique_ptr<Noise> noise;
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
    class NoisePool {
    public:
        NoisePool(size_t initialSize = 100);
        
        std::unique_ptr<Noise> acquire();
        void release(std::unique_ptr<Noise> noise);
        void clear();
        
        size_t getPoolSize() const;
        size_t getUsedCount() const;
        size_t getAvailableCount() const;
        
    private:
        std::vector<std::unique_ptr<Noise>> pool_;
        std::vector<std::unique_ptr<Noise>*> available_;
        std::vector<std::unique_ptr<Noise>*> used_;
        
        void expandPool(size_t newSize);
    };
    
    // Noise presets
    namespace Presets {
        std::unique_ptr<Noise> createTerrainNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createMarbleNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createWoodNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createCloudNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createFireNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createWaterNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createLavaNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createIceNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createSandNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createGrassNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createRockNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createMetalNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createFabricNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createPaperNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createConcreteNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createBrickNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createStoneNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createDirtNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createSnowNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createMudNoise(uint32_t seed = 0);
        std::unique_ptr<Noise> createGravelNoise(uint32_t seed = 0);
    }
}

} // namespace RFUtils