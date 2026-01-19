#version 330 core

// Blood Particle Vertex Shader for OpenGL
// Handles vertex transformation and setup for blood particle effects

// Uniforms
uniform mat4 uWorldMatrix;           // World transformation matrix
uniform mat4 uViewMatrix;            // View transformation matrix
uniform mat4 uProjectionMatrix;      // Projection transformation matrix
uniform mat4 uWorldViewProjection;   // Combined world-view-projection matrix
uniform mat4 uBoneTransforms[255];     // Bone transformation matrices for animation
uniform float uTime;                   // Animation time
uniform vec3 uWindDirection;         // Wind direction for particle movement
uniform float uWindStrength;           // Wind strength
uniform vec2 uUVScale;               // UV scaling factor
uniform vec2 uUVOffset;              // UV offset for animation
uniform vec4 uParticleParams;         // Particle parameters (count, size, spread, randomness)
uniform vec4 uPhysicsParams;          // Physics parameters (gravity, drag, bounce, turbulence)
uniform vec4 uDeformParams;          // Deformation parameters (amount, frequency, randomness, damping)
uniform vec4 uMaterialParams;         // Material parameters
uniform vec3 uEmitterPosition;         // Particle emitter position
uniform float uEmitterRadius;          // Emitter radius
uniform vec4 uEmitterParams;            // Emitter parameters (rate, spread, velocity, lifetime)

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
    vec2 ParticleUV;                    // Particle UV coordinates
    vec3 ViewDirection;                  // View direction
    vec4 ScreenPosition;                 // Screen space position
    float ParticleAge;                   // Particle age
    float ParticleLife;                  // Particle lifetime
    vec3 ParticleVelocity;              // Particle velocity
    vec4 InstanceData;                    // Instance data
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

vec3 CalculateParticleDeformation(vec3 position, float time, uint instanceID)
{
    // Calculate particle deformation for dynamic effects
    vec3 deformation = vec3(0.0, 0.0, 0.0);
    
    // Add wave-like deformation
    float waveFreq = uDeformParams.y;
    float waveAmp = uDeformParams.x;
    
    float wave1 = sin(position.x * waveFreq + time * 2.0 + instanceID * 0.1) * waveAmp;
    float wave2 = cos(position.y * waveFreq * 1.3 + time * 1.5 + instanceID * 0.15) * waveAmp * 0.7;
    float wave3 = sin((position.x + position.y) * waveFreq * 0.8 + time * 1.8 + instanceID * 0.2) * waveAmp * 0.5;
    
    deformation.x += wave1 + wave2 + wave3;
    deformation.y += wave1 * 0.5 + wave2 * 0.3 + wave3 * 0.2;
    deformation.z += wave1 * 0.3 + wave2 * 0.2 + wave3 * 0.1;
    
    // Add turbulence
    float turbulence = sin(position.x * 10.0 + time * 5.0 + instanceID) * cos(position.y * 8.0 + time * 3.0 + instanceID * 0.5);
    turbulence *= uDeformParams.z * 0.1;
    deformation += vec3(turbulence, turbulence * 0.7, turbulence * 0.3);
    
    // Add wind effect
    float windEffect = dot(uWindDirection, position) * uWindStrength * 0.1;
    deformation.x += sin(windEffect + time) * uDeformParams.x * 0.3;
    deformation.y += cos(windEffect + time * 0.7) * uDeformParams.x * 0.2;
    deformation.z += sin(windEffect + time * 1.3) * uDeformParams.x * 0.1;
    
    // Apply damping
    deformation *= uDeformParams.w;
    
    return deformation;
}

vec2 CalculateParticleUV(vec2 baseUV, vec3 worldPos, float time, uint instanceID)
{
    // Calculate animated UV coordinates for particle effects
    vec2 particleUV = baseUV;
    
    // Base particle animation
    particleUV += uUVOffset * time * 0.5;
    
    // Add turbulence to flow
    float turbulence = sin(worldPos.x * 2.0 + time * 3.0 + instanceID) * cos(worldPos.y * 1.5 + time * 2.0 + instanceID * 0.5);
    turbulence *= uParticleParams.z * 0.1;
    particleUV += vec2(turbulence, turbulence * 0.7);
    
    // Add particle-specific animation
    float particleAnimation = sin(time * 2.0 + instanceID * 0.5) * 0.5 + 0.5;
    particleUV += particleAnimation * 0.02;
    
    // Apply UV scaling
    particleUV = particleUV * uUVScale;
    
    return particleUV;
}

vec3 CalculateAnimatedNormal(vec3 normal, vec3 position, float time, uint instanceID)
{
    // Animate normal for dynamic lighting effects
    vec3 animatedNormal = normal;
    
    // Add subtle normal animation
    float normalFreq = 3.0;
    float normalAmp = 0.15;
    
    animatedNormal.x += sin(position.x * normalFreq + time * 2.5 + instanceID * 0.3) * normalAmp;
    animatedNormal.y += cos(position.y * normalFreq * 1.3 + time * 1.8 + instanceID * 0.2) * normalAmp;
    animatedNormal.z += sin((position.x + position.y) * normalFreq * 0.8 + time * 2.2 + instanceID * 0.4) * normalAmp * 0.5;
    
    return normalize(animatedNormal);
}

vec4 CalculateVertexColor(vec4 baseColor, vec3 position, float time, uint instanceID)
{
    // Calculate dynamic vertex color for particles
    vec4 vertexColor = baseColor;
    
    // Add pulsing effect
    float pulse = sin(time * 3.0 + instanceID * 0.1) * 0.5 + 0.5;
    vertexColor.rgb *= (1.0 + pulse * uParticleParams.w * 0.3);
    
    // Add position-based color variation
    float positionFactor = sin(position.x * 0.5 + instanceID * 0.2) * cos(position.y * 0.3 + instanceID * 0.3);
    vertexColor.rgb *= (1.0 + positionFactor * 0.15);
    
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

vec3 CalculateParticlePosition(vec3 basePosition, uint instanceID)
{
    // Calculate position for individual particles
    vec3 particlePosition = basePosition;
    
    // Add particle position based on instance ID
    float particleOffset = float(instanceID) * uParticleParams.z; // Particle spacing
    particlePosition.x += particleOffset;
    particlePosition.z += particleOffset * 0.5; // Spread in Z as well
    
    // Add random offset for natural variation
    float randomOffset = sin(instanceID * 123.456) * cos(instanceID * 789.012) * uParticleParams.w;
    particlePosition.x += randomOffset;
    particlePosition.y += cos(instanceID * 345.678) * randomOffset * 0.5;
    particlePosition.z += sin(instanceID * 567.890) * randomOffset * 0.3;
    
    return particlePosition;
}

vec3 CalculateParticleVelocity(vec3 position, float time, uint instanceID)
{
    // Calculate velocity for particle physics simulation
    vec3 velocity = vec3(0.0, 0.0, 0.0);
    
    // Emitter velocity
    vec3 emitterVelocity = uEmitterParams.yzw;
    velocity += emitterVelocity;
    
    // Gravity effect
    float gravity = uPhysicsParams.x;
    velocity.y -= gravity * (1.0 + instanceID * 0.1);
    
    // Add wind effect
    velocity.x += uWindDirection.x * uWindStrength * 0.2;
    velocity.z += uWindDirection.z * uWindStrength * 0.2;
    
    // Add turbulence
    float turbulence = uPhysicsParams.w;
    vec3 turbulenceVelocity = vec3(
        sin(position.x * 5.0 + time * 2.0 + instanceID) * cos(position.y * 3.0 + time * 1.5 + instanceID * 0.5),
        cos(position.x * 4.0 + time * 1.8 + instanceID * 0.3) * sin(position.z * 3.5 + time * 2.2 + instanceID * 0.7),
        sin(position.z * 6.0 + time * 2.5 + instanceID * 0.2) * cos(position.x * 4.5 + time * 1.2 + instanceID * 0.4)
    );
    velocity += turbulenceVelocity * turbulence * 0.3;
    
    return velocity;
}

float CalculateParticleAge(float time, uint instanceID)
{
    // Calculate particle age based on time and instance ID
    float particleAge = time;
    
    // Add instance-based age offset for staggered spawning
    float ageOffset = float(instanceID) * uEmitterParams.x * 0.1;
    particleAge += ageOffset;
    
    // Apply lifetime
    particleAge = mod(particleAge, uEmitterParams.w);
    
    return particleAge;
}

float CalculateParticleLife(uint instanceID)
{
    // Calculate particle lifetime
    float particleLife = uEmitterParams.w;
    
    // Add variation based on instance ID
    float variation = sin(instanceID * 0.5) * 0.2 + 0.8;
    particleLife *= variation;
    
    return particleLife;
}

// Main vertex shader
void main()
{
    VS_OUT output;
    
    // Start with base position
    vec3 position = aPosition;
    
    // Calculate particle position for this instance
    position = CalculateParticlePosition(position, gl_InstanceID);
    
    // Calculate particle age and lifetime
    output.ParticleAge = CalculateParticleAge(uTime, gl_InstanceID);
    output.ParticleLife = CalculateParticleLife(gl_InstanceID);
    
    // Calculate particle velocity
    output.ParticleVelocity = CalculateParticleVelocity(position, uTime, gl_InstanceID);
    
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
    
    // Apply particle deformation
    vec3 deformation = CalculateParticleDeformation(position, uTime, gl_InstanceID);
    position += deformation;
    
    // Add particle physics simulation
    position += output.ParticleVelocity * uTime * 0.1;
    
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
    
    // Animate normal for dynamic effects
    output.Normal = CalculateAnimatedNormal(output.Normal, worldPosition.xyz, uTime, gl_InstanceID);
    
    // Pass through texture coordinates with animation
    output.TexCoord = aTexCoord;
    output.ParticleUV = CalculateParticleUV(output.TexCoord, worldPosition.xyz, uTime, gl_InstanceID);
    
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