#version 330 core

// Glow Fragment Shader for OpenGL
// Applies various glow, bloom, and luminance effects

// Input textures
uniform sampler2D uTexture;           // Source texture
uniform sampler2D uGlowTexture;        // Pre-computed glow texture
uniform sampler2D uLuminanceTexture;   // Luminance map
uniform sampler2D uMaskTexture;        // Mask texture for selective glow
uniform sampler2D uNoiseTexture;       // Noise texture for animated glow
uniform vec2 uResolution;             // Screen resolution
uniform float uTime;                  // Animation time
uniform float uStrength;              // Glow strength/intensity
uniform float uThreshold;             // Luminance threshold for glow
uniform float uSoftKnee;              // Soft knee for threshold
uniform float uRadius;                // Glow radius
uniform int uGlowType;                // Glow type (0=basic, 1=selective, 2=directional, 3=animated)
uniform vec3 uGlowColor;              // Glow color tint
uniform bool uUseGlowColor;           // Use custom glow color
uniform bool uAnimated;               // Animate the glow
uniform float uAnimationSpeed;        // Animation speed
uniform float uPulseFrequency;        // Pulse frequency
uniform float uPulseAmplitude;        // Pulse amplitude
uniform bool uBloomEnabled;           // Enable bloom effect
uniform float uBloomStrength;         // Bloom strength
uniform int uBloomMipLevel;           // Bloom mip level
uniform bool uLensFlare;              // Add lens flare
uniform vec3 uFlareColor;             // Flare color
uniform float uFlareIntensity;        // Flare intensity
uniform bool uRadialGlow;             // Radial glow effect
uniform vec2 uGlowCenter;             // Glow center
uniform float uGlowFalloff;           // Glow falloff
uniform bool uGaussianGlow;            // Gaussian blur glow
uniform float uSigma;                 // Gaussian sigma
uniform bool uSelectiveGlow;          // Selective glow based on color
uniform vec3 uSelectColor;            // Color to select for glow
uniform float uSelectTolerance;       // Color selection tolerance
uniform bool uLuminanceGlow;          // Luminance-based glow
uniform float uLuminanceBoost;        // Luminance boost factor
uniform bool uHDRGlow;                // HDR glow effect
uniform float uHDRRange;              // HDR range
uniform bool uNeonGlow;               // Neon glow effect
uniform float uNeonWidth;             // Neon line width
uniform vec3 uNeonColor;              // Neon color
uniform bool uFireGlow;               // Fire glow effect
uniform float uFireIntensity;         // Fire intensity
uniform vec3 uFireColor;              // Fire color
uniform bool uElectricGlow;           // Electric glow effect
uniform float uElectricFrequency;     // Electric frequency
uniform vec3 uElectricColor;          // Electric color
uniform bool uMagicGlow;              // Magic glow effect
uniform float uMagicIntensity;        // Magic intensity
uniform vec3 uMagicColor;             // Magic color
uniform bool uToonGlow;               // Toon/cel shading glow
uniform float uToonThreshold;         // Toon threshold
uniform float uToonSteps;              // Toon quantization steps
uniform bool uAtmosphericGlow;        // Atmospheric glow
uniform float uAtmosphericDensity;    // Atmospheric density
uniform vec3 uAtmosphericColor;       // Atmospheric color
uniform bool uSubsurfaceGlow;         // Subsurface scattering glow
uniform float uSubsurfaceThickness;    // Subsurface thickness
uniform vec3 uSubsurfaceColor;         // Subsurface color

// Input from vertex shader
in vec2 vTexCoord;
in vec2 vScreenPos;

// Output color
out vec4 fragColor;

// Include common functions
#include "common/common.glsl"
#include "common/lighting.glsl"
#include "common/noise.glsl"

// Gaussian kernel for glow
float gaussian(float x, float sigma)
{
    return exp(-(x * x) / (2.0 * sigma * sigma)) / (sigma * sqrt(2.0 * PI));
}

// Basic glow effect
vec3 basicGlow(sampler2D tex, vec2 uv, float strength, float threshold)
{
    vec3 color = texture(tex, uv).rgb;
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    
    // Apply threshold with soft knee
    float glowAmount = smoothstep(threshold - uSoftKnee, threshold + uSoftKnee, luminance);
    
    // Apply glow
    vec3 glow = color * glowAmount * strength;
    
    return color + glow;
}

// Selective glow based on color
vec3 selectiveGlow(sampler2D tex, vec2 uv, vec3 selectColor, float tolerance, float strength)
{
    vec3 color = texture(tex, uv).rgb;
    
    // Calculate color distance
    float colorDistance = length(color - selectColor);
    float glowAmount = 1.0 - smoothstep(0.0, tolerance, colorDistance);
    
    // Apply glow
    vec3 glow = color * glowAmount * strength;
    
    return color + glow;
}

// Directional glow
vec3 directionalGlow(sampler2D tex, vec2 uv, vec2 direction, float strength, float radius)
{
    vec3 color = vec3(0.0);
    vec2 texelSize = 1.0 / uResolution;
    direction = normalize(direction);
    
    int samples = int(radius);
    
    for (int i = -samples; i <= samples; i++)
    {
        float t = float(i) / float(samples);
        vec2 sampleUV = uv + direction * t * radius * texelSize;
        vec3 sampleColor = texture(tex, sampleUV).rgb;
        
        float weight = gaussian(abs(t), uSigma);
        color += sampleColor * weight;
    }
    
    // Normalize
    color /= float(samples * 2 + 1);
    
    // Apply threshold
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    float glowAmount = smoothstep(uThreshold - uSoftKnee, uThreshold + uSoftKnee, luminance);
    
    vec3 originalColor = texture(tex, uv).rgb;
    return originalColor + color * glowAmount * strength;
}

// Radial glow
vec3 radialGlow(sampler2D tex, vec2 uv, vec2 center, float radius, float strength, float falloff)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return texture(tex, uv).rgb;
    
    direction = normalize(direction);
    
    vec3 color = vec3(0.0);
    int samples = int(radius);
    
    for (int i = 0; i <= samples; i++)
    {
        float t = float(i) / float(samples);
        float sampleDistance = t * radius;
        vec2 sampleUV = center + direction * sampleDistance;
        
        vec3 sampleColor = texture(tex, sampleUV).rgb;
        
        // Apply falloff
        float weight = pow(1.0 - t, falloff);
        color += sampleColor * weight;
    }
    
    // Normalize
    color /= float(samples + 1);
    
    // Apply threshold
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    float glowAmount = smoothstep(uThreshold - uSoftKnee, uThreshold + uSoftKnee, luminance);
    
    vec3 originalColor = texture(tex, uv).rgb;
    return originalColor + color * glowAmount * strength;
}

// Gaussian glow
vec3 gaussianGlow(sampler2D tex, vec2 uv, float radius, float sigma)
{
    vec3 color = vec3(0.0);
    vec2 texelSize = 1.0 / uResolution;
    float totalWeight = 0.0;
    
    int samples = int(radius);
    
    for (int x = -samples; x <= samples; x++)
    {
        for (int y = -samples; y <= samples; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float distance = length(vec2(float(x), float(y)));
            float weight = gaussian(distance, sigma);
            
            vec3 sampleColor = texture(tex, uv + offset).rgb;
            color += sampleColor * weight;
            totalWeight += weight;
        }
    }
    
    color /= totalWeight;
    
    // Apply threshold
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    float glowAmount = smoothstep(uThreshold - uSoftKnee, uThreshold + uSoftKnee, luminance);
    
    vec3 originalColor = texture(tex, uv).rgb;
    return originalColor + color * glowAmount * uStrength;
}

// Animated glow
vec3 animatedGlow(sampler2D tex, vec2 uv, float time, float speed, float pulseFreq, float pulseAmp)
{
    vec3 color = texture(tex, uv).rgb;
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    
    // Animated threshold
    float animatedThreshold = uThreshold + sin(time * speed) * 0.1;
    
    // Pulsing strength
    float pulse = 1.0 + sin(time * pulseFreq) * pulseAmp;
    float animatedStrength = uStrength * pulse;
    
    // Apply glow
    float glowAmount = smoothstep(animatedThreshold - uSoftKnee, animatedThreshold + uSoftKnee, luminance);
    vec3 glow = color * glowAmount * animatedStrength;
    
    return color + glow;
}

// Bloom effect
vec3 bloomEffect(sampler2D tex, sampler2D bloomTex, vec2 uv, float strength, int mipLevel)
{
    vec3 color = texture(tex, uv).rgb;
    vec3 bloomColor = textureLod(bloomTex, uv, float(mipLevel)).rgb;
    
    // Apply threshold to bloom
    bloomColor = max(vec3(0.0), bloomColor - uThreshold);
    
    return color + bloomColor * strength;
}

// Lens flare effect
vec3 lensFlare(vec2 uv, vec2 center, float intensity)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return vec3(0.0);
    
    direction = normalize(direction);
    
    vec3 flare = vec3(0.0);
    
    // Multiple flare elements
    for (int i = 0; i < 8; i++)
    {
        float flareDistance = distance * (float(i + 1) * 0.2);
        vec2 flarePos = center + direction * flareDistance;
        
        // Check if flare is within screen bounds
        if (flarePos.x >= 0.0 && flarePos.x <= 1.0 && flarePos.y >= 0.0 && flarePos.y <= 1.0)
        {
            float flareSize = 0.01 / (flareDistance + 0.1);
            float flareStrength = intensity / (flareDistance + 1.0);
            
            float flare = exp(-length(uv - flarePos) / flareSize);
            flare *= flareStrength;
            
            // Different colors for different flare elements
            vec3 flareColor = uFlareColor;
            if (i % 2 == 0) flareColor *= vec3(1.0, 0.9, 0.8); // Warm
            else flareColor *= vec3(0.8, 0.9, 1.0); // Cool
            
            flare += flareColor * flare;
        }
    }
    
    return flare;
}

// Neon glow effect
vec3 neonGlow(sampler2D tex, vec2 uv, vec3 neonColor, float width)
{
    vec3 color = texture(tex, uv).rgb;
    vec2 texelSize = 1.0 / uResolution;
    
    // Sample surrounding pixels for edge detection
    vec3 center = color;
    vec3 left = texture(tex, uv + vec2(-texelSize.x, 0.0)).rgb;
    vec3 right = texture(tex, uv + vec2(texelSize.x, 0.0)).rgb;
    vec3 top = texture(tex, uv + vec2(0.0, texelSize.y)).rgb;
    vec3 bottom = texture(tex, uv + vec2(0.0, -texelSize.y)).rgb;
    
    // Calculate gradient
    vec3 gradient = abs(left - center) + abs(right - center) + abs(top - center) + abs(bottom - center);
    float edgeStrength = length(gradient);
    
    // Apply neon glow to edges
    vec3 neon = neonColor * edgeStrength * width;
    
    return color + neon;
}

// Fire glow effect
vec3 fireGlow(sampler2D tex, vec2 uv, float time, float intensity, vec3 fireColor)
{
    vec3 color = texture(tex, uv).rgb;
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    
    // Animated fire effect
    vec2 fireUV = uv + vec2(0.0, -time * 0.1);
    float fire = noise2D(fireUV * 10.0) * intensity;
    
    // Apply fire glow
    float glowAmount = smoothstep(uThreshold - uSoftKnee, uThreshold + uSoftKnee, luminance);
    vec3 glow = fireColor * fire * glowAmount;
    
    return color + glow;
}

// Electric glow effect
vec3 electricGlow(sampler2D tex, vec2 uv, float time, float frequency, vec3 electricColor)
{
    vec3 color = texture(tex, uv).rgb;
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    
    // Animated electric effect
    float electric = sin(uv.x * frequency + time) * cos(uv.y * frequency + time * 1.3);
    electric = abs(electric) * noise2D(uv * 20.0 + time);
    
    // Apply electric glow
    float glowAmount = smoothstep(uThreshold - uSoftKnee, uThreshold + uSoftKnee, luminance);
    vec3 glow = electricColor * electric * glowAmount;
    
    return color + glow;
}

// Magic glow effect
vec3 magicGlow(sampler2D tex, vec2 uv, float time, float intensity, vec3 magicColor)
{
    vec3 color = texture(tex, uv).rgb;
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    
    // Animated magic effect
    vec2 magicUV = uv * 5.0 + time * 0.05;
    float magic = fractalNoise2D(magicUV, 4, 0.5, 2.0) * intensity;
    
    // Add sparkles
    float sparkle = random(vec3(uv * 100.0, time));
    magic += sparkle * 0.5;
    
    // Apply magic glow
    float glowAmount = smoothstep(uThreshold - uSoftKnee, uThreshold + uSoftKnee, luminance);
    vec3 glow = magicColor * magic * glowAmount;
    
    return color + glow;
}

// Toon glow effect
vec3 toonGlow(sampler2D tex, vec2 uv, float threshold, float steps)
{
    vec3 color = texture(tex, uv).rgb;
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    
    // Quantize luminance
    float quantizedLuminance = floor(luminance * steps) / steps;
    
    // Apply threshold
    float glowAmount = step(threshold, quantizedLuminance);
    vec3 glow = color * glowAmount * uStrength;
    
    return color + glow;
}

// Atmospheric glow effect
vec3 atmosphericGlow(sampler2D tex, vec2 uv, float density, vec3 atmColor)
{
    vec3 color = texture(tex, uv).rgb;
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    
    // Atmospheric scattering
    float scatter = exp(-luminance * density);
    vec3 glow = atmColor * (1.0 - scatter) * uStrength;
    
    return color + glow;
}

// Subsurface scattering glow
vec3 subsurfaceGlow(sampler2D tex, vec2 uv, float thickness, vec3 subsurfaceColor)
{
    vec3 color = texture(tex, uv).rgb;
    
    // Simulate subsurface scattering
    vec2 texelSize = 1.0 / uResolution;
    vec3 scattered = vec3(0.0);
    
    for (int x = -2; x <= 2; x++)
    {
        for (int y = -2; y <= 2; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize * thickness;
            vec3 sampleColor = texture(tex, uv + offset).rgb;
            float weight = exp(-length(vec2(float(x), float(y))) / thickness);
            scattered += sampleColor * weight;
        }
    }
    
    scattered /= 25.0; // Normalize
    
    // Apply subsurface glow
    vec3 glow = scattered * subsurfaceColor * uStrength;
    
    return color + glow;
}

// HDR glow effect
vec3 hdrGlow(sampler2D tex, vec2 uv, float range)
{
    vec3 color = texture(tex, uv).rgb;
    
    // HDR processing
    vec3 hdrColor = color / range;
    hdrColor = toneMapACES(hdrColor);
    
    // Apply glow to bright areas
    float luminance = dot(hdrColor, vec3(0.299, 0.587, 0.114));
    float glowAmount = smoothstep(uThreshold - uSoftKnee, uThreshold + uSoftKnee, luminance);
    vec3 glow = hdrColor * glowAmount * uStrength;
    
    return color + glow;
}

// Main glow function
vec3 applyGlow(sampler2D tex, vec2 uv)
{
    vec3 result;
    
    switch (uGlowType)
    {
        case 0: // Basic glow
            result = basicGlow(tex, uv, uStrength, uThreshold);
            break;
        case 1: // Selective glow
            result = selectiveGlow(tex, uv, uSelectColor, uSelectTolerance, uStrength);
            break;
        case 2: // Directional glow
            vec2 direction = vec2(cos(uTime), sin(uTime)); // Animated direction
            result = directionalGlow(tex, uv, direction, uStrength, uRadius);
            break;
        case 3: // Animated glow
            result = animatedGlow(tex, uv, uTime, uAnimationSpeed, uPulseFrequency, uPulseAmplitude);
            break;
        default:
            result = basicGlow(tex, uv, uStrength, uThreshold);
            break;
    }
    
    // Apply additional glow effects
    if (uRadialGlow)
    {
        vec3 radialResult = radialGlow(tex, uv, uGlowCenter, uRadius, uStrength, uGlowFalloff);
        result = mix(result, radialResult, 0.5);
    }
    
    if (uGaussianGlow)
    {
        vec3 gaussianResult = gaussianGlow(tex, uv, uRadius, uSigma);
        result = mix(result, gaussianResult, 0.5);
    }
    
    if (uNeonGlow)
    {
        vec3 neonResult = neonGlow(tex, uv, uNeonColor, uNeonWidth);
        result += neonResult;
    }
    
    if (uFireGlow)
    {
        vec3 fireResult = fireGlow(tex, uv, uTime, uFireIntensity, uFireColor);
        result += fireResult;
    }
    
    if (uElectricGlow)
    {
        vec3 electricResult = electricGlow(tex, uv, uTime, uElectricFrequency, uElectricColor);
        result += electricResult;
    }
    
    if (uMagicGlow)
    {
        vec3 magicResult = magicGlow(tex, uv, uTime, uMagicIntensity, uMagicColor);
        result += magicResult;
    }
    
    if (uToonGlow)
    {
        vec3 toonResult = toonGlow(tex, uv, uToonThreshold, uToonSteps);
        result = mix(result, toonResult, 0.5);
    }
    
    if (uAtmosphericGlow)
    {
        vec3 atmResult = atmosphericGlow(tex, uv, uAtmosphericDensity, uAtmosphericColor);
        result += atmResult;
    }
    
    if (uSubsurfaceGlow)
    {
        vec3 subsurfaceResult = subsurfaceGlow(tex, uv, uSubsurfaceThickness, uSubsurfaceColor);
        result += subsurfaceResult;
    }
    
    if (uHDRGlow)
    {
        vec3 hdrResult = hdrGlow(tex, uv, uHDRRange);
        result = mix(result, hdrResult, 0.5);
    }
    
    // Apply bloom if enabled
    if (uBloomEnabled && uGlowTexture != sampler2D(0))
    {
        vec3 bloomResult = bloomEffect(tex, uGlowTexture, uv, uBloomStrength, uBloomMipLevel);
        result += bloomResult;
    }
    
    // Apply lens flare if enabled
    if (uLensFlare)
    {
        vec3 flareResult = lensFlare(uv, uGlowCenter, uFlareIntensity);
        result += flareResult;
    }
    
    // Apply color tint if enabled
    if (uUseGlowColor)
    {
        result = mix(result, result * uGlowColor, 0.5);
    }
    
    return result;
}

// Luminance-based glow
vec3 luminanceGlow(sampler2D tex, sampler2D lumTex, vec2 uv, float boost)
{
    vec3 color = texture(tex, uv).rgb;
    float luminance = texture(lumTex, uv).r;
    
    // Boost luminance
    luminance *= boost;
    
    // Apply glow
    float glowAmount = smoothstep(uThreshold - uSoftKnee, uThreshold + uSoftKnee, luminance);
    vec3 glow = color * glowAmount * uStrength;
    
    return color + glow;
}

// Main function
void main()
{
    vec3 color;
    
    // Choose glow method
    if (uLuminanceGlow && uLuminanceTexture != sampler2D(0))
    {
        color = luminanceGlow(uTexture, uLuminanceTexture, vTexCoord, uLuminanceBoost);
    }
    else
    {
        color = applyGlow(uTexture, vTexCoord);
    }
    
    // Apply mask if available
    if (uMaskTexture != sampler2D(0))
    {
        float mask = texture(uMaskTexture, vTexCoord).r;
        vec3 originalColor = texture(uTexture, vTexCoord).rgb;
        color = mix(originalColor, color, mask);
    }
    
    // Ensure color is in valid range
    color = saturate(color);
    
    fragColor = vec4(color, 1.0);
}