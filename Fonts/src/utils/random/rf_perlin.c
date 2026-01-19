/******************************************************************************
 * Red Files - Perlin Noise Generation
 *
 * Implements Ken Perlin's improved noise algorithm with 2D, 3D, and 4D
 * noise generation, gradient functions, and various noise utilities.
 *
 * Copyright (c) [2025] [Red Files Contributors]
 * All rights reserved.
 ******************************************************************************/

#include "rf_perlin.h"
#include "rf_math.h"
#include <math.h>
#include <stdlib.h>

/* Permutation table for Perlin noise */
static const int PERMUTATION_TABLE[256] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
    35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
    134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
    55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
    18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
    250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
    189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
    172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
    228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
    107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

/* Extended permutation table */
static int g_permutation[512];
static bool g_permutation_initialized = false;

/* Gradient vectors for 2D, 3D, and 4D */
static const float GRADIENT_2D[8][2] = {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1},
    {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
};

static const float GRADIENT_3D[12][3] = {
    {1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0},
    {1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},
    {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1}
};

static const float GRADIENT_4D[32][4] = {
    {0, 1, 1, 1}, {0, 1, 1, -1}, {0, 1, -1, 1}, {0, 1, -1, -1},
    {0, -1, 1, 1}, {0, -1, 1, -1}, {0, -1, -1, 1}, {0, -1, -1, -1},
    {1, 0, 1, 1}, {1, 0, 1, -1}, {1, 0, -1, 1}, {1, 0, -1, -1},
    {-1, 0, 1, 1}, {-1, 0, 1, -1}, {-1, 0, -1, 1}, {-1, 0, -1, -1},
    {1, 1, 0, 1}, {1, 1, 0, -1}, {1, -1, 0, 1}, {1, -1, 0, -1},
    {-1, 1, 0, 1}, {-1, 1, 0, -1}, {-1, -1, 0, 1}, {-1, -1, 0, -1},
    {1, 1, 1, 0}, {1, 1, -1, 0}, {1, -1, 1, 0}, {1, -1, -1, 0},
    {-1, 1, 1, 0}, {-1, 1, -1, 0}, {-1, -1, 1, 0}, {-1, -1, -1, 0}
};

/* Initialize permutation table */
static void rf_perlin_init_permutation(void) {
    if (!g_permutation_initialized) {
        for (int i = 0; i < 256; i++) {
            g_permutation[i] = PERMUTATION_TABLE[i];
            g_permutation[i + 256] = PERMUTATION_TABLE[i];
        }
        g_permutation_initialized = true;
    }
}

/* Fade function (6t^5 - 15t^4 + 10t^3) */
static float rf_perlin_fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

/* Linear interpolation */
static float rf_perlin_lerp(float a, float b, float t) {
    return a + t * (b - a);
}

/* Gradient function for 2D */
static float rf_perlin_grad_2d(int hash, float x, float y) {
    int h = hash & 7;
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

/* Gradient function for 3D */
static float rf_perlin_grad_3d(int hash, float x, float y, float z) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

/* Gradient function for 4D */
static float rf_perlin_grad_4d(int hash, float x, float y, float z, float w) {
    int h = hash & 31;
    const float* grad = GRADIENT_4D[h];
    return grad[0] * x + grad[1] * y + grad[2] * z + grad[3] * w;
}

/* 2D Perlin noise */
float rf_perlin_2d(float x, float y) {
    rf_perlin_init_permutation();
    
    /* Find unit cube that contains point */
    int X = (int)floorf(x) & 255;
    int Y = (int)floorf(y) & 255;
    
    /* Find relative x, y, z of point in cube */
    x -= floorf(x);
    y -= floorf(y);
    
    /* Compute fade curves for each of x, y, z */
    float u = rf_perlin_fade(x);
    float v = rf_perlin_fade(y);
    
    /* Hash coordinates of the 8 cube corners */
    int A = g_permutation[X] + Y;
    int AA = g_permutation[A];
    int AB = g_permutation[A + 1];
    int B = g_permutation[X + 1] + Y;
    int BA = g_permutation[B];
    int BB = g_permutation[B + 1];
    
    /* Add blended results from 8 corners of cube */
    return rf_perlin_lerp(
        rf_perlin_lerp(
            rf_perlin_grad_2d(g_permutation[AA], x, y),
            rf_perlin_grad_2d(g_permutation[BA], x - 1.0f, y),
            u
        ),
        rf_perlin_lerp(
            rf_perlin_grad_2d(g_permutation[AB], x, y - 1.0f),
            rf_perlin_grad_2d(g_permutation[BB], x - 1.0f, y - 1.0f),
            u
        ),
        v
    );
}

/* 3D Perlin noise */
float rf_perlin_3d(float x, float y, float z) {
    rf_perlin_init_permutation();
    
    /* Find unit cube that contains point */
    int X = (int)floorf(x) & 255;
    int Y = (int)floorf(y) & 255;
    int Z = (int)floorf(z) & 255;
    
    /* Find relative x, y, z of point in cube */
    x -= floorf(x);
    y -= floorf(y);
    z -= floorf(z);
    
    /* Compute fade curves for each of x, y, z */
    float u = rf_perlin_fade(x);
    float v = rf_perlin_fade(y);
    float w = rf_perlin_fade(z);
    
    /* Hash coordinates of the 8 cube corners */
    int A = g_permutation[X] + Y;
    int AA = g_permutation[A] + Z;
    int AB = g_permutation[A + 1] + Z;
    int B = g_permutation[X + 1] + Y;
    int BA = g_permutation[B] + Z;
    int BB = g_permutation[B + 1] + Z;
    
    /* Add blended results from 8 corners of cube */
    return rf_perlin_lerp(
        w,
        rf_perlin_lerp(
            u,
            rf_perlin_grad_3d(g_permutation[AA], x, y, z),
            rf_perlin_grad_3d(g_permutation[BA], x - 1.0f, y, z)
        ),
        rf_perlin_lerp(
            u,
            rf_perlin_grad_3d(g_permutation[AB], x, y - 1.0f, z),
            rf_perlin_grad_3d(g_permutation[BB], x - 1.0f, y - 1.0f, z)
        ),
        v,
        rf_perlin_lerp(
            u,
            rf_perlin_grad_3d(g_permutation[AA + 1], x, y, z - 1.0f),
            rf_perlin_grad_3d(g_permutation[BA + 1], x - 1.0f, y, z - 1.0f)
        ),
        rf_perlin_lerp(
            u,
            rf_perlin_grad_3d(g_permutation[AB + 1], x, y - 1.0f, z - 1.0f),
            rf_perlin_grad_3d(g_permutation[BB + 1], x - 1.0f, y - 1.0f, z - 1.0f)
        )
    );
}

/* 4D Perlin noise */
float rf_perlin_4d(float x, float y, float z, float w) {
    rf_perlin_init_permutation();
    
    /* Find unit hypercube that contains point */
    int X = (int)floorf(x) & 255;
    int Y = (int)floorf(y) & 255;
    int Z = (int)floorf(z) & 255;
    int W = (int)floorf(w) & 255;
    
    /* Find relative x, y, z, w of point in hypercube */
    x -= floorf(x);
    y -= floorf(y);
    z -= floorf(z);
    w -= floorf(w);
    
    /* Compute fade curves for each of x, y, z, w */
    float u = rf_perlin_fade(x);
    float v = rf_perlin_fade(y);
    float p = rf_perlin_fade(z);
    float q = rf_perlin_fade(w);
    
    /* Hash coordinates of the 16 hypercube corners */
    int A = g_permutation[X] + Y;
    int AA = g_permutation[A] + Z;
    int AB = g_permutation[A + 1] + Z;
    int B = g_permutation[X + 1] + Y;
    int BA = g_permutation[B] + Z;
    int BB = g_permutation[B + 1] + Z;
    
    int AAA = g_permutation[AA] + W;
    int AAB = g_permutation[AA + 1] + W;
    int ABA = g_permutation[AB] + W;
    int ABB = g_permutation[AB + 1] + W;
    int BAA = g_permutation[BA] + W;
    int BAB = g_permutation[BA + 1] + W;
    int BBA = g_permutation[BB] + W;
    int BBB = g_permutation[BB + 1] + W;
    
    /* Add blended results from 16 corners of hypercube */
    float n0000 = rf_perlin_grad_4d(g_permutation[AAA], x, y, z, w);
    float n1000 = rf_perlin_grad_4d(g_permutation[BAA], x - 1.0f, y, z, w);
    float n0100 = rf_perlin_grad_4d(g_permutation[ABA], x, y - 1.0f, z, w);
    float n1100 = rf_perlin_grad_4d(g_permutation[BBA], x - 1.0f, y - 1.0f, z, w);
    float n0010 = rf_perlin_grad_4d(g_permutation[AAB], x, y, z - 1.0f, w);
    float n1010 = rf_perlin_grad_4d(g_permutation[BAB], x - 1.0f, y, z - 1.0f, w);
    float n0110 = rf_perlin_grad_4d(g_permutation[ABB], x, y - 1.0f, z - 1.0f, w);
    float n1110 = rf_perlin_grad_4d(g_permutation[BBB], x - 1.0f, y - 1.0f, z - 1.0f, w);
    float n0001 = rf_perlin_grad_4d(g_permutation[AAA + 1], x, y, z, w - 1.0f);
    float n1001 = rf_perlin_grad_4d(g_permutation[BAA + 1], x - 1.0f, y, z, w - 1.0f);
    float n0101 = rf_perlin_grad_4d(g_permutation[ABA + 1], x, y - 1.0f, z, w - 1.0f);
    float n1101 = rf_perlin_grad_4d(g_permutation[BBA + 1], x - 1.0f, y - 1.0f, z, w - 1.0f);
    float n0011 = rf_perlin_grad_4d(g_permutation[AAB + 1], x, y, z - 1.0f, w - 1.0f);
    float n1011 = rf_perlin_grad_4d(g_permutation[BAB + 1], x - 1.0f, y, z - 1.0f, w - 1.0f);
    float n0111 = rf_perlin_grad_4d(g_permutation[ABB + 1], x, y - 1.0f, z - 1.0f, w - 1.0f);
    float n1111 = rf_perlin_grad_4d(g_permutation[BBB + 1], x - 1.0f, y - 1.0f, z - 1.0f, w - 1.0f);
    
    /* Interpolate along X */
    float nx000 = rf_perlin_lerp(n0000, n1000, u);
    float nx010 = rf_perlin_lerp(n0100, n1100, u);
    float nx001 = rf_perlin_lerp(n0010, n1010, u);
    float nx011 = rf_perlin_lerp(n0110, n1110, u);
    float nx100 = rf_perlin_lerp(n0001, n1001, u);
    float nx110 = rf_perlin_lerp(n0101, n1101, u);
    float nx101 = rf_perlin_lerp(n0011, n1011, u);
    float nx111 = rf_perlin_lerp(n0111, n1111, u);
    
    /* Interpolate along Y */
    float nxy00 = rf_perlin_lerp(nx000, nx010, v);
    float nxy01 = rf_perlin_lerp(nx001, nx011, v);
    float nxy10 = rf_perlin_lerp(nx100, nx110, v);
    float nxy11 = rf_perlin_lerp(nx101, nx111, v);
    
    /* Interpolate along Z */
    float nxyz0 = rf_perlin_lerp(nxy00, nxy01, p);
    float nxyz1 = rf_perlin_lerp(nxy10, nxy11, p);
    
    /* Interpolate along W */
    return rf_perlin_lerp(nxyz0, nxyz1, q);
}

/* Fractal Brownian Motion (FBM) with Perlin noise */
float rf_perlin_fbm_2d(float x, float y, int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float max_value = 0.0f;
    
    for (int i = 0; i < octaves; i++) {
        value += rf_perlin_2d(x * frequency, y * frequency) * amplitude;
        max_value += amplitude;
        
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return value / max_value;
}

float rf_perlin_fbm_3d(float x, float y, float z, int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float max_value = 0.0f;
    
    for (int i = 0; i < octaves; i++) {
        value += rf_perlin_3d(x * frequency, y * frequency, z * frequency) * amplitude;
        max_value += amplitude;
        
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return value / max_value;
}

float rf_perlin_fbm_4d(float x, float y, float z, float w, int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float max_value = 0.0f;
    
    for (int i = 0; i < octaves; i++) {
        value += rf_perlin_4d(x * frequency, y * frequency, z * frequency, w * frequency) * amplitude;
        max_value += amplitude;
        
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return value / max_value;
}

/* Ridged noise with Perlin */
float rf_perlin_ridged_2d(float x, float y, int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float weight = 1.0f;
    
    for (int i = 0; i < octaves; i++) {
        float signal = rf_perlin_2d(x * frequency, y * frequency);
        signal = fabsf(signal);
        signal = 1.0f - signal;
        
        value += signal * amplitude * weight;
        weight = rf_clamp(signal * weight, 0.0f, 1.0f);
        
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return value;
}

float rf_perlin_ridged_3d(float x, float y, float z, int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float weight = 1.0f;
    
    for (int i = 0; i < octaves; i++) {
        float signal = rf_perlin_3d(x * frequency, y * frequency, z * frequency);
        signal = fabsf(signal);
        signal = 1.0f - signal;
        
        value += signal * amplitude * weight;
        weight = rf_clamp(signal * weight, 0.0f, 1.0f);
        
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return value;
}

/* Turbulence with Perlin */
float rf_perlin_turbulence_2d(float x, float y, int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    
    for (int i = 0; i < octaves; i++) {
        value += fabsf(rf_perlin_2d(x * frequency, y * frequency)) * amplitude;
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return value;
}

float rf_perlin_turbulence_3d(float x, float y, float z, int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    
    for (int i = 0; i < octaves; i++) {
        value += fabsf(rf_perlin_3d(x * frequency, y * frequency, z * frequency)) * amplitude;
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return value;
}

/* Marble noise with Perlin */
float rf_perlin_marble_2d(float x, float y, int octaves, float persistence, float lacunarity) {
    float noise = rf_perlin_fbm_2d(x, y, octaves, persistence, lacunarity);
    return sinf(x * 4.0f + noise * RF_PI * 2.0f);
}

float rf_perlin_marble_3d(float x, float y, float z, int octaves, float persistence, float lacunarity) {
    float noise = rf_perlin_fbm_3d(x, y, z, octaves, persistence, lacunarity);
    return sinf(x * 4.0f + noise * RF_PI * 2.0f);
}

/* Wood noise with Perlin */
float rf_perlin_wood_2d(float x, float y, int octaves, float persistence, float lacunarity) {
    float noise = rf_perlin_fbm_2d(x, y, octaves, persistence, lacunarity);
    float rings = noise * 20.0f;
    return rings - floorf(rings);
}

float rf_perlin_wood_3d(float x, float y, float z, int octaves, float persistence, float lacunarity) {
    float noise = rf_perlin_fbm_3d(x, y, z, octaves, persistence, lacunarity);
    float rings = noise * 20.0f;
    return rings - floorf(rings);
}

/* Generate Perlin noise texture */
void rf_perlin_generate_texture_2d(float* texture, int width, int height,
                                  float scale_x, float scale_y,
                                  int octaves, float persistence, float lacunarity) {
    if (!texture || width <= 0 || height <= 0) {
        return;
    }
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float nx = (float)x / width * scale_x;
            float ny = (float)y / height * scale_y;
            
            texture[y * width + x] = rf_perlin_fbm_2d(nx, ny, octaves, persistence, lacunarity);
        }
    }
}

void rf_perlin_generate_texture_3d(float* texture, int width, int height, int depth,
                                  float scale_x, float scale_y, float scale_z,
                                  int octaves, float persistence, float lacunarity) {
    if (!texture || width <= 0 || height <= 0 || depth <= 0) {
        return;
    }
    
    for (int z = 0; z < depth; z++) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float nx = (float)x / width * scale_x;
                float ny = (float)y / height * scale_y;
                float nz = (float)z / depth * scale_z;
                
                texture[(z * height + y) * width + x] = rf_perlin_fbm_3d(nx, ny, nz, octaves, persistence, lacunarity);
            }
        }
    }
}

/* Perlin noise utility functions */

/* Scale Perlin noise to range */
float rf_perlin_scale(float noise, float min_val, float max_val) {
    return rf_lerp(min_val, max_val, rf_clamp01(noise));
}

/* Bias Perlin noise towards higher values */
float rf_perlin_bias(float noise, float bias) {
    return powf(noise, logf(bias) / logf(0.5f));
}

/* Gain Perlin noise towards middle values */
float rf_perlin_gain(float noise, float gain) {
    if (noise < 0.5f) {
        return rf_perlin_bias(noise * 2.0f, gain) / 2.0f;
    } else {
        return 1.0f - rf_perlin_bias((1.0f - noise) * 2.0f, gain) / 2.0f;
    }
}

/* Get normalized Perlin noise (0 to 1) */
float rf_perlin_normalized_2d(float x, float y) {
    float noise = rf_perlin_2d(x, y);
    return (noise + 1.0f) * 0.5f;
}

float rf_perlin_normalized_3d(float x, float y, float z) {
    float noise = rf_perlin_3d(x, y, z);
    return (noise + 1.0f) * 0.5f;
}

float rf_perlin_normalized_4d(float x, float y, float z, float w) {
    float noise = rf_perlin_4d(x, y, z, w);
    return (noise + 1.0f) * 0.5f;
}

/* Octave-based Perlin noise with automatic normalization */
float rf_perlin_octaves_2d(float x, float y, int octaves) {
    return rf_perlin_fbm_2d(x, y, octaves, 0.5f, 2.0f);
}

float rf_perlin_octaves_3d(float x, float y, float z, int octaves) {
    return rf_perlin_fbm_3d(x, y, z, octaves, 0.5f, 2.0f);
}

float rf_perlin_octaves_4d(float x, float y, float z, float w, int octaves) {
    return rf_perlin_fbm_4d(x, y, z, w, octaves, 0.5f, 2.0f);
}