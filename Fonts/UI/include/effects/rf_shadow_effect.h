#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFEffects {

// Shadow effect types
enum class ShadowType {
    DROP_SHADOW,
    INNER_SHADOW,
    TEXT_SHADOW,
    BOX_SHADOW,
    CUSTOM
};

// Shadow rendering modes
enum class ShadowRenderMode {
    HARD,
    SOFT,
    GAUSSIAN,
    BOX,
    CUSTOM
};

// Shadow quality levels
enum class ShadowQuality {
    LOW,
    MEDIUM,
    HIGH,
    ULTRA,
    CUSTOM
};

// Shadow color modes
enum class ShadowColorMode {
    SOLID,
    GRADIENT,
    MULTI_COLOR,
    CUSTOM
};

// Shadow properties
struct ShadowProperties {
    ShadowType type;
    ShadowRenderMode renderMode;
    ShadowQuality quality;
    ShadowColorMode colorMode;
    float32 offsetX;
    float32 offsetY;
    float32 blurRadius;
    float32 spreadRadius;
    float32 opacity;
    float32 angle;
    float32 distance;
    float32 softness;
    float32 falloff;
    float32 intensity;
    bool enableAntialiasing;
    bool enableDithering;
    bool enableHDR;
    bool enableAnimation;
    bool enablePulsing;
    bool enableColorShift;
    bool enableMotionBlur;
    uint32_t sampleCount;
    uint32_t passCount;
    float32 animationSpeed;
    float32 pulseSpeed;
    float32 colorShiftSpeed;
    float32 motionBlurStrength;
    std::array<float32, 4> primaryColor;
    std::array<float32, 4> secondaryColor;
    std::array<float32, 4> backgroundColor;
    
    ShadowProperties() : type(ShadowType::DROP_SHADOW), renderMode(ShadowRenderMode::SOFT), quality(ShadowQuality::MEDIUM), 
                         colorMode(ShadowColorMode::SOLID), offsetX(5.0f), offsetY(5.0f), blurRadius(10.0f), 
                         spreadRadius(0.0f), opacity(0.5f), angle(45.0f), distance(7.07f), softness(1.0f), 
                         falloff(1.0f), intensity(1.0f), enableAntialiasing(true), enableDithering(false), 
                         enableHDR(false), enableAnimation(false), enablePulsing(false), enableColorShift(false), 
                         enableMotionBlur(false), sampleCount(8), passCount(1), animationSpeed(1.0f), 
                         pulseSpeed(1.0f), colorShiftSpeed(1.0f), motionBlurStrength(0.5f), 
                         primaryColor{0.0f, 0.0f, 0.0f, 0.5f}, secondaryColor{0.0f, 0.0f, 0.0f, 0.3f}, 
                         backgroundColor{1.0f, 1.0f, 1.0f, 0.0f} {}
};

// Shadow animation state
struct ShadowAnimationState {
    float32 time;
    float32 pulsePhase;
    float32 colorShiftPhase;
    float32 offsetXModulation;
    float32 offsetYModulation;
    float32 blurRadiusModulation;
    float32 opacityModulation;
    float32 angleModulation;
    std::array<float32, 4> currentColor;
    bool isActive;
    bool isPulsing;
    bool isColorShifting;
    bool isMotionBlurring;
    
    ShadowAnimationState() : time(0.0f), pulsePhase(0.0f), colorShiftPhase(0.0f), offsetXModulation(1.0f), 
                           offsetYModulation(1.0f), blurRadiusModulation(1.0f), opacityModulation(1.0f), 
                           angleModulation(1.0f), currentColor{0.0f, 0.0f, 0.0f, 0.5f}, isActive(false), 
                           isPulsing(false), isColorShifting(false), isMotionBlurring(false) {}
};

// Shadow statistics
struct ShadowStatistics {
    uint32_t totalRenders;
    uint32_t successfulRenders;
    uint32_t failedRenders;
    float32 averageRenderTime;
    float32 averageBlurRadius;
    float32 averageOpacity;
    float32 averageOffsetX;
    float32 averageOffsetY;
    uint64_t totalRenderTime;
    uint32_t sampleCount;
    uint32_t passCount;
    float32 cacheHitRatio;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    
    ShadowStatistics() : totalRenders(0), successfulRenders(0), failedRenders(0), averageRenderTime(0.0f), 
                         averageBlurRadius(0.0f), averageOpacity(0.0f), averageOffsetX(0.0f), averageOffsetY(0.0f), 
                         totalRenderTime(0), sampleCount(0), passCount(0), cacheHitRatio(0.0f), cacheHits(0), 
                         cacheMisses(0) {}
};

// Shadow effect
class ShadowEffect {
public:
    ShadowEffect();
    virtual ~ShadowEffect() = default;
    
    // Effect management
    void initialize(const ShadowProperties& properties);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Properties
    void setProperties(const ShadowProperties& properties);
    const ShadowProperties& getProperties() const;
    void setType(ShadowType type);
    void setRenderMode(ShadowRenderMode renderMode);
    void setQuality(ShadowQuality quality);
    void setColorMode(ShadowColorMode colorMode);
    void setOffsetX(float32 offsetX);
    void setOffsetY(float32 offsetY);
    void setBlurRadius(float32 blurRadius);
    void setSpreadRadius(float32 spreadRadius);
    void setOpacity(float32 opacity);
    void setAngle(float32 angle);
    void setDistance(float32 distance);
    void setSoftness(float32 softness);
    void setFalloff(float32 falloff);
    void setIntensity(float32 intensity);
    
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
    void enableMotionBlur(bool enabled);
    void setAnimationSpeed(float32 speed);
    void setPulseSpeed(float32 speed);
    void setColorShiftSpeed(float32 speed);
    void setMotionBlurStrength(float32 strength);
    void startAnimation();
    void stopAnimation();
    void pauseAnimation();
    void resumeAnimation();
    bool isAnimationActive() const;
    
    // Rendering
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const;
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage, const ShadowAnimationState& animationState) const;
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
    bool validateProperties(const ShadowProperties& properties) const;
    bool validateImage(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    bool validateDimensions(uint32_t width, uint32_t height) const;
    
    // Animation state
    ShadowAnimationState getAnimationState() const;
    void updateAnimationState(float32 deltaTime);
    void setAnimationState(const ShadowAnimationState& state);
    
    // Performance
    void enableCache(bool enabled);
    bool isCacheEnabled() const;
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    void clearCache();
    
    // Statistics
    ShadowStatistics getStatistics() const;
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
    void addShadowEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void removeShadowEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void clearShadowEffectEventListeners();
    
    // Utility methods
    void cloneFrom(const ShadowEffect& other);
    virtual std::unique_ptr<ShadowEffect> clone() const;
    bool equals(const ShadowEffect& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    ShadowProperties properties_;
    ShadowAnimationState animationState_;
    ShadowStatistics statistics_;
    
    bool isInitialized_;
    bool statisticsEnabled_;
    bool debuggingEnabled_;
    bool cacheEnabled_;
    bool animationEnabled_;
    bool pulsingEnabled_;
    bool colorShiftEnabled_;
    bool motionBlurEnabled_;
    
    size_t currentCacheSize_;
    std::vector<std::string> debugMessages_;
    
    // Protected helper methods
    virtual void triggerShadowEffectEvent(const std::string& eventType);
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage) const;
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage, const ShadowAnimationState& animationState) const;
    
    // Rendering helpers
    virtual bool renderDropShadow(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                  std::vector<uint8>& outputImage) const;
    virtual bool renderInnerShadow(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                  std::vector<uint8>& outputImage) const;
    virtual bool renderTextShadow(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                 std::vector<uint8>& outputImage) const;
    virtual bool renderBoxShadow(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                std::vector<uint8>& outputImage) const;
    
    // Color helpers
    virtual std::array<float32, 4> calculateShadowColor(const std::array<float32, 4>& baseColor, 
                                                       const ShadowAnimationState& animationState) const;
    virtual std::array<float32, 4> interpolateColor(const std::array<float32, 4>& color1, 
                                                   const std::array<float32, 4>& color2, float32 t) const;
    virtual std::array<float32, 4> calculateGradientColor(float32 distance, float32 maxDistance) const;
    virtual std::array<float32, 4> calculateMultiColor(float32 phase) const;
    
    // Animation helpers
    virtual void updateAnimationStateInternal(float32 deltaTime);
    virtual float32 calculatePulseModulation(float32 time) const;
    virtual float32 calculateColorShiftPhase(float32 time) const;
    virtual std::array<float32, 4> calculateAnimatedColor(const std::array<float32, 4>& baseColor, 
                                                           const ShadowAnimationState& animationState) const;
    virtual std::array<float32, 4> calculateMotionBlurColor(const std::array<float32, 4>& baseColor, 
                                                          const ShadowAnimationState& animationState) const;
    
    // Quality helpers
    virtual uint32_t calculateSampleCount(ShadowQuality quality) const;
    virtual uint32_t calculatePassCount(ShadowQuality quality) const;
    virtual float32 calculateKernelWeight(float32 distance, float32 radius, float32 falloff) const;
    virtual std::vector<float32> generateGaussianKernel(float32 radius, float32 sigma) const;
    virtual std::vector<float32> generateBoxKernel(float32 radius) const;
    
    // Validation helpers
    virtual bool validatePropertiesInternal(const ShadowProperties& properties) const;
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
    virtual std::string formatProperties(const ShadowProperties& properties) const;
};

// Specialized shadow effects
class DropShadowEffect : public ShadowEffect {
public:
    DropShadowEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<ShadowEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class InnerShadowEffect : public ShadowEffect {
public:
    InnerShadowEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<ShadowEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class AnimatedShadowEffect : public ShadowEffect {
public:
    AnimatedShadowEffect();
    void enableAnimation(bool enabled) override;
    void startAnimation() override;
    void stopAnimation() override;
    void updateAnimationState(float32 deltaTime) override;
    std::unique_ptr<ShadowEffect> clone() const override;
    
protected:
    void updateAnimationStateInternal(float32 deltaTime) override;
};

class GradientShadowEffect : public ShadowEffect {
public:
    GradientShadowEffect();
    void setGradientColors(const std::vector<std::array<float32, 4>>& colors);
    std::vector<std::array<float32, 4>> getGradientColors() const;
    std::unique_ptr<ShadowEffect> clone() const override;
    
protected:
    std::array<float32, 4> calculateGradientColor(float32 distance, float32 maxDistance) const override;
    
private:
    std::vector<std::array<float32, 4>> gradientColors_;
};

// Shadow effect factory
class ShadowEffectFactory {
public:
    static std::unique_ptr<ShadowEffect> createShadowEffect(ShadowType type);
    static std::unique_ptr<DropShadowEffect> createDropShadowEffect();
    static std::unique_ptr<InnerShadowEffect> createInnerShadowEffect();
    static std::unique_ptr<AnimatedShadowEffect> createAnimatedShadowEffect();
    static std::unique_ptr<GradientShadowEffect> createGradientShadowEffect();
    static ShadowProperties createDefaultProperties(ShadowType type);
    static std::vector<ShadowType> getSupportedShadowTypes();
    static std::vector<ShadowRenderMode> getSupportedRenderModes();
    static std::vector<ShadowQuality> getSupportedQualityLevels();
    static std::vector<ShadowColorMode> getSupportedColorModes();
    static ShadowType detectBestType(const std::vector<uint8>& image, uint32_t width, uint32_t height);
};

} // namespace RFEffects