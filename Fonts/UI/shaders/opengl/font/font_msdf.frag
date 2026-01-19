#version 330 core

// Multi-channel Signed Distance Field (MSDF) Font Fragment Shader
// High-quality font rendering using MSDF technique

// Input textures
uniform sampler2D uTexture;           // MSDF texture (RGB channels for distance field)
uniform sampler2D uGradientTexture;    // Optional gradient texture
uniform vec2 uResolution;             // Screen resolution
uniform vec4 uTextColor;              // Font color (RGBA)
uniform vec4 uOutlineColor;           // Outline color
uniform vec4 uShadowColor;            // Shadow color
uniform vec4 uGlowColor;              // Glow color
uniform vec4 uBackgroundColor;         // Background color
uniform float uPixelRange;             // Pixel range for MSDF
uniform float uAlphaThreshold;        // Alpha threshold
uniform float uOutlineWidth;           // Outline width
uniform float uOutlineDistance;        // Outline distance threshold
uniform float uShadowOffsetX;          // Shadow X offset
uniform float uShadowOffsetY;          // Shadow Y offset
uniform float uShadowBlur;             // Shadow blur amount
uniform float uGlowIntensity;          // Glow intensity
uniform float uGlowSize;               // Glow size
uniform bool uEnableOutline;            // Enable outline effect
uniform bool uEnableShadow;             // Enable shadow effect
uniform bool uEnableGlow;               // Enable glow effect
uniform bool uEnableGradient;          // Enable gradient effect
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
uniform int uDebugChannel;              // Debug channel (0=RGB, 1=R, 2=G, 3=B)
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

// Input from vertex shader
in vec2 vTexCoord;
in vec2 vScreenPos;
in vec4 vColor;
in float vAlpha;
in vec3 vWorldPos;
in vec3 vNormal;

// Output color
out vec4 fragColor;

// Include common functions
#include "common/common.glsl"
#include "common/noise.glsl"

// MSDF median calculation
float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

// MSDF sample and process
float sampleMSDF(vec2 uv, float pixelRange)
{
    vec3 msdf = texture(uTexture, uv).rgb;
    
    // Apply pixel range transformation
    float signedDistance = median(msdf.r, msdf.g, msdf.b) - 0.5;
    signedDistance *= pixelRange;
    
    return signedDistance;
}

// Antialiased step function
float aaStep(float threshold, float value)
{
    float afwidth = length(vec2(dFdx(value), dFdy(value))) * uAntialiasingStrength;
    return smoothstep(threshold - afwidth, threshold + afwidth, value);
}

// Enhanced MSDF rendering with antialiasing
vec4 renderMSDF(vec2 uv, vec4 color, float pixelRange)
{
    // Sample MSDF
    float signedDistance = sampleMSDF(uv, pixelRange);
    
    // Apply antialiasing
    float alpha = aaStep(uAlphaThreshold, signedDistance);
    
    // Apply color
    vec4 result = vec4(color.rgb, alpha * color.a);
    
    return result;
}

// Outline effect for MSDF
vec4 renderMSDFOutline(vec2 uv, vec4 textColor, vec4 outlineColor, float outlineWidth, float pixelRange)
{
    // Sample MSDF
    float signedDistance = sampleMSDF(uv, pixelRange);
    
    // Calculate outline alpha
    float outlineAlpha = aaStep(uAlphaThreshold - outlineWidth, signedDistance);
    float textAlpha = aaStep(uAlphaThreshold, signedDistance);
    
    // Combine outline and text
    vec4 outline = vec4(outlineColor.rgb, outlineAlpha * outlineColor.a);
    vec4 text = vec4(textColor.rgb, textAlpha * textColor.a);
    
    // Blend text over outline
    vec4 result = mix(outline, text, text.a);
    
    return result;
}

// Shadow effect for MSDF
vec4 renderMSDFShadow(vec2 uv, vec4 textColor, vec4 shadowColor, float shadowX, float shadowY, float blur, float pixelRange)
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    vec2 shadowOffset = vec2(shadowX, shadowY) * texelSize;
    
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
                float shadowAlpha = aaStep(uAlphaThreshold, sampleMSDF(uv + shadowOffset + offset, pixelRange));
                float weight = exp(-length(vec2(float(x), float(y))) / blur);
                shadowResult += vec4(shadowColor.rgb, shadowAlpha * shadowColor.a) * weight;
            }
        }
        shadowResult /= float((blurRadius * 2 + 1) * (blurRadius * 2 + 1));
    }
    else
    {
        float shadowAlpha = aaStep(uAlphaThreshold, sampleMSDF(uv + shadowOffset, pixelRange));
        shadowResult = vec4(shadowColor.rgb, shadowAlpha * shadowColor.a);
    }
    
    // Render text
    vec4 textResult = renderMSDF(uv, textColor, pixelRange);
    
    // Blend text over shadow
    vec4 result = mix(shadowResult, textResult, textResult.a);
    
    return result;
}

// Glow effect for MSDF
vec4 renderMSDFGlow(vec2 uv, vec4 textColor, vec4 glowColor, float glowSize, float glowIntensity, float pixelRange)
{
    // Sample MSDF
    float signedDistance = sampleMSDF(uv, pixelRange);
    
    // Calculate glow based on distance
    float glowAmount = smoothstep(uAlphaThreshold + glowSize, uAlphaThreshold, signedDistance);
    glowAmount *= glowIntensity;
    
    // Calculate text alpha
    float textAlpha = aaStep(uAlphaThreshold, signedDistance);
    
    // Combine glow and text
    vec4 glow = vec4(glowColor.rgb, glowAmount * glowColor.a);
    vec4 text = vec4(textColor.rgb, textAlpha * textColor.a);
    
    // Blend text over glow
    vec4 result = mix(glow, text, text.a);
    
    return result;
}

// Gradient effect for MSDF
vec4 renderMSDFGradient(vec2 uv, vec4 baseColor, sampler2D gradientTex, float pixelRange)
{
    // Sample MSDF
    float signedDistance = sampleMSDF(uv, pixelRange);
    float alpha = aaStep(uAlphaThreshold, signedDistance);
    
    // Sample gradient
    vec4 gradientColor = texture(gradientTex, uv);
    
    // Apply gradient to text
    vec4 result = vec4(baseColor.rgb * gradientColor.rgb, alpha * baseColor.a);
    
    return result;
}

// Subpixel rendering for MSDF
vec4 renderMSDFSubpixel(vec2 uv, vec4 color, vec3 weights, float pixelRange)
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    
    // Sample subpixels
    vec3 msdfR = texture(uTexture, uv + vec2(-texelSize.x * 0.33, 0.0)).rgb;
    vec3 msdfG = texture(uTexture, uv).rgb;
    vec3 msdfB = texture(uTexture, uv + vec2(texelSize.x * 0.33, 0.0)).rgb;
    
    // Calculate distances for each subpixel
    float distR = median(msdfR.r, msdfR.g, msdfR.b) - 0.5;
    float distG = median(msdfG.r, msdfG.g, msdfG.b) - 0.5;
    float distB = median(msdfB.r, msdfB.g, msdfB.b) - 0.5;
    
    // Apply pixel range
    distR *= pixelRange;
    distG *= pixelRange;
    distB *= pixelRange;
    
    // Apply antialiasing
    float alphaR = aaStep(uAlphaThreshold, distR);
    float alphaG = aaStep(uAlphaThreshold, distG);
    float alphaB = aaStep(uAlphaThreshold, distB);
    
    // Combine with weights
    vec3 finalColor = vec3(alphaR, alphaG, alphaB) * weights;
    float finalAlpha = (alphaR + alphaG + alphaB) / 3.0;
    
    return vec4(color.rgb * finalColor, finalAlpha * color.a);
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
    
    return animatedColor;
}

// Debug visualization
vec4 renderDebug(vec2 uv, int channel)
{
    vec3 msdf = texture(uTexture, uv).rgb;
    
    switch (channel)
    {
        case 0: // RGB combined
            return vec4(msdf, 1.0);
        case 1: // Red channel
            return vec4(vec3(msdf.r), 1.0);
        case 2: // Green channel
            return vec4(vec3(msdf.g), 1.0);
        case 3: // Blue channel
            return vec4(vec3(msdf.b), 1.0);
        default:
            return vec4(msdf, 1.0);
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
vec4 renderFontMSDF(vec2 uv)
{
    // Calculate pixel range based on screen resolution
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    float pixelRange = uPixelRange * length(texelSize * uResolution);
    
    vec4 result;
    
    // Debug mode
    if (uDebugMode)
    {
        return renderDebug(uv, uDebugChannel);
    }
    
    // Subpixel rendering
    if (uSubpixelRendering)
    {
        result = renderMSDFSubpixel(uv, uTextColor, uSubpixelWeights, pixelRange);
    }
    else
    {
        // Standard MSDF rendering
        result = renderMSDF(uv, uTextColor, pixelRange);
    }
    
    // Apply outline
    if (uEnableOutline)
    {
        result = renderMSDFOutline(uv, result, uOutlineColor, uOutlineWidth, pixelRange);
    }
    
    // Apply shadow
    if (uEnableShadow)
    {
        result = renderMSDFShadow(uv, result, uShadowColor, uShadowOffsetX, uShadowOffsetY, uShadowBlur, pixelRange);
    }
    
    // Apply glow
    if (uEnableGlow)
    {
        result = renderMSDFGlow(uv, result, uGlowColor, uGlowSize, uGlowIntensity, pixelRange);
    }
    
    // Apply gradient
    if (uEnableGradient && uGradientTexture != sampler2D(0))
    {
        result = renderMSDFGradient(uv, result, uGradientTexture, pixelRange);
    }
    
    // Apply vertex color
    result *= vColor;
    
    // Apply vertex alpha
    result.a *= vAlpha;
    
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
    vec4 color = renderFontMSDF(vTexCoord);
    
    // Ensure color is in valid range
    color = saturate(color);
    
    fragColor = color;
}