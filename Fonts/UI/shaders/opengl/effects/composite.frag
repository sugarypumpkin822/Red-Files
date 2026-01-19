#version 330 core

// Composite Fragment Shader for OpenGL
// Combines multiple render targets and applies post-processing effects

// Input textures
uniform sampler2D uSceneTexture;        // Main scene texture
uniform sampler2D uBloomTexture;        // Bloom/glow texture
uniform sampler2D uDepthTexture;        // Depth buffer
uniform sampler2D uNormalTexture;       // Normal buffer
uniform sampler2D uMotionTexture;       // Motion vectors
uniform sampler2D uOcclusionTexture;    // Ambient occlusion
uniform sampler2D uReflectionTexture;   // Reflection buffer
uniform sampler2D uRefractionTexture;    // Refraction buffer
uniform sampler2D uLUTTexture;          // Color lookup table
uniform sampler2D uNoiseTexture;        // Noise texture
uniform sampler2D uVignetteTexture;     // Vignette mask
uniform sampler2D uGradientTexture;     // Color gradient
uniform sampler2D uMaskTexture;          // Effect mask
uniform sampler2D uFogTexture;          // Fog density
uniform sampler2D uLensTexture;          // Lens distortion
uniform samplerCube uEnvironmentMap;     // Environment cubemap
uniform samplerCube uIrradianceMap;      // Irradiance cubemap
uniform sampler2D uBRDFLUT;             // BRDF lookup table

// Uniform parameters
uniform vec2 uResolution;              // Screen resolution
uniform float uTime;                    // Animation time
uniform vec3 uCameraPosition;           // Camera world position
uniform vec3 uCameraDirection;          // Camera view direction
uniform mat4 uViewMatrix;               // View matrix
uniform mat4 uProjectionMatrix;         // Projection matrix
uniform mat4 uInverseViewMatrix;        // Inverse view matrix
uniform mat4 uInverseProjectionMatrix;  // Inverse projection matrix

// Post-processing parameters
uniform float uExposure;                // Exposure adjustment
uniform float uGamma;                   // Gamma correction
uniform float uContrast;                // Contrast
uniform float uBrightness;              // Brightness
uniform float uSaturation;               // Saturation
uniform vec3 uColorFilter;              // Color filter/tint
uniform float uColorTemperature;        // Color temperature (Kelvin)
uniform float uColorTint;               // Color tint (green-magenta)
uniform float uVibrance;                // Vibrance
uniform float uLift;                    // Lift (shadows)
uniform float uGammaGain;               // Gamma (midtones)
uniform float uGain;                    // Gain (highlights)

// Bloom parameters
uniform float uBloomStrength;           // Bloom intensity
uniform float uBloomThreshold;          // Bloom threshold
uniform float uBloomTint;               // Bloom color tint
uniform bool uBloomEnabled;             // Enable bloom
uniform int uBloomMipLevel;             // Bloom mip level

// Fog parameters
uniform vec3 uFogColor;                 // Fog color
uniform float uFogDensity;              // Fog density
uniform float uFogStart;                 // Fog start distance
uniform float uFogEnd;                   // Fog end distance
uniform bool uFogEnabled;                // Enable fog
uniform int uFogType;                   // Fog type (0=linear, 1=exponential, 2=exponential2)

// Vignette parameters
uniform float uVignetteStrength;        // Vignette strength
uniform vec2 uVignetteCenter;           // Vignette center
uniform float uVignetteRadius;          // Vignette radius
uniform bool uVignetteEnabled;          // Enable vignette

// Tone mapping parameters
uniform int uToneMapping;               // Tone mapping method (0=none, 1=ACES, 2=Reinhard, 3=Uncharted)
uniform float uToneMappingStrength;     // Tone mapping strength

// Film grain parameters
uniform float uGrainStrength;           // Grain strength
uniform float uGrainScale;              // Grain scale
uniform bool uGrainEnabled;             // Enable grain

// Dithering parameters
uniform bool uDitheringEnabled;         // Enable dithering
uniform float uDitherStrength;          // Dithering strength

// Screen effects
uniform bool uScreenEffect;             // Enable screen effects
uniform int uScreenEffectType;          // Screen effect type
uniform float uScreenEffectStrength;    // Screen effect strength

// Color grading
uniform bool uColorGradingEnabled;      // Enable color grading
uniform vec3 uShadowsColor;             // Shadows color
uniform vec3 uMidtonesColor;            // Midtones color
uniform vec3 uHighlightsColor;          // Highlights color
uniform float uShadowsRange;            // Shadows range
uniform float uHighlightsRange;         // Highlights range

// Input from vertex shader
in vec2 vTexCoord;
in vec2 vScreenPos;

// Output color
out vec4 fragColor;

// Include common functions
#include "common/common.glsl"
#include "common/lighting.glsl"
#include "common/noise.glsl"

// Color temperature to RGB conversion
vec3 temperatureToRGB(float kelvin)
{
    float temp = kelvin / 100.0;
    float red, green, blue;
    
    // Calculate red
    if (temp <= 66.0)
    {
        red = 1.0;
    }
    else
    {
        red = clamp(temp - 60.0, 0.0, 100.0) / 100.0;
        red = 1.293 * pow(red, -0.1332047592);
    }
    
    // Calculate green
    if (temp <= 66.0)
    {
        green = 0.39008157876901960784 * log(temp) - 0.63184144378862745098;
    }
    else
    {
        green = clamp(temp - 60.0, 0.0, 100.0) / 100.0;
        green = 1.293 * pow(green, -0.0755148492);
    }
    
    // Calculate blue
    if (temp >= 66.0)
    {
        blue = 1.0;
    }
    else if (temp >= 19.0)
    {
        blue = 0.54320678911019607843 * log(temp - 10.0) - 1.19625408914;
    }
    else
    {
        blue = 0.0;
    }
    
    return vec3(red, green, blue);
}

// Tone mapping functions
vec3 toneMapACES(vec3 color)
{
    const float A = 2.51;
    const float B = 0.03;
    const float C = 2.43;
    const float D = 0.59;
    const float E = 0.14;
    return clamp((color * (A * color + B)) / (color * (C * color + D) + E), 0.0, 1.0);
}

vec3 toneMapReinhard(vec3 color)
{
    return color / (1.0 + color);
}

vec3 toneMapUncharted(vec3 color)
{
    const float A = 0.15;
    const float B = 0.50;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.02;
    const float F = 0.30;
    
    return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
}

// Color grading functions
vec3 colorGradeLiftGammaGain(vec3 color)
{
    // Lift (affects shadows)
    vec3 lift = vec3(uLift) * uShadowsColor;
    
    // Gamma (affects midtones)
    vec3 gamma = vec3(uGammaGain) * uMidtonesColor;
    
    // Gain (affects highlights)
    vec3 gain = vec3(uGain) * uHighlightsColor;
    
    // Apply lift/gamma/gain
    color = color * gain + lift;
    color = pow(color, 1.0 / gamma);
    
    return color;
}

vec3 colorGradeThreeWay(vec3 color)
{
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    
    // Shadows
    float shadowWeight = smoothstep(0.0, uShadowsRange, luminance);
    vec3 shadowsColor = mix(uShadowsColor, vec3(1.0), shadowWeight);
    
    // Midtones
    float midtoneWeight = smoothstep(uShadowsRange, 1.0 - uHighlightsRange, luminance);
    vec3 midtonesColor = mix(uMidtonesColor, vec3(1.0), midtoneWeight);
    
    // Highlights
    float highlightWeight = smoothstep(1.0 - uHighlightsRange, 1.0, luminance);
    vec3 highlightsColor = mix(uHighlightsColor, vec3(1.0), highlightWeight);
    
    // Combine
    color *= shadowsColor * midtonesColor * highlightsColor;
    
    return color;
}

// Vibrance adjustment
vec3 adjustVibrance(vec3 color, float vibrance)
{
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    vec3 gray = vec3(luminance);
    
    float saturation = max(color.r, max(color.g, color.b)) - min(color.r, min(color.g, color.b));
    float weight = saturate(abs(saturation) * vibrance);
    
    return mix(gray, color, 1.0 + weight);
}

// Film grain
vec3 applyFilmGrain(vec3 color, vec2 uv, float time)
{
    vec2 grainUV = uv * uGrainScale;
    float grain = texture(uNoiseTexture, grainUV + time * 0.1).r;
    grain = grain * 2.0 - 1.0;
    
    return color + grain * uGrainStrength * 0.1;
}

// Dithering
vec3 applyDithering(vec3 color, vec2 uv)
{
    float dither = texture(uNoiseTexture, uv * 2.0).r;
    dither = (dither - 0.5) * uDitherStrength;
    
    return color + vec3(dither) / 255.0;
}

// Vignette
vec3 applyVignette(vec3 color, vec2 uv)
{
    vec2 center = uVignetteCenter;
    vec2 direction = uv - center;
    float distance = length(direction);
    
    float vignette = 1.0 - smoothstep(uVignetteRadius * 0.5, uVignetteRadius, distance);
    vignette = pow(vignette, uVignetteStrength);
    
    if (uVignetteTexture != sampler2D(0))
    {
        vec4 vignetteMask = texture(uVignetteTexture, uv);
        vignette *= vignetteMask.r;
    }
    
    return color * vignette;
}

// Fog effects
vec3 applyFog(vec3 color, float depth, vec3 worldPos)
{
    if (!uFogEnabled) return color;
    
    float fogFactor = 0.0;
    float viewDistance = length(worldPos - uCameraPosition);
    
    switch (uFogType)
    {
        case 0: // Linear fog
            fogFactor = linearFog(viewDistance, uFogStart, uFogEnd);
            break;
        case 1: // Exponential fog
            fogFactor = exponentialFog(viewDistance, uFogDensity);
            break;
        case 2: // Exponential squared fog
            fogFactor = exponentialSquaredFog(viewDistance, uFogDensity);
            break;
    }
    
    // Apply fog texture if available
    if (uFogTexture != sampler2D(0))
    {
        vec2 fogUV = vec2(viewDistance * 0.01, uTime * 0.1);
        vec3 fogColor = texture(uFogTexture, fogUV).rgb;
        return mix(color, fogColor, fogFactor);
    }
    
    return mix(color, uFogColor, fogFactor);
}

// Bloom effect
vec3 applyBloom(vec3 color, vec2 uv)
{
    if (!uBloomEnabled) return color;
    
    vec3 bloomColor = textureLod(uBloomTexture, uv, float(uBloomMipLevel)).rgb;
    
    // Apply threshold
    bloomColor = max(vec3(0.0), bloomColor - uBloomThreshold);
    
    // Apply tint
    vec3 bloomTint = vec3(uBloomTint);
    bloomColor *= bloomTint;
    
    return color + bloomColor * uBloomStrength;
}

// Screen space reflections
vec3 applySSR(vec3 color, vec2 uv, vec3 normal, vec3 viewDir, float roughness)
{
    if (uReflectionTexture == sampler2D(0)) return color;
    
    vec4 reflection = texture(uReflectionTexture, uv);
    
    // Roughness affects reflection strength
    float reflectionStrength = 1.0 - roughness;
    
    return mix(color, reflection.rgb, reflection.a * reflectionStrength);
}

// Ambient occlusion
vec3 applyAO(vec3 color, vec2 uv)
{
    if (uOcclusionTexture == sampler2D(0)) return color;
    
    float ao = texture(uOcclusionTexture, uv).r;
    
    return color * ao;
}

// Color lookup table
vec3 applyLUT(vec3 color)
{
    if (uLUTTexture == sampler2D(0)) return color;
    
    // Convert to LUT coordinates
    float blueIndex = color.b * 63.0;
    vec2 lutUV;
    lutUV.x = (floor(blueIndex) + color.r) / 64.0;
    lutUV.y = (63.0 - floor(blueIndex) + color.g) / 64.0;
    
    return texture(uLUTTexture, lutUV).rgb;
}

// Screen effects
vec3 applyScreenEffect(vec3 color, vec2 uv, float time)
{
    if (!uScreenEffect) return color;
    
    switch (uScreenEffectType)
    {
        case 0: // Scanlines
            float scanline = sin(uv.y * uResolution.y * 2.0) * 0.5 + 0.5;
            color *= mix(1.0, scanline, uScreenEffectStrength);
            break;
            
        case 1: // CRT effect
            vec2 crtUV = uv * 2.0 - 1.0;
            float distortion = length(crtUV) * 0.1;
            crtUV *= (1.0 + distortion);
            crtUV = crtUV * 0.5 + 0.5;
            
            if (crtUV.x >= 0.0 && crtUV.x <= 1.0 && crtUV.y >= 0.0 && crtUV.y <= 1.0)
            {
                color = texture(uSceneTexture, crtUV).rgb;
            }
            
            // Add scanlines
            float scanline2 = sin(uv.y * uResolution.y * 3.0) * 0.5 + 0.5;
            color *= mix(1.0, scanline2, uScreenEffectStrength * 0.5);
            break;
            
        case 2: // Glitch effect
            float glitch = random(vec3(uv * 10.0, time)) * uScreenEffectStrength;
            vec2 glitchUV = uv;
            glitchUV.x += glitch * 0.01;
            color = texture(uSceneTexture, glitchUV).rgb;
            
            // Color channel separation
            color.r = texture(uSceneTexture, uv + vec2(glitch * 0.02, 0.0)).r;
            color.b = texture(uSceneTexture, uv - vec2(glitch * 0.02, 0.0)).b;
            break;
            
        case 3: // Dream effect
            float dream = sin(time * 0.5) * 0.5 + 0.5;
            vec2 dreamUV = uv + vec2(sin(time + uv.x * 10.0), cos(time + uv.y * 10.0)) * 0.01 * uScreenEffectStrength;
            color = mix(color, texture(uSceneTexture, dreamUV).rgb, dream * uScreenEffectStrength);
            break;
    }
    
    return color;
}

// Lens distortion
vec3 applyLensDistortion(vec3 color, vec2 uv)
{
    if (uLensTexture == sampler2D(0)) return color;
    
    vec4 lensData = texture(uLensTexture, uv);
    vec2 distortedUV = uv + (lensData.rg - 0.5) * 0.1;
    
    if (distortedUV.x >= 0.0 && distortedUV.x <= 1.0 && distortedUV.y >= 0.0 && distortedUV.y <= 1.0)
    {
        return texture(uSceneTexture, distortedUV).rgb;
    }
    
    return color;
}

// Environment reflection
vec3 applyEnvironmentReflection(vec3 color, vec3 normal, vec3 viewDir, float roughness, float metallic)
{
    if (uEnvironmentMap == samplerCube(0)) return color;
    
    vec3 reflectDir = reflect(-viewDir, normal);
    
    // Sample environment map with roughness
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 envColor = textureLod(uEnvironmentMap, reflectDir, roughness * MAX_REFLECTION_LOD).rgb;
    
    // Sample irradiance for diffuse
    vec3 irradiance = texture(uIrradianceMap, normal).rgb;
    
    // Sample BRDF LUT
    vec2 brdf = texture(uBRDFLUT, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
    
    // Calculate Fresnel
    vec3 F0 = mix(vec3(0.04), color, metallic);
    vec3 F = calculateFresnelSchlick(max(dot(normal, viewDir), 0.0), F0);
    
    // Combine diffuse and specular
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    vec3 diffuse = irradiance * kD;
    vec3 specular = envColor * (F * brdf.x + brdf.y);
    
    return color * (diffuse + specular) * 0.5;
}

// Motion blur
vec3 applyMotionBlur(vec3 color, vec2 uv)
{
    if (uMotionTexture == sampler2D(0)) return color;
    
    vec2 motion = texture(uMotionTexture, uv).rg;
    motion *= 0.5; // Scale motion
    
    if (length(motion) < 0.001) return color;
    
    vec3 blurredColor = vec3(0.0);
    int samples = 8;
    
    for (int i = 0; i < samples; i++)
    {
        float t = float(i) / float(samples - 1) - 0.5;
        vec2 sampleUV = uv + motion * t;
        blurredColor += texture(uSceneTexture, sampleUV).rgb;
    }
    
    blurredColor /= float(samples);
    
    return mix(color, blurredColor, 0.5);
}

// Main composite function
vec3 compositeScene(vec2 uv)
{
    // Sample main scene
    vec3 color = texture(uSceneTexture, uv).rgb;
    
    // Get depth and reconstruct world position
    float depth = texture(uDepthTexture, uv).r;
    vec3 worldPos = screenToWorld(vec3(uv * uResolution, depth), uInverseViewProjection, uResolution);
    
    // Get normal and view direction
    vec3 normal = texture(uNormalTexture, uv).rgb * 2.0 - 1.0;
    vec3 viewDir = normalize(uCameraPosition - worldPos);
    
    // Apply bloom
    color = applyBloom(color, uv);
    
    // Apply ambient occlusion
    color = applyAO(color, uv);
    
    // Apply screen space reflections
    float roughness = 0.5; // Default roughness
    if (uNormalTexture != sampler2D(0))
    {
        // Extract roughness from normal texture alpha if available
        roughness = texture(uNormalTexture, uv).a;
    }
    color = applySSR(color, uv, normal, viewDir, roughness);
    
    // Apply environment reflections
    float metallic = 0.0; // Default metallic
    color = applyEnvironmentReflection(color, normal, viewDir, roughness, metallic);
    
    // Apply motion blur
    color = applyMotionBlur(color, uv);
    
    // Apply fog
    color = applyFog(color, depth, worldPos);
    
    // Apply lens distortion
    color = applyLensDistortion(color, uv);
    
    // Apply screen effects
    color = applyScreenEffect(color, uv, uTime);
    
    // Apply color grading
    if (uColorGradingEnabled)
    {
        color = colorGradeThreeWay(color);
        color = colorGradeLiftGammaGain(color);
    }
    
    // Apply color adjustments
    color *= uExposure;
    color = colorGrade(color, uContrast, uBrightness, uSaturation);
    color = adjustVibrance(color, uVibrance);
    
    // Apply color temperature and tint
    vec3 tempColor = temperatureToRGB(uColorTemperature);
    color *= tempColor;
    color = mix(color, color * vec3(1.0 + uColorTint, 1.0 - uColorTint, 1.0), 0.5);
    
    // Apply color filter
    color *= uColorFilter;
    
    // Apply vignette
    if (uVignetteEnabled)
    {
        color = applyVignette(color, uv);
    }
    
    // Apply film grain
    if (uGrainEnabled)
    {
        color = applyFilmGrain(color, uv, uTime);
    }
    
    // Apply dithering
    if (uDitheringEnabled)
    {
        color = applyDithering(color, uv);
    }
    
    // Apply LUT
    color = applyLUT(color);
    
    // Apply tone mapping
    switch (uToneMapping)
    {
        case 1:
            color = toneMapACES(color);
            break;
        case 2:
            color = toneMapReinhard(color);
            break;
        case 3:
            color = toneMapUncharted(color);
            break;
    }
    
    // Apply gamma correction
    color = gammaCorrect(color, uGamma);
    
    // Apply mask if available
    if (uMaskTexture != sampler2D(0))
    {
        float mask = texture(uMaskTexture, uv).r;
        vec3 originalColor = texture(uSceneTexture, uv).rgb;
        color = mix(originalColor, color, mask);
    }
    
    return color;
}

// Main function
void main()
{
    vec3 color = compositeScene(vTexCoord);
    
    // Ensure color is in valid range
    color = saturate(color);
    
    fragColor = vec4(color, 1.0);
}