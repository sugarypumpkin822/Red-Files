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
class Quaternion;
class Matrix3;

// 4x4 Matrix class
class Matrix4 {
public:
    // Matrix elements (column-major order)
    float32 m00, m01, m02, m03; // Column 0
    float32 m10, m11, m12, m13; // Column 1
    float32 m20, m21, m22, m23; // Column 2
    float32 m30, m31, m32, m33; // Column 3
    
    // Constructors
    Matrix4();
    Matrix4(float32 m00, float32 m01, float32 m02, float32 m03,
            float32 m10, float32 m11, float32 m12, float32 m13,
            float32 m20, float32 m21, float32 m22, float32 m23,
            float32 m30, float32 m31, float32 m32, float32 m33);
    Matrix4(const float32* elements);
    Matrix4(const std::array<float32, 16>& elements);
    Matrix4(const Matrix3& matrix3); // Convert 3x3 to 4x4
    Matrix4(const Vector4& column0, const Vector4& column1, const Vector4& column2, const Vector4& column3);
    
    // Copy constructor
    Matrix4(const Matrix4& other);
    
    // Assignment operator
    Matrix4& operator=(const Matrix4& other);
    
    // Element access
    float32& operator()(size_t row, size_t col) { return (&m00)[col * 4 + row]; }
    const float32& operator()(size_t row, size_t col) const { return (&m00)[col * 4 + row]; }
    
    float32& operator[](size_t index) { return (&m00)[index]; }
    const float32& operator[](size_t index) const { return (&m00)[index]; }
    
    // Column access
    Vector4 getColumn(size_t col) const;
    void setColumn(size_t col, const Vector4& column);
    
    // Row access
    Vector4 getRow(size_t row) const;
    void setRow(size_t row, const Vector4& row);
    
    // Diagonal access
    Vector4 getDiagonal() const;
    void setDiagonal(const Vector4& diagonal);
    
    // Array access
    float32* data() { return &m00; }
    const float32* data() const { return &m00; }
    
    // Comparison operators
    bool operator==(const Matrix4& other) const;
    bool operator!=(const Matrix4& other) const;
    
    // Arithmetic operators
    Matrix4 operator+(const Matrix4& other) const;
    Matrix4 operator-(const Matrix4& other) const;
    Matrix4 operator*(const Matrix4& other) const;
    Matrix4 operator*(float32 scalar) const;
    Matrix4 operator/(float32 scalar) const;
    
    // Assignment arithmetic operators
    Matrix4& operator+=(const Matrix4& other);
    Matrix4& operator-=(const Matrix4& other);
    Matrix4& operator*=(const Matrix4& other);
    Matrix4& operator*=(float32 scalar);
    Matrix4& operator/=(float32 scalar);
    
    // Unary operators
    Matrix4 operator+() const;
    Matrix4 operator-() const;
    
    // Matrix operations
    Matrix4 transpose() const;
    Matrix4 inverse() const;
    float32 determinant() const;
    Matrix4 adjugate() const;
    Matrix4 cofactor() const;
    
    bool isInvertible() const;
    bool isOrthogonal() const;
    bool isIdentity() const;
    bool isSymmetric() const;
    bool isSkewSymmetric() const;
    bool isUpperTriangular() const;
    bool isLowerTriangular() const;
    bool isDiagonal() const;
    
    // Transform operations
    Vector4 transformPoint(const Vector4& point) const;
    Vector3 transformPoint(const Vector3& point) const;
    Vector2 transformPoint(const Vector2& point) const;
    
    Vector4 transformVector(const Vector4& vector) const;
    Vector3 transformVector(const Vector3& vector) const;
    Vector2 transformVector(const Vector2& vector) const;
    
    Vector4 transformDirection(const Vector4& direction) const;
    Vector3 transformDirection(const Vector3& direction) const;
    Vector2 transformDirection(const Vector2& direction) const;
    
    // Decomposition
    void decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) const;
    Vector3 getTranslation() const;
    Quaternion getRotation() const;
    Vector3 getScale() const;
    Vector3 getEulerAngles() const;
    
    void setTranslation(const Vector3& translation);
    void setTranslation(float32 x, float32 y, float32 z);
    void setRotation(const Quaternion& rotation);
    void setRotation(const Vector3& euler);
    void setScale(const Vector3& scale);
    void setScale(float32 scale);
    void setScale(float32 scaleX, float32 scaleY, float32 scaleZ);
    
    // Static factory methods
    static Matrix4 zero();
    static Matrix4 identity();
    static Matrix4 ones();
    
    // Translation matrices
    static Matrix4 translation(const Vector3& translation);
    static Matrix4 translation(float32 x, float32 y, float32 z);
    
    // Rotation matrices
    static Matrix4 rotation(const Quaternion& quaternion);
    static Matrix4 rotation(const Vector3& axis, float32 angle);
    static Matrix4 rotationX(float32 angle);
    static Matrix4 rotationY(float32 angle);
    static Matrix4 rotationZ(float32 angle);
    static Matrix4 rotationEuler(const Vector3& euler);
    static Matrix4 rotationEuler(float32 x, float32 y, float32 z);
    static Matrix4 rotationFromTo(const Vector3& from, const Vector3& to);
    
    // Scale matrices
    static Matrix4 scale(const Vector3& scale);
    static Matrix4 scale(float32 scale);
    static Matrix4 scale(float32 scaleX, float32 scaleY, float32 scaleZ);
    
    // Projection matrices
    static Matrix4 orthographic(float32 left, float32 right, float32 bottom, float32 top, float32 near, float32 far);
    static Matrix4 perspective(float32 fov, float32 aspect, float32 near, float32 far);
    static Matrix4 perspectiveFov(float32 fov, float32 width, float32 height, float32 near, float32 far);
    static Matrix4 frustum(float32 left, float32 right, float32 bottom, float32 top, float32 near, float32 far);
    
    // View matrices
    static Matrix4 lookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
    static Matrix4 lookTo(const Vector3& eye, const Vector3& direction, const Vector3& up);
    
    // Reflection matrices
    static Matrix4 reflection(const Vector3& normal);
    static Matrix4 reflection(const Vector3& normal, const Vector3& point);
    static Matrix4 reflection(const Vector4& plane);
    
    // Shadow matrices
    static Matrix4 shadow(const Vector3& lightDirection, const Vector4& plane);
    static Matrix4 shadow(const Vector3& lightPosition, const Vector4& plane);
    
    // Shear matrices
    static Matrix4 shear(float32 xy, float32 xz, float32 yx, float32 yz, float32 zx, float32 zy);
    static Matrix4 shearX(float32 y, float32 z);
    static Matrix4 shearY(float32 x, float32 z);
    static Matrix4 shearZ(float32 x, float32 y);
    
    // Composition
    static Matrix4 compose(const Vector3& translation, const Quaternion& rotation, const Vector3& scale);
    static Matrix4 compose(const Vector3& translation, const Vector3& euler, const Vector3& scale);
    
    // Interpolation
    static Matrix4 lerp(const Matrix4& a, const Matrix4& b, float32 t);
    static Matrix4 slerp(const Matrix4& a, const Matrix4& b, float32 t);
    static Matrix4 nlerp(const Matrix4& a, const Matrix4& b, float32 t);
    
    // Utility methods
    Matrix4 abs() const;
    Matrix4 round() const;
    Matrix4 floor() const;
    Matrix4 ceil() const;
    Matrix4 min(const Matrix4& other) const;
    Matrix4 max(const Matrix4& other) const;
    Matrix4 clamp(const Matrix4& min, const Matrix4& max) const;
    
    Matrix4 normalize() const;
    Matrix4 orthonormalize() const;
    Matrix4 orthogonalize() const;
    
    // String conversion
    std::string toString() const;
    
    // Constants
    static constexpr float32 EPSILON = 0.0001f;
};

// Global operators for scalar operations
inline Matrix4 operator*(float32 scalar, const Matrix4& matrix) {
    return matrix * scalar;
}

// Utility functions
inline Matrix4 abs(const Matrix4& matrix) {
    return matrix.abs();
}

inline Matrix4 min(const Matrix4& a, const Matrix4& b) {
    return a.min(b);
}

inline Matrix4 max(const Matrix4& a, const Matrix4& b) {
    return a.max(b);
}

inline Matrix4 clamp(const Matrix4& value, const Matrix4& min, const Matrix4& max) {
    return value.clamp(min, max);
}

inline Matrix4 lerp(const Matrix4& a, const Matrix4& b, float32 t) {
    return Matrix4::lerp(a, b, t);
}

inline Matrix4 slerp(const Matrix4& a, const Matrix4& b, float32 t) {
    return Matrix4::slerp(a, b, t);
}

inline Matrix4 nlerp(const Matrix4& a, const Matrix4& b, float32 t) {
    return Matrix4::nlerp(a, b, t);
}

// Matrix utilities namespace
namespace Matrix4Utils {
    // Matrix operations
    Matrix4 transpose(const Matrix4& matrix);
    Matrix4 inverse(const Matrix4& matrix);
    float32 determinant(const Matrix4& matrix);
    Matrix4 adjugate(const Matrix4& matrix);
    Matrix4 cofactor(const Matrix4& matrix);
    
    bool isInvertible(const Matrix4& matrix);
    bool isOrthogonal(const Matrix4& matrix);
    bool isIdentity(const Matrix4& matrix);
    bool isSymmetric(const Matrix4& matrix);
    bool isSkewSymmetric(const Matrix4& matrix);
    bool isUpperTriangular(const Matrix4& matrix);
    bool isLowerTriangular(const Matrix4& matrix);
    bool isDiagonal(const Matrix4& matrix);
    
    // Matrix decomposition
    bool decompose(const Matrix4& matrix, Vector3& translation, Quaternion& rotation, Vector3& scale);
    bool decomposeSVD(const Matrix4& matrix, Matrix4& U, Vector4& S, Matrix4& V);
    bool decomposeQR(const Matrix4& matrix, Matrix4& Q, Matrix4& R);
    bool decomposeLU(const Matrix4& matrix, Matrix4& L, Matrix4& U);
    bool decomposeCholesky(const Matrix4& matrix, Matrix4& L);
    
    // Matrix factorization
    Matrix4 cholesky(const Matrix4& matrix);
    Matrix4 lu(const Matrix4& matrix);
    Matrix4 qr(const Matrix4& matrix);
    Matrix4 svd(const Matrix4& matrix);
    
    // Matrix eigenvalues and eigenvectors
    std::vector<float32> eigenvalues(const Matrix4& matrix);
    std::vector<Vector4> eigenvectors(const Matrix4& matrix);
    std::pair<std::vector<float32>, std::vector<Vector4>> eigen(const Matrix4& matrix);
    
    // Matrix norms
    float32 norm(const Matrix4& matrix);
    float32 norm1(const Matrix4& matrix); // L1 norm
    float32 norm2(const Matrix4& matrix); // L2 norm
    float32 normInf(const Matrix4& matrix); // Infinity norm
    float32 normFrobenius(const Matrix4& matrix); // Frobenius norm
    
    float32 conditionNumber(const Matrix4& matrix);
    float32 rank(const Matrix4& matrix);
    float32 trace(const Matrix4& matrix);
    
    // Matrix comparison
    bool equals(const Matrix4& a, const Matrix4& b, float32 epsilon = Matrix4::EPSILON);
    bool approximatelyEquals(const Matrix4& a, const Matrix4& b, float32 epsilon = Matrix4::EPSILON);
    
    // Matrix interpolation
    Matrix4 interpolateLinear(const Matrix4& a, const Matrix4& b, float32 t);
    Matrix4 interpolateSpherical(const Matrix4& a, const Matrix4& b, float32 t);
    Matrix4 interpolateNormalized(const Matrix4& a, const Matrix4& b, float32 t);
    
    // Matrix smoothing
    Matrix4 smoothStep(const Matrix4& a, const Matrix4& b, float32 t);
    Matrix4 smootherStep(const Matrix4& a, const Matrix4& b, float32 t);
    
    // Matrix validation
    bool isValid(const Matrix4& matrix);
    bool isFinite(const Matrix4& matrix);
    bool hasNaN(const Matrix4& matrix);
    bool hasInfinity(const Matrix4& matrix);
    
    // Matrix debugging
    std::string toString(const Matrix4& matrix);
    void debugPrint(const Matrix4& matrix);
    void debugPrintFormatted(const Matrix4& matrix);
    
    // Matrix conversion
    Matrix3 toMatrix3(const Matrix4& matrix);
    Matrix4 fromMatrix3(const Matrix3& matrix);
    
    // Matrix transformations
    Matrix4 translate(const Matrix4& matrix, const Vector3& translation);
    Matrix4 rotate(const Matrix4& matrix, const Quaternion& rotation);
    Matrix4 rotate(const Matrix4& matrix, const Vector3& axis, float32 angle);
    Matrix4 scale(const Matrix4& matrix, const Vector3& scale);
    
    Matrix4 transform(const Matrix4& matrix, const Vector3& translation, const Quaternion& rotation, const Vector3& scale);
    
    // Matrix projections
    Matrix4 project(const Matrix4& matrix, const Matrix4& projection);
    Matrix4 unproject(const Matrix4& matrix, const Matrix4& projection);
    
    // Matrix coordinate systems
    Matrix4 worldToView(const Matrix4& world, const Vector3& eye, const Vector3& target, const Vector3& up);
    Matrix4 viewToWorld(const Matrix4& view, const Vector3& eye, const Vector3& target, const Vector3& up);
    Matrix4 worldToScreen(const Matrix4& world, const Matrix4& view, const Matrix4& projection);
    Matrix4 screenToWorld(const Matrix4& screen, const Matrix4& view, const Matrix4& projection);
    
    // Matrix animation
    class MatrixAnimator {
    public:
        MatrixAnimator();
        MatrixAnimator(const Matrix4& start, const Matrix4& end, float32 duration);
        
        void setStart(const Matrix4& start);
        void setEnd(const Matrix4& end);
        void setDuration(float32 duration);
        void setLoop(bool loop);
        void setPingPong(bool pingPong);
        
        Matrix4 update(float32 deltaTime);
        Matrix4 getValue(float32 t) const;
        
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
        Matrix4 start_;
        Matrix4 end_;
        float32 duration_;
        float32 time_;
        bool playing_;
        bool loop_;
        bool pingPong_;
        bool forward_;
    };
    
    // Matrix cache
    class MatrixCache {
    public:
        MatrixCache(size_t maxSize = 1000);
        
        void put(const std::string& key, const Matrix4& matrix);
        Matrix4 get(const std::string& key) const;
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
            Matrix4 matrix;
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
    class MatrixPool {
    public:
        MatrixPool(size_t initialSize = 100);
        
        Matrix4* acquire();
        void release(Matrix4* matrix);
        void clear();
        
        size_t getPoolSize() const;
        size_t getUsedCount() const;
        size_t getAvailableCount() const;
        
    private:
        std::vector<std::unique_ptr<Matrix4>> pool_;
        std::vector<Matrix4*> available_;
        std::vector<Matrix4*> used_;
        
        void expandPool(size_t newSize);
    };
    
    // Matrix stack
    class MatrixStack {
    public:
        MatrixStack();
        
        void push(const Matrix4& matrix);
        void pop();
        void clear();
        
        Matrix4 top() const;
        size_t size() const;
        bool empty() const;
        
        Matrix4 multiply(const Matrix4& matrix);
        Matrix4 translate(const Vector3& translation);
        Matrix4 rotate(const Quaternion& rotation);
        Matrix4 scale(const Vector3& scale);
        
    private:
        std::vector<Matrix4> stack_;
    };
    
    // Matrix builder
    class MatrixBuilder {
    public:
        MatrixBuilder();
        
        MatrixBuilder& identity();
        MatrixBuilder& zero();
        MatrixBuilder& translation(const Vector3& translation);
        MatrixBuilder& translation(float32 x, float32 y, float32 z);
        MatrixBuilder& rotation(const Quaternion& rotation);
        MatrixBuilder& rotation(const Vector3& axis, float32 angle);
        MatrixBuilder& rotationX(float32 angle);
        MatrixBuilder& rotationY(float32 angle);
        MatrixBuilder& rotationZ(float32 angle);
        MatrixBuilder& scale(const Vector3& scale);
        MatrixBuilder& scale(float32 scale);
        MatrixBuilder& scale(float32 scaleX, float32 scaleY, float32 scaleZ);
        
        MatrixBuilder& perspective(float32 fov, float32 aspect, float32 near, float32 far);
        MatrixBuilder& orthographic(float32 left, float32 right, float32 bottom, float32 top, float32 near, float32 far);
        MatrixBuilder& lookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
        
        MatrixBuilder& multiply(const Matrix4& matrix);
        MatrixBuilder& transpose();
        MatrixBuilder& inverse();
        
        Matrix4 build() const;
        operator Matrix4() const;
        
    private:
        Matrix4 matrix_;
    };
    
    // Matrix utilities
    Matrix4 createBillboard(const Vector3& position, const Vector3& cameraPosition, const Vector3& cameraUp, const Vector3& cameraRight);
    Matrix4 createConstrainedBillboard(const Vector3& position, const Vector3& cameraPosition, const Vector3& cameraUp, const Vector3& cameraRight, const Vector3& objectAxis);
    
    Matrix4 createReflection(const Vector3& normal);
    Matrix4 createReflection(const Vector3& normal, const Vector3& point);
    Matrix4 createReflection(const Vector4& plane);
    
    Matrix4 createShadow(const Vector3& lightDirection, const Vector4& plane);
    Matrix4 createShadow(const Vector3& lightPosition, const Vector4& plane);
    
    Matrix4 createShear(float32 xy, float32 xz, float32 yx, float32 yz, float32 zx, float32 zy);
    Matrix4 createShearX(float32 y, float32 z);
    Matrix4 createShearY(float32 x, float32 z);
    Matrix4 createShearZ(float32 x, float32 y);
    
    Matrix4 createNormalMatrix(const Matrix4& matrix);
    Matrix4 createInverseTranspose(const Matrix4& matrix);
    
    // Matrix analysis
    struct MatrixAnalysis {
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
        Vector4 eigenvectors[4];
    };
    
    MatrixAnalysis analyze(const Matrix4& matrix);
    
    // Matrix optimization
    Matrix4 optimize(const Matrix4& matrix);
    Matrix4 compress(const Matrix4& matrix);
    Matrix4 decompress(const Matrix4& matrix);
    
    // Matrix validation
    bool validate(const Matrix4& matrix);
    bool validateTransform(const Matrix4& matrix);
    bool validateProjection(const Matrix4& matrix);
    bool validateView(const Matrix4& matrix);
    
    // Matrix debugging
    void debugPrintMatrix(const Matrix4& matrix);
    void debugPrintMatrixFormatted(const Matrix4& matrix);
    void debugPrintMatrixAnalysis(const Matrix4& matrix);
}

// Hash function for Matrix4
struct Matrix4Hash {
    size_t operator()(const Matrix4& matrix) const noexcept {
        size_t h1 = std::hash<float32>{}(matrix.m00);
        size_t h2 = std::hash<float32>{}(matrix.m01);
        size_t h3 = std::hash<float32>{}(matrix.m02);
        size_t h4 = std::hash<float32>{}(matrix.m03);
        size_t h5 = std::hash<float32>{}(matrix.m10);
        size_t h6 = std::hash<float32>{}(matrix.m11);
        size_t h7 = std::hash<float32>{}(matrix.m12);
        size_t h8 = std::hash<float32>{}(matrix.m13);
        size_t h9 = std::hash<float32>{}(matrix.m20);
        size_t h10 = std::hash<float32>{}(matrix.m21);
        size_t h11 = std::hash<float32>{}(matrix.m22);
        size_t h12 = std::hash<float32>{}(matrix.m23);
        size_t h13 = std::hash<float32>{}(matrix.m30);
        size_t h14 = std::hash<float32>{}(matrix.m31);
        size_t h15 = std::hash<float32>{}(matrix.m32);
        size_t h16 = std::hash<float32>{}(matrix.m33);
        
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ 
               (h5 << 4) ^ (h6 << 5) ^ (h7 << 6) ^ (h8 << 7) ^
               (h9 << 8) ^ (h10 << 9) ^ (h11 << 10) ^ (h12 << 11) ^
               (h13 << 12) ^ (h14 << 13) ^ (h15 << 14) ^ (h16 << 15);
    }
};

} // namespace RFUtils