#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <cmath>

namespace RFUtils {

// Forward declarations
class Vector3;
class Vector4;
class Matrix3;
class Matrix4;

// Quaternion class
class Quaternion {
public:
    float32 x, y, z, w;
    
    // Constructors
    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Quaternion(float32 x, float32 y, float32 z, float32 w) : x(x), y(y), z(z), w(w) {}
    Quaternion(const float32* elements) : x(elements[0]), y(elements[1]), z(elements[2]), w(elements[3]) {}
    Quaternion(const std::array<float32, 4>& elements) : x(elements[0]), y(elements[1]), z(elements[2]), w(elements[3]) {}
    Quaternion(const Vector3& axis, float32 angle);
    Quaternion(const Vector3& euler);
    Quaternion(const Matrix3& matrix);
    Quaternion(const Matrix4& matrix);
    Quaternion(const Vector4& vector); // x, y, z, w
    
    // Copy constructor
    Quaternion(const Quaternion& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
    
    // Assignment operator
    Quaternion& operator=(const Quaternion& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }
    
    // Component access
    float32& operator[](size_t index) { return (&x)[index]; }
    const float32& operator[](size_t index) const { return (&x)[index]; }
    
    // Array access
    float32* data() { return &x; }
    const float32* data() const { return &x; }
    
    // Vector access
    Vector3 getVector() const;
    void setVector(const Vector3& vector);
    float32 getScalar() const { return w; }
    void setScalar(float32 scalar) { w = scalar; }
    
    // Axis-angle access
    Vector3 getAxis() const;
    float32 getAngle() const;
    void setAxisAngle(const Vector3& axis, float32 angle);
    
    // Comparison operators
    bool operator==(const Quaternion& other) const;
    bool operator!=(const Quaternion& other) const;
    
    // Arithmetic operators
    Quaternion operator+(const Quaternion& other) const;
    Quaternion operator-(const Quaternion& other) const;
    Quaternion operator*(const Quaternion& other) const;
    Quaternion operator*(float32 scalar) const;
    Quaternion operator/(float32 scalar) const;
    
    // Assignment arithmetic operators
    Quaternion& operator+=(const Quaternion& other);
    Quaternion& operator-=(const Quaternion& other);
    Quaternion& operator*=(const Quaternion& other);
    Quaternion& operator*=(float32 scalar);
    Quaternion& operator/=(float32 scalar);
    
    // Unary operators
    Quaternion operator+() const;
    Quaternion operator-() const;
    
    // Quaternion operations
    Quaternion conjugate() const;
    Quaternion inverse() const;
    Quaternion normalize() const;
    Quaternion negate() const;
    
    float32 dot(const Quaternion& other) const;
    float32 length() const;
    float32 lengthSquared() const;
    
    bool isNormalized() const;
    bool isIdentity() const;
    bool isPure() const;
    bool isValid() const;
    
    // Rotation operations
    Quaternion rotate(const Quaternion& other) const;
    Quaternion rotate(const Vector3& axis, float32 angle) const;
    Quaternion rotateX(float32 angle) const;
    Quaternion rotateY(float32 angle) const;
    Quaternion rotateZ(float32 angle) const;
    
    // Interpolation
    static Quaternion lerp(const Quaternion& a, const Quaternion& b, float32 t);
    static Quaternion slerp(const Quaternion& a, const Quaternion& b, float32 t);
    static Quaternion nlerp(const Quaternion& a, const Quaternion& b, float32 t);
    
    // Look rotation
    static Quaternion lookRotation(const Vector3& forward, const Vector3& up = Vector3::up());
    static Quaternion lookTo(const Vector3& direction, const Vector3& up = Vector3::up());
    static Quaternion fromToRotation(const Vector3& from, const Vector3& to);
    
    // Static factory methods
    static Quaternion identity();
    static Quaternion zero();
    
    // Axis-angle constructors
    static Quaternion fromAxisAngle(const Vector3& axis, float32 angle);
    static Quaternion fromEuler(const Vector3& euler);
    static Quaternion fromEuler(float32 x, float32 y, float32 z);
    static Quaternion fromMatrix(const Matrix3& matrix);
    static Quaternion fromMatrix(const Matrix4& matrix);
    
    // Direction constructors
    static Quaternion fromDirection(const Vector3& direction);
    static Quaternion fromForwardUp(const Vector3& forward, const Vector3& up);
    static Quaternion fromLookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
    
    // Swing-twist decomposition
    Quaternion swingTwist(const Vector3& twistAxis) const;
    Quaternion getSwing(const Vector3& twistAxis) const;
    Quaternion getTwist(const Vector3& twistAxis) const;
    
    // Transform operations
    Vector3 transformPoint(const Vector3& point) const;
    Vector3 transformVector(const Vector3& vector) const;
    Vector3 transformDirection(const Vector3& direction) const;
    
    // Conversion operations
    Vector3 toEuler() const;
    Vector3 toAxisAngle() const;
    Matrix3 toMatrix3() const;
    Matrix4 toMatrix4() const;
    Vector4 toVector4() const;
    
    // Utility methods
    Quaternion abs() const;
    Quaternion round() const;
    Quaternion floor() const;
    Quaternion ceil() const;
    Quaternion min(const Quaternion& other) const;
    Quaternion max(const Quaternion& other) const;
    Quaternion clamp(const Quaternion& min, const Quaternion& max) const;
    
    // Angle operations
    float32 angle() const;
    float32 angleTo(const Quaternion& other) const;
    float32 angleBetween(const Quaternion& a, const Quaternion& b);
    
    // Distance operations
    float32 distanceTo(const Quaternion& other) const;
    float32 distance(const Quaternion& a, const Quaternion& b);
    
    // Power operations
    Quaternion pow(float32 power) const;
    Quaternion log() const;
    Quaternion exp() const;
    
    // String conversion
    std::string toString() const;
    
    // Constants
    static constexpr float32 EPSILON = 0.0001f;
};

// Global operators for scalar operations
inline Quaternion operator*(float32 scalar, const Quaternion& quaternion) {
    return quaternion * scalar;
}

// Utility functions
inline Quaternion abs(const Quaternion& quaternion) {
    return quaternion.abs();
}

inline Quaternion min(const Quaternion& a, const Quaternion& b) {
    return a.min(b);
}

inline Quaternion max(const Quaternion& a, const Quaternion& b) {
    return a.max(b);
}

inline Quaternion clamp(const Quaternion& value, const Quaternion& min, const Quaternion& max) {
    return value.clamp(min, max);
}

inline Quaternion lerp(const Quaternion& a, const Quaternion& b, float32 t) {
    return Quaternion::lerp(a, b, t);
}

inline Quaternion slerp(const Quaternion& a, const Quaternion& b, float32 t) {
    return Quaternion::slerp(a, b, t);
}

inline Quaternion nlerp(const Quaternion& a, const Quaternion& b, float32 t) {
    return Quaternion::nlerp(a, b, t);
}

// Quaternion utilities namespace
namespace QuaternionUtils {
    // Quaternion operations
    Quaternion conjugate(const Quaternion& quaternion);
    Quaternion inverse(const Quaternion& quaternion);
    Quaternion normalize(const Quaternion& quaternion);
    Quaternion negate(const Quaternion& quaternion);
    
    float32 dot(const Quaternion& a, const Quaternion& b);
    float32 length(const Quaternion& quaternion);
    float32 lengthSquared(const Quaternion& quaternion);
    
    bool isNormalized(const Quaternion& quaternion);
    bool isIdentity(const Quaternion& quaternion);
    bool isPure(const Quaternion& quaternion);
    bool isValid(const Quaternion& quaternion);
    
    // Quaternion comparison
    bool equals(const Quaternion& a, const Quaternion& b, float32 epsilon = Quaternion::EPSILON);
    bool approximatelyEquals(const Quaternion& a, const Quaternion& b, float32 epsilon = Quaternion::EPSILON);
    
    // Quaternion interpolation
    Quaternion interpolateLinear(const Quaternion& a, const Quaternion& b, float32 t);
    Quaternion interpolateSpherical(const Quaternion& a, const Quaternion& b, float32 t);
    Quaternion interpolateNormalized(const Quaternion& a, const Quaternion& b, float32 t);
    
    // Quaternion smoothing
    Quaternion smoothStep(const Quaternion& a, const Quaternion& b, float32 t);
    Quaternion smootherStep(const Quaternion& a, const Quaternion& b, float32 t);
    
    // Quaternion validation
    bool validate(const Quaternion& quaternion);
    bool isFinite(const Quaternion& quaternion);
    bool hasNaN(const Quaternion& quaternion);
    bool hasInfinity(const Quaternion& quaternion);
    
    // Quaternion debugging
    std::string toString(const Quaternion& quaternion);
    void debugPrint(const Quaternion& quaternion);
    void debugPrintFormatted(const Quaternion& quaternion);
    
    // Quaternion conversion
    Vector3 toEuler(const Quaternion& quaternion);
    Vector3 toAxisAngle(const Quaternion& quaternion);
    Matrix3 toMatrix3(const Quaternion& quaternion);
    Matrix4 toMatrix4(const Quaternion& quaternion);
    Vector4 toVector4(const Quaternion& quaternion);
    
    Quaternion fromEuler(const Vector3& euler);
    Quaternion fromEuler(float32 x, float32 y, float32 z);
    Quaternion fromAxisAngle(const Vector3& axis, float32 angle);
    Quaternion fromMatrix(const Matrix3& matrix);
    Quaternion fromMatrix(const Matrix4& matrix);
    Quaternion fromVector4(const Vector4& vector);
    
    // Quaternion transformations
    Quaternion rotate(const Quaternion& quaternion, const Quaternion& rotation);
    Quaternion rotate(const Quaternion& quaternion, const Vector3& axis, float32 angle);
    Quaternion rotateX(const Quaternion& quaternion, float32 angle);
    Quaternion rotateY(const Quaternion& quaternion, float32 angle);
    Quaternion rotateZ(const Quaternion& quaternion, float32 angle);
    
    // Quaternion look operations
    Quaternion lookRotation(const Vector3& forward, const Vector3& up = Vector3::up());
    Quaternion lookTo(const Vector3& direction, const Vector3& up = Vector3::up());
    Quaternion fromToRotation(const Vector3& from, const Vector3& to);
    Quaternion lookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
    
    // Quaternion angle operations
    float32 angle(const Quaternion& quaternion);
    float32 angleTo(const Quaternion& a, const Quaternion& b);
    float32 angleBetween(const Quaternion& a, const Quaternion& b);
    
    // Quaternion distance operations
    float32 distanceTo(const Quaternion& a, const Quaternion& b);
    float32 distance(const Quaternion& a, const Quaternion& b);
    
    // Quaternion power operations
    Quaternion pow(const Quaternion& quaternion, float32 power);
    Quaternion log(const Quaternion& quaternion);
    Quaternion exp(const Quaternion& quaternion);
    
    // Quaternion swing-twist decomposition
    Quaternion swingTwist(const Quaternion& quaternion, const Vector3& twistAxis);
    Quaternion getSwing(const Quaternion& quaternion, const Vector3& twistAxis);
    Quaternion getTwist(const Quaternion& quaternion, const Vector3& twistAxis);
    
    // Quaternion animation
    class QuaternionAnimator {
    public:
        QuaternionAnimator();
        QuaternionAnimator(const Quaternion& start, const Quaternion& end, float32 duration);
        
        void setStart(const Quaternion& start);
        void setEnd(const Quaternion& end);
        void setDuration(float32 duration);
        void setLoop(bool loop);
        void setPingPong(bool pingPong);
        void setInterpolationMode(int mode); // 0=Linear, 1=Spherical, 2=Normalized
        
        Quaternion update(float32 deltaTime);
        Quaternion getValue(float32 t) const;
        
        bool isPlaying() const;
        bool isFinished() const;
        float32 getProgress() const;
        float32 getTime() const;
        float32 getDuration() const;
        
        void play();
        void pause();
        void stop();
        void reset();
        
    private:
        Quaternion start_;
        Quaternion end_;
        float32 duration_;
        float32 time_;
        bool playing_;
        bool loop_;
        bool pingPong_;
        bool forward_;
        int interpolationMode_;
    };
    
    // Quaternion cache
    class QuaternionCache {
    public:
        QuaternionCache(size_t maxSize = 1000);
        
        void put(const std::string& key, const Quaternion& quaternion);
        Quaternion get(const std::string& key) const;
        bool contains(const std::string& key) const;
        void remove(const std::string& key);
        void clear();
        
        size_t size() const;
        size_t maxSize() const;
        void setMaxSize(size_t maxSize);
        
        double getHitRate() const;
        size_t getHitCount() const;
        size_t getMissCount() const;
        
    private:
        struct CacheEntry {
            Quaternion quaternion;
            size_t accessCount;
            uint64_t lastAccessTime;
        };
        
        std::map<std::string, CacheEntry> cache_;
        size_t maxSize_;
        size_t hitCount_;
        size_t missCount_;
        
        void evictLeastRecentlyUsed();
    };
    
    // Quaternion pool
    class QuaternionPool {
    public:
        QuaternionPool(size_t initialSize = 100);
        
        Quaternion* acquire();
        void release(Quaternion* quaternion);
        void clear();
        
        size_t getPoolSize() const;
        size_t getUsedCount() const;
        size_t getAvailableCount() const;
        
    private:
        std::vector<std::unique_ptr<Quaternion>> pool_;
        std::vector<Quaternion*> available_;
        std::vector<Quaternion*> used_;
        
        void expandPool(size_t newSize);
    };
    
    // Quaternion stack
    class QuaternionStack {
    public:
        QuaternionStack();
        
        void push(const Quaternion& quaternion);
        void pop();
        void clear();
        
        Quaternion top() const;
        size_t size() const;
        bool empty() const;
        
        Quaternion multiply(const Quaternion& quaternion);
        Quaternion rotate(const Vector3& axis, float32 angle);
        Quaternion rotateX(float32 angle);
        Quaternion rotateY(float32 angle);
        Quaternion rotateZ(float32 angle);
        
    private:
        std::vector<Quaternion> stack_;
    };
    
    // Quaternion builder
    class QuaternionBuilder {
    public:
        QuaternionBuilder();
        
        QuaternionBuilder& identity();
        QuaternionBuilder& zero();
        QuaternionBuilder& axisAngle(const Vector3& axis, float32 angle);
        QuaternionBuilder& euler(const Vector3& euler);
        QuaternionBuilder& euler(float32 x, float32 y, float32 z);
        QuaternionBuilder& matrix(const Matrix3& matrix);
        QuaternionBuilder& matrix(const Matrix4& matrix);
        
        QuaternionBuilder& multiply(const Quaternion& quaternion);
        QuaternionBuilder& rotate(const Vector3& axis, float32 angle);
        QuaternionBuilder& rotateX(float32 angle);
        QuaternionBuilder& rotateY(float32 angle);
        QuaternionBuilder& rotateZ(float32 angle);
        QuaternionBuilder& normalize();
        QuaternionBuilder& conjugate();
        QuaternionBuilder& inverse();
        
        Quaternion build() const;
        operator Quaternion() const;
        
    private:
        Quaternion quaternion_;
    };
    
    // Quaternion utilities
    Quaternion createBillboard(const Vector3& position, const Vector3& cameraPosition, const Vector3& cameraUp, const Vector3& cameraRight);
    Quaternion createConstrainedBillboard(const Vector3& position, const Vector3& cameraPosition, const Vector3& cameraUp, const Vector3& cameraRight, const Vector3& objectAxis);
    
    Quaternion createFromDirection(const Vector3& direction);
    Quaternion createFromForwardUp(const Vector3& forward, const Vector3& up);
    Quaternion createFromLookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
    
    Quaternion createFromToRotation(const Vector3& from, const Vector3& to);
    Quaternion createFromAxisAngle(const Vector3& axis, float32 angle);
    Quaternion createFromEuler(const Vector3& euler);
    Quaternion createFromEuler(float32 x, float32 y, float32 z);
    
    // Quaternion analysis
    struct QuaternionAnalysis {
        bool isIdentity;
        bool isNormalized;
        bool isPure;
        bool isValid;
        float32 length;
        float32 lengthSquared;
        float32 angle;
        Vector3 axis;
        Vector3 euler;
        float32 dot;
        float32 distance;
    };
    
    QuaternionAnalysis analyze(const Quaternion& quaternion);
    QuaternionAnalysis analyze(const Quaternion& a, const Quaternion& b);
    
    // Quaternion optimization
    Quaternion optimize(const Quaternion& quaternion);
    Quaternion compress(const Quaternion& quaternion);
    Quaternion decompress(const Quaternion& quaternion);
    
    // Quaternion validation
    bool validate(const Quaternion& quaternion);
    bool validateRotation(const Quaternion& quaternion);
    bool validateInterpolation(const Quaternion& a, const Quaternion& b, float32 t);
    
    // Quaternion debugging
    void debugPrintQuaternion(const Quaternion& quaternion);
    void debugPrintQuaternionFormatted(const Quaternion& quaternion);
    void debugPrintQuaternionAnalysis(const Quaternion& quaternion);
    void debugPrintQuaternionAnalysis(const Quaternion& a, const Quaternion& b);
    
    // Quaternion math utilities
    Quaternion slerpShortestPath(const Quaternion& a, const Quaternion& b, float32 t);
    Quaternion slerpUnclamped(const Quaternion& a, const Quaternion& b, float32 t);
    Quaternion squad(const Quaternion& q0, const Quaternion& q1, const Quaternion& q2, const Quaternion& q3, float32 t);
    
    // Quaternion integration
    Quaternion integrate(const Quaternion& rotation, const Vector3& angularVelocity, float32 deltaTime);
    Quaternion integrateGyroscope(const Quaternion& rotation, const Vector3& gyro, float32 deltaTime);
    Quaternion integrateAccelerometer(const Quaternion& rotation, const Vector3& accel, float32 deltaTime);
    
    // Quaternion filtering
    Quaternion complementaryFilter(const Quaternion& gyro, const Quaternion& accel, float32 alpha);
    Quaternion kalmanFilter(const Quaternion& rotation, const Vector3& measurement, const Vector3& prediction, float32 dt);
    Quaternion lowPassFilter(const Quaternion& current, const Quaternion& previous, float32 alpha);
    Quaternion highPassFilter(const Quaternion& current, const Quaternion& previous, float32 alpha);
    
    // Quaternion constraints
    Quaternion constrainToAxis(const Quaternion& quaternion, const Vector3& axis);
    Quaternion constrainToPlane(const Quaternion& quaternion, const Vector3& normal);
    Quaternion constrainToCone(const Quaternion& quaternion, const Vector3& axis, float32 maxAngle);
    Quaternion constrainToSwing(const Quaternion& quaternion, const Vector3& twistAxis, float32 maxSwingAngle);
    
    // Quaternion blending
    Quaternion blend(const Quaternion& a, const Quaternion& b, float32 weight);
    Quaternion blendWeighted(const std::vector<Quaternion>& quaternions, const std::vector<float32>& weights);
    Quaternion blendAdditive(const Quaternion& base, const Quaternion& additive, float32 weight);
    
    // Quaternion IK (Inverse Kinematics)
    class IKSolver {
    public:
        IKSolver();
        
        Quaternion solveCCD(const std::vector<Vector3>& joints, const std::vector<float32>& lengths, 
                         const Vector3& target, size_t maxIterations = 10, float32 tolerance = 0.01f);
        Quaternion solveFABRIK(const std::vector<Vector3>& joints, const std::vector<float32>& lengths,
                             const Vector3& target, size_t maxIterations = 10, float32 tolerance = 0.01f);
        Quaternion solveJacobian(const std::vector<Vector3>& joints, const std::vector<float32>& lengths,
                                const Vector3& target, size_t maxIterations = 10, float32 tolerance = 0.01f);
        
    private:
        std::vector<Vector3> joints_;
        std::vector<float32> lengths_;
        Vector3 target_;
        size_t maxIterations_;
        float32 tolerance_;
        
        void updateJoints(const std::vector<Quaternion>& rotations);
        Vector3 getEndEffector() const;
        float32 getError() const;
    };
    
    // Quaternion physics
    class RigidBody {
    public:
        RigidBody();
        
        void setOrientation(const Quaternion& orientation);
        void setAngularVelocity(const Vector3& angularVelocity);
        void setInertia(const Vector3& inertia);
        void setMass(float32 mass);
        
        Quaternion getOrientation() const;
        Vector3 getAngularVelocity() const;
        Vector3 getInertia() const;
        float32 getMass() const;
        
        void applyTorque(const Vector3& torque);
        void applyImpulse(const Vector3& impulse);
        
        void update(float32 deltaTime);
        void integrate(float32 deltaTime);
        
    private:
        Quaternion orientation_;
        Vector3 angularVelocity_;
        Vector3 inertia_;
        float32 mass_;
        Vector3 torque_;
        Vector3 impulse_;
    };
    
    // Quaternion animation curves
    class AnimationCurve {
    public:
        struct KeyFrame {
            float32 time;
            Quaternion value;
            Quaternion inTangent;
            Quaternion outTangent;
        };
        
        AnimationCurve();
        
        void addKeyFrame(float32 time, const Quaternion& value);
        void addKeyFrame(float32 time, const Quaternion& value, const Quaternion& inTangent, const Quaternion& outTangent);
        
        Quaternion evaluate(float32 time) const;
        Quaternion evaluateLinear(float32 time) const;
        Quaternion evaluateSpherical(float32 time) const;
        Quaternion evaluateCubic(float32 time) const;
        
        void clear();
        size_t getKeyFrameCount() const;
        const KeyFrame& getKeyFrame(size_t index) const;
        
    private:
        std::vector<KeyFrame> keyFrames_;
        
        size_t findKeyFrameIndex(float32 time) const;
        Quaternion interpolateLinear(const KeyFrame& a, const KeyFrame& b, float32 t) const;
        Quaternion interpolateSpherical(const KeyFrame& a, const KeyFrame& b, float32 t) const;
        Quaternion interpolateCubic(const KeyFrame& a, const KeyFrame& b, const KeyFrame& c, const KeyFrame& d, float32 t) const;
    };
}

// Hash function for Quaternion
struct QuaternionHash {
    size_t operator()(const Quaternion& quaternion) const noexcept {
        size_t h1 = std::hash<float32>{}(quaternion.x);
        size_t h2 = std::hash<float32>{}(quaternion.y);
        size_t h3 = std::hash<float32>{}(quaternion.z);
        size_t h4 = std::hash<float32>{}(quaternion.w);
        
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
};

} // namespace RFUtils