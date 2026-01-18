#version 450 core

// Font Vertex Shader for Vulkan
// Working font vertex shader

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec4 aColor;
layout(location = 4) in vec4 aCharData;

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec4 vColor;
layout(location = 3) out vec4 vCharData;
layout(location = 4) out vec3 vWorldPos;
layout(location = 5) out vec3 vViewPos;

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

layout(push_constant) uniform PushConstants {
    mat4 modelMatrix;
    vec4 charColor;
    vec4 charSize;
    vec4 animationData;
} pushConstants;

// Animation functions
vec3 applyWave(vec3 pos, float time, float frequency, float amplitude) {
    float wave = sin(pos.x * frequency + time) * amplitude;
    return pos + vec3(0.0, wave, 0.0);
}

vec3 applyPulse(vec3 pos, float time, float frequency, float amplitude) {
    float pulse = sin(time * frequency) * amplitude;
    return pos * (1.0 + pulse);
}

void main() {
    // Get character properties
    float charIndex = aCharData.x;
    float charAge = aCharData.y;
    float charSize = aCharData.z;
    float charType = aCharData.w;
    
    // Apply animations
    vec3 animatedPos = aPosition;
    animatedPos = applyWave(animatedPos, camera.time.x, font.animationParams.x, font.animationParams.y);
    animatedPos = applyPulse(animatedPos, camera.time.x, font.animationParams.z, font.animationParams.w);
    
    // Transform position
    vec4 worldPos = pushConstants.modelMatrix * vec4(animatedPos, 1.0);
    vec4 viewPos = camera.viewMatrix * worldPos;
    vec4 clipPos = camera.projectionMatrix * viewPos;
    
    // Transform normal
    vNormal = mat3(pushConstants.modelMatrix) * aNormal;
    
    // Pass through texture coordinates
    vTexCoord = aTexCoord;
    
    // Apply color modulation
    vColor = aColor * pushConstants.charColor * font.fontColor;
    
    // Pass through character data
    vCharData = aCharData;
    
    // Pass positions
    vWorldPos = worldPos.xyz;
    vViewPos = viewPos.xyz;
    
    // Set final position
    gl_Position = clipPos;
    gl_PointSize = charSize * font.fontSize.x;
}
