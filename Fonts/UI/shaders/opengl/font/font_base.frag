#version 330 core

// Base Font Fragment Shader for OpenGL
// Basic font rendering without advanced effects

// Input textures
uniform sampler2D uTexture;           // Font texture (alpha channel)
uniform vec2 uResolution;             // Screen resolution
uniform vec4 uTextColor;              // Font color (RGBA)
uniform vec4 uBackgroundColor;         // Background color
uniform float uAlphaThreshold;        // Alpha threshold for rendering
uniform bool uUseAlphaThreshold;      // Enable alpha threshold
uniform bool uPremultipliedAlpha;     // Texture uses premultiplied alpha
uniform bool uGammaCorrect;           // Apply gamma correction
uniform float uGamma;                 // Gamma value
uniform bool uDithering;              // Enable dithering
uniform float uDitherStrength;         // Dithering strength
uniform bool uPixelPerfect;           // Pixel-perfect rendering
uniform vec2 uPixelSize;              // Pixel size for pixel-perfect
uniform bool uSubpixelRendering;       // Subpixel rendering
uniform vec3 uSubpixelWeights;         // RGB subpixel weights
uniform bool uAntialiasing;            // Enable antialiasing
uniform float uAntialiasingWidth;      // Antialiasing width
uniform bool uOutline;                 // Enable outline
uniform vec4 uOutlineColor;            // Outline color
uniform float uOutlineWidth;           // Outline width
uniform bool uShadow;                  // Enable shadow
uniform vec4 uShadowColor;             // Shadow color
uniform vec2 uShadowOffset;            // Shadow offset
uniform float uShadowBlur;             // Shadow blur
uniform bool uGradient;                // Enable gradient
uniform vec4 uGradientStart;           // Gradient start color
uniform vec4 uGradientEnd;             // Gradient end color
uniform vec2 uGradientDirection;       // Gradient direction
uniform bool uTextureUV;               // Use texture UV for gradient
uniform float uTime;                   // Animation time
uniform bool uAnimated;                // Enable animation
uniform float uAnimationSpeed;         // Animation speed
uniform float uAnimationPhase;         // Animation phase

// Input from vertex shader
in vec2 vTexCoord;
in vec2 vScreenPos;
in vec4 vColor;
in float vAlpha;

// Output color
out vec4 fragColor;

// Include common functions
#include "common/common.glsl"

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

// Subpixel rendering
vec3 subpixelRender(sampler2D tex, vec2 uv, vec2 texelSize, vec3 weights)
{
    vec3 color;
    
    // Sample RGB subpixels
    color.r = texture(tex, uv + vec2(-texelSize.x * 0.33, 0.0)).r;
    color.g = texture(tex, uv).g;
    color.b = texture(tex, uv + vec2(texelSize.x * 0.33, 0.0)).b;
    
    // Apply subpixel weights
    color *= weights;
    
    return color;
}

// Antialiasing
float antialiasAlpha(float alpha, vec2 uv, vec2 texelSize)
{
    float aa = 0.0;
    
    // Sample surrounding pixels for antialiasing
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize * uAntialiasingWidth;
            float sample = texture(uTexture, uv + offset).a;
            float weight = 1.0 - length(vec2(float(x), float(y))) / 1.414;
            aa += sample * weight;
        }
    }
    
    aa /= 9.0; // Normalize
    
    return mix(alpha, aa, 0.5);
}

// Outline effect
float outlineAlpha(float alpha, vec2 uv, vec2 texelSize)
{
    float outline = 0.0;
    
    // Sample surrounding pixels for outline
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            if (x == 0 && y == 0) continue;
            
            vec2 offset = vec2(float(x), float(y)) * texelSize * uOutlineWidth;
            float sample = texture(uTexture, uv + offset).a;
            outline = max(outline, sample);
        }
    }
    
    // Create outline where there's no original pixel but surrounding pixels exist
    return outline * (1.0 - alpha);
}

// Shadow effect
vec4 renderShadow(vec2 uv, vec2 texelSize)
{
    vec2 shadowUV = uv + uShadowOffset * texelSize;
    
    // Apply blur to shadow
    vec4 shadowColor = vec4(0.0);
    if (uShadowBlur > 0.0)
    {
        int blurRadius = int(uShadowBlur);
        for (int x = -blurRadius; x <= blurRadius; x++)
        {
            for (int y = -blurRadius; y <= blurRadius; y++)
            {
                vec2 offset = vec2(float(x), float(y)) * texelSize;
                float sample = texture(uTexture, shadowUV + offset).a;
                float weight = exp(-length(vec2(float(x), float(y))) / uShadowBlur);
                shadowColor += vec4(uShadowColor.rgb, sample) * weight;
            }
        }
        shadowColor /= float((blurRadius * 2 + 1) * (blurRadius * 2 + 1));
    }
    else
    {
        float shadowAlpha = texture(uTexture, shadowUV).a;
        shadowColor = vec4(uShadowColor.rgb, shadowAlpha);
    }
    
    return shadowColor;
}

// Gradient effect
vec4 applyGradient(vec4 baseColor, vec2 uv)
{
    vec4 gradientColor;
    
    if (uTextureUV)
    {
        // Use texture UV for gradient
        float t = dot(uv - 0.5, normalize(uGradientDirection)) + 0.5;
        gradientColor = mix(uGradientStart, uGradientEnd, saturate(t));
    }
    else
    {
        // Use screen position for gradient
        vec2 screenPos = vScreenPos / uResolution;
        float t = dot(screenPos - 0.5, normalize(uGradientDirection)) + 0.5;
        gradientColor = mix(uGradientStart, uGradientEnd, saturate(t));
    }
    
    return vec4(baseColor.rgb * gradientColor.rgb, baseColor.a);
}

// Animation effect
vec4 applyAnimation(vec4 baseColor, float time)
{
    if (!uAnimated) return baseColor;
    
    // Pulsing animation
    float pulse = sin(time * uAnimationSpeed + uAnimationPhase) * 0.5 + 0.5;
    
    // Apply to alpha
    baseColor.a *= (0.5 + pulse * 0.5);
    
    // Apply to color intensity
    baseColor.rgb *= (0.8 + pulse * 0.2);
    
    return baseColor;
}

// Main rendering function
vec4 renderFont(vec2 uv)
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    
    // Sample texture
    vec4 texColor = texture(uTexture, uv);
    
    // Handle premultiplied alpha
    if (uPremultipliedAlpha)
    {
        texColor.rgb /= max(texColor.a, 0.001);
    }
    
    float alpha = texColor.a;
    
    // Apply alpha threshold
    if (uUseAlphaThreshold)
    {
        alpha = smoothstep(uAlphaThreshold - 0.01, uAlphaThreshold + 0.01, alpha);
    }
    
    // Apply antialiasing
    if (uAntialiasing)
    {
        alpha = antialiasAlpha(alpha, uv, texelSize);
    }
    
    // Pixel-perfect rendering
    if (uPixelPerfect)
    {
        vec2 pixelUV = floor(uv * uPixelSize) / uPixelSize;
        alpha = texture(uTexture, pixelUV).a;
    }
    
    // Subpixel rendering
    vec3 finalColor;
    if (uSubpixelRendering)
    {
        finalColor = subpixelRender(uTexture, uv, texelSize, uSubpixelWeights);
    }
    else
    {
        finalColor = vec3(texColor.r);
    }
    
    // Apply text color
    vec4 baseColor = vec4(finalColor * uTextColor.rgb, alpha * uTextColor.a);
    
    // Apply vertex color
    baseColor *= vColor;
    
    // Apply outline
    if (uOutline)
    {
        float outlineAlpha = outlineAlpha(alpha, uv, texelSize);
        vec4 outlineColor = vec4(uOutlineColor.rgb, outlineAlpha * uOutlineColor.a);
        baseColor = mix(outlineColor, baseColor, baseColor.a);
    }
    
    // Apply shadow
    vec4 finalColor = baseColor;
    if (uShadow)
    {
        vec4 shadowColor = renderShadow(uv, texelSize);
        finalColor = mix(shadowColor, baseColor, baseColor.a);
    }
    
    // Apply gradient
    if (uGradient)
    {
        finalColor = applyGradient(finalColor, uv);
    }
    
    // Apply animation
    finalColor = applyAnimation(finalColor, uTime);
    
    // Apply dithering
    if (uDithering)
    {
        float ditherValue = dither(vScreenPos / uResolution, uDitherStrength);
        finalColor.rgb += vec3(ditherValue) / 255.0;
    }
    
    // Apply gamma correction
    if (uGammaCorrect)
    {
        finalColor.rgb = pow(finalColor.rgb, vec3(1.0 / uGamma));
    }
    
    return finalColor;
}

// Main function
void main()
{
    vec4 color = renderFont(vTexCoord);
    
    // Ensure color is in valid range
    color = saturate(color);
    
    fragColor = color;
}