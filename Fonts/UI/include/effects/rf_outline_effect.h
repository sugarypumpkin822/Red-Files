#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <array>

namespace RFEffects {

// Outline effect types
enum class OutlineType {
    SOLID,
    DASHED,
    DOTTED,
    DOUBLE,
    CUSTOM
};

// Outline join styles
enum class OutlineJoinStyle {
    MITER,
    ROUND,
    BEVEL,
    SQUARE,
    CUSTOM
};

// Outline cap styles
enum class OutlineCapStyle {
    BUTT,
    ROUND,
    SQUARE,
    CUSTOM
};

// Outline rendering modes
enum class OutlineRenderMode {
    PIXEL_PERFECT,
    ANTI_ALIASED,
    SUBPIXEL,
    CUSTOM
};

// Outline quality levels
enum class OutlineQuality {
    LOW,
    MEDIUM,
    HIGH,
    ULTRA,
    CUSTOM
};

// Outline color modes
enum class OutlineColorMode {
    SOLID,
    GRADIENT,
    MULTI_COLOR,
    CUSTOM
};

// Outline properties
struct OutlineProperties {
    OutlineType type;
    OutlineJoinStyle joinStyle;
    OutlineCapStyle capStyle;
    OutlineRenderMode renderMode;
    OutlineQuality quality;
    OutlineColorMode colorMode;
    float32 thickness;
    float32 innerThickness;
    float32 outerThickness;
    float32 opacity;
    float32 softness;
    float32 intensity;
    bool enableAntialiasing;
    bool enableDithering;
    bool enableHDR;
    bool enableAnimation;
    bool enablePulsing;
    bool enableColorShift;
    bool enableGradient;
    bool enableMultiColor;
    uint32_t dashPattern;
    uint32_t dotPattern;
    float32 dashLength;
    float32 dotSpacing;
    float32 animationSpeed;
    float32 pulseSpeed;
    float32 colorShiftSpeed;
    std::array<float32, 4> primaryColor;
    std::array<float32, 4> secondaryColor;
    std::array<float32, 4> backgroundColor;
    std::vector<std::array<float32, 4>> gradientColors;
    
    OutlineProperties() : type(OutlineType::SOLID), joinStyle(OutlineJoinStyle::ROUND), capStyle(OutlineCapStyle::ROUND), 
                        renderMode(OutlineRenderMode::ANTI_ALIASED), quality(OutlineQuality::MEDIUM), 
                        colorMode(OutlineColorMode::SOLID), thickness(2.0f), innerThickness(0.0f), outerThickness(0.0f), 
                        opacity(1.0f), softness(1.0f), intensity(1.0f), enableAntialiasing(true), enableDithering(false), 
                        enableHDR(false), enableAnimation(false), enablePulsing(false), enableColorShift(false), 
                        enableGradient(false), enableMultiColor(false), dashPattern(0x5555), dotPattern(0xAAAA), 
                        dashLength(5.0f), dotSpacing(2.0f), animationSpeed(1.0f), 
                        pulseSpeed(1.0f), colorShiftSpeed(1.0f), primaryColor{1.0f, 1.0f, 1.0f, 1.0f}, 
                        secondaryColor{0.5f, 0.5f, 0.5f, 1.0f}, backgroundColor{0.0f, 0.0f, 0.0f, 0.0f} {}
};

// Outline animation state
struct OutlineAnimationState {
    float32 time;
    float32 pulsePhase;
    float32 colorShiftPhase;
    float32 thicknessModulation;
    float32 opacityModulation;
    float32 intensityModulation;
    std::array<float32, 4> currentColor;
    std::array<float32, 4> currentSecondaryColor;
    bool isActive;
    bool isPulsing;
    bool isColorShifting;
    bool isGradientAnimating;
    bool isMultiColorAnimating;
    
    OutlineAnimationState() : time(0.0f), pulsePhase(0.0f), colorShiftPhase(0.0f), thicknessModulation(1.0f), 
                           opacityModulation(1.0f), intensityModulation(1.0f), 
                           currentColor{1.0f, 1.0f, 1.0f, 1.0f}, 
                           currentSecondaryColor{0.5f, 0.5f, 0.5f, 1.0f}, isActive(false), 
                           isPulsing(false), isColorShifting(false), isGradientAnimating(false), isMultiColorAnimating(false) {}
};

// Outline statistics
struct OutlineStatistics {
    uint32_t totalRenders;
    uint32_t successfulRenders;
    uint32_t failedRenders;
    float32 averageRenderTime;
    float32 averageThickness;
    float32 averageOpacity;
    float32 averageIntensity;
    uint64_t totalRenderTime;
    uint32_t sampleCount;
    uint32_t passCount;
    float32 cacheHitRatio;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    
    OutlineStatistics() : totalRenders(0), successfulRenders(0), failedRenders(0), averageRenderTime(0.0f), 
                           averageThickness(0.0f), averageOpacity(0.0f), averageIntensity(0.0f), 
                           totalRenderTime(0), sampleCount(0), passCount(0), cacheHitRatio(0.0f), cacheHits(0), 
                           cacheMisses(0) {}
};

// Outline effect
class OutlineEffect {
public:
    OutlineEffect();
    virtual ~OutlineEffect() = default;
    
    // Effect management
    void initialize(const OutlineProperties& properties);
    void reset();
    void shutdown();
    bool isInitialized() const;
    
    // Properties
    void setProperties(const OutlineProperties& properties);
    const OutlineProperties& getProperties() const;
    void setType(OutlineType type);
    void setJoinStyle(OutlineJoinStyle joinStyle);
    void setCapStyle(OutlineCapStyle capStyle);
    void setRenderMode(OutlineRenderMode renderMode);
    void setQuality(OutlineQuality quality);
    void setColorMode(OutlineColorMode colorMode);
    void setThickness(float32 thickness);
    void setInnerThickness(float32 innerThickness);
    void setOuterThickness(float32 outerThickness);
    void setOpacity(float32 opacity);
    void setSoftness(float32 softness);
    void setIntensity(float32 intensity);
    
    // Pattern settings
    void setDashPattern(uint32_t pattern);
    void setDotPattern(uint32 pattern);
    void setDashLength(float32 length);
    void setDotSpacing(float32 spacing);
    
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
    void enablePulsing(bool enabled);
    void enableColorShift(bool enabled);
    void enableGradient(bool enabled);
    void enableMultiColor(bool enabled);
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
                std::vector<uint8>& outputImage, const OutlineAnimationState& animationState) const;
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
    bool validateProperties(const OutlineProperties& properties) const;
    bool validateImage(const std::vector<uint8>& image, uint32_t width, uint32_t height) const;
    bool validateDimensions(uint32_t width, uint32_t height) const;
    
    // Animation state
    OutlineAnimationState getAnimationState() const;
    void updateAnimationState(float32 deltaTime);
    void setAnimationState(const OutlineAnimationState& state);
    
    // Performance
    void enableCache(bool enabled);
    bool isCacheEnabled() const;
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCacheUsage() const;
    void clearCache();
    
    // Statistics
    OutlineStatistics getStatistics() const;
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
    void addOutlineEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void removeOutlineEffectEventListener(const std::string& eventType, std::function<void()> callback);
    void clearOutlineEffectEventListeners();
    
    // Utility methods
    void cloneFrom(const OutlineEffect& other);
    virtual std::unique_ptr<OutlineEffect> clone() const;
    bool equals(const OutlineEffect& other) const;
    size_t hash() const;
    std::string toString() const;
    
protected:
    // Protected members
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    OutlineProperties properties_;
    OutlineAnimationState animationState_;
    OutlineStatistics statistics_;
    
    bool isInitialized_;
    bool statisticsEnabled_;
    bool debuggingEnabled_;
    bool cacheEnabled_;
    bool animationEnabled_;
    bool pulsingEnabled_;
    bool colorShiftEnabled_;
    bool gradientEnabled_;
    bool multiColorEnabled_;
    
    size_t currentCacheSize_;
    std::vector<std::string> debugMessages_;
    
    // Protected helper methods
    virtual void triggerOutlineEffectEvent(const std::string& eventType);
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage) const;
    virtual bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                               std::vector<uint8>& outputImage, const OutlineAnimationState& animationState) const;
    
    // Rendering helpers
    virtual bool renderSolidOutline(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                   std::vector<uint8>& outputImage) const;
    virtual bool renderDashedOutline(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                    std::vector<uint8>& outputImage) const;
    virtual bool renderDottedOutline(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                    std::vector<uint8>& outputImage) const;
    virtual bool renderDoubleOutline(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                                    std::vector<uint8>& outputImage) const;
    
    // Color helpers
    virtual std::array<float32, 4> calculateOutlineColor(const std::array<float32, 4>& baseColor, 
                                                       const OutlineAnimationState& animationState) const;
    virtual std::array<float32, 4> interpolateColor(const std::array<float32, 4>& color1, 
                                                   const std::array<float32, 4>& color2, float32 t) const;
    virtual std::array<float32, 4> calculateGradientColor(float32 distance, float32 maxDistance) const;
    virtual std::array<float32, 4> calculateMultiColor(float32 phase) const;
    
    // Animation helpers
    virtual void updateAnimationStateInternal(float32 deltaTime);
    virtual float32 calculatePulseModulation(float32 time) const;
    virtual float32 calculateColorShiftPhase(float32 time) const;
    virtual std::array<float32, 4> calculateAnimatedColor(const std::array<float32, 4>& baseColor, 
                                                           const OutlineAnimationState& animationState) const;
    
    // Quality helpers
    virtual uint32_t calculateSampleCount(OutlineQuality quality) const;
    virtual uint32_t calculatePassCount(OutlineQuality quality) const;
    virtual float32 calculateOutlineWidth(float32 thickness, float32 softness) const;
    virtual std::vector<float32> generateDashPattern(float32 dashLength, float32 dotSpacing) const;
    virtual std::vector<float32> generateDotPattern(float32 dotSpacing) const;
    
    // Validation helpers
    virtual bool validatePropertiesInternal(const OutlineProperties& properties) const;
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
    virtual std::string formatProperties(const OutlineProperties& properties) const;
};

// Specialized outline effects
class SolidOutlineEffect : public OutlineEffect {
public:
    SolidOutlineEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<OutlineEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class DashedOutlineEffect : public OutlineEffect {
public:
    DashedOutlineEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<OutlineEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class DottedOutlineEffect : public OutlineEffect {
public:
    DottedOutlineEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<OutlineEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class DoubleOutlineEffect : public OutlineEffect {
public:
    DoubleOutlineEffect();
    bool render(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                std::vector<uint8>& outputImage) const override;
    std::unique_ptr<OutlineEffect> clone() const override;
    
protected:
    bool renderInternal(const std::vector<uint8>& sourceImage, uint32_t width, uint32_t height, 
                       std::vector<uint8>& outputImage) const override;
};

class AnimatedOutlineEffect : public OutlineEffect {
public:
    AnimatedOutlineEffect();
    void enableAnimation(bool enabled) override;
    void startAnimation() override;
    void stopAnimation() override;
    void updateAnimationState(float32 deltaTime) override;
    std::unique_ptr<OutlineEffect> clone() const override;
    
protected:
    void updateAnimationStateInternal(float32 deltaTime) override;
};

class GradientOutlineEffect : public OutlineEffect {
public:
    GradientOutlineEffect();
    void setGradientColors(const std::vector<std::array<float32, 4>>& colors);
    std::vector<std::array<float32, 4>> getGradientColors() const;
    std::unique_ptr<OutlineEffect> clone() const override;
    
protected:
    std::array<float32, 4> calculateGradientColor(float32 distance, float32 maxDistance) const override;
    
private:
    std::vector<std::array<float32, 4>> gradientColors_;
};

// Outline effect factory
class OutlineEffectFactory {
public:
    static std::unique_ptr<OutlineEffect> createOutlineEffect(OutlineType type);
    static std::unique_ptr<SolidOutlineEffect> createSolidOutlineEffect();
    static std::unique_ptr<DashedOutlineEffect> createDashedOutlineEffect();
    static std::unique_ptr<DottedOutlineEffect> createDottedOutlineEffect();
    static std::unique_ptr<DoubleOutlineEffect> createDoubleOutlineEffect();
    static std::unique_ptr<AnimatedOutlineEffect> createAnimatedOutlineEffect();
    static std::unique_ptr<GradientOutlineEffect> createGradientOutlineEffect();
    static OutlineProperties createDefaultProperties(OutlineType type);
    static std::vector<OutlineType> getSupportedOutlineTypes();
    static std::vector<OutlineJoinStyle> getSupportedJoinStyles();
    static std::vector<OutlineCapStyle> getSupportedCapStyles();
    static std::vector<OutlineRenderMode> getSupportedRenderModes();
    static std::vector<OutlineQuality> getSupportedQualityLevels();
    static std::vector<OutlineColorMode> getSupportedColorModes();
    static OutlineType detectBestType(const std::vector<uint8>& image, uint32_t width, uint32_t height);
};

} // namespace RFEffects