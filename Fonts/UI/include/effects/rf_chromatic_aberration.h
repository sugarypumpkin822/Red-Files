#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFEffects {

// Chromatic aberration types
enum class ChromaticAberrationType {
    RADIAL,
    TANGENTIAL,
    AXIAL,
    LATERAL,
    CUSTOM
};

// Chromatic aberration modes
enum class ChromaticAberrationMode {
    RGB,
    LAB,
    HSV,
    CUSTOM
};

// Chromatic aberration quality levels
enum class ChromaticAberrationQuality {
    LOW,
    MEDIUM,
    HIGH,
    ULTRA,
    CUSTOM
};

// Chromatic aberration color modes
enum class ChromaticAberrationColorMode {
    SOLID,
    GRADIENT,
    MULTI_COLOR,
    CUSTOM
};

// Chromatic aberration properties
struct ChromaticAberrationProperties {
    ChromaticAberrationType type;
    ChromaticAberrationMode mode;
    ChromaticAberrationQuality quality;
    ChromaticAberrationColorMode colorMode;
    float32 redOffset;
    float32 greenOffset;
    float32 blueOffset;
    float32 intensity;
    float32 radius;
    float32 centerX;
    float32 centerY;
    float32 angle;
    float32 frequency;
    float32 phase;
    float32 duration;
    float32 delay;
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
    
    ChromaticAberrationProperties() : type(ChromaticAberrationType::RADIAL), mode(ChromaticAberrationMode::RGB), 
                                    quality(ChromaticAberrationQuality::MEDIUM), colorMode(ChromaticAberrationColorMode::SOLID), 
                                    redOffset(2.0f), greenOffset(1.0f), blueOffset(-1.0f), intensity(1.0f), 
                                    radius(50.0f), centerX(0.5f), centerY(0.5f), angle(0.0f), frequency(1.0f), 
                                    phase(0.0f), duration(1.0f), delay(0.0f), enableAntialiasing(true), enableDithering(false), 
                                    enableHDR(false), enableAnimation(true), enableLooping(false), enablePingPong(false), 
                                    enableReverse(false), enableAutoReverse(false), sampleCount(8), passCount(1), 
                                    animationSpeed(1.0f), primaryColor{1.0f, 0.0f, 0.0f, 1.0f}, 
                                    secondaryColor{0.0f, 1.0f, 0.0f, 1.0f}, backgroundColor{0.0f, 0.0f, 1.0f, 1.0f} {}
};

// Chromatic aberration animation state
struct ChromaticAberrationAnimationState {
    float32 time;
    float32 currentRedOffset;
    float32 currentGreenOffset;
    float32 currentBlueOffset;
    float32 currentIntensity;
    float32 currentRadius;
    float32 currentCenterX;
    float32 currentCenterY;
    float32 currentAngle;
    float32 currentFrequency;
    float32 currentPhase;
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
    
    ChromaticAberrationAnimationState() : time(0.0f), currentRedOffset(2.0f), currentGreenOffset(1.0f), currentBlueOffset(-1.0f), 
                                       currentIntensity(1.0f), currentRadius(50.0f), currentCenterX(0.5f), currentCenterY(0.5f), 
                                       currentAngle(0.0f), currentFrequency(1.0f), currentPhase(0.0f), 
                                       currentColor{1.0f, 0.0f, 0.0f, 1.0f}, isActive(false), isPlaying(false), isPaused(false), 
                                       isLooping(false), isPingPong(false), isReversed(false), isAutoReversing(false), 
                                       loopCount(0), currentLoop(0) {}
};

// Chromatic aberration statistics
struct ChromaticAberrationStatistics {
    uint32_t totalRenders;
    uint32_t successfulRenders;
    uint32_t failedRenders;
    float32 averageRenderTime;
    float32 averageIntensity;
    float32 averageRadius;
    float32 averageRedOffset;
    float32 averageGreenOffset;
    float32 averageBlueOffset;
    uint64_t totalRenderTime;
    uint32_t sampleCount;
    uint32_t passCount;
    float32 cacheHitRatio;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    uint32_t totalAnimations;
    uint32_t completedAnimations;
    uint32_t loopedAnimations;
    
    ChromaticAberrationStatistics() : totalRenders(0), successfulRenders(0), failedRenders(0), averageRenderTime(0.0f), 
                                      averageIntensity(0.0f), averageRadius(0.0f), averageRedOffset(0.0f), 
                                      averageGreenOffset(0.0f), averageBlueOffset(0.0f), totalRenderTime(0), 
                                      sampleCount(0), passCount(0), cacheHitRatio(0.0f), cacheHits(0), cacheMisses(0), 
                                      totalAnimations(0), completedAnimations(0), loopedAnimations(0) {}
};

// Chromatic aberration effect
class ChromaticAberrationEffect {
public:
    ChromaticAberrationEffect();
    virtual ~ChromaticAberrationEffect() = default;
    
    // Effect management
    void initialize(const ChromaticAberrationProperties& properties);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Properties
    void setProperties(const ChromaticAberrationProperties& properties);
    const ChromaticAberrationProperties& getProperties() const;
    void setType(ChromaticAberrationType type);
    void setMode(ChromaticAberrationMode mode);
    void setQuality(ChromaticAberrationQuality quality);
    void setColorMode(ChromaticAberrationColorMode colorMode);
    void setRedOffset(float32 redOffset);
    void setGreenOffset(float32 greenOffset);
    void setBlueOffset(float32 blueOffset);
    void setIntensity(float32 intensity);
    void setRadius(float32 radius);
    void setCenterX(float32 centerX);
    void setCenterY(float32 centerY);
    void setAngle(float32 angle);
    void setFrequency(float32 frequency);
    void setPhase(float32 phase);
    void setDuration(float32 duration);
    void setDelay(float32 delay);
    
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
                std::vector<uint8>& outputImage, const ChromaticAberrationAnimationState& animationState) const;
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
    bool validateProperties(const ChromaticAberrationProperties& properties) const;
    bool validateImage(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    bool validateDimensions(uint32_t width, uint32_t height) const;
    
    // Animation state
    ChromaticAberrationAnimationState getAnimationState() const;
    void updateAnimationState(float32 deltaTime);
    void setAnimationState(const ChromaticAberrationAnimationState& state);
    
    // Performance
    void enableCache(bool enabled);
    bool isCacheEnabled() const;
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    void clearCache();
    
    // Statistics
    ChromaticAberrationStatistics getStatistics() const;
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
    void addChromaticAberrationEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void removeChromaticAberrationEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void clearChromaticAberrationEffectEventListeners();
    
    // Utility methods
    void cloneFrom(const ChromaticAberrationEffect& other);
    virtual std::unique_ptr<ChromaticAberrationEffect> clone() const;
    bool equals(const ChromaticAberrationEffect& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    ChromaticAberrationProperties properties_;
    ChromaticAberrationAnimationState animationState_;
    ChromaticAberrationStatistics statistics_;
    
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
    virtual void triggerChromaticAberrationEffectEvent(const std::string& eventType);
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage) const;
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage, const ChromaticAberrationAnimationState& animationState) const;
    
    // Rendering helpers
    virtual bool renderRadialChromaticAberration(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                                   std::vector<uint8>& outputImage) const;
    virtual bool renderTangentialChromaticAberration(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                                      std::vector<uint8>& outputImage) const;
    virtual bool renderAxialChromaticAberration(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                                std::vector<uint8>& outputImage) const;
    virtual bool renderLateralChromaticAberration(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                                  std::vector<uint8>& outputImage) const;
    
    // Color helpers
    virtual std::array<float32, 4> calculateChromaticAberrationColor(const std::array<float32, 4>& baseColor, 
                                                                   const ChromaticAberrationAnimationState& animationState) const;
    virtual std::array<float32, 4> interpolateColor(const std::array<float32, 4>& color1, 
                                                   const std::array<float32, 4>& color2, float32 t) const;
    virtual std::array<float32, 4> calculateGradientColor(float32 progress) const;
    virtual std::array<float32, 4> calculateMultiColor(float32 phase) const;
    virtual std::array<float32, 4> applyRGBOffset(const std::array<float32, 4>& color, float32 redOffset, float32 greenOffset, float32 blueOffset) const;
    
    // Animation helpers
    virtual void updateAnimationStateInternal(float32 deltaTime);
    virtual float32 calculateRedOffset(float32 time, float32 duration, float32 delay, float32 redOffset) const;
    virtual float32 calculateGreenOffset(float32 time, float32 duration, float32 delay, float32 greenOffset) const;
    virtual float32 calculateBlueOffset(float32 time, float32 duration, float32 delay, float32 blueOffset) const;
    virtual float32 calculateIntensity(float32 time, float32 duration, float32 delay, float32 intensity) const;
    virtual float32 calculateRadius(float32 time, float32 duration, float32 delay, float32 radius) const;
    virtual float32 calculateCenterX(float32 time, float32 duration, float32 delay, float32 centerX) const;
    virtual float32 calculateCenterY(float32 time, float32 duration, float32 delay, float32 centerY) const;
    virtual float32 calculateAngle(float32 time, float32 duration, float32 delay, float32 angle, float32 frequency) const;
    virtual float32 calculatePhase(float32 time, float32 duration, float32 delay, float32 phase, float32 frequency) const;
    virtual std::array<float32, 4> calculateAnimatedColor(const ChromaticAberrationAnimationState& animationState) const;
    virtual void handleLooping(ChromaticAberrationAnimationState& state) const;
    virtual void handlePingPong(ChromaticAberrationAnimationState& state) const;
    virtual void handleReverse(ChromaticAberrationAnimationState& state) const;
    virtual void handleAutoReverse(ChromaticAberrationAnimationState& state) const;
    
    // Quality helpers
    virtual uint32_t calculateSampleCount(ChromaticAberrationQuality quality) const;
    virtual uint32_t calculatePassCount(ChromaticAberrationQuality quality) const;
    virtual float32 calculateChromaticAberrationQuality(ChromaticAberrationQuality quality) const;
    
    // Validation helpers
    virtual bool validatePropertiesInternal(const ChromaticAberrationProperties& properties) const;
    virtual bool validateImageInternal(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    virtual bool validateDimensionsInternal(uint32_t width, uint32_t height) const;
    
    // Cache helpers
    virtual std::string generateCacheKey(const std::vector<uint8>& image, uint32_t width, uint32_t height, 
                                        const ChromaticAberrationAnimationState& animationState) const;
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
    virtual void logAnimationOperation(const std::string& operation, const ChromaticAberrationAnimationState& state);
    virtual std::string formatProperties(const ChromaticAberrationProperties& properties) const;
    virtual std::string formatAnimationState(const ChromaticAberrationAnimationState& state) const;
};

// Specialized chromatic aberration effects
class RadialChromaticAberrationEffect : public ChromaticAberrationEffect {
public:
    RadialChromaticAberrationEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<ChromaticAberrationEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class TangentialChromaticAberrationEffect : public ChromaticAberrationEffect {
public:
    TangentialChromaticAberrationEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<ChromaticAberrationEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class AxialChromaticAberrationEffect : public ChromaticAberrationEffect {
public:
    AxialChromaticAberrationEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<ChromaticAberrationEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class LateralChromaticAberrationEffect : public ChromaticAberrationEffect {
public:
    LateralChromaticAberrationEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<ChromaticAberrationEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class AnimatedChromaticAberrationEffect : public ChromaticAberrationEffect {
public:
    AnimatedChromaticAberrationEffect();
    void enableAnimation(bool enabled) override;
    void startAnimation() override;
    void stopAnimation() override;
    void updateAnimationState(float32 deltaTime) override;
    std::unique_ptr<ChromaticAberrationEffect> clone() const override;
    
protected:
    void updateAnimationStateInternal(float32 deltaTime) override;
};

class GradientChromaticAberrationEffect : public ChromaticAberrationEffect {
public:
    GradientChromaticAberrationEffect();
    void setGradientColors(const std::vector<std::array<float32, 4>>& colors);
    std::vector<std::array<float32, 4>> getGradientColors() const;
    std::unique_ptr<ChromaticAberrationEffect> clone() const override;
    
protected:
    std::array<float32, 4> calculateGradientColor(float32 progress) const override;
    
private:
    std::vector<std::array<float32, 4>> gradientColors_;
};

// Chromatic aberration effect factory
class ChromaticAberrationEffectFactory {
public:
    static std::unique_ptr<ChromaticAberrationEffect> createChromaticAberrationEffect(ChromaticAberrationType type);
    static std::unique_ptr<RadialChromaticAberrationEffect> createRadialChromaticAberrationEffect();
    static std::unique_ptr<TangentialChromaticAberrationEffect> createTangentialChromaticAberrationEffect();
    static std::unique_ptr<AxialChromaticAberrationEffect> createAxialChromaticAberrationEffect();
    static std::unique_ptr<LateralChromaticAberrationEffect> createLateralChromaticAberrationEffect();
    static std::unique_ptr<AnimatedChromaticAberrationEffect> createAnimatedChromaticAberrationEffect();
    static std::unique_ptr<GradientChromaticAberrationEffect> createGradientChromaticAberrationEffect();
    static ChromaticAberrationProperties createDefaultProperties(ChromaticAberrationType type);
    static std::vector<ChromaticAberrationType> getSupportedChromaticAberrationTypes();
    static std::vector<ChromaticAberrationMode> getSupportedModes();
    static std::vector<ChromaticAberrationQuality> getSupportedQualityLevels();
    static std::vector<ChromaticAberrationColorMode> getSupportedColorModes();
    static ChromaticAberrationType detectBestType(const std::vector<uint8>& image, uint32_t width, uint32_t height);
};

} // namespace RFEffects