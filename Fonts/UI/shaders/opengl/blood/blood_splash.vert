#version 330 core

// Blood Splash Vertex Shader for OpenGL
// Handles vertex transformation and setup for blood splash effects

// Uniforms
uniform mat4 uWorldMatrix;           // World transformation matrix
uniform mat4 uViewMatrix;            // View transformation matrix
uniform mat4 uProjectionMatrix;      // Projection transformation matrix
uniform mat4 uWorldViewProjection;   // Combined world-view-projection matrix
uniform mat4 uBoneTransforms[255];     // Bone transformation matrices for animation
uniform float uTime;                   // Animation time
uniform vec3 uWindDirection;         // Wind direction for splash movement
uniform float uWindStrength;           // Wind strength
uniform vec2 uUVScale;               // UV scaling factor
uniform vec2 uUVOffset;              // UV offset for animation
uniform vec4 uSplashParams;          // Splash parameters (intensity, spread, direction, randomness)
uniform vec4 uPhysicsParams;          // Physics parameters (gravity, drag, bounce, turbulence)
uniform vec4 uDeformParams;          // Deformation parameters (amount, frequency, randomness, damping)
uniform vec4 uMaterialParams;         // Material parameters
uniform vec3 uSplashCenter;          // Splash center position
uniform float uSplashRadius;         // Splash radius
uniform float uSplashHeight;         // Splash height
uniform float uSplashDuration;       // Splash duration
uniform float uSplashAge;           // Splash age

// Input vertex structure
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aTexCoord;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in vec3 aBitangent;
layout(location = 6) in ivec4 aBoneIndices;
layout(location = 7) in vec4 aBoneWeights;
layout(location = 8) in vec3 aInstancePosition;
layout(location = 9) in vec4 aInstanceRotation;
layout(location = 10) in vec3 aInstanceScale;
layout(location = 11) in uint aInstanceID;
layout(location = 12) in vec4 aCharacterData;  // Character-specific data

// Output to fragment shader
out VS_OUT {
    vec4 Position;                     // Clip space position
    vec2 TexCoord;                      // Texture coordinates
    vec3 Normal;                        // Normal vector
    vec3 Tangent;                       // Tangent vector
    vec3 Bitangent;                     // Bitangent vector
    vec3 WorldPosition;                 // World space position
    vec4 Color;                         // Vertex color
    vec2 SplashUV;                      // Splash UV coordinates
    vec3 ViewDirection;                  // View direction
    vec4 ScreenPosition;                 // Screen space position
    float DistanceFromCenter;           // Distance from splash center
    float SplashAge;                    // Splash age
    vec3 SplashVelocity;                // Splash velocity
    vec4 InstanceData;                  // Instance data
    float FogFactor;                     // Fog factor
} vs_out;

// Utility functions
vec3 RotateVector(vec3 vector, vec4 rotation)
{
    // Rotate vector using quaternion
    vec3 uv = cross(rotation.xyz, vector);
    uv = uv + rotation.xyz * dot(rotation.xyz, uv);
    vec3 rotated = vector + uv * 2.0 * rotation.w + cross(rotation.xyz, uv) * 2.0;
    return rotated;
}

mat4 CreateRotationMatrix(vec4 rotation)
{
    // Create rotation matrix from quaternion
    float x = rotation.x;
    float y = rotation.y;
    float z = rotation.z;
    float w = rotation.w;
    
    mat4 matrix = mat4(
        1.0 - 2.0 * (y * y + z * z), 2.0 * (x * y - z * w), 2.0 * (x * z + y * w), 0.0,
        2.0 * (x * y + z * w), 1.0 - 2.0 * (x * x + z * z), 2.0 * (y * z - x * w), 0.0,
        2.0 * (x * z - y * w), 2.0 * (y * z + x * w), 1.0 - 2.0 * (x * x + y * y), 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    
    return matrix;
}

mat4 CreateScaleMatrix(vec3 scale)
{
    // Create scale matrix
    return mat4(
        scale.x, 0.0, 0.0, 0.0,
        0.0, scale.y, 0.0, 0.0,
        0.0, 0.0, scale.z, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 CreateTranslationMatrix(vec3 translation)
{
    // Create translation matrix
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        translation.x, translation.y, translation.z, 1.0
    );
}

vec3 CalculateSplashDeformation(vec3 position, float time, uint instanceID)
{
    // Calculate blood splash deformation for dynamic effects
    vec3 deformation = vec3(0.0, 0.0, 0.0);
    
    // Add wave-like deformation for splash surface
    float waveFreq = uDeformParams.y;
    float waveAmp = uDeformParams.x * 0.2; // More pronounced waves for splash
    
    float wave1 = sin(position.x * waveFreq + time * 3.0 + instanceID * 0.1) * waveAmp;
    float wave2 = cos(position.y * waveFreq * 1.3 + time * 2.5 + instanceID * 0.15) * waveAmp * 0.7;
    float wave3 = sin((position.x + position.y) * waveFreq * 0.8 + time * 4.0 + instanceID * 0.2) * waveAmp * 0.5;
    
    deformation.x += wave1 + wave2 + wave3;
    deformation.y += wave1 * 0.8 + wave2 * 0.5 + wave3 * 0.3; // More vertical movement
    deformation.z += wave1 * 0.4 + wave2 * 0.3 + wave3 * 0.2;
    
    // Add turbulence
    float turbulence = sin(position.x * 12.0 + time * 6.0 + instanceID) * cos(position.y * 10.0 + time * 4.0 + instanceID * 0.5);
    turbulence *= uDeformParams.z * 0.15;
    deformation += vec3(turbulence, turbulence * 0.8, turbulence * 0.6);
    
    // Add wind effect on splash
    float windEffect = dot(uWindDirection, position) * uWindStrength * 0.1;
    deformation.x += sin(windEffect + time * 3.0) * uDeformParams.x * 0.4;
    deformation.y += cos(windEffect + time * 2.0) * uDeformParams.x * 0.3;
    deformation.z += sin(windEffect + time * 2.5) * uDeformParams.x * 0.2;
    
    // Apply damping
    deformation *= uDeformParams.w;
    
    return deformation;
}

vec2 CalculateSplashUV(vec2 baseUV, vec3 worldPos, float time, uint instanceID)
{
    // Calculate animated UV coordinates for splash effects
    vec2 splashUV = baseUV;
    
    // Base splash animation
    splashUV += uUVOffset * time * 0.8;
    
    // Add spreading animation to UV
    float spreadFactor = clamp(time / uSplashDuration, 0.0, 1.0);
    splashUV = mix(splashUV, splashUV * (1.0 + spreadFactor * 0.5), spreadFactor);
    
    // Add turbulence to flow
    float turbulence = sin(worldPos.x * 2.5 + time * 4.0 + instanceID) * cos(worldPos.y * 2.0 + time * 3.0 + instanceID * 0.5);
    turbulence *= uSplashParams.w * 0.1;
    splashUV += vec2(turbulence, turbulence * 0.7);
    
    // Add splash direction bias
    vec2 splashDir = normalize(uSplashParams.xy);
    float dirBias = dot(baseUV - 0.5, splashDir);
    splashUV += splashDir * dirBias * 0.1 * spreadFactor;
    
    // Apply UV scaling
    splashUV = splashUV * uUVScale;
    
    return splashUV;
}

vec3 CalculateAnimatedNormal(vec3 normal, vec3 position, float time, uint instanceID)
{
    // Animate normal for dynamic splash surface lighting effects
    vec3 animatedNormal = normal;
    
    // Add normal animation for splash surface
    float normalFreq = 2.5;
    float normalAmp = 0.2;
    
    animatedNormal.x += sin(position.x * normalFreq + time * 3.0 + instanceID * 0.3) * normalAmp;
    animatedNormal.y += cos(position.y * normalFreq * 1.3 + time * 2.5 + instanceID * 0.2) * normalAmp;
    animatedNormal.z += sin((position.x + position.y) * normalFreq * 0.8 + time * 2.8 + instanceID * 0.4) * normalAmp * 0.5;
    
    return normalize(animatedNormal);
}

vec4 CalculateVertexColor(vec4 baseColor, vec3 position, float time, uint instanceID)
{
    // Calculate dynamic vertex color for blood splash
    vec4 vertexColor = baseColor;
    
    // Add pulsing effect
    float pulse = sin(time * 4.0 + instanceID * 0.1) * 0.5 + 0.5;
    vertexColor.rgb *= (1.0 + pulse * uSplashParams.x * 0.4);
    
    // Add position-based color variation
    float positionFactor = sin(position.x * 0.4 + instanceID * 0.2) * cos(position.y * 0.3 + instanceID * 0.3);
    vertexColor.rgb *= (1.0 + positionFactor * 0.1);
    
    // Apply material color
    vertexColor *= uMaterialParams;
    
    return vertexColor;
}

float CalculateFogFactor(vec3 worldPos, vec3 cameraPos, float fogStart, float fogEnd)
{
    // Calculate fog factor for depth attenuation
    float distance = length(worldPos - cameraPos);
    float fogFactor = (distance - fogStart) / (fogEnd - fogStart);
    return clamp(fogFactor, 0.0, 1.0);
}

mat4 GetSkinningTransform(ivec4 boneIndices, vec4 boneWeights)
{
    // Calculate skinning transformation from bone weights
    mat4 skinTransform = mat4(0.0);
    
    for (int i = 0; i < 4; i++)
    {
        if (boneWeights[i] > 0.0)
        {
            int boneIndex = boneIndices[i];
            skinTransform += uBoneTransforms[boneIndex] * boneWeights[i];
        }
    }
    
    return skinTransform;
}

vec3 CalculateSplashPosition(vec3 basePosition, uint instanceID)
{
    // Calculate position for individual splash sections
    vec3 splashPosition = basePosition;
    
    // Add splash position based on instance ID
    float splashOffset = float(instanceID) * uSplashRadius * 0.3;
    splashPosition.x += splashOffset;
    splashPosition.z += splashOffset * 0.5; // Spread in Z as well
    
    // Add random offset for natural variation
    float randomOffset = sin(instanceID * 123.456) * cos(instanceID * 789.012) * uSplashParams.w * 0.2;
    splashPosition.x += randomOffset;
    splashPosition.y += cos(instanceID * 345.678) * randomOffset * 0.1;
    splashPosition.z += sin(instanceID * 567.890) * randomOffset * 0.15;
    
    return splashPosition;
}

float CalculateDistanceFromCenter(vec3 worldPos)
{
    // Calculate distance from splash center
    return length(worldPos - uSplashCenter);
}

float CalculateSplashAge(float time, uint instanceID)
{
    // Calculate splash age based on time and instance ID
    float splashAge = time;
    
    // Add instance-based age offset for staggered splashing
    float ageOffset = float(instanceID) * uSplashDuration * 0.05;
    splashAge += ageOffset;
    
    return splashAge;
}

vec3 CalculateSplashVelocity(vec3 position, float time, uint instanceID)
{
    // Calculate velocity for splash physics simulation
    vec3 velocity = vec3(0.0, 0.0, 0.0);
    
    // Initial splash velocity (upward and outward)
    vec3 splashDirection = normalize(vec3(uSplashParams.xy, uSplashHeight));
    float splashIntensity = uSplashParams.x;
    velocity += splashDirection * splashIntensity * (1.0 + instanceID * 0.1);
    
    // Gravity effect
    float gravity = uPhysicsParams.x;
    velocity.y -= gravity * time * 0.5;
    
    // Add wind effect
    velocity.x += uWindDirection.x * uWindStrength * 0.3;
    velocity.z += uWindDirection.z * uWindStrength * 0.3;
    
    // Add turbulence
    float turbulence = uPhysicsParams.w;
    vec3 turbulenceVelocity = vec3(
        sin(position.x * 6.0 + time * 3.0 + instanceID) * cos(position.y * 4.0 + time * 2.0 + instanceID * 0.5),
        cos(position.x * 5.0 + time * 2.5 + instanceID * 0.3) * sin(position.z * 3.5 + time * 3.0 + instanceID * 0.7),
        sin(position.z * 7.0 + time * 3.5 + instanceID * 0.2) * cos(position.x * 4.0 + time * 2.0 + instanceID * 0.4)
    );
    velocity += turbulenceVelocity * turbulence * 0.4;
    
    return velocity;
}

vec3 CalculateSplashSurfaceNormal(vec3 position, float time, uint instanceID)
{
    // Calculate surface normal for splash
    vec3 surfaceNormal = vec3(0.0, 1.0, 0.0); // Base upward normal
    
    // Add splash waves
    float waveFreq = 3.0;
    float waveAmp = 0.15;
    
    surfaceNormal.x += sin(position.x * waveFreq + time * 3.0 + instanceID * 0.2) * waveAmp;
    surfaceNormal.z += cos(position.z * waveFreq + time * 2.0 + instanceID * 0.3) * waveAmp;
    
    return normalize(surfaceNormal);
}

// Main vertex shader
void main()
{
    VS_OUT output;
    
    // Start with base position
    vec3 position = aPosition;
    
    // Calculate splash position for this instance
    position = CalculateSplashPosition(position, gl_InstanceID);
    
    // Calculate distance from splash center
    output.DistanceFromCenter = CalculateDistanceFromCenter(position);
    
    // Calculate splash age
    output.SplashAge = CalculateSplashAge(uTime, gl_InstanceID);
    
    // Calculate splash velocity
    output.SplashVelocity = CalculateSplashVelocity(position, uTime, gl_InstanceID);
    
    // Apply instance transformation
    mat4 instanceMatrix = CreateTranslationMatrix(aInstancePosition);
    mat4 rotationMatrix = CreateRotationMatrix(aInstanceRotation);
    mat4 scaleMatrix = CreateScaleMatrix(aInstanceScale);
    
    mat4 instanceTransform = scaleMatrix * rotationMatrix * instanceMatrix;
    
    // Apply skinning if bones are used
    if (any(greaterThan(aBoneWeights, vec4(0.0)))
    {
        mat4 skinTransform = GetSkinningTransform(aBoneIndices, aBoneWeights);
        position = (skinTransform * vec4(position, 1.0)).xyz;
        aNormal = normalize((skinTransform * vec4(aNormal, 0.0)).xyz);
        aTangent = normalize((skinTransform * vec4(aTangent, 0.0)).xyz);
        aBitangent = normalize((skinTransform * vec4(aBitangent, 0.0)).xyz);
    }
    
    // Apply instance transformation
    position = (instanceTransform * vec4(position, 1.0)).xyz;
    
    // Calculate world position
    vec4 worldPosition = uWorldMatrix * vec4(position, 1.0);
    output.WorldPosition = worldPosition.xyz;
    
    // Apply splash deformation
    vec3 deformation = CalculateSplashDeformation(position, uTime, gl_InstanceID);
    position += deformation;
    
    // Add splash physics simulation
    position += output.SplashVelocity * uTime * 0.1;
    
    // Transform to world space
    worldPosition = uWorldMatrix * vec4(position, 1.0);
    output.WorldPosition = worldPosition.xyz;
    
    // Transform to view space
    vec4 viewPosition = uViewMatrix * worldPosition;
    
    // Transform to projection space
    output.Position = uProjectionMatrix * viewPosition;
    output.ScreenPosition = output.Position;
    
    // Transform normals to world space
    mat3 worldMatrix3x3 = mat3(uWorldMatrix);
    output.Normal = normalize(worldMatrix3x3 * aNormal);
    output.Tangent = normalize(worldMatrix3x3 * aTangent);
    output.Bitangent = normalize(worldMatrix3x3 * aBitangent);
    
    // Calculate splash surface normal
    vec3 surfaceNormal = CalculateSplashSurfaceNormal(worldPosition.xyz, uTime, gl_InstanceID);
    output.Normal = mix(output.Normal, surfaceNormal, 0.6); // Blend with surface normal
    
    // Animate normal for dynamic effects
    output.Normal = CalculateAnimatedNormal(output.Normal, worldPosition.xyz, uTime, gl_InstanceID);
    
    // Pass through texture coordinates with animation
    output.TexCoord = aTexCoord;
    output.SplashUV = CalculateSplashUV(output.TexCoord, worldPosition.xyz, uTime, gl_InstanceID);
    
    // Calculate vertex color
    output.Color = CalculateVertexColor(aColor, worldPosition.xyz, uTime, gl_InstanceID);
    
    // Calculate view direction
    vec3 cameraPos = (uViewMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz; // Camera position in world space
    output.ViewDirection = normalize(cameraPos - worldPosition.xyz);
    
    // Store instance data for fragment shader
    output.InstanceData = vec4(aInstancePosition, float(gl_InstanceID));
    
    // Calculate fog factor
    output.FogFactor = CalculateFogFactor(worldPosition.xyz, cameraPos, 10.0, 100.0);
    
    gl_Position = output.Position;
}