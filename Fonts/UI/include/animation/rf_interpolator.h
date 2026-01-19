#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <map>

namespace RFAnimation {

// Interpolation types
enum class InterpolationType {
    LINEAR,
    STEP,
    CUBIC,
    CATMULL_ROM,
    BEZIER,
    HERMITE,
    BSPLINE,
    CUSTOM
};

// Interpolation result structure
template<typename T>
struct InterpolationResult {
    T value;
    T velocity;
    T acceleration;
    bool isValid;
    float time;
    
    InterpolationResult() : value(T()), velocity(T()), acceleration(T()), isValid(false), time(0.0f) {}
    InterpolationResult(const T& val, const T& vel, const T& acc, bool valid, float tm)
        : value(val), velocity(vel), acceleration(acc), isValid(valid), time(tm) {}
};

// Interpolator interface
template<typename T>
class Interpolator {
public:
    virtual ~Interpolator() = default;
    
    // Core interpolation methods
    virtual InterpolationResult interpolate(const T& from, const T& to, float t) = 0.0f) = 0;
    virtual InterpolationResult interpolateWithVelocity(const T& from, const T& to, const T& velocity, 
                                          float t) = 0.0f);
    virtual InterpolationResult interpolateWithAcceleration(const T& from, const T& to, const T& acceleration, 
                                                   float t) = 0.0f);
    
    // Configuration
    virtual void setDuration(float duration);
    virtual void setEasing(EasingFunction easing);
    virtual void setInterpolationType(InterpolationType type);
    
    // State management
    virtual void start(const T& from, const T& to);
    virtual void pause();
    virtual void resume();
    virtual void stop();
    virtual void reset();
    
    // State queries
    virtual bool isPlaying() const;
    virtual bool isPaused() const;
    virtual bool isCompleted() const;
    virtual float getCurrentTime() const;
    virtual float getDuration() const;
    virtual T getCurrentValue() const;
    virtual T getStartValue() const;
    virtual T getEndValue() const;
    virtual T getVelocity() const;
    virtual T getAcceleration() const;
    
    // Event handling
    virtual void addEventListener(const std::string& eventType, 
                              std::function<void(const InterpolationResult&)> callback);
    virtual void removeEventListener(const std::string& eventType, 
                                 std::function<void(const InterpolationResult&)> callback);
    virtual void clearEventListeners();
    
    // Utility methods
    virtual void cloneFrom(const Interpolator& other);
    virtual std::unique_ptr<Interpolator<T>> clone() const;
    
protected:
    // Protected members
    T startValue_;
    T endValue_;
    T currentValue_;
    T velocity_;
    T acceleration_;
    float currentTime_;
    float duration_;
    EasingFunction easing_;
    InterpolationType type_;
    
    bool isPlaying_;
    bool isPaused_;
    bool isCompleted_;
    
    // Protected helper methods
    virtual void calculateVelocity();
    virtual void calculateAcceleration();
    virtual void updateInterpolation(float deltaTime);
    virtual void triggerEvent(const std::string& eventType, 
                             const InterpolationResult& result);
    
    // Helper methods
    virtual T interpolateValue(float t) const;
    virtual T interpolateVelocity(float t) const;
    virtual T interpolateAcceleration(float t) const;
};

// Linear interpolator
template<typename T>
class LinearInterpolator : public Interpolator<T> {
public:
    LinearInterpolator();
    ~LinearInterpolator() = default;
    
    InterpolationResult interpolate(const T& from, const T& to, float t) override;
    InterpolationResult interpolateWithVelocity(const T& from, const T& to, const T& velocity, 
                                          float t) override;
    InterpolationResult interpolateWithAcceleration(const T& from, const T& to, const T& acceleration, 
                                                   float t) override;
};

// Step interpolator
template<typename T>
class StepInterpolator : public Interpolator<T> {
public:
    StepInterpolator();
    ~StepInterpolator() = default;
    
    void setStepSize(int steps);
    void setStepFunction(std::function<T(float, T, float)> stepFunction);
    
    InterpolationResult interpolate(const T& from, const T& to, float t) override;
    InterpolationResult interpolateWithVelocity(const T& from, const T& to, const T& velocity, 
                                          float t) override;
    InterpolationResult interpolateWithAcceleration(const T& from, const T& to, const T& acceleration, 
                                                   float t) override;
    
private:
    int currentStep_;
    int totalSteps_;
    std::function<T(float, T, float)> stepFunction_;
    T stepStartValue_;
    T stepEndValue_;
    float stepProgress_;
};

// Cubic interpolator
template<typename T>
class CubicInterpolator : public Interpolator<T> {
public:
    CubicInterpolator();
    ~CubicInterpolator() = default;
    
    InterpolationResult interpolate(const T& from, const T& to, float t) override;
    InterpolationResult interpolateWithVelocity(const T& from, const T& to, const T& velocity, 
                                          float t) override;
    InterpolationResult interpolateWithAcceleration(const T& from, const T& to, const T& acceleration, 
                                                   float t) override;
    
private:
    T p0, p1, p2, p3;
    T m0, m1, m2, m3;
    T a0, a1, a2, a3;
};

// Catmull-Rom interpolator
template<typename T>
class CatmullRomInterpolator : public Interpolator<T> {
public:
    CatmullRomInterpolator();
    ~CatmullRomInterpolator() = default;
    
    InterpolationResult interpolate(const T& from, const T& to, float t) override;
    InterpolationResult interpolateWithVelocity(const T& from, const T& to, const T& velocity, 
                                          float t) override;
    InterpolationResult interpolateWithAcceleration(const T& const& to, const T& acceleration, 
                                                   float t) override;
    
private:
    std::vector<T> controlPoints_;
    std::vector<float> knots_;
    std::vector<float> knots_;
};

// Bezier interpolator
template<typename T>
class BezierInterpolator : public Interpolator<T> {
public:
    BezierInterpolator();
    ~BezierInterpolator() = default;
    
    void setControlPoints(const std::vector<T>& controlPoints);
    
    InterpolationResult interpolate(const T& from, const T& to, float t) override;
    InterpolationResult interpolateWithVelocity(const T& from, const T& to, const T& velocity, 
                                          float t) override;
    InterpolationResult interpolateWithAcceleration(const T& const to, const T& acceleration, 
                                                   float t) override;
    
private:
    std::vector<T> controlPoints_;
    std::vector<float> coefficients_;
    
    T calculateBezierPoint(float t, int i, int n, const std::vector<T>& controlPoints);
    T deCasteljauBisector(float t, int i, int n, const std::vector<T>& controlPoints);
};

// Hermite interpolator
template<typename T>
class HermiteInterpolator : public Interpolator<T> {
public:
    HermiteInterpolator();
    ~HermiteInterpolator() = default;
    
    void setTangents(const std::vector<T>& inTangents, const std::vector<T>& outTangents);
    
    InterpolationResult interpolate(const T& from, const T& to, float t) override;
    InterpolationResult interpolateWithVelocity(const T& from, const T& to, const T& velocity, 
                                          float t) override;
    InterpolationResult interpolateWithAcceleration(const T& const to, const T& acceleration, 
                                                   float t) override;
    
private:
    std::vector<T> points_;
    std::vector<T> inTangents_;
    std::vector<T> outTangents_;
    T tension_;
    T bias_;
};

// B-spline interpolator
template<typename T>
class BSplineInterpolator : public Interpolator<T> {
public:
    BSplineInterpolator();
    ~BSplineInterpolator() = default;
    
    void setControlPoints(const std::vector<T>& controlPoints);
    void setTension(float tension);
    void setBias(float bias);
    
    InterpolationResult interpolate(const T& from, const T& to, float t) override;
    InterpolationResult interpolateWithVelocity(const T& from, const T& to, const T& velocity, 
                                          float t) override;
    InterpolationResult interpolateWithAcceleration(const T& const to, const T& acceleration, 
                                                   float t) override;
    
private:
    std::vector<T> controlPoints_;
    std::vector<float> knots_;
    std::vector<float> coefficients_;
    float tension_;
    float bias_;
};

// Custom interpolator
template<typename T>
class CustomInterpolator : public Interpolator<T> {
public:
    CustomInterpolator();
    ~CustomInterpolator() = default;
    
    void setInterpolationFunction(std::function<InterpolationResult&> function);
    
    InterpolationResult interpolate(const T& from, const T& to, float t) override;
    InterpolationResult interpolateWithVelocity(const T& from, const T& to, const T& velocity, 
                                          float t) override;
    InterpolationResult interpolateWithAcceleration(const T& const to, const T& acceleration, 
                                                   float t) override;
    
private:
    std::function<InterpolationResult&> customFunction_;
};

// Interpolator factory
class InterpolatorFactory {
public:
    template<typename T>
    static std::unique_ptr<Interpolator<T>> createInterpolator(InterpolationType type);
    static std::vector<std::string> getAvailableTypes();
    template<typename T>
    static void registerInterpolator(const std::string& name, 
                                   std::function<std::unique_ptr<Interpolator<T>> creator);
};

} // namespace RFAnimation