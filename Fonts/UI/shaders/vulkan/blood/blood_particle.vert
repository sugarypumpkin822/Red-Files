#version 450 core

// Blood Particle Vertex Shader for Vulkan
// Working blood particle vertex shader

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec4 aColor;
layout(location = 4) in vec4 aProperties;

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec4 vColor;
layout(location = 3) out vec4 vProperties;
layout(location = 4) out vec3 vWorldPos;
layout(location = 5) out vec3 vViewPos;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewProjectionMatrix;
    vec4 cameraPosition;
    vec4 time;
} camera;

layout(set = 0, binding = 1) uniform sampler2D particleTexture;

layout(push_constant) uniform PushConstants {
    mat4 modelMatrix;
    vec4 particleColor;
    vec4 particleSize;
    vec4 animationParams;
} pushConstants;

void main() {
    // Transform position
    vec4 worldPos = pushConstants.modelMatrix * vec4(aPosition, 1.0);
    vec4 viewPos = camera.viewMatrix * worldPos;
    vec4 clipPos = camera.projectionMatrix * viewPos;
    
    // Transform normal
    vNormal = mat3(pushConstants.modelMatrix) * aNormal;
    
    // Pass through texture coordinates
    vTexCoord = aTexCoord;
    
    // Apply color modulation
    vColor = aColor * pushConstants.particleColor;
    
    // Pass through properties
    vProperties = aProperties;
    
    // Pass positions
    vWorldPos = worldPos.xyz;
    vViewPos = viewPos.xyz;
    
    // Set final position
    gl_Position = clipPos;
    gl_PointSize = pushConstants.particleSize.x;
}
