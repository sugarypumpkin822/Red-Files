#version 330 core

// Base Font Vertex Shader for OpenGL
// Basic font rendering vertex processing

// Input attributes
layout(location = 0) in vec3 aPosition;     // Vertex position (x, y, z)
layout(location = 1) in vec2 aTexCoord;     // Texture coordinates
layout(location = 2) in vec4 aColor;        // Vertex color
layout(location = 3) in float aAlpha;       // Vertex alpha
layout(location = 4) in vec3 aNormal;        // Vertex normal (for 3D effects)
layout(location = 5) in vec4 aTangent;       // Vertex tangent (for advanced effects)
layout(location = 6) in vec2 aSize;          // Vertex size (for scaling)
layout(location = 7) in float aRotation;     // Vertex rotation
layout(location = 8) in vec4 aBoneWeights;   // Bone weights (for skeletal animation)
layout(location = 9) in ivec4 aBoneIndices;  // Bone indices (for skeletal animation)
layout(location = 10) in vec4 aInstanceData; // Instance data (for instanced rendering)

// Uniform matrices
uniform mat4 uModelMatrix;              // Model matrix
uniform mat4 uViewMatrix;               // View matrix
uniform mat4 uProjectionMatrix;         // Projection matrix
uniform mat4 uNormalMatrix;             // Normal matrix
uniform mat4 uMVPMatrix;                // Model-View-Projection matrix
uniform mat4 uInverseModelMatrix;        // Inverse model matrix
uniform mat4 uInverseViewMatrix;         // Inverse view matrix
uniform mat4 uInverseProjectionMatrix;   // Inverse projection matrix

// Transform parameters
uniform vec2 uScreenSize;               // Screen size
uniform vec2 uPixelSize;                // Pixel size
uniform float uScale;                   // Global scale
uniform vec2 uScale2D;                  // 2D scale
uniform float uRotation;                // Global rotation
uniform vec3 uRotation3D;               // 3D rotation
uniform vec2 uTranslation;              // 2D translation
uniform vec3 uTranslation3D;            // 3D translation
uniform bool uIs3D;                     // 3D mode flag
uniform bool uInstanced;                // Instanced rendering flag

// Animation parameters
uniform float uTime;                    // Animation time
uniform bool uAnimated;                 // Enable animation
uniform float uAnimationSpeed;          // Animation speed
uniform float uWaveAmplitude;           // Wave animation amplitude
uniform float uWaveFrequency;           // Wave animation frequency
uniform float uWavePhase;               // Wave animation phase
uniform bool uWaveX;                    // Wave on X axis
uniform bool uWaveY;                    // Wave on Y axis
uniform bool uWaveZ;                    // Wave on Z axis
uniform bool uPulse;                    // Pulse animation
uniform float uPulseSpeed;              // Pulse speed
uniform float uPulseAmount;             // Pulse amount
uniform bool uShake;                    // Shake animation
uniform float uShakeAmount;             // Shake amount
uniform float uShakeSpeed;              // Shake speed

// Skeletal animation
uniform mat4 uBoneMatrices[100];        // Bone transformation matrices
uniform int uNumBones;                  // Number of bones

// Distortion parameters
uniform bool uDistortion;               // Enable distortion
uniform float uDistortionAmount;        // Distortion amount
uniform float uDistortionFrequency;      // Distortion frequency
uniform vec2 uDistortionDirection;      // Distortion direction

// UV manipulation
uniform bool uUVScroll;                 // Enable UV scrolling
uniform vec2 uUVScrollSpeed;            // UV scroll speed
uniform bool uUVRotate;                 // Enable UV rotation
uniform float uUVRotationSpeed;         // UV rotation speed
uniform bool uUVScale;                  // Enable UV scaling
uniform vec2 uUVScale;                  // UV scale
uniform bool uUVOffset;                 // Enable UV offset
uniform vec2 uUVOffset;                 // UV offset

// Output to fragment shader
out vec2 vTexCoord;                     // Texture coordinates
out vec4 vColor;                        // Vertex color
out float vAlpha;                       // Vertex alpha
out vec2 vScreenPos;                   // Screen position
out vec3 vWorldPos;                     // World position
out vec3 vNormal;                       // Normal
out vec3 vTangent;                      // Tangent
out vec3 vBitangent;                    // Bitangent
out float vDepth;                       // Depth
out vec2 vViewPos;                      // View position
out vec3 vViewDirection;                // View direction
out float vFogFactor;                   // Fog factor

// Include common functions
#include "common/common.glsl"

// Wave animation
vec3 applyWaveAnimation(vec3 position, float time)
{
    vec3 animatedPos = position;
    
    if (uWaveX)
    {
        animatedPos.x += sin(position.y * uWaveFrequency + time * uAnimationSpeed + uWavePhase) * uWaveAmplitude;
    }
    
    if (uWaveY)
    {
        animatedPos.y += sin(position.x * uWaveFrequency + time * uAnimationSpeed + uWavePhase) * uWaveAmplitude;
    }
    
    if (uWaveZ)
    {
        animatedPos.z += sin(length(position.xy) * uWaveFrequency + time * uAnimationSpeed + uWavePhase) * uWaveAmplitude;
    }
    
    return animatedPos;
}

// Pulse animation
vec3 applyPulseAnimation(vec3 position, float time)
{
    if (!uPulse) return position;
    
    float pulse = sin(time * uPulseSpeed) * uPulseAmount;
    return position * (1.0 + pulse);
}

// Shake animation
vec3 applyShakeAnimation(vec3 position, float time)
{
    if (!uShake) return position;
    
    vec3 shakeOffset;
    shakeOffset.x = sin(time * uShakeSpeed * 1.7) * uShakeAmount;
    shakeOffset.y = cos(time * uShakeSpeed * 2.3) * uShakeAmount;
    shakeOffset.z = sin(time * uShakeSpeed * 1.1) * uShakeAmount;
    
    return position + shakeOffset;
}

// Skeletal animation
vec3 applySkeletalAnimation(vec3 position)
{
    if (uNumBones == 0) return position;
    
    vec3 animatedPos = vec3(0.0);
    
    for (int i = 0; i < 4; i++)
    {
        if (i >= uNumBones) break;
        
        int boneIndex = aBoneIndices[i];
        float boneWeight = aBoneWeights[i];
        
        if (boneWeight > 0.0 && boneIndex < 100)
        {
            vec4 transformedPos = uBoneMatrices[boneIndex] * vec4(position, 1.0);
            animatedPos += transformedPos.xyz * boneWeight;
        }
    }
    
    return animatedPos;
}

// Distortion effect
vec3 applyDistortion(vec3 position, float time)
{
    if (!uDistortion) return position;
    
    vec2 distorted = position.xy;
    float distortion = sin(length(position.xy) * uDistortionFrequency + time) * uDistortionAmount;
    
    distorted += normalize(uDistortionDirection) * distortion;
    
    return vec3(distorted, position.z);
}

// UV manipulation
vec2 manipulateUV(vec2 uv, float time)
{
    vec2 manipulatedUV = uv;
    
    // UV scrolling
    if (uUVScroll)
    {
        manipulatedUV += uUVScrollSpeed * time;
    }
    
    // UV rotation
    if (uUVRotate)
    {
        float angle = time * uUVRotationSpeed;
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);
        
        vec2 centeredUV = manipulatedUV - 0.5;
        manipulatedUV = vec2(
            centeredUV.x * cosAngle - centeredUV.y * sinAngle,
            centeredUV.x * sinAngle + centeredUV.y * cosAngle
        ) + 0.5;
    }
    
    // UV scaling
    if (uUVScale)
    {
        manipulatedUV = (manipulatedUV - 0.5) * uUVScale + 0.5;
    }
    
    // UV offset
    if (uUVOffset)
    {
        manipulatedUV += uUVOffset;
    }
    
    return manipulatedUV;
}

// Calculate fog factor
float calculateFogFactor(vec3 worldPos, vec3 cameraPos)
{
    float distance = length(worldPos - cameraPos);
    return exp(-distance * 0.001); // Simple exponential fog
}

// Main vertex processing
void main()
{
    // Start with vertex position
    vec3 position = aPosition;
    
    // Apply vertex-level transformations
    position *= aSize.x; // Apply vertex size scaling
    
    // Apply skeletal animation
    position = applySkeletalAnimation(position);
    
    // Apply animations
    if (uAnimated)
    {
        position = applyWaveAnimation(position, uTime);
        position = applyPulseAnimation(position, uTime);
        position = applyShakeAnimation(position, uTime);
    }
    
    // Apply distortion
    position = applyDistortion(position, uTime);
    
    // Apply vertex rotation
    if (aRotation != 0.0)
    {
        float angle = aRotation;
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);
        
        vec2 rotated = vec2(
            position.x * cosAngle - position.y * sinAngle,
            position.x * sinAngle + position.y * cosAngle
        );
        position.xy = rotated;
    }
    
    // Apply global transformations
    position *= uScale;
    position.xy *= uScale2D;
    
    // Apply global rotation
    if (uRotation != 0.0)
    {
        float angle = uRotation;
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);
        
        vec2 rotated = vec2(
            position.x * cosAngle - position.y * sinAngle,
            position.x * sinAngle + position.y * cosAngle
        );
        position.xy = rotated;
    }
    
    // Apply 3D rotation
    if (length(uRotation3D) > 0.0)
    {
        // Create rotation quaternion from Euler angles
        vec3 euler = uRotation3D;
        quat rotation = quatFromEuler(euler.x, euler.y, euler.z);
        position = quatRotate(rotation, position);
    }
    
    // Apply translation
    position.xy += uTranslation;
    position += uTranslation3D;
    
    // Transform to world space
    vec4 worldPos4 = uModelMatrix * vec4(position, 1.0);
    vec3 worldPos = worldPos4.xyz;
    
    // Transform to view space
    vec4 viewPos4 = uViewMatrix * worldPos4;
    vec3 viewPos = viewPos4.xyz;
    
    // Transform to clip space
    vec4 clipPos = uProjectionMatrix * viewPos4;
    
    // Apply MVP matrix directly if available
    if (length(uMVPMatrix[0]) > 0.0)
    {
        clipPos = uMVPMatrix * vec4(position, 1.0);
    }
    
    // Final position
    gl_Position = clipPos;
    
    // Pass through texture coordinates with manipulation
    vTexCoord = manipulateUV(aTexCoord, uTime);
    
    // Pass through color with alpha
    vColor = aColor;
    vAlpha = aAlpha;
    
    // Calculate screen position
    vScreenPos = clipPos.xy / clipPos.w * 0.5 + 0.5;
    vScreenPos *= uScreenSize;
    
    // Pass world position
    vWorldPos = worldPos;
    
    // Transform normal
    vNormal = normalize(mat3(uNormalMatrix) * aNormal);
    
    // Transform tangent
    vTangent = normalize(mat3(uNormalMatrix) * aTangent.xyz);
    
    // Calculate bitangent
    vBitangent = cross(vNormal, vTangent) * aTangent.w;
    
    // Calculate depth
    vDepth = gl_Position.z / gl_Position.w;
    
    // Calculate view position
    vViewPos = viewPos.xy;
    
    // Calculate view direction
    vec3 cameraPos = (uInverseViewMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    vViewDirection = normalize(worldPos - cameraPos);
    
    // Calculate fog factor
    vFogFactor = calculateFogFactor(worldPos, cameraPos);
    
    // Handle instanced rendering
    if (uInstanced)
    {
        // Apply instance transformation
        vec2 instancePos = aInstanceData.xy;
        float instanceScale = aInstanceData.z;
        float instanceRotation = aInstanceData.w;
        
        // Apply instance transformation to position
        vec2 instanceTransformed = vec2(
            instancePos.x + cos(instanceRotation) * position.x * instanceScale - sin(instanceRotation) * position.y * instanceScale,
            instancePos.y + sin(instanceRotation) * position.x * instanceScale + cos(instanceRotation) * position.y * instanceScale
        );
        
        // Update position
        position.xy = instanceTransformed;
        
        // Recalculate final position
        worldPos4 = uModelMatrix * vec4(position, 1.0);
        viewPos4 = uViewMatrix * worldPos4;
        clipPos = uProjectionMatrix * viewPos4;
        gl_Position = clipPos;
        
        // Update derived values
        vScreenPos = clipPos.xy / clipPos.w * 0.5 + 0.5;
        vScreenPos *= uScreenSize;
        vWorldPos = worldPos4.xyz;
    }
    
    // Handle 2D vs 3D rendering
    if (!uIs3D)
    {
        // For 2D rendering, ensure Z is 0 and W is 1
        gl_Position.z = 0.0;
        gl_Position.w = 1.0;
    }
}