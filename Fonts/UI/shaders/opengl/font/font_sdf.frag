#version 330 core

// Signed Distance Field (SDF) Font Fragment Shader
// High-quality font rendering using SDF technique

// Input textures
uniform sampler2D uTexture;           // SDF texture (single channel distance field)
uniform sampler2D uGradientTexture;    // Optional gradient texture
uniform sampler2D uNoiseTexture;       // Noise texture for animated effects
uniform vec2 uResolution;             // Screen resolution
uniform vec4 uTextColor;              // Font color (RGBA)
uniform vec4 uOutlineColor;           // Outline color
uniform vec4 uShadowColor;            // Shadow color
uniform vec4 uGlowColor;              // Glow color
uniform vec4 uBackgroundColor;         // Background color
uniform float uPixelRange;             // Pixel range for SDF
uniform float uAlphaThreshold;        // Alpha threshold for rendering
uniform float uOutlineWidth;           // Outline width
uniform float uOutlineDistance;        // Outline distance threshold
uniform float uOutlineSoftness;        // Outline edge softness
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
uniform int uDebugChannel;              // Debug channel (0=SDF, 1=text, 2=outline)
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
uniform bool uDistortion;              // Enable distortion
uniform float uDistortionAmount;       // Distortion amount
uniform float uDistortionFrequency;     // Distortion frequency
uniform vec2 uDistortionDirection;      // Distortion direction
uniform bool uSoftEdges;               // Soft edges
uniform float uSoftEdgeAmount;          // Soft edge amount
uniform bool uRoundedCorners;           // Rounded corners
uniform float uCornerRadius;            // Corner radius
uniform bool uInnerGlow;               // Inner glow effect
uniform float uInnerGlowSize;           // Inner glow size
uniform float uInnerGlowIntensity;      // Inner glow intensity
uniform bool uOuterGlow;               // Outer glow effect
uniform float uOuterGlowSize;           // Outer glow size
uniform float uOuterGlowIntensity;      // Outer glow intensity
uniform bool uGradientMode;            // Gradient mode (0=linear, 1=radial, 2=angular)
uniform vec2 uGradientStart;            // Gradient start point
uniform vec2 uGradientEnd;              // Gradient end point
uniform bool uTextureGradient;          // Use texture for gradient
uniform bool uNoiseEffect;              // Noise effect
uniform float uNoiseScale;             // Noise scale
uniform float uNoiseAmount;            // Noise amount
uniform bool uFlickerEffect;           // Flicker effect
uniform float uFlickerSpeed;            // Flicker speed
uniform float uFlickerAmount;          // Flicker amount

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

// SDF sample and process
float sampleSDF(vec2 uv, float pixelRange)
{
    float distance = texture(uTexture, uv).r;
    
    // Convert from [0,1] to signed distance
    distance = distance * 2.0 - 1.0;
    
    // Apply pixel range transformation
    distance *= pixelRange;
    
    return distance;
}

// Antialiased step function
float aaStep(float threshold, float value)
{
    float afwidth = length(vec2(dFdx(value), dFdy(value))) * uAntialiasingStrength;
    return smoothstep(threshold - afwidth, threshold + afwidth, value);
}

// Enhanced SDF rendering with antialiasing
vec4 renderSDF(vec2 uv, vec4 color, float pixelRange)
{
    // Sample SDF
    float distance = sampleSDF(uv, pixelRange);
    
    // Apply antialiasing
    float alpha = aaStep(uAlphaThreshold, distance);
    
    // Apply color
    vec4 result = vec4(color.rgb, alpha * color.a);
    
    return result;
}

// Outline effect for SDF
vec4 renderSDFOutline(vec2 uv, vec4 textColor, vec4 outlineColor, float outlineWidth, float pixelRange)
{
    // Sample SDF
    float distance = sampleSDF(uv, pixelRange);
    
    // Calculate outline alpha
    float outlineAlpha = aaStep(uAlphaThreshold - outlineWidth, distance);
    float textAlpha = aaStep(uAlphaThreshold, distance);
    
    // Combine outline and text
    vec4 outline = vec4(outlineColor.rgb, outlineAlpha * outlineColor.a);
    vec4 text = vec4(textColor.rgb, textAlpha * textColor.a);
    
    // Blend text over outline
    vec4 result = mix(outline, text, text.a);
    
    return result;
}

// Shadow effect for SDF
vec4 renderSDFShadow(vec2 uv, vec4 textColor, vec4 shadowColor, float shadowX, float shadowY, float blur, float pixelRange)
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
                float shadowDistance = sampleSDF(uv + shadowOffset + offset, pixelRange);
                float shadowAlpha = aaStep(uAlphaThreshold, shadowDistance);
                float weight = exp(-length(vec2(float(x), float(y))) / blur);
                shadowResult += vec4(shadowColor.rgb, shadowAlpha * shadowColor.a) * weight;
            }
        }
        shadowResult /= float((blurRadius * 2 + 1) * (blurRadius * 2 + 1));
    }
    else
    {
        float shadowDistance = sampleSDF(uv + shadowOffset, pixelRange);
        float shadowAlpha = aaStep(uAlphaThreshold, shadowDistance);
        shadowResult = vec4(shadowColor.rgb, shadowAlpha * shadowColor.a);
    }
    
    // Render text
    vec4 textResult = renderSDF(uv, textColor, pixelRange);
    
    // Blend text over shadow
    vec4 result = mix(shadowResult, textResult, textResult.a);
    
    return result;
}

// Glow effect for SDF
vec4 renderSDFGlow(vec2 uv, vec4 textColor, vec4 glowColor, float glowSize, float glowIntensity, float pixelRange)
{
    // Sample SDF
    float distance = sampleSDF(uv, pixelRange);
    
    // Calculate glow based on distance
    float glowAmount = smoothstep(uAlphaThreshold + glowSize, uAlphaThreshold, distance);
    glowAmount *= glowIntensity;
    
    // Calculate text alpha
    float textAlpha = aaStep(uAlphaThreshold, distance);
    
    // Combine glow and text
    vec4 glow = vec4(glowColor.rgb, glowAmount * glowColor.a);
    vec4 text = vec4(textColor.rgb, textAlpha * textColor.a);
    
    // Blend text over glow
    vec4 result = mix(glow, text, text.a);
    
    return result;
}

// Inner glow effect
vec4 renderSDFInnerGlow(vec2 uv, vec4 textColor, vec4 glowColor, float glowSize, float glowIntensity, float pixelRange)
{
    // Sample SDF
    float distance = sampleSDF(uv, pixelRange);
    
    // Calculate inner glow based on distance
    float glowAmount = smoothstep(uAlphaThreshold - glowSize, uAlphaThreshold, distance);
    glowAmount *= glowIntensity;
    
    // Calculate text alpha
    float textAlpha = aaStep(uAlphaThreshold, distance);
    
    // Apply inner glow only inside text
    vec4 glow = vec4(glowColor.rgb, glowAmount * glowColor.a * textAlpha);
    vec4 text = vec4(textColor.rgb, textAlpha * textColor.a);
    
    // Blend glow with text
    vec4 result = mix(text, glow, glow.a);
    
    return result;
}

// Outer glow effect
vec4 renderSDFOuterGlow(vec2 uv, vec4 textColor, vec4 glowColor, float glowSize, float glowIntensity, float pixelRange)
{
    // Sample SDF
    float distance = sampleSDF(uv, pixelRange);
    
    // Calculate outer glow based on distance
    float glowAmount = smoothstep(uAlphaThreshold, uAlphaThreshold + glowSize, distance);
    glowAmount *= glowIntensity;
    
    // Calculate text alpha
    float textAlpha = aaStep(uAlphaThreshold, distance);
    
    // Apply outer glow only outside text
    vec4 glow = vec4(glowColor.rgb, glowAmount * glowColor.a * (1.0 - textAlpha));
    vec4 text = vec4(textColor.rgb, textAlpha * textColor.a);
    
    // Blend text over glow
    vec4 result = mix(glow, text, text.a);
    
    return result;
}

// Gradient effect for SDF
vec4 renderSDFGradient(vec2 uv, vec4 baseColor, sampler2D gradientTex, float pixelRange, int mode)
{
    // Sample SDF
    float distance = sampleSDF(uv, pixelRange);
    float alpha = aaStep(uAlphaThreshold, distance);
    
    // Calculate gradient based on mode
    vec4 gradientColor;
    if (uTextureGradient && gradientTex != sampler2D(0))
    {
        gradientColor = texture(gradientTex, uv);
    }
    else
    {
        float t = 0.0;
        
        switch (mode)
        {
            case 0: // Linear gradient
                t = dot(uv - uGradientStart, normalize(uGradientEnd - uGradientStart)) / length(uGradientEnd - uGradientStart);
                break;
            case 1: // Radial gradient
                t = length(uv - uGradientStart) / length(uGradientEnd - uGradientStart);
                break;
            case 2: // Angular gradient
                vec2 direction = uv - uGradientStart;
                t = atan(direction.y, direction.x) / TWO_PI + 0.5;
                break;
        }
        
        gradientColor = mix(uGradientStart, uGradientEnd, saturate(t));
    }
    
    // Apply gradient to text
    vec4 result = vec4(baseColor.rgb * gradientColor.rgb, alpha * baseColor.a);
    
    return result;
}

// Subpixel rendering for SDF
vec4 renderSDFSubpixel(vec2 uv, vec4 color, vec3 weights, float pixelRange)
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    
    // Sample subpixels
    float distR = sampleSDF(uv + vec2(-texelSize.x * 0.33, 0.0), pixelRange);
    float distG = sampleSDF(uv, pixelRange);
    float distB = sampleSDF(uv + vec2(texelSize.x * 0.33, 0.0), pixelRange);
    
    // Apply antialiasing
    float alphaR = aaStep(uAlphaThreshold, distR);
    float alphaG = aaStep(uAlphaThreshold, distG);
    float alphaB = aaStep(uAlphaThreshold, distB);
    
    // Combine with weights
    vec3 finalColor = vec3(alphaR, alphaG, alphaB) * weights;
    float finalAlpha = (alphaR + alphaG + alphaB) / 3.0;
    
    return vec4(color.rgb * finalColor, finalAlpha * color.a);
}

// Soft edges effect
float applySoftEdges(float alpha, vec2 uv, float amount)
{
    if (!uSoftEdges) return alpha;
    
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    float edgeDistance = length(vec2(dFdx(alpha), dFdy(alpha))) * amount;
    
    return smoothstep(0.0, edgeDistance, alpha);
}

// Rounded corners effect
float applyRoundedCorners(float alpha, vec2 uv, float cornerRadius)
{
    if (!uRoundedCorners) return alpha;
    
    vec2 corner = vec2(0.5, 0.5);
    float distanceToCorner = length(uv - corner);
    
    if (distanceToCorner > cornerRadius)
    {
        float cornerAlpha = 1.0 - smoothstep(cornerRadius - 0.01, cornerRadius + 0.01, distanceToCorner);
        return alpha * cornerAlpha;
    }
    
    return alpha;
}

// Noise effect
float applyNoiseEffect(float alpha, vec2 uv, float time, float scale, float amount)
{
    if (!uNoiseEffect) return alpha;
    
    vec2 noiseUV = uv * scale + time * 0.1;
    float noise = noise2D(noiseUV) * amount;
    
    return alpha * (1.0 + noise);
}

// Flicker effect
float applyFlickerEffect(float alpha, float time, float speed, float amount)
{
    if (!uFlickerEffect) return alpha;
    
    float flicker = sin(time * speed) * amount;
    return alpha * (1.0 + flicker);
}

// Distortion effect
vec2 applyDistortion(vec2 uv, float time, float amount, float frequency)
{
    if (!uDistortion) return uv;
    
    float distortion = sin(length(uv - 0.5) * frequency + time) * amount;
    vec2 direction = normalize(uv - 0.5);
    
    return uv + direction * distortion;
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
    float distance = sampleSDF(uv, uPixelRange);
    float alpha = aaStep(uAlphaThreshold, distance);
    
    switch (channel)
    {
        case 0: // SDF distance field
            vec3 sdfColor = vec3(distance * 0.5 + 0.5);
            return vec4(sdfColor, 1.0);
        case 1: // Text only
            return vec4(uTextColor.rgb, alpha * uTextColor.a);
        case 2: // Outline only
            float outlineAlpha = aaStep(uAlphaThreshold - uOutlineWidth, distance);
            return vec4(uOutlineColor.rgb, outlineAlpha * uOutlineColor.a);
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
vec4 renderFontSDF(vec2 uv)
{
    // Calculate pixel range based on screen resolution
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    float pixelRange = uPixelRange * length(texelSize * uResolution);
    
    // Apply distortion if enabled
    vec2 distortedUV = applyDistortion(uv, uTime, uDistortionAmount, uDistortionFrequency);
    
    vec4 result;
    
    // Debug mode
    if (uDebugMode)
    {
        return renderDebug(distortedUV, uDebugChannel);
    }
    
    // Subpixel rendering
    if (uSubpixelRendering)
    {
        result = renderSDFSubpixel(distortedUV, uTextColor, uSubpixelWeights, pixelRange);
    }
    else
    {
        // Standard SDF rendering
        result = renderSDF(distortedUV, uTextColor, pixelRange);
    }
    
    // Apply outline
    if (uEnableOutline)
    {
        result = renderSDFOutline(distortedUV, result, uOutlineColor, uOutlineWidth, pixelRange);
    }
    
    // Apply shadow
    if (uEnableShadow)
    {
        result = renderSDFShadow(distortedUV, result, uShadowColor, uShadowOffsetX, uShadowOffsetY, uShadowBlur, pixelRange);
    }
    
    // Apply glow effects
    if (uEnableGlow)
    {
        if (uInnerGlow)
        {
            result = renderSDFInnerGlow(distortedUV, result, uGlowColor, uInnerGlowSize, uInnerGlowIntensity, pixelRange);
        }
        else if (uOuterGlow)
        {
            result = renderSDFOuterGlow(distortedUV, result, uGlowColor, uOuterGlowSize, uOuterGlowIntensity, pixelRange);
        }
        else
        {
            result = renderSDFGlow(distortedUV, result, uGlowColor, uGlowSize, uGlowIntensity, pixelRange);
        }
    }
    
    // Apply gradient
    if (uEnableGradient)
    {
        result = renderSDFGradient(distortedUV, result, uGradientTexture, pixelRange, uGradientMode);
    }
    
    // Apply vertex color
    result *= vColor;
    
    // Apply vertex alpha
    result.a *= vAlpha;
    
    // Apply soft edges
    result.a = applySoftEdges(result.a, distortedUV, uSoftEdgeAmount);
    
    // Apply rounded corners
    result.a = applyRoundedCorners(result.a, distortedUV, uCornerRadius);
    
    // Apply noise effect
    result.a = applyNoiseEffect(result.a, distortedUV, uTime, uNoiseScale, uNoiseAmount);
    
    // Apply flicker effect
    result.a = applyFlickerEffect(result.a, uTime, uFlickerSpeed, uFlickerAmount);
    
    // Apply animation
    result = applyAnimation(result, distortedUV, uTime);
    
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
    vec4 color = renderFontSDF(vTexCoord);
    
    // Ensure color is in valid range
    color = saturate(color);
    
    fragColor = color;
}