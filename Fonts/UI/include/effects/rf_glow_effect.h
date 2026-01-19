#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFEffects {

// Glow effect types
enum class GlowType {
    OUTER_GLOW,
    INNER_GLOW,
    BOTH_GLOW,
    CUSTOM
};

// Glow blend modes
enum class GlowBlendMode {
    NORMAL,
    ADDITIVE,
    MULTIPLY,
    SCREEN,
    OVERLAY,
    SOFT_LIGHT,
    HARD_LIGHT,
    COLOR_DODGE,
    COLOR_BURN,
    CUSTOM
};

// Glow quality levels
enum class GlowQuality {
    LOW,
    MEDIUM,
    HIGH,
    ULTRA,
    CUSTOM
};

// Glow color modes
enum class GlowColorMode {
    SOLID,
    GRADIENT,
    RAINBOW,
    PULSE,
    CUSTOM
};

// Glow properties
struct GlowProperties {
    GlowType type;
    GlowBlendMode blendMode;
    GlowQuality quality;
    GlowColorMode colorMode;
    float32 intensity;
    float32 radius;
    float32 threshold;
    float32 softness;
    float32 spread;
    float32 falloff;
    float32 saturation;
    float32 brightness;
    float32 contrast;
    float32 opacity;
    bool enableAntialiasing;
    bool enableDithering;
    bool enableHDR;
    bool enableAnimation;
    bool enablePulsing;
    bool enableColorShift;
    uint32_t sampleCount;
    uint32_t passCount;
    float32 animationSpeed;
    float32 pulseSpeed;
    float32 colorShiftSpeed;
    std::array<float32, 4> primaryColor;
    std::array<float32, 4> secondaryColor;
    std::array<float32, 4> backgroundColor;
    
    GlowProperties() : type(GlowType::OUTER_GLOW), blendMode(GlowBlendMode::NORMAL), quality(GlowQuality::MEDIUM), 
                        colorMode(GlowColorMode::SOLID), intensity(1.0f), radius(10.0f), threshold(0.5f), 
                        softness(1.0f), spread(1.0f), falloff(1.0f), saturation(1.0f), brightness(1.0f), 
                        contrast(1.0f), opacity(1.0f), enableAntialiasing(true), enableDithering(false), 
                        enableHDR(false), enableAnimation(false), enablePulsing(false), enableColorShift(false), 
                        sampleCount(8), passCount(1), animationSpeed(1.0f), pulseSpeed(1.0f), 
                        colorShiftSpeed(1.0f), primaryColor{1.0f, 1.0f, 1.0f, 1.0f}, 
                        secondaryColor{1.0f, 0.5f, 0.0f, 1.0f}, backgroundColor{0.0f, 0.0f, 0.0f, 0.0f} {}
};

// Glow animation state
struct GlowAnimationState {
    float32 time;
    float32 pulsePhase;
    float32 colorShiftPhase;
    float32 intensityModulation;
    float32 radiusModulation;
    std::array<float32, 4> currentColor;
    bool isActive;
    bool isPulsing;
    bool isColorShifting;
    
    GlowAnimationState() : time(0.0f), pulsePhase(0.0f), colorShiftPhase(0.0f), intensityModulation(1.0f), 
                           radiusModulation(1.0f), currentColor{1.0f, 1.0f, 1.0f, 1.0f}, isActive(false), 
                           isPulsing(false), isColorShifting(false) {}
};

// Glow statistics
struct GlowStatistics {
    uint32_t totalRenders;
    uint32_t successfulRenders;
    uint32_t failedRenders;
    float32 averageRenderTime;
    float32 averageIntensity;
    float32 averageRadius;
    uint64_t totalRenderTime;
    uint32_t sampleCount;
    uint32_t passCount;
    float32 cacheHitRatio;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    
    GlowStatistics() : totalRenders(0), successfulRenders(0), failedRenders(0), averageRenderTime(0.0f), 
                       averageIntensity(0.0f), averageRadius(0.0f), totalRenderTime(0), sampleCount(0), 
                       passCount(0), cacheHitRatio(0.0f), cacheHits(0), cacheMisses(0) {}
};

// Glow effect
class GlowEffect {
public:
    GlowEffect();
    virtual ~GlowEffect() = default;
    
    // Effect management
    void initialize(const GlowProperties& properties);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Properties
    void setProperties(const GlowProperties& properties);
    const GlowProperties& getProperties() const;
    void setType(GlowType type);
    void setBlendMode(GlowBlendMode blendMode);
    void setQuality(GlowQuality quality);
    void setColorMode(GlowColorMode colorMode);
    void setIntensity(float32 intensity);
    void setRadius(float32 radius);
    void setThreshold(float32 threshold);
    void setSoftness(float32 softness);
    void setSpread(float32 spread);
    void setFalloff(float32 falloff);
    void setOpacity(float32 opacity);
    
    // Color management
    void setPrimaryColor(const std::array<float32, 4>& color);
    void setSecondaryColor(const std::array<float32, 4>& color);
    void setBackgroundColor(const std::array<float32, 4>& color);
    const std::array<float32, 4>& getPrimaryColor() const;
    const std::array<float32, 4>& getSecondaryColor() const;
    const std::array<float32, 4>& getBackgroundColor() const;
    
    // Animation control
    void enableAnimation(bool enabled);
    void enablePulsing(bool enabled);
    void enableColorShift(bool enabled);
    void setAnimationSpeed(float32 speed);
    void setPulseSpeed(float32 speed);
    void setColorShiftSpeed(float32 speed);
    void startAnimation();
    void stopAnimation();
    void pauseAnimation();
    void resumeAnimation();
    bool isAnimationActive() const;
    
    // Rendering
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const;
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage, const GlowAnimationState& animationState) const;
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                uint32_t channels, std::vector<uint8>& outputImage) const;
    
    // Batch rendering
    bool renderBatch(const std::vector<std::vector<uint8>>& sourceImages, 
                    const std::vector<std::pair<uint32_t, uint32_t>>& dimensions, 
                    std::vector<std::vector<uint8>>& outputImages) const;
    
    // Quality settings
    void setSampleCount(uint32_t sampleCount);
    void setPassCount(uint32_t passCount);
    void enableAntialiasing(bool enabled);
    void enableDithering(bool enabled);
    void enableHDR(bool enabled);
    
    // Validation
    bool validateProperties(const GlowProperties& properties) const;
    bool validateImage(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    bool validateDimensions(uint32_t width, uint32_t height) const;
    
    // Animation state
    GlowAnimationState getAnimationState() const;
    void updateAnimationState(float32 deltaTime);
    void setAnimationState(const GlowAnimationState& state);
    
    // Performance
    void enableCache(bool enabled);
    bool isCacheEnabled() const;
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    void clearCache();
    
    // Statistics
    GlowStatistics getStatistics() const;
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
    void addGlowEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void removeGlowEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void clearGlowEffectEventListeners();
    
    // Utility methods
    void cloneFrom(const GlowEffect& other);
    virtual std::unique_ptr<GlowEffect> clone() const;
    bool equals(const GlowEffect& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    GlowProperties properties_;
    GlowAnimationState animationState_;
    GlowStatistics statistics_;
    
    bool isInitialized_;
    bool statisticsEnabled_;
    bool debuggingEnabled_;
    bool cacheEnabled_;
    bool animationEnabled_;
    bool pulsingEnabled_;
    bool colorShiftEnabled_;
    
    size_t currentCacheSize_;
    std::vector<std::string> debugMessages_;
    
    // Protected helper methods
    virtual void triggerGlowEffectEvent(const std::string& eventType);
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage) const;
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage, const GlowAnimationState& animationState) const;
    
    // Rendering helpers
    virtual bool renderOuterGlow(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                 std::vector<uint8>& outputImage) const;
    virtual bool renderInnerGlow(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                 std::vector<uint8>& outputImage) const;
    virtual bool renderBothGlow(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                std::vector<uint8>& outputImage) const;
    
    // Color helpers
    virtual std::array<float32, 4> calculateGlowColor(const std::array<float32, 4>& baseColor, 
                                                     const GlowAnimationState& animationState) const;
    virtual std::array<float32, 4> interpolateColor(const std::array<float32, 4>& color1, 
                                                   const std::array<float32, 4>& color2, float32 t) const;
    virtual std::array<float32, 4> calculateGradientColor(float32 distance, float32 maxDistance) const;
    virtual std::array<float32, 4> calculateRainbowColor(float32 phase) const;
    
    // Animation helpers
    virtual void updateAnimationStateInternal(float32 deltaTime);
    virtual float32 calculatePulseModulation(float32 time) const;
    virtual float32 calculateColorShiftPhase(float32 time) const;
    virtual std::array<float32, 4> calculateAnimatedColor(const std::array<float32, 4>& baseColor, 
                                                           const GlowAnimationState& animationState) const;
    
    // Quality helpers
    virtual uint32_t calculateSampleCount(GlowQuality quality) const;
    virtual uint32_t calculatePassCount(GlowQuality quality) const;
    virtual float32 calculateKernelWeight(float32 distance, float32 radius, float32 falloff) const;
    virtual std::vector<float32> generateGaussianKernel(float32 radius, float32 sigma) const;
    
    // Validation helpers
    virtual bool validatePropertiesInternal(const GlowProperties& properties) const;
    virtual bool validateImageInternal(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    virtual bool validateDimensionsInternal(uint32_t width, uint32_t height) const;
    
    // Cache helpers
    virtual std::string generateCacheKey(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    virtual bool getFromCache(const std::string& key, std::vector<uint8>& outputImage) const;
    virtual void addToCache(const std::string& key, const std::vector<uint8>& outputImage);
    virtual void removeFromCache(const std::string& key);
    virtual void updateCache();
    virtual void cleanupCache();
    
    // Statistics helpers
    virtual void updateRenderStatistics(bool success, uint64_t renderTime);
    virtual void updateCacheStatistics(bool hit);
    virtual void calculateStatistics();
    
    // Debug helpers
    virtual void addDebugMessage(const std::string& message);
    virtual void logRenderOperation(const std::string& operation, uint32_t width, uint32_t height);
    virtual std::string formatProperties(const GlowProperties& properties) const;
};

// Specialized glow effects
class OuterGlowEffect : public GlowEffect {
public:
    OuterGlowEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<GlowEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class InnerGlowEffect : public GlowEffect {
public:
    InnerGlowEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<GlowEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class AnimatedGlowEffect : public GlowEffect {
public:
    AnimatedGlowEffect();
    void enableAnimation(bool enabled) override;
    void startAnimation() override;
    void stopAnimation() override;
    void updateAnimationState(float32 deltaTime) override;
    std::unique_ptr<GlowEffect> clone() const override;
    
protected:
    void updateAnimationStateInternal(float32 deltaTime) override;
};

class GradientGlowEffect : public GlowEffect {
public:
    GradientGlowEffect();
    void setGradientColors(const std::vector<std::array<float32, 4>>& colors);
    std::vector<std::array<float32, 4>> getGradientColors() const;
    std::unique_ptr<GlowEffect> clone() const override;
    
protected:
    std::array<float32, 4> calculateGradientColor(float32 distance, float32 maxDistance) const override;
    
private:
    std::vector<std::array<float32, 4>> gradientColors_;
};

// Glow effect factory
class GlowEffectFactory {
public:
    static std::unique_ptr<GlowEffect> createGlowEffect(GlowType type);
    static std::unique_ptr<OuterGlowEffect> createOuterGlowEffect();
    static std::unique_ptr<InnerGlowEffect> createInnerGlowEffect();
    static std::unique_ptr<AnimatedGlowEffect> createAnimatedGlowEffect();
    static std::unique_ptr<GradientGlowEffect> createGradientGlowEffect();
    static GlowProperties createDefaultProperties(GlowType type);
    static std::vector<GlowType> getSupportedGlowTypes();
    static std::vector<GlowBlendMode> getSupportedBlendModes();
    static std::vector<GlowQuality> getSupportedQualityLevels();
    static std::vector<GlowColorMode> getSupportedColorModes();
    static GlowType detectBestType(const std::vector<uint8>& image, uint32_t width, uint32_t height);
};

} // namespace RFEffects