#version 330 core

// Common OpenGL Shader Functions and Utilities
// Shared utility functions for all OpenGL shaders

// Constants
const float PI = 3.14159265358979323846264338327950288419716939937510;
const float TWO_PI = 6.28318530717958647692528676655900576839433872975021;
const float HALF_PI = 1.57079632679489661923132169163975144209858469968755;
const float INV_PI = 0.31830988618379067153776752674502872406891929148091;
const float DEG_TO_RAD = PI / 180.0;
const float RAD_TO_DEG = 180.0 / PI;
const float GOLDEN_RATIO = 1.61803398874989484820458683436563811772030917980576;

// Mathematical Functions
float saturate(float value)
{
    return clamp(value, 0.0, 1.0);
}

vec2 saturate(vec2 value)
{
    return clamp(value, vec2(0.0), vec2(1.0));
}

vec3 saturate(vec3 value)
{
    return clamp(value, vec3(0.0), vec3(1.0));
}

vec4 saturate(vec4 value)
{
    return clamp(value, vec4(0.0), vec4(1.0));
}

float smoothstep01(float edge0, float edge1, float x)
{
    return smoothstep(edge0, edge1, saturate(x));
}

vec2 smoothstep01(vec2 edge0, vec2 edge1, vec2 x)
{
    return smoothstep(edge0, edge1, saturate(x));
}

vec3 smoothstep01(vec3 edge0, vec3 edge1, vec3 x)
{
    return smoothstep(edge0, edge1, saturate(x));
}

vec4 smoothstep01(vec4 edge0, vec4 edge1, vec4 x)
{
    return smoothstep(edge0, edge1, saturate(x));
}

float linearstep(float edge0, float edge1, float x)
{
    return saturate((x - edge0) / (edge1 - edge0));
}

vec2 linearstep(vec2 edge0, vec2 edge1, vec2 x)
{
    return saturate((x - edge0) / (edge1 - edge0));
}

vec3 linearstep(vec3 edge0, vec3 edge1, vec3 x)
{
    return saturate((x - edge0) / (edge1 - edge0));
}

vec4 linearstep(vec4 edge0, vec4 edge1, vec4 x)
{
    return saturate((x - edge0) / (edge1 - edge0));
}

// Vector Operations
float length2(vec2 v)
{
    return dot(v, v);
}

float length2(vec3 v)
{
    return dot(v, v);
}

float length2(vec4 v)
{
    return dot(v, v);
}

float distance2(vec2 a, vec2 b)
{
    return dot(a - b, a - b);
}

float distance2(vec3 a, vec3 b)
{
    return dot(a - b, a - b);
}

float distance2(vec4 a, vec4 b)
{
    return dot(a - b, a - b);
}

vec2 normalize2(vec2 v)
{
    float len = length(v);
    return len > 0.0 ? v / len : vec2(0.0);
}

vec3 normalize2(vec3 v)
{
    float len = length(v);
    return len > 0.0 ? v / len : vec3(0.0);
}

vec4 normalize2(vec4 v)
{
    float len = length(v);
    return len > 0.0 ? v / len : vec4(0.0);
}

vec2 reflect2(vec2 I, vec2 N)
{
    return I - 2.0 * dot(N, I) * N;
}

vec3 reflect3(vec3 I, vec3 N)
{
    return I - 2.0 * dot(N, I) * N;
}

vec4 reflect4(vec4 I, vec4 N)
{
    return I - 2.0 * dot(N, I) * N;
}

vec2 refract2(vec2 I, vec2 N, float eta)
{
    float k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
    if (k < 0.0)
        return vec2(0.0);
    else
        return eta * I - (eta * dot(N, I) + sqrt(k)) * N;
}

vec3 refract3(vec3 I, vec3 N, float eta)
{
    float k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
    if (k < 0.0)
        return vec3(0.0);
    else
        return eta * I - (eta * dot(N, I) + sqrt(k)) * N;
}

vec4 refract4(vec4 I, vec4 N, float eta)
{
    float k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
    if (k < 0.0)
        return vec4(0.0);
    else
        return eta * I - (eta * dot(N, I) + sqrt(k)) * N;
}

// Color Operations
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 rgb2hsl(vec3 c)
{
    float maxVal = max(c.r, max(c.g, c.b));
    float minVal = min(c.r, min(c.g, c.b));
    float delta = maxVal - minVal;
    
    vec3 hsl = vec3(0.0, 0.0, (maxVal + minVal) * 0.5);
    
    if (delta > 0.0)
    {
        hsl.y = (hsl.z <= 0.5) ? (delta / (maxVal + minVal)) : (delta / (2.0 - maxVal - minVal));
        
        if (maxVal == c.r)
            hsl.x = (c.g - c.b) / delta + (c.g < c.b ? 6.0 : 0.0);
        else if (maxVal == c.g)
            hsl.x = (c.b - c.r) / delta + 2.0;
        else
            hsl.x = (c.r - c.g) / delta + 4.0;
        
        hsl.x /= 6.0;
    }
    
    return hsl;
}

vec3 hsl2rgb(vec3 c)
{
    float h = c.x * 6.0;
    float s = c.y;
    float l = c.z;
    
    float c1 = (1.0 - abs(2.0 * l - 1.0)) * s;
    float x = c1 * (1.0 - abs(mod(h, 2.0) - 1.0));
    float m = l - c1 * 0.5;
    
    vec3 rgb;
    
    if (h < 1.0)
        rgb = vec3(c1, x, 0.0);
    else if (h < 2.0)
        rgb = vec3(x, c1, 0.0);
    else if (h < 3.0)
        rgb = vec3(0.0, c1, x);
    else if (h < 4.0)
        rgb = vec3(0.0, x, c1);
    else if (h < 5.0)
        rgb = vec3(x, 0.0, c1);
    else
        rgb = vec3(c1, 0.0, x);
    
    return rgb + m;
}

vec3 rgb2ycbcr(vec3 c)
{
    float y = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
    float cb = (c.b - y) * 0.564 + 0.5;
    float cr = (c.r - y) * 0.713 + 0.5;
    return vec3(y, cb, cr);
}

vec3 ycbcr2rgb(vec3 c)
{
    float y = c.x;
    float cb = c.y - 0.5;
    float cr = c.z - 0.5;
    
    float r = y + 1.403 * cr;
    float g = y - 0.344 * cb - 0.714 * cr;
    float b = y + 1.773 * cb;
    
    return vec3(r, g, b);
}

// Matrix Operations
mat4 translate(mat4 m, vec3 v)
{
    return mat4(
        m[0][0], m[0][1], m[0][2], m[0][3],
        m[1][0], m[1][1], m[1][2], m[1][3],
        m[2][0], m[2][1], m[2][2], m[2][3],
        m[3][0] + v.x, m[3][1] + v.y, m[3][2] + v.z, m[3][3]
    );
}

mat4 rotate(mat4 m, float angle, vec3 axis)
{
    float a = angle;
    float c = cos(a);
    float s = sin(a);
    
    axis = normalize(axis);
    vec3 temp = (1.0 - c) * axis;
    
    mat4 rot = mat4(
        c + temp.x * axis.x, temp.x * axis.y + s * axis.z, temp.x * axis.z - s * axis.y, 0.0,
        temp.y * axis.x - s * axis.z, c + temp.y * axis.y, temp.y * axis.z + s * axis.x, 0.0,
        temp.z * axis.x + s * axis.y, temp.z * axis.y - s * axis.x, c + temp.z * axis.z, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    
    return m * rot;
}

mat4 scale(mat4 m, vec3 v)
{
    return mat4(
        m[0][0] * v.x, m[0][1], m[0][2], m[0][3],
        m[1][0], m[1][1] * v.y, m[1][2], m[1][3],
        m[2][0], m[2][1], m[2][2] * v.z, m[2][3],
        m[3][0], m[3][1], m[3][2], m[3][3]
    );
}

mat4 orthographic(float left, float right, float bottom, float top, float near, float far)
{
    return mat4(
        2.0 / (right - left), 0.0, 0.0, 0.0,
        0.0, 2.0 / (top - bottom), 0.0, 0.0,
        0.0, 0.0, -2.0 / (far - near), 0.0,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1.0
    );
}

mat4 perspective(float fov, float aspect, float near, float far)
{
    float f = 1.0 / tan(fov * 0.5);
    return mat4(
        f / aspect, 0.0, 0.0, 0.0,
        0.0, f, 0.0, 0.0,
        0.0, 0.0, (far + near) / (near - far), -1.0,
        0.0, 0.0, (2.0 * far * near) / (near - far), 0.0
    );
}

mat4 lookAt(vec3 eye, vec3 center, vec3 up)
{
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    
    return mat4(
        s.x, u.x, -f.x, 0.0,
        s.y, u.y, -f.y, 0.0,
        s.z, u.z, -f.z, 0.0,
        -dot(s, eye), -dot(u, eye), dot(f, eye), 1.0
    );
}

// Quaternion Operations
vec4 quatFromAxisAngle(vec3 axis, float angle)
{
    axis = normalize(axis);
    float halfAngle = angle * 0.5;
    float s = sin(halfAngle);
    return vec4(axis * s, cos(halfAngle));
}

vec4 quatFromEuler(vec3 euler)
{
    vec3 halfEuler = euler * 0.5;
    float cx = cos(halfEuler.x);
    float sx = sin(halfEuler.x);
    float cy = cos(halfEuler.y);
    float sy = sin(halfEuler.y);
    float cz = cos(halfEuler.z);
    float sz = sin(halfEuler.z);
    
    return vec4(
        sx * cy * cz - cx * sy * sz,
        cx * sy * cz + sx * cy * sz,
        cx * cy * sz - sx * sy * cz,
        cx * cy * cz + sx * sy * sz
    );
}

vec4 quatMultiply(vec4 q1, vec4 q2)
{
    return vec4(
        q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
        q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
        q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
        q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
    );
}

vec4 quatConjugate(vec4 q)
{
    return vec4(-q.x, -q.y, -q.z, q.w);
}

vec4 quatInverse(vec4 q)
{
    return quatConjugate(q) / dot(q, q);
}

vec3 quatRotate(vec4 q, vec3 v)
{
    return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

mat4 quatToMatrix(vec4 q)
{
    q = normalize(q);
    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;
    
    return mat4(
        1.0 - 2.0 * (yy + zz), 2.0 * (xy - wz), 2.0 * (xz + wy), 0.0,
        2.0 * (xy + wz), 1.0 - 2.0 * (xx + zz), 2.0 * (yz - wx), 0.0,
        2.0 * (xz - wy), 2.0 * (yz + wx), 1.0 - 2.0 * (xx + yy), 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

// Texture Operations
vec2 rotateUV(vec2 uv, float angle, vec2 center)
{
    float s = sin(angle);
    float c = cos(angle);
    uv -= center;
    return vec2(uv.x * c - uv.y * s, uv.x * s + uv.y * c) + center;
}

vec2 scaleUV(vec2 uv, vec2 scale, vec2 center)
{
    return (uv - center) * scale + center;
}

vec2 translateUV(vec2 uv, vec2 offset)
{
    return uv + offset;
}

vec2 flipUV(vec2 uv, bool flipX, bool flipY)
{
    return vec2(flipX ? 1.0 - uv.x : uv.x, flipY ? 1.0 - uv.y : uv.y);
}

vec2 repeatUV(vec2 uv, vec2 repeat)
{
    return mod(uv * repeat, 1.0);
}

vec2 mirrorUV(vec2 uv, vec2 repeat)
{
    vec2 repeated = mod(uv * repeat, 2.0);
    return mix(repeated, 2.0 - repeated, step(1.0, repeated));
}

// Random Functions
float random(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float random(vec3 st)
{
    return fract(sin(dot(st.xyz, vec3(12.9898, 78.233, 37.719))) * 43758.5453123);
}

float random(vec4 st)
{
    return fract(sin(dot(st.xyzw, vec4(12.9898, 78.233, 37.719, 62.466))) * 43758.5453123);
}

vec2 random2(vec2 st)
{
    return vec2(
        random(st),
        random(st + vec2(0.1, 0.1))
    );
}

vec3 random3(vec3 st)
{
    return vec3(
        random(st),
        random(st + vec3(0.1, 0.1, 0.1)),
        random(st + vec3(0.2, 0.2, 0.2))
    );
}

vec4 random4(vec4 st)
{
    return vec4(
        random(st),
        random(st + vec4(0.1, 0.1, 0.1, 0.1)),
        random(st + vec4(0.2, 0.2, 0.2, 0.2)),
        random(st + vec4(0.3, 0.3, 0.3, 0.3))
    );
}

// Interpolation Functions
float cubicInterpolate(float a, float b, float c, float d, float t)
{
    float p = d - c - (a - b);
    float q = a - b - p;
    float r = c - a;
    return p * t * t * t + q * t * t + r * t + b;
}

vec2 cubicInterpolate(vec2 a, vec2 b, vec2 c, vec2 d, float t)
{
    return vec2(
        cubicInterpolate(a.x, b.x, c.x, d.x, t),
        cubicInterpolate(a.y, b.y, c.y, d.y, t)
    );
}

vec3 cubicInterpolate(vec3 a, vec3 b, vec3 c, vec3 d, float t)
{
    return vec3(
        cubicInterpolate(a.x, b.x, c.x, d.x, t),
        cubicInterpolate(a.y, b.y, c.y, d.y, t),
        cubicInterpolate(a.z, b.z, c.z, d.z, t)
    );
}

vec4 cubicInterpolate(vec4 a, vec4 b, vec4 c, vec4 d, float t)
{
    return vec4(
        cubicInterpolate(a.x, b.x, c.x, d.x, t),
        cubicInterpolate(a.y, b.y, c.y, d.y, t),
        cubicInterpolate(a.z, b.z, c.z, d.z, t),
        cubicInterpolate(a.w, b.w, c.w, d.w, t)
    );
}

// Utility Functions
float remap(float value, float fromMin, float fromMax, float toMin, float toMax)
{
    return toMin + (value - fromMin) * (toMax - toMin) / (fromMax - fromMin);
}

vec2 remap(vec2 value, vec2 fromMin, vec2 fromMax, vec2 toMin, vec2 toMax)
{
    return toMin + (value - fromMin) * (toMax - toMin) / (fromMax - fromMin);
}

vec3 remap(vec3 value, vec3 fromMin, vec3 fromMax, vec3 toMin, vec3 toMax)
{
    return toMin + (value - fromMin) * (toMax - toMin) / (fromMax - fromMin);
}

vec4 remap(vec4 value, vec4 fromMin, vec4 fromMax, vec4 toMin, vec4 toMax)
{
    return toMin + (value - fromMin) * (toMax - toMin) / (fromMax - fromMin);
}

float linstep(float edge0, float edge1, float x)
{
    return saturate((x - edge0) / (edge1 - edge0));
}

vec2 linstep(vec2 edge0, vec2 edge1, vec2 x)
{
    return saturate((x - edge0) / (edge1 - edge0));
}

vec3 linstep(vec3 edge0, vec3 edge1, vec3 x)
{
    return saturate((x - edge0) / (edge1 - edge0));
}

vec4 linstep(vec4 edge0, vec4 edge1, vec4 x)
{
    return saturate((x - edge0) / (edge1 - edge0));
}

float inverseLerp(float a, float b, float value)
{
    return (value - a) / (b - a);
}

vec2 inverseLerp(vec2 a, vec2 b, vec2 value)
{
    return (value - a) / (b - a);
}

vec3 inverseLerp(vec3 a, vec3 b, vec3 value)
{
    return (value - a) / (b - a);
}

vec4 inverseLerp(vec4 a, vec4 b, vec4 value)
{
    return (value - a) / (b - a);
}

float pingpong(float t, float length)
{
    return length - abs(mod(t, length * 2.0) - length);
}

vec2 pingpong(vec2 t, vec2 length)
{
    return length - abs(mod(t, length * 2.0) - length);
}

vec3 pingpong(vec3 t, vec3 length)
{
    return length - abs(mod(t, length * 2.0) - length);
}

vec4 pingpong(vec4 t, vec4 length)
{
    return length - abs(mod(t, length * 2.0) - length);
}

// Gamma Correction
vec3 gammaCorrect(vec3 color, float gamma)
{
    return pow(color, vec3(1.0 / gamma));
}

vec3 gammaUncorrect(vec3 color, float gamma)
{
    return pow(color, vec3(gamma));
}

// Tone Mapping
vec3 reinhard(vec3 color)
{
    return color / (1.0 + color);
}

vec3 reinhardLuminance(vec3 color)
{
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    return color * (luminance / (1.0 + luminance));
}

vec3 aces(vec3 color)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return saturate((color * (a * color + b)) / (color * (c * color + d) + e));
}

// Fog Functions
float linearFog(float distance, float start, float end)
{
    return saturate((end - distance) / (end - start));
}

float exponentialFog(float distance, float density)
{
    return saturate(exp(-density * distance));
}

float exponentialSquaredFog(float distance, float density)
{
    return saturate(exp(-density * density * distance * distance));
}

// Screen Space Effects
vec2 screenSpaceToUV(vec2 screenPos, vec2 resolution)
{
    return screenPos / resolution;
}

vec2 uvToScreenSpace(vec2 uv, vec2 resolution)
{
    return uv * resolution;
}

vec3 worldToScreen(vec3 worldPos, mat4 viewProjection, vec2 resolution)
{
    vec4 clipPos = viewProjection * vec4(worldPos, 1.0);
    vec3 ndcPos = clipPos.xyz / clipPos.w;
    vec2 screenPos = (ndcPos.xy * 0.5 + 0.5) * resolution;
    return vec3(screenPos, ndcPos.z);
}

vec3 screenToWorld(vec3 screenPos, mat4 inverseViewProjection, vec2 resolution)
{
    vec2 ndcPos = (screenPos.xy / resolution) * 2.0 - 1.0;
    vec4 clipPos = vec4(ndcPos, screenPos.z, 1.0);
    vec4 worldPos = inverseViewProjection * clipPos;
    return worldPos.xyz / worldPos.w;
}

// Depth Functions
float linearizeDepth(float depth, float near, float far)
{
    return (2.0 * near) / (far + near - depth * (far - near));
}

float viewSpaceDepth(float depth, float near, float far)
{
    return (2.0 * near * far) / (far + near - depth * (far - near));
}

// Utility Macros
#define EPSILON 1e-6
#define INFINITY 1e6
#define PI_OVER_2 (PI * 0.5)
#define PI_OVER_4 (PI * 0.25)
#define TWO_PI_INV (1.0 / TWO_PI)
#define GOLDEN_RATIO_INV (1.0 / GOLDEN_RATIO)

// Common Constants
const vec3 BLACK = vec3(0.0, 0.0, 0.0);
const vec3 WHITE = vec3(1.0, 1.0, 1.0);
const vec3 RED = vec3(1.0, 0.0, 0.0);
const vec3 GREEN = vec3(0.0, 1.0, 0.0);
const vec3 BLUE = vec3(0.0, 0.0, 1.0);
const vec3 YELLOW = vec3(1.0, 1.0, 0.0);
const vec3 CYAN = vec3(0.0, 1.0, 1.0);
const vec3 MAGENTA = vec3(1.0, 0.0, 1.0);

const vec4 BLACK4 = vec4(0.0, 0.0, 0.0, 1.0);
const vec4 WHITE4 = vec4(1.0, 1.0, 1.0, 1.0);
const vec4 RED4 = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 GREEN4 = vec4(0.0, 1.0, 0.0, 1.0);
const vec4 BLUE4 = vec4(0.0, 0.0, 1.0, 1.0);
const vec4 YELLOW4 = vec4(1.0, 1.0, 0.0, 1.0);
const vec4 CYAN4 = vec4(0.0, 1.0, 1.0, 1.0);
const vec4 MAGENTA4 = vec4(1.0, 0.0, 1.0, 1.0);

// Common Matrices
const mat3 IDENTITY3 = mat3(
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0
);

const mat4 IDENTITY4 = mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
);

// Common Vectors
const vec3 UP = vec3(0.0, 1.0, 0.0);
const vec3 DOWN = vec3(0.0, -1.0, 0.0);
const vec3 RIGHT = vec3(1.0, 0.0, 0.0);
const vec3 LEFT = vec3(-1.0, 0.0, 0.0);
const vec3 FORWARD = vec3(0.0, 0.0, -1.0);
const vec3 BACKWARD = vec3(0.0, 0.0, 1.0);

// Debug Functions (for development)
#ifdef DEBUG
vec3 debugColor(float value)
{
    if (value < 0.0) return vec3(1.0, 0.0, 0.0); // Red for negative
    if (value > 1.0) return vec3(0.0, 0.0, 1.0); // Blue for >1
    return vec3(value, value, value); // Grayscale for 0-1
}

vec3 debugColor(vec2 value)
{
    return vec3(value.x, value.y, 0.5);
}

vec3 debugColor(vec3 value)
{
    return value;
}

vec3 debugColor(vec4 value)
{
    return value.rgb;
}
#endif