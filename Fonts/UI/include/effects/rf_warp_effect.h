#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFEffects {

// Warp effect types
enum class WarpType {
    BARREL,
    PINCUSHION,
    SWIRL,
    TWIST,
    FISHEYEYE,
    CUSTOM
};

// Warp distortion modes
enum class WarpDistortionMode {
    LINEAR,
    RADIAL,
    ANGULAR,
    CUSTOM
};

// Warp quality levels
enum class WarpQuality {
    LOW,
    MEDIUM,
    HIGH,
    ULTRA,
    CUSTOM
};

// Warp color modes
enum class WarpColorMode {
    SOLID,
    GRADIENT,
    MULTI_COLOR,
    CUSTOM
};

// Warp properties
struct WarpProperties {
    WarpType type;
    WarpDistortionMode distortionMode;
    WarpQuality quality;
    WarpColorMode colorMode;
    float32 intensity;
    float32 radius;
    float32 angle;
    float32 frequency;
    float32 phase;
    float32 duration;
    float32 delay;
    float32 centerX;
    float32 centerY;
    float32 strength;
    float32 softness;
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
    
    WarpProperties() : type(WarpType::BARREL), distortionMode(WarpDistortionMode::LINEAR), quality(WarpQuality::MEDIUM), 
                       colorMode(WarpColorMode::SOLID), intensity(0.5f), radius(50.0f), angle(0.0f), frequency(1.0f), 
                       phase(0.0f), duration(1.0f), delay(0.0f), centerX(0.5f), centerY(0.5f), 
                       strength(1.0f), softness(1.0f), enableAntialiasing(true), enableDithering(false), 
                       enableHDR(false), enableAnimation(true), enableLooping(false), enablePingPong(false), 
                       enableReverse(false), enableAutoReverse(false), sampleCount(8), passCount(1), 
                       animationSpeed(1.0f), primaryColor{1.0f, 1.0f, 1.0f, 1.0f}, 
                       secondaryColor{0.5f, 0.5f, 0.5f, 1.0f}, backgroundColor{0.0f, 0.0f, 0.0f, 0.0f} {}
};

// Warp animation state
struct WarpAnimationState {
    float32 time;
    float32 currentIntensity;
    float32 currentRadius;
    float32 currentAngle;
    float32 currentFrequency;
    float32 currentPhase;
    float32 currentCenterX;
    float32 currentCenterY;
    float32 currentStrength;
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
    
    WarpAnimationState() : time(0.0f), currentIntensity(0.5f), currentRadius(50.0f), currentAngle(0.0f), 
                          currentFrequency(1.0f), currentPhase(0.0f), currentCenterX(0.5f), currentCenterY(0.5f), 
                          currentStrength(1.0f), currentColor{1.0f, 1.0f, 1.0f, 1.0f}, isActive(false), 
                          isPlaying(false), isPaused(false), isLooping(false), isPingPong(false), 
                          isReversed(false), isAutoReversing(false), loopCount(0), currentLoop(0) {}
};

// Warp statistics
struct WarpStatistics {
    uint32_t totalRenders;
    uint32_t successfulRenders;
    uint32_t failedRenders;
    float32 averageRenderTime;
    float32 averageIntensity;
    float32 averageRadius;
    float32 averageAngle;
    float32 averageFrequency;
    uint64_t totalRenderTime;
    uint32_t sampleCount;
    uint32_t passCount;
    float32 cacheHitRatio;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    uint32_t totalAnimations;
    uint32_t completedAnimations;
    uint32_t loopedAnimations;
    
    WarpStatistics() : totalRenders(0), successfulRenders(0), failedRenders(0), averageRenderTime(0.0f), 
                       averageIntensity(0.0f), averageRadius(0.0f), averageAngle(0.0f), averageFrequency(0.0f), 
                       totalRenderTime(0), sampleCount(0), passCount(0), cacheHitRatio(0.0f), cacheHits(0), 
                       cacheMisses(0), totalAnimations(0), completedAnimations(0), loopedAnimations(0) {}
};

// Warp effect
class WarpEffect {
public:
    WarpEffect();
    virtual ~WarpEffect() = default;
    
    // Effect management
    void initialize(const WarpProperties& properties);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Properties
    void setProperties(const WarpProperties& properties);
    const WarpProperties& getProperties() const;
    void setType(WarpType type);
    void setDistortionMode(WarpDistortionMode distortionMode);
    void setQuality(WarpQuality quality);
    void setColorMode(WarpColorMode colorMode);
    void setIntensity(float32 intensity);
    void setRadius(float32 radius);
    void setAngle(float32 angle);
    void setFrequency(float32 frequency);
    void setPhase(float32 phase);
    void setDuration(float32 duration);
    void setDelay(float32 delay);
    void setCenterX(float32 centerX);
    void setCenterY(float32 centerY);
    void setStrength(float32 strength);
    void setSoftness(float32 softness);
    
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
                std::vector<uint8>& outputImage, const WarpAnimationState& animationState) const;
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
    bool validateProperties(const WarpProperties& properties) const;
    bool validateImage(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    bool validateDimensions(uint32_t width, uint32_t height) const;
    
    // Animation state
    WarpAnimationState getAnimationState() const;
    void updateAnimationState(float32 deltaTime);
    void setAnimationState(const WarpAnimationState& state);
    
    // Performance
    void enableCache(bool enabled);
    bool isCacheEnabled() const;
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    void clearCache();
    
    // Statistics
    WarpStatistics getStatistics() const;
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
    void addWarpEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void removeWarpEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void clearWarpEffectEventListeners();
    
    // Utility methods
    void cloneFrom(const WarpEffect& other);
    virtual std::unique_ptr<WarpEffect> clone() const;
    bool equals(const WarpEffect& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    WarpProperties properties_;
    WarpAnimationState animationState_;
    WarpStatistics statistics_;
    
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
    virtual void triggerWarpEffectEvent(const std::string& eventType);
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage) const;
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage, const WarpAnimationState& animationState) const;
    
    // Rendering helpers
    virtual bool renderBarrelWarp(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                   std::vector<uint8>& outputImage) const;
    virtual bool renderPinchushionWarp(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                        std::vector<uint8>& outputImage) const;
    virtual bool renderSwirlWarp(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                 std::vector<uint8>& outputImage) const;
    virtual bool renderTwistWarp(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                 std::vector<uint8>& outputImage) const;
    virtual bool renderFisheyeWarp(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                   std::vector<uint8>& outputImage) const;
    
    // Color helpers
    virtual std::array<float32, 4> calculateWarpColor(const std::array<float32, 4>& baseColor, 
                                                     const WarpAnimationState& animationState) const;
    virtual std::array<float32, 4> interpolateColor(const std::array<float32, 4>& color1, 
                                                   const std::array<float32, 4>& color2, float32 t) const;
    virtual std::array<float32, 4> calculateGradientColor(float32 progress) const;
    virtual std::array<float32, 4> calculateMultiColor(float32 phase) const;
    
    // Animation helpers
    virtual void updateAnimationStateInternal(float32 deltaTime);
    virtual float32 calculateIntensity(float32 time, float32 duration, float32 delay, float32 intensity) const;
    virtual float32 calculateRadius(float32 time, float32 duration, float32 delay, float32 radius) const;
    virtual float32 calculateAngle(float32 time, float32 duration, float32 delay, float32 angle, float32 frequency) const;
    virtual float32 calculatePhase(float32 time, float32 duration, float32 delay, float32 phase, float32 frequency) const;
    virtual float32 calculateCenterX(float32 time, float32 duration, float32 delay, float32 centerX) const;
    virtual float32 calculateCenterY(float32 time, float32 duration, float32 delay, float32 centerY) const;
    virtual float32 calculateStrength(float32 time, float32 duration, float32 delay, float32 strength) const;
    virtual std::array<float32, 4> calculateAnimatedColor(const WarpAnimationState& animationState) const;
    virtual void handleLooping(WarpAnimationState& state) const;
    virtual void handlePingPong(WarpAnimationState& state) const;
    virtual void handleReverse(WarpAnimationState& state) const;
    virtual void handleAutoReverse(WarpAnimationState& state) const;
    
    // Quality helpers
    virtual uint32_t calculateSampleCount(WarpQuality quality) const;
    virtual uint32_t calculatePassCount(WarpQuality quality) const;
    virtual float32 calculateWarpQuality(WarpQuality quality) const;
    
    // Validation helpers
    virtual bool validatePropertiesInternal(const WarpProperties& properties) const;
    virtual bool validateImageInternal(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    virtual bool validateDimensionsInternal(uint32_t width, uint32_t height) const;
    
    // Cache helpers
    virtual std::string generateCacheKey(const std::vector<uint8>& image, uint32_t width, uint32_t height, 
                                        const WarpAnimationState& animationState) const;
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
    virtual void logAnimationOperation(const std::string& operation, const WarpAnimationState& state);
    virtual std::string formatProperties(const WarpProperties& properties) const;
    virtual std::string formatAnimationState(const WarpAnimationState& state) const;
};

// Specialized warp effects
class BarrelWarpEffect : public WarpEffect {
public:
    BarrelWarpEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<WarpEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class PinchusionWarpEffect : public WarpEffect {
public:
    PinchusionWarpEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<WarpEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class SwirlWarpEffect : public WarpEffect {
public:
    SwirlWarpEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<WarpEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class TwistWarpEffect : public WarpEffect {
public:
    TwistWarpEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<WarpEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class FisheyeWarpEffect : public WarpEffect {
public:
    FisheyeWarpEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<WarpEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class AnimatedWarpEffect : public WarpEffect {
public:
    AnimatedWarpEffect();
    void enableAnimation(bool enabled) override;
    void startAnimation() override;
    void stopAnimation() override;
    void updateAnimationState(float32 deltaTime) override;
    std::unique_ptr<WarpEffect> clone() const override;
    
protected:
    void updateAnimationStateInternal(float32 deltaTime) override;
};

class GradientWarpEffect : public WarpEffect {
public:
    GradientWarpEffect();
    void setGradientColors(const std::vector<std::array<float32, 4>>& colors);
    std::vector<std::array<float32, 4>> getGradientColors() const;
    std::unique_ptr<WarpEffect> clone() const override;
    
protected:
    std::array<float32, 4> calculateGradientColor(float32 progress) const override;
    
private:
    std::vector<std::array<float32, 4>> gradientColors_;
};

// Warp effect factory
class WarpEffectFactory {
public:
    static std::unique_ptr<WarpEffect> createWarpEffect(WarpType type);
    static std::unique_ptr<BarrelWarpEffect> createBarrelWarpEffect();
    static std::unique_ptr<PinchusionWarpEffect> createPinchusionWarpEffect();
    static std::unique_ptr<SwirlWarpEffect> createSwirlWarpEffect();
    static std::unique_ptr<TwistWarpEffect> createTwistWarpEffect();
    static std::unique_ptr<FisheyeWarpEffect> createFisheyeWarpEffect();
    static std::unique_ptr<AnimatedWarpEffect> createAnimatedWarpEffect();
    static std::unique_ptr<GradientWarpEffect> createGradientWarpEffect();
    static WarpProperties createDefaultProperties(WarpType type);
    static std::vector<WarpType> getSupportedWarpTypes();
    static std::vector<WarpDistortionMode> getSupportedDistortionModes();
    static std::vector<WarpQuality> getSupportedQualityLevels();
    static std::vector<WarpColorMode> getSupportedColorModes();
    static WarpType detectBestType(const std::vector<uint8>& image, uint32_t width, uint32_t height);
};

} // namespace RFEffects