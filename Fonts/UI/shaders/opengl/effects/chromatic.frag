#version 330 core

// Chromatic Aberration Fragment Shader for OpenGL
// Simulates color fringing and lens dispersion effects

// Input textures
uniform sampler2D uTexture;           // Source texture
uniform sampler2D uLensTexture;      // Optional lens distortion texture
uniform vec2 uResolution;             // Screen resolution
uniform float uStrength;              // Aberration strength
uniform float uTime;                  // Animation time
uniform vec2 uCenter;                 // Aberration center
uniform float uRadius;                // Effect radius
uniform int uAberrationType;          // Type of aberration (0=radial, 1=directional, 2=spectral)
uniform vec3 uChromaticShift;         // RGB shift amounts
uniform bool uAnimate;                // Animate the effect
uniform float uDistortion;            // Lens distortion amount
uniform float uDispersion;            // Spectral dispersion
uniform bool uUseLensTexture;         // Use lens texture for distortion
uniform float uFocalLength;           // Simulated focal length
uniform float uAperture;              // Simulated aperture size
uniform float uWavelength[3];         // RGB wavelengths (in nanometers)
uniform bool uRefractiveIndex;        // Use refractive index calculations
uniform float uTemperature;           // Temperature for thermal effects
uniform bool uVignette;               // Add vignette effect
uniform float uVignetteStrength;      // Vignette strength
uniform vec4 uVignetteColor;          // Vignette color
uniform bool uFilmGrain;              // Add film grain
uniform float uGrainStrength;         // Grain strength
uniform bool uAnamorphic;             // Anamorphic lens effect
uniform float uAnamorphicSqueeze;     // Anamorphic squeeze factor
uniform bool uLensFlare;              // Add lens flare
uniform vec3 uFlareColor;             // Flare color
uniform float uFlareIntensity;        // Flare intensity

// Input from vertex shader
in vec2 vTexCoord;
in vec2 vScreenPos;

// Output color
out vec4 fragColor;

// Include common functions
#include "common/common.glsl"
#include "common/noise.glsl"

// Wavelength to RGB conversion
vec3 wavelengthToRGB(float wavelength)
{
    // Simplified wavelength to RGB conversion
    float r, g, b;
    
    if (wavelength >= 380.0 && wavelength < 440.0)
    {
        r = -(wavelength - 440.0) / (440.0 - 380.0);
        g = 0.0;
        b = 1.0;
    }
    else if (wavelength >= 440.0 && wavelength < 490.0)
    {
        r = 0.0;
        g = (wavelength - 440.0) / (490.0 - 440.0);
        b = 1.0;
    }
    else if (wavelength >= 490.0 && wavelength < 510.0)
    {
        r = 0.0;
        g = 1.0;
        b = -(wavelength - 510.0) / (510.0 - 490.0);
    }
    else if (wavelength >= 510.0 && wavelength < 580.0)
    {
        r = (wavelength - 510.0) / (580.0 - 510.0);
        g = 1.0;
        b = 0.0;
    }
    else if (wavelength >= 580.0 && wavelength < 645.0)
    {
        r = 1.0;
        g = -(wavelength - 645.0) / (645.0 - 580.0);
        b = 0.0;
    }
    else if (wavelength >= 645.0 && wavelength < 781.0)
    {
        r = 1.0;
        g = 0.0;
        b = 0.0;
    }
    else
    {
        r = 0.0;
        g = 0.0;
        b = 0.0;
    }
    
    return vec3(r, g, b);
}

// Refractive index calculation (Cauchy's equation)
float refractiveIndex(float wavelength, float A, float B)
{
    // Simplified Cauchy's equation: n = A + B/λ²
    float lambda = wavelength * 1e-3; // Convert nm to μm
    return A + B / (lambda * lambda);
}

// Lens distortion calculation
vec2 lensDistortion(vec2 uv, vec2 center, float strength, float k1, float k2)
{
    vec2 p = uv - center;
    float r = length(p);
    
    if (r < 0.001) return uv;
    
    float r2 = r * r;
    float r4 = r2 * r2;
    
    // Barrel distortion formula
    float distortion = 1.0 + k1 * r2 + k2 * r4;
    
    return center + p * distortion * strength;
}

// Radial chromatic aberration
vec3 radialChromaticAberration(sampler2D tex, vec2 uv, vec2 center, float strength)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return texture(tex, uv).rgb;
    
    direction = normalize(direction);
    
    // Sample RGB channels at different distances
    vec2 redUV = uv + direction * strength * uChromaticShift.r * distance;
    vec2 greenUV = uv + direction * strength * uChromaticShift.g * distance;
    vec2 blueUV = uv + direction * strength * uChromaticShift.b * distance;
    
    vec3 color;
    color.r = texture(tex, redUV).r;
    color.g = texture(tex, greenUV).g;
    color.b = texture(tex, blueUV).b;
    
    return color;
}

// Directional chromatic aberration
vec3 directionalChromaticAberration(sampler2D tex, vec2 uv, vec2 direction, float strength)
{
    direction = normalize(direction);
    
    // Sample RGB channels at different offsets
    vec2 redUV = uv + direction * strength * uChromaticShift.r;
    vec2 greenUV = uv + direction * strength * uChromaticShift.g;
    vec2 blueUV = uv + direction * strength * uChromaticShift.b;
    
    vec3 color;
    color.r = texture(tex, redUV).r;
    color.g = texture(tex, greenUV).g;
    color.b = texture(tex, blueUV).b;
    
    return color;
}

// Spectral chromatic aberration (wavelength-based)
vec3 spectralChromaticAberration(sampler2D tex, vec2 uv, vec2 center, float strength)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return texture(tex, uv).rgb;
    
    direction = normalize(direction);
    
    vec3 color = vec3(0.0);
    
    for (int i = 0; i < 3; i++)
    {
        float wavelength = uWavelength[i];
        float shift = strength * (wavelength - 550.0) / 100.0; // Normalize around green
        
        if (uRefractiveIndex)
        {
            // Use refractive index for more accurate dispersion
            float n = refractiveIndex(wavelength, 1.5, 0.004); // Crown glass approximation
            shift *= (n - 1.5) * 10.0; // Scale for visibility
        }
        
        vec2 sampleUV = uv + direction * shift * distance;
        vec3 channelColor = texture(tex, sampleUV).rgb;
        
        // Weight by wavelength sensitivity
        vec3 wavelengthColor = wavelengthToRGB(wavelength);
        color += channelColor * wavelengthColor;
    }
    
    return color;
}

// Temperature-based aberration (thermal effects)
vec3 thermalChromaticAberration(sampler2D tex, vec2 uv, vec2 center, float strength, float temperature)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return texture(tex, uv).rgb;
    
    direction = normalize(direction);
    
    // Temperature affects dispersion
    float tempFactor = 1.0 + (temperature - 20.0) / 100.0; // Normalize around 20°C
    
    vec3 color;
    vec2 redUV = uv + direction * strength * uChromaticShift.r * distance * tempFactor;
    vec2 greenUV = uv + direction * strength * uChromaticShift.g * distance;
    vec2 blueUV = uv + direction * strength * uChromaticShift.b * distance / tempFactor;
    
    color.r = texture(tex, redUV).r;
    color.g = texture(tex, greenUV).g;
    color.b = texture(tex, blueUV).b;
    
    return color;
}

// Anamorphic chromatic aberration
vec3 anamorphicChromaticAberration(sampler2D tex, vec2 uv, vec2 center, float strength, float squeeze)
{
    vec2 direction = uv - center;
    direction.y *= squeeze; // Squeeze vertical direction
    
    float distance = length(direction);
    
    if (distance < 0.001) return texture(tex, uv).rgb;
    
    direction = normalize(direction);
    direction.y /= squeeze; // Un-squeeze for sampling
    
    vec3 color;
    vec2 redUV = uv + direction * strength * uChromaticShift.r * distance;
    vec2 greenUV = uv + direction * strength * uChromaticShift.g * distance;
    vec2 blueUV = uv + direction * strength * uChromaticShift.b * distance;
    
    color.r = texture(tex, redUV).r;
    color.g = texture(tex, greenUV).g;
    color.b = texture(tex, blueUV).b;
    
    return color;
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
    for (int i = 0; i < 6; i++)
    {
        float flareDistance = distance * (float(i + 1) * 0.3);
        vec2 flarePos = center + direction * flareDistance;
        
        // Check if flare is within screen bounds
        if (flarePos.x >= 0.0 && flarePos.x <= 1.0 && flarePos.y >= 0.0 && flarePos.y <= 1.0)
        {
            float flareSize = 0.02 / (flareDistance + 0.1);
            float flareStrength = intensity / (flareDistance + 1.0);
            
            float flare = exp(-length(uv - flarePos) / flareSize);
            flare *= flareStrength;
            
            // Different colors for different flare elements
            vec3 flareColor = uFlareColor;
            if (i % 2 == 0) flareColor *= vec3(1.0, 0.8, 0.6); // Warm
            else flareColor *= vec3(0.6, 0.8, 1.0); // Cool
            
            flare += flareColor * flare;
        }
    }
    
    return flare;
}

// Vignette effect
vec3 vignette(vec3 color, vec2 uv, vec2 center, float strength)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    float vignette = 1.0 - smoothstep(0.0, 1.0, distance * strength);
    vignette = pow(vignette, 2.0); // Make vignette fall off more gradually
    
    return mix(color * uVignetteColor.rgb, color, vignette);
}

// Film grain effect
vec3 filmGrain(vec3 color, vec2 uv, float time, float strength)
{
    float grain = random(vec3(uv * 100.0, time)) * 2.0 - 1.0;
    grain = grain * strength;
    
    return color + grain;
}

// Animated chromatic aberration
vec3 animatedChromaticAberration(sampler2D tex, vec2 uv, vec2 center, float strength, float time)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return texture(tex, uv).rgb;
    
    direction = normalize(direction);
    
    // Animate the shift amounts
    float animStrength = strength * (1.0 + sin(time * 2.0) * 0.3);
    
    vec2 redUV = uv + direction * animStrength * uChromaticShift.r * distance;
    vec2 greenUV = uv + direction * animStrength * uChromaticShift.g * distance;
    vec2 blueUV = uv + direction * animStrength * uChromaticShift.b * distance;
    
    vec3 color;
    color.r = texture(tex, redUV).r;
    color.g = texture(tex, greenUV).g;
    color.b = texture(tex, blueUV).b;
    
    return color;
}

// Main chromatic aberration function
vec3 applyChromaticAberration(sampler2D tex, vec2 uv)
{
    vec3 result;
    
    // Apply lens distortion if enabled
    vec2 distortedUV = uv;
    if (uDistortion > 0.0)
    {
        if (uUseLensTexture)
        {
            vec4 lensData = texture(uLensTexture, uv);
            distortedUV = lensDistortion(uv, uCenter, 1.0, uDistortion, lensData.r);
        }
        else
        {
            distortedUV = lensDistortion(uv, uCenter, 1.0, uDistortion, uDistortion * 0.5);
        }
    }
    
    // Apply aberration based on type
    switch (uAberrationType)
    {
        case 0: // Radial
            result = radialChromaticAberration(tex, distortedUV, uCenter, uStrength);
            break;
        case 1: // Directional
            vec2 dir = vec2(cos(uTime), sin(uTime)); // Animated direction
            result = directionalChromaticAberration(tex, distortedUV, dir, uStrength);
            break;
        case 2: // Spectral
            result = spectralChromaticAberration(tex, distortedUV, uCenter, uStrength);
            break;
        case 3: // Thermal
            result = thermalChromaticAberration(tex, distortedUV, uCenter, uStrength, uTemperature);
            break;
        case 4: // Anamorphic
            result = anamorphicChromaticAberration(tex, distortedUV, uCenter, uStrength, uAnamorphicSqueeze);
            break;
        default:
            result = radialChromaticAberration(tex, distortedUV, uCenter, uStrength);
            break;
    }
    
    // Apply animation if enabled
    if (uAnimate && uTime > 0.0)
    {
        result = animatedChromaticAberration(tex, distortedUV, uCenter, uStrength, uTime);
    }
    
    return result;
}

// Depth-aware chromatic aberration
vec3 depthAwareChromaticAberration(sampler2D tex, sampler2D depthTex, vec2 uv, vec2 center, float strength)
{
    float depth = texture(depthTex, uv).r;
    float depthFactor = 1.0 - depth; // Closer objects have more aberration
    
    vec3 result = applyChromaticAberration(tex, uv);
    
    // Blend with original based on depth
    vec3 original = texture(tex, uv).rgb;
    return mix(original, result, depthFactor);
}

// Edge-aware chromatic aberration
vec3 edgeAwareChromaticAberration(sampler2D tex, vec2 uv, vec2 center, float strength)
{
    vec2 texelSize = 1.0 / uResolution;
    
    // Calculate edge strength
    vec3 centerColor = texture(tex, uv).rgb;
    vec3 rightColor = texture(tex, uv + vec2(texelSize.x, 0.0)).rgb;
    vec3 bottomColor = texture(tex, uv + vec2(0.0, texelSize.y)).rgb;
    
    float edgeStrength = length(centerColor - rightColor) + length(centerColor - bottomColor);
    edgeStrength = saturate(edgeStrength * 2.0); // Scale and clamp
    
    vec3 aberrated = applyChromaticAberration(tex, uv);
    
    // Blend based on edge strength
    return mix(centerColor, aberrated, edgeStrength * strength);
}

// Main function
void main()
{
    vec3 color;
    
    // Apply chromatic aberration
    color = applyChromaticAberration(uTexture, vTexCoord);
    
    // Apply lens flare if enabled
    if (uLensFlare)
    {
        vec3 flare = lensFlare(vTexCoord, uCenter, uFlareIntensity);
        color += flare;
    }
    
    // Apply vignette if enabled
    if (uVignette)
    {
        color = vignette(color, vTexCoord, uCenter, uVignetteStrength);
    }
    
    // Apply film grain if enabled
    if (uFilmGrain)
    {
        color = filmGrain(color, vTexCoord, uTime, uGrainStrength);
    }
    
    // Apply dispersion effect
    if (uDispersion > 0.0)
    {
        vec2 direction = vTexCoord - uCenter;
        float distance = length(direction);
        
        if (distance > 0.001)
        {
            direction = normalize(direction);
            
            // Add rainbow-like dispersion
            for (int i = 0; i < 7; i++)
            {
                float wavelength = 400.0 + float(i) * 50.0; // 400-700nm range
                vec3 wavelengthColor = wavelengthToRGB(wavelength);
                float shift = uDispersion * (wavelength - 550.0) / 100.0;
                
                vec2 sampleUV = vTexCoord + direction * shift * distance;
                vec3 sampleColor = texture(uTexture, sampleUV).rgb;
                
                color += sampleColor * wavelengthColor * 0.1;
            }
        }
    }
    
    // Apply aperture-based aberration
    if (uAperture > 0.0)
    {
        float apertureFactor = 1.0 / (uAperture + 1.0);
        vec3 apertureColor = applyChromaticAberration(uTexture, vTexCoord);
        color = mix(color, apertureColor, apertureFactor * 0.5);
    }
    
    // Ensure color is in valid range
    color = saturate(color);
    
    fragColor = vec4(color, 1.0);
}