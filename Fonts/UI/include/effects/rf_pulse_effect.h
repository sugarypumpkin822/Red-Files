#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFEffects {

// Pulse effect types
enum class PulseType {
    ALPHA_PULSE,
    COLOR_PULSE,
    SCALE_PULSE,
    ROTATION_PULSE,
    POSITION_PULSE,
    CUSTOM
};

// Pulse waveforms
enum class PulseWaveform {
    SINE,
    SQUARE,
    TRIANGLE,
    SAWTOOTH,
    NOISE,
    CUSTOM
};

// Pulse quality levels
enum class PulseQuality {
    LOW,
    MEDIUM,
    HIGH,
    ULTRA,
    CUSTOM
};

// Pulse color modes
enum class PulseColorMode {
    SOLID,
    GRADIENT,
    MULTI_COLOR,
    CUSTOM
};

// Pulse properties
struct PulseProperties {
    PulseType type;
    PulseWaveform waveform;
    PulseQuality quality;
    PulseColorMode colorMode;
    float32 frequency;
    float32 amplitude;
    float32 phase;
    float32 offset;
    float32 duration;
    float32 delay;
    float32 minAlpha;
    float32 maxAlpha;
    float32 minScale;
    float32 maxScale;
    float32 minRotation;
    float32 maxRotation;
    float32 minOffsetX;
    float32 maxOffsetX;
    float32 minOffsetY;
    float32 maxOffsetY;
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
    
    PulseProperties() : type(PulseType::ALPHA_PULSE), waveform(PulseWaveform::SINE), quality(PulseQuality::MEDIUM), 
                       colorMode(PulseColorMode::SOLID), frequency(1.0f), amplitude(1.0f), phase(0.0f), 
                       offset(0.0f), duration(1.0f), delay(0.0f), minAlpha(0.0f), maxAlpha(1.0f), 
                       minScale(0.8f), maxScale(1.2f), minRotation(-15.0f), maxRotation(15.0f), 
                       minOffsetX(-5.0f), maxOffsetX(5.0f), minOffsetY(-5.0f), maxOffsetY(5.0f), 
                       enableAntialiasing(true), enableDithering(false), enableHDR(false), enableAnimation(true), 
                       enableLooping(true), enablePingPong(false), enableReverse(false), enableAutoReverse(false), 
                       sampleCount(8), passCount(1), animationSpeed(1.0f), 
                       primaryColor{1.0f, 1.0f, 1.0f, 1.0f}, secondaryColor{0.5f, 0.5f, 0.5f, 1.0f}, 
                       backgroundColor{0.0f, 0.0f, 0.0f, 0.0f} {}
};

// Pulse animation state
struct PulseAnimationState {
    float32 time;
    float32 phase;
    float32 currentAlpha;
    float32 currentScale;
    float32 currentRotation;
    float32 currentOffsetX;
    float32 currentOffsetY;
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
    
    PulseAnimationState() : time(0.0f), phase(0.0f), currentAlpha(1.0f), currentScale(1.0f), 
                          currentRotation(0.0f), currentOffsetX(0.0f), currentOffsetY(0.0f), 
                          currentColor{1.0f, 1.0f, 1.0f, 1.0f}, isActive(false), isPlaying(false), 
                          isPaused(false), isLooping(false), isPingPong(false), isReversed(false), 
                          isAutoReversing(false), loopCount(0), currentLoop(0) {}
};

// Pulse statistics
struct PulseStatistics {
    uint32_t totalRenders;
    uint32_t successfulRenders;
    uint32_t failedRenders;
    float32 averageRenderTime;
    float32 averageFrequency;
    float32 averageAmplitude;
    float32 averagePhase;
    uint64_t totalRenderTime;
    uint32_t sampleCount;
    uint32_t passCount;
    float32 cacheHitRatio;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    uint32_t totalAnimations;
    uint32_t completedAnimations;
    uint32_t loopedAnimations;
    
    PulseStatistics() : totalRenders(0), successfulRenders(0), failedRenders(0), averageRenderTime(0.0f), 
                        averageFrequency(0.0f), averageAmplitude(0.0f), averagePhase(0.0f), 
                        totalRenderTime(0), sampleCount(0), passCount(0), cacheHitRatio(0.0f), cacheHits(0), 
                        cacheMisses(0), totalAnimations(0), completedAnimations(0), loopedAnimations(0) {}
};

// Pulse effect
class PulseEffect {
public:
    PulseEffect();
    virtual ~PulseEffect() = default;
    
    // Effect management
    void initialize(const PulseProperties& properties);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Properties
    void setProperties(const PulseProperties& properties);
    const PulseProperties& getProperties() const;
    void setType(PulseType type);
    void setWaveform(PulseWaveform waveform);
    void setQuality(PulseQuality quality);
    void setColorMode(PulseColorMode colorMode);
    void setFrequency(float32 frequency);
    void setAmplitude(float32 amplitude);
    void setPhase(float32 phase);
    void setOffset(float32 offset);
    void setDuration(float32 duration);
    void setDelay(float32 delay);
    void setMinAlpha(float32 minAlpha);
    void setMaxAlpha(float32 maxAlpha);
    void setMinScale(float32 minScale);
    void setMaxScale(float32 maxScale);
    void setMinRotation(float32 minRotation);
    void setMaxRotation(float32 maxRotation);
    void setMinOffsetX(float32 minOffsetX);
    void setMaxOffsetX(float32 maxOffsetX);
    void setMinOffsetY(float32 minOffsetY);
    void setMaxOffsetY(float32 maxOffsetY);
    
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
                std::vector<uint8>& outputImage, const PulseAnimationState& animationState) const;
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
    bool validateProperties(const PulseProperties& properties) const;
    bool validateImage(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    bool validateDimensions(uint32_t width, uint32_t height) const;
    
    // Animation state
    PulseAnimationState getAnimationState() const;
    void updateAnimationState(float32 deltaTime);
    void setAnimationState(const PulseAnimationState& state);
    
    // Waveform functions
    float32 calculateWaveform(float32 phase, PulseWaveform waveform) const;
    float32 calculateSineWaveform(float32 phase) const;
    float32 calculateSquareWaveform(float32 phase) const;
    float32 calculateTriangleWaveform(float32 phase) const;
    float32 calculateSawtoothWaveform(float32 phase) const;
    float32 calculateNoiseWaveform(float32 phase) const;
    
    // Performance
    void enableCache(bool enabled);
    bool isCacheEnabled() const;
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    void clearCache();
    
    // Statistics
    PulseStatistics getStatistics() const;
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
    void addPulseEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void removePulseEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void clearPulseEffectEventListeners();
    
    // Utility methods
    void cloneFrom(const PulseEffect& other);
    virtual std::unique_ptr<PulseEffect> clone() const;
    bool equals(const PulseEffect& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    PulseProperties properties_;
    PulseAnimationState animationState_;
    PulseStatistics statistics_;
    
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
    virtual void triggerPulseEffectEvent(const std::string& eventType);
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage) const;
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage, const PulseAnimationState& animationState) const;
    
    // Rendering helpers
    virtual bool renderAlphaPulse(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                 std::vector<uint8>& outputImage) const;
    virtual bool renderColorPulse(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                  std::vector<uint8>& outputImage) const;
    virtual bool renderScalePulse(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                 std::vector<uint8>& outputImage) const;
    virtual bool renderRotationPulse(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                    std::vector<uint8>& outputImage) const;
    virtual bool renderPositionPulse(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                     std::vector<uint8>& outputImage) const;
    
    // Color helpers
    virtual std::array<float32, 4> calculatePulseColor(const std::array<float32, 4>& baseColor, 
                                                       const PulseAnimationState& animationState) const;
    virtual std::array<float32, 4> interpolateColor(const std::array<float32, 4>& color1, 
                                                   const std::array<float32, 4>& color2, float32 t) const;
    virtual std::array<float32, 4> calculateGradientColor(float32 progress) const;
    virtual std::array<float32, 4> calculateMultiColor(float32 phase) const;
    
    // Animation helpers
    virtual void updateAnimationStateInternal(float32 deltaTime);
    virtual float32 calculatePhase(float32 time, float32 frequency, float32 offset) const;
    virtual float32 calculateAlpha(float32 phase, float32 minAlpha, float32 maxAlpha, float32 amplitude) const;
    virtual float32 calculateScale(float32 phase, float32 minScale, float32 maxScale, float32 amplitude) const;
    virtual float32 calculateRotation(float32 phase, float32 minRotation, float32 maxRotation, float32 amplitude) const;
    virtual float32 calculateOffset(float32 phase, float32 minOffset, float32 maxOffset, float32 amplitude) const;
    virtual std::array<float32, 4> calculateAnimatedColor(const PulseAnimationState& animationState) const;
    virtual void handleLooping(PulseAnimationState& state) const;
    virtual void handlePingPong(PulseAnimationState& state) const;
    virtual void handleReverse(PulseAnimationState& state) const;
    virtual void handleAutoReverse(PulseAnimationState& state) const;
    
    // Quality helpers
    virtual uint32_t calculateSampleCount(PulseQuality quality) const;
    virtual uint32_t calculatePassCount(PulseQuality quality) const;
    virtual float32 calculatePulseQuality(PulseQuality quality) const;
    
    // Validation helpers
    virtual bool validatePropertiesInternal(const PulseProperties& properties) const;
    virtual bool validateImageInternal(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    virtual bool validateDimensionsInternal(uint32_t width, uint32_t height) const;
    
    // Cache helpers
    virtual std::string generateCacheKey(const std::vector<uint8>& image, uint32_t width, uint32_t height, 
                                        const PulseAnimationState& animationState) const;
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
    virtual void logAnimationOperation(const std::string& operation, const PulseAnimationState& state);
    virtual std::string formatProperties(const PulseProperties& properties) const;
    virtual std::string formatAnimationState(const PulseAnimationState& state) const;
};

// Specialized pulse effects
class AlphaPulseEffect : public PulseEffect {
public:
    AlphaPulseEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<PulseEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class ColorPulseEffect : public PulseEffect {
public:
    ColorPulseEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<PulseEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class ScalePulseEffect : public PulseEffect {
public:
    ScalePulseEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<PulseEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class RotationPulseEffect : public PulseEffect {
public:
    RotationPulseEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<PulseEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class PositionPulseEffect : public PulseEffect {
public:
    PositionPulseEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<PulseEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class AnimatedPulseEffect : public PulseEffect {
public:
    AnimatedPulseEffect();
    void enableAnimation(bool enabled) override;
    void startAnimation() override;
    void stopAnimation() override;
    void updateAnimationState(float32 deltaTime) override;
    std::unique_ptr<PulseEffect> clone() const override;
    
protected:
    void updateAnimationStateInternal(float32 deltaTime) override;
};

class GradientPulseEffect : public PulseEffect {
public:
    GradientPulseEffect();
    void setGradientColors(const std::vector<std::array<float32, 4>>& colors);
    std::vector<std::array<float32, 4>> getGradientColors() const;
    std::unique_ptr<PulseEffect> clone() const override;
    
protected:
    std::array<float32, 4> calculateGradientColor(float32 progress) const override;
    
private:
    std::vector<std::array<float32, 4>> gradientColors_;
};

// Pulse effect factory
class PulseEffectFactory {
public:
    static std::unique_ptr<PulseEffect> createPulseEffect(PulseType type);
    static std::unique_ptr<AlphaPulseEffect> createAlphaPulseEffect();
    static std::unique_ptr<ColorPulseEffect> createColorPulseEffect();
    static std::unique_ptr<ScalePulseEffect> createScalePulseEffect();
    static std::unique_ptr<RotationPulseEffect> createRotationPulseEffect();
    static std::unique_ptr<PositionPulseEffect> createPositionPulseEffect();
    static std::unique_ptr<AnimatedPulseEffect> createAnimatedPulseEffect();
    static std::unique_ptr<GradientPulseEffect> createGradientPulseEffect();
    static PulseProperties createDefaultProperties(PulseType type);
    static std::vector<PulseType> getSupportedPulseTypes();
    static std::vector<PulseWaveform> getSupportedWaveforms();
    static std::vector<PulseQuality> getSupportedQualityLevels();
    static std::vector<PulseColorMode> getSupportedColorModes();
    static PulseType detectBestType(const std::vector<uint8>& image, uint32_t width, uint32_t height);
};

} // namespace RFEffects