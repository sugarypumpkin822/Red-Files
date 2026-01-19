#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <chrono>
#include <random>

namespace RFUtils {

// Random number generator types
enum class RandomType {
    LinearCongruential,
    MersenneTwister,
    XorShift,
    XorShift64,
    XorShift128,
    XorShift256,
    PCG,
    SplitMix64,
    WELL512,
    WELL1024,
    WELL2048,
    ISAAC,
    ISAAC64,
    ChaCha,
    ChaCha20,
    Secure,
    Hardware
};

// Random distribution types
enum class DistributionType {
    Uniform,
    Normal,
    Exponential,
    Gamma,
    Beta,
    ChiSquared,
    StudentT,
    FisherF,
    LogNormal,
    Weibull,
    Pareto,
    Triangular,
    Discrete,
    Poisson,
    Binomial,
    Geometric,
    NegativeBinomial,
    Hypergeometric,
    Bernoulli,
    Categorical,
    Dirichlet,
    Multinomial
};

// Random number generator base class
class RandomGenerator {
public:
    RandomGenerator() = default;
    virtual ~RandomGenerator() = default;
    
    // Seed operations
    virtual void seed(uint32_t seed) = 0;
    virtual void seed(uint64_t seed) = 0;
    virtual void seed(const std::vector<uint32_t>& seeds) = 0;
    virtual void seedFromTime() = 0;
    virtual void seedFromHardware() = 0;
    
    // Basic random number generation
    virtual uint32_t nextUint32() = 0;
    virtual uint64_t nextUint64() = 0;
    virtual int32_t nextInt32() = 0;
    virtual int64_t nextInt64() = 0;
    virtual float32 nextFloat32() = 0;
    virtual float64 nextFloat64() = 0;
    virtual bool nextBool() = 0;
    
    // Range-based generation
    virtual int32_t nextInt32(int32_t min, int32_t max) = 0;
    virtual int64_t nextInt64(int64_t min, int64_t max) = 0;
    virtual float32 nextFloat32(float32 min, float32 max) = 0;
    virtual float64 nextFloat64(float64 min, float64 max) = 0;
    
    // Distribution-based generation
    virtual float32 nextNormal(float32 mean = 0.0f, float32 stdDev = 1.0f) = 0;
    virtual float64 nextNormal(double mean = 0.0, double stdDev = 1.0) = 0;
    virtual float32 nextExponential(float32 lambda = 1.0f) = 0;
    virtual float64 nextExponential(double lambda = 1.0) = 0;
    virtual float32 nextGamma(float32 shape, float32 scale) = 0;
    virtual float64 nextGamma(double shape, double scale) = 0;
    virtual float32 nextBeta(float32 alpha, float32 beta) = 0;
    virtual float64 nextBeta(double alpha, double beta) = 0;
    virtual float32 nextChiSquared(float32 degreesOfFreedom) = 0;
    virtual float64 nextChiSquared(double degreesOfFreedom) = 0;
    virtual float32 nextStudentT(float32 degreesOfFreedom) = 0;
    virtual float64 nextStudentT(double degreesOfFreedom) = 0;
    virtual float32 nextFisherF(float32 d1, float32 d2) = 0;
    virtual float64 nextFisherF(double d1, double d2) = 0;
    virtual float32 nextLogNormal(float32 mean, float32 stdDev) = 0;
    virtual float64 nextLogNormal(double mean, double stdDev) = 0;
    virtual float32 nextWeibull(float32 shape, float32 scale) = 0;
    virtual float64 nextWeibull(double shape, double scale) = 0;
    virtual float32 nextPareto(float32 shape, float32 scale) = 0;
    virtual float64 nextPareto(double shape, double scale) = 0;
    virtual float32 nextTriangular(float32 a, float32 b, float32 c) = 0;
    virtual float64 nextTriangular(double a, double b, double c) = 0;
    
    // Discrete distributions
    virtual int32_t nextPoisson(float32 mean) = 0;
    virtual int64_t nextPoisson(double mean) = 0;
    virtual int32_t nextBinomial(int32_t trials, float32 probability) = 0;
    virtual int64_t nextBinomial(int64_t trials, double probability) = 0;
    virtual int32_t nextGeometric(float32 probability) = 0;
    virtual int64_t nextGeometric(double probability) = 0;
    virtual int32_t nextNegativeBinomial(int32_t failures, float32 probability) = 0;
    virtual int64_t nextNegativeBinomial(int64_t failures, double probability) = 0;
    virtual int32_t nextHypergeometric(int32_t population, int32_t successes, int32_t draws) = 0;
    virtual int64_t nextHypergeometric(int64_t population, int64_t successes, int64_t draws) = 0;
    virtual bool nextBernoulli(float32 probability) = 0;
    virtual bool nextBernoulli(double probability) = 0;
    virtual size_t nextCategorical(const std::vector<float32>& probabilities) = 0;
    virtual size_t nextCategorical(const std::vector<double>& probabilities) = 0;
    
    // Multi-dimensional distributions
    virtual std::vector<float32> nextDirichlet(const std::vector<float32>& alpha) = 0;
    virtual std::vector<double> nextDirichlet(const std::vector<double>& alpha) = 0;
    virtual std::vector<int32_t> nextMultinomial(int32_t trials, const std::vector<float32>& probabilities) = 0;
    virtual std::vector<int64_t> nextMultinomial(int64_t trials, const std::vector<double>& probabilities) = 0;
    
    // Utility methods
    virtual void discard(uint64_t count) = 0;
    virtual void jump() = 0;
    virtual void jump(uint64_t steps) = 0;
    virtual RandomType getType() const = 0;
    virtual std::string getName() const = 0;
    virtual uint64_t getStateSize() const = 0;
    virtual std::vector<uint32_t> getState() const = 0;
    virtual void setState(const std::vector<uint32_t>& state) = 0;
    virtual void saveState(const std::string& filename) const = 0;
    virtual void loadState(const std::string& filename) = 0;
    
    // Comparison operators
    virtual bool operator==(const RandomGenerator& other) const = 0;
    virtual bool operator!=(const RandomGenerator& other) const = 0;
};

// Linear Congruential Generator
class LinearCongruentialGenerator : public RandomGenerator {
public:
    LinearCongruentialGenerator(uint32_t seed = 0);
    LinearCongruentialGenerator(uint64_t seed);
    LinearCongruentialGenerator(const std::vector<uint32_t>& seeds);
    
    void seed(uint32_t seed) override;
    void seed(uint64_t seed) override;
    void seed(const std::vector<uint32_t>& seeds) override;
    void seedFromTime() override;
    void seedFromHardware() override;
    
    uint32_t nextUint32() override;
    uint64_t nextUint64() override;
    int32_t nextInt32() override;
    int64_t nextInt64() override;
    float32 nextFloat32() override;
    float64 nextFloat64() override;
    bool nextBool() override;
    
    int32_t nextInt32(int32_t min, int32_t max) override;
    int64_t nextInt64(int64_t min, int64_t max) override;
    float32 nextFloat32(float32 min, float32 max) override;
    float64 nextFloat64(float64 min, float64 max) override;
    
    float32 nextNormal(float32 mean = 0.0f, float32 stdDev = 1.0f) override;
    float64 nextNormal(double mean = 0.0, double stdDev = 1.0) override;
    float32 nextExponential(float32 lambda = 1.0f) override;
    float64 nextExponential(double lambda = 1.0) override;
    float32 nextGamma(float32 shape, float32 scale) override;
    float64 nextGamma(double shape, double scale) override;
    float32 nextBeta(float32 alpha, float32 beta) override;
    float64 nextBeta(double alpha, double beta) override;
    float32 nextChiSquared(float32 degreesOfFreedom) override;
    float64 nextChiSquared(double degreesOfFreedom) override;
    float32 nextStudentT(float32 degreesOfFreedom) override;
    float64 nextStudentT(double degreesOfFreedom) override;
    float32 nextFisherF(float32 d1, float32 d2) override;
    float64 nextFisherF(double d1, double d2) override;
    float32 nextLogNormal(float32 mean, float32 stdDev) override;
    float64 nextLogNormal(double mean, double stdDev) override;
    float32 nextWeibull(float32 shape, float32 scale) override;
    float64 nextWeibull(double shape, double scale) override;
    float32 nextPareto(float32 shape, float32 scale) override;
    float64 nextPareto(double shape, double scale) override;
    float32 nextTriangular(float32 a, float32 b, float32 c) override;
    float64 nextTriangular(double a, double b, double c) override;
    
    int32_t nextPoisson(float32 mean) override;
    int64_t nextPoisson(double mean) override;
    int32_t nextBinomial(int32_t trials, float32 probability) override;
    int64_t nextBinomial(int64_t trials, double probability) override;
    int32_t nextGeometric(float32 probability) override;
    int64_t nextGeometric(double probability) override;
    int32_t nextNegativeBinomial(int32_t failures, float32 probability) override;
    int64_t nextNegativeBinomial(int64_t failures, double probability) override;
    int32_t nextHypergeometric(int32_t population, int32_t successes, int32_t draws) override;
    int64_t nextHypergeometric(int64_t population, int64_t successes, int64_t draws) override;
    bool nextBernoulli(float32 probability) override;
    bool nextBernoulli(double probability) override;
    size_t nextCategorical(const std::vector<float32>& probabilities) override;
    size_t nextCategorical(const std::vector<double>& probabilities) override;
    
    std::vector<float32> nextDirichlet(const std::vector<float32>& alpha) override;
    std::vector<double> nextDirichlet(const std::vector<double>& alpha) override;
    std::vector<int32_t> nextMultinomial(int32_t trials, const std::vector<float32>& probabilities) override;
    std::vector<int64_t> nextMultinomial(int64_t trials, const std::vector<double>& probabilities) override;
    
    void discard(uint64_t count) override;
    void jump() override;
    void jump(uint64_t steps) override;
    RandomType getType() const override;
    std::string getName() const override;
    uint64_t getStateSize() const override;
    std::vector<uint32_t> getState() const override;
    void setState(const std::vector<uint32_t>& state) override;
    void saveState(const std::string& filename) const override;
    void loadState(const std::string& filename) override;
    
    bool operator==(const RandomGenerator& other) const override;
    bool operator!=(const RandomGenerator& other) const override;
    
    // LCG-specific methods
    void setParameters(uint32_t a, uint32_t c, uint32_t m);
    uint32_t getA() const { return a_; }
    uint32_t getC() const { return c_; }
    uint32_t getM() const { return m_; }
    uint32_t getState() const { return state_; }
    
private:
    uint32_t state_;
    uint32_t a_, c_, m_;
    static constexpr uint32_t DEFAULT_A = 1664525u;
    static constexpr uint32_t DEFAULT_C = 1013904223u;
    static constexpr uint32_t DEFAULT_M = 0xFFFFFFFFu;
};

// Mersenne Twister Generator
class MersenneTwisterGenerator : public RandomGenerator {
public:
    MersenneTwisterGenerator(uint32_t seed = 0);
    MersenneTwisterGenerator(uint64_t seed);
    MersenneTwisterGenerator(const std::vector<uint32_t>& seeds);
    
    void seed(uint32_t seed) override;
    void seed(uint64_t seed) override;
    void seed(const std::vector<uint32_t>& seeds) override;
    void seedFromTime() override;
    void seedFromHardware() override;
    
    uint32_t nextUint32() override;
    uint64_t nextUint64() override;
    int32_t nextInt32() override;
    int64_t nextInt64() override;
    float32 nextFloat32() override;
    float64 nextFloat64() override;
    bool nextBool() override;
    
    int32_t nextInt32(int32_t min, int32_t max) override;
    int64_t nextInt64(int64_t min, int64_t max) override;
    float32 nextFloat32(float32 min, float32 max) override;
    float64 nextFloat64(float64 min, float64 max) override;
    
    float32 nextNormal(float32 mean = 0.0f, float32 stdDev = 1.0f) override;
    float64 nextNormal(double mean = 0.0, double stdDev = 1.0) override;
    float32 nextExponential(float32 lambda = 1.0f) override;
    float64 nextExponential(double lambda = 1.0) override;
    float32 nextGamma(float32 shape, float32 scale) override;
    float64 nextGamma(double shape, double scale) override;
    float32 nextBeta(float32 alpha, float32 beta) override;
    float64 nextBeta(double alpha, double beta) override;
    float32 nextChiSquared(float32 degreesOfFreedom) override;
    float64 nextChiSquared(double degreesOfFreedom) override;
    float32 nextStudentT(float32 degreesOfFreedom) override;
    float64 nextStudentT(double degreesOfFreedom) override;
    float32 nextFisherF(float32 d1, float32 d2) override;
    float64 nextFisherF(double d1, double d2) override;
    float32 nextLogNormal(float32 mean, float32 stdDev) override;
    float64 nextLogNormal(double mean, double stdDev) override;
    float32 nextWeibull(float32 shape, float32 scale) override;
    float64 nextWeibull(double shape, double scale) override;
    float32 nextPareto(float32 shape, float32 scale) override;
    float64 nextPareto(double shape, double scale) override;
    float32 nextTriangular(float32 a, float32 b, float32 c) override;
    float64 nextTriangular(double a, double b, double c) override;
    
    int32_t nextPoisson(float32 mean) override;
    int64_t nextPoisson(double mean) override;
    int32_t nextBinomial(int32_t trials, float32 probability) override;
    int64_t nextBinomial(int64_t trials, double probability) override;
    int32_t nextGeometric(float32 probability) override;
    int64_t nextGeometric(double probability) override;
    int32_t nextNegativeBinomial(int32_t failures, float32 probability) override;
    int64_t nextNegativeBinomial(int64_t failures, double probability) override;
    int32_t nextHypergeometric(int32_t population, int32_t successes, int32_t draws) override;
    int64_t nextHypergeometric(int64_t population, int64_t successes, int64_t draws) override;
    bool nextBernoulli(float32 probability) override;
    bool nextBernoulli(double probability) override;
    size_t nextCategorical(const std::vector<float32>& probabilities) override;
    size_t nextCategorical(const std::vector<double>& probabilities) override;
    
    std::vector<float32> nextDirichlet(const std::vector<float32>& alpha) override;
    std::vector<double> nextDirichlet(const std::vector<double>& alpha) override;
    std::vector<int32_t> nextMultinomial(int32_t trials, const std::vector<float32>& probabilities) override;
    std::vector<int64_t> nextMultinomial(int64_t trials, const std::vector<double>& probabilities) override;
    
    void discard(uint64_t count) override;
    void jump() override;
    void jump(uint64_t steps) override;
    RandomType getType() const override;
    std::string getName() const override;
    uint64_t getStateSize() const override;
    std::vector<uint32_t> getState() const override;
    void setState(const std::vector<uint32_t>& state) override;
    void saveState(const std::string& filename) const override;
    void loadState(const std::string& filename) override;
    
    bool operator==(const RandomGenerator& other) const override;
    bool operator!=(const RandomGenerator& other) const override;
    
private:
    static constexpr size_t N = 624;
    static constexpr size_t M = 397;
    static constexpr uint32_t MATRIX_A = 0x9908b0dfu;
    static constexpr uint32_t UPPER_MASK = 0x80000000u;
    static constexpr uint32_t LOWER_MASK = 0x7fffffffu;
    
    std::array<uint32_t, N> mt_;
    size_t index_;
    
    void twist();
    void initialize(uint32_t seed);
};

// XorShift Generator
class XorShiftGenerator : public RandomGenerator {
public:
    XorShiftGenerator(uint32_t seed = 0);
    XorShiftGenerator(uint64_t seed);
    XorShiftGenerator(const std::vector<uint32_t>& seeds);
    
    void seed(uint32_t seed) override;
    void seed(uint64_t seed) override;
    void seed(const std::vector<uint32_t>& seeds) override;
    void seedFromTime() override;
    void seedFromHardware() override;
    
    uint32_t nextUint32() override;
    uint64_t nextUint64() override;
    int32_t nextInt32() override;
    int64_t nextInt64() override;
    float32 nextFloat32() override;
    float64 nextFloat64() override;
    bool nextBool() override;
    
    int32_t nextInt32(int32_t min, int32_t max) override;
    int64_t nextInt64(int64_t min, int64_t max) override;
    float32 nextFloat32(float32 min, float32 max) override;
    float64 nextFloat64(float64 min, float64 max) override;
    
    float32 nextNormal(float32 mean = 0.0f, float32 stdDev = 1.0f) override;
    float64 nextNormal(double mean = 0.0, double stdDev = 1.0) override;
    float32 nextExponential(float32 lambda = 1.0f) override;
    float64 nextExponential(double lambda = 1.0) override;
    float32 nextGamma(float32 shape, float32 scale) override;
    float64 nextGamma(double shape, double scale) override;
    float32 nextBeta(float32 alpha, float32 beta) override;
    float64 nextBeta(double alpha, double beta) override;
    float32 nextChiSquared(float32 degreesOfFreedom) override;
    float64 nextChiSquared(double degreesOfFreedom) override;
    float32 nextStudentT(float32 degreesOfFreedom) override;
    float64 nextStudentT(double degreesOfFreedom) override;
    float32 nextFisherF(float32 d1, float32 d2) override;
    float64 nextFisherF(double d1, double d2) override;
    float32 nextLogNormal(float32 mean, float32 stdDev) override;
    float64 nextLogNormal(double mean, double stdDev) override;
    float32 nextWeibull(float32 shape, float32 scale) override;
    float64 nextWeibull(double shape, double scale) override;
    float32 nextPareto(float32 shape, float32 scale) override;
    float64 nextPareto(double shape, double scale) override;
    float32 nextTriangular(float32 a, float32 b, float32 c) override;
    float64 nextTriangular(double a, double b, double c) override;
    
    int32_t nextPoisson(float32 mean) override;
    int64_t nextPoisson(double mean) override;
    int32_t nextBinomial(int32_t trials, float32 probability) override;
    int64_t nextBinomial(int64_t trials, double probability) override;
    int32_t nextGeometric(float32 probability) override;
    int64_t nextGeometric(double probability) override;
    int32_t nextNegativeBinomial(int32_t failures, float32 probability) override;
    int64_t nextNegativeBinomial(int64_t failures, double probability) override;
    int32_t nextHypergeometric(int32_t population, int32_t successes, int32_t draws) override;
    int64_t nextHypergeometric(int64_t population, int64_t successes, int64_t draws) override;
    bool nextBernoulli(float32 probability) override;
    bool nextBernoulli(double probability) override;
    size_t nextCategorical(const std::vector<float32>& probabilities) override;
    size_t nextCategorical(const std::vector<double>& probabilities) override;
    
    std::vector<float32> nextDirichlet(const std::vector<float32>& alpha) override;
    std::vector<double> nextDirichlet(const std::vector<double>& alpha) override;
    std::vector<int32_t> nextMultinomial(int32_t trials, const std::vector<float32>& probabilities) override;
    std::vector<int64_t> nextMultinomial(int64_t trials, const std::vector<double>& probabilities) override;
    
    void discard(uint64_t count) override;
    void jump() override;
    void jump(uint64_t steps) override;
    RandomType getType() const override;
    std::string getName() const override;
    uint64_t getStateSize() const override;
    std::vector<uint32_t> getState() const override;
    void setState(const std::vector<uint32_t>& state) override;
    void saveState(const std::string& filename) const override;
    void loadState(const std::string& filename) override;
    
    bool operator==(const RandomGenerator& other) const override;
    bool operator!=(const RandomGenerator& other) const override;
    
private:
    uint32_t state_;
};

// Random number generator factory
class RandomFactory {
public:
    static std::unique_ptr<RandomGenerator> create(RandomType type);
    static std::unique_ptr<RandomGenerator> create(RandomType type, uint32_t seed);
    static std::unique_ptr<RandomGenerator> create(RandomType type, uint64_t seed);
    static std::unique_ptr<RandomGenerator> create(RandomType type, const std::vector<uint32_t>& seeds);
    static std::unique_ptr<RandomGenerator> createFromTime(RandomType type);
    static std::unique_ptr<RandomGenerator> createFromHardware(RandomType type);
    
    static std::vector<std::string> getAvailableGenerators();
    static RandomType getTypeFromName(const std::string& name);
    static std::string getNameFromType(RandomType type);
    
    // Default generators
    static std::unique_ptr<RandomGenerator> createDefault();
    static std::unique_ptr<RandomGenerator> createFast();
    static std::unique_ptr<RandomGenerator> createSecure();
    static std::unique_ptr<RandomGenerator> createHighQuality();
};

// Random utilities
namespace RandomUtils {
    // Global random generator
    RandomGenerator& getDefaultGenerator();
    RandomGenerator& getFastGenerator();
    RandomGenerator& getSecureGenerator();
    RandomGenerator& getHighQualityGenerator();
    
    void setDefaultGenerator(std::unique_ptr<RandomGenerator> generator);
    void setFastGenerator(std::unique_ptr<RandomGenerator> generator);
    void setSecureGenerator(std::unique_ptr<RandomGenerator> generator);
    void setHighQualityGenerator(std::unique_ptr<RandomGenerator> generator);
    
    // Convenience functions using default generator
    uint32_t randomUint32();
    uint64_t randomUint64();
    int32_t randomInt32();
    int64_t randomInt64();
    float32 randomFloat32();
    float64 randomFloat64();
    bool randomBool();
    
    int32_t randomInt32(int32_t min, int32_t max);
    int64_t randomInt64(int64_t min, int64_t max);
    float32 randomFloat32(float32 min, float32 max);
    float64 randomFloat64(float64 min, float64 max);
    
    float32 randomNormal(float32 mean = 0.0f, float32 stdDev = 1.0f);
    float64 randomNormal(double mean = 0.0, double stdDev = 1.0);
    float32 randomExponential(float32 lambda = 1.0f);
    float64 randomExponential(double lambda = 1.0);
    float32 randomGamma(float32 shape, float32 scale);
    float64 randomGamma(double shape, double scale);
    float32 randomBeta(float32 alpha, float32 beta);
    float64 randomBeta(double alpha, double beta);
    float32 randomChiSquared(float32 degreesOfFreedom);
    float64 randomChiSquared(double degreesOfFreedom);
    float32 randomStudentT(float32 degreesOfFreedom);
    float64 randomStudentT(double degreesOfFreedom);
    float32 randomFisherF(float32 d1, float32 d2);
    float64 randomFisherF(double d1, double d2);
    float32 randomLogNormal(float32 mean, float32 stdDev);
    float64 randomLogNormal(double mean, double stdDev);
    float32 randomWeibull(float32 shape, float32 scale);
    float64 randomWeibull(double shape, double scale);
    float32 randomPareto(float32 shape, float32 scale);
    float64 randomPareto(double shape, double scale);
    float32 randomTriangular(float32 a, float32 b, float32 c);
    float64 randomTriangular(double a, double b, double c);
    
    int32_t randomPoisson(float32 mean);
    int64_t randomPoisson(double mean);
    int32_t randomBinomial(int32_t trials, float32 probability);
    int64_t randomBinomial(int64_t trials, double probability);
    int32_t randomGeometric(float32 probability);
    int64_t randomGeometric(double probability);
    int32_t randomNegativeBinomial(int32_t failures, float32 probability);
    int64_t randomNegativeBinomial(int64_t failures, double probability);
    int32_t randomHypergeometric(int32_t population, int32_t successes, int32_t draws);
    int64_t randomHypergeometric(int64_t population, int64_t successes, int64_t draws);
    bool randomBernoulli(float32 probability);
    bool randomBernoulli(double probability);
    size_t randomCategorical(const std::vector<float32>& probabilities);
    size_t randomCategorical(const std::vector<double>& probabilities);
    
    std::vector<float32> randomDirichlet(const std::vector<float32>& alpha);
    std::vector<double> randomDirichlet(const std::vector<double>& alpha);
    std::vector<int32_t> randomMultinomial(int32_t trials, const std::vector<float32>& probabilities);
    std::vector<int64_t> randomMultinomial(int64_t trials, const std::vector<double>& probabilities);
    
    // Array generation
    void randomArray(uint32_t* array, size_t size);
    void randomArray(uint64_t* array, size_t size);
    void randomArray(int32_t* array, size_t size);
    void randomArray(int64_t* array, size_t size);
    void randomArray(float32* array, size_t size);
    void randomArray(double* array, size_t size);
    void randomArray(bool* array, size_t size);
    
    void randomArray(uint32_t* array, size_t size, uint32_t min, uint32_t max);
    void randomArray(uint64_t* array, size_t size, uint64_t min, uint64_t max);
    void randomArray(int32_t* array, size_t size, int32_t min, int32_t max);
    void randomArray(int64_t* array, size_t size, int64_t min, int64_t max);
    void randomArray(float32* array, size_t size, float32 min, float32 max);
    void randomArray(double* array, size_t size, double min, double max);
    
    // Shuffling
    void shuffle(uint32_t* array, size_t size);
    void shuffle(uint64_t* array, size_t size);
    void shuffle(int32_t* array, size_t size);
    void shuffle(int64_t* array, size_t size);
    void shuffle(float32* array, size_t size);
    void shuffle(double* array, size_t size);
    void shuffle(bool* array, size_t size);
    
    template<typename T>
    void shuffle(std::vector<T>& vector);
    
    // Sampling
    template<typename T>
    std::vector<T> sample(const std::vector<T>& population, size_t count, bool withReplacement = false);
    
    template<typename T>
    T sample(const std::vector<T>& population);
    
    template<typename T>
    std::vector<T> weightedSample(const std::vector<T>& population, const std::vector<float32>& weights, size_t count);
    
    template<typename T>
    T weightedSample(const std::vector<T>& population, const std::vector<float32>& weights);
    
    // Random strings
    std::string randomString(size_t length, const std::string& charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    std::string randomHexString(size_t length);
    std::string randomBase64String(size_t length);
    std::string randomUUID();
    std::string randomGUID();
    
    // Random bytes
    std::vector<uint8_t> randomBytes(size_t count);
    void randomBytes(uint8_t* buffer, size_t count);
    
    // Random permutations
    std::vector<size_t> randomPermutation(size_t size);
    std::vector<size_t> randomPermutation(size_t size, size_t count);
    
    // Random walks
    std::vector<int32_t> randomWalk1D(size_t steps, int32_t stepSize = 1);
    std::vector<std::pair<int32_t, int32_t>> randomWalk2D(size_t steps, int32_t stepSize = 1);
    std::vector<std::tuple<int32_t, int32_t, int32_t>> randomWalk3D(size_t steps, int32_t stepSize = 1);
    
    // Random matrices
    std::vector<std::vector<float32>> randomMatrix(size_t rows, size_t cols, float32 min = 0.0f, float32 max = 1.0f);
    std::vector<std::vector<double>> randomMatrix(size_t rows, size_t cols, double min = 0.0, double max = 1.0);
    
    // Random graphs
    std::vector<std::vector<bool>> randomGraph(size_t vertices, float32 edgeProbability);
    std::vector<std::vector<bool>> randomDAG(size_t vertices, float32 edgeProbability);
    std::vector<std::vector<bool>> randomTree(size_t vertices);
    std::vector<std::vector<bool>> randomCompleteGraph(size_t vertices);
    
    // Random testing utilities
    bool testRandomness(const std::vector<uint32_t>& data);
    bool testUniformity(const std::vector<float32>& data);
    bool testNormality(const std::vector<float32>& data);
    float32 calculateEntropy(const std::vector<uint32_t>& data);
    float32 calculateKolmogorovSmirnov(const std::vector<float32>& data);
    
    // Random number quality tests
    struct RandomQualityReport {
        bool passesChiSquared;
        bool passesKolmogorovSmirnov;
        bool passesRunsTest;
        bool passesSerialCorrelation;
        float32 chiSquaredPValue;
        float32 ksPValue;
        float32 runsPValue;
        float32 serialCorrelationPValue;
        float32 entropy;
        float32 minEntropy;
        float32 maxEntropy;
    };
    
    RandomQualityReport testQuality(const std::vector<uint32_t>& data);
    RandomQualityReport testQuality(const std::vector<float32>& data);
    
    // Random number generator comparison
    struct GeneratorComparison {
        std::string name;
        RandomType type;
        float32 speed; // numbers per second
        float32 quality; // 0.0 to 1.0
        uint64_t stateSize;
        bool supportsJump;
        bool supportsDiscard;
        bool supportsParallel;
    };
    
    std::vector<GeneratorComparison> compareGenerators();
    GeneratorComparison getGeneratorInfo(RandomType type);
}

// Template implementations
template<typename T>
void RandomUtils::shuffle(std::vector<T>& vector) {
    RandomGenerator& rng = getDefaultGenerator();
    for (size_t i = vector.size() - 1; i > 0; --i) {
        size_t j = static_cast<size_t>(rng.nextUint64() % (i + 1));
        std::swap(vector[i], vector[j]);
    }
}

template<typename T>
std::vector<T> RandomUtils::sample(const std::vector<T>& population, size_t count, bool withReplacement) {
    if (withReplacement) {
        std::vector<T> result;
        result.reserve(count);
        RandomGenerator& rng = getDefaultGenerator();
        for (size_t i = 0; i < count; ++i) {
            size_t index = static_cast<size_t>(rng.nextUint64() % population.size());
            result.push_back(population[index]);
        }
        return result;
    } else {
        if (count > population.size()) {
            count = population.size();
        }
        std::vector<T> result = population;
        shuffle(result);
        result.resize(count);
        return result;
    }
}

template<typename T>
T RandomUtils::sample(const std::vector<T>& population) {
    RandomGenerator& rng = getDefaultGenerator();
    size_t index = static_cast<size_t>(rng.nextUint64() % population.size());
    return population[index];
}

template<typename T>
std::vector<T> RandomUtils::weightedSample(const std::vector<T>& population, const std::vector<float32>& weights, size_t count) {
    std::vector<T> result;
    result.reserve(count);
    
    // Create cumulative weights
    std::vector<float32> cumulativeWeights(weights.size());
    cumulativeWeights[0] = weights[0];
    for (size_t i = 1; i < weights.size(); ++i) {
        cumulativeWeights[i] = cumulativeWeights[i - 1] + weights[i];
    }
    
    RandomGenerator& rng = getDefaultGenerator();
    float32 totalWeight = cumulativeWeights.back();
    
    for (size_t i = 0; i < count; ++i) {
        float32 randomValue = rng.nextFloat32(0.0f, totalWeight);
        auto it = std::lower_bound(cumulativeWeights.begin(), cumulativeWeights.end(), randomValue);
        size_t index = std::distance(cumulativeWeights.begin(), it);
        result.push_back(population[index]);
    }
    
    return result;
}

template<typename T>
T RandomUtils::weightedSample(const std::vector<T>& population, const std::vector<float32>& weights) {
    auto samples = weightedSample(population, weights, 1);
    return samples[0];
}

} // namespace RFUtils