#version 330 core

// Signed Distance Field (SDF) Font Vertex Shader
// Enhanced vertex processing for SDF font rendering

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
layout(location = 11) in vec2 aUVOffset;     // UV offset for texture atlas
layout(location = 12) in vec2 aUVScale;      // UV scale for texture atlas
layout(location = 13) in float aDepth;       // Depth for layering
layout(location = 14) in vec3 aVelocity;     // Vertex velocity (for motion effects)
layout(location = 15) in vec4 aMorphWeights; // Morph target weights

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
uniform float uDepthBias;               // Depth bias for layering

// SDF specific parameters
uniform float uPixelRange;              // SDF pixel range
uniform vec2 uTextureSize;              // Texture size for SDF
uniform float uScreenScale;             // Screen scale for SDF
uniform bool uAutoPixelRange;           // Auto-calculate pixel range
uniform bool uHighQuality;              // High-quality rendering mode
uniform float uSharpness;               // Sharpness factor
uniform bool uAdaptiveRendering;        // Adaptive rendering based on distance

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
uniform bool uTypewriter;               // Typewriter effect
uniform float uTypewriterSpeed;          // Typewriter speed
uniform float uTypewriterProgress;       // Typewriter progress (0-1)
uniform bool uFadeIn;                   // Fade in effect
uniform float uFadeInSpeed;             // Fade in speed
uniform float uFadeInDelay;             // Fade in delay
uniform bool uSlideIn;                  // Slide in effect
uniform vec2 uSlideInDirection;          // Slide in direction
uniform float uSlideInDistance;         // Slide in distance
uniform bool uBounce;                   // Bounce animation
uniform float uBounceHeight;            // Bounce height
uniform float uBounceSpeed;             // Bounce speed
uniform bool uRotate3D;                 // 3D rotation animation
uniform vec3 uRotationAxis;             // Rotation axis
uniform float uRotationSpeed;           // Rotation speed

// Skeletal animation
uniform mat4 uBoneMatrices[100];        // Bone transformation matrices
uniform int uNumBones;                  // Number of bones

// Morph targets
uniform mat4 uMorphTargets[10];         // Morph target matrices
uniform int uNumMorphTargets;           // Number of morph targets

// Distortion parameters
uniform bool uDistortion;               // Enable distortion
uniform float uDistortionAmount;        // Distortion amount
uniform float uDistortionFrequency;      // Distortion frequency
uniform vec2 uDistortionDirection;      // Distortion direction
uniform bool uWaveDistortion;           // Wave distortion
uniform float uWaveDistortionAmplitude; // Wave distortion amplitude
uniform float uWaveDistortionFrequency; // Wave distortion frequency
uniform bool uTurbulence;               // Turbulence effect
uniform float uTurbulenceAmount;        // Turbulence amount
uniform float uTurbulenceFrequency;     // Turbulence frequency

// UV manipulation for SDF
uniform bool uUVScroll;                 // Enable UV scrolling
uniform vec2 uUVScrollSpeed;            // UV scroll speed
uniform bool uUVRotate;                 // Enable UV rotation
uniform float uUVRotationSpeed;         // UV rotation speed
uniform bool uUVScale;                  // Enable UV scaling
uniform vec2 uUVScale;                  // UV scale
uniform bool uUVOffset;                 // Enable UV offset
uniform vec2 uUVOffset;                 // UV offset
uniform bool uUVWarp;                   // Enable UV warping
uniform float uUVWarpAmount;            // UV warp amount
uniform float uUVWarpFrequency;         // UV warp frequency
uniform bool uUVNoise;                  // UV noise
uniform float uUVNoiseAmount;           // UV noise amount
uniform float uUVNoiseScale;            // UV noise scale

// Lighting parameters (for 3D effects)
uniform vec3 uLightDirection;           // Light direction
uniform vec3 uLightColor;               // Light color
uniform float uLightIntensity;          // Light intensity
uniform vec3 uAmbientColor;             // Ambient color
uniform float uAmbientIntensity;        // Ambient intensity
uniform bool uEnableLighting;           // Enable lighting
uniform bool uEnableShadows;            // Enable shadows
uniform vec3 uCameraPosition;           // Camera position for lighting

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
out float vPixelRange;                  // Calculated pixel range
out vec2 vTexelSize;                    // Texel size
out float vAnimationProgress;           // Animation progress
out vec3 vLighting;                     // Lighting contribution
out float vDistance;                    // Distance to camera
out vec3 vVelocity;                     // Vertex velocity
out float vSharpness;                   // Sharpness factor

// Include common functions
#include "common/common.glsl"
#include "common/noise.glsl"

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

// Typewriter effect
float calculateTypewriterProgress(vec2 position, float time)
{
    if (!uTypewriter) return 1.0;
    
    float progress = mod(time * uTypewriterSpeed, uTypewriterProgress + 0.001);
    
    // Calculate character position in text
    float charPosition = position.x; // Assuming X represents character position
    
    return smoothstep(charPosition - 0.1, charPosition + 0.1, progress);
}

// Fade in effect
float calculateFadeInAlpha(float time)
{
    if (!uFadeIn) return 1.0;
    
    float fadeTime = time - uFadeInDelay;
    if (fadeTime < 0.0) return 0.0;
    
    return smoothstep(0.0, uFadeInSpeed, fadeTime);
}

// Slide in effect
vec3 applySlideIn(vec3 position, float time)
{
    if (!uSlideIn) return position;
    
    float slideProgress = smoothstep(0.0, 1.0, time);
    vec3 slideOffset = vec3(uSlideInDirection * uSlideInDistance * (1.0 - slideProgress), 0.0);
    
    return position + slideOffset;
}

// Bounce animation
vec3 applyBounceAnimation(vec3 position, float time)
{
    if (!uBounce) return position;
    
    float bounceTime = time * uBounceSpeed;
    float bounce = abs(sin(bounceTime)) * uBounceHeight;
    
    return position + vec3(0.0, bounce, 0.0);
}

// 3D rotation animation
vec3 applyRotate3DAnimation(vec3 position, float time)
{
    if (!uRotate3D) return position;
    
    float angle = time * uRotationSpeed;
    vec3 axis = normalize(uRotationAxis);
    
    // Rodrigues' rotation formula
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);
    vec3 term1 = position * cosAngle;
    vec3 term2 = cross(axis, position) * sinAngle;
    vec3 term3 = axis * dot(axis, position) * (1.0 - cosAngle);
    
    return term1 + term2 + term3;
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

// Morph target animation
vec3 applyMorphAnimation(vec3 position)
{
    if (uNumMorphTargets == 0) return position;
    
    vec3 morphedPos = position;
    
    for (int i = 0; i < uNumMorphTargets; i++)
    {
        if (i >= 10) break;
        
        float morphWeight = aMorphWeights[i];
        if (morphWeight > 0.0)
        {
            vec4 morphed = uMorphTargets[i] * vec4(position, 1.0);
            morphedPos = mix(morphedPos, morphed.xyz, morphWeight);
        }
    }
    
    return morphedPos;
}

// Distortion effect
vec3 applyDistortion(vec3 position, float time)
{
    if (!uDistortion) return position;
    
    vec3 distortedPos = position;
    
    if (uWaveDistortion)
    {
        float distortion = sin(length(position.xy) * uWaveDistortionFrequency + time) * uWaveDistortionAmplitude;
        distortedPos.xy += normalize(uDistortionDirection) * distortion;
    }
    else if (uTurbulence)
    {
        vec2 turbulence = vec2(
            noise2D(position.xy * uTurbulenceFrequency + time),
            noise2D(position.xy * uTurbulenceFrequency * 1.3 + time * 1.1)
        ) * uTurbulenceAmount;
        distortedPos.xy += turbulence;
    }
    else
    {
        float distortion = sin(length(position.xy) * uDistortionFrequency + time) * uDistortionAmount;
        distortedPos.xy += normalize(uDistortionDirection) * distortion;
    }
    
    return distortedPos;
}

// UV manipulation for SDF
vec2 manipulateUV(vec2 uv, float time)
{
    vec2 manipulatedUV = uv;
    
    // Apply vertex-level UV transforms
    manipulatedUV *= aUVScale;
    manipulatedUV += aUVOffset;
    
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
    
    // UV warping
    if (uUVWarp)
    {
        vec2 warpOffset = vec2(
            sin(manipulatedUV.y * uUVWarpFrequency + time) * uUVWarpAmount,
            cos(manipulatedUV.x * uUVWarpFrequency + time) * uUVWarpAmount
        );
        manipulatedUV += warpOffset;
    }
    
    // UV noise
    if (uUVNoise)
    {
        vec2 noiseOffset = vec2(
            noise2D(manipulatedUV * uUVNoiseScale + time),
            noise2D(manipulatedUV * uUVNoiseScale * 1.7 + time * 1.3)
        ) * uUVNoiseAmount;
        manipulatedUV += noiseOffset;
    }
    
    return manipulatedUV;
}

// Calculate pixel range for SDF
float calculatePixelRange(vec2 screenSize, vec2 textureSize, float screenScale, float distance)
{
    if (uAutoPixelRange)
    {
        // Auto-calculate based on screen, texture size, and distance
        float baseRange = uPixelRange * length(screenSize / textureSize) * screenScale;
        
        // Adaptive rendering based on distance
        if (uAdaptiveRendering)
        {
            float distanceFactor = 1.0 + distance * 0.001; // Adjust based on distance
            return baseRange * distanceFactor;
        }
        
        return baseRange;
    }
    else
    {
        return uPixelRange;
    }
}

// Calculate lighting for 3D effects
vec3 calculateLighting(vec3 normal, vec3 worldPos, vec3 viewDir)
{
    if (!uEnableLighting) return vec3(1.0);
    
    vec3 lightDir = normalize(uLightDirection);
    vec3 normalDir = normalize(normal);
    
    // Diffuse lighting
    float diffuse = max(dot(normalDir, lightDir), 0.0);
    vec3 diffuseColor = uLightColor * diffuse * uLightIntensity;
    
    // Ambient lighting
    vec3 ambientColor = uAmbientColor * uAmbientIntensity;
    
    return ambientColor + diffuseColor;
}

// Calculate fog factor
float calculateFogFactor(vec3 worldPos, vec3 cameraPos)
{
    float distance = length(worldPos - cameraPos);
    return exp(-distance * 0.001); // Simple exponential fog
}

// Calculate sharpness based on distance
float calculateSharpness(float distance)
{
    if (!uAdaptiveRendering) return uSharpness;
    
    // Reduce sharpness for distant objects
    float distanceFactor = 1.0 - smoothstep(100.0, 1000.0, distance);
    return uSharpness * (0.5 + distanceFactor * 0.5);
}

// Main vertex processing
void main()
{
    // Start with vertex position
    vec3 position = aPosition;
    
    // Apply vertex-level transformations
    position *= aSize.x; // Apply vertex size scaling
    
    // Apply morph animation
    position = applyMorphAnimation(position);
    
    // Apply skeletal animation
    position = applySkeletalAnimation(position);
    
    // Apply animations
    if (uAnimated)
    {
        position = applyWaveAnimation(position, uTime);
        position = applyPulseAnimation(position, uTime);
        position = applyShakeAnimation(position, uTime);
        position = applySlideIn(position, uTime);
        position = applyBounceAnimation(position, uTime);
        position = applyRotate3DAnimation(position, uTime);
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
    
    // Apply depth bias
    clipPos.z += uDepthBias * aDepth;
    
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
    vec3 cameraPos = uCameraPosition;
    vViewDirection = normalize(worldPos - cameraPos);
    
    // Calculate distance to camera
    vDistance = length(worldPos - cameraPos);
    
    // Calculate fog factor
    vFogFactor = calculateFogFactor(worldPos, cameraPos);
    
    // Calculate SDF-specific values
    vTexelSize = 1.0 / uTextureSize;
    vPixelRange = calculatePixelRange(uScreenSize, uTextureSize, uScreenScale, vDistance);
    vSharpness = calculateSharpness(vDistance);
    
    // Calculate animation progress
    vAnimationProgress = calculateTypewriterProgress(position.xy, uTime);
    vAnimationProgress *= calculateFadeInAlpha(uTime);
    
    // Calculate lighting
    vLighting = calculateLighting(vNormal, vWorldPos, vViewDirection);
    
    // Pass velocity
    vVelocity = aVelocity;
    
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
        vDistance = length(vWorldPos - cameraPos);
    }
    
    // Handle 2D vs 3D rendering
    if (!uIs3D)
    {
        // For 2D rendering, ensure Z is 0 and W is 1
        gl_Position.z = 0.0;
        gl_Position.w = 1.0;
    }
    
    // Apply high-quality mode adjustments
    if (uHighQuality)
    {
        // Additional precision for high-quality rendering
        vPixelRange *= 1.5; // Increase pixel range for better quality
        vSharpness *= 1.2; // Increase sharpness
    }
}