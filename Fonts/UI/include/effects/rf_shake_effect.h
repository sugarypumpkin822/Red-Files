#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFEffects {

// Shake effect types
enum class ShakeType {
    POSITION_SHAKE,
    ROTATION_SHAKE,
    SCALE_SHAKE,
    COLOR_SHAKE,
    CUSTOM
};

// Shake patterns
enum class ShakePattern {
    RANDOM,
    PERLIN_NOISE,
    SINE_WAVE,
    CIRCULAR,
    ELLIPTICAL,
    CUSTOM
};

// Shake quality levels
enum class ShakeQuality {
    LOW,
    MEDIUM,
    HIGH,
    ULTRA,
    CUSTOM
};

// Shake color modes
enum class ShakeColorMode {
    SOLID,
    GRADIENT,
    MULTI_COLOR,
    CUSTOM
};

// Shake properties
struct ShakeProperties {
    ShakeType type;
    ShakePattern pattern;
    ShakeQuality quality;
    ShakeColorMode colorMode;
    float32 intensity;
    float32 frequency;
    float32 duration;
    float32 delay;
    float32 damping;
    float32 randomness;
    float32 minOffsetX;
    float32 maxOffsetX;
    float32 minOffsetY;
    float32 maxOffsetY;
    float32 minRotation;
    float32 maxRotation;
    float32 minScale;
    float32 maxScale;
    float32 minColorShift;
    float32 maxColorShift;
    bool enableAntialiasing;
    bool enableDithering;
    bool enableHDR;
    bool enableAnimation;
    bool enableLooping;
    bool enablePingPong;
    bool enableReverse;
    bool enableAutoReverse;
    uint32_t sampleCount;
    uint32_t passCount;
    float32 animationSpeed;
    uint32_t seed;
    std::array<float32, 4> primaryColor;
    std::array<float32, 4> secondaryColor;
    std::array<float32, 4> backgroundColor;
    std::vector<std::array<float32, 4>> gradientColors;
    
    ShakeProperties() : type(ShakeType::POSITION_SHAKE), pattern(ShakePattern::RANDOM), quality(ShakeQuality::MEDIUM), 
                       colorMode(ShakeColorMode::SOLID), intensity(1.0f), frequency(10.0f), duration(1.0f), 
                       delay(0.0f), damping(0.9f), randomness(0.5f), minOffsetX(-10.0f), maxOffsetX(10.0f), 
                       minOffsetY(-10.0f), maxOffsetY(10.0f), minRotation(-15.0f), maxRotation(15.0f), 
                       minScale(0.9f), maxScale(1.1f), minColorShift(-0.1f), maxColorShift(0.1f), 
                       enableAntialiasing(true), enableDithering(false), enableHDR(false), enableAnimation(true), 
                       enableLooping(false), enablePingPong(false), enableReverse(false), enableAutoReverse(false), 
                       sampleCount(8), passCount(1), animationSpeed(1.0f), seed(12345), 
                       primaryColor{1.0f, 1.0f, 1.0f, 1.0f}, secondaryColor{0.5f, 0.5f, 0.5f, 1.0f}, 
                       backgroundColor{0.0f, 0.0f, 0.0f, 0.0f} {}
};

// Shake animation state
struct ShakeAnimationState {
    float32 time;
    float32 currentOffsetX;
    float32 currentOffsetY;
    float32 currentRotation;
    float32 currentScale;
    std::array<float32, 4> currentColor;
    float32 dampingFactor;
    float32 noisePhase;
    bool isActive;
    bool isPlaying;
    bool isPaused;
    bool isLooping;
    bool isPingPong;
    bool isReversed;
    bool isAutoReversing;
    uint32_t loopCount;
    uint32_t currentLoop;
    
    ShakeAnimationState() : time(0.0f), currentOffsetX(0.0f), currentOffsetY(0.0f), currentRotation(0.0f), 
                          currentScale(1.0f), currentColor{1.0f, 1.0f, 1.0f, 1.0f}, dampingFactor(1.0f), 
                          noisePhase(0.0f), isActive(false), isPlaying(false), isPaused(false), isLooping(false), 
                          isPingPong(false), isReversed(false), isAutoReversing(false), loopCount(0), currentLoop(0) {}
};

// Shake statistics
struct ShakeStatistics {
    uint32_t totalRenders;
    uint32_t successfulRenders;
    uint32_t failedRenders;
    float32 averageRenderTime;
    float32 averageIntensity;
    float32 averageFrequency;
    float32 averageDuration;
    uint64_t totalRenderTime;
    uint32_t sampleCount;
    uint32_t passCount;
    float32 cacheHitRatio;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    uint32_t totalAnimations;
    uint32_t completedAnimations;
    uint32_t loopedAnimations;
    
    ShakeStatistics() : totalRenders(0), successfulRenders(0), failedRenders(0), averageRenderTime(0.0f), 
                        averageIntensity(0.0f), averageFrequency(0.0f), averageDuration(0.0f), 
                        totalRenderTime(0), sampleCount(0), passCount(0), cacheHitRatio(0.0f), cacheHits(0), 
                        cacheMisses(0), totalAnimations(0), completedAnimations(0), loopedAnimations(0) {}
};

// Shake effect
class ShakeEffect {
public:
    ShakeEffect();
    virtual ~ShakeEffect() = default;
    
    // Effect management
    void initialize(const ShakeProperties& properties);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Properties
    void setProperties(const ShakeProperties& properties);
    const ShakeProperties& getProperties() const;
    void setType(ShakeType type);
    void setPattern(ShakePattern pattern);
    void setQuality(ShakeQuality quality);
    void setColorMode(ShakeColorMode colorMode);
    void setIntensity(float32 intensity);
    void setFrequency(float32 frequency);
    void setDuration(float32 duration);
    void setDelay(float32 delay);
    void setDamping(float32 damping);
    void setRandomness(float32 randomness);
    void setMinOffsetX(float32 minOffsetX);
    void setMaxOffsetX(float32 maxOffsetX);
    void setMinOffsetY(float32 minOffsetY);
    void setMaxOffsetY(float32 maxOffsetY);
    void setMinRotation(float32 minRotation);
    void setMaxRotation(float32 maxRotation);
    void setMinScale(float32 minScale);
    void setMaxScale(float32 maxScale);
    void setMinColorShift(float32 minColorShift);
    void setMaxColorShift(float32 maxColorShift);
    void setSeed(uint32_t seed);
    
    // Color management
    void setPrimaryColor(const std::array<float32, 4>& color);
    void setSecondaryColor(const std::array<float32, 4>& color);
    void setBackgroundColor(const std::array<float32, 4>& color);
    void setGradientColors(const std::vector<std::array<float32, 4>>& colors);
    const std::array<float32, 4>& getPrimaryColor() const;
    const std::array<float32, 4>& getSecondaryColor() const;
    const std::array<float32, 4>& getBackgroundColor() const;
    const std::vector<std::array<float32, 4>>& getGradientColors() const;
    
    // Animation control
    void enableAnimation(bool enabled);
    void enableLooping(bool enabled);
    void enablePingPong(bool enabled);
    void enableReverse(bool enabled);
    void enableAutoReverse(bool enabled);
    void setAnimationSpeed(float32 speed);
    void setLoopCount(uint32_t loopCount);
    void startAnimation();
    void stopAnimation();
    void pauseAnimation();
    void resumeAnimation();
    void restartAnimation();
    bool isAnimationActive() const;
    bool isAnimationPlaying() const;
    bool isAnimationPaused() const;
    
    // Rendering
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const;
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage, const ShakeAnimationState& animationState) const;
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                uint32_t channels, std::vector<uint8>& outputImage) const;
    
    // Batch rendering
    bool renderBatch(const std::vector<std::vector<uint8>>& sourceImages, 
                    const std::vector<std::pair<uint32_t, uint32_t>>& dimensions, 
                    std::vector<std::vector<uint8>>& outputImages) const;
    
    // Quality settings
    void enableAntialiasing(bool enabled);
    void enableDithering(bool enabled);
    void enableHDR(bool enabled);
    
    // Validation
    bool validateProperties(const ShakeProperties& properties) const;
    bool validateImage(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    bool validateDimensions(uint32_t width, uint32_t height) const;
    
    // Animation state
    ShakeAnimationState getAnimationState() const;
    void updateAnimationState(float32 deltaTime);
    void setAnimationState(const ShakeAnimationState& state);
    
    // Pattern functions
    float32 calculatePattern(float32 time, ShakePattern pattern) const;
    float32 calculateRandomPattern(float32 time) const;
    float32 calculatePerlinNoisePattern(float32 time) const;
    float32 calculateSineWavePattern(float32 time) const;
    float32 calculateCircularPattern(float32 time) const;
    float32 calculateEllipticalPattern(float32 time) const;
    
    // Performance
    void enableCache(bool enabled);
    bool isCacheEnabled() const;
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    void clearCache();
    
    // Statistics
    ShakeStatistics getStatistics() const;
    void updateStatistics();
    void resetStatistics();
    void enableStatistics(bool enabled);
    bool isStatisticsEnabled() const;
    
    // Debugging
    void enableDebugging(bool enabled);
    bool isDebuggingEnabled() const;
    std::vector<std::string> getDebugMessages() const;
    void clearDebugMessages();
    void dumpEffect() const;
    std::string dumpEffectToString() const;
    
    // Event handling
    void addShakeEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void removeShakeEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void clearShakeEffectEventListeners();
    
    // Utility methods
    void cloneFrom(const ShakeEffect& other);
    virtual std::unique_ptr<ShakeEffect> clone() const;
    bool equals(const ShakeEffect& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    ShakeProperties properties_;
    ShakeAnimationState animationState_;
    ShakeStatistics statistics_;
    
    bool isInitialized_;
    bool statisticsEnabled_;
    bool debuggingEnabled_;
    bool cacheEnabled_;
    bool animationEnabled_;
    bool loopingEnabled_;
    bool pingPongEnabled_;
    bool reverseEnabled_;
    bool autoReverseEnabled_;
    
    size_t currentCacheSize_;
    std::vector<std::string> debugMessages_;
    
    // Protected helper methods
    virtual void triggerShakeEffectEvent(const std::string& eventType);
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage) const;
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage, const ShakeAnimationState& animationState) const;
    
    // Rendering helpers
    virtual bool renderPositionShake(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                      std::vector<uint8>& outputImage) const;
    virtual bool renderRotationShake(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                       std::vector<uint8>& outputImage) const;
    virtual bool renderScaleShake(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                   std::vector<uint8>& outputImage) const;
    virtual bool renderColorShake(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                  std::vector<uint8>& outputImage) const;
    
    // Color helpers
    virtual std::array<float32, 4> calculateShakeColor(const std::array<float32, 4>& baseColor, 
                                                       const ShakeAnimationState& animationState) const;
    virtual std::array<float32, 4> interpolateColor(const std::array<float32, 4>& color1, 
                                                   const std::array<float32, 4>& color2, float32 t) const;
    virtual std::array<float32, 4> calculateGradientColor(float32 progress) const;
    virtual std::array<float32, 4> calculateMultiColor(float32 phase) const;
    
    // Animation helpers
    virtual void updateAnimationStateInternal(float32 deltaTime);
    virtual float32 calculateDampingFactor(float32 time, float32 damping, float32 duration) const;
    virtual float32 calculateOffset(float32 pattern, float32 minOffset, float32 maxOffset, float32 intensity, float32 randomness) const;
    virtual float32 calculateRotation(float32 pattern, float32 minRotation, float32 maxRotation, float32 intensity, float32 randomness) const;
    virtual float32 calculateScale(float32 pattern, float32 minScale, float32 maxScale, float32 intensity, float32 randomness) const;
    virtual float32 calculateColorShift(float32 pattern, float32 minColorShift, float32 maxColorShift, float32 intensity, float32 randomness) const;
    virtual std::array<float32, 4> calculateAnimatedColor(const ShakeAnimationState& animationState) const;
    virtual void handleLooping(ShakeAnimationState& state) const;
    virtual void handlePingPong(ShakeAnimationState& state) const;
    virtual void handleReverse(ShakeAnimationState& state) const;
    virtual void handleAutoReverse(ShakeAnimationState& state) const;
    
    // Pattern helpers
    virtual float32 generateRandomValue(uint32_t seed) const;
    virtual float32 generatePerlinNoise(float32 x, float32 y, float32 z, uint32_t seed) const;
    virtual float32 interpolateNoise(float32 a, float32 b, float32 c, float32 d, float32 t) const;
    virtual float32 fade(float32 t) const;
    virtual float32 lerp(float32 a, float32 b, float32 t) const;
    
    // Quality helpers
    virtual uint32_t calculateSampleCount(ShakeQuality quality) const;
    virtual uint32_t calculatePassCount(ShakeQuality quality) const;
    virtual float32 calculateShakeQuality(ShakeQuality quality) const;
    
    // Validation helpers
    virtual bool validatePropertiesInternal(const ShakeProperties& properties) const;
    virtual bool validateImageInternal(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    virtual bool validateDimensionsInternal(uint32_t width, uint32_t height) const;
    
    // Cache helpers
    virtual std::string generateCacheKey(const std::vector<uint8>& image, uint32_t width, uint32_t height, 
                                        const ShakeAnimationState& animationState) const;
    virtual bool getFromCache(const std::string& key, std::vector<uint8>& outputImage) const;
    virtual void addToCache(const std::string& key, const std::vector<uint8>& outputImage);
    virtual void removeFromCache(const std::string& key);
    virtual void updateCache();
    virtual void cleanupCache();
    
    // Statistics helpers
    virtual void updateRenderStatistics(bool success, uint64_t renderTime);
    virtual void updateAnimationStatistics(bool completed, bool looped);
    virtual void updateCacheStatistics(bool hit);
    virtual void calculateStatistics();
    
    // Debug helpers
    virtual void addDebugMessage(const std::string& message);
    virtual void logRenderOperation(const std::string& operation, uint32_t width, uint32_t height);
    virtual void logAnimationOperation(const std::string& operation, const ShakeAnimationState& state);
    virtual std::string formatProperties(const ShakeProperties& properties) const;
    virtual std::string formatAnimationState(const ShakeAnimationState& state) const;
};

// Specialized shake effects
class PositionShakeEffect : public ShakeEffect {
public:
    PositionShakeEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<ShakeEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class RotationShakeEffect : public ShakeEffect {
public:
    RotationShakeEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<ShakeEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class ScaleShakeEffect : public ShakeEffect {
public:
    ScaleShakeEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<ShakeEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class ColorShakeEffect : public ShakeEffect {
public:
    ColorShakeEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<ShakeEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class AnimatedShakeEffect : public ShakeEffect {
public:
    AnimatedShakeEffect();
    void enableAnimation(bool enabled) override;
    void startAnimation() override;
    void stopAnimation() override;
    void updateAnimationState(float32 deltaTime) override;
    std::unique_ptr<ShakeEffect> clone() const override;
    
protected:
    void updateAnimationStateInternal(float32 deltaTime) override;
};

class GradientShakeEffect : public ShakeEffect {
public:
    GradientShakeEffect();
    void setGradientColors(const std::vector<std::array<float32, 4>>& colors);
    std::vector<std::array<float32, 4>> getGradientColors() const;
    std::unique_ptr<ShakeEffect> clone() const override;
    
protected:
    std::array<float32, 4> calculateGradientColor(float32 progress) const override;
    
private:
    std::vector<std::array<float32, 4>> gradientColors_;
};

// Shake effect factory
class ShakeEffectFactory {
public:
    static std::unique_ptr<ShakeEffect> createShakeEffect(ShakeType type);
    static std::unique_ptr<PositionShakeEffect> createPositionShakeEffect();
    static std::unique_ptr<RotationShakeEffect> createRotationShakeEffect();
    static std::unique_ptr<ScaleShakeEffect> createScaleShakeEffect();
    static std::unique_ptr<ColorShakeEffect> createColorShakeEffect();
    static std::unique_ptr<AnimatedShakeEffect> createAnimatedShakeEffect();
    static std::unique_ptr<GradientShakeEffect> createGradientShakeEffect();
    static ShakeProperties createDefaultProperties(ShakeType type);
    static std::vector<ShakeType> getSupportedShakeTypes();
    static std::vector<ShakePattern> getSupportedPatterns();
    static std::vector<ShakeQuality> getSupportedQualityLevels();
    static std::vector<ShakeColorMode> getSupportedColorModes();
    static ShakeType detectBestType(const std::vector<uint8>& image, uint32_t width, uint32_t height);
};

} // namespace RFEffects