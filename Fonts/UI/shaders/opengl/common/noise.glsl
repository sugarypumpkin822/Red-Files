#version 330 core

// Common OpenGL Noise Functions
// Shared noise generation functions for all OpenGL shaders

// Pseudo-random number generator
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

// 2D Noise Functions
float noise2D(vec2 st)
{
    vec2 i = floor(st);
    vec2 f = fract(st);
    
    // Smoothstep interpolation
    vec2 u = f * f * (3.0 - 2.0 * f);
    
    // Four corners of the grid cell
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    
    // Mix the four corners
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float noise2D(vec2 st, int octaves, float persistence, float lacunarity)
{
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float maxValue = 0.0;
    
    for (int i = 0; i < octaves; i++)
    {
        value += amplitude * noise2D(st * frequency);
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return value / maxValue;
}

float fractalNoise2D(vec2 st, int octaves, float persistence, float lacunarity)
{
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    
    for (int i = 0; i < octaves; i++)
    {
        value += amplitude * noise2D(st * frequency);
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return value;
}

float turbulence2D(vec2 st, int octaves, float persistence, float lacunarity)
{
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    
    for (int i = 0; i < octaves; i++)
    {
        value += amplitude * abs(noise2D(st * frequency) * 2.0 - 1.0);
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return value;
}

float ridgedNoise2D(vec2 st, int octaves, float persistence, float lacunarity)
{
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float weight = 1.0;
    
    for (int i = 0; i < octaves; i++)
    {
        float signal = abs(noise2D(st * frequency));
        signal = 1.0 - signal;
        signal *= signal;
        signal *= weight;
        
        value += signal * amplitude;
        weight = clamp(signal, 0.0, 1.0);
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return value;
}

// 3D Noise Functions
float noise3D(vec3 st)
{
    vec3 i = floor(st);
    vec3 f = fract(st);
    
    // Smoothstep interpolation
    vec3 u = f * f * (3.0 - 2.0 * f);
    
    // Eight corners of the grid cell
    float a = random(i);
    float b = random(i + vec3(1.0, 0.0, 0.0));
    float c = random(i + vec3(0.0, 1.0, 0.0));
    float d = random(i + vec3(1.0, 1.0, 0.0));
    float e = random(i + vec3(0.0, 0.0, 1.0));
    float fval = random(i + vec3(1.0, 0.0, 1.0));
    float g = random(i + vec3(0.0, 1.0, 1.0));
    float h = random(i + vec3(1.0, 1.0, 1.0));
    
    // Trilinear interpolation
    float x1 = mix(a, b, u.x);
    float x2 = mix(c, d, u.x);
    float x3 = mix(e, fval, u.x);
    float x4 = mix(g, h, u.x);
    
    float y1 = mix(x1, x2, u.y);
    float y2 = mix(x3, x4, u.y);
    
    return mix(y1, y2, u.z);
}

float noise3D(vec3 st, int octaves, float persistence, float lacunarity)
{
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float maxValue = 0.0;
    
    for (int i = 0; i < octaves; i++)
    {
        value += amplitude * noise3D(st * frequency);
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return value / maxValue;
}

float fractalNoise3D(vec3 st, int octaves, float persistence, float lacunarity)
{
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    
    for (int i = 0; i < octaves; i++)
    {
        value += amplitude * noise3D(st * frequency);
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return value;
}

float turbulence3D(vec3 st, int octaves, float persistence, float lacunarity)
{
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    
    for (int i = 0; i < octaves; i++)
    {
        value += amplitude * abs(noise3D(st * frequency) * 2.0 - 1.0);
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return value;
}

float ridgedNoise3D(vec3 st, int octaves, float persistence, float lacunarity)
{
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float weight = 1.0;
    
    for (int i = 0; i < octaves; i++)
    {
        float signal = abs(noise3D(st * frequency));
        signal = 1.0 - signal;
        signal *= signal;
        signal *= weight;
        
        value += signal * amplitude;
        weight = clamp(signal, 0.0, 1.0);
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return value;
}

// 4D Noise Functions
float noise4D(vec4 st)
{
    vec4 i = floor(st);
    vec4 f = fract(st);
    
    // Smoothstep interpolation
    vec4 u = f * f * (3.0 - 2.0 * f);
    
    // Sixteen corners of the 4D grid cell
    float a = random(i);
    float b = random(i + vec4(1.0, 0.0, 0.0, 0.0));
    float c = random(i + vec4(0.0, 1.0, 0.0, 0.0));
    float d = random(i + vec4(1.0, 1.0, 0.0, 0.0));
    float e = random(i + vec4(0.0, 0.0, 1.0, 0.0));
    float fval = random(i + vec4(1.0, 0.0, 1.0, 0.0));
    float g = random(i + vec4(0.0, 1.0, 1.0, 0.0));
    float h = random(i + vec4(1.0, 1.0, 1.0, 0.0));
    float i2 = random(i + vec4(0.0, 0.0, 0.0, 1.0));
    float j = random(i + vec4(1.0, 0.0, 0.0, 1.0));
    float k = random(i + vec4(0.0, 1.0, 0.0, 1.0));
    float l = random(i + vec4(1.0, 1.0, 0.0, 1.0));
    float m = random(i + vec4(0.0, 0.0, 1.0, 1.0));
    float n = random(i + vec4(1.0, 0.0, 1.0, 1.0));
    float o = random(i + vec4(0.0, 1.0, 1.0, 1.0));
    float p = random(i + vec4(1.0, 1.0, 1.0, 1.0));
    
    // Trilinear interpolation in 4D
    float x1 = mix(a, b, u.x);
    float x2 = mix(c, d, u.x);
    float x3 = mix(e, fval, u.x);
    float x4 = mix(g, h, u.x);
    float x5 = mix(i2, j, u.x);
    float x6 = mix(k, l, u.x);
    float x7 = mix(m, n, u.x);
    float x8 = mix(o, p, u.x);
    
    float y1 = mix(x1, x2, u.y);
    float y2 = mix(x3, x4, u.y);
    float y3 = mix(x5, x6, u.y);
    float y4 = mix(x7, x8, u.y);
    
    float z1 = mix(y1, y2, u.z);
    float z2 = mix(y3, y4, u.z);
    
    return mix(z1, z2, u.w);
}

float noise4D(vec4 st, int octaves, float persistence, float lacunarity)
{
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float maxValue = 0.0;
    
    for (int i = 0; i < octaves; i++)
    {
        value += amplitude * noise4D(st * frequency);
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return value / maxValue;
}

float fractalNoise4D(vec4 st, int octaves, float persistence, float lacunarity)
{
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    
    for (int i = 0; i < octaves; i++)
    {
        value += amplitude * noise4D(st * frequency);
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return value;
}

// Perlin-like gradient noise
vec2 gradient2D(vec2 st)
{
    st = st * 2.0 - 1.0;
    float angle = random(st) * TWO_PI;
    return vec2(cos(angle), sin(angle));
}

float dotGradient2D(vec2 st, vec2 gradient)
{
    return dot(st, gradient);
}

float perlinNoise2D(vec2 st)
{
    vec2 i = floor(st);
    vec2 f = fract(st);
    
    // Smoothstep interpolation
    vec2 u = f * f * (3.0 - 2.0 * f);
    
    // Four corners with gradients
    vec2 g00 = gradient2D(i);
    vec2 g10 = gradient2D(i + vec2(1.0, 0.0));
    vec2 g01 = gradient2D(i + vec2(0.0, 1.0));
    vec2 g11 = gradient2D(i + vec2(1.0, 1.0));
    
    // Dot products
    float n00 = dotGradient2D(f, g00);
    float n10 = dotGradient2D(f - vec2(1.0, 0.0), g10);
    float n01 = dotGradient2D(f - vec2(0.0, 1.0), g01);
    float n11 = dotGradient2D(f - vec2(1.0, 1.0), g11);
    
    // Mix the four corners
    return mix(mix(n00, n10, u.x), mix(n01, n11, u.x), u.y);
}

// Worley Noise (Cellular Noise)
vec2 worleyNoise2D(vec2 st)
{
    vec2 i = floor(st);
    vec2 f = fract(st);
    
    float minDist = 1.0;
    vec2 minPoint = vec2(0.0);
    
    // Check surrounding cells
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            vec2 neighbor = vec2(float(x), float(y));
            vec2 point = random2(i + neighbor);
            vec2 diff = neighbor + point - f;
            float dist = length(diff);
            
            if (dist < minDist)
            {
                minDist = dist;
                minPoint = point;
            }
        }
    }
    
    return vec2(minDist, minPoint.x);
}

float worleyNoise2D(vec2 st, int cellType)
{
    vec2 i = floor(st);
    vec2 f = fract(st);
    
    float minDist = 1.0;
    float secondMinDist = 1.0;
    
    // Check surrounding cells
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            vec2 neighbor = vec2(float(x), float(y));
            vec2 point = random2(i + neighbor);
            vec2 diff = neighbor + point - f;
            float dist = length(diff);
            
            if (dist < minDist)
            {
                secondMinDist = minDist;
                minDist = dist;
            }
            else if (dist < secondMinDist)
            {
                secondMinDist = dist;
            }
        }
    }
    
    switch (cellType)
    {
        case 0: return minDist;                    // F1
        case 1: return secondMinDist - minDist;     // F2-F1
        case 2: return secondMinDist;               // F2
        default: return minDist;
    }
}

// 3D Worley Noise
vec3 worleyNoise3D(vec3 st)
{
    vec3 i = floor(st);
    vec3 f = fract(st);
    
    float minDist = 1.0;
    vec3 minPoint = vec3(0.0);
    
    // Check surrounding cells
    for (int z = -1; z <= 1; z++)
    {
        for (int y = -1; y <= 1; y++)
        {
            for (int x = -1; x <= 1; x++)
            {
                vec3 neighbor = vec3(float(x), float(y), float(z));
                vec3 point = random3(i + neighbor);
                vec3 diff = neighbor + point - f;
                float dist = length(diff);
                
                if (dist < minDist)
                {
                    minDist = dist;
                    minPoint = point;
                }
            }
        }
    }
    
    return vec3(minDist, minPoint.x, minPoint.y);
}

// Simplex Noise (2D)
vec2 simplex2D(vec2 st)
{
    const float F1 = 0.3660254037844386; // (sqrt(3) - 1) / 6
    const float G1 = 0.21132486540518713; // (3 - sqrt(3)) / 6
    
    // Skew the input space to determine which simplex cell we're in
    vec2 s = (st.x + st.y) * F1;
    vec2 ij = floor(st + s);
    vec2 t = (ij.x + ij.y) * G1;
    vec2 uv0 = ij - t;
    
    // Unskew the cell origin back to (x,y) space
    vec2 f0 = st - uv0;
    
    // For the 2D case, the simplex shape is an equilateral triangle
    vec2 i1 = (f0.x > f0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    
    // Step 2: Determine the three simplex corners
    vec2 uv1 = f0 + i1 + G1;
    vec2 uv2 = f0 + vec2(1.0, 1.0) + 2.0 * G1;
    
    // Calculate the contribution from the three corners
    vec3 m = max(0.5 - vec3(dot(f0, f0), dot(uv1, uv1), dot(uv2, uv2)), 0.0);
    
    // Calculate noise value
    vec3 g = vec3(random(ij), random(ij + i1), random(ij + vec2(1.0, 1.0)));
    return 70.0 * dot(m, g);
}

float simplexNoise2D(vec2 st)
{
    return simplex2D(st).x;
}

// Value Noise with Hermite Interpolation
float valueNoise2D(vec2 st)
{
    vec2 i = floor(st);
    vec2 f = fract(st);
    
    // Hermite interpolation
    vec2 u = f * f * (3.0 - 2.0 * f);
    
    // Four corners of the grid cell
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    
    // Mix the four corners
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

// Marble Noise
float marbleNoise2D(vec2 st, float frequency, float amplitude)
{
    float value = 0.0;
    float amp = amplitude;
    float freq = frequency;
    
    for (int i = 0; i < 6; i++)
    {
        value += abs(sin(st.x * freq + noise2D(st * freq) * amp));
        freq *= 2.0;
        amp *= 0.5;
    }
    
    return value / 6.0;
}

// Wood Noise
float woodNoise2D(vec2 st, float frequency)
{
    vec2 p = st * frequency;
    float n = noise2D(p);
    return abs(frac(n + 0.5) - 0.5) * 2.0;
}

// Fire Noise
float fireNoise2D(vec2 st, float time)
{
    vec2 p = st * 2.0;
    p.y -= time * 0.5;
    
    float n = 0.0;
    n += noise2D(p * 1.0) * 0.5;
    n += noise2D(p * 2.0) * 0.25;
    n += noise2D(p * 4.0) * 0.125;
    n += noise2D(p * 8.0) * 0.0625;
    
    return n;
}

// Cloud Noise
float cloudNoise2D(vec2 st, float time)
{
    vec2 p = st + time * 0.1;
    
    float n = 0.0;
    n += noise2D(p * 1.0) * 0.5;
    n += noise2D(p * 2.0) * 0.25;
    n += noise2D(p * 4.0) * 0.125;
    n += noise2D(p * 8.0) * 0.0625;
    
    return smoothstep(0.3, 0.7, n);
}

// Ocean Noise
float oceanNoise2D(vec2 st, float time)
{
    vec2 p = st + time * 0.05;
    
    float n = 0.0;
    n += sin(p.x * 2.0 + time) * 0.5;
    n += sin(p.y * 3.0 + time * 1.5) * 0.3;
    n += noise2D(p * 4.0) * 0.2;
    
    return n * 0.5 + 0.5;
}

// Terrain Noise
float terrainNoise2D(vec2 st, float heightScale)
{
    float n = 0.0;
    n += noise2D(st * 1.0) * heightScale;
    n += noise2D(st * 2.0) * heightScale * 0.5;
    n += noise2D(st * 4.0) * heightScale * 0.25;
    n += noise2D(st * 8.0) * heightScale * 0.125;
    
    return n;
}

// Utility Functions
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

// Noise mixing functions
float mixNoise2D(vec2 st, float weight1, float weight2)
{
    float n1 = noise2D(st);
    float n2 = noise2D(st * 2.0);
    return mix(n1, n2, weight1) * weight2;
}

float addNoise2D(vec2 st, float scale1, float scale2)
{
    return noise2D(st * scale1) + noise2D(st * scale2) * 0.5;
}

float multiplyNoise2D(vec2 st, float scale1, float scale2)
{
    return noise2D(st * scale1) * noise2D(st * scale2);
}

// Animated noise functions
float animatedNoise2D(vec2 st, float time, float speed)
{
    vec2 p = st + time * speed;
    return noise2D(p);
}

float animatedFractalNoise2D(vec2 st, float time, float speed, int octaves, float persistence, float lacunarity)
{
    vec2 p = st + time * speed;
    return fractalNoise2D(p, octaves, persistence, lacunarity);
}

// Turbulent flow noise
float flowNoise2D(vec2 st, float time, float flowSpeed)
{
    vec2 flow = vec2(
        sin(time * flowSpeed),
        cos(time * flowSpeed)
    );
    return noise2D(st + flow);
}

// Rotated noise
float rotatedNoise2D(vec2 st, float angle)
{
    float s = sin(angle);
    float c = cos(angle);
    vec2 rotated = vec2(
        st.x * c - st.y * s,
        st.x * s + st.y * c
    );
    return noise2D(rotated);
}

// Scaled noise
float scaledNoise2D(vec2 st, float scale)
{
    return noise2D(st * scale);
}

// Offset noise
float offsetNoise2D(vec2 st, vec2 offset)
{
    return noise2D(st + offset);
}

// Domain warping
float warpedNoise2D(vec2 st, float warpAmount)
{
    vec2 offset = vec2(
        noise2D(st) * warpAmount,
        noise2D(st + vec2(100.0, 100.0)) * warpAmount
    );
    return noise2D(st + offset);
}

// Multi-octave domain warping
float warpedFractalNoise2D(vec2 st, float warpAmount, int octaves, float persistence, float lacunarity)
{
    vec2 offset = vec2(0.0);
    for (int i = 0; i < octaves; i++)
    {
        float scale = pow(lacunarity, float(i));
        float amp = pow(persistence, float(i));
        offset += vec2(
            noise2D(st * scale) * amp,
            noise2D(st * scale + vec2(100.0, 100.0)) * amp
        ) * warpAmount;
    }
    return noise2D(st + offset);
}

// Noise derivatives (for normal generation)
vec2 noiseDerivative2D(vec2 st)
{
    float epsilon = 0.001;
    float dx = (noise2D(st + vec2(epsilon, 0.0)) - noise2D(st - vec2(epsilon, 0.0))) / (2.0 * epsilon);
    float dy = (noise2D(st + vec2(0.0, epsilon)) - noise2D(st - vec2(0.0, epsilon))) / (2.0 * epsilon);
    return vec2(dx, dy);
}

vec3 noiseDerivative3D(vec3 st)
{
    float epsilon = 0.001;
    float dx = (noise3D(st + vec3(epsilon, 0.0, 0.0)) - noise3D(st - vec3(epsilon, 0.0, 0.0))) / (2.0 * epsilon);
    float dy = (noise3D(st + vec3(0.0, epsilon, 0.0)) - noise3D(st - vec3(0.0, epsilon, 0.0))) / (2.0 * epsilon);
    float dz = (noise3D(st + vec3(0.0, 0.0, epsilon)) - noise3D(st - vec3(0.0, 0.0, epsilon))) / (2.0 * epsilon);
    return vec3(dx, dy, dz);
}

// Normal generation from noise
vec3 noiseNormal2D(vec2 st, float strength)
{
    vec2 derivative = noiseDerivative2D(st);
    return normalize(vec3(-derivative.x, strength, -derivative.y));
}

vec3 noiseNormal3D(vec3 st, float strength)
{
    vec3 derivative = noiseDerivative3D(st);
    return normalize(vec3(-derivative.x, -derivative.y, strength));
}

// Utility macros for common noise patterns
#define NOISE_PATTERNS \
    float marble(vec2 st, float freq, float amp) { return marbleNoise2D(st, freq, amp); } \
    float wood(vec2 st, float freq) { return woodNoise2D(st, freq); } \
    float fire(vec2 st, float time) { return fireNoise2D(st, time); } \
    float clouds(vec2 st, float time) { return cloudNoise2D(st, time); } \
    float ocean(vec2 st, float time) { return oceanNoise2D(st, time); } \
    float terrain(vec2 st, float height) { return terrainNoise2D(st, height); }