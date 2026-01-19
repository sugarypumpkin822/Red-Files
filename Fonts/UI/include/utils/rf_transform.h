#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <memory>

namespace RFUtils {

// Forward declarations
class Vector2;
class Vector3;
class Vector4;
class Quaternion;
class Matrix3;
class Matrix4;
class Rect;

// Transform class
class Transform {
public:
    // Constructors
    Transform();
    Transform(const Vector3& position, const Quaternion& rotation, const Vector3& scale);
    Transform(const Vector3& position, const Vector3& euler, const Vector3& scale);
    Transform(const Matrix4& matrix);
    
    // Copy constructor
    Transform(const Transform& other);
    
    // Assignment operator
    Transform& operator=(const Transform& other);
    
    // Comparison operators
    bool operator==(const Transform& other) const;
    bool operator!=(const Transform& other) const;
    
    // Transform operations
    Transform combine(const Transform& other) const;
    Transform inverse() const;
    Transform lerp(const Transform& other, float32 t) const;
    Transform slerp(const Transform& other, float32 t) const;
    Transform nlerp(const Transform& other, float32 t) const;
    
    // Position access
    Vector3 getPosition() const;
    void setPosition(const Vector3& position);
    void setPosition(float32 x, float32 y, float32 z);
    
    // Rotation access
    Quaternion getRotation() const;
    void setRotation(const Quaternion& rotation);
    void setRotation(const Vector3& euler);
    void setRotation(float32 x, float32 y, float32 z);
    
    // Scale access
    Vector3 getScale() const;
    void setScale(const Vector3& scale);
    void setScale(float32 scale);
    void setScale(float32 scaleX, float32 scaleY, float32 scaleZ);
    
    // Euler angles access
    Vector3 getEulerAngles() const;
    void setEulerAngles(const Vector3& euler);
    void setEulerAngles(float32 x, float32 y, float32 z);
    
    // Transform operations
    Vector3 transformPoint(const Vector3& point) const;
    Vector3 transformVector(const Vector3& vector) const;
    Vector3 transformDirection(const Vector3& direction) const;
    
    Vector2 transformPoint(const Vector2& point) const;
    Vector2 transformVector(const Vector2& vector) const;
    Vector2 transformDirection(const Vector2& direction) const;
    
    // Matrix conversion
    Matrix4 getMatrix() const;
    void setMatrix(const Matrix4& matrix);
    
    // Decomposition
    void decompose(Vector3& position, Quaternion& rotation, Vector3& scale) const;
    void decompose(Vector3& position, Vector3& euler, Vector3& scale) const;
    
    // Utility methods
    bool isIdentity() const;
    bool isValid() const;
    bool hasNegativeScale() const;
    bool hasUniformScale() const;
    
    float32 getLossyScale() const;
    Vector3 getRight() const;
    Vector3 getUp() const;
    Vector3 getForward() const;
    
    // Static factory methods
    static Transform identity();
    static Transform zero();
    
    static Transform fromPosition(const Vector3& position);
    static Transform fromPosition(float32 x, float32 y, float32 z);
    
    static Transform fromRotation(const Quaternion& rotation);
    static Transform fromRotation(const Vector3& euler);
    static Transform fromRotation(float32 x, float32 y, float32 z);
    
    static Transform fromScale(const Vector3& scale);
    static Transform fromScale(float32 scale);
    static Transform fromScale(float32 scaleX, float32 scaleY, float32 scaleZ);
    
    static Transform lookAt(const Vector3& eye, const Vector3& target, const Vector3& up = Vector3::up());
    static Transform lookTo(const Vector3& eye, const Vector3& direction, const Vector3& up = Vector3::up());
    
    // String conversion
    std::string toString() const;
    
    // Constants
    static constexpr float32 EPSILON = 0.0001f;
    
private:
    Vector3 position_;
    Quaternion rotation_;
    Vector3 scale_;
    
    void normalizeScale();
};

// Transform2D class
class Transform2D {
public:
    // Constructors
    Transform2D();
    Transform2D(const Vector2& position, float32 rotation, const Vector2& scale);
    Transform2D(const Matrix3& matrix);
    
    // Copy constructor
    Transform2D(const Transform2D& other);
    
    // Assignment operator
    Transform2D& operator=(const Transform2D& other);
    
    // Comparison operators
    bool operator==(const Transform2D& other) const;
    bool operator!=(const Transform2D& other) const;
    
    // Transform operations
    Transform2D combine(const Transform2D& other) const;
    Transform2D inverse() const;
    Transform2D lerp(const Transform2D& other, float32 t) const;
    Transform2D slerp(const Transform2D& other, float32 t) const;
    Transform2D nlerp(const Transform2D& other, float32 t) const;
    
    // Position access
    Vector2 getPosition() const;
    void setPosition(const Vector2& position);
    void setPosition(float32 x, float32 y);
    
    // Rotation access
    float32 getRotation() const;
    void setRotation(float32 rotation);
    
    // Scale access
    Vector2 getScale() const;
    void setScale(const Vector2& scale);
    void setScale(float32 scale);
    void setScale(float32 scaleX, float32 scaleY);
    
    // Transform operations
    Vector2 transformPoint(const Vector2& point) const;
    Vector2 transformVector(const Vector2& vector) const;
    Vector2 transformDirection(const Vector2& direction) const;
    
    // Matrix conversion
    Matrix3 getMatrix() const;
    void setMatrix(const Matrix3& matrix);
    
    // Decomposition
    void decompose(Vector2& position, float32& rotation, Vector2& scale) const;
    
    // Utility methods
    bool isIdentity() const;
    bool isValid() const;
    bool hasNegativeScale() const;
    bool hasUniformScale() const;
    
    float32 getLossyScale() const;
    Vector2 getRight() const;
    Vector2 getUp() const;
    
    // Static factory methods
    static Transform2D identity();
    static Transform2D zero();
    
    static Transform2D fromPosition(const Vector2& position);
    static Transform2D fromPosition(float32 x, float32 y);
    
    static Transform2D fromRotation(float32 rotation);
    
    static Transform2D fromScale(const Vector2& scale);
    static Transform2D fromScale(float32 scale);
    static Transform2D fromScale(float32 scaleX, float32 scaleY);
    
    static Transform2D lookAt(const Vector2& eye, const Vector2& target);
    static Transform2D lookTo(const Vector2& eye, const Vector2& direction);
    
    // String conversion
    std::string toString() const;
    
    // Constants
    static constexpr float32 EPSILON = 0.0001f;
    
private:
    Vector2 position_;
    float32 rotation_;
    Vector2 scale_;
    
    void normalizeScale();
};

// Transform utilities namespace
namespace TransformUtils {
    // Transform operations
    Transform combine(const Transform& a, const Transform& b);
    Transform inverse(const Transform& transform);
    Transform lerp(const Transform& a, const Transform& b, float32 t);
    Transform slerp(const Transform& a, const Transform& b, float32 t);
    Transform nlerp(const Transform& a, const Transform& b, float32 t);
    
    Transform2D combine(const Transform2D& a, const Transform2D& b);
    Transform2D inverse(const Transform2D& transform);
    Transform2D lerp(const Transform2D& a, const Transform2D& b, float32 t);
    Transform2D slerp(const Transform2D& a, const Transform2D& b, float32 t);
    Transform2D nlerp(const Transform2D& a, const Transform2D& b, float32 t);
    
    // Transform comparison
    bool equals(const Transform& a, const Transform& b, float32 epsilon = Transform::EPSILON);
    bool approximatelyEquals(const Transform& a, const Transform& b, float32 epsilon = Transform::EPSILON);
    
    bool equals(const Transform2D& a, const Transform2D& b, float32 epsilon = Transform2D::EPSILON);
    bool approximatelyEquals(const Transform2D& a, const Transform2D& b, float32 epsilon = Transform2D::EPSILON);
    
    // Transform validation
    bool validate(const Transform& transform);
    bool validate(const Transform2D& transform);
    
    bool isFinite(const Transform& transform);
    bool isFinite(const Transform2D& transform);
    
    bool hasNaN(const Transform& transform);
    bool hasNaN(const Transform2D& transform);
    
    bool hasInfinity(const Transform& transform);
    bool hasInfinity(const Transform2D& transform);
    
    // Transform debugging
    std::string toString(const Transform& transform);
    std::string toString(const Transform2D& transform);
    void debugPrint(const Transform& transform);
    void debugPrint(const Transform2D& transform);
    void debugPrintFormatted(const Transform& transform);
    void debugPrintFormatted(const Transform2D& transform);
    
    // Transform conversion
    Matrix4 toMatrix(const Transform& transform);
    Matrix3 toMatrix(const Transform2D& transform);
    
    Transform fromMatrix(const Matrix4& matrix);
    Transform2D fromMatrix(const Matrix3& matrix);
    
    // Transform hierarchy
    class TransformNode {
    public:
        TransformNode();
        TransformNode(const Transform& localTransform);
        TransformNode(const std::string& name, const Transform& localTransform);
        
        // Hierarchy operations
        void setParent(TransformNode* parent);
        TransformNode* getParent() const { return parent_; }
        
        void addChild(TransformNode* child);
        void removeChild(TransformNode* child);
        void removeChildren();
        
        TransformNode* getChild(size_t index) const;
        TransformNode* findChild(const std::string& name) const;
        size_t getChildCount() const;
        std::vector<TransformNode*> getChildren() const;
        
        // Transform operations
        const Transform& getLocalTransform() const { return localTransform_; }
        void setLocalTransform(const Transform& transform);
        
        Transform getWorldTransform() const;
        void setWorldTransform(const Transform& transform);
        
        // Position operations
        Vector3 getLocalPosition() const;
        void setLocalPosition(const Vector3& position);
        
        Vector3 getWorldPosition() const;
        void setWorldPosition(const Vector3& position);
        
        // Rotation operations
        Quaternion getLocalRotation() const;
        void setLocalRotation(const Quaternion& rotation);
        
        Quaternion getWorldRotation() const;
        void setWorldRotation(const Quaternion& rotation);
        
        // Scale operations
        Vector3 getLocalScale() const;
        void setLocalScale(const Vector3& scale);
        
        Vector3 getWorldScale() const;
        void setWorldScale(const Vector3& scale);
        
        // Utility operations
        Vector3 getRight() const;
        Vector3 getUp() const;
        Vector3 getForward() const;
        
        Vector3 getWorldRight() const;
        Vector3 getWorldUp() const;
        Vector3 getWorldForward() const;
        
        // Transform operations
        Vector3 transformPoint(const Vector3& point) const;
        Vector3 transformVector(const Vector3& vector) const;
        Vector3 transformDirection(const Vector3& direction) const;
        
        Vector3 inverseTransformPoint(const Vector3& point) const;
        Vector3 inverseTransformVector(const Vector3& vector) const;
        Vector3 inverseTransformDirection(const Vector3& direction) const;
        
        // Node operations
        const std::string& getName() const { return name_; }
        void setName(const std::string& name) { name_ = name; }
        
        bool isRoot() const { return parent_ == nullptr; }
        bool isLeaf() const { return children_.empty(); }
        
        size_t getDepth() const;
        TransformNode* getRoot() const;
        std::vector<TransformNode*> getHierarchy() const;
        std::vector<TransformNode*> getSiblings() const;
        
        // Matrix operations
        Matrix4 getLocalMatrix() const;
        Matrix4 getWorldMatrix() const;
        Matrix4 getInverseWorldMatrix() const;
        
        // Validation
        bool isValid() const;
        bool validateHierarchy() const;
        
    private:
        std::string name_;
        Transform localTransform_;
        TransformNode* parent_;
        std::vector<TransformNode*> children_;
        
        void invalidateWorldTransform();
        mutable Transform cachedWorldTransform_;
        mutable bool worldTransformValid_;
        
        Transform computeWorldTransform() const;
    };
    
    // Transform2D hierarchy
    class Transform2DNode {
    public:
        Transform2DNode();
        Transform2DNode(const Transform2D& localTransform);
        Transform2DNode(const std::string& name, const Transform2D& localTransform);
        
        // Hierarchy operations
        void setParent(Transform2DNode* parent);
        Transform2DNode* getParent() const { return parent_; }
        
        void addChild(Transform2DNode* child);
        void removeChild(Transform2DNode* child);
        void removeChildren();
        
        Transform2DNode* getChild(size_t index) const;
        Transform2DNode* findChild(const std::string& name) const;
        size_t getChildCount() const;
        std::vector<Transform2DNode*> getChildren() const;
        
        // Transform operations
        const Transform2D& getLocalTransform() const { return localTransform_; }
        void setLocalTransform(const Transform2D& transform);
        
        Transform2D getWorldTransform() const;
        void setWorldTransform(const Transform2D& transform);
        
        // Position operations
        Vector2 getLocalPosition() const;
        void setLocalPosition(const Vector2& position);
        
        Vector2 getWorldPosition() const;
        void setWorldPosition(const Vector2& position);
        
        // Rotation operations
        float32 getLocalRotation() const;
        void setLocalRotation(float32 rotation);
        
        float32 getWorldRotation() const;
        void setWorldRotation(float32 rotation);
        
        // Scale operations
        Vector2 getLocalScale() const;
        void setLocalScale(const Vector2& scale);
        
        Vector2 getWorldScale() const;
        void setWorldScale(const Vector2& scale);
        
        // Utility operations
        Vector2 getRight() const;
        Vector2 getUp() const;
        
        Vector2 getWorldRight() const;
        Vector2 getWorldUp() const;
        
        // Transform operations
        Vector2 transformPoint(const Vector2& point) const;
        Vector2 transformVector(const Vector2& vector) const;
        Vector2 transformDirection(const Vector2& direction) const;
        
        Vector2 inverseTransformPoint(const Vector2& point) const;
        Vector2 inverseTransformVector(const Vector2& vector) const;
        Vector2 inverseTransformDirection(const Vector2& direction) const;
        
        // Node operations
        const std::string& getName() const { return name_; }
        void setName(const std::string& name) { name_ = name; }
        
        bool isRoot() const { return parent_ == nullptr; }
        bool isLeaf() const { return children_.empty(); }
        
        size_t getDepth() const;
        Transform2DNode* getRoot() const;
        std::vector<Transform2DNode*> getHierarchy() const;
        std::vector<Transform2DNode*> getSiblings() const;
        
        // Matrix operations
        Matrix3 getLocalMatrix() const;
        Matrix3 getWorldMatrix() const;
        Matrix3 getInverseWorldMatrix() const;
        
        // Validation
        bool isValid() const;
        bool validateHierarchy() const;
        
    private:
        std::string name_;
        Transform2D localTransform_;
        Transform2DNode* parent_;
        std::vector<Transform2DNode*> children_;
        
        void invalidateWorldTransform();
        mutable Transform2D cachedWorldTransform_;
        mutable bool worldTransformValid_;
        
        Transform2D computeWorldTransform() const;
    };
    
    // Transform animation
    class TransformAnimator {
    public:
        TransformAnimator();
        TransformAnimator(const Transform& start, const Transform& end, float32 duration);
        
        void setStart(const Transform& start);
        void setEnd(const Transform& end);
        void setDuration(float32 duration);
        void setLoop(bool loop);
        void setPingPong(bool pingPong);
        void setInterpolationMode(int mode); // 0=Linear, 1=Spherical, 2=Normalized
        
        Transform update(float32 deltaTime);
        Transform getValue(float32 t) const;
        
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
        Transform start_;
        Transform end_;
        float32 duration_;
        float32 time_;
        bool playing_;
        bool loop_;
        bool pingPong_;
        bool forward_;
        int interpolationMode_;
    };
    
    // Transform2D animation
    class Transform2DAnimator {
    public:
        Transform2DAnimator();
        Transform2DAnimator(const Transform2D& start, const Transform2D& end, float32 duration);
        
        void setStart(const Transform2D& start);
        void setEnd(const Transform2D& end);
        void setDuration(float32 duration);
        void setLoop(bool loop);
        void setPingPong(bool pingPong);
        void setInterpolationMode(int mode); // 0=Linear, 1=Spherical, 2=Normalized
        
        Transform2D update(float32 deltaTime);
        Transform2D getValue(float32 t) const;
        
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
        Transform2D start_;
        Transform2D end_;
        float32 duration_;
        float32 time_;
        bool playing_;
        bool loop_;
        bool pingPong_;
        bool forward_;
        int interpolationMode_;
    };
    
    // Transform cache
    class TransformCache {
    public:
        TransformCache(size_t maxSize = 1000);
        
        void put(const std::string& key, const Transform& transform);
        Transform get(const std::string& key) const;
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
            Transform transform;
            size_t accessCount;
            uint64_t lastAccessTime;
        };
        
        std::map<std::string, CacheEntry> cache_;
        size_t maxSize_;
        size_t hitCount_;
        size_t missCount_;
        
        void evictLeastRecentlyUsed();
    };
    
    // Transform2D cache
    class Transform2DCache {
    public:
        Transform2DCache(size_t maxSize = 1000);
        
        void put(const std::string& key, const Transform2D& transform);
        Transform2D get(const std::string& key) const;
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
            Transform2D transform;
            size_t accessCount;
            uint64_t lastAccessTime;
        };
        
        std::map<std::string, CacheEntry> cache_;
        size_t maxSize_;
        size_t hitCount_;
        size_t missCount_;
        
        void evictLeastRecentlyUsed();
    };
    
    // Transform pool
    class TransformPool {
    public:
        TransformPool(size_t initialSize = 100);
        
        Transform* acquire();
        void release(Transform* transform);
        void clear();
        
        size_t getPoolSize() const;
        size_t getUsedCount() const;
        size_t getAvailableCount() const;
        
    private:
        std::vector<std::unique_ptr<Transform>> pool_;
        std::vector<Transform*> available_;
        std::vector<Transform*> used_;
        
        void expandPool(size_t newSize);
    };
    
    // Transform2D pool
    class Transform2DPool {
    public:
        Transform2DPool(size_t initialSize = 100);
        
        Transform2D* acquire();
        void release(Transform2D* transform);
        void clear();
        
        size_t getPoolSize() const;
        size_t getUsedCount() const;
        size_t getAvailableCount() const;
        
    private:
        std::vector<std::unique_ptr<Transform2D>> pool_;
        std::vector<Transform2D*> available_;
        std::vector<Transform2D*> used_;
        
        void expandPool(size_t newSize);
    };
    
    // Transform stack
    class TransformStack {
    public:
        TransformStack();
        
        void push(const Transform& transform);
        void pop();
        void clear();
        
        Transform top() const;
        size_t size() const;
        bool empty() const;
        
        Transform multiply(const Transform& transform);
        Transform translate(const Vector3& translation);
        Transform rotate(const Quaternion& rotation);
        Transform scale(const Vector3& scale);
        
    private:
        std::vector<Transform> stack_;
    };
    
    // Transform2D stack
    class Transform2DStack {
    public:
        Transform2DStack();
        
        void push(const Transform2D& transform);
        void pop();
        void clear();
        
        Transform2D top() const;
        size_t size() const;
        bool empty() const;
        
        Transform2D multiply(const Transform2D& transform);
        Transform2D translate(const Vector2& translation);
        Transform2D rotate(float32 rotation);
        Transform2D scale(const Vector2& scale);
        
    private:
        std::vector<Transform2D> stack_;
    };
    
    // Transform builder
    class TransformBuilder {
    public:
        TransformBuilder();
        
        TransformBuilder& identity();
        TransformBuilder& zero();
        TransformBuilder& position(const Vector3& position);
        TransformBuilder& position(float32 x, float32 y, float32 z);
        TransformBuilder& rotation(const Quaternion& rotation);
        TransformBuilder& rotation(const Vector3& euler);
        TransformBuilder& rotation(float32 x, float32 y, float32 z);
        TransformBuilder& scale(const Vector3& scale);
        TransformBuilder& scale(float32 scale);
        TransformBuilder& scale(float32 scaleX, float32 scaleY, float32 scaleZ);
        
        TransformBuilder& translate(const Vector3& translation);
        TransformBuilder& translate(float32 x, float32 y, float32 z);
        TransformBuilder& rotate(const Quaternion& rotation);
        TransformBuilder& rotate(const Vector3& axis, float32 angle);
        TransformBuilder& rotateX(float32 angle);
        TransformBuilder& rotateY(float32 angle);
        TransformBuilder& rotateZ(float32 angle);
        TransformBuilder& scale(const Vector3& scale);
        TransformBuilder& scale(float32 scaleX, float32 scaleY, float32 scaleZ);
        
        TransformBuilder& lookAt(const Vector3& target, const Vector3& up = Vector3::up());
        TransformBuilder& lookTo(const Vector3& direction, const Vector3& up = Vector3::up());
        
        TransformBuilder& multiply(const Transform& transform);
        TransformBuilder& inverse();
        
        Transform build() const;
        operator Transform() const;
        
    private:
        Transform transform_;
    };
    
    // Transform2D builder
    class Transform2DBuilder {
    public:
        Transform2DBuilder();
        
        Transform2DBuilder& identity();
        Transform2DBuilder& zero();
        Transform2DBuilder& position(const Vector2& position);
        Transform2DBuilder& position(float32 x, float32 y);
        Transform2DBuilder& rotation(float32 rotation);
        Transform2DBuilder& scale(const Vector2& scale);
        Transform2DBuilder& scale(float32 scale);
        Transform2DBuilder& scale(float32 scaleX, float32 scaleY);
        
        Transform2DBuilder& translate(const Vector2& translation);
        Transform2DBuilder& translate(float32 x, float32 y);
        Transform2DBuilder& rotate(float32 rotation);
        Transform2DBuilder& scale(const Vector2& scale);
        Transform2DBuilder& scale(float32 scaleX, float32 scaleY);
        
        Transform2DBuilder& lookAt(const Vector2& target);
        Transform2DBuilder& lookTo(const Vector2& direction);
        
        Transform2DBuilder& multiply(const Transform2D& transform);
        Transform2DBuilder& inverse();
        
        Transform2D build() const;
        operator Transform2D() const;
        
    private:
        Transform2D transform_;
    };
    
    // Transform utilities
    Transform createTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale);
    Transform createTransform(const Vector3& position, const Vector3& euler, const Vector3& scale);
    Transform createTransform(const Matrix4& matrix);
    
    Transform2D createTransform2D(const Vector2& position, float32 rotation, const Vector2& scale);
    Transform2D createTransform2D(const Matrix3& matrix);
    
    Transform createLookAt(const Vector3& eye, const Vector3& target, const Vector3& up = Vector3::up());
    Transform createLookTo(const Vector3& eye, const Vector3& direction, const Vector3& up = Vector3::up());
    
    Transform2D createLookAt(const Vector2& eye, const Vector2& target);
    Transform2D createLookTo(const Vector2& eye, const Vector2& direction);
    
    // Transform analysis
    struct TransformAnalysis {
        bool isIdentity;
        bool isValid;
        bool hasNegativeScale;
        bool hasUniformScale;
        float32 lossyScale;
        Vector3 right;
        Vector3 up;
        Vector3 forward;
        float32 positionMagnitude;
        float32 rotationAngle;
        float32 scaleMagnitude;
    };
    
    struct Transform2DAnalysis {
        bool isIdentity;
        bool isValid;
        bool hasNegativeScale;
        bool hasUniformScale;
        float32 lossyScale;
        Vector2 right;
        Vector2 up;
        float32 positionMagnitude;
        float32 rotationAngle;
        float32 scaleMagnitude;
    };
    
    TransformAnalysis analyze(const Transform& transform);
    Transform2DAnalysis analyze(const Transform2D& transform);
    
    // Transform optimization
    Transform optimize(const Transform& transform);
    Transform compress(const Transform& transform);
    Transform decompress(const Transform& transform);
    
    Transform2D optimize(const Transform2D& transform);
    Transform2D compress(const Transform2D& transform);
    Transform2D decompress(const Transform2D& transform);
    
    // Transform validation
    bool validate(const Transform& transform);
    bool validateTransform(const Transform& transform);
    
    bool validate(const Transform2D& transform);
    bool validateTransform2D(const Transform2D& transform);
    
    // Transform debugging
    void debugPrintTransform(const Transform& transform);
    void debugPrintTransformFormatted(const Transform& transform);
    void debugPrintTransformAnalysis(const Transform& transform);
    
    void debugPrintTransform2D(const Transform2D& transform);
    void debugPrintTransform2DFormatted(const Transform2D& transform);
    void debugPrintTransform2DAnalysis(const Transform2D& transform);
    
    // Transform bounds
    class Bounds {
    public:
        Bounds();
        Bounds(const Vector3& center, const Vector3& size);
        Bounds(const Vector3& min, const Vector3& max);
        
        Vector3 getCenter() const;
        void setCenter(const Vector3& center);
        
        Vector3 getSize() const;
        void setSize(const Vector3& size);
        
        Vector3 getMin() const;
        Vector3 getMax() const;
        
        float32 getVolume() const;
        
        bool contains(const Vector3& point) const;
        bool contains(const Bounds& other) const;
        bool intersects(const Bounds& other) const;
        
        Bounds encapsulate(const Vector3& point) const;
        Bounds encapsulate(const Bounds& other) const;
        
        Bounds expand(float32 amount) const;
        Bounds expand(const Vector3& amount) const;
        
        Bounds transform(const Transform& transform) const;
        
        static Bounds fromPoints(const std::vector<Vector3>& points);
        static Bounds fromTransform(const Transform& transform);
        
    private:
        Vector3 center_;
        Vector3 size_;
        
        void updateMinMax();
        mutable Vector3 min_;
        mutable Vector3 max_;
        mutable bool minMaxValid_;
        
        void computeMinMax() const;
    };
    
    // Transform2D bounds
    class Bounds2D {
    public:
        Bounds2D();
        Bounds2D(const Vector2& center, const Vector2& size);
        Bounds2D(const Vector2& min, const Vector2& max);
        
        Vector2 getCenter() const;
        void setCenter(const Vector2& center);
        
        Vector2 getSize() const;
        void setSize(const Vector2& size);
        
        Vector2 getMin() const;
        Vector2 getMax() const;
        
        float32 getArea() const;
        
        bool contains(const Vector2& point) const;
        bool contains(const Bounds2D& other) const;
        bool intersects(const Bounds2D& other) const;
        
        Bounds2D encapsulate(const Vector2& point) const;
        Bounds2D encapsulate(const Bounds2D& other) const;
        
        Bounds2D expand(float32 amount) const;
        Bounds2D expand(const Vector2& amount) const;
        
        Bounds2D transform(const Transform2D& transform) const;
        
        static Bounds2D fromPoints(const std::vector<Vector2>& points);
        static Bounds2D fromTransform(const Transform2D& transform);
        
    private:
        Vector2 center_;
        Vector2 size_;
        
        void updateMinMax();
        mutable Vector2 min_;
        mutable Vector2 max_;
        mutable bool minMaxValid_;
        
        void computeMinMax() const;
    };
}

// Hash functions for Transform classes
struct TransformHash {
    size_t operator()(const Transform& transform) const noexcept {
        size_t h1 = std::hash<Vector3>{}(transform.getPosition());
        size_t h2 = std::hash<Quaternion>{}(transform.getRotation());
        size_t h3 = std::hash<Vector3>{}(transform.getScale());
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

struct Transform2DHash {
    size_t operator()(const Transform2D& transform) const noexcept {
        size_t h1 = std::hash<Vector2>{}(transform.getPosition());
        size_t h2 = std::hash<float32>{}(transform.getRotation());
        size_t h3 = std::hash<Vector2>{}(transform.getScale());
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

} // namespace RFUtils