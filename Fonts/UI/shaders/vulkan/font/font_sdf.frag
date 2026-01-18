#version 450 core

// Font SDF Fragment Shader for Vulkan
// Working signed distance field font fragment shader

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec4 vColor;
layout(location = 3) in vec4 vCharData;
layout(location = 4) in vec3 vWorldPos;
layout(location = 5) in vec3 vViewPos;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragNormal;
layout(location = 2) out vec4 fragMaterial;
layout(location = 3) out vec4 fragDepth;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewProjectionMatrix;
    vec4 cameraPosition;
    vec4 time;
} camera;

layout(set = 0, binding = 1) uniform FontData {
    vec4 fontColor;
    vec4 fontSize;
    vec4 fontSpacing;
    vec4 animationParams;
} font;

layout(set = 0, binding = 2) uniform sampler2D sdfTexture;

layout(set = 0, binding = 3) uniform EffectData {
    vec4 outlineColor;
    vec4 shadowColor;
    vec4 glowColor;
    vec4 effectParams;
} effects;

layout(set = 0, binding = 4) uniform SDFData {
    vec4 sdfThreshold;
    vec4 sdfSoftness;
    vec4 outlineWidth;
    vec4 outlineSoftness;
} sdf;

// Random number generator
float random(vec2 seed) {
    return fract(sin(dot(seed.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

// Simple noise function
float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    
    float a = random(i.xy + p.z);
    float b = random(i.xy + vec2(1.0, 0.0) + p.z);
    float c = random(i.xy + vec2(0.0, 1.0) + p.z);
    float d = random(i.xy + vec2(1.0, 1.0) + p.z);
    
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// Advanced SDF rendering functions
float renderSDF(float sdfValue, float threshold, float softness) {
    return smoothstep(threshold - softness, threshold + softness, sdfValue);
}

float renderSDFOutline(float sdfValue, float threshold, float softness, float outlineWidth) {
    float sdf = sdfValue;
    float alpha = renderSDF(sdf, threshold, softness);
    float outlineAlpha = renderSDF(sdf + outlineWidth, threshold, softness * 2.0);
    return max(alpha, outlineAlpha * 0.5);
}

float renderSDFShadow(float sdfValue, vec2 uv, vec2 shadowOffset, float shadowBlur) {
    vec2 shadowUV = uv + shadowOffset;
    float shadowSDF = texture(sdfTexture, shadowUV).r;
    float shadowAlpha = renderSDF(shadowSDF, sdf.sdfThreshold.x, shadowBlur);
    return shadowAlpha * 0.3;
}

float renderSDFGlow(float sdfValue, float threshold, float softness, float glowIntensity, float glowRadius) {
    float sdf = sdfValue;
    float glow = renderSDF(sdf - glowRadius, threshold, softness * 2.0);
    return glow * glowIntensity;
}

vec3 renderSDFGradient(float sdfValue, vec4 color1, vec4 color2, vec4 color3, vec4 color4, float gradientType) {
    vec3 gradientColor;
    if (gradientType < 0.5) {
        // Linear gradient
        gradientColor = mix(color1.rgb, color2.rgb, sdfValue);
    } else if (gradientType < 1.5) {
        // Radial gradient
        gradientColor = mix(color1.rgb, color2.rgb, sdfValue * sdfValue);
    } else if (gradientType < 2.5) {
        // Angular gradient
        gradientColor = mix(color1.rgb, color2.rgb, atan(sdfValue, sdfValue) / 3.14159 + 0.5);
    } else {
        // Four-color gradient
        gradientColor = mix(mix(color1.rgb, color2.rgb, sdfValue), mix(color3.rgb, color4.rgb, sdfValue), 0.5);
    }
    return gradientColor;
}

// SDF distortion effects
vec2 renderSDFDistortion(vec2 uv, float intensity, float scale, float speed, float type) {
    vec2 distortUV = uv * scale + camera.time.x * speed;
    vec2 distortion = vec2(
        noise(vec3(distortUV, camera.time.x)),
        noise(vec3(distortUV + 100.0, camera.time.x))
    ) - 0.5;
    
    if (type < 0.5) {
        // Simple distortion
        return uv + distortion * intensity;
    } else if (type < 1.5) {
        // Wave distortion
        float wave = sin(uv.x * 10.0 + camera.time.x * speed) * 0.1;
        return uv + vec2(0.0, wave) * intensity;
    } else if (type < 2.5) {
        // Swirl distortion
        vec2 center = vec2(0.5);
        vec2 offset = uv - center;
        float angle = length(offset) * 5.0 + camera.time.x * speed;
        mat2 rotation = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
        return center + rotation * offset * intensity;
    } else {
        // Bubble distortion
        vec2 center = vec2(0.5);
        vec2 offset = uv - center;
        float distance = length(offset);
        float bubble = 1.0 + sin(distance * 20.0 - camera.time.x * speed) * 0.1;
        return center + offset * bubble * intensity;
    }
}

// SDF animation functions
vec3 renderSDFAnimation(vec2 uv, vec3 color, float speed, float amplitude, float frequency, float phase, float type) {
    float time = camera.time.x * speed + phase;
    
    if (type < 0.5) {
        // Pulse animation
        float pulse = sin(time * frequency) * amplitude;
        return color * (1.0 + pulse);
    } else if (type < 1.5) {
        // Wave animation
        float wave = sin(uv.x * frequency + time) * amplitude;
        return color * (1.0 + wave);
    } else if (type < 2.5) {
        // Ripple animation
        vec2 center = vec2(0.5);
        float distance = length(uv - center);
        float ripple = sin(distance * frequency - time) * amplitude;
        return color * (1.0 + ripple);
    } else {
        // Breathing animation
        float breathe = sin(time * frequency) * amplitude;
        return color * (1.0 + breathe);
    }
}

void main() {
    // Sample SDF texture
    float sdfValue = texture(sdfTexture, vTexCoord).r;
    
    // Apply distortion
    vec2 distortedUV = renderSDFDistortion(vTexCoord, effects.effectParams.x, effects.effectParams.y, effects.effectParams.z, effects.effectParams.w);
    float distortedSDF = texture(sdfTexture, distortedUV).r;
    
    // Get character properties
    float charIndex = vCharData.x;
    float charAge = vCharData.y;
    float charSize = vCharData.z;
    float charType = vCharData.w;
    
    // Apply SDF rendering
    float sdfAlpha = renderSDF(distortedSDF, sdf.sdfThreshold.x, sdf.sdfSoftness.x);
    float outlineAlpha = renderSDFOutline(distortedSDF, sdf.sdfThreshold.x, sdf.sdfSoftness.x, sdf.outlineWidth.x);
    float shadowAlpha = renderSDFShadow(distortedSDF, vTexCoord, vec2(0.01, 0.01), 0.01);
    float glowAlpha = renderSDFGlow(distortedSDF, sdf.sdfThreshold.x, sdf.sdfSoftness.x, effects.glowColor.a, 0.1);
    
    // Apply gradient
    vec3 gradientColor = renderSDFGradient(distortedSDF, font.fontColor, effects.outlineColor, effects.shadowColor, effects.glowColor, charType);
    
    // Apply animation
    vec3 animatedColor = renderSDFAnimation(distortedUV, vColor.rgb, font.animationParams.x, font.animationParams.y, font.animationParams.z, font.animationParams.w, charType);
    
    // Add animated noise
    float noiseValue = noise(vWorldPos + camera.time.xxx * 0.5);
    vec3 noiseColor = vec3(noiseValue * 0.05);
    
    // Calculate final color
    vec3 baseColor = animatedColor * font.fontColor.rgb;
    vec3 finalColor = baseColor;
    
    // Apply gradient
    finalColor = mix(finalColor, gradientColor, 0.5);
    
    // Apply outline
    if (outlineAlpha > 0.5) {
        finalColor = mix(finalColor, effects.outlineColor.rgb, outlineAlpha * 0.5);
    }
    
    // Apply shadow
    finalColor *= (1.0 - shadowAlpha * 0.3);
    
    // Apply glow
    finalColor += effects.glowColor.rgb * glowAlpha;
    
    // Apply noise
    finalColor += noiseColor;
    
    // Calculate final alpha
    float finalAlpha = sdfAlpha * vColor.a;
    finalAlpha = max(finalAlpha, outlineAlpha * 0.5);
    finalAlpha = max(finalAlpha, glowAlpha * 0.5);
    
    // Age-based effects
    if (charAge > 0.5) {
        float fadeFactor = 1.0 - (charAge - 0.5) * 2.0;
        finalAlpha *= fadeFactor;
    }
    
    // Size-based effects
    if (charSize < 0.5) {
        float sharpness = charSize * 2.0;
        finalAlpha = mix(finalAlpha, 1.0, sharpness);
    }
    
    // Output final color
    fragColor = vec4(finalColor, finalAlpha);
    fragNormal = vec4(normalize(vNormal), 1.0);
    fragMaterial = vec4(vCharData.xyz, 1.0);
    fragDepth = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
}
