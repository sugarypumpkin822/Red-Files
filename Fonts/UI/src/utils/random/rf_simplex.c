/******************************************************************************
 * Red Files - Simplex Noise Generation
 *
 * Implements Ken Perlin's Simplex noise algorithm with 2D, 3D, and 4D
 * noise generation, gradient functions, and various noise utilities.
 *
 * Copyright (c) [2025] [Red Files Contributors]
 * All rights reserved.
 ******************************************************************************/

#include "rf_simplex.h"
#include "rf_math.h"
#include <math.h>
#include <stdlib.h>

/* Simplex noise permutation table */
static const int SIMPLEX_PERMUTATION[256] = {
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
static int g_simplex_perm[512];
static bool g_simplex_initialized = false;

/* Gradient vectors for Simplex noise */
static const float GRADIENT_3D[12][3] = {
    {1,1,0}, {-1,1,0}, {1,-1,0}, {-1,-1,0},
    {1,0,1}, {-1,0,1}, {1,0,-1}, {-1,0,-1},
    {0,1,1}, {0,-1,1}, {0,1,-1}, {0,-1,-1}
};

static const float GRADIENT_4D[32][4] = {
    {0,1,1,1}, {0,1,1,-1}, {0,1,-1,1}, {0,1,-1,-1},
    {0,-1,1,1}, {0,-1,1,-1}, {0,-1,-1,1}, {0,-1,-1,-1},
    {1,0,1,1}, {1,0,1,-1}, {1,0,-1,1}, {1,0,-1,-1},
    {-1,0,1,1}, {-1,0,1,-1}, {-1,0,-1,1}, {-1,0,-1,-1},
    {1,1,0,1}, {1,1,0,-1}, {1,-1,0,1}, {1,-1,0,-1},
    {-1,1,0,1}, {-1,1,0,-1}, {-1,-1,0,1}, {-1,-1,0,-1},
    {1,1,1,0}, {1,1,-1,0}, {1,-1,1,0}, {1,-1,-1,0},
    {-1,1,1,0}, {-1,1,-1,0}, {-1,-1,1,0}, {-1,-1,-1,0}
};

/* Initialize permutation table */
static void rf_simplex_init_permutation(void) {
    if (!g_simplex_initialized) {
        for (int i = 0; i < 256; i++) {
            g_simplex_perm[i] = SIMPLEX_PERMUTATION[i];
            g_simplex_perm[i + 256] = SIMPLEX_PERMUTATION[i];
        }
        g_simplex_initialized = true;
    }
}

/* Dot product for gradient */
static float rf_simplex_dot_2d(const float* g, float x, float y) {
    return g[0] * x + g[1] * y;
}

static float rf_simplex_dot_3d(const float* g, float x, float y, float z) {
    return g[0] * x + g[1] * y + g[2] * z;
}

static float rf_simplex_dot_4d(const float* g, float x, float y, float z, float w) {
    return g[0] * x + g[1] * y + g[2] * z + g[3] * w;
}

/* 2D Simplex noise */
float rf_simplex_2d(float x, float y) {
    rf_simplex_init_permutation();
    
    /* Noise contributions from the three corners */
    float n0, n1, n2;
    
    /* Skew the input space to determine which simplex cell we are in */
    const float F2 = 0.5f * (sqrtf(3.0f) - 1.0f);
    float s = (x + y) * F2;
    int i = (int)floorf(x + s);
    int j = (int)floorf(y + s);
    
    /* Unskew the cell origin back to (x,y) space */
    const float G2 = (3.0f - sqrtf(3.0f)) / 6.0f;
    float t = (i + j) * G2;
    float X0 = i - t;
    float Y0 = j - t;
    float x0 = x - X0;
    float y0 = y - Y0;
    
    /* Determine which simplex we are in */
    int i1, j1;
    if (x0 > y0) {
        i1 = 1; j1 = 0;
    } else {
        i1 = 0; j1 = 1;
    }
    
    /* Offsets for second (middle) corner of simplex in (i,j) coords */
    float x1 = x0 - i1 + G2;
    float y1 = y0 - j1 + G2;
    
    /* Offsets for last corner of simplex in (i,j) coords */
    float x2 = x0 - 1.0f + 2.0f * G2;
    float y2 = y0 - 1.0f + 2.0f * G2;
    
    /* Work out the hashed gradient indices of the three simplex corners */
    int ii = i & 255;
    int jj = j & 255;
    int gi0 = g_simplex_perm[ii + g_simplex_perm[jj]] % 12;
    int gi1 = g_simplex_perm[ii + i1 + g_simplex_perm[jj + j1]] % 12;
    int gi2 = g_simplex_perm[ii + 1 + g_simplex_perm[jj + 1]] % 12;
    
    /* Calculate the contribution from the three corners */
    float t0 = 0.5f - x0 * x0 - y0 * y0;
    if (t0 < 0.0f) {
        n0 = 0.0f;
    } else {
        t0 *= t0;
        n0 = t0 * t0 * rf_simplex_dot_2d(GRADIENT_3D[gi0], x0, y0);
    }
    
    float t1 = 0.5f - x1 * x1 - y1 * y1;
    if (t1 < 0.0f) {
        n1 = 0.0f;
    } else {
        t1 *= t1;
        n1 = t1 * t1 * rf_simplex_dot_2d(GRADIENT_3D[gi1], x1, y1);
    }
    
    float t2 = 0.5f - x2 * x2 - y2 * y2;
    if (t2 < 0.0f) {
        n2 = 0.0f;
    } else {
        t2 *= t2;
        n2 = t2 * t2 * rf_simplex_dot_2d(GRADIENT_3D[gi2], x2, y2);
    }
    
    /* Add contributions from each corner to get the final noise value */
    return 70.0f * (n0 + n1 + n2);
}

/* 3D Simplex noise */
float rf_simplex_3d(float x, float y, float z) {
    rf_simplex_init_permutation();
    
    /* Noise contributions from the eight corners */
    float n0, n1, n2, n3;
    
    /* Skew the input space to determine which simplex cell we are in */
    const float F3 = 1.0f / 3.0f;
    float s = (x + y + z) * F3;
    int i = (int)floorf(x + s);
    int j = (int)floorf(y + s);
    int k = (int)floorf(z + s);
    
    /* Unskew the cell origin back to (x,y,z) space */
    const float G3 = 1.0f / 6.0f;
    float t = (i + j + k) * G3;
    float X0 = i - t;
    float Y0 = j - t;
    float Z0 = k - t;
    float x0 = x - X0;
    float y0 = y - Y0;
    float z0 = z - Z0;
    
    /* Determine which simplex we are in */
    int i1, j1, k1;
    int i2, j2, k2;
    
    if (x0 >= y0) {
        if (y0 >= z0) {
            i1 = 1; j1 = 0; k1 = 0;
            i2 = 1; j2 = 1; k2 = 0;
        } else if (x0 >= z0) {
            i1 = 1; j1 = 0; k1 = 0;
            i2 = 1; j2 = 0; k2 = 1;
        } else {
            i1 = 0; j1 = 0; k1 = 1;
            i2 = 1; j2 = 0; k2 = 1;
        }
    } else {
        if (y0 < z0) {
            i1 = 0; j1 = 0; k1 = 1;
            i2 = 0; j2 = 1; k2 = 1;
        } else if (x0 < z0) {
            i1 = 0; j1 = 1; k1 = 0;
            i2 = 0; j2 = 1; k2 = 1;
        } else {
            i1 = 0; j1 = 1; k1 = 0;
            i2 = 1; j2 = 1; k2 = 0;
        }
    }
    
    /* Offsets for second corner in (i,j,k) coords */
    float x1 = x0 - i1 + G3;
    float y1 = y0 - j1 + G3;
    float z1 = z0 - k1 + G3;
    
    /* Offsets for third corner in (i,j,k) coords */
    float x2 = x0 - i2 + 2.0f * G3;
    float y2 = y0 - j2 + 2.0f * G3;
    float z2 = z0 - k2 + 2.0f * G3;
    
    /* Offsets for last corner in (i,j,k) coords */
    float x3 = x0 - 1.0f + 3.0f * G3;
    float y3 = y0 - 1.0f + 3.0f * G3;
    float z3 = z0 - 1.0f + 3.0f * G3;
    
    /* Work out the hashed gradient indices of the four simplex corners */
    int ii = i & 255;
    int jj = j & 255;
    int kk = k & 255;
    
    int gi0 = g_simplex_perm[ii + g_simplex_perm[jj + g_simplex_perm[kk]]] % 12;
    int gi1 = g_simplex_perm[ii + i1 + g_simplex_perm[jj + j1 + g_simplex_perm[kk + k1]]] % 12;
    int gi2 = g_simplex_perm[ii + i2 + g_simplex_perm[jj + j2 + g_simplex_perm[kk + k2]]] % 12;
    int gi3 = g_simplex_perm[ii + 1 + g_simplex_perm[jj + 1 + g_simplex_perm[kk + 1]]] % 12;
    
    /* Calculate the contribution from the four corners */
    float t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;
    if (t0 < 0.0f) {
        n0 = 0.0f;
    } else {
        t0 *= t0;
        n0 = t0 * t0 * rf_simplex_dot_3d(GRADIENT_3D[gi0], x0, y0, z0);
    }
    
    float t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
    if (t1 < 0.0f) {
        n1 = 0.0f;
    } else {
        t1 *= t1;
        n1 = t1 * t1 * rf_simplex_dot_3d(GRADIENT_3D[gi1], x1, y1, z1);
    }
    
    float t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;
    if (t2 < 0.0f) {
        n2 = 0.0f;
    } else {
        t2 *= t2;
        n2 = t2 * t2 * rf_simplex_dot_3d(GRADIENT_3D[gi2], x2, y2, z2);
    }
    
    float t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;
    if (t3 < 0.0f) {
        n3 = 0.0f;
    } else {
        t3 *= t3;
        n3 = t3 * t3 * rf_simplex_dot_3d(GRADIENT_3D[gi3], x3, y3, z3);
    }
    
    /* Add contributions from each corner to get the final noise value */
    return 32.0f * (n0 + n1 + n2 + n3);
}

/* 4D Simplex noise */
float rf_simplex_4d(float x, float y, float z, float w) {
    rf_simplex_init_permutation();
    
    /* Simplified 4D simplex noise implementation */
    /* Use 3D simplex noise with w as time dimension */
    float noise3d1 = rf_simplex_3d(x, y, z);
    float noise3d2 = rf_simplex_3d(x + w, y + w, z + w);
    
    return rf_lerp(noise3d1, noise3d2, 0.5f);
}

/* Fractal Brownian Motion (FBM) with Simplex noise */
float rf_simplex_fbm_2d(float x, float y, int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float max_value = 0.0f;
    
    for (int i = 0; i < octaves; i++) {
        value += rf_simplex_2d(x * frequency, y * frequency) * amplitude;
        max_value += amplitude;
        
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return value / max_value;
}

float rf_simplex_fbm_3d(float x, float y, float z, int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float max_value = 0.0f;
    
    for (int i = 0; i < octaves; i++) {
        value += rf_simplex_3d(x * frequency, y * frequency, z * frequency) * amplitude;
        max_value += amplitude;
        
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return value / max_value;
}

float rf_simplex_fbm_4d(float x, float y, float z, float w, int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float max_value = 0.0f;
    
    for (int i = 0; i < octaves; i++) {
        value += rf_simplex_4d(x * frequency, y * frequency, z * frequency, w * frequency) * amplitude;
        max_value += amplitude;
        
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return value / max_value;
}

/* Ridged noise with Simplex */
float rf_simplex_ridged_2d(float x, float y, int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float weight = 1.0f;
    
    for (int i = 0; i < octaves; i++) {
        float signal = rf_simplex_2d(x * frequency, y * frequency);
        signal = fabsf(signal);
        signal = 1.0f - signal;
        
        value += signal * amplitude * weight;
        weight = rf_clamp(signal * weight, 0.0f, 1.0f);
        
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return value;
}

float rf_simplex_ridged_3d(float x, float y, float z, int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float weight = 1.0f;
    
    for (int i = 0; i < octaves; i++) {
        float signal = rf_simplex_3d(x * frequency, y * frequency, z * frequency);
        signal = fabsf(signal);
        signal = 1.0f - signal;
        
        value += signal * amplitude * weight;
        weight = rf_clamp(signal * weight, 0.0f, 1.0f);
        
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return value;
}

/* Turbulence with Simplex */
float rf_simplex_turbulence_2d(float x, float y, int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    
    for (int i = 0; i < octaves; i++) {
        value += fabsf(rf_simplex_2d(x * frequency, y * frequency)) * amplitude;
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return value;
}

float rf_simplex_turbulence_3d(float x, float y, float z, int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    
    for (int i = 0; i < octaves; i++) {
        value += fabsf(rf_simplex_3d(x * frequency, y * frequency, z * frequency)) * amplitude;
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return value;
}

/* Marble noise with Simplex */
float rf_simplex_marble_2d(float x, float y, int octaves, float persistence, float lacunarity) {
    float noise = rf_simplex_fbm_2d(x, y, octaves, persistence, lacunarity);
    return sinf(x * 4.0f + noise * RF_PI * 2.0f);
}

float rf_simplex_marble_3d(float x, float y, float z, int octaves, float persistence, float lacunarity) {
    float noise = rf_simplex_fbm_3d(x, y, z, octaves, persistence, lacunarity);
    return sinf(x * 4.0f + noise * RF_PI * 2.0f);
}

/* Wood noise with Simplex */
float rf_simplex_wood_2d(float x, float y, int octaves, float persistence, float lacunarity) {
    float noise = rf_simplex_fbm_2d(x, y, octaves, persistence, lacunarity);
    float rings = noise * 20.0f;
    return rings - floorf(rings);
}

float rf_simplex_wood_3d(float x, float y, float z, int octaves, float persistence, float lacunarity) {
    float noise = rf_simplex_fbm_3d(x, y, z, octaves, persistence, lacunarity);
    float rings = noise * 20.0f;
    return rings - floorf(rings);
}

/* Generate Simplex noise texture */
void rf_simplex_generate_texture_2d(float* texture, int width, int height,
                                   float scale_x, float scale_y,
                                   int octaves, float persistence, float lacunarity) {
    if (!texture || width <= 0 || height <= 0) {
        return;
    }
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float nx = (float)x / width * scale_x;
            float ny = (float)y / height * scale_y;
            
            texture[y * width + x] = rf_simplex_fbm_2d(nx, ny, octaves, persistence, lacunarity);
        }
    }
}

void rf_simplex_generate_texture_3d(float* texture, int width, int height, int depth,
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
                
                texture[(z * height + y) * width + x] = rf_simplex_fbm_3d(nx, ny, nz, octaves, persistence, lacunarity);
            }
        }
    }
}

/* Simplex noise utility functions */

/* Scale Simplex noise to range */
float rf_simplex_scale(float noise, float min_val, float max_val) {
    return rf_lerp(min_val, max_val, rf_clamp01(noise));
}

/* Bias Simplex noise towards higher values */
float rf_simplex_bias(float noise, float bias) {
    return powf(noise, logf(bias) / logf(0.5f));
}

/* Gain Simplex noise towards middle values */
float rf_simplex_gain(float noise, float gain) {
    if (noise < 0.5f) {
        return rf_simplex_bias(noise * 2.0f, gain) / 2.0f;
    } else {
        return 1.0f - rf_simplex_bias((1.0f - noise) * 2.0f, gain) / 2.0f;
    }
}

/* Get normalized Simplex noise (0 to 1) */
float rf_simplex_normalized_2d(float x, float y) {
    float noise = rf_simplex_2d(x, y);
    return (noise + 1.0f) * 0.5f;
}

float rf_simplex_normalized_3d(float x, float y, float z) {
    float noise = rf_simplex_3d(x, y, z);
    return (noise + 1.0f) * 0.5f;
}

float rf_simplex_normalized_4d(float x, float y, float z, float w) {
    float noise = rf_simplex_4d(x, y, z, w);
    return (noise + 1.0f) * 0.5f;
}

/* Octave-based Simplex noise with automatic normalization */
float rf_simplex_octaves_2d(float x, float y, int octaves) {
    return rf_simplex_fbm_2d(x, y, octaves, 0.5f, 2.0f);
}

float rf_simplex_octaves_3d(float x, float y, float z, int octaves) {
    return rf_simplex_fbm_3d(x, y, z, octaves, 0.5f, 2.0f);
}

float rf_simplex_octaves_4d(float x, float y, float z, float w, int octaves) {
    return rf_simplex_fbm_4d(x, y, z, w, octaves, 0.5f, 2.0f);
}