#version 330 core

// Font Outline Fragment Shader for OpenGL
// Specialized shader for rendering font outlines with various effects

// Input textures
uniform sampler2D uTexture;           // Font texture (alpha channel)
uniform sampler2D uOutlineTexture;    // Separate outline texture (optional)
uniform sampler2D uGradientTexture;    // Gradient texture for outline
uniform sampler2D uNoiseTexture;       // Noise texture for animated outlines
uniform vec2 uResolution;             // Screen resolution
uniform vec4 uTextColor;              // Font color (RGBA)
uniform vec4 uOutlineColor;           // Outline color
uniform vec4 uShadowColor;            // Shadow color
uniform vec4 uGlowColor;              // Glow color
uniform float uOutlineWidth;           // Outline width in pixels
uniform float uOutlineDistance;        // Outline distance threshold
uniform float uOutlineSoftness;        // Outline edge softness
uniform float uOutlineIntensity;       // Outline intensity
uniform bool uEnableOutline;            // Enable outline effect
uniform bool uEnableShadow;             // Enable shadow effect
uniform bool uEnableGlow;               // Enable glow effect
uniform bool uEnableGradient;          // Enable gradient on outline
uniform bool uEnableTextureOutline;     // Use separate texture for outline
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
uniform int uDebugChannel;              // Debug channel (0=text, 1=outline, 2=combined)
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
uniform bool uGlowAnimation;           // Glow animation
uniform float uGlowPulseSpeed;         // Glow pulse speed
uniform float uGlowPulseAmount;        // Glow pulse amount
uniform bool uOutlineGlow;             // Glow on outline only
uniform bool uDoubleOutline;           // Double outline effect
uniform vec4 uOuterOutlineColor;       // Outer outline color
uniform float uOuterOutlineWidth;      // Outer outline width
uniform bool uRoundedOutline;          // Rounded outline corners
uniform float uRoundedness;            // Roundness factor
uniform bool uBeveledOutline;          // Beveled outline effect
uniform vec3 uBevelDirection;          // Bevel light direction
uniform float uBevelAmount;            // Bevel amount
uniform bool uDashedOutline;           // Dashed outline
uniform float uDashLength;              // Dash length
uniform float uDashGap;                // Dash gap
uniform float uDashOffset;             // Dash offset
uniform bool uDottedOutline;           // Dotted outline
uniform float uDotSize;                // Dot size
uniform float uDotSpacing;             // Dot spacing
uniform bool uGradientOutline;         // Gradient outline
uniform vec4 uGradientStart;           // Gradient start color
uniform vec4 uGradientEnd;             // Gradient end color
uniform vec2 uGradientDirection;       // Gradient direction
uniform bool uNoiseOutline;            // Noisy outline
uniform float uNoiseAmount;            // Noise amount
uniform float uNoiseScale;             // Noise scale
uniform bool uDistortedOutline;        // Distorted outline
uniform float uDistortionAmount;       // Distortion amount
uniform float uDistortionFrequency;     // Distortion frequency

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

// Calculate outline alpha
float calculateOutlineAlpha(float textAlpha, vec2 uv, float width, float softness)
{
    if (uEnableTextureOutline && uOutlineTexture != sampler2D(0))
    {
        // Use separate outline texture
        float outlineAlpha = sampleTextureAA(uOutlineTexture, uv);
        return smoothstep(uOutlineDistance - softness, uOutlineDistance + softness, outlineAlpha);
    }
    else
    {
        // Calculate outline from text texture
        vec2 texelSize = 1.0 / textureSize(uTexture, 0);
        float maxAlpha = 0.0;
        
        // Sample surrounding pixels to find outline
        int samples = int(width);
        for (int x = -samples; x <= samples; x++)
        {
            for (int y = -samples; y <= samples; y++)
            {
                if (x == 0 && y == 0) continue;
                
                vec2 offset = vec2(float(x), float(y)) * texelSize;
                float sample = sampleTextureAA(uTexture, uv + offset);
                maxAlpha = max(maxAlpha, sample);
            }
        }
        
        // Create outline where there's no text but surrounding pixels exist
        float outlineAlpha = maxAlpha * (1.0 - textAlpha);
        return smoothstep(uOutlineDistance - softness, uOutlineDistance + softness, outlineAlpha);
    }
}

// Rounded outline calculation
float calculateRoundedOutline(float textAlpha, vec2 uv, float width, float roundness)
{
    if (!uRoundedOutline) return calculateOutlineAlpha(textAlpha, uv, width, uOutlineSoftness);
    
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    float outlineAlpha = 0.0;
    float totalWeight = 0.0;
    
    int samples = int(width);
    for (int x = -samples; x <= samples; x++)
    {
        for (int y = -samples; y <= samples; y++)
        {
            if (x == 0 && y == 0) continue;
            
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float distance = length(vec2(float(x), float(y)));
            float sample = sampleTextureAA(uTexture, uv + offset);
            
            // Apply roundness weighting
            float weight = exp(-distance / roundness);
            outlineAlpha += sample * weight;
            totalWeight += weight;
        }
    }
    
    outlineAlpha = (outlineAlpha / totalWeight) * (1.0 - textAlpha);
    return smoothstep(uOutlineDistance - uOutlineSoftness, uOutlineDistance + uOutlineSoftness, outlineAlpha);
}

// Beveled outline effect
float calculateBeveledOutline(float textAlpha, vec2 uv, float width, vec3 lightDir, float amount)
{
    if (!uBeveledOutline) return calculateOutlineAlpha(textAlpha, uv, width, uOutlineSoftness);
    
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    vec3 normal = vec3(0.0, 0.0, 1.0);
    
    // Calculate normal from surrounding pixels
    int samples = int(width);
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            if (x == 0 && y == 0) continue;
            
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float sample = sampleTextureAA(uTexture, uv + offset);
            
            // Calculate normal contribution
            vec3 contribution = vec3(float(x), float(y), sample * amount);
            normal += contribution;
        }
    }
    
    normal = normalize(normal);
    
    // Calculate lighting
    float lighting = max(dot(normal, normalize(lightDir)), 0.0);
    
    float outlineAlpha = calculateOutlineAlpha(textAlpha, uv, width, uOutlineSoftness);
    return outlineAlpha * (0.5 + lighting * 0.5);
}

// Dashed outline effect
float calculateDashedOutline(float outlineAlpha, vec2 uv, float dashLength, float dashGap, float offset)
{
    if (!uDashedOutline) return outlineAlpha;
    
    // Calculate position along outline
    float position = length(uv - 0.5) + offset;
    float dashPhase = mod(position, dashLength + dashGap);
    
    // Create dashed pattern
    float dash = step(dashLength, dashPhase);
    
    return outlineAlpha * (1.0 - dash);
}

// Dotted outline effect
float calculateDottedOutline(float outlineAlpha, vec2 uv, float dotSize, float dotSpacing)
{
    if (!uDottedOutline) return outlineAlpha;
    
    // Calculate dot positions
    vec2 grid = floor(uv / dotSpacing) * dotSpacing;
    vec2 center = grid + dotSpacing * 0.5;
    float distance = length(uv - center);
    
    // Create dotted pattern
    float dot = step(dotSize, distance);
    
    return outlineAlpha * (1.0 - dot);
}

// Gradient outline effect
vec4 applyGradientOutline(vec4 baseColor, vec2 uv, vec4 startColor, vec4 endColor, vec2 direction)
{
    float t = dot(uv - 0.5, normalize(direction)) + 0.5;
    vec4 gradientColor = mix(startColor, endColor, saturate(t));
    
    return vec4(baseColor.rgb * gradientColor.rgb, baseColor.a);
}

// Noisy outline effect
float applyNoiseOutline(float outlineAlpha, vec2 uv, float time, float amount, float scale)
{
    if (!uNoiseOutline) return outlineAlpha;
    
    vec2 noiseUV = uv * scale + time * 0.1;
    float noise = noise2D(noiseUV) * amount;
    
    return outlineAlpha * (1.0 + noise);
}

// Distorted outline effect
vec2 applyDistortion(vec2 uv, float time, float amount, float frequency)
{
    if (!uDistortedOutline) return uv;
    
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
    
    // Glow animation
    if (uGlowAnimation)
    {
        float glowPulse = sin(time * uGlowPulseSpeed) * uGlowPulseAmount;
        vec4 glowColor = vec4(uGlowColor.rgb, animatedColor.a * (0.5 + glowPulse * 0.5));
        animatedColor = mix(animatedColor, glowColor, 0.3);
    }
    
    return animatedColor;
}

// Double outline effect
vec4 renderDoubleOutline(vec4 textColor, vec4 innerOutline, vec4 outerOutline, float innerWidth, float outerWidth)
{
    if (!uDoubleOutline) return innerOutline;
    
    // Blend text over inner outline
    vec4 result = mix(innerOutline, textColor, textColor.a);
    
    // Blend outer outline
    result = mix(outerOutline, result, result.a);
    
    return result;
}

// Subpixel rendering for outline
vec4 renderOutlineSubpixel(vec2 uv, vec4 color, vec3 weights, float width)
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    
    // Sample subpixels
    float alphaR = sampleTextureAA(uTexture, uv + vec2(-texelSize.x * 0.33, 0.0));
    float alphaG = sampleTextureAA(uTexture, uv);
    float alphaB = sampleTextureAA(uTexture, uv + vec2(texelSize.x * 0.33, 0.0));
    
    // Calculate outline for each subpixel
    float outlineR = calculateOutlineAlpha(alphaR, uv, width, uOutlineSoftness);
    float outlineG = calculateOutlineAlpha(alphaG, uv, width, uOutlineSoftness);
    float outlineB = calculateOutlineAlpha(alphaB, uv, width, uOutlineSoftness);
    
    // Combine with weights
    vec3 finalColor = vec3(outlineR, outlineG, outlineB) * weights;
    float finalAlpha = (outlineR + outlineG + outlineB) / 3.0;
    
    return vec4(color.rgb * finalColor, finalAlpha * color.a);
}

// Debug visualization
vec4 renderDebug(vec2 uv, int channel)
{
    float textAlpha = sampleTextureAA(uTexture, uv);
    float outlineAlpha = calculateOutlineAlpha(textAlpha, uv, uOutlineWidth, uOutlineSoftness);
    
    switch (channel)
    {
        case 0: // Text only
            return vec4(uTextColor.rgb, textAlpha * uTextColor.a);
        case 1: // Outline only
            return vec4(uOutlineColor.rgb, outlineAlpha * uOutlineColor.a);
        case 2: // Combined
            vec4 text = vec4(uTextColor.rgb, textAlpha * uTextColor.a);
            vec4 outline = vec4(uOutlineColor.rgb, outlineAlpha * uOutlineColor.a);
            return mix(outline, text, text.a);
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
vec4 renderFontOutline(vec2 uv)
{
    // Apply distortion if enabled
    vec2 distortedUV = applyDistortion(uv, uTime, uDistortionAmount, uDistortionFrequency);
    
    // Sample text alpha
    float textAlpha = sampleTextureAA(uTexture, distortedUV);
    
    // Debug mode
    if (uDebugMode)
    {
        return renderDebug(distortedUV, uDebugChannel);
    }
    
    // Calculate outline alpha
    float outlineAlpha;
    if (uRoundedOutline)
    {
        outlineAlpha = calculateRoundedOutline(textAlpha, distortedUV, uOutlineWidth, uRoundedness);
    }
    else if (uBeveledOutline)
    {
        outlineAlpha = calculateBeveledOutline(textAlpha, distortedUV, uOutlineWidth, uBevelDirection, uBevelAmount);
    }
    else
    {
        outlineAlpha = calculateOutlineAlpha(textAlpha, distortedUV, uOutlineWidth, uOutlineSoftness);
    }
    
    // Apply pattern effects
    if (uDashedOutline)
    {
        outlineAlpha = calculateDashedOutline(outlineAlpha, distortedUV, uDashLength, uDashGap, uDashOffset);
    }
    else if (uDottedOutline)
    {
        outlineAlpha = calculateDottedOutline(outlineAlpha, distortedUV, uDotSize, uDotSpacing);
    }
    
    // Apply noise
    if (uNoiseOutline)
    {
        outlineAlpha = applyNoiseOutline(outlineAlpha, distortedUV, uTime, uNoiseAmount, uNoiseScale);
    }
    
    // Create outline color
    vec4 outlineColor = vec4(uOutlineColor.rgb, outlineAlpha * uOutlineColor.a * uOutlineIntensity);
    
    // Apply gradient to outline
    if (uGradientOutline)
    {
        outlineColor = applyGradientOutline(outlineColor, distortedUV, uGradientStart, uGradientEnd, uGradientDirection);
    }
    else if (uEnableGradient && uGradientTexture != sampler2D(0))
    {
        vec4 gradientColor = texture(uGradientTexture, distortedUV);
        outlineColor.rgb *= gradientColor.rgb;
    }
    
    // Create text color
    vec4 textColor = vec4(uTextColor.rgb, textAlpha * uTextColor.a);
    
    // Apply vertex color
    textColor *= vColor;
    outlineColor *= vColor;
    
    // Apply vertex alpha
    textColor.a *= vAlpha;
    outlineColor.a *= vAlpha;
    
    // Subpixel rendering
    if (uSubpixelRendering)
    {
        outlineColor = renderOutlineSubpixel(distortedUV, outlineColor, uSubpixelWeights, uOutlineWidth);
    }
    
    // Combine text and outline
    vec4 result = mix(outlineColor, textColor, textColor.a);
    
    // Apply double outline
    if (uDoubleOutline)
    {
        float outerOutlineAlpha = calculateOutlineAlpha(textAlpha + outlineAlpha, distortedUV, uOuterOutlineWidth, uOutlineSoftness);
        vec4 outerOutlineColor = vec4(uOuterOutlineColor.rgb, outerOutlineAlpha * uOuterOutlineColor.a);
        result = renderDoubleOutline(textColor, outlineColor, outerOutlineColor, uOutlineWidth, uOuterOutlineWidth);
    }
    
    // Apply glow
    if (uEnableGlow)
    {
        vec4 glowColor = vec4(uGlowColor.rgb, outlineAlpha * uGlowColor.a);
        if (uOutlineGlow)
        {
            result = mix(glowColor, result, result.a);
        }
        else
        {
            result = mix(result, glowColor, 0.3);
        }
    }
    
    // Apply shadow
    if (uEnableShadow)
    {
        vec2 shadowOffset = vec2(uShadowOffsetX, uShadowOffsetY) / uResolution;
        vec4 shadowColor = vec4(uShadowColor.rgb, sampleTextureAA(uTexture, distortedUV + shadowOffset) * uShadowColor.a);
        result = mix(shadowColor, result, result.a);
    }
    
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
    vec4 color = renderFontOutline(vTexCoord);
    
    // Ensure color is in valid range
    color = saturate(color);
    
    fragColor = color;
}