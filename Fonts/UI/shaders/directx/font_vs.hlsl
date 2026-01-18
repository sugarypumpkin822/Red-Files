// Font Rendering Vertex Shader for DirectX
// Handles vertex transformation and setup for high-quality text rendering

cbuffer TransformConstants : register(b0)
{
    matrix WorldMatrix;           // World transformation matrix
    matrix ViewMatrix;            // View transformation matrix
    matrix ProjectionMatrix;      // Projection transformation matrix
    matrix WorldViewProjection;   // Combined world-view-projection matrix
    float4x4 BoneTransforms[255]; // Bone transformation matrices for animation
    float Time;                   // Animation time
    float2 UVScale;               // UV scaling factor
    float2 UVOffset;              // UV offset for animation
    float4 AnimationParams;        // Animation parameters (speed, type, intensity, phase)
    float4 DeformParams;          // Deformation parameters (amount, frequency, randomness, damping)
    float4 TextParams;            // Text-specific parameters (size, spacing, alignment, kerning)
    float4 EffectParams;          // General effect parameters
};

cbuffer MaterialConstants : register(b1)
{
    float4 MaterialColor;         // Base material color
    float Metallic;               // Metallic factor
    float Roughness;             // Roughness factor
    float Emission;               // Emission intensity
    float2 TextureScale;          // Texture scaling
    float2 TextureOffset;         // Texture offset
    float4 VertexColor;           // Vertex color multiplier
    float3 AmbientColor;          // Ambient color for 3D text
    float AmbientIntensity;       // Ambient intensity
};

// Input vertex structure
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    uint4 BoneIndices : BONEINDICES;
    float4 BoneWeights : BONEWEIGHTS;
    float3 InstancePosition : INSTANCE_POSITION;
    float4 InstanceRotation : INSTANCE_ROTATION;
    float3 InstanceScale : INSTANCE_SCALE;
    uint InstanceID : SV_InstanceID;
    float4 CharacterData : CHARACTER_DATA;  // Character-specific data (char code, size, etc.)
};

// Output structure to pixel shader
struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float2 SDFCoord : TEXCOORD1;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float3 WorldPosition : WORLD_POSITION;
    float4 Color : COLOR;
    float3 ViewDirection : TEXCOORD2;
    float4 ScreenPosition : TEXCOORD3;
    float2 UVAnimation : TEXCOORD4;
    float3 WorldNormal : WORLD_NORMAL;
    float4 InstanceData : TEXCOORD5;
    float FogFactor : TEXCOORD6;
    float4 CharacterInfo : TEXCOORD7;  // Character information for pixel shader
    float3 TextPosition : TEXCOORD8;     // Text-relative position
};

// Utility functions
float3 RotateVector(float3 vector, float4 rotation)
{
    // Rotate vector using quaternion
    float3 uv = cross(rotation.xyz, vector);
    uv = uv + rotation.xyz * dot(rotation.xyz, uv);
    float3 rotated = vector + uv * 2.0 * rotation.w + cross(rotation.xyz, uv) * 2.0;
    return rotated;
}

float4x4 CreateRotationMatrix(float4 rotation)
{
    // Create rotation matrix from quaternion
    float x = rotation.x;
    float y = rotation.y;
    float z = rotation.z;
    float w = rotation.w;
    
    float4x4 matrix = float4x4(
        1.0 - 2.0 * (y * y + z * z), 2.0 * (x * y - z * w), 2.0 * (x * z + y * w), 0.0,
        2.0 * (x * y + z * w), 1.0 - 2.0 * (x * x + z * z), 2.0 * (y * z - x * w), 0.0,
        2.0 * (x * z - y * w), 2.0 * (y * z + x * w), 1.0 - 2.0 * (x * x + y * y), 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    
    return matrix;
}

float4x4 CreateScaleMatrix(float3 scale)
{
    // Create scale matrix
    return float4x4(
        scale.x, 0.0, 0.0, 0.0,
        0.0, scale.y, 0.0, 0.0,
        0.0, 0.0, scale.z, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

float4x4 CreateTranslationMatrix(float3 translation)
{
    // Create translation matrix
    return float4x4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        translation.x, translation.y, translation.z, 1.0
    );
}

float3 CalculateTextDeformation(float3 position, float time, uint instanceID)
{
    // Calculate text deformation for dynamic effects
    float3 deformation = float3(0.0, 0.0, 0.0);
    
    // Add wave-like deformation for wavy text
    float waveFreq = DeformParams.y;
    float waveAmp = DeformParams.x;
    
    float wave1 = sin(position.x * waveFreq + time * AnimationParams.x + instanceID * 0.1) * waveAmp;
    float wave2 = cos(position.y * waveFreq * 1.3 + time * AnimationParams.x * 0.7 + instanceID * 0.15) * waveAmp * 0.7;
    float wave3 = sin((position.x + position.y) * waveFreq * 0.8 + time * AnimationParams.x * 1.2 + instanceID * 0.2) * waveAmp * 0.5;
    
    deformation.y += wave1 + wave2 + wave3;
    
    // Add turbulence
    float turbulence = sin(position.x * 10.0 + time * 5.0 + instanceID) * cos(position.y * 8.0 + time * 3.0 + instanceID * 0.5);
    turbulence *= DeformParams.z * 0.05;
    deformation.y += turbulence;
    
    // Add random jitter
    float jitter = sin(time * 10.0 + instanceID * 123.456) * cos(time * 7.0 + instanceID * 789.012);
    deformation.x += jitter * DeformParams.z * 0.02;
    deformation.z += cos(time * 8.0 + instanceID * 345.678) * DeformParams.z * 0.02;
    
    // Apply damping
    deformation *= DeformParams.w;
    
    return deformation;
}

float2 CalculateTextUV(float2 baseUV, float3 worldPos, float time, uint instanceID)
{
    // Calculate animated UV coordinates for text effects
    float2 textUV = baseUV;
    
    // Add scrolling effect
    textUV += UVOffset * time * 0.1;
    
    // Add character-specific animation
    float charAnimation = sin(time * AnimationParams.x + instanceID * 0.5) * 0.5 + 0.5;
    textUV += charAnimation * 0.01;
    
    // Add turbulence to UV
    float turbulence = sin(worldPos.x * 2.0 + time * 3.0 + instanceID) * cos(worldPos.y * 1.5 + time * 2.0 + instanceID * 0.5);
    turbulence *= EffectParams.z * 0.05;
    textUV += float2(turbulence, turbulence * 0.7);
    
    // Apply UV scaling
    textUV = textUV * UVScale;
    
    return textUV;
}

float3 CalculateAnimatedNormal(float3 normal, float3 position, float time, uint instanceID)
{
    // Animate normal for dynamic lighting effects on text
    float3 animatedNormal = normal;
    
    // Add subtle normal animation
    float normalFreq = 2.0;
    float normalAmp = 0.1;
    
    animatedNormal.x += sin(position.x * normalFreq + time * 2.0 + instanceID * 0.3) * normalAmp;
    animatedNormal.y += cos(position.y * normalFreq * 1.3 + time * 1.5 + instanceID * 0.2) * normalAmp;
    animatedNormal.z += sin((position.x + position.y) * normalFreq * 0.8 + time * 1.8 + instanceID * 0.4) * normalAmp * 0.5;
    
    return normalize(animatedNormal);
}

float4 CalculateVertexColor(float4 baseColor, float3 position, float time, uint instanceID)
{
    // Calculate dynamic vertex color for text
    float4 vertexColor = baseColor;
    
    // Add pulsing effect
    float pulse = sin(time * AnimationParams.x + instanceID * 0.1) * 0.5 + 0.5;
    vertexColor.rgb *= (1.0 + pulse * AnimationParams.y * 0.2);
    
    // Add position-based color variation
    float positionFactor = sin(position.x * 0.5 + instanceID * 0.2) * cos(position.y * 0.3 + instanceID * 0.3);
    vertexColor.rgb *= (1.0 + positionFactor * 0.1);
    
    // Apply material color
    vertexColor *= MaterialColor;
    
    return vertexColor;
}

float CalculateFogFactor(float3 worldPos, float3 cameraPos, float fogStart, float fogEnd)
{
    // Calculate fog factor for depth attenuation
    float distance = length(worldPos - cameraPos);
    float fogFactor = saturate((distance - fogStart) / (fogEnd - fogStart));
    return fogFactor;
}

float3 CalculateTextNormal(float3 baseNormal, float3 position, float time)
{
    // Calculate text-specific normal for embossed/3D text effects
    float3 textNormal = baseNormal;
    
    // Add embossing effect
    float embossStrength = TextParams.w * 0.1;
    textNormal.x += sin(position.x * 10.0 + time * 2.0) * embossStrength;
    textNormal.y += cos(position.y * 10.0 + time * 1.5) * embossStrength;
    
    return normalize(textNormal);
}

float2 CalculateSDFUV(float2 baseUV, float3 worldPos, float time)
{
    // Calculate UV coordinates for SDF font rendering
    float2 sdfUV = baseUV;
    
    // Add sub-pixel precision
    sdfUV += float2(0.5 / 1024.0, 0.5 / 1024.0); // Assuming 1024x1024 SDF texture
    
    // Add animation for dynamic effects
    sdfUV += sin(time * 0.5 + worldPos.x * 0.1) * 0.01;
    sdfUV += cos(time * 0.3 + worldPos.y * 0.1) * 0.01;
    
    return sdfUV;
}

// Bone animation function
float4x4 GetSkinningTransform(uint4 boneIndices, float4 boneWeights)
{
    // Calculate skinning transformation from bone weights
    float4x4 skinTransform = float4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    for (int i = 0; i < 4; i++)
    {
        if (boneWeights[i] > 0.0)
        {
            uint boneIndex = boneIndices[i];
            skinTransform += BoneTransforms[boneIndex] * boneWeights[i];
        }
    }
    
    return skinTransform;
}

float3 CalculateTextPosition(float3 basePosition, uint instanceID)
{
    // Calculate text-relative position for character-specific effects
    float3 textPosition = basePosition;
    
    // Add character offset based on instance ID
    float charOffset = (float)instanceID * TextParams.y; // Character spacing
    textPosition.x += charOffset;
    
    // Add line breaks (simplified)
    float lineOffset = floor(instanceID * TextParams.z) * TextParams.w; // Line height
    textPosition.y -= lineOffset;
    
    return textPosition;
}

// Main vertex shader
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    // Start with base position
    float3 position = input.Position;
    
    // Calculate text-relative position
    float3 textPosition = CalculateTextPosition(position, input.InstanceID);
    output.TextPosition = textPosition;
    
    // Apply instance transformation
    float4x4 instanceMatrix = CreateTranslationMatrix(input.InstancePosition);
    float4x4 rotationMatrix = CreateRotationMatrix(input.InstanceRotation);
    float4x4 scaleMatrix = CreateScaleMatrix(input.InstanceScale);
    
    float4x4 instanceTransform = mul(scaleMatrix, mul(rotationMatrix, instanceMatrix));
    
    // Apply skinning if bones are used
    if (any(input.BoneWeights > 0.0))
    {
        float4x4 skinTransform = GetSkinningTransform(input.BoneIndices, input.BoneWeights);
        position = mul(float4(position, 1.0), skinTransform).xyz;
        input.Normal = normalize(mul(input.Normal, (float3x3)skinTransform));
        input.Tangent = normalize(mul(input.Tangent, (float3x3)skinTransform));
        input.Bitangent = normalize(mul(input.Bitangent, (float3x3)skinTransform));
    }
    
    // Apply instance transformation
    position = mul(float4(position, 1.0), instanceTransform).xyz;
    
    // Calculate world position
    float3 worldPos = mul(float4(position, 1.0), WorldMatrix).xyz;
    
    // Apply text deformation
    float3 deformation = CalculateTextDeformation(position, Time, input.InstanceID);
    position += deformation;
    
    // Transform to world space
    float4 worldPosition = mul(float4(position, 1.0), WorldMatrix);
    output.WorldPosition = worldPosition.xyz;
    
    // Transform to view space
    float4 viewPosition = mul(worldPosition, ViewMatrix);
    
    // Transform to projection space
    output.Position = mul(viewPosition, ProjectionMatrix);
    output.ScreenPosition = output.Position;
    
    // Transform normals to world space
    float3x3 worldMatrix3x3 = (float3x3)WorldMatrix;
    output.Normal = normalize(mul(input.Normal, worldMatrix3x3));
    output.WorldNormal = output.Normal;
    output.Tangent = normalize(mul(input.Tangent, worldMatrix3x3));
    output.Bitangent = normalize(mul(input.Bitangent, worldMatrix3x3));
    
    // Calculate text-specific normal
    output.WorldNormal = CalculateTextNormal(output.WorldNormal, worldPos, Time);
    
    // Animate normal for dynamic effects
    output.Normal = CalculateAnimatedNormal(output.Normal, worldPos, Time, input.InstanceID);
    
    // Pass through texture coordinates with animation
    output.TexCoord = input.TexCoord * TextureScale + TextureOffset;
    output.SDFCoord = CalculateSDFUV(output.TexCoord, worldPos, Time);
    output.UVAnimation = CalculateTextUV(output.TexCoord, worldPos, Time, input.InstanceID);
    
    // Calculate vertex color
    output.Color = CalculateVertexColor(input.Color * VertexColor, worldPos, Time, input.InstanceID);
    
    // Calculate view direction
    float3 cameraPos = mul(float4(0, 0, 0, 1), ViewMatrix).xyz; // Camera position in world space
    output.ViewDirection = normalize(cameraPos - worldPos);
    
    // Store instance data for pixel shader
    output.InstanceData = float4(input.InstancePosition, input.InstanceID);
    
    // Store character information
    output.CharacterInfo = input.CharacterData;
    
    // Calculate fog factor
    output.FogFactor = CalculateFogFactor(worldPos, cameraPos, 10.0, 100.0);
    
    return output;
}