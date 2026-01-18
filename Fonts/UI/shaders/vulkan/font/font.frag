#version 450 core

// Font Fragment Shader for Vulkan
// Working font fragment shader

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

layout(set = 0, binding = 2) uniform sampler2D fontTexture;

layout(set = 0, binding = 3) uniform sampler2D sdfTexture;

layout(set = 0, binding = 4) uniform EffectData {
    vec4 outlineColor;
    vec4 shadowColor;
    vec4 glowColor;
    vec4 effectParams;
} effects;

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

// SDF rendering functions
float renderSDF(float sdfValue, float threshold, float softness) {
    return smoothstep(threshold - softness, threshold + softness, sdfValue);
}

float renderOutline(float sdfValue, float outlineWidth, float outlineSoftness) {
    float outline = smoothstep(outlineWidth - outlineSoftness, outlineWidth + outlineSoftness, sdfValue);
    return outline;
}

float renderShadow(vec2 uv, vec2 shadowOffset, float shadowBlur) {
    vec2 shadowUV = uv + shadowOffset;
    float shadow = texture(fontTexture, shadowUV).a;
    return smoothstep(0.5 - shadowBlur, 0.5 + shadowBlur, shadow);
}

vec3 renderGlow(vec2 uv, vec3 color, float intensity, float radius) {
    vec2 center = vec2(0.5);
    float distance = length(uv - center);
    float glow = exp(-distance * distance / (radius * radius));
    return color * glow * intensity;
}

void main() {
    // Sample textures
    vec4 fontTex = texture(fontTexture, vTexCoord);
    vec4 sdfTex = texture(sdfTexture, vTexCoord);
    
    // Get character properties
    float charIndex = vCharData.x;
    float charAge = vCharData.y;
    float charSize = vCharData.z;
    float charType = vCharData.w;
    
    // SDF rendering
    float sdfAlpha = renderSDF(sdfTex.r, 0.5, 0.1);
    float outlineAlpha = renderOutline(sdfTex.r, effects.effectParams.x, effects.effectParams.y);
    float shadowAlpha = renderShadow(vTexCoord, vec2(0.01, 0.01), effects.effectParams.z);
    
    // Apply effects
    vec3 glowColor = renderGlow(vTexCoord, effects.glowColor.rgb, effects.glowColor.a, effects.effectParams.w);
    
    // Add animated noise
    float noiseValue = noise(vWorldPos + camera.time.xxx * 0.5);
    vec3 noiseColor = vec3(noiseValue * 0.05);
    
    // Calculate final color
    vec3 baseColor = vColor.rgb * font.fontColor.rgb;
    vec3 finalColor = baseColor;
    
    // Apply outline
    if (outlineAlpha > 0.5) {
        finalColor = mix(finalColor, effects.outlineColor.rgb, outlineAlpha * 0.5);
    }
    
    // Apply shadow
    finalColor *= (1.0 - shadowAlpha * 0.3);
    
    // Apply glow
    finalColor += glowColor;
    
    // Apply noise
    finalColor += noiseColor;
    
    // Calculate final alpha
    float finalAlpha = sdfAlpha * fontTex.a * vColor.a;
    finalAlpha = max(finalAlpha, outlineAlpha * 0.5);
    
    // Age-based effects
    if (charAge > 0.5) {
        float fadeFactor = 1.0 - (charAge - 0.5) * 2.0;
        finalAlpha *= fadeFactor;
    }
    
    // Output final color
    fragColor = vec4(finalColor, finalAlpha);
    fragNormal = vec4(normalize(vNormal), 1.0);
    fragMaterial = vec4(vCharData.xyz, 1.0);
    fragDepth = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
}
