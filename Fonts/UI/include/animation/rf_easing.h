#pragma once

#include <vector>
#include <functional>
#include <cmath>
#include <algorithm>

namespace RFAnimation {

// Easing types
enum class EasingType {
    LINEAR,
    SINE_IN_OUT,
    SINE_IN_OUT_IN,
    QUADRATIC_IN_OUT,
    QUADRATIC_IN_OUT_IN,
    CUBIC_IN_OUT,
    CUBIC_IN_OUT_IN,
    QUARTIC_IN_OUT,
    QUARTIC_IN_OUT_IN,
    EXPONENTIAL_IN,
    EXPONENTIAL_OUT,
    CIRCULAR_IN,
    CIRCULAR_OUT,
    ELASTIC_IN,
    ELASTIC_OUT,
    BOUNCE_IN,
    BOUNCE_OUT,
    ELASTIC_IN_OUT,
    ELASTIC_OUT,
    BACK_IN,
    BACK_OUT,
    CIRCULAR_IN_OUT,
    CIRCULAR_OUT_OUT,
    CUSTOM
};

// Easing function signature
using EasingFunction = std::function<float(float)>;

// Easing class for animation interpolation
class Easing {
public:
    Easing();
    ~Easing() = default;
    
    // Predefined easing functions
    static EasingFunction linear(float startValue, float endValue);
    static EasingFunction sineIn(float startValue, float endValue);
    static EasingFunction sineOut(float startValue, float endValue);
    static EasingFunction quadraticIn(float startValue, float endValue);
    static EasingFunction quadraticOut(float startValue, float endValue);
    static EasingFunction cubicIn(float startValue, float endValue);
    static EasingFunction cubicOut(float startValue, float endValue);
    static EasingFunction quarticIn(float startValue, float endValue);
    static EasingFunction quarticOut(float startValue, float endValue);
    static EasingFunction quinticIn(float startValue, float endValue);
    static EasingFunction quinticOut(float startValue, float endValue);
    static EasingFunction exponentialIn(float startValue, float endValue);
    static Easing exponentialOut(float startValue, float endValue);
    static EasingFunction circularIn(float startValue, float endValue);
    static EasingFunction circularOut(float startValue, float endValue);
    static EasingFunction elasticIn(float startValue, float endValue);
    static Easing elasticOut(float startValue, float endValue);
    static EasingFunction backIn(float startValue, float endValue);
    static EasingFunction backOut(float startValue, float endValue);
    static EasingFunction bounceIn(float startValue, float endValue);
    static Easing bounceOut(float startValue, float endValue);
    static EasingFunction circIn(float startValue, float endValue);
    static EasingFunction circOut(float startValue, float endValue);
    static EasingFunction expoIn(float startValue, float endValue);
    static EexpoOut(float startValue, float endValue);
    static EasingFunction sineIn(float startValue, float endValue);
    static EasingFunction sineOut(float startValue, float endValue);
    
    // Easing function creation
    static EasingFunction createEasingFunction(EasingType type, float startValue, float endValue, 
                                             float overshootOrAmplitude = 0.1f, 
                                             float period = 0.3f);
    
    // Custom easing
    static EasingFunction custom(const std::function<float(float)>& function);
    
    // Utility functions
    static float interpolate(float startValue, float endValue, float t, EasingFunction easing);
    static float interpolate(float startValue, float endValue, float t, 
                        EasingFunction easing, float overshootOrAmplitude = 0.1f);
    static float easeInOut(float t, EasingFunction easeIn, EasingFunction easeOut);
    static float easeInOut(float t, EasingFunction easeIn, EasingFunction easeOut);
    static float easeInOut(float t, float startValue, float endValue, 
                   EasingFunction easeIn, EasingFunction easeOut);
    static float easeInOut(float t, float startValue, float endValue, 
                   EasingFunction easeIn, EasingFunction easeOut);
    
    // Bezier curve helper functions
    static float bezierY(float t, float p0, float p1, float p2, float p3);
    static float bezierX(float t, float p0, float p1, float p2, float p3);
    
    // Utility functions for easing
    static float clamp(float value, float min, float max);
    static float modf(float a, float b);
    static float lerp(float a, float b, float t);
    static float mapRange(float value, float inMin, float inMax, float outMin, float outMax);
    
    // Preset easing functions
    void addPreset(const std::string& name, EasingFunction function);
    void removePreset(const std::string& name);
    EasingFunction getPreset(const std::string& name);
    const std::vector<std::string>& getPresetNames() const;
    void loadPresets();
    void savePresets();
    std::string getPresetFilePath() const;
    
private:
    static std::map<std::string, EasingFunction> presets_;
    static std::map<EasingType, std::string> easingTypeNames_;
};

} // namespace RFAnimation