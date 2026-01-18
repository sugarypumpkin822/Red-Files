#version 330 core

// Blur Fragment Shader for OpenGL
// Applies various blur effects including Gaussian, box, and motion blur

// Input texture
uniform sampler2D uTexture;           // Source texture
uniform vec2 uResolution;             // Screen resolution
uniform vec2 uDirection;              // Blur direction (for directional blur)
uniform float uRadius;                // Blur radius
uniform float uStrength;              // Blur strength/intensity
uniform int uBlurType;                // Blur type (0=box, 1=gaussian, 2=radial, 3=motion)
uniform float uTime;                  // Animation time (for animated effects)
uniform vec2 uCenter;                 // Center point for radial blur
uniform float uAngle;                 // Angle for directional blur
uniform float uKernel[9];             // Custom kernel weights
uniform bool uUseCustomKernel;        // Whether to use custom kernel
uniform float uSigma;                 // Gaussian sigma value
uniform int uSamples;                 // Number of samples for high-quality blur
uniform bool uSeparable;              // Whether to use separable blur
uniform float uThreshold;             // Threshold for selective blur
uniform vec4 uBlurColor;              // Color tint for blur effect
uniform bool uLumaBlur;               // Blur based on luminance
uniform float uLumaThreshold;         // Luminance threshold for selective blur

// Input from vertex shader
in vec2 vTexCoord;
in vec2 vScreenPos;

// Output color
out vec4 fragColor;

// Include common functions
#include "common/common.glsl"

// Gaussian kernel calculation
float gaussian(float x, float sigma)
{
    return exp(-(x * x) / (2.0 * sigma * sigma)) / (sigma * sqrt(2.0 * PI));
}

// Box blur
vec4 boxBlur(sampler2D tex, vec2 uv, vec2 texelSize, float radius)
{
    vec4 color = vec4(0.0);
    int samples = int(radius);
    
    for (int x = -samples; x <= samples; x++)
    {
        for (int y = -samples; y <= samples; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            color += texture(tex, uv + offset);
        }
    }
    
    int totalSamples = (samples * 2 + 1) * (samples * 2 + 1);
    return color / float(totalSamples);
}

// Gaussian blur
vec4 gaussianBlur(sampler2D tex, vec2 uv, vec2 texelSize, float radius, float sigma)
{
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;
    int samples = int(radius);
    
    for (int x = -samples; x <= samples; x++)
    {
        for (int y = -samples; y <= samples; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float distance = length(vec2(float(x), float(y)));
            float weight = gaussian(distance, sigma);
            
            color += texture(tex, uv + offset) * weight;
            totalWeight += weight;
        }
    }
    
    return color / totalWeight;
}

// Separable Gaussian blur (horizontal pass)
vec4 separableGaussianBlurH(sampler2D tex, vec2 uv, vec2 texelSize, float radius, float sigma)
{
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;
    int samples = int(radius);
    
    for (int x = -samples; x <= samples; x++)
    {
        vec2 offset = vec2(float(x), 0.0) * texelSize;
        float weight = gaussian(abs(float(x)), sigma);
        
        color += texture(tex, uv + offset) * weight;
        totalWeight += weight;
    }
    
    return color / totalWeight;
}

// Separable Gaussian blur (vertical pass)
vec4 separableGaussianBlurV(sampler2D tex, vec2 uv, vec2 texelSize, float radius, float sigma)
{
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;
    int samples = int(radius);
    
    for (int y = -samples; y <= samples; y++)
    {
        vec2 offset = vec2(0.0, float(y)) * texelSize;
        float weight = gaussian(abs(float(y)), sigma);
        
        color += texture(tex, uv + offset) * weight;
        totalWeight += weight;
    }
    
    return color / totalWeight;
}

// Radial blur
vec4 radialBlur(sampler2D tex, vec2 uv, vec2 center, float radius, float strength)
{
    vec4 color = vec4(0.0);
    vec2 direction = uv - center;
    float distance = length(direction);
    direction = normalize(direction);
    
    int samples = int(radius);
    
    for (int i = 0; i <= samples; i++)
    {
        float t = float(i) / float(samples);
        float offset = t * strength * distance;
        vec2 sampleUV = uv + direction * offset;
        
        color += texture(tex, sampleUV);
    }
    
    return color / float(samples + 1);
}

// Motion blur
vec4 motionBlur(sampler2D tex, vec2 uv, vec2 direction, float strength)
{
    vec4 color = vec4(0.0);
    direction = normalize(direction);
    
    int samples = int(strength);
    
    for (int i = -samples; i <= samples; i++)
    {
        float t = float(i) / float(samples);
        vec2 offset = direction * t * strength;
        color += texture(tex, uv + offset);
    }
    
    return color / float(samples * 2 + 1);
}

// Directional blur
vec4 directionalBlur(sampler2D tex, vec2 uv, vec2 texelSize, float radius, float angle)
{
    vec4 color = vec4(0.0);
    vec2 direction = vec2(cos(angle), sin(angle));
    
    int samples = int(radius);
    
    for (int i = -samples; i <= samples; i++)
    {
        float t = float(i) / float(samples);
        vec2 offset = direction * t * radius * texelSize;
        color += texture(tex, uv + offset);
    }
    
    return color / float(samples * 2 + 1);
}

// Custom kernel blur
vec4 customKernelBlur(sampler2D tex, vec2 uv, vec2 texelSize, float kernel[9])
{
    vec4 color = vec4(0.0);
    int index = 0;
    
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            color += texture(tex, uv + offset) * kernel[index];
            index++;
        }
    }
    
    return color;
}

// Selective blur based on threshold
vec4 selectiveBlur(sampler2D tex, vec2 uv, vec2 texelSize, float radius, float threshold)
{
    vec4 originalColor = texture(tex, uv);
    float luminance = dot(originalColor.rgb, vec3(0.299, 0.587, 0.114));
    
    if (luminance < threshold)
    {
        return boxBlur(tex, uv, texelSize, radius);
    }
    
    return originalColor;
}

// Luminance-based blur
vec4 luminanceBlur(sampler2D tex, vec2 uv, vec2 texelSize, float radius, float threshold)
{
    vec4 originalColor = texture(tex, uv);
    float luminance = dot(originalColor.rgb, vec3(0.299, 0.587, 0.114));
    
    float blurAmount = smoothstep(threshold, 1.0, 1.0 - luminance);
    vec4 blurredColor = gaussianBlur(tex, uv, texelSize, radius, uSigma);
    
    return mix(originalColor, blurredColor, blurAmount);
}

// Depth-aware blur (requires depth texture)
vec4 depthAwareBlur(sampler2D tex, sampler2D depthTex, vec2 uv, vec2 texelSize, float radius, float sigma)
{
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;
    float centerDepth = texture(depthTex, uv).r;
    
    int samples = int(radius);
    
    for (int x = -samples; x <= samples; x++)
    {
        for (int y = -samples; y <= samples; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float sampleDepth = texture(depthTex, uv + offset).r;
            
            float depthDiff = abs(centerDepth - sampleDepth);
            float depthWeight = exp(-depthDiff * 50.0); // Adjust sensitivity
            
            float distance = length(vec2(float(x), float(y)));
            float spatialWeight = gaussian(distance, sigma);
            
            float totalWeight = depthWeight * spatialWeight;
            
            color += texture(tex, uv + offset) * totalWeight;
            totalWeight += totalWeight;
        }
    }
    
    return color / totalWeight;
}

// Smart blur (preserves edges)
vec4 smartBlur(sampler2D tex, vec2 uv, vec2 texelSize, float radius, float sigma)
{
    vec4 centerColor = texture(tex, uv);
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;
    
    int samples = int(radius);
    
    for (int x = -samples; x <= samples; x++)
    {
        for (int y = -samples; y <= samples; y++)
        {
            if (x == 0 && y == 0) continue;
            
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            vec4 sampleColor = texture(tex, uv + offset);
            
            // Calculate color difference
            float colorDiff = length(centerColor.rgb - sampleColor.rgb);
            float edgeWeight = exp(-colorDiff * 10.0); // Adjust sensitivity
            
            float distance = length(vec2(float(x), float(y)));
            float spatialWeight = gaussian(distance, sigma);
            
            float totalWeight = edgeWeight * spatialWeight;
            
            color += sampleColor * totalWeight;
            totalWeight += totalWeight;
        }
    }
    
    return mix(centerColor, color / totalWeight, 0.5);
}

// Anamorphic blur (stretched horizontally)
vec4 anamorphicBlur(sampler2D tex, vec2 uv, vec2 texelSize, float horizontalRadius, float verticalRadius, float sigma)
{
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;
    
    int hSamples = int(horizontalRadius);
    int vSamples = int(verticalRadius);
    
    for (int x = -hSamples; x <= hSamples; x++)
    {
        for (int y = -vSamples; y <= vSamples; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float distance = length(vec2(float(x) / horizontalRadius, float(y) / verticalRadius));
            float weight = gaussian(distance, sigma);
            
            color += texture(tex, uv + offset) * weight;
            totalWeight += weight;
        }
    }
    
    return color / totalWeight;
}

// Bokeh blur (circular aperture simulation)
vec4 bokehBlur(sampler2D tex, vec2 uv, vec2 texelSize, float radius, float blades)
{
    vec4 color = vec4(0.0);
    int samples = int(radius);
    
    for (int i = 0; i < samples; i++)
    {
        float angle = float(i) / float(samples) * TWO_PI;
        float bladeAngle = TWO_PI / blades;
        float currentBladeAngle = floor(angle / bladeAngle) * bladeAngle;
        
        vec2 offset = vec2(cos(currentBladeAngle), sin(currentBladeAngle)) * radius * texelSize;
        color += texture(tex, uv + offset);
    }
    
    return color / float(samples);
}

// Variable blur (radius varies based on texture)
vec4 variableBlur(sampler2D tex, vec2 uv, vec2 texelSize, float maxRadius, float sigma)
{
    vec4 blurMap = texture(tex, uv);
    float radius = blurMap.a * maxRadius; // Use alpha channel as blur map
    
    return gaussianBlur(tex, uv, texelSize, radius, sigma);
}

// Main blur function
vec4 applyBlur(sampler2D tex, vec2 uv, vec2 texelSize)
{
    vec4 result;
    
    switch (uBlurType)
    {
        case 0: // Box blur
            result = boxBlur(tex, uv, texelSize, uRadius);
            break;
        case 1: // Gaussian blur
            result = gaussianBlur(tex, uv, texelSize, uRadius, uSigma);
            break;
        case 2: // Radial blur
            result = radialBlur(tex, uv, uCenter, uRadius, uStrength);
            break;
        case 3: // Motion blur
            result = motionBlur(tex, uv, uDirection, uStrength);
            break;
        case 4: // Directional blur
            result = directionalBlur(tex, uv, texelSize, uRadius, uAngle);
            break;
        case 5: // Custom kernel blur
            result = customKernelBlur(tex, uv, texelSize, uKernel);
            break;
        case 6: // Selective blur
            result = selectiveBlur(tex, uv, texelSize, uRadius, uThreshold);
            break;
        case 7: // Luminance blur
            result = luminanceBlur(tex, uv, texelSize, uRadius, uLumaThreshold);
            break;
        case 8: // Smart blur
            result = smartBlur(tex, uv, texelSize, uRadius, uSigma);
            break;
        case 9: // Anamorphic blur
            result = anamorphicBlur(tex, uv, texelSize, uRadius, uRadius * 0.3, uSigma);
            break;
        case 10: // Bokeh blur
            result = bokehBlur(tex, uv, texelSize, uRadius, 6.0);
            break;
        default:
            result = gaussianBlur(tex, uv, texelSize, uRadius, uSigma);
            break;
    }
    
    // Apply color tint
    result.rgb = mix(result.rgb, result.rgb * uBlurColor.rgb, uBlurColor.a);
    
    // Apply strength
    vec4 originalColor = texture(tex, uv);
    result = mix(originalColor, result, uStrength);
    
    return result;
}

// High-quality multi-pass blur
vec4 highQualityBlur(sampler2D tex, vec2 uv, vec2 texelSize)
{
    vec4 result = texture(tex, uv);
    
    // Multiple passes with different radii
    for (int pass = 0; pass < 3; pass++)
    {
        float passRadius = uRadius * (float(pass + 1) / 3.0);
        vec4 passResult = gaussianBlur(tex, uv, texelSize, passRadius, uSigma);
        result = mix(result, passResult, 0.33);
    }
    
    return result;
}

// Animated blur (radius changes over time)
vec4 animatedBlur(sampler2D tex, vec2 uv, vec2 texelSize)
{
    float animatedRadius = uRadius + sin(uTime * 2.0) * uRadius * 0.5;
    return gaussianBlur(tex, uv, texelSize, animatedRadius, uSigma);
}

// Main function
void main()
{
    vec2 texelSize = 1.0 / uResolution;
    vec4 result;
    
    // Choose blur method based on settings
    if (uSeparable && (uBlurType == 1 || uBlurType == 0))
    {
        // Separable blur (more efficient for large kernels)
        vec4 horizontal = separableGaussianBlurH(uTexture, vTexCoord, texelSize, uRadius, uSigma);
        result = separableGaussianBlurV(uTexture, vTexCoord, texelSize, uRadius, uSigma);
    }
    else if (uSamples > 1)
    {
        // High-quality multi-pass blur
        result = highQualityBlur(uTexture, vTexCoord, texelSize);
    }
    else if (uTime > 0.0)
    {
        // Animated blur
        result = animatedBlur(uTexture, vTexCoord, texelSize);
    }
    else
    {
        // Standard blur
        result = applyBlur(uTexture, vTexCoord, texelSize);
    }
    
    // Apply luminance-based blur if enabled
    if (uLumaBlur)
    {
        result = luminanceBlur(uTexture, vTexCoord, texelSize, uRadius, uLumaThreshold);
    }
    
    fragColor = result;
}