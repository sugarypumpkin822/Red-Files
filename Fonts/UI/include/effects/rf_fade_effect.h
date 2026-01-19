#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFEffects {

// Fade effect types
enum class FadeType {
    FADE_IN,
    FADE_OUT,
    FADE_TO_COLOR,
    FADE_FROM_COLOR,
    CROSS_FADE,
    CUSTOM
};

// Fade easing functions
enum class FadeEasing {
    LINEAR,
    EASE_IN,
    EASE_OUT,
    EASE_IN_OUT,
    CUBIC_IN,
    CUBIC_OUT,
    CUBIC_IN_OUT,
    QUARTIC_IN,
    QUARTIC_OUT,
    QUARTIC_IN_OUT,
    SINE_IN,
    SINE_OUT,
    SINE_IN_OUT,
    EXPONENTIAL_IN,
    EXPONENTIAL_OUT,
    EXPONENTIAL_IN_OUT,
    CIRCULAR_IN,
    CIRCULAR_OUT,
    CIRCULAR_IN_OUT,
    ELASTIC_IN,
    ELASTIC_OUT,
    ELASTIC_IN_OUT,
    BOUNCE_IN,
    BOUNCE_OUT,
    BOUNCE_IN_OUT,
    CUSTOM
};

// Fade quality levels
enum class FadeQuality {
    LOW,
    MEDIUM,
    HIGH,
    ULTRA,
    CUSTOM
};

// Fade color modes
enum class FadeColorMode {
    SOLID,
    GRADIENT,
    MULTI_COLOR,
    CUSTOM
};

// Fade properties
struct FadeProperties {
    FadeType type;
    FadeEasing easing;
    FadeQuality quality;
    FadeColorMode colorMode;
    float32 duration;
    float32 delay;
    float32 startOpacity;
    float32 endOpacity;
    float32 currentOpacity;
    float32 progress;
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
    std::array<float32, 4> startColor;
    std::array<float32, 4> endColor;
    std::array<float32, 4> currentColor;
    std::vector<std::array<float32, 4>> gradientColors;
    
    FadeProperties() : type(FadeType::FADE_IN), easing(FadeEasing::LINEAR), quality(FadeQuality::MEDIUM), 
                      colorMode(FadeColorMode::SOLID), duration(1.0f), delay(0.0f), startOpacity(0.0f), 
                      endOpacity(1.0f), currentOpacity(0.0f), progress(0.0f), enableAntialiasing(true), 
                      enableDithering(false), enableHDR(false), enableAnimation(true), enableLooping(false), 
                      enablePingPong(false), enableReverse(false), enableAutoReverse(false), 
                      sampleCount(8), passCount(1), animationSpeed(1.0f), 
                      startColor{0.0f, 0.0f, 0.0f, 0.0f}, endColor{1.0f, 1.0f, 1.0f, 1.0f}, 
                      currentColor{0.0f, 0.0f, 0.0f, 0.0f} {}
};

// Fade animation state
struct FadeAnimationState {
    float32 time;
    float32 progress;
    float32 opacity;
    std::array<float32, 4> color;
    bool isActive;
    bool isPlaying;
    bool isPaused;
    bool isLooping;
    bool isPingPong;
    bool isReversed;
    bool isAutoReversing;
    uint32_t loopCount;
    uint32_t currentLoop;
    
    FadeAnimationState() : time(0.0f), progress(0.0f), opacity(0.0f), color{1.0f, 1.0f, 1.0f, 1.0f}, 
                         isActive(false), isPlaying(false), isPaused(false), isLooping(false), 
                         isPingPong(false), isReversed(false), isAutoReversing(false), 
                         loopCount(0), currentLoop(0) {}
};

// Fade statistics
struct FadeStatistics {
    uint32_t totalRenders;
    uint32_t successfulRenders;
    uint32_t failedRenders;
    float32 averageRenderTime;
    float32 averageOpacity;
    float32 averageProgress;
    uint64_t totalRenderTime;
    uint32_t sampleCount;
    uint32_t passCount;
    float32 cacheHitRatio;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    uint32_t totalAnimations;
    uint32_t completedAnimations;
    uint32_t loopedAnimations;
    
    FadeStatistics() : totalRenders(0), successfulRenders(0), failedRenders(0), averageRenderTime(0.0f), 
                       averageOpacity(0.0f), averageProgress(0.0f), totalRenderTime(0), sampleCount(0), 
                       passCount(0), cacheHitRatio(0.0f), cacheHits(0), cacheMisses(0), 
                       totalAnimations(0), completedAnimations(0), loopedAnimations(0) {}
};

// Fade effect
class FadeEffect {
public:
    FadeEffect();
    virtual ~FadeEffect() = default;
    
    // Effect management
    void initialize(const FadeProperties& properties);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Properties
    void setProperties(const FadeProperties& properties);
    const FadeProperties& getProperties() const;
    void setType(FadeType type);
    void setEasing(FadeEasing easing);
    void setQuality(FadeQuality quality);
    void setColorMode(FadeColorMode colorMode);
    void setDuration(float32 duration);
    void setDelay(float32 delay);
    void setStartOpacity(float32 startOpacity);
    void setEndOpacity(float32 endOpacity);
    void setCurrentOpacity(float32 currentOpacity);
    void setProgress(float32 progress);
    
    // Color management
    void setStartColor(const std::array<float32, 4>& color);
    void setEndColor(const std::array<float32, 4>& color);
    void setCurrentColor(const std::array<float32, 4>& color);
    void setGradientColors(const std::vector<std::array<float32, 4>>& colors);
    const std::array<float32, 4>& getStartColor() const;
    const std::array<float32, 4>& getEndColor() const;
    const std::array<float32, 4>& getCurrentColor() const;
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
                std::vector<uint8>& outputImage, const FadeAnimationState& animationState) const;
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                uint32_t channels, std::vector<uint8>& outputImage) const;
    
    // Cross-fade rendering
    bool renderCrossFade(const std::vector<uint8>& sourceImage, const std::vector<uint8>& targetImage, 
                        uint32_t width, uint32_t height, std::vector<uint8>& outputImage) const;
    bool renderCrossFade(const std::vector<uint8>& sourceImage, const std::vector<uint8>& targetImage, 
                        uint32_t width, uint32_t height, std::vector<uint8>& outputImage, 
                        const FadeAnimationState& animationState) const;
    
    // Batch rendering
    bool renderBatch(const std::vector<std::vector<uint8>>& sourceImages, 
                    const std::vector<std::pair<uint32_t, uint32_t>>& dimensions, 
                    std::vector<std::vector<uint8>>& outputImages) const;
    
    // Quality settings
    void enableAntialiasing(bool enabled);
    void enableDithering(bool enabled);
    void enableHDR(bool enabled);
    
    // Validation
    bool validateProperties(const FadeProperties& properties) const;
    bool validateImage(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    bool validateDimensions(uint32_t width, uint32_t height) const;
    
    // Animation state
    FadeAnimationState getAnimationState() const;
    void updateAnimationState(float32 deltaTime);
    void setAnimationState(const FadeAnimationState& state);
    
    // Easing functions
    float32 calculateEasing(float32 progress, FadeEasing easing) const;
    float32 calculateLinearEasing(float32 progress) const;
    float32 calculateEaseInEasing(float32 progress) const;
    float32 calculateEaseOutEasing(float32 progress) const;
    float32 calculateEaseInOutEasing(float32 progress) const;
    float32 calculateCubicInEasing(float32 progress) const;
    float32 calculateCubicOutEasing(float32 progress) const;
    float32 calculateCubicInOutEasing(float32 progress) const;
    float32 calculateSineInEasing(float32 progress) const;
    float32 calculateSineOutEasing(float32 progress) const;
    float32 calculateSineInOutEasing(float32 progress) const;
    float32 calculateExponentialInEasing(float32 progress) const;
    float32 calculateExponentialOutEasing(float32 progress) const;
    float32 calculateExponentialInOutEasing(float32 progress) const;
    float32 calculateCircularInEasing(float32 progress) const;
    float32 calculateCircularOutEasing(float32 progress) const;
    float32 calculateCircularInOutEasing(float32 progress) const;
    float32 calculateElasticInEasing(float32 progress) const;
    float32 calculateElasticOutEasing(float32 progress) const;
    float32 calculateElasticInOutEasing(float32 progress) const;
    float32 calculateBounceInEasing(float32 progress) const;
    float32 calculateBounceOutEasing(float32 progress) const;
    float32 calculateBounceInOutEasing(float32 progress) const;
    
    // Performance
    void enableCache(bool enabled);
    bool isCacheEnabled() const;
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    void clearCache();
    
    // Statistics
    FadeStatistics getStatistics() const;
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
    void addFadeEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void removeFadeEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void clearFadeEffectEventListeners();
    
    // Utility methods
    void cloneFrom(const FadeEffect& other);
    virtual std::unique_ptr<FadeEffect> clone() const;
    bool equals(const FadeEffect& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    FadeProperties properties_;
    FadeAnimationState animationState_;
    FadeStatistics statistics_;
    
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
    virtual void triggerFadeEffectEvent(const std::string& eventType);
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage) const;
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage, const FadeAnimationState& animationState) const;
    
    // Rendering helpers
    virtual bool renderFadeIn(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                             std::vector<uint8>& outputImage) const;
    virtual bool renderFadeOut(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                              std::vector<uint8>& outputImage) const;
    virtual bool renderFadeToColor(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                   std::vector<uint8>& outputImage) const;
    virtual bool renderFadeFromColor(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                   std::vector<uint8>& outputImage) const;
    virtual bool renderCrossFadeInternal(const std::vector<uint8>& sourceImage, const std::vector<uint8>& targetImage, 
                                       uint32_t width, uint32_t height, std::vector<uint8>& outputImage) const;
    
    // Color helpers
    virtual std::array<float32, 4> calculateFadeColor(const std::array<float32, 4>& startColor, 
                                                     const std::array<float32, 4>& endColor, float32 progress) const;
    virtual std::array<float32, 4> interpolateColor(const std::array<float32, 4>& color1, 
                                                   const std::array<float32, 4>& color2, float32 t) const;
    virtual std::array<float32, 4> calculateGradientColor(float32 progress) const;
    virtual std::array<float32, 4> calculateMultiColor(float32 progress) const;
    
    // Animation helpers
    virtual void updateAnimationStateInternal(float32 deltaTime);
    virtual float32 calculateProgress(float32 time, float32 duration, float32 delay) const;
    virtual float32 calculateOpacity(float32 progress, float32 startOpacity, float32 endOpacity, FadeEasing easing) const;
    virtual std::array<float32, 4> calculateAnimatedColor(const FadeAnimationState& animationState) const;
    virtual void handleLooping(FadeAnimationState& state) const;
    virtual void handlePingPong(FadeAnimationState& state) const;
    virtual void handleReverse(FadeAnimationState& state) const;
    virtual void handleAutoReverse(FadeAnimationState& state) const;
    
    // Quality helpers
    virtual uint32_t calculateSampleCount(FadeQuality quality) const;
    virtual uint32_t calculatePassCount(FadeQuality quality) const;
    virtual float32 calculateFadeQuality(FadeQuality quality) const;
    
    // Validation helpers
    virtual bool validatePropertiesInternal(const FadeProperties& properties) const;
    virtual bool validateImageInternal(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    virtual bool validateDimensionsInternal(uint32_t width, uint32_t height) const;
    
    // Cache helpers
    virtual std::string generateCacheKey(const std::vector<uint8>& image, uint32_t width, uint32_t height, 
                                        const FadeAnimationState& animationState) const;
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
    virtual void logAnimationOperation(const std::string& operation, const FadeAnimationState& state);
    virtual std::string formatProperties(const FadeProperties& properties) const;
    virtual std::string formatAnimationState(const FadeAnimationState& state) const;
};

// Specialized fade effects
class FadeInEffect : public FadeEffect {
public:
    FadeInEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<FadeEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class FadeOutEffect : public FadeEffect {
public:
    FadeOutEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<FadeEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class FadeToColorEffect : public FadeEffect {
public:
    FadeToColorEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<FadeEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class FadeFromColorEffect : public FadeEffect {
public:
    FadeFromColorEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<FadeEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class CrossFadeEffect : public FadeEffect {
public:
    CrossFadeEffect();
    bool renderCrossFade(const std::vector<uint8>& sourceImage, const std::vector<uint8>& targetImage, 
                        uint32_t width, uint32_t height, std::vector<uint8>& outputImage) const override;
    std::unique_ptr<FadeEffect> clone() const override;
    
protected:
    bool renderCrossFadeInternal(const std::vector<uint8>& sourceImage, const std::vector<uint8>& targetImage, 
                                uint32_t width, uint32_t height, std::vector<uint8>& outputImage) const override;
};

class AnimatedFadeEffect : public FadeEffect {
public:
    AnimatedFadeEffect();
    void enableAnimation(bool enabled) override;
    void startAnimation() override;
    void stopAnimation() override;
    void updateAnimationState(float32 deltaTime) override;
    std::unique_ptr<FadeEffect> clone() const override;
    
protected:
    void updateAnimationStateInternal(float32 deltaTime) override;
};

class GradientFadeEffect : public FadeEffect {
public:
    GradientFadeEffect();
    void setGradientColors(const std::vector<std::array<float32, 4>>& colors);
    std::vector<std::array<float32, 4>> getGradientColors() const;
    std::unique_ptr<FadeEffect> clone() const override;
    
protected:
    std::array<float32, 4> calculateGradientColor(float32 progress) const override;
    
private:
    std::vector<std::array<float32, 4>> gradientColors_;
};

// Fade effect factory
class FadeEffectFactory {
public:
    static std::unique_ptr<FadeEffect> createFadeEffect(FadeType type);
    static std::unique_ptr<FadeInEffect> createFadeInEffect();
    static std::unique_ptr<FadeOutEffect> createFadeOutEffect();
    static std::unique_ptr<FadeToColorEffect> createFadeToColorEffect();
    static std::unique_ptr<FadeFromColorEffect> createFadeFromColorEffect();
    static std::unique_ptr<CrossFadeEffect> createCrossFadeEffect();
    static std::unique_ptr<AnimatedFadeEffect> createAnimatedFadeEffect();
    static std::unique_ptr<GradientFadeEffect> createGradientFadeEffect();
    static FadeProperties createDefaultProperties(FadeType type);
    static std::vector<FadeType> getSupportedFadeTypes();
    static std::vector<FadeEasing> getSupportedEasingFunctions();
    static std::vector<FadeQuality> getSupportedQualityLevels();
    static std::vector<FadeColorMode> getSupportedColorModes();
    static FadeType detectBestType(const std::vector<uint8>& image, uint32_t width, uint32_t height);
};

} // namespace RFEffects