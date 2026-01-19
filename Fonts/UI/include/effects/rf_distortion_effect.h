#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFEffects {

// Distortion effect types
enum class DistortionType {
    WAVE,
    RIPPLE,
    NOISE,
    PIXELATE,
    BLUR,
    CUSTOM
};

// Distortion modes
enum class DistortionMode {
    HORIZONTAL,
    VERTICAL,
    RADIAL,
    ANGULAR,
    CUSTOM
};

// Distortion quality levels
enum class DistortionQuality {
    LOW,
    MEDIUM,
    HIGH,
    ULTRA,
    CUSTOM
};

// Distortion color modes
enum class DistortionColorMode {
    SOLID,
    GRADIENT,
    MULTI_COLOR,
    CUSTOM
};

// Distortion properties
struct DistortionProperties {
    DistortionType type;
    DistortionMode mode;
    DistortionQuality quality;
    DistortionColorMode colorMode;
    float32 amplitude;
    float32 frequency;
    float32 phase;
    float32 wavelength;
    float32 speed;
    float32 intensity;
    float32 radius;
    float32 angle;
    float32 pixelSize;
    float32 blurRadius;
    float32 noiseScale;
    float32 noiseStrength;
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
    std::array<float32, 4> primaryColor;
    std::array<float32, 4> secondaryColor;
    std::array<float32, 4> backgroundColor;
    std::vector<std::array<float32, 4>> gradientColors;
    
    DistortionProperties() : type(DistortionType::WAVE), mode(DistortionMode::HORIZONTAL), quality(DistortionQuality::MEDIUM), 
                           colorMode(DistortionColorMode::SOLID), amplitude(10.0f), frequency(1.0f), phase(0.0f), 
                           wavelength(50.0f), speed(1.0f), intensity(1.0f), radius(50.0f), angle(0.0f), 
                           pixelSize(4.0f), blurRadius(5.0f), noiseScale(0.1f), noiseStrength(1.0f), 
                           enableAntialiasing(true), enableDithering(false), enableHDR(false), enableAnimation(true), 
                           enableLooping(false), enablePingPong(false), enableReverse(false), enableAutoReverse(false), 
                           sampleCount(8), passCount(1), animationSpeed(1.0f), primaryColor{1.0f, 1.0f, 1.0f, 1.0f}, 
                           secondaryColor{0.5f, 0.5f, 0.5f, 1.0f}, backgroundColor{0.0f, 0.0f, 0.0f, 0.0f} {}
};

// Distortion animation state
struct DistortionAnimationState {
    float32 time;
    float32 currentAmplitude;
    float32 currentFrequency;
    float32 currentPhase;
    float32 currentWavelength;
    float32 currentSpeed;
    float32 currentIntensity;
    float32 currentRadius;
    float32 currentAngle;
    float32 currentPixelSize;
    float32 currentBlurRadius;
    float32 currentNoiseScale;
    float32 currentNoiseStrength;
    std::array<float32, 4> currentColor;
    bool isActive;
    bool isPlaying;
    bool isPaused;
    bool isLooping;
    bool isPingPong;
    bool isReversed;
    bool isAutoReversing;
    uint32_t loopCount;
    uint32_t currentLoop;
    
    DistortionAnimationState() : time(0.0f), currentAmplitude(10.0f), currentFrequency(1.0f), currentPhase(0.0f), 
                              currentWavelength(50.0f), currentSpeed(1.0f), currentIntensity(1.0f), currentRadius(50.0f), 
                              currentAngle(0.0f), currentPixelSize(4.0f), currentBlurRadius(5.0f), currentNoiseScale(0.1f), 
                              currentNoiseStrength(1.0f), currentColor{1.0f, 1.0f, 1.0f, 1.0f}, isActive(false), 
                              isPlaying(false), isPaused(false), isLooping(false), isPingPong(false), 
                              isReversed(false), isAutoReversing(false), loopCount(0), currentLoop(0) {}
};

// Distortion statistics
struct DistortionStatistics {
    uint32_t totalRenders;
    uint32_t successfulRenders;
    uint32_t failedRenders;
    float32 averageRenderTime;
    float32 averageAmplitude;
    float32 averageFrequency;
    float32 averageIntensity;
    float32 averageRadius;
    uint64_t totalRenderTime;
    uint32_t sampleCount;
    uint32_t passCount;
    float32 cacheHitRatio;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    uint32_t totalAnimations;
    uint32_t completedAnimations;
    uint32_t loopedAnimations;
    
    DistortionStatistics() : totalRenders(0), successfulRenders(0), failedRenders(0), averageRenderTime(0.0f), 
                             averageAmplitude(0.0f), averageFrequency(0.0f), averageIntensity(0.0f), averageRadius(0.0f), 
                             totalRenderTime(0), sampleCount(0), passCount(0), cacheHitRatio(0.0f), cacheHits(0), 
                             cacheMisses(0), totalAnimations(0), completedAnimations(0), loopedAnimations(0) {}
};

// Distortion effect
class DistortionEffect {
public:
    DistortionEffect();
    virtual ~DistortionEffect() = default;
    
    // Effect management
    void initialize(const DistortionProperties& properties);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Properties
    void setProperties(const DistortionProperties& properties);
    const DistortionProperties& getProperties() const;
    void setType(DistortionType type);
    void setMode(DistortionMode mode);
    void setQuality(DistortionQuality quality);
    void setColorMode(DistortionColorMode colorMode);
    void setAmplitude(float32 amplitude);
    void setFrequency(float32 frequency);
    void setPhase(float32 phase);
    void setWavelength(float32 wavelength);
    void setSpeed(float32 speed);
    void setIntensity(float32 intensity);
    void setRadius(float32 radius);
    void setAngle(float32 angle);
    void setPixelSize(float32 pixelSize);
    void setBlurRadius(float32 blurRadius);
    void setNoiseScale(float32 noiseScale);
    void setNoiseStrength(float32 noiseStrength);
    
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
                std::vector<uint8>& outputImage, const DistortionAnimationState& animationState) const;
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
    bool validateProperties(const DistortionProperties& properties) const;
    bool validateImage(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    bool validateDimensions(uint32_t width, uint32_t height) const;
    
    // Animation state
    DistortionAnimationState getAnimationState() const;
    void updateAnimationState(float32 deltaTime);
    void setAnimationState(const DistortionAnimationState& state);
    
    // Performance
    void enableCache(bool enabled);
    bool isCacheEnabled() const;
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    void clearCache();
    
    // Statistics
    DistortionStatistics getStatistics() const;
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
    void addDistortionEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void removeDistortionEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void clearDistortionEffectEventListeners();
    
    // Utility methods
    void cloneFrom(const DistortionEffect& other);
    virtual std::unique_ptr<DistortionEffect> clone() const;
    bool equals(const DistortionEffect& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    DistortionProperties properties_;
    DistortionAnimationState animationState_;
    DistortionStatistics statistics_;
    
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
    virtual void triggerDistortionEffectEvent(const std::string& eventType);
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage) const;
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage, const DistortionAnimationState& animationState) const;
    
    // Rendering helpers
    virtual bool renderWaveDistortion(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                      std::vector<uint8>& outputImage) const;
    virtual bool renderRippleDistortion(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                        std::vector<uint8>& outputImage) const;
    virtual bool renderNoiseDistortion(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                       std::vector<uint8>& outputImage) const;
    virtual bool renderPixelateDistortion(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                          std::vector<uint8>& outputImage) const;
    virtual bool renderBlurDistortion(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                     std::vector<uint8>& outputImage) const;
    
    // Color helpers
    virtual std::array<float32, 4> calculateDistortionColor(const std::array<float32, 4>& baseColor, 
                                                           const DistortionAnimationState& animationState) const;
    virtual std::array<float32, 4> interpolateColor(const std::array<float32, 4>& color1, 
                                                   const std::array<float32, 4>& color2, float32 t) const;
    virtual std::array<float32, 4> calculateGradientColor(float32 progress) const;
    virtual std::array<float32, 4> calculateMultiColor(float32 phase) const;
    
    // Animation helpers
    virtual void updateAnimationStateInternal(float32 deltaTime);
    virtual float32 calculateAmplitude(float32 time, float32 duration, float32 delay, float32 amplitude) const;
    virtual float32 calculateFrequency(float32 time, float32 duration, float32 delay, float32 frequency) const;
    virtual float32 calculatePhase(float32 time, float32 duration, float32 delay, float32 phase, float32 speed) const;
    virtual float32 calculateWavelength(float32 time, float32 duration, float32 delay, float32 wavelength) const;
    virtual float32 calculateIntensity(float32 time, float32 duration, float32 delay, float32 intensity) const;
    virtual float32 calculateRadius(float32 time, float32 duration, float32 delay, float32 radius) const;
    virtual float32 calculateAngle(float32 time, float32 duration, float32 delay, float32 angle) const;
    virtual float32 calculatePixelSize(float32 time, float32 duration, float32 delay, float32 pixelSize) const;
    virtual float32 calculateBlurRadius(float32 time, float32 duration, float32 delay, float32 blurRadius) const;
    virtual float32 calculateNoiseScale(float32 time, float32 duration, float32 delay, float32 noiseScale) const;
    virtual float32 calculateNoiseStrength(float32 time, float32 duration, float32 delay, float32 noiseStrength) const;
    virtual std::array<float32, 4> calculateAnimatedColor(const DistortionAnimationState& animationState) const;
    virtual void handleLooping(DistortionAnimationState& state) const;
    virtual void handlePingPong(DistortionAnimationState& state) const;
    virtual void handleReverse(DistortionAnimationState& state) const;
    virtual void handleAutoReverse(DistortionAnimationState& state) const;
    
    // Quality helpers
    virtual uint32_t calculateSampleCount(DistortionQuality quality) const;
    virtual uint32_t calculatePassCount(DistortionQuality quality) const;
    virtual float32 calculateDistortionQuality(DistortionQuality quality) const;
    
    // Validation helpers
    virtual bool validatePropertiesInternal(const DistortionProperties& properties) const;
    virtual bool validateImageInternal(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    virtual bool validateDimensionsInternal(uint32_t width, uint32_t height) const;
    
    // Cache helpers
    virtual std::string generateCacheKey(const std::vector<uint8>& image, uint32_t width, uint32_t height, 
                                        const DistortionAnimationState& animationState) const;
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
    virtual void logAnimationOperation(const std::string& operation, const DistortionAnimationState& state);
    virtual std::string formatProperties(const DistortionProperties& properties) const;
    virtual std::string formatAnimationState(const DistortionAnimationState& state) const;
};

// Specialized distortion effects
class WaveDistortionEffect : public DistortionEffect {
public:
    WaveDistortionEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<DistortionEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class RippleDistortionEffect : public DistortionEffect {
public:
    RippleDistortionEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<DistortionEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class NoiseDistortionEffect : public DistortionEffect {
public:
    NoiseDistortionEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<DistortionEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class PixelateDistortionEffect : public DistortionEffect {
public:
    PixelateDistortionEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<DistortionEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class BlurDistortionEffect : public DistortionEffect {
public:
    BlurDistortionEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<DistortionEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class AnimatedDistortionEffect : public DistortionEffect {
public:
    AnimatedDistortionEffect();
    void enableAnimation(bool enabled) override;
    void startAnimation() override;
    void stopAnimation() override;
    void updateAnimationState(float32 deltaTime) override;
    std::unique_ptr<DistortionEffect> clone() const override;
    
protected:
    void updateAnimationStateInternal(float32 deltaTime) override;
};

class GradientDistortionEffect : public DistortionEffect {
public:
    GradientDistortionEffect();
    void setGradientColors(const std::vector<std::array<float32, 4>>& colors);
    std::vector<std::array<float32, 4>> getGradientColors() const;
    std::unique_ptr<DistortionEffect> clone() const override;
    
protected:
    std::array<float32, 4> calculateGradientColor(float32 progress) const override;
    
private:
    std::vector<std::array<float32, 4>> gradientColors_;
};

// Distortion effect factory
class DistortionEffectFactory {
public:
    static std::unique_ptr<DistortionEffect> createDistortionEffect(DistortionType type);
    static std::unique_ptr<WaveDistortionEffect> createWaveDistortionEffect();
    static std::unique_ptr<RippleDistortionEffect> createRippleDistortionEffect();
    static std::unique_ptr<NoiseDistortionEffect> createNoiseDistortionEffect();
    static std::unique_ptr<PixelateDistortionEffect> createPixelateDistortionEffect();
    static std::unique_ptr<BlurDistortionEffect> createBlurDistortionEffect();
    static std::unique_ptr<AnimatedDistortionEffect> createAnimatedDistortionEffect();
    static std::unique_ptr<GradientDistortionEffect> createGradientDistortionEffect();
    static DistortionProperties createDefaultProperties(DistortionType type);
    static std::vector<DistortionType> getSupportedDistortionTypes();
    static std::vector<DistortionMode> getSupportedModes();
    static std::vector<DistortionQuality> getSupportedQualityLevels();
    static std::vector<DistortionColorMode> getSupportedColorModes();
    static DistortionType detectBestType(const std::vector<uint8>& image, uint32_t width, uint32_t height);
};

} // namespace RFEffects
