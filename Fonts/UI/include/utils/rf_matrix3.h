#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <cmath>

namespace RFUtils {

// Forward declarations
class Vector2;
class Vector3;
class Vector4;
class Matrix4;

// 3x3 Matrix class
class Matrix3 {
public:
    // Matrix elements (column-major order)
    float32 m00, m01, m02; // Column 0
    float32 m10, m11, m12; // Column 1
    float32 m20, m21, m22; // Column 2
    
    // Constructors
    Matrix3();
    Matrix3(float32 m00, float32 m01, float32 m02,
            float32 m10, float32 m11, float32 m12,
            float32 m20, float32 m21, float32 m22);
    Matrix3(const float32* elements);
    Matrix3(const std::array<float32, 9>& elements);
    Matrix3(const Vector2& column0, const Vector2& column1, const Vector2& column2);
    Matrix3(const Vector3& column0, const Vector3& column1, const Vector3& column2);
    Matrix3(const Matrix4& matrix4); // Extract 3x3 from 4x4
    
    // Copy constructor
    Matrix3(const Matrix3& other);
    
    // Assignment operator
    Matrix3& operator=(const Matrix3& other);
    
    // Element access
    float32& operator()(size_t row, size_t col) { return (&m00)[col * 3 + row]; }
    const float32& operator()(size_t row, size_t col) const { return (&m00)[col * 3 + row]; }
    
    float32& operator[](size_t index) { return (&m00)[index]; }
    const float32& operator[](size_t index) const { return (&m00)[index]; }
    
    // Column access
    Vector3 getColumn(size_t col) const;
    void setColumn(size_t col, const Vector3& column);
    
    // Row access
    Vector3 getRow(size_t row) const;
    void setRow(size_t row, const Vector3& row);
    
    // Diagonal access
    Vector3 getDiagonal() const;
    void setDiagonal(const Vector3& diagonal);
    
    // Array access
    float32* data() { return &m00; }
    const float32* data() const { return &m00; }
    
    // Comparison operators
    bool operator==(const Matrix3& other) const;
    bool operator!=(const Matrix3& other) const;
    
    // Arithmetic operators
    Matrix3 operator+(const Matrix3& other) const;
    Matrix3 operator-(const Matrix3& other) const;
    Matrix3 operator*(const Matrix3& other) const;
    Matrix3 operator*(float32 scalar) const;
    Matrix3 operator/(float32 scalar) const;
    
    // Assignment arithmetic operators
    Matrix3& operator+=(const Matrix3& other);
    Matrix3& operator-=(const Matrix3& other);
    Matrix3& operator*=(const Matrix3& other);
    Matrix3& operator*=(float32 scalar);
    Matrix3& operator/=(float32 scalar);
    
    // Unary operators
    Matrix3 operator+() const;
    Matrix3 operator-() const;
    
    // Matrix operations
    Matrix3 transpose() const;
    Matrix3 inverse() const;
    float32 determinant() const;
    Matrix3 adjugate() const;
    Matrix3 cofactor() const;
    
    bool isInvertible() const;
    bool isOrthogonal() const;
    bool isIdentity() const;
    bool isSymmetric() const;
    bool isSkewSymmetric() const;
    bool isUpperTriangular() const;
    bool isLowerTriangular() const;
    bool isDiagonal() const;
    
    // Transform operations
    Vector3 transformPoint(const Vector3& point) const;
    Vector2 transformPoint(const Vector2& point) const;
    
    Vector3 transformVector(const Vector3& vector) const;
    Vector2 transformVector(const Vector2& vector) const;
    
    Vector3 transformDirection(const Vector3& direction) const;
    Vector2 transformDirection(const Vector2& direction) const;
    
    // Decomposition
    void decompose(Vector2& translation, float32& rotation, Vector2& scale) const;
    Vector2 getTranslation() const;
    float32 getRotation() const;
    Vector2 getScale() const;
    
    void setTranslation(const Vector2& translation);
    void setTranslation(float32 x, float32 y);
    void setRotation(float32 rotation);
    void setScale(const Vector2& scale);
    void setScale(float32 scale);
    void setScale(float32 scaleX, float32 scaleY);
    
    // Static factory methods
    static Matrix3 zero();
    static Matrix3 identity();
    static Matrix3 ones();
    
    // Translation matrices
    static Matrix3 translation(const Vector2& translation);
    static Matrix3 translation(float32 x, float32 y);
    
    // Rotation matrices
    static Matrix3 rotation(float32 angle);
    static Matrix3 rotationFromTo(const Vector2& from, const Vector2& to);
    
    // Scale matrices
    static Matrix3 scale(const Vector2& scale);
    static Matrix3 scale(float32 scale);
    static Matrix3 scale(float32 scaleX, float32 scaleY);
    
    // Reflection matrices
    static Matrix3 reflection(const Vector2& normal);
    static Matrix3 reflection(const Vector2& normal, const Vector2& point);
    
    // Shear matrices
    static Matrix3 shear(float32 xy, float32 yx);
    static Matrix3 shearX(float32 y);
    static Matrix3 shearY(float32 x);
    
    // Composition
    static Matrix3 compose(const Vector2& translation, float32 rotation, const Vector2& scale);
    
    // Interpolation
    static Matrix3 lerp(const Matrix3& a, const Matrix3& b, float32 t);
    static Matrix3 slerp(const Matrix3& a, const Matrix3& b, float32 t);
    static Matrix3 nlerp(const Matrix3& a, const Matrix3& b, float32 t);
    
    // Utility methods
    Matrix3 abs() const;
    Matrix3 round() const;
    Matrix3 floor() const;
    Matrix3 ceil() const;
    Matrix3 min(const Matrix3& other) const;
    Matrix3 max(const Matrix3& other) const;
    Matrix3 clamp(const Matrix3& min, const Matrix3& max) const;
    
    Matrix3 normalize() const;
    Matrix3 orthonormalize() const;
    Matrix3 orthogonalize() const;
    
    // String conversion
    std::string toString() const;
    
    // Constants
    static constexpr float32 EPSILON = 0.0001f;
};

// Global operators for scalar operations
inline Matrix3 operator*(float32 scalar, const Matrix3& matrix) {
    return matrix * scalar;
}

// Utility functions
inline Matrix3 abs(const Matrix3& matrix) {
    return matrix.abs();
}

inline Matrix3 min(const Matrix3& a, const Matrix3& b) {
    return a.min(b);
}

inline Matrix3 max(const Matrix3& a, const Matrix3& b) {
    return a.max(b);
}

inline Matrix3 clamp(const Matrix3& value, const Matrix3& min, const Matrix3& max) {
    return value.clamp(min, max);
}

inline Matrix3 lerp(const Matrix3& a, const Matrix3& b, float32 t) {
    return Matrix3::lerp(a, b, t);
}

inline Matrix3 slerp(const Matrix3& a, const Matrix3& b, float32 t) {
    return Matrix3::slerp(a, b, t);
}

inline Matrix3 nlerp(const Matrix3& a, const Matrix3& b, float32 t) {
    return Matrix3::nlerp(a, b, t);
}

// Matrix utilities namespace
namespace Matrix3Utils {
    // Matrix operations
    Matrix3 transpose(const Matrix3& matrix);
    Matrix3 inverse(const Matrix3& matrix);
    float32 determinant(const Matrix3& matrix);
    Matrix3 adjugate(const Matrix3& matrix);
    Matrix3 cofactor(const Matrix3& matrix);
    
    bool isInvertible(const Matrix3& matrix);
    bool isOrthogonal(const Matrix3& matrix);
    bool isIdentity(const Matrix3& matrix);
    bool isSymmetric(const Matrix3& matrix);
    bool isSkewSymmetric(const Matrix3& matrix);
    bool isUpperTriangular(const Matrix3& matrix);
    bool isLowerTriangular(const Matrix3& matrix);
    bool isDiagonal(const Matrix3& matrix);
    
    // Matrix decomposition
    bool decompose(const Matrix3& matrix, Vector2& translation, float32& rotation, Vector2& scale);
    bool decomposeSVD(const Matrix3& matrix, Matrix3& U, Vector3& S, Matrix3& V);
    bool decomposeQR(const Matrix3& matrix, Matrix3& Q, Matrix3& R);
    bool decomposeLU(const Matrix3& matrix, Matrix3& L, Matrix3& U);
    bool decomposeCholesky(const Matrix3& matrix, Matrix3& L);
    
    // Matrix factorization
    Matrix3 cholesky(const Matrix3& matrix);
    Matrix3 lu(const Matrix3& matrix);
    Matrix3 qr(const Matrix3& matrix);
    Matrix3 svd(const Matrix3& matrix);
    
    // Matrix eigenvalues and eigenvectors
    std::vector<float32> eigenvalues(const Matrix3& matrix);
    std::vector<Vector3> eigenvectors(const Matrix3& matrix);
    std::pair<std::vector<float32>, std::vector<Vector3>> eigen(const Matrix3& matrix);
    
    // Matrix norms
    float32 norm(const Matrix3& matrix);
    float32 norm1(const Matrix3& matrix); // L1 norm
    float32 norm2(const Matrix3& matrix); // L2 norm
    float32 normInf(const Matrix3& matrix); // Infinity norm
    float32 normFrobenius(const Matrix3& matrix); // Frobenius norm
    
    float32 conditionNumber(const Matrix3& matrix);
    float32 rank(const Matrix3& matrix);
    float32 trace(const Matrix3& matrix);
    
    // Matrix comparison
    bool equals(const Matrix3& a, const Matrix3& b, float32 epsilon = Matrix3::EPSILON);
    bool approximatelyEquals(const Matrix3& a, const Matrix3& b, float32 epsilon = Matrix3::EPSILON);
    
    // Matrix interpolation
    Matrix3 interpolateLinear(const Matrix3& a, const Matrix3& b, float32 t);
    Matrix3 interpolateSpherical(const Matrix3& a, const Matrix3& b, float32 t);
    Matrix3 interpolateNormalized(const Matrix3& a, const Matrix3& b, float32 t);
    
    // Matrix smoothing
    Matrix3 smoothStep(const Matrix3& a, const Matrix3& b, float32 t);
    Matrix3 smootherStep(const Matrix3& a, const Matrix3& b, float32 t);
    
    // Matrix validation
    bool isValid(const Matrix3& matrix);
    bool isFinite(const Matrix3& matrix);
    bool hasNaN(const Matrix3& matrix);
    bool hasInfinity(const Matrix3& matrix);
    
    // Matrix debugging
    std::string toString(const Matrix3& matrix);
    void debugPrint(const Matrix3& matrix);
    void debugPrintFormatted(const Matrix3& matrix);
    
    // Matrix conversion
    Matrix4 toMatrix4(const Matrix3& matrix);
    Matrix3 fromMatrix4(const Matrix4& matrix);
    
    // Matrix transformations
    Matrix3 translate(const Matrix3& matrix, const Vector2& translation);
    Matrix3 rotate(const Matrix3& matrix, float32 rotation);
    Matrix3 scale(const Matrix3& matrix, const Vector2& scale);
    
    Matrix3 transform(const Matrix3& matrix, const Vector2& translation, float32 rotation, const Vector2& scale);
    
    // Matrix coordinate systems
    Matrix3 worldToView(const Matrix3& world, const Vector2& eye, const Vector2& target, const Vector2& up);
    Matrix3 viewToWorld(const Matrix3& view, const Vector2& eye, const Vector2& target, const Vector2& up);
    
    // Matrix animation
    class Matrix3Animator {
    public:
        Matrix3Animator();
        Matrix3Animator(const Matrix3& start, const Matrix3& end, float32 duration);
        
        void setStart(const Matrix3& start);
        void setEnd(const Matrix3& end);
        void setDuration(float32 duration);
        void setLoop(bool loop);
        void setPingPong(bool pingPong);
        
        Matrix3 update(float32 deltaTime);
        Matrix3 getValue(float32 t) const;
        
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
        Matrix3 start_;
        Matrix3 end_;
        float32 duration_;
        float32 time_;
        bool playing_;
        bool loop_;
        bool pingPong_;
        bool forward_;
    };
    
    // Matrix cache
    class Matrix3Cache {
    public:
        Matrix3Cache(size_t maxSize = 1000);
        
        void put(const std::string& key, const Matrix3& matrix);
        Matrix3 get(const std::string& key) const;
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
            Matrix3 matrix;
            size_t accessCount;
            uint64_t lastAccessTime;
        };
        
        std::map<std::string, CacheEntry> cache_;
        size_t maxSize_;
        size_t hitCount_;
        size_t missCount_;
        
        void evictLeastRecentlyUsed();
    };
    
    // Matrix pool
    class Matrix3Pool {
    public:
        Matrix3Pool(size_t initialSize = 100);
        
        Matrix3* acquire();
        void release(Matrix3* matrix);
        void clear();
        
        size_t getPoolSize() const;
        size_t getUsedCount() const;
        size_t getAvailableCount() const;
        
    private:
        std::vector<std::unique_ptr<Matrix3>> pool_;
        std::vector<Matrix3*> available_;
        std::vector<Matrix3*> used_;
        
        void expandPool(size_t newSize);
    };
    
    // Matrix stack
    class Matrix3Stack {
    public:
        Matrix3Stack();
        
        void push(const Matrix3& matrix);
        void pop();
        void clear();
        
        Matrix3 top() const;
        size_t size() const;
        bool empty() const;
        
        Matrix3 multiply(const Matrix3& matrix);
        Matrix3 translate(const Vector2& translation);
        Matrix3 rotate(float32 rotation);
        Matrix3 scale(const Vector2& scale);
        
    private:
        std::vector<Matrix3> stack_;
    };
    
    // Matrix builder
    class Matrix3Builder {
    public:
        Matrix3Builder();
        
        Matrix3Builder& identity();
        Matrix3Builder& zero();
        Matrix3Builder& translation(const Vector2& translation);
        Matrix3Builder& translation(float32 x, float32 y);
        Matrix3Builder& rotation(float32 rotation);
        Matrix3Builder& scale(const Vector2& scale);
        Matrix3Builder& scale(float32 scale);
        Matrix3Builder& scale(float32 scaleX, float32 scaleY);
        
        Matrix3Builder& multiply(const Matrix3& matrix);
        Matrix3Builder& transpose();
        Matrix3Builder& inverse();
        
        Matrix3 build() const;
        operator Matrix3() const;
        
    private:
        Matrix3 matrix_;
    };
    
    // Matrix utilities
    Matrix3 createBillboard(const Vector2& position, const Vector2& cameraPosition, const Vector2& cameraUp, const Vector2& cameraRight);
    Matrix3 createConstrainedBillboard(const Vector2& position, const Vector2& cameraPosition, const Vector2& cameraUp, const Vector2& cameraRight, const Vector2& objectAxis);
    
    Matrix3 createReflection(const Vector2& normal);
    Matrix3 createReflection(const Vector2& normal, const Vector2& point);
    
    Matrix3 createShear(float32 xy, float32 yx);
    Matrix3 createShearX(float32 y);
    Matrix3 createShearY(float32 x);
    
    Matrix3 createNormalMatrix(const Matrix3& matrix);
    Matrix3 createInverseTranspose(const Matrix3& matrix);
    
    // Matrix analysis
    struct Matrix3Analysis {
        bool isIdentity;
        bool isOrthogonal;
        bool isSymmetric;
        bool isSkewSymmetric;
        bool isUpperTriangular;
        bool isLowerTriangular;
        bool isDiagonal;
        bool isInvertible;
        float32 determinant;
        float32 trace;
        float32 conditionNumber;
        float32 rank;
        Vector3 eigenvalues;
        Vector3 eigenvectors[3];
    };
    
    Matrix3Analysis analyze(const Matrix3& matrix);
    
    // Matrix optimization
    Matrix3 optimize(const Matrix3& matrix);
    Matrix3 compress(const Matrix3& matrix);
    Matrix3 decompress(const Matrix3& matrix);
    
    // Matrix validation
    bool validate(const Matrix3& matrix);
    bool validateTransform(const Matrix3& matrix);
    
    // Matrix debugging
    void debugPrintMatrix(const Matrix3& matrix);
    void debugPrintMatrixFormatted(const Matrix3& matrix);
    void debugPrintMatrixAnalysis(const Matrix3& matrix);
    
    // 2D specific utilities
    Matrix3 createRotationFromAngle(float32 angle);
    Matrix3 createRotationFromDirection(const Vector2& direction);
    Matrix3 createRotationFromVectors(const Vector2& from, const Vector2& to);
    
    Matrix3 createScaleFromVector(const Vector2& scale);
    Matrix3 createScaleFromUniform(float32 scale);
    Matrix3 createScaleFromNonUniform(float32 scaleX, float32 scaleY);
    
    Matrix3 createTranslationFromVector(const Vector2& translation);
    Matrix3 createTranslationFromCoordinates(float32 x, float32 y);
    
    // 2D coordinate system utilities
    Matrix3 createWorldToScreen(const Vector2& worldOrigin, const Vector2& worldScale, float32 worldRotation);
    Matrix3 createScreenToWorld(const Vector2& screenOrigin, const Vector2& screenScale, float32 screenRotation);
    
    // 2D transformation utilities
    Matrix3 createTransform(const Vector2& position, float32 rotation, const Vector2& scale);
    Matrix3 createInverseTransform(const Vector2& position, float32 rotation, const Vector2& scale);
    
    // 2D projection utilities
    Matrix3 createOrthographic(float32 left, float32 right, float32 bottom, float32 top);
    Matrix3 createPerspective(float32 fov, float32 aspect, float32 near, float32 far);
    
    // 2D view utilities
    Matrix3 createLookAt(const Vector2& eye, const Vector2& target, const Vector2& up);
    Matrix3 createLookTo(const Vector2& eye, const Vector2& direction, const Vector2& up);
}

// Hash function for Matrix3
struct Matrix3Hash {
    size_t operator()(const Matrix3& matrix) const noexcept {
        size_t h1 = std::hash<float32>{}(matrix.m00);
        size_t h2 = std::hash<float32>{}(matrix.m01);
        size_t h3 = std::hash<float32>{}(matrix.m02);
        size_t h4 = std::hash<float32>{}(matrix.m10);
        size_t h5 = std::hash<float32>{}(matrix.m11);
        size_t h6 = std::hash<float32>{}(matrix.m12);
        size_t h7 = std::hash<float32>{}(matrix.m20);
        size_t h8 = std::hash<float32>{}(matrix.m21);
        size_t h9 = std::hash<float32>{}(matrix.m22);
        
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5) ^ (h7 << 6) ^ (h8 << 7) ^ (h9 << 8);
    }
};

} // namespace RFUtils