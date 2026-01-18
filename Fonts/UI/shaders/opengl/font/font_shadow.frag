#version 330 core

// Font Shadow Fragment Shader for OpenGL
// Specialized shader for rendering font shadows with various effects

// Input textures
uniform sampler2D uTexture;           // Font texture (alpha channel)
uniform sampler2D uShadowTexture;    // Separate shadow texture (optional)
uniform sampler2D uNoiseTexture;       // Noise texture for animated shadows
uniform sampler2D uGradientTexture;    // Gradient texture for shadow
uniform vec2 uResolution;             // Screen resolution
uniform vec4 uTextColor;              // Font color (RGBA)
uniform vec4 uShadowColor;            // Shadow color
uniform vec4 uGlowColor;              // Glow color
uniform float uShadowOffsetX;          // Shadow X offset
uniform float uShadowOffsetY;          // Shadow Y offset
uniform float uShadowBlur;             // Shadow blur amount
uniform float uShadowIntensity;        // Shadow intensity
uniform float uShadowAlpha;            // Shadow alpha
uniform bool uEnableShadow;             // Enable shadow effect
uniform bool uEnableGlow;               // Enable glow effect
uniform bool uEnableGradient;          // Enable gradient on shadow
uniform bool uEnableTextureShadow;     // Use separate texture for shadow
uniform bool uEnableAntialiasing;       // Enable antialiasing
uniform float uAntialiasingStrength;    // Antialiasing strength
uniform bool uPremultipliedAlpha;     // Texture uses premultiplied alpha
uniform bool uGammaCorrect;           // Apply gamma correction
uniform float uGamma;                 // Gamma value
uniform bool uDithering;              // Enable dithering
uniform float uDitherStrength;         // Dithering strength
uniform bool uSubpixelRendering;       // Subpixel rendering
uniform vec3 uSubpixelWeights;         // RGB subpixel weights
uniform bool uDebugMode;               // Debug visualization mode
uniform int uDebugChannel;              // Debug channel (0=text, 1=shadow, 2=combined)
uniform float uTime;                   // Animation time
uniform bool uAnimated;                // Enable animation
uniform float uAnimationSpeed;         // Animation speed
uniform float uAnimationPhase;         // Animation phase
uniform bool uPulseAnimation;          // Pulse animation
uniform float uPulseSpeed;             // Pulse speed
uniform float uPulseAmount;            // Pulse amount
uniform bool uWaveAnimation;           // Wave animation
uniform float uWaveAmplitude;          // Wave amplitude
uniform float uWaveFrequency;          // Wave frequency
uniform bool uColorShift;              // Color shift animation
uniform float uColorShiftSpeed;        // Color shift speed
uniform bool uFlickerAnimation;        // Flicker animation
uniform float uFlickerSpeed;            // Flicker speed
uniform float uFlickerAmount;          // Flicker amount
uniform bool uBreathingAnimation;       // Breathing animation
uniform float uBreathingSpeed;          // Breathing speed
uniform float uBreathingAmount;        // Breathing amount
uniform bool uPerspectiveShadow;        // Perspective shadow
uniform float uPerspectiveAmount;      // Perspective amount
uniform bool uRadialShadow;            // Radial shadow
uniform float uRadialRadius;           // Radial radius
uniform float uRadialStrength;         // Radial strength
uniform bool uMultipleShadows;          // Multiple shadows
uniform int uShadowCount;               // Number of shadows
uniform vec2 uShadowOffsets[5];         // Shadow offsets for multiple shadows
uniform vec4 uShadowColors[5];         // Shadow colors for multiple shadows
uniform float uShadowIntensities[5];    // Shadow intensities for multiple shadows
uniform bool uVolumetricShadow;         // Volumetric shadow
uniform float uVolumetricDensity;      // Volumetric density
uniform float uVolumetricSamples;       // Volumetric samples
uniform bool uSoftShadow;               // Soft shadow
uniform float uSoftShadowRadius;        // Soft shadow radius
uniform float uSoftShadowSamples;       // Soft shadow samples
uniform bool uContactShadow;            // Contact shadow
uniform float uContactDistance;         // Contact distance
uniform float uContactFade;             // Contact fade distance
uniform bool uDistortedShadow;         // Distorted shadow
uniform float uDistortionAmount;       // Distortion amount
uniform float uDistortionFrequency;     // Distortion frequency
uniform vec2 uDistortionDirection;      // Distortion direction
uniform bool uNoiseShadow;              // Noisy shadow
uniform float uNoiseAmount;            // Noise amount
uniform float uNoiseScale;             // Noise scale
uniform bool uGradientShadow;           // Gradient shadow
uniform vec4 uGradientStart;           // Gradient start color
uniform vec4 uGradientEnd;             // Gradient end color
uniform vec2 uGradientDirection;       // Gradient direction
uniform bool uShadowGlow;              // Shadow glow effect
uniform float uShadowGlowSize;         // Shadow glow size
uniform float uShadowGlowIntensity;    // Shadow glow intensity
uniform bool uShadowFade;              // Shadow fade with distance
uniform float uShadowFadeDistance;      // Shadow fade distance
uniform float uShadowFadeStrength;      // Shadow fade strength
uniform bool uShadowWobble;             // Shadow wobble effect
uniform float uWobbleAmount;            // Wobble amount
uniform float uWobbleFrequency;         // Wobble frequency
uniform bool uShadowPulse;              // Shadow pulse effect
uniform float uShadowPulseSpeed;        // Shadow pulse speed
uniform float uShadowPulseAmount;       // Shadow pulse amount

// Input from vertex shader
in vec2 vTexCoord;
in vec2 vScreenPos;
in vec4 vColor;
in float vAlpha;
in vec3 vWorldPos;
in vec3 vNormal;
in vec3 vViewDirection;
in float vDepth;

// Output color
out vec4 fragColor;

// Include common functions
#include "common/common.glsl"
#include "common/noise.glsl"

// Sample texture with antialiasing
float sampleTextureAA(sampler2D tex, vec2 uv)
{
    if (!uEnableAntialiasing) return texture(tex, uv).a;
    
    vec2 texelSize = 1.0 / textureSize(tex, 0);
    float alpha = 0.0;
    float totalWeight = 0.0;
    
    // Sample surrounding pixels for antialiasing
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float sample = texture(tex, uv + offset).a;
            float weight = 1.0 - length(vec2(float(x), float(y))) / 1.414;
            alpha += sample * weight;
            totalWeight += weight;
        }
    }
    
    return alpha / totalWeight;
}

// Basic shadow rendering
vec4 renderBasicShadow(vec2 uv, vec4 textColor, vec4 shadowColor, float offsetX, float offsetY, float blur)
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    vec2 shadowOffset = vec2(offsetX, offsetY) * texelSize;
    
    // Render shadow with blur
    vec4 shadowResult = vec4(0.0);
    if (blur > 0.0)
    {
        int blurRadius = int(blur);
        for (int x = -blurRadius; x <= blurRadius; x++)
        {
            for (int y = -blurRadius; y <= blurRadius; y++)
            {
                vec2 offset = vec2(float(x), float(y)) * texelSize;
                float shadowAlpha = sampleTextureAA(uTexture, uv + shadowOffset + offset);
                float weight = exp(-length(vec2(float(x), float(y))) / blur);
                shadowResult += vec4(shadowColor.rgb, shadowAlpha * shadowColor.a) * weight;
            }
        }
        shadowResult /= float((blurRadius * 2 + 1) * (blurRadius * 2 + 1));
    }
    else
    {
        float shadowAlpha = sampleTextureAA(uTexture, uv + shadowOffset);
        shadowResult = vec4(shadowColor.rgb, shadowAlpha * shadowColor.a);
    }
    
    // Render text
    float textAlpha = sampleTextureAA(uTexture, uv);
    vec4 textResult = vec4(textColor.rgb, textAlpha * textColor.a);
    
    // Blend text over shadow
    vec4 result = mix(shadowResult, textResult, textResult.a);
    
    return result;
}

// Perspective shadow
vec4 renderPerspectiveShadow(vec2 uv, vec4 textColor, vec4 shadowColor, float offsetX, float offsetY, float perspective, float blur)
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    
    // Calculate perspective offset based on UV position
    vec2 perspectiveOffset = vec2(offsetX, offsetY) * texelSize * (1.0 + uv.y * perspective);
    
    // Render shadow with perspective
    vec4 shadowResult = vec4(0.0);
    if (blur > 0.0)
    {
        int blurRadius = int(blur * (1.0 + uv.y * perspective));
        for (int x = -blurRadius; x <= blurRadius; x++)
        {
            for (int y = -blurRadius; y <= blurRadius; y++)
            {
                vec2 offset = vec2(float(x), float(y)) * texelSize;
                float shadowAlpha = sampleTextureAA(uTexture, uv + perspectiveOffset + offset);
                float weight = exp(-length(vec2(float(x), float(y))) / blur);
                shadowResult += vec4(shadowColor.rgb, shadowAlpha * shadowColor.a) * weight;
            }
        }
        shadowResult /= float((blurRadius * 2 + 1) * (blurRadius * 2 + 1));
    }
    else
    {
        float shadowAlpha = sampleTextureAA(uTexture, uv + perspectiveOffset);
        shadowResult = vec4(shadowColor.rgb, shadowAlpha * shadowColor.a);
    }
    
    // Render text
    float textAlpha = sampleTextureAA(uTexture, uv);
    vec4 textResult = vec4(textColor.rgb, textAlpha * textColor.a);
    
    // Blend text over shadow
    vec4 result = mix(shadowResult, textResult, textResult.a);
    
    return result;
}

// Radial shadow
vec4 renderRadialShadow(vec2 uv, vec4 textColor, vec4 shadowColor, float radius, float strength, float blur)
{
    vec2 center = vec2(0.5, 0.5);
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return vec4(textColor.rgb, sampleTextureAA(uTexture, uv) * textColor.a);
    
    direction = normalize(direction);
    
    // Calculate radial shadow
    vec4 shadowResult = vec4(0.0);
    int samples = int(radius);
    
    for (int i = 0; i <= samples; i++)
    {
        float t = float(i) / float(samples);
        float sampleDistance = t * radius;
        vec2 sampleUV = center + direction * sampleDistance;
        
        vec2 texelSize = 1.0 / textureSize(uTexture, 0);
        vec2 shadowOffset = direction * sampleDistance * texelSize;
        
        float shadowAlpha = sampleTextureAA(uTexture, sampleUV + shadowOffset);
        float weight = pow(1.0 - t, 2.0) * strength;
        shadowResult += vec4(shadowColor.rgb, shadowAlpha * shadowColor.a) * weight;
    }
    
    shadowResult /= float(samples + 1);
    
    // Render text
    float textAlpha = sampleTextureAA(uTexture, uv);
    vec4 textResult = vec4(textColor.rgb, textAlpha * textColor.a);
    
    // Blend text over shadow
    vec4 result = mix(shadowResult, textResult, textResult.a);
    
    return result;
}

// Multiple shadows
vec4 renderMultipleShadows(vec2 uv, vec4 textColor)
{
    vec4 result = vec4(textColor.rgb, sampleTextureAA(uTexture, uv) * textColor.a);
    
    for (int i = 0; i < uShadowCount && i < 5; i++)
    {
        vec4 shadowResult = renderBasicShadow(uv, result, uShadowColors[i], 
                                            uShadowOffsets[i].x, uShadowOffsets[i].y, 
                                            uShadowBlur * (1.0 + float(i) * 0.5));
        result = mix(shadowResult, result, result.a);
    }
    
    return result;
}

// Volumetric shadow
vec4 renderVolumetricShadow(vec2 uv, vec4 textColor, vec4 shadowColor, float offsetX, float offsetY, float density, int samples)
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    vec2 shadowOffset = vec2(offsetX, offsetY) * texelSize;
    
    vec4 volumetricResult = vec4(0.0);
    
    for (int i = 0; i < samples; i++)
    {
        float t = float(i) / float(samples - 1);
        vec2 sampleOffset = shadowOffset * t;
        float shadowAlpha = sampleTextureAA(uTexture, uv + sampleOffset);
        
        float weight = exp(-t * density);
        volumetricResult += vec4(shadowColor.rgb, shadowAlpha * shadowColor.a) * weight;
    }
    
    volumetricResult /= float(samples);
    
    // Render text
    float textAlpha = sampleTextureAA(uTexture, uv);
    vec4 textResult = vec4(textColor.rgb, textAlpha * textColor.a);
    
    // Blend text over volumetric shadow
    vec4 result = mix(volumetricResult, textResult, textResult.a);
    
    return result;
}

// Soft shadow
vec4 renderSoftShadow(vec2 uv, vec4 textColor, vec4 shadowColor, float offsetX, float offsetY, float radius, int samples)
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    vec2 shadowOffset = vec2(offsetX, offsetY) * texelSize;
    
    vec4 softResult = vec4(0.0);
    
    for (int i = 0; i < samples; i++)
    {
        float angle = float(i) / float(samples) * TWO_PI;
        vec2 sampleOffset = vec2(cos(angle), sin(angle)) * radius * texelSize;
        
        float shadowAlpha = sampleTextureAA(uTexture, uv + shadowOffset + sampleOffset);
        softResult += vec4(shadowColor.rgb, shadowAlpha * shadowColor.a);
    }
    
    softResult /= float(samples);
    
    // Render text
    float textAlpha = sampleTextureAA(uTexture, uv);
    vec4 textResult = vec4(textColor.rgb, textAlpha * textColor.a);
    
    // Blend text over soft shadow
    vec4 result = mix(softResult, textResult, textResult.a);
    
    return result;
}

// Contact shadow
vec4 renderContactShadow(vec2 uv, vec4 textColor, vec4 shadowColor, float distance, float fade)
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    
    // Sample surrounding pixels to find contact points
    float maxAlpha = 0.0;
    vec2 contactOffset = vec2(0.0);
    
    for (int x = -2; x <= 2; x++)
    {
        for (int y = -2; y <= 2; y++)
        {
            if (x == 0 && y == 0) continue;
            
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float sample = sampleTextureAA(uTexture, uv + offset);
            
            if (sample > maxAlpha)
            {
                maxAlpha = sample;
                contactOffset = offset;
            }
        }
    }
    
    // Calculate contact shadow
    float contactDistance = length(contactOffset);
    float contactShadow = smoothstep(distance, distance + fade, contactDistance);
    
    vec4 shadowResult = vec4(shadowColor.rgb, contactShadow * shadowColor.a);
    
    // Render text
    float textAlpha = sampleTextureAA(uTexture, uv);
    vec4 textResult = vec4(textColor.rgb, textAlpha * textColor.a);
    
    // Blend text over contact shadow
    vec4 result = mix(shadowResult, textResult, textResult.a);
    
    return result;
}

// Distorted shadow
vec4 renderDistortedShadow(vec2 uv, vec4 textColor, vec4 shadowColor, float offsetX, float offsetY, float time, float amount, float frequency)
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    vec2 shadowOffset = vec2(offsetX, offsetY) * texelSize;
    
    // Apply distortion to shadow offset
    float distortion = sin(length(uv - 0.5) * frequency + time) * amount;
    vec2 direction = normalize(uDistortionDirection);
    shadowOffset += direction * distortion * texelSize;
    
    // Render shadow
    float shadowAlpha = sampleTextureAA(uTexture, uv + shadowOffset);
    vec4 shadowResult = vec4(shadowColor.rgb, shadowAlpha * shadowColor.a);
    
    // Render text
    float textAlpha = sampleTextureAA(uTexture, uv);
    vec4 textResult = vec4(textColor.rgb, textAlpha * textColor.a);
    
    // Blend text over shadow
    vec4 result = mix(shadowResult, textResult, textResult.a);
    
    return result;
}

// Noisy shadow
vec4 renderNoisyShadow(vec2 uv, vec4 textColor, vec4 shadowColor, float offsetX, float offsetY, float time, float amount, float scale)
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    vec2 shadowOffset = vec2(offsetX, offsetY) * texelSize;
    
    // Apply noise to shadow offset
    vec2 noiseUV = uv * scale + time * 0.1;
    vec2 noiseOffset = vec2(noise2D(noiseUV), noise2D(noiseUV + 0.5)) * amount * texelSize;
    shadowOffset += noiseOffset;
    
    // Render shadow
    float shadowAlpha = sampleTextureAA(uTexture, uv + shadowOffset);
    vec4 shadowResult = vec4(shadowColor.rgb, shadowAlpha * shadowColor.a);
    
    // Render text
    float textAlpha = sampleTextureAA(uTexture, uv);
    vec4 textResult = vec4(textColor.rgb, textAlpha * textColor.a);
    
    // Blend text over shadow
    vec4 result = mix(shadowResult, textResult, textResult.a);
    
    return result;
}

// Gradient shadow
vec4 applyGradientShadow(vec4 baseColor, vec2 uv, vec4 startColor, vec4 endColor, vec2 direction)
{
    float t = dot(uv - 0.5, normalize(direction)) + 0.5;
    vec4 gradientColor = mix(startColor, endColor, saturate(t));
    
    return vec4(baseColor.rgb * gradientColor.rgb, baseColor.a);
}

// Shadow glow effect
vec4 applyShadowGlow(vec4 baseColor, vec2 uv, vec4 glowColor, float glowSize, float intensity)
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    
    // Sample surrounding pixels for glow
    vec4 glowResult = vec4(0.0);
    int glowRadius = int(glowSize);
    
    for (int x = -glowRadius; x <= glowRadius; x++)
    {
        for (int y = -glowRadius; y <= glowRadius; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float sample = sampleTextureAA(uTexture, uv + offset);
            float weight = exp(-length(vec2(float(x), float(y))) / glowSize);
            glowResult += vec4(glowColor.rgb, sample * glowColor.a) * weight * intensity;
        }
    }
    
    glowResult /= float((glowRadius * 2 + 1) * (glowRadius * 2 + 1));
    
    return mix(baseColor, glowResult, 0.3);
}

// Shadow fade with distance
float applyShadowFade(float alpha, float depth, float fadeDistance, float fadeStrength)
{
    if (!uShadowFade) return alpha;
    
    float fadeFactor = smoothstep(fadeDistance, fadeDistance + fadeStrength, depth);
    return alpha * fadeFactor;
}

// Shadow wobble effect
vec2 applyShadowWobble(vec2 offset, float time, float amount, float frequency)
{
    if (!uShadowWobble) return offset;
    
    float wobble = sin(time * frequency) * amount;
    return offset + vec2(wobble, wobble * 0.5);
}

// Shadow pulse effect
float applyShadowPulse(float alpha, float time, float speed, float amount)
{
    if (!uShadowPulse) return alpha;
    
    float pulse = sin(time * speed) * amount;
    return alpha * (1.0 + pulse);
}

// Animated effects
vec4 applyAnimation(vec4 baseColor, vec2 uv, float time)
{
    if (!uAnimated) return baseColor;
    
    vec4 animatedColor = baseColor;
    
    // Pulse animation
    if (uPulseAnimation)
    {
        float pulse = sin(time * uPulseSpeed + uAnimationPhase) * uPulseAmount;
        animatedColor.a *= (0.5 + pulse * 0.5);
        animatedColor.rgb *= (0.8 + pulse * 0.2);
    }
    
    // Wave animation
    if (uWaveAnimation)
    {
        float wave = sin(uv.x * uWaveFrequency + time * uAnimationSpeed) * uWaveAmplitude;
        animatedColor.rgb += vec3(wave * 0.1);
    }
    
    // Color shift animation
    if (uColorShift)
    {
        float hueShift = time * uColorShiftSpeed;
        vec3 hsv = rgbToHSV(animatedColor.rgb);
        hsv.x = mod(hsv.x + hueShift, 1.0);
        animatedColor.rgb = hsvToRGB(hsv);
    }
    
    // Flicker animation
    if (uFlickerAnimation)
    {
        float flicker = sin(time * uFlickerSpeed * 5.0) * uFlickerAmount;
        animatedColor.a *= (1.0 + flicker);
    }
    
    // Breathing animation
    if (uBreathingAnimation)
    {
        float breathe = sin(time * uBreathingSpeed) * uBreathingAmount;
        animatedColor.a *= (0.8 + breathe * 0.2);
    }
    
    return animatedColor;
}

// Subpixel rendering for shadow
vec4 renderShadowSubpixel(vec2 uv, vec4 color, vec3 weights, float offsetX, float offsetY)
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    
    // Sample subpixels
    float alphaR = sampleTextureAA(uTexture, uv + vec2(-texelSize.x * 0.33 + offsetX * texelSize.x, offsetY * texelSize.y));
    float alphaG = sampleTextureAA(uTexture, uv + vec2(offsetX * texelSize.x, offsetY * texelSize.y));
    float alphaB = sampleTextureAA(uTexture, uv + vec2(texelSize.x * 0.33 + offsetX * texelSize.x, offsetY * texelSize.y));
    
    // Combine with weights
    vec3 finalColor = vec3(alphaR, alphaG, alphaB) * weights;
    float finalAlpha = (alphaR + alphaG + alphaB) / 3.0;
    
    return vec4(color.rgb * finalColor, finalAlpha * color.a);
}

// Debug visualization
vec4 renderDebug(vec2 uv, int channel)
{
    float textAlpha = sampleTextureAA(uTexture, uv);
    float shadowAlpha = sampleTextureAA(uTexture, uv + vec2(uShadowOffsetX, uShadowOffsetY) / textureSize(uTexture, 0));
    
    switch (channel)
    {
        case 0: // Text only
            return vec4(uTextColor.rgb, textAlpha * uTextColor.a);
        case 1: // Shadow only
            return vec4(uShadowColor.rgb, shadowAlpha * uShadowColor.a);
        case 2: // Combined
            vec4 text = vec4(uTextColor.rgb, textAlpha * uTextColor.a);
            vec4 shadow = vec4(uShadowColor.rgb, shadowAlpha * uShadowColor.a);
            return mix(shadow, text, text.a);
        default:
            return vec4(1.0, 0.0, 1.0, 1.0); // Magenta for error
    }
}

// Dithering function
float dither(vec2 uv, float strength)
{
    // Bayer matrix dithering
    const float bayer4x4[16] = float[](
        0.0,  8.0,  2.0, 10.0,
        12.0, 4.0, 14.0,  6.0,
        3.0, 11.0,  1.0,  9.0,
        15.0, 7.0, 13.0,  5.0
    );
    
    vec2 pixel = floor(uv * uResolution);
    int index = int(mod(pixel.x, 4.0)) + int(mod(pixel.y, 4.0)) * 4;
    
    return (bayer4x4[index] / 16.0 - 0.5) * strength;
}

// Main rendering function
vec4 renderFontShadow(vec2 uv)
{
    vec4 result;
    
    // Debug mode
    if (uDebugMode)
    {
        return renderDebug(uv, uDebugChannel);
    }
    
    // Calculate shadow offset with wobble
    vec2 shadowOffset = vec2(uShadowOffsetX, uShadowOffsetY);
    shadowOffset = applyShadowWobble(shadowOffset, uTime, uWobbleAmount, uWobbleFrequency);
    
    // Render shadow based on type
    if (uMultipleShadows)
    {
        result = renderMultipleShadows(uv, uTextColor);
    }
    else if (uVolumetricShadow)
    {
        result = renderVolumetricShadow(uv, uTextColor, uShadowColor, shadowOffset.x, shadowOffset.y, 
                                       uVolumetricDensity, int(uVolumetricSamples));
    }
    else if (uSoftShadow)
    {
        result = renderSoftShadow(uv, uTextColor, uShadowColor, shadowOffset.x, shadowOffset.y,
                                  uSoftShadowRadius, int(uSoftShadowSamples));
    }
    else if (uContactShadow)
    {
        result = renderContactShadow(uv, uTextColor, uShadowColor, uContactDistance, uContactFade);
    }
    else if (uPerspectiveShadow)
    {
        result = renderPerspectiveShadow(uv, uTextColor, uShadowColor, shadowOffset.x, shadowOffset.y,
                                         uPerspectiveAmount, uShadowBlur);
    }
    else if (uRadialShadow)
    {
        result = renderRadialShadow(uv, uTextColor, uShadowColor, uRadialRadius, uRadialStrength, uShadowBlur);
    }
    else if (uDistortedShadow)
    {
        result = renderDistortedShadow(uv, uTextColor, uShadowColor, shadowOffset.x, shadowOffset.y,
                                      uTime, uDistortionAmount, uDistortionFrequency);
    }
    else if (uNoiseShadow)
    {
        result = renderNoisyShadow(uv, uTextColor, uShadowColor, shadowOffset.x, shadowOffset.y,
                                  uTime, uNoiseAmount, uNoiseScale);
    }
    else
    {
        result = renderBasicShadow(uv, uTextColor, uShadowColor, shadowOffset.x, shadowOffset.y, uShadowBlur);
    }
    
    // Apply gradient to shadow
    if (uGradientShadow)
    {
        result = applyGradientShadow(result, uv, uGradientStart, uGradientEnd, uGradientDirection);
    }
    else if (uEnableGradient && uGradientTexture != sampler2D(0))
    {
        vec4 gradientColor = texture(uGradientTexture, uv);
        result.rgb *= gradientColor.rgb;
    }
    
    // Apply shadow glow
    if (uShadowGlow)
    {
        result = applyShadowGlow(result, uv, uGlowColor, uShadowGlowSize, uShadowGlowIntensity);
    }
    
    // Apply shadow fade
    result.a = applyShadowFade(result.a, vDepth, uShadowFadeDistance, uShadowFadeStrength);
    
    // Apply shadow pulse
    result.a = applyShadowPulse(result.a, uTime, uShadowPulseSpeed, uShadowPulseAmount);
    
    // Apply vertex color
    result *= vColor;
    
    // Apply vertex alpha
    result.a *= vAlpha;
    
    // Apply shadow intensity
    result.a *= uShadowIntensity;
    
    // Subpixel rendering
    if (uSubpixelRendering)
    {
        result = renderShadowSubpixel(uv, result, uSubpixelWeights, shadowOffset.x, shadowOffset.y);
    }
    
    // Apply animation
    result = applyAnimation(result, uv, uTime);
    
    // Apply dithering
    if (uDithering)
    {
        float ditherValue = dither(vScreenPos / uResolution, uDitherStrength);
        result.rgb += vec3(ditherValue) / 255.0;
    }
    
    // Apply gamma correction
    if (uGammaCorrect)
    {
        result.rgb = pow(result.rgb, vec3(1.0 / uGamma));
    }
    
    return result;
}

// Main function
void main()
{
    vec4 color = renderFontShadow(vTexCoord);
    
    // Ensure color is in valid range
    color = saturate(color);
    
    fragColor = color;
}