#version 330 core

// Blood Pool Vertex Shader for OpenGL
// Handles vertex transformation and setup for blood pool effects

// Uniforms
uniform mat4 uWorldMatrix;           // World transformation matrix
uniform mat4 uViewMatrix;            // View transformation matrix
uniform mat4 uProjectionMatrix;      // Projection transformation matrix
uniform mat4 uWorldViewProjection;   // Combined world-view-projection matrix
uniform mat4 uBoneTransforms[255];     // Bone transformation matrices for animation
uniform float uTime;                   // Animation time
uniform vec3 uWindDirection;         // Wind direction for pool movement
uniform float uWindStrength;           // Wind strength
uniform vec2 uUVScale;               // UV scaling factor
uniform vec2 uUVOffset;              // UV offset for animation
uniform vec4 uPoolParams;            // Pool parameters (size, depth, viscosity, surface tension)
uniform vec4 uDeformParams;          // Deformation parameters (amount, frequency, randomness, damping)
uniform vec4 uMaterialParams;         // Material parameters
uniform vec3 uPoolCenter;            // Pool center position
uniform float uPoolRadius;           // Pool radius
uniform float uSpreadRate;           // Pool spreading rate
uniform float uCoagulationRate;      // Pool coagulation rate

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
    vec2 PoolUV;                        // Pool UV coordinates
    vec3 ViewDirection;                  // View direction
    vec4 ScreenPosition;                 // Screen space position
    float DistanceFromCenter;           // Distance from pool center
    float PoolAge;                      // Pool age
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

vec3 CalculatePoolDeformation(vec3 position, float time, uint instanceID)
{
    // Calculate blood pool deformation for liquid surface effects
    vec3 deformation = vec3(0.0, 0.0, 0.0);
    
    // Add wave-like deformation for liquid surface
    float waveFreq = uDeformParams.y;
    float waveAmp = uDeformParams.x * 0.1; // Subtle waves for liquid
    
    float wave1 = sin(position.x * waveFreq + time * 1.5 + instanceID * 0.1) * waveAmp;
    float wave2 = cos(position.y * waveFreq * 1.3 + time * 1.2 + instanceID * 0.15) * waveAmp * 0.7;
    float wave3 = sin((position.x + position.y) * waveFreq * 0.8 + time * 1.8 + instanceID * 0.2) * waveAmp * 0.5;
    
    deformation.y += wave1 + wave2 + wave3;
    
    // Add surface tension waves
    float tensionFreq = uDeformParams.y * 2.0;
    float tensionAmp = uDeformParams.x * 0.05;
    
    float tension1 = sin(position.x * tensionFreq + time * 3.0 + instanceID * 0.3) * tensionAmp;
    float tension2 = cos(position.y * tensionFreq * 1.5 + time * 2.5 + instanceID * 0.25) * tensionAmp * 0.8;
    
    deformation.y += tension1 + tension2;
    
    // Add turbulence
    float turbulence = sin(position.x * 8.0 + time * 4.0 + instanceID) * cos(position.y * 6.0 + time * 3.0 + instanceID * 0.5);
    turbulence *= uDeformParams.z * 0.05;
    deformation.y += turbulence;
    
    // Add wind effect on surface
    float windEffect = dot(uWindDirection, position) * uWindStrength * 0.05;
    deformation.x += sin(windEffect + time * 2.0) * uDeformParams.x * 0.2;
    deformation.z += cos(windEffect + time * 1.5) * uDeformParams.x * 0.15;
    
    // Apply damping
    deformation *= uDeformParams.w;
    
    return deformation;
}

vec2 CalculatePoolUV(vec2 baseUV, vec3 worldPos, float time, uint instanceID)
{
    // Calculate animated UV coordinates for blood pool effects
    vec2 poolUV = baseUV;
    
    // Base pool animation (slow movement)
    poolUV += uUVOffset * time * 0.1;
    
    // Add spreading animation to UV
    float spreadFactor = clamp(time * uSpreadRate * 0.05, 0.0, 1.0);
    poolUV = mix(poolUV, poolUV * (1.0 + spreadFactor * 0.2), spreadFactor);
    
    // Add turbulence to flow
    float turbulence = sin(worldPos.x * 1.5 + time * 2.0 + instanceID) * cos(worldPos.y * 1.0 + time * 1.5 + instanceID * 0.5);
    turbulence *= uPoolParams.z * 0.05; // Viscosity affects flow
    poolUV += vec2(turbulence, turbulence * 0.7);
    
    // Add coagulation-based UV changes
    float coagulationFactor = clamp(time * uCoagulationRate * 0.02, 0.0, 1.0);
    poolUV *= (1.0 - coagulationFactor * 0.1); // UV movement slows as it coagulates
    
    // Apply UV scaling
    poolUV = poolUV * uUVScale;
    
    return poolUV;
}

vec3 CalculateAnimatedNormal(vec3 normal, vec3 position, float time, uint instanceID)
{
    // Animate normal for dynamic liquid surface lighting effects
    vec3 animatedNormal = normal;
    
    // Add subtle normal animation for liquid surface
    float normalFreq = 1.5;
    float normalAmp = 0.08;
    
    animatedNormal.x += sin(position.x * normalFreq + time * 2.0 + instanceID * 0.3) * normalAmp;
    animatedNormal.y += cos(position.y * normalFreq * 1.3 + time * 1.5 + instanceID * 0.2) * normalAmp;
    animatedNormal.z += sin((position.x + position.y) * normalFreq * 0.8 + time * 1.8 + instanceID * 0.4) * normalAmp * 0.5;
    
    return normalize(animatedNormal);
}

vec4 CalculateVertexColor(vec4 baseColor, vec3 position, float time, uint instanceID)
{
    // Calculate dynamic vertex color for blood pool
    vec4 vertexColor = baseColor;
    
    // Add subtle pulsing effect
    float pulse = sin(time * 1.5 + instanceID * 0.1) * 0.5 + 0.5;
    vertexColor.rgb *= (1.0 + pulse * uPoolParams.w * 0.1); // Surface tension affects pulsing
    
    // Add position-based color variation
    float positionFactor = sin(position.x * 0.3 + instanceID * 0.2) * cos(position.y * 0.2 + instanceID * 0.3);
    vertexColor.rgb *= (1.0 + positionFactor * 0.05);
    
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

vec3 CalculatePoolPosition(vec3 basePosition, uint instanceID)
{
    // Calculate position for individual pool sections
    vec3 poolPosition = basePosition;
    
    // Add pool position based on instance ID for larger pools
    float poolOffset = float(instanceID) * uPoolParams.x * 0.5;
    poolPosition.x += poolOffset;
    poolPosition.z += poolOffset * 0.3; // Spread in Z as well
    
    // Add random offset for natural variation
    float randomOffset = sin(instanceID * 123.456) * cos(instanceID * 789.012) * uPoolParams.w * 0.1;
    poolPosition.x += randomOffset;
    poolPosition.y += cos(instanceID * 345.678) * randomOffset * 0.05;
    poolPosition.z += sin(instanceID * 567.890) * randomOffset * 0.08;
    
    return poolPosition;
}

float CalculateDistanceFromCenter(vec3 worldPos)
{
    // Calculate distance from pool center
    return length(worldPos - uPoolCenter);
}

float CalculatePoolAge(float time, uint instanceID)
{
    // Calculate pool age based on time and instance ID
    float poolAge = time;
    
    // Add instance-based age offset for staggered spreading
    float ageOffset = float(instanceID) * 0.1;
    poolAge += ageOffset;
    
    return poolAge;
}

vec3 CalculatePoolSurfaceNormal(vec3 position, float time, uint instanceID)
{
    // Calculate surface normal for liquid pool
    vec3 surfaceNormal = vec3(0.0, 1.0, 0.0); // Base upward normal
    
    // Add surface waves
    float waveFreq = 2.0;
    float waveAmp = 0.1;
    
    surfaceNormal.x += sin(position.x * waveFreq + time * 2.0 + instanceID * 0.2) * waveAmp;
    surfaceNormal.z += cos(position.z * waveFreq + time * 1.5 + instanceID * 0.3) * waveAmp;
    
    return normalize(surfaceNormal);
}

// Main vertex shader
void main()
{
    VS_OUT output;
    
    // Start with base position
    vec3 position = aPosition;
    
    // Calculate pool position for this instance
    position = CalculatePoolPosition(position, gl_InstanceID);
    
    // Calculate distance from pool center
    output.DistanceFromCenter = CalculateDistanceFromCenter(position);
    
    // Calculate pool age
    output.PoolAge = CalculatePoolAge(uTime, gl_InstanceID);
    
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
    
    // Apply pool deformation
    vec3 deformation = CalculatePoolDeformation(position, uTime, gl_InstanceID);
    position += deformation;
    
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
    
    // Calculate pool surface normal
    vec3 surfaceNormal = CalculatePoolSurfaceNormal(worldPosition.xyz, uTime, gl_InstanceID);
    output.Normal = mix(output.Normal, surfaceNormal, 0.5); // Blend with surface normal
    
    // Animate normal for dynamic effects
    output.Normal = CalculateAnimatedNormal(output.Normal, worldPosition.xyz, uTime, gl_InstanceID);
    
    // Pass through texture coordinates with animation
    output.TexCoord = aTexCoord;
    output.PoolUV = CalculatePoolUV(output.TexCoord, worldPosition.xyz, uTime, gl_InstanceID);
    
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